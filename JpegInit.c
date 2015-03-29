// Reset Decoder, fetch all tables

typedef struct _JPEG_DECODER_STATE
{

} JPEG_DECODER_STATE, *PJPEG_DECODER_STATE;

JPEG_DECODER_STATE DecoderState;       // Jpeg decoder context

#define JPEG_SUCCESS        0
#define JPEG_WRONG_FORMAT   1
#define JPEG_BAD_QUANT_TAB  3
#define JPEG_HUFFMAN_ERR    4
#define JPEG_WRONG_SCAN     5
#define JPEG_BAD_FRAME      6

ULONG JpegInit ( PUCHAR JpegData, ULONG JpegSize )
{
    PUCHAR Data;
    PUCHAR DataEnd;
    ULONG Result;
    LONG Length;
    LONG Count;
    LONG NumComponents;
    PUCHAR Comptr;

    //
    // Reset jpeg decoder context
    //

    gBootServices->SetMem ( &DecoderState, sizeof(JPEG_DECODER_STATE), 0 );

    //
    // Check SOI
    //

    if ( rdi[0] != 0xFF || rdi[0] != 0xd8 ) return JPEG_WRONG_FORMAT;

    //
    // Setup scan loop.
    //

    Data = &JpegData[2];

    DecoderState.SOIFrame = JpegData;

    DataEnd = JpegData + JpegSize;

    if ( Data >= DataEnd) return JPEG_SUCCESS;

    //
    // Scan frames
    //

    for ( Data; Data < DataEnd; )
    {
        if ( Data[0] != 0xFF || Data[1] == 0 )      // Skip garbage and empty frames
        {
            Data++;
            continue;
        }

        switch ( Data[0] )
        {
            case 0xC0:      // Baseline DCT, non-differential, Huffman
                DecoderState.BaselineDCT = Data;
                Result = FetchBaselineDCT (Data);
                if ( Result != JPEG_SUCCESS ) return Result;
                break;

            case 0xC4:      // Define Huffman table(s)
                Result = FetchHuffman (Data);
                if ( Result != JPEG_SUCCESS) return Result;
                break;

            case 0xDA:      // Start of scan (SOS)
                DecoderState.StartOfScan = Data;

                Length = (Data[2] << 8) | Data[3];

                NumComponents = Data[4];

                if ( NumComponents != 3 || NumComponents != 1) return JPEG_WRONG_SCAN;

                if ( Data[Length - 1] != 0) return JPEG_WRONG_SCAN;       // Ss

                if ( Data[Length] != 0x3F ) return JPEG_WRONG_SCAN;       // Se

                if ( Data[Length + 1] != 0) return JPEG_WRONG_SCAN;       // Ah / Al

                //
                // Check components
                //

                Count  = 0;
                if ( NumComponents )
                {
                    Comptr = &Data[5];
                    do
                    {
                        if ( Comptr[0] > 3 ) return JPEG_WRONG_SCAN;        // Cs
                        if ( (Comptr[1] & 0xF0) > 0x10 ) return JPEG_WRONG_SCAN;    // Td
                        if ( (Comptr[1] & 0x0F) > 1 ) return JPEG_WRONG_SCAN;       // Ta
                        Count++;
                        Comptr += 2;
                    } while ( Count < NumComponents );
                }
                break;

            case 0xD8:      // Define quantization table(s)
                break;


loc_180001799:
.text:0000000180001799                 movzx   eax, byte ptr [rbx+3]
.text:000000018000179D                 movzx   ecx, byte ptr [rbx+2]
.text:00000001800017A1                 xor     r9d, r9d
.text:00000001800017A4                 shl     ecx, 8
.text:00000001800017A7                 lea     r8, [rbx+4]
.text:00000001800017AB                 lea     ecx, [rcx+rax-2]
.text:00000001800017AF                 mov     eax, 7E07E07Fh
.text:00000001800017B4                 imul    ecx
.text:00000001800017B6                 sar     edx, 5
.text:00000001800017B9                 mov     eax, edx
.text:00000001800017BB                 shr     eax, 1Fh
.text:00000001800017BE                 add     edx, eax
.text:00000001800017C0                 movzx   r10d, dl
.text:00000001800017C4                 test    dl, dl
.text:00000001800017C6                 jz      loc_1800018A1
.text:00000001800017CC
.text:00000001800017CC loc_1800017CC:                          ; CODE XREF: Proc1+162j
.text:00000001800017CC                 mov     al, [r8]
.text:00000001800017CF                 and     al, 0Fh
.text:00000001800017D1                 cmp     al, 3
.text:00000001800017D4                 ja      loc_1800018DB
.text:00000001800017DA                 test    byte ptr [r8], 0F0h
.text:00000001800017DE                 jnz     loc_1800018DB
.text:00000001800017E4                 movzx   eax, byte ptr [r8]
.text:00000001800017E8                 lea     rcx, [r8+1]
.text:00000001800017EC                 inc     r9d
.text:00000001800017EF                 and     eax, 0Fh
.text:00000001800017F2                 add     r8, 41h
.text:00000001800017F6                 mov     [r12+rax*8+30h], rcx
.text:00000001800017FB                 cmp     r9d, r10d
.text:00000001800017FE                 jb      short loc_1800017CC
.text:0000000180001800                 jmp     loc_1800018A1

            case 0xDD:      // Define restart interval
                DecoderState.RestartInterval = (Data[4] << 8) | Data[5];
                break;

            case 0xE0:      // Reserved for application
                DecoderState.CustomFrame = Data;
                break;

            //
            // Unsupported frames
            //

            case 0xC1:      // Extended sequential DCT
            case 0xC2:      // Progressive DCT
            case 0xC3:      // Lossless (sequential)
            case 0xC5:      // Differential sequential DCT
            case 0xC6:      // Differential progressive DCT
            case 0xC7:      // Differential lossless (sequential)
            case 0xC8:      // Reserved for JPEG extensions
            case 0xC9:      // Extended sequential DCT
            case 0xCA:      // Progressive DCT
            case 0xCB:      // Lossless (sequential)
            case 0xCC:      // Define arithmetic coding conditioning(s)
            case 0xCD:      // Differential sequential DCT
            case 0xCE:      // Differential progressive DCT
            case 0xCF:      // Differential lossless (sequential)
                return JPEG_BAD_FRAME;

            case 0xFE:      // Comment
                break;

            default:
                DecoderState.LastBogusFrame = Data;
                break;
        }

        //
        // Advance data pointer on next frame
        //

        if ( Data[2] == 0xFF ) Data += 2;       // SOI
        else Data += (Data[2] << 8) + Data[3] + 2;      // Other frames
    }
}



