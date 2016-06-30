/*
 * Slave to RPi
 * 
 * by Nicholas Johnson
 * May 4th, 2016
 * 
 */
#include <Wire.h>

#define SLAVE_ADDRESS 0x04
#define FLOATS_SENT 2
float data[FLOATS_SENT];

const int micPin= 0;              // pin for analog microphone input
const float dc_offset = 512;        // DC Offset to be removed from incoming signal
const float numberOfSamples = 5818.0;  // Number of samples to read at a time
const float aref_voltage = 3.3;   // Reference voltage of the Arduino ADC

float db, RMS_voltage, runningAverage, peakValue;
long signal, RMS, sample;

int averagedOver = 23; // number of readings (1 reading = numSamples * 0.022)

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  analogReference(EXTERNAL);

  // set prescale to 16 = 1Mhz. ~22µs per sameple
  bitClear(ADCSRA,ADPS0);
  bitClear(ADCSRA,ADPS1);
  bitSet(ADCSRA,ADPS2);
  
  Wire.begin(SLAVE_ADDRESS);       // define location of this device
  Wire.onRequest(sendData); // add callback function
  //starttime = millis();
}

void loop() {
  long sumOfSquares = 0;
  
  for(int i = 0; i < numberOfSamples; i++){
    sample = analogRead(micPin);    // take a sample
    signal = (sample - dc_offset);  // center at 0
    signal *= signal;               // square to remove negative values
    sumOfSquares += signal;         // sum samples
  }
  
  // divide to get the mean and then take the sqrt
  RMS = sqrt(sumOfSquares/numberOfSamples);
  
  // convert the RMS value back into a voltage and 
  RMS_voltage = RMS * (aref_voltage / 1024.0);

  if (RMS_voltage > peakValue){
    peakValue = RMS_voltage;
  }
  
  // averaged over 23 readings.  23 readings * 128ms = 3 seconds
  runningAverage=(((averagedOver-1)*runningAverage)+RMS_voltage)/averagedOver;
}

void sendData(){
  data[0] = peakValue;
  data[1] = runningAverage;
  Wire.write((byte*) &peakValue, FLOATS_SENT*sizeof(float));
  peakValue = 0;
}
