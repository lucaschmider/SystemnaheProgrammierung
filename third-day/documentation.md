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

In order to 
```c
### Clock Simulator