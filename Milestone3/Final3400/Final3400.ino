#include <Servo.h>
#define LOG_OUT 1 
#define FFT_N 128
#include <FFT.h> 
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <StackArray.h>
Servo left; 
Servo right; 
RF24 radio(9, 10);
const uint64_t pipes[2] = { 0x000000003ELL, 0x000000003FLL };
unsigned int posX, nextPosX, posY, nextPosY;
byte nextOrient = 2; 
byte orient = 2;
int leftPin = A0;
int middlePin = A5; 
int rightPin = A1; 
int IR = A3;
int leftSensor, rightSensor, middleSensor;
int lw = 3;
int fw = 4;
int rw = 2;
int pushButton = A4;
boolean IRDetected;
boolean visited[81];
//boolean inFrontier[81];
byte backpointer[81];
StackArray <byte> frontier;
int mazeWidth = 9;


void runFFT() {
  int defaultT = TIMSK0;
  int defaultADC = ADCSRA;
  int defaultADMUX = ADMUX;
  int defaultD = DIDR0;
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
    nextOrient = orient;
//  leftSpeed = 95;
//  rightSpeed = 85;
//  left.write(leftSpeed);
//  right.write(rightSpeed);
//  delay(100);
}

void turnLeft() {
  //go straight for 200 ms and then turn left
  Serial.println(F("turning left"));
  left.write(95);
  right.write(85);
  //Serial.print("orient in turnLeft : ");
  //Serial.println(orient);
  if (orient == 0) nextOrient = 3;
  else nextOrient = orient - 1;
  //Serial.print("nextOrient in turnLeft : ");
  //Serial.println(nextOrient);
  delay(200);
  left.write(85);
  right.write(85);
  //delay(800);
  delay(590);
}

void turnRight() {
  //go straight for 200 ms and then turn right
  Serial.println(F("turning right"));
  left.write(95);
  right.write(85);
  Serial.print("orient in turnRight : ");
  Serial.println(orient);
  nextOrient = orient + 1;
  if (nextOrient == 4) nextOrient = 0;
  Serial.print("nextOrient in turnRight : ");
  Serial.println(nextOrient);
  delay(200);
  left.write(95);
  right.write(95);
  //delay(800);
  delay(750);
}

void turnAround() {
  //straight for 200 ms and then turn around
  Serial.println(F("turning around"));
  nextOrient = orient + 2;
  if (nextOrient == 4) nextOrient = 0;
  if (nextOrient == 5) nextOrient = 1;
  left.write(95);
  right.write(85);
  delay(200);
  left.write(95);
  right.write(95);
  delay(2000);
  
}

void setup() {
  Serial.begin(9600);
  left.attach(6);
  right.attach(5);
  left.write(90);
  right.write(90);
  pinMode(lw, INPUT);
  pinMode(fw, INPUT);
  pinMode(rw, INPUT);
  pinMode(pushButton, INPUT); 

  radio.begin();
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  radio.startListening();

  Serial.println(F("finished setup"));
  waitForMic();

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
  leftSensor = analogRead(leftPin);
  middleSensor = analogRead(middlePin);
  rightSensor = analogRead(rightPin);

  if (leftSensor < 850  && middleSensor < 800 && rightSensor < 800) {
    Serial.println(F("at an intersection"));
    runFFT();
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
      Serial.println(F("IR detected"));
      seen |= B1000;
    }
    if (digitalRead(lw)) {
      Serial.println(F("lw detected"));
      seen |= B0100;
    }
    if (digitalRead(fw)) {
      Serial.println(F("fw detected"));
      seen |= B0010;
    }
    if (digitalRead(rw)) {
      Serial.println(F("rw detected"));
      seen |= B0001;
    }
    unsigned long orientPosition = runDFS(posX, posY, seen, orient, IRDetected);
    orient = orientPosition >> 8;
    byte currentPos = orientPosition & B11111111;
    posX = currentPos % mazeWidth;
    posY = currentPos / mazeWidth;
    Serial.print(F("after DFS, orient: "));
    Serial.print(orient);
    Serial.print(F(" pos X,Y "));
    Serial.print(posX);
    Serial.print(F(","));
    Serial.println(posY);
    
  }
  //straight line following
  else if ( middleSensor < 800) {
                left.write(105);
                right.write(75);
  }
  else {
    if (leftSensor < 850) {
      //TURN LEFT
      left.write(85);
      right.write(85);
    }
    if (rightSensor < 800) {
      //TURN RIGHT
      left.write(95);
      right.write(95);
    }
  }
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
}
#include <StackArray.h>



