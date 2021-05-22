#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>

void save_memmap(EFI_HANDLE image_handle,
                 UINTN memory_map_size,
                 VOID* memory_map,
                 UINTN map_descriptor_size) {
    EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;

    gBS->OpenProtocol(image_handle,
                      &gEfiLoadedImageProtocolGuid,
                      (VOID**)&loaded_image,
                      image_handle,
                      NULL,
                      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    gBS->OpenProtocol(loaded_image->DeviceHandle,
                      &gEfiSimpleFileSystemProtocolGuid,
                      (VOID**)&fs,
                      image_handle,
                      NULL,
                      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    EFI_FILE_PROTOCOL* root_dir;
    fs->OpenVolume(fs, &root_dir);

    EFI_FILE_PROTOCOL* memmap_file;

    root_dir->Open(root_dir,
                   &memmap_file,
                   L"\\memmap",
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   0);

    {
        CHAR8* header = "Index, Type, PhysicalStart, NumberOfPages, Attribute\n";
        UINTN len = AsciiStrLen(header);
        memmap_file->Write(memmap_file, &len, header);

        char buf[256];

        EFI_PHYSICAL_ADDRESS iter;
        int i;
        for (iter = (EFI_PHYSICAL_ADDRESS)memory_map, i = 0;
             iter < (EFI_PHYSICAL_ADDRESS)memory_map + memory_map_size;
             iter += map_descriptor_size, i++) {
            EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)iter;
            UINTN len = AsciiSPrint(buf,
                                    sizeof(buf),
                                    "%u, %x, %08lx, %lx, %lx\n",
                                    i, desc->Type, desc->PhysicalStart, desc->NumberOfPages,
                                    desc->Attribute & 0xffffflu);
            memmap_file->Write(memmap_file, &len, buf);
        }
    }

    memmap_file->Close(memmap_file);
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE image_handle,
                           EFI_SYSTEM_TABLE *system_table) {
    CHAR8 memory_map_buffer[1024 * 16];

    UINTN memory_map_size = sizeof(memory_map_buffer);
    VOID* memory_map = memory_map_buffer;
    UINTN map_key;
    UINTN map_descriptor_size;
    UINT32 map_descriptor_version;

    gBS->GetMemoryMap(&memory_map_size,
                      (EFI_MEMORY_DESCRIPTOR*)memory_map,
                      &map_key,
                      &map_descriptor_size,
                      &map_descriptor_version);

    Print(L"memory_map_buffer = %08lx, memory_map_size = %08lx\n",
          memory_map_buffer, memory_map_size);

    save_memmap(image_handle,
                memory_map_size,
                memory_map,
                map_descriptor_size);

    Print(L"Hello World");
    while (1);
    return EFI_SUCCESS;
}
