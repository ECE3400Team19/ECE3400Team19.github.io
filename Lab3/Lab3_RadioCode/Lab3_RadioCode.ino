/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios. 
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two 
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting 
 * with the serial monitor and sending a 'T'.  The ping node sends the current 
 * time to the pong node, which responds by sending the value back.  The ping 
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

RF24 radio(9,10);

//
// Topology
//

// Need to change identifier numbers for pipes
// 2(3D + N) + X
// D = 4; N = team number; X = 0 for 1 radio and X = 1 for the other radio
// identifier numbers:
//2(3*4 + 19) + 0 = 62 and 63 in decimal, 3E and 3F in hex

// Updated radio pipe addresses according to formula above:
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x000000003ELL, 0x000000003FLL };

      //variables for turning logic and decoding
      unsigned int posX, nextPosX, posY, nextPosY, orient; 
      int nextOrient;

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//




// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

//Count variable that tracks the square number we are currently working on
int count=0; //will go up to 5
// Variables for GUI
bool left_wall, front_wall, right_wall, back_wall, robot_detected;

// The role of the current running sketch
role_e role = role_pong_back;

//global fields for decoding



void setup(void)
{
  //
  // Print preamble
  //
  
  Serial.begin(9600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");
  //reset GUI
  Serial.println("reset");
  delay(1000);

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  // set size to 8 bytes in the payload
  // radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  //if ( role == role_ping_out )
  {
    //radio.openWritingPipe(pipes[0]);
    //opens pipe 1 for reading, pipes[1] = the 40-bit address of pipe 1
    radio.openReadingPipe(1,pipes[1]);
  }
  //else
  {
    //radio.openWritingPipe(pipes[1]);
    //radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}
    int inc = 0;
void loop(void)
{
  
  //
  // Ping out role
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();
    //create 7-bit sequences for each square and send it.

      if (inc > 5) inc = 0;
      unsigned long seq = 0;
      switch(inc) {
    case 0:
    //wall
      //count++;
      seq = B1110000;
      break;
    case 1:
      //count++;
      seq = B0110110;
      break;
    case 2:
      //count++;
      seq = B0110000;
      break;
    case 3:
      //count++;
      seq = B1101010;
      break;
    case 4:
      //count++;
      seq = B1100100;
      break;
    case 5:
      //count++;
      seq = B1110001;
      break;
    default:
      seq = B0000000;
      break;
      }
          Serial.println(seq, BIN);
    radio.write( &seq, sizeof(unsigned long) );
    inc++;

  
//    if (ok) {
//      printf("ok...");
//      count++;
//      printf("\n");
//      delay(30);
//    }
//    else
//      printf("failed.\n\r");

    // Now, continue listening
    //radio.startListening();
//
//    
//    //Wait here until we get a response, or timeout (250ms)
//    unsigned long started_waiting_at = millis();
//    bool timeout = false;
//    while ( ! radio.available() && ! timeout )
//      if (millis() - started_waiting_at > 200 )
//        timeout = true;
////****Let's take care of this later
//    // Describe the results
//    if ( timeout )
//    {
//      printf("Failed, response timed out.\n\r");
//    }
//    else
//    {
//      // Grab the response, compare, and send to debugging spew
//      unsigned long got_time;
//      radio.read( &got_time, sizeof(unsigned long) );
//
//      // Spew it
//      printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
//    }

    // Try again 1s later
    delay(1000);
    }



    
  // Pong back role
  //

  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() ) {


      
      // Dump the payloads until we've gotten everything      
      unsigned long got_response;
      bool done = false;
      while (!done)
      {
      
        // Fetch the data from robot Arduino, and see if this was the last one.
        done = radio.read( &got_response, sizeof(unsigned long) );


        // Spew it
        //printf("Got payload %lu...",got_response);
        delay(20);
      }
      //Serial.println(got_response, BIN);


      //-----------------------------------------------------------------------------------
      unsigned long seq = 0;
      if (inc > 5) {
        posX = 0;
        nextPosX = 0;
        nextPosY = 0;
        nextOrient =0;
        posY = 0;
        orient = 0;
        inc = 0;
      }
      switch(inc) {
    case 0:
    //wall
      //count++;
      seq = B1110000;
      break;
    case 1:
      //count++;
      seq = B0110110;
      break;
    case 2:
      //count++;
      seq = B0110000;
      break;
    case 3:
      //count++;
      seq = B1101010;
      break;
    case 4:
      //count++;
      seq = B1100100;
      break;
    case 5:
      //count++;
      seq = B1110001;
      break;
    default:
      seq = B0000000;
      break;
      }
      inc++;
      got_response = seq;
      //-----------------------------------------------------------------------------------
      
 //Decode got_response
      //Walls --> got_response[6:4]
//print the position
Serial.print(posY);
Serial.print(",");
Serial.print(posX);
Serial.print(",");

//decodes walls and prints all valid wall info
robot_detected = bitRead(got_response, 0);
left_wall = bitRead(got_response, 6);
  front_wall = bitRead(got_response, 5);
  right_wall = bitRead(got_response, 4);
  back_wall = 0;
  switch (orient) {
    case 0: 
      if (front_wall) Serial.print("north=true,");
      if (left_wall) Serial.print("west=true,");
      if (right_wall) Serial.print("east=true,");
      break;
    case 1: 
      if (front_wall) Serial.print("east=true,");
      if (left_wall) Serial.print("north=true,");
      if (right_wall) Serial.print("south=true,");
      break;
    case 2: 
      if (front_wall) Serial.print("south=true,");
      if (left_wall) Serial.print("east=true,");
      if (right_wall) Serial.print("west=true,");
      break;
    case 3: 
      if (front_wall) Serial.print("west=true,");
      if (left_wall) Serial.print("south=true,");
      if (right_wall) Serial.print("north=true,");
      break;
    default:
      break;
  }

//generate seen as in Milestone 2
int seen = (got_response & B1110000) >> 4;
if (got_response & B1) seen |= B1000;

//figure out nextOrient based on turning logic
switch (seen) {
          case B0010: 
            //wall in front only!! turn left
            nextOrient--;
            break;
          case B1000:
      //robot in front only, turn left
            nextOrient--;
            break;
          case B0110:
            //wall in front and left, turn right
            nextOrient++;
            break;
          case B0011:
            //wall in front and right, turn left
            nextOrient--;
            break;
          case B0111:
            //wall in front, right and left, turn around
            nextOrient+=2;
            break;
          case B1100:
            //wall on left, robot in front, turn right
            nextOrient++;
            break;
          case B1001:
            //wall on right, robot in front, turn left
            nextOrient--;
            break;
          case B1101:
            //wall on right and left, robot in front, turn around
            nextOrient+=2;
            break;
          //weird cases
          case B1110:
            //wall on left and front, robot in front?, turn right
            nextOrient++;
            break;
          case B1011:
            //wall on right and front, robot in front?, turn left
            nextOrient--;
            break;
          case B1111:
            //wall on right and left and front, robot in front?, turn around
            nextOrient+=2;
            break;
          default:
            //go straight
            break;
    }

//wrapping around for nextOrient
if (nextOrient == -1) nextOrient = 3;
if (nextOrient ==  4) nextOrient = 0;
if (nextOrient ==  5) nextOrient = 1;

//figure out where we're going based on nextOrient
  switch (nextOrient) {
    case 0:
      nextPosY= posY-1;
      break;
    case 1:
      nextPosX= posX+1;
      break;
    case 2:
      nextPosY= posY+1;
      break;
    case 3:
      nextPosX= posX-1;
      break;
    default:
      break;
  }

//finally, update pos and n
posX = nextPosX;
posY = nextPosY;
orient = nextOrient;

//print treasure info
//print if robot seen
      Serial.print("tshape=");
      int t_shape = (got_response & B1100) >> 2;
      if(t_shape == B00) Serial.print("None");
      else if(t_shape == B01) Serial.print("Circle");
      else if(t_shape == B10) Serial.print("Triangle");
      else Serial.print("Square");
      Serial.print(",tcolor=");
      int t_color = bitRead(got_response, 1);
      if(t_shape == B00) Serial.print("None");
      else if(t_color == B0) Serial.print("Red");
      else Serial.print("Blue");
      if (robot_detected) {    
      Serial.println(",robot=true");
      }
      else Serial.println(",robot=false");
      delay(1000);
      
//      //***********DON'T THINK THIS IS RELEVANT TO US*************
//      // First, stop listening so we can talk
//      radio.stopListening();
//      // Send the final one back.
//      radio.write( &got_time, sizeof(unsigned long) );
//      printf("Sent response.\n\r");
//
//      // Now, resume listening so we catch the next packets.
//      radio.startListening();
    }
  }

  //
  // Change roles
  //

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");

      // Become the primary transmitter (ping out)
      role = role_ping_out;
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    }
    else if ( c == 'R' && role == role_ping_out )
    {
      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");
      
      // Become the primary receiver (pong back)
      role = role_pong_back;
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
    }
  }
}