unsigned long runDFS(int posX, int posY, int seen, int orient, int robotDetected){
  // seen = B1111, robot, left, front, right
  // orient = 0 -> North, orient = 1 -> East, orient = 2 -> South, orient = 3 -> West
  
  // append (0, 0) to frontier
  // while frontier is not empty
  //      n = frontier.pop()
  //      visit n using backpointer
  //      set n to be visited
  //      figure out which places we can visit based on walls and robot
  //      add these places to frontier if they have not been visited
  //  
  Serial.println(F("-----entering DFS-----"));
  left.write(90); //stop the robot 
  right.write(90);
  delay(150);

  byte leftWall = seen >> 2 & B01;
  byte frontWall = seen >> 1 & B001;
  byte rightWall = seen & B001;
  byte northWall;
  byte southWall;
  byte eastWall;
  byte westWall;
    
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
  byte n = frontier.pop();
  if (visited[n]){
    unsigned long value = encodePos(posX, posY);
    value |= orient << 8;
    return value;
  }
  //inFrontier[n] = 0;
  Serial.print(F("fpos: "));
  Serial.println(n);
  byte currentPos;
  if (n != 0){
    // move robot to position n
    currentPos = encodePos(posX, posY);
    unsigned long orientPosition;
    while (currentPos != n){
      Serial.print(F("currpos: "));
      Serial.println(currentPos);
      if (backpointer[n] == currentPos){
        orientPosition = moveToNode(currentPos, n, orient, seen, 1); //1 indicates we should transmit over radio
        orient = orientPosition >> 8;
        currentPos = orientPosition & B11111111;
      }
      else{
        orientPosition = moveToNode(currentPos, backpointer[currentPos], orient, seen, 0); //0 indicates do not transmit over radio
        orient = orientPosition >> 8;
        currentPos = orientPosition & B11111111;        
      }
    }
  }
  seen = checkWalls();
  leftWall = seen >> 2 & B01;
  frontWall = seen >> 1 & B001;
  rightWall = seen & B001;
  switch (orient){
    case (0):
        westWall = leftWall;
        northWall = frontWall;
        eastWall = rightWall;
        southWall = 0;
        break;
    case (1):
        northWall = leftWall;
        eastWall = frontWall;
        southWall = rightWall;
        westWall = 0;
        break;
    case (2):
        eastWall = leftWall;
        southWall = frontWall;
        westWall = rightWall;
        northWall = 0;
        break;
    case (3):
        southWall = leftWall;
        westWall = frontWall;
        northWall = rightWall;
        eastWall = 0;
        break;
    default:
        break;
  }
  Serial.print(F("n "));
  Serial.println(northWall);
  Serial.print(F("e "));
  Serial.println(eastWall);
  Serial.print(F("s "));
  Serial.println(southWall);
  Serial.print(F("w "));
  Serial.println(westWall);
  
  visited[n] = 1;
  switch (orient){
    case (0):
      //facing North
      Serial.println(F("facing N"));
       
      if (!(westWall || visited[n-1] == 1)){
        Serial.print(F("add pos: "));
        Serial.println(n - 1);
        frontier.push(n - 1);
        backpointer[n-1] = n;
      }
      if (!(eastWall|| visited[n+ 1] == 1)){
        Serial.print(F("add pos: "));
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        
      }
      if (!(northWall || robotDetected || visited[n-mazeWidth] == 1)){
        Serial.print(F("add pos: "));
        Serial.println(n - mazeWidth);
        frontier.push(n-mazeWidth);
        backpointer[n-mazeWidth]=n;
        
      }
      break;
    case (1):
      //facing East
      Serial.println(F("facing E"));
      if (!(northWall || visited[n - mazeWidth] == 1 )){
        Serial.print(F("add pos: "));
        Serial.println(n - mazeWidth);
        frontier.push(n-mazeWidth); 
        backpointer[n-mazeWidth]=n;
        
      }
      if (!(southWall || visited[n + mazeWidth] == 1  )){
        Serial.print(F("add pos: "));
        Serial.println(n + mazeWidth);
        frontier.push(n + mazeWidth);
        backpointer[n+mazeWidth]=n;
        
      }
      if (!(eastWall || robotDetected || visited[n + 1] == 1  )){
        Serial.print(F("add pos: "));
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        
      }
      break;

    case (2):
      //facing South
      Serial.println(F("facing S"));
      if (!(westWall || visited[n - 1] == 1  )){
        Serial.print(F("add pos: "));
        Serial.println(n -1);        
        frontier.push(n -1);
        backpointer[n - 1]=n;
       
      }
      if (!(eastWall || visited[n + 1] == 1 )){
        Serial.print(F("add pos: "));
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
       
      }

      if (!(southWall || robotDetected || visited[n+mazeWidth] == 1 )){
        Serial.print(F("add pos: "));
        Serial.println(n + mazeWidth);        
        frontier.push(n+mazeWidth);
        backpointer[n+mazeWidth]=n;
        
      }
      break;
      case (3):
      //facing West
      Serial.println(F("facing W"));
      if (!(southWall || visited[n + mazeWidth] == 1 )){
        Serial.print(F("add pos: "));
        Serial.println(n + mazeWidth);        
        frontier.push(n + mazeWidth); 
        backpointer[n+mazeWidth]=n;
        ;
      }
      if (!(northWall || visited[n - mazeWidth] == 1 )){
        Serial.print(F("add pos: "));
        Serial.println(n - mazeWidth);        
        frontier.push(n-mazeWidth);
        backpointer[n-mazeWidth]=n;
        
      }
      if (!(westWall || robotDetected || visited[n - 1] == 1 )){
        Serial.print(F("add pos: "));
        Serial.println(n -1);        
        frontier.push(n -1);
        backpointer[n-1]=n;
     
      }
      break;
    default:
      break;
    
  } 
    unsigned long value = currentPos;
    value |= orient << 8; 
    left.write(90); //stop the robot 
    right.write(90);
    return value; 
    
}

