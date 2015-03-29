// Feed Jpeg chunks
// Allocate memory for image
// Decode
EFI_STATUS JpegDecode (
	rcx,		// EFI_SYSTEM_TABLE * SystemTable
	rdx, 		// JpegData
	r8,			// JpegSize
	r9,  		// Allocated image buffer
	arg_20
	arg_28
	arg_30
	PULONG arg_38 		// Decoder reset result
)
{
	r13 = NULL;

	r12 = r9;

	rax = rdx;

	//
	// Check parameters.
	//

	if ( rdx == NULL ) return EFI_INVALID_PARAMETER;

	if ( r8 == NULL ) return EFI_INVALID_PARAMETER;

	rbx = arg_20;
	if ( arg_20 == NULL ) return EFI_INVALID_PARAMETER;

	rdi = arg_28;
	if ( arg_28 == NULL ) return EFI_INVALID_PARAMETER;

	rsi = arg_30;
	if ( arg_30 == NULL ) return EFI_INVALID_PARAMETER;

	rbp = arg_38;
	if ( arg_38 == NULL ) return EFI_INVALID_PARAMETER;

	//
	// Reset decoder ?
	//

	*arg_38 = Proc1 ( rdx, r8 );

	if ( *arg_38 != 0 ) return EFI_UNSUPPORTED;

	//
	// Allocate image buffer ?
	//

} 


.text:000000018000031C                 movzx   eax, cs:word_180001D42
.text:0000000180000323                 mov     [rsi], rax
.text:0000000180000326                 movzx   eax, cs:word_180001D44
.text:000000018000032D                 mov     [rdi], rax
.text:0000000180000330                 movzx   eax, cs:word_180001D42
.text:0000000180000337                 movzx   ecx, cs:word_180001D44
.text:000000018000033E                 imul    ecx, eax
.text:0000000180000341                 movsxd  rdx, ecx
.text:0000000180000344                 mov     rax, 100000000h
.text:000000018000034E                 shl     rdx, 2          ; Size
.text:0000000180000352                 cmp     rdx, rax
.text:0000000180000355                 jnb     short loc_18000030D 		// EFI_UNSUPPORTED
.text:0000000180000357                 mov     [rbx], rdx
.text:000000018000035A                 mov     rax, cs:gBootServices
.text:0000000180000361                 mov     r8, r12         ; Buffer
.text:0000000180000364                 mov     ecx, 4          ; PoolType
.text:0000000180000369                 call    [rax+EFI_BOOT_SERVICES.AllocatePool]
.text:000000018000036C                 cmp     rax, r13
.text:000000018000036F                 jge     short loc_180000380
.text:0000000180000371                 mov     rax, 8000000000000009h 	// EFI_OUT_OF_RESOURCES
.text:000000018000037B                 jmp     loc_18000044B
.text:0000000180000380 ; ---------------------------------------------------------------------------
.text:0000000180000380


.text:0000000180000380 loc_180000380:                          ; CODE XREF: JpegDecode+EFj
.text:0000000180000380                 mov     [rsp+0B48h+var_B28], r13w
.text:0000000180000386                 mov     [rsp+0B48h+var_B24], r13w
.text:000000018000038C                 mov     dil, r13b
.text:000000018000038F                 mov     [rsp+0B48h+arg_8], r13b
.text:0000000180000397                 movzx   ebx, r13w
.text:000000018000039B                 call    Proc2
.text:00000001800003A0                 jmp     loc_18000042F


.text:00000001800003A5 ; ---------------------------------------------------------------------------
.text:00000001800003A5
.text:00000001800003A5 loc_1800003A5:                          ; CODE XREF: JpegDecode+1B7j
.text:00000001800003A5                 mov     rax, cs:gBootServices
.text:00000001800003AC                 lea     rcx, [rsp+0B48h+Buffer] ; Buffer
.text:00000001800003B4                 xor     r8d, r8d        ; Value
.text:00000001800003B7                 mov     edx, 600h       ; Size
.text:00000001800003BC                 call    [rax+EFI_BOOT_SERVICES.SetMem]
.text:00000001800003C2                 lea     r8, [rsp+0B48h+arg_8]
.text:00000001800003CA                 lea     rcx, [rsp+0B48h+var_B18]
.text:00000001800003CF                 mov     r9, rbp
.text:00000001800003D2                 mov     dl, dil
.text:00000001800003D5                 call    Proc3
.text:00000001800003DA                 cmp     rax, r13
.text:00000001800003DD                 jnz     short loc_18000043D
.text:00000001800003DF                 movzx   eax, cs:word_180001D08
.text:00000001800003E6                 inc     bx
.text:00000001800003E9                 cmp     ax, r13w
.text:00000001800003ED                 jz      short loc_180000400
.text:00000001800003EF                 movzx   ecx, ax
.text:00000001800003F2                 movzx   eax, bx
.text:00000001800003F5                 mov     dil, 1
.text:00000001800003F8                 cdq
.text:00000001800003F9                 idiv    ecx
.text:00000001800003FB                 cmp     edx, r13d
.text:00000001800003FE                 jz      short loc_180000403
.text:0000000180000400
.text:0000000180000400 loc_180000400:                          ; CODE XREF: JpegDecode+16Dj
.text:0000000180000400                 mov     dil, r13b
.text:0000000180000403
.text:0000000180000403 loc_180000403:                          ; CODE XREF: JpegDecode+17Ej
.text:0000000180000403                 lea     rdx, [rsp+0B48h+Buffer]
.text:000000018000040B                 lea     rcx, [rsp+0B48h+var_B18]
.text:0000000180000410                 call    Proc4
.text:0000000180000415                 lea     r9, [rsp+0B48h+var_B24]
.text:000000018000041A                 lea     r8, [rsp+0B48h+var_B28]
.text:000000018000041F                 lea     rcx, [rsp+0B48h+Buffer]
.text:0000000180000427                 mov     rdx, r12
.text:000000018000042A                 call    Proc5
.text:000000018000042F
.text:000000018000042F loc_18000042F:                          ; CODE XREF: JpegDecode+120j
.text:000000018000042F                 cmp     [rsp+0B48h+arg_8], r13b
.text:0000000180000437                 jz      loc_1800003A5
.text:000000018000043D


.text:000000018000043D loc_18000043D:                          ; CODE XREF: JpegDecode+15Dj
.text:000000018000043D                 xor     eax, eax
.text:000000018000043F                 jmp     short loc_18000044B
.text:0000000180000441 ; ---------------------------------------------------------------------------
.text:0000000180000441
.text:0000000180000441 loc_180000441:                          ; CODE XREF: JpegDecode+27j
.text:0000000180000441                                         ; JpegDecode+30j ...
.text:0000000180000441                 mov     rax, 8000000000000002h
.text:000000018000044B
.text:000000018000044B loc_18000044B:                          ; CODE XREF: JpegDecode+97j
.text:000000018000044B                                         ; JpegDecode+FBj ...
.text:000000018000044B                 lea     r11, [rsp+0B48h+var_18]
.text:0000000180000453                 mov     rbx, [r11+20h]
.text:0000000180000457                 mov     rbp, [r11+30h]
.text:000000018000045B                 mov     rsi, [r11+38h]
.text:000000018000045F                 mov     rsp, r11
.text:0000000180000462                 pop     r13
.text:0000000180000464                 pop     r12
.text:0000000180000466                 pop     rdi
.text:0000000180000467                 retn
.text:0000000180000467 JpegDecode      endp
