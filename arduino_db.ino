const int dc_offset = 328;          // DC Offset to be removed from incoming signal 
const int numberOfSamples = 128;    // Number of samples to read at a time
const float aref_voltage = 5.0;     // Reference voltage of the Arduino ADC

int sample;           // individual reading
long signal;          
long averageReading;
long RMS;             // root mean squared calculation

float db;

long runningAverage = 0;        
const float averagedOver = 16;    // time over which the samples are averaged

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
  
  // method to create a running average - essentially a smoothing filter
  runningAverage = (((averagedOver - 1) * runningAverage) + RMS) / averagedOver;
  
  // convert the RMS value back into a voltage and convert to db  
  db = 20 * log10(runningAverage * (aref_voltage / 1023.0)); 
  
  Serial.println(db);

  delay(100);
}
