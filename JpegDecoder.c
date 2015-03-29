#include <uefi.h>

typedef struct _JPEG_INTERFACE
{
    EFI_STATUS    (*jpeg_sub1) (void);            // decoder?

    EFI_STATUS    (*NextChunk) (
        EFI_SYSTEM_TABLE * SystemTable, 
        PUCHAR JpegDataStart,        // JpegDataStart 
        PUCHAR JpegDataEnd,         // JpegDataEnd 
        PULONG RequiredChunk,    // Specify required chunk to find, or 0 to find any next Jpeg chunk
                                // 0 value will be replaced by found chunk type.
        PUCHAR ChunkStart,       // Found chunk start pointer
        PULONG ChunkSize,     // Found Chunk size
        PUCHAR ChunkEnd,       // Found chunk end pointer (optional, can be NULL)
    );

} JPEG_INTERFACE, *PJPEG_INTERFACE;

EFI_SYSTEM_TABLE *gSystemTable;
EFI_BOOT_SERVICES *gBootServices;
EFI_RUNTIME_SERVICE *gRuntimeServices;

JPEG_INTERFACE   JpegInterface;

HANDLE gInterfaceHandler;

ULONG JpegTag;

#define JPEG_DECODER_GUID \
  {0xa9396a81,0x6231,0x4dd7,0xbd,0x9b,0x2e,0x6b,0xf7,0xec,0x73,0xc2}

EFI_GUID JpegDecodeGuid = JPEG_DECODER_GUID;

EfiApplicationEntryPoint (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE * SystemTable )
{
    SaveParams ( ImageHandle, SystemTable );

    gInterfaceHandler = NULL;

    JpegInterface.jpeg_sub1 = JpegSub1;
    JpegInterface.NextChunk = JpegNextChunk;

    JpegTag = 'DgpJ';

    gBootServices->InstallProtocolInterface (
        &gInterfaceHandler,
        &JpegDecodeGuid,
        EFI_NATIVE_INTERFACE,
        &JpegInterface );
}

EFI_STATUS SaveParams (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE * SystemTable )
{
    gSystemTable = SystemTable;

    gBootServices = SystemTable->BootServices;
    gRuntimeServices = SystemTable->RuntimeServices;

    return EFI_SUCCESS;
}
