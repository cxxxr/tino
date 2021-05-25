#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>
#include  <Guid/FileInfo.h>

#include "../kernel/efi.h"
#include "../kernel/elf.h"

const EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;

#define ASSERT(status) \
{ \
    EFI_STATUS status0 = status; \
    if (EFI_ERROR(status0)) { \
        Print(L"%d error: %r %d\n", __LINE__, status0, status0); \
        halt(); \
    } \
}

void halt()
{
    while (1)
        __asm__("hlt");
}

void save_memmap(EFI_FILE_PROTOCOL * root_dir,
                 UINTN memory_map_size,
                 VOID * memory_map, UINTN map_descriptor_size)
{

    EFI_FILE_PROTOCOL *memmap_file;

    root_dir->Open(root_dir,
                   &memmap_file,
                   L"\\memmap",
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE |
                   EFI_FILE_MODE_CREATE, 0);

    {
        CHAR8 *header =
            "Index, Type, PhysicalStart, NumberOfPages, Attribute\n";
        UINTN len = AsciiStrLen(header);
        memmap_file->Write(memmap_file, &len, header);

        char buf[256];

        EFI_PHYSICAL_ADDRESS iter;
        int i;
        for (iter = (EFI_PHYSICAL_ADDRESS) memory_map, i = 0;
             iter < (EFI_PHYSICAL_ADDRESS) memory_map + memory_map_size;
             iter += map_descriptor_size, i++) {
            EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *) iter;
            UINTN len = AsciiSPrint(buf,
                                    sizeof(buf),
                                    "%u, %x, %08lx, %lx, %lx\n",
                                    i, desc->Type, desc->PhysicalStart,
                                    desc->NumberOfPages,
                                    desc->Attribute & 0xffffflu);
            memmap_file->Write(memmap_file, &len, buf);
        }
    }

    memmap_file->Close(memmap_file);
}

void open_root_dir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL ** root_dir)
{
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

    gBS->OpenProtocol(image_handle,
                      &gEfiLoadedImageProtocolGuid,
                      (VOID **) & loaded_image,
                      image_handle,
                      NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    gBS->OpenProtocol(loaded_image->DeviceHandle,
                      &gEfiSimpleFileSystemProtocolGuid,
                      (VOID **) & fs,
                      image_handle,
                      NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    fs->OpenVolume(fs, root_dir);
}

void open_gop(EFI_HANDLE image_handle, EFI_GRAPHICS_OUTPUT_PROTOCOL ** gop)
{
    UINTN num_gop_handles = 0;
    EFI_HANDLE *gop_handles = NULL;
    gBS->LocateHandleBuffer(ByProtocol,
                            &gEfiGraphicsOutputProtocolGuid,
                            NULL, &num_gop_handles, &gop_handles);
    gBS->OpenProtocol(gop_handles[0],
                      &gEfiGraphicsOutputProtocolGuid,
                      (VOID **) gop,
                      image_handle,
                      NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    FreePool(gop_handles);
}

void get_load_address_range(Elf64_Ehdr * ehdr, UINTN * first_addr,
                            UINTN * last_addr)
{
    Elf64_Phdr *phdr = (Elf64_Phdr *) ((UINT64) ehdr + ehdr->e_phoff);
    UINTN first = MAX_UINT64;
    UINTN last = 0;

    for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            first = MIN(first, phdr[i].p_vaddr);
            last = MAX(last, phdr[i].p_vaddr + phdr[i].p_memsz);
        }
    }

    *first_addr = first;
    *last_addr = last;
}

void copy_load_segments(Elf64_Ehdr * ehdr)
{
    Elf64_Phdr *phdr = (Elf64_Phdr *) ((UINT64) ehdr + ehdr->e_phoff);

    for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            UINT64 source = (UINT64)ehdr + phdr[i].p_offset;
            CopyMem((VOID *) phdr[i].p_vaddr, (VOID *) source,
                    phdr[i].p_filesz);

            UINT64 remain_bytes = phdr[i].p_memsz - phdr[i].p_filesz;
            SetMem((VOID *) (phdr[i].p_vaddr + phdr[i].p_filesz),
                   remain_bytes, 0);
        }
    }
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE image_handle,
                           EFI_SYSTEM_TABLE * system_table)
{
    CHAR8 memory_map_buffer[1024 * 16];

    UINTN memory_map_size = sizeof(memory_map_buffer);
    VOID *memory_map = memory_map_buffer;
    UINTN map_key;
    UINTN map_descriptor_size;
    UINT32 map_descriptor_version;

    gBS->GetMemoryMap(&memory_map_size,
                      (EFI_MEMORY_DESCRIPTOR *) memory_map,
                      &map_key,
                      &map_descriptor_size, &map_descriptor_version);

    Print(L"memory_map_buffer = %08lx, memory_map_size = %08lx\n",
          memory_map_buffer, memory_map_size);

    EFI_FILE_PROTOCOL *root_dir;

    open_root_dir(image_handle, &root_dir);

    save_memmap(root_dir,
                memory_map_size, memory_map, map_descriptor_size);

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    open_gop(image_handle, &gop);

    {
        EFI_FILE_PROTOCOL *kernel_file;
        ASSERT(root_dir->Open(root_dir,
                              &kernel_file,
                              L"\\kernel.elf", EFI_FILE_MODE_READ, 0));

        UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
        UINT8 file_info_buffer[file_info_size];
        kernel_file->GetInfo(kernel_file,
                             &gEfiFileInfoGuid,
                             &file_info_size, file_info_buffer);

        EFI_FILE_INFO *file_info = (EFI_FILE_INFO *) file_info_buffer;
        UINTN kernel_file_size = file_info->FileSize;

        VOID *kernel_buffer;

        ASSERT(gBS->AllocatePool(EfiLoaderData, kernel_file_size,
                                 &kernel_buffer));
        ASSERT(kernel_file->Read
               (kernel_file, &kernel_file_size, kernel_buffer));

        Elf64_Ehdr *kernel_ehdr = (Elf64_Ehdr *) kernel_buffer;

        UINTN kernel_first_addr, kernel_last_addr;
        get_load_address_range(kernel_ehdr, &kernel_first_addr,
                               &kernel_last_addr);
        Print(L"kernel_first_addr = %lx, kernel_last_addr = %lx\n",
              kernel_first_addr, kernel_last_addr);
        ASSERT(gBS->AllocatePages(AllocateAddress, EfiLoaderData,
                                  (kernel_last_addr - kernel_first_addr +
                                   0xfff) / 0x1000, &kernel_first_addr));

        if (kernel_first_addr != kernel_base_addr) {
            Print(L"kernel_first_addr != 0x100000: %p\n",
                  kernel_first_addr);
            halt();
        }

        copy_load_segments(kernel_ehdr);

        ASSERT(gBS->FreePool(kernel_buffer));
    }

    {
        gBS->GetMemoryMap(&memory_map_size,
                          (EFI_MEMORY_DESCRIPTOR *) memory_map,
                          &map_key,
                          &map_descriptor_size, &map_descriptor_version);

        ASSERT(gBS->ExitBootServices(image_handle, map_key));
    }

    {
        UINT64 entry_addr = *((UINT64 *) (kernel_base_addr + 24));
        typedef void EntryPointType(_EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE);

        _EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE mode =
            (_EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE) {
            gop->Mode->MaxMode,
            gop->Mode->Mode,
            (_EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *) gop->Mode->Info,
            gop->Mode->FrameBufferBase,
            gop->Mode->FrameBufferSize
        };
        ((EntryPointType *) (entry_addr)) (mode);
    }

    while (1);
    return EFI_SUCCESS;
}
