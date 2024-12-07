/*
 * Orignal code was retrieved from here: 
 * https://projecthub.arduino.cc/mdraber/controlling-8x8-dot-matrix-with-max7219-and-arduino-0c417a
 * 
 * Code was edited by Chris Whitmire
 */

#include <SPI.h>

/////////////////////////////////////////////////////
/// spi vars
/////////////////////////////////////////////////////

int cs = 3; // can be any digital pin

// Control settings for the MAX7219 Driver for LED dot matrix (DONT CHANGE)
int decodeMode = 9;
int intensity = 10;
int scanLimit = 11;
int shutDown = 12;
int displayTest = 16;

uint8_t dry[8] = {

  B00000011,
  B00000011,
  B00111000,
  B00111010,
  B11111110,
  B10111000,
  B00111000,
  B11111111

};

uint8_t damp1[8] = {

  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B01101100,
  B11111111,
  B11111111

};

uint8_t damp2[8] = {

  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00110110,
  B11111111,
  B11111111

};

uint8_t wet1[8] = {

  B00000000,
  B00000000,
  B00000000,
  B01101100,
  B11111111,
  B11111111,
  B11111111,
  B11111111

};

uint8_t wet2[8] = {

  B00000000,
  B00000000,
  B00000000,
  B00110110,
  B11111111,
  B11111111,
  B11111111,
  B11111111

};

uint8_t drenched1[8] = {

  B00000000,
  B01101100,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111

};

uint8_t drenched2[8] = {

  B00000000,
  B00110110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111

};

/////////////////////////////////////////////////////
/// capacitor vars
/////////////////////////////////////////////////////

const int dryNum = 436;
const int wetNum = 200;

void setup() {
  
  Serial.begin(9600);

  /////////////////////////////////////////////////////
  /// spi setup
  /////////////////////////////////////////////////////
  
  // put your setup code here, to run once:
  pinMode(cs, OUTPUT);

  // start SPI connection
  SPI.begin();

  // run test to make sure it is working
  sendData(displayTest, 0x01); // start test
  delay(1000);
  sendData(displayTest, 0x00);// stop test

  // set other settings
  sendData(decodeMode, 0x00); // disable bcd mode
  sendData(intensity, 0x0f); // brightness of the leds
  sendData(scanLimit, 0x0f); // scan all digits
  sendData(shutDown, 0x01); // turn on chip

  // clear the dot matrix
  for (int i = 1; i<9; i++){
    sendData(i, B00000000);
  }

}

void loop() {

  handleCapacitor();

}

/////////////////////////////////////////////////////
/// spi functions
/////////////////////////////////////////////////////

void dryDisplay() {

  for (int row = 1 ; row <= 8 ; row++) {
  
    sendData(row, dry[row-1]);

  }

  delay(800);

  // clear the dot matrix
  for (int i = 1; i<9; i++) {

    sendData(i, B00000000);

  }

  delay(800);

}

void dampDisplay() {

  for (int row = 1 ; row <= 8 ; row++) {
  
    sendData(row, damp1[row-1]);

  }

  delay(500);

  for (int row = 1 ; row <= 8 ; row++) {
  
    sendData(row, damp2[row-1]);

  }

  delay(500);

}

void wetDisplay() {

  for (int row = 1 ; row <= 8 ; row++) {
  
    sendData(row, wet1[row-1]);

  }

  delay(500);

  for (int row = 1 ; row <= 8 ; row++) {
  
    sendData(row, wet2[row-1]);

  }

  delay(500);

}

void drenchedDisplay() {

  for (int row = 1 ; row <= 8 ; row++) {
  
    sendData(row, drenched1[row-1]);

  }

  delay(500);

  for (int row = 1 ; row <= 8 ; row++) {
  
    sendData(row, drenched2[row-1]);

  }

  delay(500);

}

/*
 * Data for sending something to the dot matrix
 */
void sendData(uint8_t address, uint8_t value) {
  // signal that you are about to send data
  digitalWrite(cs, LOW);
  // send the address
  SPI.transfer(address);
  // send the value that should be saved at the address
  SPI.transfer(value);
  // signal that you are done sending data
  digitalWrite(cs, HIGH);
}

/////////////////////////////////////////////////////
/// capacitor functions
/////////////////////////////////////////////////////

void handleCapacitor() {

  int sensorVal = analogRead(A2);
  int percentageHumidity = map(sensorVal, wetNum, dryNum, 100, 0);

  Serial.println(percentageHumidity + "%");

  if (percentageHumidity > 75 && percentageHumidity < 100) {

    drenchedDisplay();

  } else if (percentageHumidity > 50 && percentageHumidity < 75) {

    wetDisplay();

  } else if (percentageHumidity > 25 && percentageHumidity < 50) {

    dampDisplay();

  } else {

    dryDisplay();

  }
  
  delay(100);

}