unsigned long generate_sequence() {
  switch(count) {
    case 0:
    //wall
      //count++;
      return B1110000;
      break;
    case 1:
      //count++;
      return B0110110;
      break;
    case 2:
      //count++;
      return B0110000;
      break;
    case 3:
      //count++;
      return B1101010;
      break;
    case 4:
      //count++;
      return B1100100;
      break;
    case 5:
      //count++;
      return B1110001;
      break;
    default:
      return B0000000;
      break;
  }
}
//
//void print_to_GUI(unsigned long got_response) {
//      //Decode got_response
//      //Walls --> got_response[6:4]
////print the position
//Serial.print(posX);
//Serial.print(",");
//Serial.print(posY);
//Serial.print(",");
//
////decodes walls and prints all valid wall info
//left_wall = bitRead(got_response, 6);
//  front_wall = bitRead(got_response, 5);
//  right_wall = bitRead(got_response, 4);
//  back_wall = 0;
//  switch (orient) {
//    case 0: 
//      if (front_wall) Serial.print("north=true,");
//      if (left_wall) Serial.print("west=true,");
//      if (right_wall) Serial.print("east=true,");
//      break;
//    case 1: 
//      if (front_wall) Serial.print("east=true,");
//      if (left_wall) Serial.print("north=true,");
//      if (right_wall) Serial.print("south=true,");
//      break;
//    case 2: 
//      if (front_wall) Serial.print("south=true,");
//      if (left_wall) Serial.print("east=true,");
//      if (right_wall) Serial.print("west=true,");
//      break;
//    case 3: 
//      if (front_wall) Serial.print("west=true,");
//      if (left_wall) Serial.print("south=true,");
//      if (right_wall) Serial.print("north=true,");
//      break;
//    default:
//      break;
//  }
//
////generate seen as in Milestone 2
//int seen = (got_response & B1110000) >> 4;
//if (got_response & B1) seen |= B1000;
//
////figure out nextOrient based on turning logic
//switch (seen) {
//          case B0010: 
//            //wall in front only!! turn left
//            nextOrient--;
//            break;
//          case B1000:
//      //robot in front only, turn left
//            nextOrient--;
//            break;
//          case B0110:
//            //wall in front and left, turn right
//            nextOrient++;
//            break;
//          case B0011:
//            //wall in front and right, turn left
//            nextOrient--;
//            break;
//          case B0111:
//            //wall in front, right and left, turn around
//            nextOrient+=2;
//            break;
//          case B1100:
//            //wall on left, robot in front, turn right
//            nextOrient++;
//            break;
//          case B1001:
//            //wall on right, robot in front, turn left
//            nextOrient--;
//            break;
//          case B1101:
//            //wall on right and left, robot in front, turn around
//            nextOrient+=2;
//            break;
//          //weird cases
//          case B1110:
//            //wall on left and front, robot in front?, turn right
//            nextOrient++;
//            break;
//          case B1011:
//            //wall on right and front, robot in front?, turn left
//            nextOrient--;
//            break;
//          case B1111:
//            //wall on right and left and front, robot in front?, turn around
//            nextOrient+=2;
//            break;
//          default:
//            //go straight
//            break;
//    }
//
////wrapping around for nextOrient
//if (nextOrient == -1) nextOrient = 3;
//if (nextOrient ==  4) nextOrient = 0;
//if (nextOrient ==  5) nextOrient = 1;
//
////figure out where we're going based on nextOrient
//  switch (nextOrient) {
//    case 0:
//      nextPosY= posY-1;
//      break;
//    case 1:
//      nextPosX= posX+1;
//      break;
//    case 2:
//      nextPosY= posY+1;
//      break;
//    case 3:
//      nextPosX= posX-1;
//      break;
//    default:
//      break;
//  }
//
////finally, update pos and n
//posX = nextPosX;
//posY = nextPosY;
//orient = nextOrient;
//
////print treasure info
////print if robot seen
//      Serial.print("tshape=");
//      int t_shape = (got_response & B1100) >> 2;
//      if(t_shape == B00) Serial.print("None");
//      else if(t_shape == B01) Serial.print("Circle");
//      else if(t_shape == B10) Serial.print("Triangle");
//      else Serial.print("Square");
//      Serial.print(",tcolor=");
//      int t_color = bitRead(got_response, 1);
//      if(t_shape == B00) Serial.print("None");
//      else if(t_color == B0) Serial.print("Red");
//      else Serial.print("Blue");    
//      Serial.print(",robot=");
//      Serial.println(robot_detected);
//      delay(1000);
//}
//
//vim:cin:ai:sts=2 sw=2 ft=cpp
