#include <Servo.h>
#define LOG_OUT 1 // use the log output function
#define FFT_N 128 // CHANGED 128 pt FFT
#include <FFT.h> // include the library
//include Radio Headers and Library
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

Servo left; //180 turns wheel backward
Servo right; //180 turns wheel forward

RF24 radio(9,10);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x000000003ELL, 0x000000003FLL };

//Data Structure for Map Storage
//struct Square {
//  //includes color and shape, 3 bits
//  unsigned int treasure;
//  
//  unsigned int robot;
//  unsigned int northWall;
//  unsigned int southWall;
//  unsigned int eastWall;
//  unsigned int westWall;
//} pack;

//struct Square maze[81];
unsigned long transmission;
unsigned int posX, nextPosX, posY, nextPosY; 
int nextOrient, orient;

int leftPin = A0; //left
int middlePin = A5; //center
int rightPin = A1; //right
int IR = A3;
int leftSensor;
int rightSensor;
int middleSensor;
int leftSpeed;
int rightSpeed;
int lw = 3;
int fw = 4;
int rw = 2;
//int rightLED = 12;
//int frontLED = 2;
//int leftLED =  3;
//int robotLED = 4;
boolean IRDetected;
int defaultT = TIMSK0;
int defaultADC = ADCSRA;
int defaultADMUX = ADMUX;
int defaultD = DIDR0;

// MICROPHONE is in A2
// IR is in A3

//helper methods
void runFFT(){
    defaultT = TIMSK0;
    defaultADC = ADCSRA;
    defaultADMUX = ADMUX;
    defaultD = DIDR0;
    TIMSK0 = 0; // turn off timer0 for lower jitter
    ADCSRA = 0xe5; // set the adc to free running mode
    ADMUX = 0x43; // use adc3
    DIDR0 = 0x01; // turn off the digital input for adc0
    cli();  // UDRE interrupt slows this way down on arduino1.0
    
    for (int i = 0 ; i < 256 ; i += 2) {  //CHANGED
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();
//    Serial.println("start");
//    for (byte i = 0 ; i < FFT_N/2 ; i++) { 
//      Serial.println(fft_log_out[i]); // send out the data
//    }
    TIMSK0 = defaultT; 
    ADCSRA = defaultADC; 
    ADMUX = defaultADMUX; 
    DIDR0 = defaultD; 
}

void runMICFFT(){
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 256 ; i += 2) { // save 256 samples
    fft_input[i] = analogRead(A2); // put analog input (pin A0) into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }

  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
//  for (byte i = 0 ; i < FFT_N / 2 ; i++) {
//    Serial.println(fft_log_out[i]); // send out the data
//  }
}

void goStraight() {
  nextOrient = orient;
  leftSpeed = 95;
  rightSpeed =85;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(300);
}

void turnLeft() {
  //go straight for 200 ms and then turn left
  left.write(95);
  right.write(85);
  nextOrient = orient - 1;
  if (nextOrient == -1) nextOrient = 3;
  delay(200);
  leftSpeed = 85;
  rightSpeed = 85;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(700);
}

void turnRight() {
  //go straight for 200 ms and then turn right
  left.write(95);
  right.write(85);
  nextOrient = orient +1;
  if (nextOrient == 4) nextOrient = 0;
  delay(200);
  leftSpeed = 95;
  rightSpeed = 95;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(700);
}

void turnAround() {
  //straight for 200 ms and then turn around
  leftSpeed = 95;
  rightSpeed = 95;
  nextOrient = orient + 2;
  if (nextOrient == 4) nextOrient = 0;
  if (nextOrient == 5) nextOrient = 1;
  left.write(95);
  right.write(85);
  delay(200);
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(1800); 
}

//void updateMap(int seen) {
//  unsigned int addr = 9*posY + posX;
//  maze[addr].robot = (seen >> 3);
//  unsigned int leftWall = seen >> 2 & B01;
//  unsigned int frontWall = seen >> 1 & B001;
//  unsigned int rightWall = seen & B001;
//  switch (orient){
//    case (0):
//        maze[addr].westWall = leftWall;
//        maze[addr].northWall = frontWall;
//        maze[addr].eastWall = rightWall;
//        break;
//    case (1):
//        maze[addr].northWall = leftWall;
//        maze[addr].eastWall = frontWall;
//        maze[addr].southWall = rightWall;
//        break;
//    case (2):
//        maze[addr].eastWall = leftWall;
//        maze[addr].southWall = frontWall;
//        maze[addr].westWall = rightWall;
//        break;
//    case (3):
//        maze[addr].southWall = leftWall;
//        maze[addr].westWall = frontWall;
//        maze[addr].northWall = rightWall;
//        break;
//    default:
//        break;
//  }
//}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // use the serial port
  Serial.println("starting");
  left.attach(6);
  right.attach(5);
  left.write(90);
  right.write(90);
  
  //wall sensors
  pinMode(lw, INPUT);
  pinMode(fw, INPUT);
  pinMode(rw, INPUT);

  //output LEDs
  //pinMode(rightLED, OUTPUT);
  //pinMode(leftLED, OUTPUT);
  //pinMode(frontLED, OUTPUT);
  //pinMode(robotLED, OUTPUT);

  //radio setup
  radio.begin();
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  
  Serial.println("finished setup");
  waitForMic();

  //DFS stack setup
  StackArray <byte> frontier; 
  frontier.push(0);
  boolean visited[20];
  
}

