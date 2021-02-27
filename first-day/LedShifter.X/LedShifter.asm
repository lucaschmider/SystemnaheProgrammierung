    list p=PIC18F4520
    #include <p18F4520.inc>
    CONFIG  PBADEN = OFF ; Zuerst Problem, weil manche ausgänge als analoge galten

Tmp	EQU	    0x20	    ; address of free ram register
Logic_Temp1  EQU	    0x21
Logic_Temp2  EQU	    0x22
ShiftLeft   EQU	    0x23

RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

MAIN_PROG CODE                      ; let linker place main program

START
    call    Initialize	    ;	Initialize the digital pins
    Loop
    btfss   ShiftLeft,0	    ; if ShiftLeft[0] == 1 {
    call    StepUp	    ; StepUp();
    
    btfsc   ShiftLeft,0	    ; } else {
    call    StepDown
    
    movlw   0b00000001
    btfsc   LATB,7	    ; W = LATB[7] == 1 ? 0b00000001 : 0b00000000
    movlw   0b00000000
    andwf   ShiftLeft,0	    ; W = (shiftLeft == 1 && LATB != 0b10000000)
    movwf   Logic_Temp1	    ; Logic_Temp1 = (shiftLeft == 1 && LATB != 0b10000000)
    
    movlw   0b00000001
    btfss   LATB,0	    ; W = LATB[0] == 1 ? 0b00000001 : 0b00000000
    movlw   0b00000000
    movwf   Logic_Temp2
    comf    ShiftLeft,0	    ; W = !ShiftLeft
    andwf   Logic_Temp2,1   ; Logic_Temp2 = (shiftLeft == 0 && LATB != 0b00000001)
    
    movf    Logic_Temp1
    iorwf   Logic_Temp2,0   ; W = (shiftLeft == 1 && LATB != 0b10000000) || (shiftLeft == 0 && LATB != 0b00000001)
    movwf   ShiftLeft

    
    
    GOTO    Loop	    ;	Go back to the beginning of Loop

StepUp ; Problem: Tipp war rlf (gibt es gar nicht), dann als ersatz rlcf (mit carry wird nicht wieder auf 0 gesetzt), dann rlncf
    rlncf    PORTB,1	    ;	Rotate all bits in the data register by one
			    ;	position
    call    Delay500
    return
    
StepDown
    rrncf    PORTB,1	    ;	Rotate all bits in the data register by one
			    ;	position
    call    Delay500
    return
    
Initialize
    
    movlw   0b00000000	    ;	We want all pins on the port to be an output,
			    ;	a zero means that the corresponding port is set 
			    ;	to output mode. Moving that into the accumulator
    movwf   TRISB	    ;	Write the mode configuration from the
			    ;	accumulator to the register responsible for 
			    ;	the port a
    movlw   0b00000001	    ;	We want all pins on the port to be an output,
			    ;	a zero means that the corresponding port is set 
			    ;	to output mode. Moving that into the accumulator
    movwf   PORTB	    ;	Write the mode configuration from the
			    ;	accumulator to the register responsible for 
			    ;	the port a
    movlw   0b00000001
    movwf   ShiftLeft
    clrf    Logic_Temp1
    clrf    Logic_Temp2
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
	    
Delay500
    movlw	    D'255'
    call	    DelayW
    movlw	    D'255'
    call	    DelayW
    return
    END