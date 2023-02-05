#include <Arduino.h>
#include <OneWire.h>
#include <wire.h>
 
// DS18S20 Temperature chip i/o
OneWire ds(10);  // on pin 10

//*** set program constants
int HighByte, LowByte;
int TReading;
int SignBit;
float Tc_Float;
 
byte i;
byte present = 0;
byte data[12];
byte addr[8];
 
void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);

  //*** Find device address
  if ( !ds.search(addr)) {
    Serial.print("No more addresses.\n");
    ds.reset_search();
    return;
  }

  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.print("CRC is not valid!\n");
    return;
  }
 //*** Identify device
  if ( addr[0] == 0x10) {
    Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( addr[0] == 0x28) {
    Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
    Serial.print("Device family is not recognized: 0x");
    Serial.println(addr[0],HEX);
    return;
  }
}
 
void loop(void) {
 
  /*
  * we could do a ds.depower() here, 
  * but the reset will take care of it.
  */
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
 
  delay(1000);  // maybe 750ms is enough, maybe not

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
 
  // Serial.print("P=");  // RAW temp data in HEX 
  // Serial.print(present,HEX);
  // Serial.print(" ");
  for ( i = 0; i < 9; i++) {  // we need 9 bytes
    data[i] = ds.read();
    // Serial.print(data[i], HEX); //print raw tmp in HEX
    // Serial.print(" ");
  }
  // Serial.print(" CRC=");
  // Serial.print( OneWire::crc8( data, 8), HEX);
  // Serial.println();
 
  //Conversion of raw data to C
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }

  Serial.print("Value fron sensor = ");
  Serial.println(TReading);
  Serial.println("");

  /*
  * The raw sensor data from TReading is an INTEGER 
  * it must be "converted" to a FLOAT to be useful
  * So...
  * If we multiply TReading -which is an INTEGER 
  * by 0.0625 -which is a FLOAT, the result will be a
  * FLOAT
  */
  Tc_Float = TReading * .0625;  // convert from raw (INT) to deg. C (FLOAT)
  
  Serial.print("Tc_Float in deg C= ");
  if (SignBit) // If its negative
  {
    Serial.print("-");
  }
  Serial.println(Tc_Float);


}
