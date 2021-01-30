    list p=PIC18F4520
    #include <p18F4520.inc>
    CONFIG  PBADEN = OFF ; Zuerst Problem, weil manche ausgänge als analoge galten

LED_STATE   EQU	    0x20
Tmp	EQU	    0x21	    ; address of free ram register
RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

MAIN_PROG CODE                      ; let linker place main program

START
    call    Initialize	    ;	Initialize the digital pins
    Loop		    
    call    StepUp
    call    StepUp
    call    StepUp
    call    StepUp
    call    StepUp
    call    StepUp
    call    StepUp
    call    StepDown
    call    StepDown
    call    StepDown
    call    StepDown
    call    StepDown
    call    StepDown
    call    StepDown
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