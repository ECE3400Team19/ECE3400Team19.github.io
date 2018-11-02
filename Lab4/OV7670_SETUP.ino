#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21  /*TODO: write this in hex (eg. 0xAB) */
//0x42 maybe



void loop() { // run over and over

}
///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("start");
   
  // TODO: READ KEY REGISTERS
  OV7670_write_register(0x12, 128); //reset all registers, COM7
  delay(100);
  OV7670_write_register(0x12, 134); //enable color bar and RGB 
  delay(100);
  OV7670_write_register(0x0C, 8); //COM3 enable scaling, set bit[3]
  delay(100);
  //default reg values
  byte com14 = 0x0E;
  byte com17 = 8;
  byte com15 = 0xC0;
  byte clk = 0x80;
  OV7670_write_register(0x3E, com14 |= 8); //COM14 enable manual scaling, set bit[3]
  delay(100);
  OV7670_write_register(0x11, clk |= 64); //use internal clk as external clk, set bit[6]
  delay(100);
  OV7670_write_register(0x1B, 16); //set delay of PCLK = 16, 16 pixels read
  delay(100);
  OV7670_write_register(0x42, com17 |= 8); //set third bit of com17 for DSP color bar enable
  delay(100);
  OV7670_write_register(0x40, com15 |= 16); //set fourth bit of com15 for RGB 565
  delay(100);

  
//  com14 = read_register_value(0x3E);
//  delay(100);
//  clk = read_register_value(0x11);
//  delay(100);
//  com17 = read_register_value(0x42);
//  delay(100);
//  com15 = read_register_value(0x40);
//  delay(100);
//  Serial.println("done reading registers");
  // TODO: WRITE KEY REGISTERS

  // possible to dos:
  // vertical and mirror flip
  // automatic gain scaling
  Serial.println("done writing registers");
  read_key_registers();
  set_color_matrix();
}




///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
  Serial.print("0x12 ");
  Serial.println(read_register_value(0x12));
  Serial.print("0x0C");
  Serial.println(read_register_value(0x0C));
  Serial.print("0x3E");
  Serial.println(read_register_value(0x3E));
  Serial.print("0x11");
  Serial.println(read_register_value(0x11));
  Serial.print("0x1B");
  Serial.println(read_register_value(0x1B));
  Serial.print("0x42");
  Serial.println(read_register_value(0x42));
}

byte read_register_value(int register_address){
  Serial.println("trying to read register");
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Serial.println("began transmission");
  Wire.write(register_address);
  Serial.println("wrote wire");
  Wire.endTransmission();
  Serial.println("ended transmission");
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
  Serial.println("done requesting address");
  while(Wire.available()<1);
  Serial.println("wire is available");
  data = Wire.read();
  return data;
}

String OV7670_write(int start, const byte *pData, int size){
    int n,error;
    Wire.beginTransmission(OV7670_I2C_ADDRESS);
    n = Wire.write(start);
    if(n != 1){
      return "I2C ERROR WRITING START ADDRESS";   
    }
    n = Wire.write(pData, size);
    if(n != size){
      return "I2C ERROR WRITING DATA";
    }
    error = Wire.endTransmission(true);
    if(error != 0){
      return String(error);
    }
    return "no errors :)";
 }

String OV7670_write_register(int reg_address, byte data){
  return OV7670_write(reg_address, &data, 1);
 }

void set_color_matrix(){
    OV7670_write_register(0x4f, 0x80);
    OV7670_write_register(0x50, 0x80);
    OV7670_write_register(0x51, 0x00);
    OV7670_write_register(0x52, 0x22);
    OV7670_write_register(0x53, 0x5e);
    OV7670_write_register(0x54, 0x80);
    OV7670_write_register(0x56, 0x40);
    OV7670_write_register(0x58, 0x9e);
    OV7670_write_register(0x59, 0x88);
    OV7670_write_register(0x5a, 0x88);
    OV7670_write_register(0x5b, 0x44);
    OV7670_write_register(0x5c, 0x67);
    OV7670_write_register(0x5d, 0x49);
    OV7670_write_register(0x5e, 0x0e);
    OV7670_write_register(0x69, 0x00);
    OV7670_write_register(0x6a, 0x40);
    OV7670_write_register(0x6b, 0x0a);
    OV7670_write_register(0x6c, 0x0a);
    OV7670_write_register(0x6d, 0x55);
    OV7670_write_register(0x6e, 0x11);
    OV7670_write_register(0x6f, 0x9f);
    OV7670_write_register(0xb0, 0x84);
}
