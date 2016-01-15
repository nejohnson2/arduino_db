# Reading Decibel Levels with Arduino

This sketch captures an analog audio signal using [Sparkfun's INMP401 MEMS microphone](https://www.sparkfun.com/products/9868) and converts the analog reading into decibels.  Unfortunately there is not much information about doing this correctly so this repo is aimed to be a collection of information to work with audio and arduino.  

## Audio Experiemnt

### Hardware Details
The INMP401 operates on 3.3 volts and has an analog output.  This makes the setup easy - Arduino 3.3 to Vcc, Arduino Gnd to GnD and the Microphones AUD pin to the Arduino's analog 0 pin.  Easy.

A couple important things to point out.  The microphone internally offsets the DC voltage which is ideally centered at the half of the VCC which is ```3.3v / 2 = 1.65v```.(And note - I did say ideally)  This is because the ADC cannot take negative voltages.  Once the value has been converted by the ADC, the expected values should range from 0-675.18.  This is because our maximum value from the microphone is 3.3 volts and is being converted by an ADC with a 5v reference. ```(3.3 / 5) * 1023 = 675.18```.  This isnt great since we're not using the entire resolution of the ADC.  (See below for more about changing resolution).

In order to calculate the amplitude of the signal, we need to remove the DC offset.  As just mention, the signal should be centered at the center of our voltage which means that after the ADC, the center should be at ```675.18 / 2 = 337.59```.  Then we simply subtract 337.59 from each sample and we have an audio wave signal centered at 0.  The actual DC offset however, will vary.  Checkout the spec sheets for each microphone and look at the average readings from the mic in a quite place.  This should give you an approximation of where the mic is centered.

Once the DC offset has been removed, you can calculate the RMS of the signal to calculate decibels.

### Code Overview
The code is based on calculating the root mean squared(RMS) value of the incoming signal and converting this to a decibel value.  RMS is used in order to calulate the proper amplitude.  The alternate way of getting the amplitude is simply getting peak-to-peak values over a sample.  Peak-to-peak is the difference between the maximum peak and the minimum peak of a wave while RMS calculates the area under the curve which more approximates human hearing.  When working with AC signals, RMS should be used.  When working with DC signals, RMS and peak-to-peak are the same.

The following code is used to calculate the RMS.
```
  long sumOfSquares = 0;

  for (int i=0; i<numberOfSamples; i++){
    sample = analogRead(0);         // take a sample
    signal = (sample - dc_offset);  // subtract the dc offset to center the signal at 0
    signal *= signal;               // square the value to remove negative values
    sumOfSquares += signal;         // sum the values
  }
  
  // divide the sum of the squares to get the mean
  // and then take the square root of the mean
  RMS = sqrt(sumOfSquares / numberOfSamples);   
```

To convert the amplitude of the signal into decibels, we use the formula ```db = 20*log10(incoming_voltage)```.  An important not when doing this is that the db formula is actually ```20*log10(incoming_voltage / reference_voltage)```.  The divison of the reference voltage in our code is being performed by the ADC.  The Arduino's ADC has a default reference voltage of 5v which can be modified (see below).  

The code used in the sketch is also calculating a running average.  This was used for my specific application but doesnt need to be used.  The running average effectivly smooths out the signal based on the ```averagedOver``` variable.  The larger the value, the most smoothing effect.

## Extending the Arduino
**Sample rate** and **resolution** are two issues that need to be addressed with when working with audio if quality of audio is of concern.  First, the Arduino has a 10 bit ADC with an internal clock speed of 16MHz.  The ADC clock is by default set to 125kHz which is defined by a default prescaler value of 128 ```(16MHz / 128 = 125kHz)```.  The ADC uses 13 clock cycles to convert the signal which yields a default sample rate of ```125kHz / 13 = 9600Hz```.   This is approximately telephone level quality.  By reducing the prescaler value one can achieve a higher sample rate.  This is well defined [here](http://www.microsmart.co.za/technical/2014/03/01/advanced-arduino-adc/) and the [widepdia page](https://en.wikipedia.org/wiki/Sampling_(signal_processing)) is also very useful.

Regarding **resolution**, the 10 bit ADC has a default reference voltage of 5v which allows the ADC to create values between 0-1023.  The formula to calculate ADC resolution is the estimated full scale voltage range (Efsr) divided by the bit resolution.  The ```Efsr``` is basically the difference between the highest allowable voltage and the lowest allowable voltage.  Why is this useful you ask?  If we then lower the numerator - ```Efsr``` - and keep the bit resolution the same, we **increase** our overall resoultion.  The good news is that the ADC reference voltage for the Arduino can be changed!  See [analogReference](https://www.arduino.cc/en/Reference/AnalogReference).
