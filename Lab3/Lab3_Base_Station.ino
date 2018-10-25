
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

const uint64_t pipes[2] = { 0x000000003ELL, 0x000000003FLL };

unsigned long got_response;
bool done = false;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setRetries(15,15);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();

}

void genGUIPrints(unsigned long got_response) {
  // Data Breakdown:
  //Y coord: Bits[15:12]
  //X coord: Bits[11:8]
  //Treasure Shape: Bits[7:6]
  //Treasure Color: Bit[5]
  //Robot Detected: Bit[4]
  //N: Bit[3]
  //S: Bit[2]
  //E: Bit[1]
  //W: Bit[0]
  
  //print first 4 bits (y coord)
  Serial.print(got_response >> 12);
  Serial.print(",");
  //print next 4 bits (x coord)
  Serial.print((got_response >> 8) & B00001111);
  Serial.print(",");
  
  if (bitRead(got_response, 3) ) Serial.print("north=true,");
  if (bitRead(got_response, 2) ) Serial.print("south=true,");
  if (bitRead(got_response, 1) ) Serial.print("east=true,");
  if (bitRead(got_response, 0) ) Serial.print("west=true,");
  
  Serial.print("tshape=");
  int t_shape = (got_response & B11000000) >> 6;
  if(t_shape == B00) Serial.print("None");
  else if(t_shape == B01) Serial.print("Circle");
  else if(t_shape == B10) Serial.print("Triangle");
  else Serial.print("Square");
  
  Serial.print(",tcolor=");
  int t_color = bitRead(got_response, 1);
  if(t_shape == B00) Serial.print("None");
  else if(t_color == B0) Serial.print("Red");
  else Serial.print("Blue");
  
  if (bitRead(got_response, 4)) Serial.println(",robot=true");
}

void loop() {
  // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_response, sizeof(unsigned long) );

        // Spew it
        //printf("Got payload %lu...",got_response);

  // Delay just a little bit to let the other unit
  // make the transition to receiver
  delay(20);
      }
      //call the function that prints to the GUI
      genGUIPrints(got_response);
      
      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &got_response, sizeof(unsigned long) );
      //printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();

    }
}
