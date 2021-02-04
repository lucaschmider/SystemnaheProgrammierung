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

### Clock Simulator