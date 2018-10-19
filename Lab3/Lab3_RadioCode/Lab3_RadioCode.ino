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
int count=0;
// Variables for GUI
bool left_wall, front_wall, right_wall, robot_detected;

// The role of the current running sketch
role_e role = role_pong_back;

void setup(void)
{
  //
  // Print preamble
  //
  
  Serial.begin(57600);
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

void loop(void)
{
  
  //**************PING OUT ROLE CODE NOT YET MODIFIED!!!******************
  //*****Has to take what is in the current square and send the appropriate 7-bit sequence****
  //***We have to determine parameter values for each square. We will work with a virtual 2x3 maze.
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
    printf("Now sending %lu...",time);
    bool ok = radio.write( &time, sizeof(unsigned long) );
    
    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
    }

    // Try again 1s later
    delay(1000);
  }
  //***************PONG BACK ROLE COMPLETELY FINISHED, CODE COMPILES 10/18/18 10:10 PM*****
  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() ) {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      unsigned long got_response;
      bool done = false;
      while (!done)
      {
        // Fetch the data from robot Arduino, and see if this was the last one.
        done = radio.read( &got_response, sizeof(unsigned long) );
 
        print_to_GUI(got_response);
        // Spew it
        printf("Got payload %lu...",got_response);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(20);
      }
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

void print_to_GUI(unsigned long get_response) {
      //Decode got_response
      unsigned long got_response = B1010111;
      //Walls --> got_response[6:4]
      left_wall = bitRead(got_response, 6);
      front_wall = bitRead(got_response, 5);
      right_wall = bitRead(got_response, 4);    
      robot_detected = bitRead(got_response, 0);
      //Print out where the robot is
      
      switch(count) {
        case 0:
          Serial.print("0,0,north=");
          break;
        case 1:
          Serial.print("0,1,north=");
          break;
        case 2:
          Serial.print("0,2,north=");
          break;
        case 3:
          Serial.print("1,2,north=");
          break;
        case 4:
          Serial.print("1,1,north=");
          break;
        case 5:
          Serial.print("1,0,north=");
          break;
        default:
          break;
      }
      if(front_wall) Serial.print("true");
      else Serial.print("false");
      Serial.print(",west=");
      if(left_wall) Serial.print("true");
      else Serial.print("false");
      Serial.print(",east=");
      if(right_wall) Serial.print("true");
      else Serial.print("false");
      Serial.print(",tshape=");
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
      Serial.print(",robot=");
      Serial.println(robot_detected);
      delay(1000);
}

// vim:cin:ai:sts=2 sw=2 ft=cpp
