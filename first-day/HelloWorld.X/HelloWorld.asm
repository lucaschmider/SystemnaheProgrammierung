    list p=PIC16F877A
    #include <p16F877A.inc>
    
    __CONFIG _CP_OFF & _WDT_OFF & _HS_OSC & _LVP_OFF & _BODEN_OFF
    
DelayTemp	EQU	    0x20	    ; address of free ram register
RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

MAIN_PROG CODE                      ; let linker place main program

START
    call    Initialize
    Loop    
    bsf		    PORTB,0
    movlw	    D'100'
    call	    DelayW
    bcf		    PORTB,0
    call	    Delay900
    GOTO Loop
	    

Initialize
    bsf	    STATUS,RP0
    movlw   B'11111110'
    movwf   TRISB
    bcf	    STATUS,RP0
    clrf    PORTB
    return
    
; ----- W ms Delay subroutine
DelayW						    ; 1000W x Tcy + 2 us (W >= 1)
    movwf	    Tmp			    ; [1us]
dlyw1	    movlw	    D'249'		    ; [1us]
dlyw0	    addlw	    H'FF'		    ; [1us]
    btfss	    STATUS,Z		    ; [1/2us] 2us la W = 1
    goto	    dlyw0		    ; [2us]
    nop					    ; [1us]
    decfsz	    Tmp,F		    ; [1/2us] 2 us la tmp = 1
    goto	    dlyw1		    ; [2us]
    return 
	    
Delay900
    movlw	    D'255'
    call	    DelayW
    movlw	    D'255'
    call	    DelayW
    movlw	    D'255'
    call	    DelayW
    movlw	    D'255'
    call	    DelayW
    return
END 