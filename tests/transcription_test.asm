.686p                  
.model flat, stdcall   
option casemap:none    
                       
program SEGMENT        
                       
                       
PUBLIC	script
PUBLIC	proc_with_unknown_content
PUBLIC	real4_test
PUBLIC	dword_test
PUBLIC	TEXT_EXAMPLE
PUBLIC	CHARS_EXAMPLE


;========== SEGMENT:   ==========
____S0	PROC
____S0	ENDP

	xor eax, eax
	inc eax
	}

____S0_END	PROC
____S0_END	ENDP



;---------- PROCEDURE:  script ----------
ORG 234h
script	PROC

	push eax
	push edx
	
	xor  eax, eax
	inc  eax, eax
	add  eax, ecx
	xchg edx, eax
	
	pop edx
	pop eax
	ret

script	ENDP


;---------- PROCEDURE:  proc_with_unknown_content ----------
ORG 235h
proc_with_unknown_content	PROC
proc_with_unknown_content	ENDP


;========== SEGMENT:  CODE ==========
ORG 240h
____S1	PROC
____S1	ENDP

	xchg eax, ecx
	ret
}}
____S1_END	PROC
____S1_END	ENDP

ORG 304h
real4_test	REAL4	 4.0 
ORG 304h
dword_test	DWORD	 3.0 


;========== SEGMENT:  ANOTHER_SEGMENT ==========
ORG 400h
____S2	PROC
____S2	ENDP

	push eax
	pop eax
	ret

____S2_END	PROC
____S2_END	ENDP



;========== SEGMENT:  SEGMENT_WITH_NAME ==========
ORG 400h
____S3	PROC
____S3	ENDP

	push ebp
	mov ebp, esp
	leave
	ret
	
____S3_END	PROC
____S3_END	ENDP

ORG 640h
TEXT_EXAMPLE	BYTE	"This is an example of a text\"a\"'",0


;========== SEGMENT:  DATA ==========
ORG 1000h
____S4	PROC
____S4	ENDP

	DWORD local_decimal_variable = 1500
	DWORD local_hexadecimal_variable = 10h ;it must be exchanged to 10h
	
	player_damage  DWORD  0

____S4_END	PROC
____S4_END	ENDP

ORG 2000h
	WORD	 3322h 
ORG 18CE0h
CHARS_EXAMPLE	BYTE	'This is an example of a text without \'null\' char"'


;========== SEGMENT:  TEST_KEYWORDS_AND_CONTENT_CONVERSION_TEST ==========
ORG 0BFE999h
____S5	PROC
____S5	ENDP

	;#;#that was a test
	
	0FFh
	100h
	0FFh
	100h
	11b
	
	mov eax, [100h+10b]

____S5_END	PROC
____S5_END	ENDP

program ENDS           
                       