.text:00000001800014C4 FetchHuffman      proc near               ; CODE XREF: Proc1+1EBp
.text:00000001800014C4
.text:00000001800014C4 arg_0           = qword ptr  8
.text:00000001800014C4 arg_8           = qword ptr  10h
.text:00000001800014C4 arg_10          = qword ptr  18h
.text:00000001800014C4 arg_18          = qword ptr  20h
.text:00000001800014C4
.text:00000001800014C4                 mov     [rsp+arg_10], rbx
.text:00000001800014C9                 mov     [rsp+arg_18], rbp
.text:00000001800014CE                 mov     [rsp+arg_0], rcx
.text:00000001800014D3                 push    rsi
.text:00000001800014D4                 push    rdi
.text:00000001800014D5                 push    r12
.text:00000001800014D7                 push    r13
.text:00000001800014D9                 push    r14
.text:00000001800014DB                 push    r15
.text:00000001800014DD                 movzx   eax, byte ptr [rcx+2]
.text:00000001800014E1                 movzx   r8d, byte ptr [rcx+3]
.text:00000001800014E6                 lea     r9, [rcx+4]
.text:00000001800014EA                 shl     eax, 8
.text:00000001800014ED                 mov     r10, rcx
.text:00000001800014F0                 movsxd  rdx, eax
.text:00000001800014F3                 lea     rax, [rcx+rdx]
.text:00000001800014F7                 add     r8, rax
.text:00000001800014FA                 cmp     r9, r8
.text:00000001800014FD                 jnb     loc_18000167D
.text:0000000180001503                 lea     r11, qword_180001CE0
.text:000000018000150A                 mov     ebx, 1
.text:000000018000150F                 xor     edx, edx
.text:0000000180001511
.text:0000000180001511 loc_180001511:                          ; CODE XREF: Proc1_Sub1+1B3j
.text:0000000180001511                 mov     al, [r9]
.text:0000000180001514                 mov     cl, al
.text:0000000180001516                 shr     al, 3
.text:0000000180001519                 and     cl, 0Fh
.text:000000018000151C                 add     cl, al
.text:000000018000151E                 cmp     cl, 3
.text:0000000180001521                 ja      loc_180001694
.text:0000000180001527                 movzx   r15d, cl
.text:000000018000152B                 lea     r14, [r9+11h]
.text:000000018000152F                 mov     sil, dl
.text:0000000180001532                 mov     r13, r15
.text:0000000180001535                 movzx   ebp, dx
.text:0000000180001538                 mov     r12b, bl
.text:000000018000153B                 mov     [rsp+30h+arg_8], r15
.text:0000000180001540                 mov     r8d, 10h
.text:0000000180001546                 imul    r13, 170h
.text:000000018000154D                 mov     [r13+r11+78h], r10
.text:0000000180001552                 mov     al, [r9]
.text:0000000180001555                 lea     rdx, [r13+r11+0C7h]
.text:000000018000155D                 and     al, 0Fh
.text:000000018000155F                 mov     [r13+r11+80h], al
.text:0000000180001567                 mov     al, [r9]
.text:000000018000156A                 shr     al, 4
.text:000000018000156D                 inc     r9
.text:0000000180001570                 mov     rcx, r9
.text:0000000180001573                 mov     [r13+r11+81h], al
.text:000000018000157B
.text:000000018000157B loc_18000157B:                          ; CODE XREF: Proc1_Sub1+C4j
.text:000000018000157B                 mov     al, [rcx]
.text:000000018000157D                 add     rcx, rbx
.text:0000000180001580                 mov     [rdx], al
.text:0000000180001582                 add     rdx, rbx
.text:0000000180001585                 sub     r8, rbx
.text:0000000180001588                 jnz     short loc_18000157B
.text:000000018000158A                 mov     r8, rbx
.text:000000018000158D                 lea     r11, [r13+r11+0D8h]
.text:0000000180001595                 mov     edi, 10h
.text:000000018000159A                 xor     eax, eax
.text:000000018000159C
.text:000000018000159C loc_18000159C:                          ; CODE XREF: Proc1_Sub1+17Ej
.text:000000018000159C                 cmp     r12b, al
.text:000000018000159F                 jz      short loc_1800015AD
.text:00000001800015A1                 cmp     [r9], al
.text:00000001800015A4                 jz      loc_180001636
.text:00000001800015AA                 mov     r12b, al
.text:00000001800015AD
.text:00000001800015AD loc_1800015AD:                          ; CODE XREF: Proc1_Sub1+DBj
.text:00000001800015AD                 add     bp, bp
.text:00000001800015B0                 mov     rdx, r15
.text:00000001800015B3                 lea     rcx, qword_180001CE0
.text:00000001800015BA                 mov     bl, al
.text:00000001800015BC                 imul    rdx, 0B8h
.text:00000001800015C3                 add     rdx, r8
.text:00000001800015C6                 mov     [rcx+rdx*2+82h], bp
.text:00000001800015CE                 cmp     [r9], al
.text:00000001800015D1                 jbe     short loc_18000160F
.text:00000001800015D3                 mov     r10d, 1
.text:00000001800015D9                 lea     r15, qword_180001CE0
.text:00000001800015E0
.text:00000001800015E0 loc_1800015E0:                          ; CODE XREF: Proc1_Sub1+13Dj
.text:00000001800015E0                 movzx   eax, sil
.text:00000001800015E4                 add     bl, r10b
.text:00000001800015E7                 add     bp, r10w
.text:00000001800015EB                 lea     rcx, [rax+r13]
.text:00000001800015EF                 mov     al, [rax+r14]
.text:00000001800015F3                 add     sil, r10b
.text:00000001800015F6                 mov     [rcx+r15+0E8h], al
.text:00000001800015FE                 cmp     bl, [r9]
.text:0000000180001601                 jb      short loc_1800015E0
.text:0000000180001603                 mov     r15, [rsp+30h+arg_8]
.text:0000000180001608                 lea     rcx, qword_180001CE0
.text:000000018000160F
.text:000000018000160F loc_18000160F:                          ; CODE XREF: Proc1_Sub1+10Dj
.text:000000018000160F                 movzx   eax, byte ptr [r9]
.text:0000000180001613                 mov     ebx, 1
.text:0000000180001618                 add     ax, [rcx+rdx*2+82h]
.text:0000000180001620                 sub     ax, bx
.text:0000000180001623                 mov     [rcx+rdx*2+0A4h], ax
.text:000000018000162B                 mov     al, sil
.text:000000018000162E                 sub     al, [r9]
.text:0000000180001631                 mov     [r11], al
.text:0000000180001634                 xor     eax, eax
.text:0000000180001636
.text:0000000180001636 loc_180001636:                          ; CODE XREF: Proc1_Sub1+E0j
.text:0000000180001636                 add     r8, rbx
.text:0000000180001639                 add     r9, rbx
.text:000000018000163C                 add     r11, rbx
.text:000000018000163F                 sub     rdi, rbx
.text:0000000180001642                 jnz     loc_18000159C
.text:0000000180001648                 mov     r10, [rsp+30h+arg_0]
.text:000000018000164D                 movzx   r9d, sil
.text:0000000180001651                 lea     r11, qword_180001CE0
.text:0000000180001658                 movzx   eax, byte ptr [r10+2]
.text:000000018000165D                 movzx   edx, byte ptr [r10+3]
.text:0000000180001662                 add     r9, r14
.text:0000000180001665                 shl     eax, 8
.text:0000000180001668                 movsxd  rcx, eax
.text:000000018000166B                 lea     rax, [r10+rcx]
.text:000000018000166F                 add     rdx, rax
.text:0000000180001672                 cmp     r9, rdx
.text:0000000180001675                 mov     edx, edi
.text:0000000180001677                 jb      loc_180001511
.text:000000018000167D
.text:000000018000167D loc_18000167D:                          ; CODE XREF: Proc1_Sub1+39j
.text:000000018000167D                 xor     eax, eax
.text:000000018000167F
.text:000000018000167F loc_18000167F:                          ; CODE XREF: Proc1_Sub1+1D5j
.text:000000018000167F                 mov     rbx, [rsp+30h+arg_10]
.text:0000000180001684                 mov     rbp, [rsp+30h+arg_18]
.text:0000000180001689                 pop     r15
.text:000000018000168B                 pop     r14
.text:000000018000168D                 pop     r13
.text:000000018000168F                 pop     r12
.text:0000000180001691                 pop     rdi
.text:0000000180001692                 pop     rsi
.text:0000000180001693                 retn
.text:0000000180001694 ; ---------------------------------------------------------------------------
.text:0000000180001694
.text:0000000180001694 loc_180001694:                          ; CODE XREF: Proc1_Sub1+5Dj
.text:0000000180001694                 mov     eax, 4
.text:0000000180001699                 jmp     short loc_18000167F
.text:0000000180001699 Proc1_Sub1      endp







