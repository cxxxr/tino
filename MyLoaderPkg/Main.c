#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>
#include  <Guid/FileInfo.h>

#include "../kernel/efi.h"

const EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;

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

const CHAR16 *efi_status_to_string(EFI_STATUS status)
{
    switch (status) {
    case EFI_SUCCESS:
        return L"EFI_SUCCESS";
    case EFI_LOAD_ERROR:
        return L"EFI_LOAD_ERROR";
    case EFI_INVALID_PARAMETER:
        return L"EFI_INVALID_PARAMETER";
    case EFI_UNSUPPORTED:
        return L"EFI_UNSUPPORTED";
    case EFI_BAD_BUFFER_SIZE:
        return L"EFI_BAD_BUFFER_SIZE";
    case EFI_BUFFER_TOO_SMALL:
        return L"EFI_BUFFER_TOO_SMALL";
    case EFI_NOT_READY:
        return L"EFI_NOT_READY";
    case EFI_DEVICE_ERROR:
        return L"EFI_DEVICE_ERROR";
    case EFI_WRITE_PROTECTED:
        return L"EFI_WRITE_PROTECTED";
    case EFI_OUT_OF_RESOURCES:
        return L"EFI_OUT_OF_RESOURCES";
    case EFI_VOLUME_CORRUPTED:
        return L"EFI_VOLUME_CORRUPTED";
    case EFI_VOLUME_FULL:
        return L"EFI_VOLUME_FULL";
    case EFI_NO_MEDIA:
        return L"EFI_NO_MEDIA";
    case EFI_MEDIA_CHANGED:
        return L"EFI_MEDIA_CHANGED";
    case EFI_NOT_FOUND:
        return L"EFI_NOT_FOUND";
    case EFI_ACCESS_DENIED:
        return L"EFI_ACCESS_DENIED";
    case EFI_NO_RESPONSE:
        return L"EFI_NO_RESPONSE";
    case EFI_NO_MAPPING:
        return L"EFI_NO_MAPPING";
    case EFI_TIMEOUT:
        return L"EFI_TIMEOUT";
    case EFI_NOT_STARTED:
        return L"EFI_NOT_STARTED";
    case EFI_ALREADY_STARTED:
        return L"EFI_ALREADY_STARTED";
    case EFI_ABORTED:
        return L"EFI_ABORTED";
    case EFI_ICMP_ERROR:
        return L"EFI_ICMP_ERROR";
    case EFI_TFTP_ERROR:
        return L"EFI_TFTP_ERROR";
    case EFI_PROTOCOL_ERROR:
        return L"EFI_PROTOCOL_ERROR";
    case EFI_INCOMPATIBLE_VERSION:
        return L"EFI_INCOMPATIBLE_VERSION";
    case EFI_SECURITY_VIOLATION:
        return L"EFI_SECURITY_VIOLATION";
    case EFI_CRC_ERROR:
        return L"EFI_CRC_ERROR";
    case EFI_END_OF_MEDIA:
        return L"EFI_END_OF_MEDIA";
    case EFI_END_OF_FILE:
        return L"EFI_END_OF_FILE";
    case EFI_INVALID_LANGUAGE:
        return L"EFI_INVALID_LANGUAGE";
    case EFI_COMPROMISED_DATA:
        return L"EFI_COMPROMISED_DATA";
    case EFI_HTTP_ERROR:
        return L"EFI_HTTP_ERROR";
    default:
        return L"???";
    }
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
        EFI_STATUS status = root_dir->Open(root_dir,
                                           &kernel_file,
                                           L"\\kernel.elf",
                                           EFI_FILE_MODE_READ, 0);
        if (EFI_ERROR(status)) {
            Print(L"could not open kernel.elf: %s\n",
                  efi_status_to_string(status));
            while (1);
        }

        UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
        UINT8 file_info_buffer[file_info_size];
        kernel_file->GetInfo(kernel_file,
                             &gEfiFileInfoGuid,
                             &file_info_size, file_info_buffer);

        EFI_FILE_INFO *file_info = (EFI_FILE_INFO *) file_info_buffer;
        UINTN kernel_file_size = file_info->FileSize;

        EFI_PHYSICAL_ADDRESS addr = kernel_base_addr;
        gBS->AllocatePages(AllocateAddress,
                           EfiLoaderData,
                           (kernel_file_size + 0xfff) / 0x1000, &addr);
        kernel_file->Read(kernel_file, &kernel_file_size, (VOID *) addr);
        Print(L"Kernel: 0x%0lx (%lu bytes)\n", addr, kernel_file_size);
    }

    {
        gBS->GetMemoryMap(&memory_map_size,
                          (EFI_MEMORY_DESCRIPTOR *) memory_map,
                          &map_key,
                          &map_descriptor_size, &map_descriptor_version);

        EFI_STATUS status = gBS->ExitBootServices(image_handle, map_key);
        if (EFI_ERROR(status)) {
            Print(L"ExitBootServices error: %s\n",
                  efi_status_to_string(status));
            while (1);
        }
    }

    {
        UINT64 entry_addr = *((UINT64 *) (kernel_base_addr + 24));
        typedef void EntryPointType(_EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE);

        _EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE mode = (_EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE){
            gop->Mode->MaxMode,
            gop->Mode->Mode,
            (_EFI_GRAPHICS_OUTPUT_MODE_INFORMATION*)gop->Mode->Info,
            gop->Mode->FrameBufferBase,
            gop->Mode->FrameBufferSize
        };
        ((EntryPointType *) (entry_addr)) (mode);
    }

    while (1);
    return EFI_SUCCESS;
}
