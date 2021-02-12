# Third Day

## Timer 

Our target was it toggle a led each time a timer elapses. This timer should be set to elapse every second. Timers on our targeted microcontroller could either be used by polling or by using an interrupt.

### Polling

The first option to use polling is the naive way to do things. You have to initialize the timer and then use an endless loop to check if the timer has overflowed yet. If that's the case you can toggle the led and are done.
Allthough that sounds quite simple I had some problems regarding the configuration of the timer.

```c
T1CONbits.RD16    = 1;
T1CONbits.T1RUN   = 1;
T1CONbits.T1CKPS  = 0b00;
T1CONbits.T1OSCEN = 1;
T1CONbits.T1SYNC  = 1;
T1CONbits.TMR1CS  = 1;
T1CONbits.TMR1ON  = 1;
```

After consulting the datasheet i knew what all this configuration means and also why to set `TMR1CS` and `TMR1ON`. The rest of the program allthough is relatively straight forward.

```c
LEDMode = 0;
TMR1 = 0x8000;

while (1) {
    Nop();
    if (TimerOverflowed) {
        TimerOverflowed = 0;
        TMR1 = 0x8000;
        LED = !LED;
        Nop();
    }
}
```

Starting with the first line we define that we wan't to start with a led that is turned off.  
The second (and seventh respectively) need a bit more explanation. I think, that the timers of our microcontroler are counters rather than timers. The counter has a set amount of bits it can use. When an overflow occurs, the timer (or counter) is perceived to have triggered. And as the time the counter increases depends on the used crystal, that cannot be controlled via code, we have to set the value of `TMR1` so that the overflow occurs earlier.  
In the while loop we check for an overflow with the `TimerOverflowed` alias for the corresponding bit `PIR1bits.TMR1IF`. If thats the case we need reset both the overflow flag and the timers (counter) value. After that re can finally toggle the led.

### Interrupt

If we wanted to combine the ButtonReader from yesterday with the timer above (using seperate LEDs) it could happen that the performance (or better the responsiveness) of either project suffers. The best thing we could do is to use interrupts for both the button and the timer, but for the current project we will just use interrupts for the timer.  
The configuration we need for this is quite simmilar to the one we needed while using polling. In fact we configure the timer just the same way and need to add the following to enable the interrupt trigger.

```c
INTCONbits.GIE  = 1;
INTCONbits.PEIE = 1;
PIE1bits.TMR1IE = 1;
```

With these lines we enable the Timer1 Interrupt in three stages:

1. Enable interrupts at all
2. Enable that interrupts from peripheral devices are forwarded
3. Enable the interrupt source for timer 1

The next thing we have to do is defining a interrupt service routine, that will be called everytime the interrupt is fired. Inside that service routine we do pretty much the same as in the other project: we clear the overflow flag, reset the counter to the preloaded value and finally toggle the leds value.

```c
void __interrupt(high_priority) TMR1Int(void) {
    PIR1bits.TMR1IF = 0;
    TMR1 = 0x8000;
    TimerLed = !TimerLed;
    return;
}
```

In order to simulate the controller doing something else at the same time, I included the "Hello, World" project from the first day, which brings us to the following code.

```c
void __interrupt(high_priority) TMR1Int(void) {
    PIR1bits.TMR1IF = 0;
    TMR1 = 0x8000;
    TimerLed = !TimerLed;
    return;
}

void main(void) {
    T1CONbits.RD16 = 1; // Enables register read/write of TImer1 in one 16-bit operation
    T1CONbits.T1RUN = 1; // Device clock is derived from Timer1 oscillator
    T1CONbits.T1CKPS = 0b00; // 1:1 Prescaler 
    T1CONbits.T1OSCEN = 1; // Timer1 oscillator is enabled
    T1CONbits.T1SYNC = 1;
    T1CONbits.TMR1CS = 1; // External clock from pin RC0/T1OSO/T13CKI (on the rising edge)
    T1CONbits.TMR1ON = 1; // Start TMR1


    TimerLedMode = 0; // Make RB0 Output
    RandomLedMode = 0;
    TMR1 = 0x8000; // Initialize TMR1 for a 1s Tick

    PIE1bits.TMR1IE = 1; // enable TMR1 Interrupts (TMR1IE)
    INTCONbits.GIE = 1; // enable global interrupts (GIE)
    INTCONbits.PEIE = 1; // enable peripheral interrupts (PEIE)

    while (1) {
        RandomLed = !RandomLed;
        __delay_ms(500);
    }
    return;
}
```

Once I started the simulation, I immediately noticed that blinking using a timer works much more steadily. The `RandomLed` seems to blink in a more random frequency.

### Clock Simulator

The projects above require an external crystal clock. So if we want to simulate them, we need to also simulate the crystal. MPLAB X provides a tool called stimulus for that matter. By opening the `Clock Stimulus` panel, we can add a new crystal by specifying the pin the crystal is attached to alongside with some other parameters that define how the crystal operates.

![](clock-stimulus.png)