byte encodePos(int posX, int posY){
  //change from an (X, Y) position to a byte between 0 and 80
  return posX + posY*mazeWidth;
}


unsigned long moveToNode(byte curr, byte goal, int o, int seen, bool transmit){
  posX = curr % mazeWidth;
  posY = curr/mazeWidth;
  orient = o;
  switch (orient){
    case(0):
      //facing North
      Serial.println(F("-> node, facing N"));
      if (goal - curr == 1){
        //goal is to the East
        turnRight();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnLeft();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        goStraight();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnAround();
      }
      break;
    case (1):
      //facing East
      Serial.println(F("-> node, facing E"));
      if (goal - curr == 1){
        //goal is to the East
        goStraight();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnAround();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnLeft();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnRight();
      }
      break; 
    case (2):
      Serial.println(F("-> node, facing S"));
      //facing South
      if (goal - curr == 1){
        //goal is to the East
        turnLeft();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnRight();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnAround();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        goStraight();
      }
      break;   
    case (3):
      //facing West
      Serial.println(F("-> node, facing W"));
      if (goal - curr == 1){
        //goal is to the East
        turnAround();
      }
      if (curr - goal == 1){
        //goal is to the West
        goStraight();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnRight();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnLeft();
      }
      break; 
  }

  //figure out where we moved
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

  
  if (transmit){  
            unsigned long transmission = 0;
            unsigned int yCoor = 0;
            if (posY > 0){
               yCoor = posY - 1;
            }
            transmission |= posY << 12;
            transmission |= posX << 8;
            //the rest is treasure stuff we haven't gotten to yet
            transmission |= IRDetected << 4;
            byte leftWall = seen >> 2 & B01;
            byte frontWall = seen >> 1 & B001;
            byte rightWall = seen & B001;
            byte northWall;
            byte southWall;
            byte eastWall;
            byte westWall;
            
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
            //if (ok) Serial.print("ok...");
            //else Serial.println("failed");
        
            //-------------------------------------------------------------------------------------------------------------------------
            //response waiting loop, may change it / remove it if it hampers performance too much or line following gets thrown off
            radio.startListening();
            unsigned long started_waiting_at = millis();
            bool timeout = false;
            while ( ! radio.available() && ! timeout ) {
              if (millis() - started_waiting_at > 200 ) timeout = true;
        
                //--------------------------------------------
             if ( middleSensor < 800) {
                //Serial.println("going straight");
                left.write(105);
                right.write(75);
            }
             else {
                if (leftSensor < 850) {
                  //TURN LEFT
                  //Serial.println("turning left onto line");
                  left.write(85);
                  right.write(85);
                }
                if (rightSensor < 800) {
                  //TURN RIGHT
                  // Serial.println("turning right onto line")
                  left.write(95);
                  right.write(95);
                }
              }
              //-----------------------------------------------
            }
            // Describe the results
            if ( timeout )
            {
              //Serial.println("Failed, response timed out.\n\r");
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
    }
    else{
      delay(100);
    }
    //--------------------------------------------------------------------------------------------------------------------
    
    //lastly, update the position and orientation accordingly
    Serial.print("orient ");
    Serial.print(orient);
    orient = nextOrient;
    Serial.print(" nextOrient ");
    Serial.print(orient);
    Serial.print(" posX,Y ");
    Serial.print(posX);
    Serial.print(",");
    Serial.print(posY);
    posY = nextPosY;
    posX = nextPosX;
    Serial.print(" nextPosX,Y ");
    Serial.print(posX);
    Serial.print(",");
    Serial.println(posY);

    // move to an intersection
    moveToIntersection();  
    unsigned long value = encodePos(posX, posY);
    value |= orient << 8; 
    return value; 
}

void moveToIntersection(){
  while (1){
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);
    if (leftSensor < 850  && middleSensor < 800 && rightSensor < 800){
      
  //    runFFT(); //true indicates run FFT for IR
  //    //check if IR sensor detects another robot based on FFT
  //    IRDetected = 0;
  //    for (int i = 15; i < 25; i++){ //changed
  //      if (fft_log_out[i] > 125){
  //        //Serial.println(fft_log_out[i]);
  //        IRDetected = 1;
  //        break;
  //      }
  //    }  
  
    //TODO: figure out what to do if robot is in path
      return;
    }
  else if ( middleSensor < 800) {
    //Serial.println("going straight");
                left.write(105);
                right.write(75);
  }
  else {
    if (leftSensor < 850) {
      //TURN LEFT
      //Serial.println("turning left onto line");
      left.write(85);
      right.write(85);
    }
    if (rightSensor < 800) {
      //TURN RIGHT
      // Serial.println("turning right onto line")
      left.write(95);
      right.write(95);
    }
  }
  }
}

int checkWalls(){
    int seen = B0000;
    if (IRDetected){  
      //Serial.println("IR detected");       
      seen |= B1000;
    }
    if (digitalRead(lw)){  
      Serial.println(F("lw detected"));  
      seen |= B0100;
    }
    if (digitalRead(fw)){  
      Serial.println(F("fw detected"));  
      seen |= B0010;
    }
    if (digitalRead(rw)){
      Serial.println(F("rw detected"));     
      seen |= B0001;
    }
    return seen;
}






