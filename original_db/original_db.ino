/*
  Arduino DB
  
  This sketch reads an analog input from a microphone and
  converts the signal into amplitude measured in decibels.
  
  created 20 Jan 2016
  
  By Nicholas Johnson

*/
const int dc_offset = 512;          // DC Offset to be removed from incoming signal 
const int numberOfSamples = 128;    // Number of samples to read at a time
const float aref_voltage = 3.3;     // Reference voltage of the Arduino ADC
const float alpha = 0.95;           // smoothing value

int sample;           // individual reading
long signal;          
long averageReading;
long RMS;             // root mean squared calculation

float db;
long smoothedValue = 0;

void setup() {
  Serial.begin(57600);
}

void loop() {
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
  
  // smoothing filter
  smoothedValue = (alpha * smoothedValue) + ((1-alpha) * RMS);
  
  // convert the RMS value back into a voltage and convert to db  
  db = 20 * log10(smoothedValue * (aref_voltage / 1024.0)); 
  
  Serial.println(db);
}
