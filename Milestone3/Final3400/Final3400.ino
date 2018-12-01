#include <Servo.h>
#define LOG_OUT 1 // use the log output function
#define FFT_N 128 // CHANGED 128 pt FFT
#include <FFT.h> // include the library
//include Radio Headers and Library
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <StackArray.h>

Servo left; //180 turns wheel backward
Servo right; //180 turns wheel forward

RF24 radio(9, 10);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x000000003ELL, 0x000000003FLL };

//struct Square maze[81];
unsigned long transmission;
unsigned int posX, nextPosX, posY, nextPosY;
byte nextOrient = 2; 
byte orient = 2;

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
int pushButton = 7;
boolean IRDetected;
int defaultT = TIMSK0;
int defaultADC = ADCSRA;
int defaultADMUX = ADMUX;
int defaultD = DIDR0;


boolean visited[81];
boolean inFrontier[81];
byte backpointer[81];
StackArray <byte> frontier;
int mazeWidth = 4;
// MICROPHONE is in A2
// IR is in A3

//helper methods
void runFFT() {
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
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
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

void runMICFFT() {
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
  //Serial.println("going straight lol");
//  nextOrient = orient;
//  leftSpeed = 95;
//  rightSpeed = 85;
//  left.write(leftSpeed);
//  right.write(rightSpeed);
//  delay(100);
}

void turnLeft() {
  //go straight for 200 ms and then turn left
  //Serial.println("turning left");
  left.write(95);
  right.write(85);
  Serial.print("orient in turnLeft : ");
  Serial.println(orient);
  if (orient == 0) nextOrient = 3;
  else nextOrient = orient - 1;
  Serial.print("nextOrient in turnLeft : ");
  Serial.println(nextOrient);
  delay(200);
  leftSpeed = 85;
  rightSpeed = 85;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(800);
}

void turnRight() {
  //go straight for 200 ms and then turn right
  Serial.println("turning right");
  left.write(95);
  right.write(85);
  nextOrient = orient + 1;
  if (nextOrient == 4) nextOrient = 0;
  delay(200);
  leftSpeed = 95;
  rightSpeed = 95;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(800);
}

void turnAround() {
  //straight for 200 ms and then turn around
  Serial.println("turning around");
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
  pinMode(pushButton, INPUT);

  //radio setup
  radio.begin();
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  radio.startListening();

  Serial.println("finished setup");
  waitForMic();

  //DFS stack setup
  frontier.push(0);

}

void checkWallSensors() {
  Serial.print("left wall : ");
  Serial.print(digitalRead(lw));
  Serial.print(" front wall : ");
  Serial.print(digitalRead(fw));
  Serial.print(" right wall : ");
  Serial.print(digitalRead(rw));
  Serial.println();
  delay(50);
}

void checkLineSensors(){
  leftSensor = analogRead(leftPin);
  middleSensor = analogRead(middlePin);
  rightSensor = analogRead(rightPin);
  Serial.print("left  : ");
  Serial.print(leftSensor);
  Serial.print(" middle  : ");
  Serial.print(middleSensor);
  Serial.print(" right  : ");
  Serial.print(rightSensor);
  Serial.println();
}

void waitForMic() {
  while (1) {
    
    int buttonState = digitalRead(pushButton);
    Serial.print("buttonState ");
    Serial.println(buttonState);
    if (buttonState == HIGH) break;
//    runMICFFT();
//    Serial.println("waiting for 660 tone");
//    Serial.println(fft_log_out[10]);
//    if (fft_log_out[10] > 60) {
//      //Serial.println(fft_log_out[10]);
//      Serial.println("660 Hz signal detected");
//      break;
//    }
    delay(50);
  }
}

void loop() {
  //read line sensors
  leftSensor = analogRead(leftPin);
  middleSensor = analogRead(middlePin);
  rightSensor = analogRead(rightPin);


  //At Intersection, need to decide where to go
  if (leftSensor < 850  && middleSensor < 800 && rightSensor < 800) {
    //FFT IR
    Serial.println("at an intersection");
    runFFT(); //true indicates run FFT for IR

    //check if IR sensor detects another robot based on FFT

    //for (int i = 38; i < 48; i++){  //LEGACY : 256 bit fft
    IRDetected = 0;
    for (int i = 15; i < 25; i++) { //changed
      if (fft_log_out[i] > 125) {
        //Serial.println(fft_log_out[i]);
        IRDetected = 1;
        break;
      }
    }

    //What can you see?
    int seen = B0000;
    if (IRDetected) {
      Serial.println("IR detected");
      seen |= B1000;
    }
    //left wall
    if (digitalRead(lw)) {
      Serial.println("left wall detected");
      seen |= B0100;
    }
    //front wall
    if (digitalRead(fw)) {
      Serial.println("front wall detected");
      seen |= B0010;
    }
    //right wall
    if (digitalRead(rw)) {
      Serial.println("right wall detected");
      seen |= B0001;
    }

    unsigned long orientPosition;
    orientPosition = runDFS(posX, posY, seen, orient, IRDetected);
    orient = orientPosition >> 8;
    byte currentPos = orientPosition & B11111111;
    posX = currentPos % mazeWidth;
    posY = currentPos / mazeWidth;
    Serial.print("after DFS, orient: ");
    Serial.print(orient);
    Serial.print(" pos X,Y ");
    Serial.print(posX);
    Serial.print(",");
    Serial.println(posY);
    //Update the map in Storage based on current orientation
    //updateMap(seen);
    
  }
  //straight line following
  else if ( middleSensor < 800) {
    //Serial.println("going straight");
    leftSpeed = 95;
    rightSpeed = 85;
    left.write(leftSpeed);
    right.write(rightSpeed);
  }
  else {
    if (leftSensor < 800) {
      //TURN LEFT
      //Serial.println("turning left onto line");
      leftSpeed = 85;
      rightSpeed = 85;
    }
    if (rightSensor < 800) {
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
  radio.openReadingPipe(1, pipes[1]);
}

