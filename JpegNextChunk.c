//
// Parse Jpeg chunks data.
//

EFI_STATUS JpegNextChunk (
    EFI_SYSTEM_TABLE * SystemTable, 
    PUCHAR JpegDataStart,        // JpegDataStart, starting search pointer
    PUCHAR JpegDataEnd,         // JpegDataEnd, end search pointer
    PULONG RequiredChunk,    // Specify required chunk to find, or 0 to find any next Jpeg chunk
                            // 0 value will be replaced by found chunk type.
    PUCHAR ChunkStart,       // Found chunk start pointer
    PULONG ChunkSize,     // Found Chunk size
    PUCHAR ChunkEnd,       // Found chunk end pointer (optional, can be NULL)
)
{
    //
    // Check parameters.
    //

    if (JpegDataStart == NULL) return EFI_INVALID_PARAMETER;
    if (JpegDataEnd == NULL) return EFI_INVALID_PARAMETER;
    if (JpegDataStart > JpegDataEnd) return EFI_INVALID_PARAMETER;

    if (RequiredChunk == NULL) return EFI_INVALID_PARAMETER;        // Missed in JpegDecoder.EFI

    if (ChunkStart == NULL) return EFI_INVALID_PARAMETER;       // Missed in JpegDecoder.EFI

    if (ChunkSize == NULL) return EFI_INVALID_PARAMETER;

    //
    // Try to find next Jpeg chunk.
    //

    for ( JpegDataStart ; JpegDataStart < JpegDataEnd ; JpegDataStart++ )
    {
        if ( JpegDataStart[0] != 0xff ) continue;       // First byte should be 0xFF

        if ( JpegDataStart[1] == 0 ) continue;          // Seconds byte shouldn't be zero (chunk type)

        if ( JpegDataStart[2] != 0xFF )         // Found not SOF chunk (this is actually bogus check, better test with 0xd8 )
        {
            *ChunkSize = (JpegDataStart[2] << 8) + JpegDataStart[3] + 2;    // Get chunk size + 2 bytes for marker
        }
        else *ChunkSize = 2;       // SOF chunk found (size = 2 bytes, SOF marker only)

        //
        // Found some chunk
        //

        if ( *RequiredChunk == 0 || *RequiredChunk == JpegDataStart[1])
        {
            if ( *RequiredChunk == 0 ) *RequiredChunk = JpegDataStart[1];   // Return found chunk type

            *ChunkStart = JpegDataStart;
            if ( ChunkEnd )
            {
                *ChunkEnd = JpegDataStart + *ChunkSize;
            }

            return EFI_SUCCESS;
        }

        JpegDataStart += *ChunkSize - 1;       // Require additional decrement before next iteration
    }

    //
    // No suitable Jpeg chunk found.
    //

    *ChunkStart = NULL;
    *ChunkSize = 0;
    if ( ChunkEnd ) *ChunkEnd = NULL;
    
    return EFI_NOT_FOUND;
}
