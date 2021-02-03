# Second Day

## LED Shifter (C-Version)

It was our first task of the day to rewrite the LED shifter project in C. As I had the code working in assembler that was not a very hard task. After configuring the environment, I started with a main routine like so

```c
void main(void) {
    TRISB = 0b00000000;
    LATB = 0b0000001;
    shiftLeft = 1;
    while(1){
       for(int i = 0; i < 7; i++) {
           LATB = LATB << 1;
           delayHalfSecond();
       }
       for(int i = 0; i < 7; i++) {
           LATB = LATB >> 1;
           delayHalfSecond();
       }
    }
    return;
}

void delayHalfSecond(void) {
    __delay_ms(250);
    __delay_ms(250);
}
```

As you can see, in the first two lines of code, we start by initializing the pins. Again, `TRISB` is responsible for the direction of the pins where a `0` means output mode and `1` means input mode. As every bit stands for one pin of the port we net to set it to `0`, for clarity I always use the binary representation (`0b00000000`) ab it would be also possible to use decimal or other representations like `0x00`. On the other hand `LATB` is resposible for the value of the pins. That is where the bit literal representation really shines with `0b00000001` I can directly see that there is a HIGH at the first pin and a LOW on all others, thats not the case with decimal (`1`) or octal (`0x01`) representation. But I also have to say, that this type of literal is not standard C syntax, you have to have a compiler that knows how to interpret this syntax (which XC8 does).

### Improving the code

As you can see, I simply ported the code from assembler to C. But with C we also have access to much easier handling of logical expressions.

```c
void main(void) {
    TRISB = 0b00000000;
    LATB = 0b0000001;
    volatile char shiftLeft = 1;
    while(1){
        LATB = shiftLeft == 1 ? LATB << 1 : LATB >> 1;
        shiftLeft = (shiftLeft == 1 && LATB != 0b10000000) 
                || (shiftLeft == 0 && LATB == 0b00000001);
        
        delayHalfSecond();
    }
    return;
}
```

After initializing the chip in the same way as before, I declare a new variable.
```c
volatile char shiftLeft = 1;
```
Volatile is a keyword that I heard I should use to indicate a variable can change (allthough I don't belive that this makes a difference in that particular case). As there is no `boolean` type in C, I use a char because it's the numeric type with the smallest byte size. 

Starting in the loop, the first thing I do is to update the animation to the next frame by using the ternary operator `?` in combination with the bitwise shift operators `<<` and `>>`.
After updating a frame I need to decide in which direction I should go in the next execution of the loop. I found out, that there are two possible reasons why I should go left:

* I was already going left and the outer most led is not yet reached
* I was goning right and have reached the outer most led

So the translation into C like clauses looks as follows
```c
shiftLeft = (shiftLeft == 1 && LATB != 0b10000000) 
            || (shiftLeft == 0 && LATB == 0b00000001);
```
## Button-Reader (C-Version)

This time we should implement some code that acts just like some light switches we have at home. So every time the signal encounters a falling edge, the state of an led should toggle. 
I started by defining some aliases because we now have a lot of symbols which sound almost the same. We do that in the `Definitions.h` file where we also defined our settings and the clockspeed for our delay functions.

```c
#define Button PORTBbits.RB0
#define ButtonMode TRISBbits.RB0
#define LED LATBbits.LATB1
#define LEDMode TRISBbits.RB1
```

You can see, that we use different registers for the input `Button` (`PORTB` with its struct `PORTBbits` for easier access to just a single bit) and the output `LED` (`LATB` with `LATBbits` respectively).  
After doing that I thought, that it would be pretty straightforward to implement the desired behaviour.
```c
void main(void) {
    ButtonMode = 1;
    LEDMode = 0;
    LATB = 0;
    volatile char previousState = 0;
    while(1) {
        if (Button == 0 && previousState != Button) {
            LED = !LED;
            __delay_ms(20);
        }
        previousState = Button;
    }
    
    return;
}
```

I started as always by setting the modes of my pins and clearing the whole port alongside with defining the variable `previousState` to hold the state of `Button` in the iteration that was executed last. This variable is necessary as otherwise the led would toggle every iteration if the `Button` state is LOW.
In the Loop I simply check whether `Button` is currently LOW and also has changed since the last check has occured. If that has happened, I toggle the `LED` state and wait `20ms` in order to debounce the button a bit.  
That solution sometimes worked and sometimes not. I guess that is because the state of `Button` could change during the execution of one iteration.
I fixed the issue by introducing a local constant as follows.
```c
const char currentState = Button;
```
I then replaced all references to `Button` with ones to `currentState`.
## Debugging with Stimulus
