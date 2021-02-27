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

## Analog to digital converter

The goal of this project is it to read an analogue signal from one port of the microcontroller and control the duty cycle of another pins pwm output. Allthough that sounds quite useless as you could simply connect a potentiometer to the load (in this case a led), that would be very inefficient especcialy for higher loads.  
Our input pin will be `AN0`, which shares its lead with `RA0` and our output pin will be `CCP1` via the same lead as `RC2`. For that matter we begin by defining the pin modes as we always did at de beginning of the `main()` routine.

```c
TRISAbits.RA0 = 1;
TRISCbits.RC2 = 0;
```

The next step is to configure the analogue to digital converter module through the following lines.

```c
ADCON0bits.CHS = 0b0000;
ADCON0bits.GO  = 0;
ADCON0bits.ADON = 1;
ADCON1bits.VCFG = 0b00;
ADCON1bits.PCFG = 0b1110;
ADCON2bits.ADFM = 1;
ADCON2bits.ACQT = 0b010;
ADCON2bits.ADCS = 0b100;
```

Our microcontroller is able to handle an analogue input on 13 different channels (`AN0` to `AN12`) as the datasheet advises us, the nibble `0b0000` represents `AN0` and has to be written to the third to sixth bits of `ADCON0`. In the code above we use the reference `ADCON0bits.CHS` which addresses exactly those bits.  
In order to start a conversion, we need to set `GO` bit of `ADCON0` to `1`, but as we haven't configured the module completely, we set it to low, just in case.  
As the next step we configure the reference voltage to use for our conversion. We choose `Vss` (ground) over `AN2` for the negative reference and `Vcc` over `AN3` for the positive reference. According to the datasheet we have to set `ADCON1bits.VCFG` to `0b00` in order to achieve this. If we set one of those bits we would instruct the microcontroller to use the external reference voltage. The first bit in this literal controls the negative, the second the positive reference. We don't need external references as we don't need the very high accuracy we would gain.  
It is possible, that only certain pins on `PORTA` are used as analogue pins. To control this behaviour, the datasheet contains a table showing us that we need to set `ADCON1bits.PCFG` to `0b1110` if we wanted all pins except `AN0` to be digital.  
In the next step we need to solve a small problem: our microcontroller has a ten bit ADC build into it. One register is able to hold eight bit. To solve that, we use two registers. What we have now is six bits of free space and the choice between aligning the actually used bits to the left or to the right side. We do that as we set `ADCON2bits.ADFM` which results in an alignment called "right justified".  
Unfortunally, the conversion does not happen instantly. We have to wait a certain amount of time. And depending on the speed of our crystal, we have to set `ADCON2bits.ADCS` accordingly. In our case we go for four cycles. The same goes for the data acquisition.  
From now on we are (theoretically) able to read the analogue signal on `AN0`. In order to display the value, we need to configure `TIMER2` aswell.


```c
T2CONbits.T2CKPS  = 0b01;
T2CONbits.T2OUTPS = 0b0000;
T2CONbits.TMR2ON  = 1;
PR2 = 99;
```

We have the option to work with both pre- and postscalers to influence the timers speed. A postscaler is pretty much the same as a prescaler but is applied on the output signal instead of the input signal, both have the ability to devide an input frequency. As we use a 1:4 prescaler in this project, I will explain prescalers by the one we use. You can imagine one as a two bit counter with overflow detection. Each time the input signal occurs the counter is increased by one. In the moment the counter tries to count up to the four an overflow occurs which resets the timer and gives an output signal. So we have effectively divided our input frequency by four.  
To enable the prescaler we need to set `T2CONbits.T2CKPS` to one of the values mentioned in the datasheet - `0b01` enables the desired 1:4 prescaler. Due to the fact that we don't want a postscaler we set `T2CONbits.T2OUTPS` to `0b0000` which will use a 1:1 postscaler, effectively disabling it.  
Now that the timer is configured properly, we can activate it by setting `T2CONbits.TMR2ON`.  
As a last step we have to set `PR2` to 99. That is responsible for defining how long one pmw period is (that is the sum of the time the pin is on and the time the pin is off). The datasheet gives us the equation `PWM Period = (PR2 + 1) * 4 * Tosc * Presc2` with `Tosc` beeing the period of the used oscillator (in our case with a 16Mhz chrystal: 62.5ns) and `Presc2` beeing the value of our prescaler. With those values we can calculate the pmw period ourselves to `PWM Period = (99 + 1) * 4 * 62.5ns * 4 = 100us`.

Now that we have configured both the adc and timer 2, we can enable pwm mode for `CCP1` as a last step with just a single line of code.

```c
CCP1CONbits.CCP1M = 0b1100;
```

The remaining programm is quite simple and follows these steps in an endless loop.

1. Start the conversion
2. Wait until it is finished
3. Adjust the duty cycle

```c
while(1){    
    ADCON0bits.GO = 1;
    while(!ADCON0bits.GO);

    CCPR1L = ADRES / 16;
    CCP1CONbits.DC1B = 0b00;
}
```

The first line accomplishes the first step. By setting the `GO` bit, we instruct the adc to start converting.  
The adc clears the `GO` bit once its done, so we wait for this to happen in the second line.  
Now the result is written into `ADRES` and we assign it to the duty cycle control register `CCPR1L`. Here we have the same problem as earlier with the ten bit value we need to store using eight bit registers. We overcome that by dividing through sixteen and simply setting the last two bits (stored in `CCP1CONbits.DC1B`) to zero.

## Pulse width modulation

As the pwm project is pretty much the same as the previous project with the reading functionality cut out, I won't explain it in the same depth. The Code is the following:

```c
void main(void) {

    // Configuration as explained previously
    TRISCbits.RC2 = 0;
    T2CONbits.T2CKPS  = 0b01;
    T2CONbits.T2OUTPS = 0b0000;
    T2CONbits.TMR2ON  = 1;
    PR2 = 99;
    CCP1CONbits.CCP1M = 0b1100;

    // Relevant code
    CCPR1L = 0b00101000;        // Line A
    CCP1CONbits.DC1B = 0b00;    // Line B

    while(1);
    return;
}
```

I've marked the only two relevant lines (A & B). They are responsible for defining the desired duty cycle. The eight MSBs of the ten bit duty cycle value are stored in the register `CCPR1L` and the two remaining bits in `CCP1CONbits.DC1B`. So in other words, our programm sets the duty cycle of `CCP1` (=`RC2`) to `0b0010100000` (for further reference: **CCP**) and then goes into an endless loop to hold this duty cycle. If we convert **CCP** into its decimal representation, we get 160. The datasheet says us, that the duty cycle is calculated according to the following formula `DutyCycle = CCP * Tosc * Presc2` with `Tosc` beeing the oscillatior period of `62.5ns` and `Presc2` beeing the value of the prescaler from timer to (in our case 4). As a result we get `DutyCycle = 160 * 62.5ns * 4 = 40us`. As the period of one pwm cycle is `100us` our duty cycle equals 40%. 