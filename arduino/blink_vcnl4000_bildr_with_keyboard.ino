#include <Wire.h>
#define VCNL4000_ADDRESS 0x13  //I2C Address of the board

uint8_t buf[8] = { 0 };   /* Keyboard report buffer */


// jumper pin for debugging with speaker sound

int debugPin = 0;
int debugState = HIGH;          // assume we're debugging

// This is for averaging signals

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 5;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
long total = 0;                 // the running total
int average = 0;                // the average
int activeBlink = 0;            // is the person currently blinking?


void setup(){
  Serial.begin(9600);  // Serial's used to debug and print data
  Wire.begin();  // initialize I2C stuff
  initVCNL4000(); //initilize and setup the board

  // pinMode(debugPin, INPUT);
  // debugState = digitalRead(debugPin);
  
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  
}


void loop(){
  // unsigned int ambientValue = readAmbient(); //can a tiny bit slow
  
  int proximityValue = readProximity();

  // find the average of the last 10 signals 
  
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = proximityValue;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;

 
  // debug
  /*
  Serial.print(" Current:  ");
  Serial.print(proximityValue);
  Serial.print(", Average: ");
  Serial.print(average);
  Serial.print(", Difference: ");
  Serial.println(proximityValue - average);
  */
  

  // if this sample is some amount higher than the average, buzz a speaker on pin 8

  if ((proximityValue - average) > 100) {
    if (activeBlink == 0) {
      activeBlink = 1;
      blink();
    }
  }

  // if there's not any difference from the average, we're not in a real blink anymore
  if ((proximityValue - average) < 100) {
    if (activeBlink == 1) {
      activeBlink = 0;
    }
  }
  
}

void blink() {
    // someone just blinked!
    
    buf[2] = 44;    // Spacebar
    Serial.write(buf, 8); // Send keypress
    releaseKey();
    
    if (debugState == HIGH) {
      tone(8, 100, 5);
    }
    
}


void releaseKey() 
{
  buf[0] = 0;
  buf[2] = 0;
  Serial.write(buf, 8);  // Release key  
  // delay(500);
}





// ------ stock code for the VCNL4000

void initVCNL4000(){
  byte temp = readVCNLByte(0x81);

  /*VNCL400 init params
   Feel free to play with any of these values, but check the datasheet first!*/
  writeVCNLByte(0x84, 0x0F);  // Configures ambient light measures - Single conversion mode, 128 averages
  writeVCNLByte(0x83, 15);  // sets IR current in steps of 10mA 0-200mA --> 200mA
  writeVCNLByte(0x89, 2);  // Proximity IR test signal freq, 0-3 - 781.25 kHz
  writeVCNLByte(0x8A, 0x81);  // proximity modulator timing - 129, recommended by Vishay 
}


unsigned int readProximity(){
  // readProximity() returns a 16-bit value from the VCNL4000's proximity data registers
  byte temp = readVCNLByte(0x80);
  writeVCNLByte(0x80, temp | 0x08);  // command the sensor to perform a proximity measure

  while(!(readVCNLByte(0x80)&0x20));  // Wait for the proximity data ready bit to be set
  unsigned int data = readVCNLByte(0x87) << 8;
  data |= readVCNLByte(0x88);

  return data;
}


unsigned int readAmbient(){
  // readAmbient() returns a 16-bit value from the VCNL4000's ambient light data registers
  byte temp = readVCNLByte(0x80);
  writeVCNLByte(0x80, temp | 0x10);  // command the sensor to perform ambient measure

  while(!(readVCNLByte(0x80)&0x40));  // wait for the proximity data ready bit to be set
  unsigned int data = readVCNLByte(0x85) << 8;
  data |= readVCNLByte(0x86);

  return data;
}


void writeVCNLByte(byte address, byte data){
  // writeVCNLByte(address, data) writes a single byte of data to address
  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}


byte readVCNLByte(byte address){
  // readByte(address) reads a single byte of data from address
  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(VCNL4000_ADDRESS, 1);
  while(!Wire.available());
  byte data = Wire.read();

  return data;
}
