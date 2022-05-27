
/* This is the minimal example of code to drive a Teensy 4.0
to run the TinyLev (Marzo 2017 https://doi.org/10.1063/1.4989995). 


@author: David Grier
@modified: Mia Morrell
Modified May 27, 2022


The TinyLev uses an L298N dual H-bridge driver to power the
upper and lower arrays of piezoelectric transducers.

The nominal resonant frequency of these transducers is 40 kHz.

Each H-bridge is controlled by two pins which should alternate 
between 10 and 01 at the driving frequency. The FlexPWM modules output signal
on 2 channels, channel A and channel B. Each of the FlexPWM modules in the Teensy 4.0 
can be programmed to provide this alternating pattern by 
setting the "invert" bit on channel B.

Suitable pairs of pins are:
   pins 2 and 3    FlexPWM4_2A and FlexPWM4_2B
   pins 6 and 9    FlexPWM2_2A and FlexPWM2_2B
   pins 7 and 8    FlexPWM1_3B and FlexPWM1_3A
   pins 22 and 23  FlexPWM1_3B and FlexPWM1_3A 
see https://forum.pjrc.com/threads/62880-Teensy-4-0-PWM-reversing-H-Bridge-driver 

For documentation on FlexPWM, see https://www.pjrc.com/teensy/IMXRT1060RM_rev3.pdf 
beginning pg 3091. */

 

#define FREQUENCY 40000. // define the transducer resonant frequency: 40 kHz

#define PWMRES 8        // PWM resolution 8 bits = 256 steps
#define PWMSTEPS 256    // to match PWMRES: there are 256 steps

#define INVERTB 0x0200; // bit pattern to invert channel B: make the pins alternate between 10 and 01 
                        // this inversion of signals is required to drive the H-bridge: https://en.wikipedia.org/wiki/H-bridge

// choose the pins that we will use to send the PWM signals
const int upperA = 2;   // FlexPWM4_2A
const int upperB = 3;   // FlexPWM4_2B
const int lowerA = 6;   // FlexPWM2_2A
const int lowerB = 9;   // FlexPWM2_2B

// initialize LED blinking to show that the driver is running:
const int ledPin = LED_BUILTIN;
const long blinkDuration = 500;
unsigned long lastBlink = 0;
int ledState = LOW;

// here, set the same amplitude for all the pins
void setAmp(uint16_t amp) {
  Serial.printf("\namplitude: %u ", amp);
  analogWrite(upperA, amp); 
  analogWrite(upperB, amp);
  analogWrite(lowerA, amp);
  analogWrite(lowerB, amp);
  
  FLEXPWM4_SM2OCTRL |= INVERTB;
  FLEXPWM2_SM2OCTRL |= INVERTB;
}

// set the pin frequencies: here frq is the driving frequency
// set the same frequency for all pins
void setFreq(float frq) {
  analogWriteFrequency(upperA, frq);
  analogWriteFrequency(upperB, frq);
  analogWriteFrequency(lowerA, frq);
  analogWriteFrequency(lowerB, frq);
}

// show the values of the output control register
void showFlexPWMInit(void) {
  Serial.println("FlexPWM2:" );
  Serial.printf("  SM0OCTL: %04X ", FLEXPWM2_SM0OCTRL); 
  Serial.printf("  SM1OCTL: %04X ", FLEXPWM2_SM1OCTRL);
  Serial.printf("  SM2OCTL: %04X ", FLEXPWM2_SM2OCTRL);
  Serial.printf("  SM3OCTL: %04X\n", FLEXPWM2_SM3OCTRL);
  
  Serial.println("FlexPWM4:" );
  Serial.printf("  SM0OCTL: %04X ", FLEXPWM4_SM0OCTRL);
  Serial.printf("  SM1OCTL: %04X ", FLEXPWM4_SM1OCTRL);
  Serial.printf("  SM2OCTL: %04X ", FLEXPWM4_SM2OCTRL);
  Serial.printf("  SM3OCTL: %04X\n", FLEXPWM4_SM3OCTRL); 
}

// fun blinking for testing
void blink(void) {
  unsigned long getTime = millis();

  if (getTime - lastBlink >= blinkDuration) { 
    lastBlink = getTime;
    ledState = !ledState; // turn on/off LED
    digitalWrite(ledPin, ledState);
  }
}

// set the frequency and amplitude 
void setup(void) {
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
  delay(100); // wait for PC to respond

  Serial.print("\nNYU TinyLev Driver");
  analogWriteRes(PWMRES); // write PWM resolution
  Serial.print("\nbefore initialization:");
  s     howFlexPWMInit();
  setFreq(FREQUENCY);      // set the signal frequency
  setAmp(PWMSTEPS/2 - 1); // set PWM amplitude to 50% duty cycle
  Serial.print("\nafter initialization:");
  showFlexPWMInit();
}

void loop() {
  blink(); // if the LED blinks, a square wave should be generated (see Supplementary Fig. 8.1, Marzo 2017 https://doi.org/10.1063/1.4989995)


}