void checkWallSensors(){
  Serial.print("left wall : ");
  Serial.print(digitalRead(lw));
  Serial.print(" front wall : ");
  Serial.print(digitalRead(fw));
  Serial.print(" right wall : ");
  Serial.print(digitalRead(rw));
  Serial.println();
  delay(50);
}

void waitForMic(){
  while(1){
    runMICFFT(); //false indicates run FFT for microphone
    Serial.println("waiting for 660 tone");
    if (fft_log_out[10] > 60){
      //Serial.println(fft_log_out[10]);
      Serial.println("660 Hz signal detected");
      break;
      }
   delay(50);
  }
}

void loop() {
    //Serial.println("entering loop");
    //read line sensors
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);    

    
    //At Intersection, need to decide where to go
    if (leftSensor < 800  && middleSensor< 800 && rightSensor < 800){
        //FFT IR
        Serial.println("at an intersection");
        runFFT(); //true indicates run FFT for IR

        //check if IR sensor detects another robot based on FFT
        
        //for (int i = 38; i < 48; i++){  //LEGACY : 256 bit fft
        IRDetected = 0;
        for (int i = 15; i < 25; i++){ //changed
          if (fft_log_out[i] > 125){
            //Serial.println(fft_log_out[i]);
            IRDetected = 1;
            break;
          }
        }
        
        //What can you see?
        int seen = B0000;
        //digitalWrite(leftLED, LOW);
        //digitalWrite(rightLED, LOW);
        //digitalWrite(frontLED, LOW);
        //digitalWrite(robotLED, LOW);
        if (IRDetected){  
          Serial.println("IR detected");       
          seen |= B1000;
          //digitalWrite(robotLED, HIGH);
        }
        //left wall
        if (digitalRead(lw)){  
          Serial.println("left wall detected");  
          seen |= B0100;
          //digitalWrite(leftLED, HIGH);
        }
        //front wall
        if (digitalRead(fw)){  
          Serial.println("front wall detected");  
          seen |= B0010;
          //digitalWrite(frontLED, HIGH);
        }
        //right wall
        if (digitalRead(rw)){
          Serial.println("right wall detected");     
          seen |= B0001;
          //digitalWrite(rightLED, HIGH);
        }
        
        //Update the map in Storage based on current orientation
        //updateMap(seen);

        //turn accordingly
        switch (seen) {
          case B0010: 
            //wall in front only!! turn left
            turnLeft();
            Serial.println("wall in front");
            break;
          case B1000:
            turnLeft();
            Serial.println("robot in front");
            break;
          case B0110:
            //wall in front and left, turn right
            turnRight();
            Serial.println("wall in front and left, turn right");
            break;
          case B0011:
            //wall in front and right, turn left
            turnLeft();
            Serial.println("wall in front and right, turn left");
            break;
          case B0111:
            //wall in front, right and left, turn around
            turnAround();
            Serial.println("wall in front, right and left, turn around");
            break;
          case B1100:
            //wall on left, robot in front, turn right
            turnRight();
            Serial.println("wall on left, robot in front, turn right");
            break;
          case B1001:
            //wall on right, robot in front, turn left
            turnLeft();
            Serial.println("wall on right, robot in front, turn left");
            break;
          case B1101:
            //wall on right and left, robot in front, turn around
            turnAround();
            Serial.println("wall on right and left, robot in front, turn around");
            break;
          //weird cases
          case B1110:
            //wall on left and front, robot in front?, turn right
            turnRight();
            Serial.println("wall on left and front, robot in front?, turn right");
            break;
          case B1011:
            //wall on right and front, robot in front?, turn left
            turnLeft();
            Serial.println("wall on right and front, robot in front?, turn left");
            break;
          case B1111:
            //wall on right and left and front, robot in front?, turn around
            turnAround();
            Serial.println("wall on right and left and front, robot in front?, turn around");
            break;
          default:
            //go straight
            goStraight();
            Serial.println("going straight");
            Serial.println(seen);
            break;
    }    
    //figure out where we're going based on how we turned
      switch (nextOrient) {
        case (0):
          nextPosY= posY-1;
          break;
        case (1):
          nextPosX= posX+1;
          break;
        case (2):
          nextPosY= posY+1;
          break;
        case (3):
          nextPosX= posX-1;
          break;
        default:
          break;
    }

    
    //Radio Transmission
    //set up the message to transmit
    transmission = 0;
    transmission |= posY << 12;
    transmission |= posX << 8;
    //the rest is treasure stuff we haven't gotten to yet
    transmission |= IRDetected << 4;
    unsigned int leftWall = seen >> 2 & B01;
    unsigned int frontWall = seen >> 1 & B001;
    unsigned int rightWall = seen & B001;
    unsigned int northWall;
    unsigned int southWall;
    unsigned int eastWall;
    unsigned int westWall;
  switch (orient){
    case (0):
        westWall = leftWall;
        northWall = frontWall;
        eastWall = rightWall;
        break;
    case (1):
        northWall = leftWall;
        eastWall = frontWall;
        southWall = rightWall;
        break;
    case (2):
        eastWall = leftWall;
        southWall = frontWall;
        westWall = rightWall;
        break;
    case (3):
        southWall = leftWall;
        westWall = frontWall;
        northWall = rightWall;
        break;
    default:
        break;
  }
    transmission |= northWall << 3;
    transmission |= southWall << 2;
    transmission |= eastWall << 1;
    transmission |= westWall;


    
    //send over Radio and wait for a response
    radio.stopListening();
    
    bool ok = radio.write( &transmission, sizeof(unsigned long) );
    if (ok) Serial.print("ok...");
    else Serial.println("failed");

    //-------------------------------------------------------------------------------------------------------------------------
    //response waiting loop, may change it / remove it if it hampers performance too much or line following gets thrown off
    radio.startListening();
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout ) {
      if (millis() - started_waiting_at > 200 ) timeout = true;

        //--------------------------------------------
        if ( middleSensor < 800){
        //Serial.println("going straight");
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 800){
          //TURN LEFT
          //Serial.println("turning left onto line");
          leftSpeed = 85;
          rightSpeed = 85;
        }
        if (rightSensor < 800){
          //TURN RIGHT
         // Serial.println("turning right onto line")
          leftSpeed = 95;
          rightSpeed = 95;
        }
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      //-----------------------------------------------
    }
    // Describe the results
    if ( timeout )
    {
      Serial.println("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      Serial.print("Got response");
      Serial.println(got_time, BIN);
    }
    //--------------------------------------------------------------------------------------------------------------------
    
    //lastly, update the position and orientation accordingly
    Serial.print("orient ");
    Serial.print(orient);
    orient = nextOrient;
    Serial.print(" nextOrient ");
    Serial.print(orient);
    Serial.print(" posX ");
    Serial.print(posX);
    Serial.print(" posY ");
    Serial.print(posY);
    posY = nextPosY;
    posX = nextPosX;
    Serial.print(" nextPosX ");
    Serial.print(posX);
    Serial.print(" nextPosY ");
    Serial.print(posY);
    Serial.println();
    
        
    }
    //straight line following
    else if ( middleSensor < 800){
        //Serial.println("going straight");
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 800){
          //TURN LEFT
          //Serial.println("turning left onto line");
          leftSpeed = 85;
          rightSpeed = 85;
        }
        if (rightSensor < 800){
          //TURN RIGHT
         // Serial.println("turning right onto line")
          leftSpeed = 95;
          rightSpeed = 95;
        }
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
        //role = role_ping_out;
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
}
