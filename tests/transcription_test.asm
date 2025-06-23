.686                   
.model flat, stdcall   
option casemap:none    
                       
program SEGMENT        
                       
                       

;---------- SEGMENT ----------
ORG 0BFE999h
__S0_TEST_KEYWORDS	PROC
__S0_TEST_KEYWORDS	ENDP

	;that was a test

__S0_TEST_KEYWORDS__END	PROC
__S0_TEST_KEYWORDS__END	ENDP

ORG 2000h
__V0_	WORD	 0x3322 

;---------- SEGMENT ----------
ORG 1000h
__S1_DATA	PROC
__S1_DATA	ENDP

	DWORD local_decimal_variable = 1500
	DWORD local_hexadecimal_variable = 0x10 ;it must be exchanged to 10h
	
	player_damage  DWORD  0

__S1_DATA__END	PROC
__S1_DATA__END	ENDP


;---------- SEGMENT ----------
ORG 400h
__S2_ANOTHER_SEGMENT	PROC
__S2_ANOTHER_SEGMENT	ENDP

	push eax
	pop eax
	ret

__S2_ANOTHER_SEGMENT__END	PROC
__S2_ANOTHER_SEGMENT__END	ENDP


;---------- SEGMENT ----------
ORG 400h
__S3_SEGMENT_WITH_NAME	PROC
__S3_SEGMENT_WITH_NAME	ENDP

	push ebp
	mov ebp, esp
	leave
	ret
	
__S3_SEGMENT_WITH_NAME__END	PROC
__S3_SEGMENT_WITH_NAME__END	ENDP

ORG 304h
__V1_real4_test	REAL4	 4.0 
ORG 304h
__V2_dword_test	DWORD	 3.0 

;---------- SEGMENT ----------
ORG 240h
__S4_CODE	PROC
__S4_CODE	ENDP

	xchg eax, ecx
	ret
}}
__S4_CODE__END	PROC
__S4_CODE__END	ENDP


;---------- SEGMENT ----------
ORG 0h
__S5_	PROC
__S5_	ENDP

	xor eax, eax
	inc eax
	}

__S5___END	PROC
__S5___END	ENDP

program ENDS           
                       