.text:00000001800013F4 FetchBaselineDCT      proc near               ; CODE XREF: Proc1+1FCp
.text:00000001800013F4                 cmp     byte ptr [rcx+4], 8
.text:00000001800013F8                 mov     r8, rcx
.text:00000001800013FB                 jnz     loc_1800014BC   ; EFI_NOT_READY
.text:0000000180001401                 xor     r9b, r9b
.text:0000000180001404                 cmp     [rcx+9], r9b
.text:0000000180001408                 jz      loc_1800014BC   ; EFI_NOT_READY
.text:000000018000140E                 cmp     byte ptr [rcx+9], 3
.text:0000000180001412                 ja      loc_1800014BC   ; EFI_NOT_READY
.text:0000000180001418                 movzx   eax, byte ptr [rcx+6]
.text:000000018000141C                 movzx   edx, byte ptr [rcx+5]
.text:0000000180001420                 mov     r10d, 100h
.text:0000000180001426                 imul    dx, r10w
.text:000000018000142B                 add     dx, ax
.text:000000018000142E                 mov     cs:word_180001D44, dx
.text:0000000180001435                 movzx   ecx, byte ptr [rcx+7]
.text:0000000180001439                 movzx   eax, byte ptr [r8+8]
.text:000000018000143E                 imul    cx, r10w
.text:0000000180001443                 add     cx, ax
.text:0000000180001446                 mov     r10b, r9b
.text:0000000180001449                 mov     cs:word_180001D42, cx
.text:0000000180001450                 mov     al, [r8+9]
.text:0000000180001454                 mov     cs:byte_180001D40, al
.text:000000018000145A                 cmp     [r8+9], r9b
.text:000000018000145E                 jbe     short loc_1800014B9
.text:0000000180001460                 lea     r11, qword_180001CE0
.text:0000000180001467
.text:0000000180001467 loc_180001467:                          ; CODE XREF: Proc1_Sub2+BDj
.text:0000000180001467                 movzx   edx, r10b
.text:000000018000146B                 lea     rax, [r8+rdx*2]
.text:000000018000146F                 mov     cl, [rdx+rax+0Bh]
.text:0000000180001473                 shr     cl, 4
.text:0000000180001476                 mov     [r11+rdx*4+66h], cl
.text:000000018000147B                 mov     al, [rdx+rax+0Bh]
.text:000000018000147F                 and     al, 0Fh
.text:0000000180001481                 mov     [r11+rdx*4+67h], al
.text:0000000180001486                 imul    cl
.text:0000000180001488                 lea     rcx, [r8+rdx*2+8]
.text:000000018000148D                 mov     [r11+rdx*4+68h], al
.text:0000000180001492                 mov     cl, [rdx+rcx+4]
.text:0000000180001496                 mov     [r11+rdx*4+69h], cl
.text:000000018000149B                 cmp     al, 1
.text:000000018000149D                 jz      short loc_1800014A7
.text:000000018000149F                 cmp     al, 2
.text:00000001800014A1                 jz      short loc_1800014A7
.text:00000001800014A3                 cmp     al, 4
.text:00000001800014A5                 jnz     short loc_1800014BC ; EFI_NOT_READY
.text:00000001800014A7
.text:00000001800014A7 loc_1800014A7:                          ; CODE XREF: Proc1_Sub2+A9j
.text:00000001800014A7                                         ; Proc1_Sub2+ADj
.text:00000001800014A7                 inc     r10b
.text:00000001800014AA                 add     r9b, al
.text:00000001800014AD                 cmp     r10b, [r8+9]
.text:00000001800014B1                 jb      short loc_180001467
.text:00000001800014B3                 cmp     r9b, 0Ah
.text:00000001800014B7                 ja      short loc_1800014BC ; EFI_NOT_READY
.text:00000001800014B9
.text:00000001800014B9 loc_1800014B9:                          ; CODE XREF: Proc1_Sub2+6Aj
.text:00000001800014B9                 xor     eax, eax
.text:00000001800014BB                 retn
.text:00000001800014BC ; ---------------------------------------------------------------------------
.text:00000001800014BC
.text:00000001800014BC loc_1800014BC:                          ; CODE XREF: Proc1_Sub2+7j
.text:00000001800014BC                                         ; Proc1_Sub2+14j ...
.text:00000001800014BC                 mov     eax, 6          ; EFI_NOT_READY
.text:00000001800014C1                 retn
.text:00000001800014C1 Proc1_Sub2      endp