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
#include "../kernel/frame_buffer.h"
#include "../kernel/main.h"

const EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;

#define ASSERT(status) \
{ \
    EFI_STATUS status0 = status; \
    if (EFI_ERROR(status0)) { \
        Print(L"%d error: %r %d\n", __LINE__, status0, status0); \
        halt(); \
    } \
}

static EntryParams entry_params;

void halt()
{
    while (1)
        __asm__("hlt");
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
            UINT64 source = (UINT64) ehdr + phdr[i].p_offset;
            CopyMem((VOID *) phdr[i].p_vaddr, (VOID *) source,
                    phdr[i].p_filesz);

            UINT64 remain_bytes = phdr[i].p_memsz - phdr[i].p_filesz;
            SetMem((VOID *) (phdr[i].p_vaddr + phdr[i].p_filesz),
                   remain_bytes, 0);
        }
    }
}

void read_file(EFI_FILE_PROTOCOL * file, VOID ** buffer)
{
    UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 file_info_buffer[file_info_size];
    file->GetInfo(file,
                  &gEfiFileInfoGuid, &file_info_size, file_info_buffer);

    EFI_FILE_INFO *file_info = (EFI_FILE_INFO *) file_info_buffer;
    UINTN file_size = file_info->FileSize;

    ASSERT(gBS->AllocatePool(EfiLoaderData, file_size, buffer));
    ASSERT(file->Read(file, &file_size, *buffer));
}

void load_kernel(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL *root_dir)
{
    EFI_FILE_PROTOCOL *kernel_file;
    ASSERT(root_dir->Open(root_dir,
                          &kernel_file,
                          L"\\kernel.elf", EFI_FILE_MODE_READ, 0));

    VOID *kernel_buffer;
    read_file(kernel_file, &kernel_buffer);

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
        Print(L"kernel_first_addr != 0x100000: %p\n", kernel_first_addr);
        halt();
    }

    copy_load_segments(kernel_ehdr);

    ASSERT(gBS->FreePool(kernel_buffer));
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

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    open_gop(image_handle, &gop);

    EFI_FILE_PROTOCOL *root_dir;
    open_root_dir(image_handle, &root_dir);

    VOID *volume_image;

    {
        EFI_FILE_PROTOCOL *volume_file;
        ASSERT(root_dir->Open(root_dir, &volume_file, L"\\fat_disk",
                              EFI_FILE_MODE_READ, 0));
        read_file(volume_file, &volume_image);
    }

    load_kernel(image_handle, root_dir);

    {
        gBS->GetMemoryMap(&memory_map_size,
                          (EFI_MEMORY_DESCRIPTOR *) memory_map,
                          &map_key,
                          &map_descriptor_size, &map_descriptor_version);

        ASSERT(gBS->ExitBootServices(image_handle, map_key));
    }

    {
        UINT64 entry_addr = (UINT64)((Elf64_Ehdr*)kernel_base_addr)->e_entry;
        typedef void EntryPointType(EntryParams *);

        PixelFormat pixel_format;

        switch (gop->Mode->Info->PixelFormat) {
        default:
        case PixelRedGreenBlueReserved8BitPerColor:
            pixel_format = PIXEL_RED_GREEN_BLUE_RESERVED_8BIT_PER_COLOR;
            break;
        case PixelBlueGreenRedReserved8BitPerColor:
            pixel_format = PIXEL_BLUE_GREEN_RED_RESERVED_8BIT_PER_COLOR;
            break;
        }

        FrameBuffer frame_buffer = (FrameBuffer) {
            pixel_format,
            gop->Mode->Info->PixelsPerScanLine,
            gop->Mode->Info->HorizontalResolution,
            gop->Mode->Info->VerticalResolution,
            (UINT8 *) gop->Mode->FrameBufferBase,
            gop->Mode->FrameBufferSize
        };

        entry_params.frame_buffer = &frame_buffer;
        entry_params.memory_map.base = memory_map;
        entry_params.memory_map.size = memory_map_size;
        entry_params.memory_map.descriptor_size = map_descriptor_size;
        entry_params.volume_image = volume_image;

        EntryParams *arg = &entry_params;

        ((EntryPointType *) (entry_addr)) (arg);
    }

    return EFI_SUCCESS;
}
