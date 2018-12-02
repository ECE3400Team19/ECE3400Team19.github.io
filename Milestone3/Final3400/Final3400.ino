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
unsigned long transmission;
unsigned int posX, nextPosX, posY, nextPosY;
byte nextOrient = 2; 
byte orient = 2;
int leftPin = A0;
int middlePin = A5; 
int rightPin = A1; 
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
unsigned long orientPosition;

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
  //Serial.print("orient in turnLeft : ");
  //Serial.println(orient);
  if (orient == 0) nextOrient = 3;
  else nextOrient = orient - 1;
  //Serial.print("nextOrient in turnLeft : ");
  //Serial.println(nextOrient);
  delay(200);
  leftSpeed = 85;
  rightSpeed = 85;
  left.write(leftSpeed);
  right.write(rightSpeed);
  //delay(800);
  delay(600);
}

void turnRight() {
  //go straight for 200 ms and then turn right
  Serial.println("turning right");
  left.write(95);
  right.write(85);
  //Serial.print("orient in turnRight : ");
  //Serial.println(orient);
  nextOrient = orient + 1;
  if (nextOrient == 4) nextOrient = 0;
  //Serial.print("nextOrient in turnRight : ");
  //Serial.println(nextOrient);
  delay(200);
  leftSpeed = 95;
  rightSpeed = 95;
  left.write(leftSpeed);
  right.write(rightSpeed);
  //delay(800);
  delay(600);
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
  //delay(1800);
  delay(1600);
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

  Serial.println("finished setup");
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
    if (leftSensor < 850) {
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
#include <StackArray.h>
byte n;
byte currentPos;

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
  Serial.println("-----entering DFS-----");
  left.write(90); //stop the robot 
  right.write(90);
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
  n = frontier.pop();
  inFrontier[n] = 0;
  Serial.print("fpos: ");
  Serial.println(n);
  
  if (n != 0){
    // move robot to position n
    currentPos = encodePos(posX, posY);
    unsigned long orientPosition;
    while (currentPos != n){
      Serial.print("currpos: ");
      Serial.println(currentPos);
      if (backpointer[n] == currentPos){
        orientPosition = moveToNode(currentPos, n, orient, seen);
        orient = orientPosition >> 8;
        currentPos = orientPosition & B11111111;
      }
      else{
        orientPosition = moveToNode(currentPos, backpointer[currentPos], orient, seen);
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
  Serial.print("n ");
  Serial.println(northWall);
  Serial.print("e ");
  Serial.println(eastWall);
  Serial.print("s ");
  Serial.println(southWall);
  Serial.print("w ");
  Serial.println(westWall);
  
  visited[n] = 1;
  //Serial.println("adding to frontier");
  switch (orient){
    case (0):
      //facing North
      Serial.println("facing N");
       
      if (!(westWall || visited[n-1] == 1 || inFrontier[n-1] )){
        Serial.print("add pos: ");
        Serial.println(n - mazeWidth);
        frontier.push(n - 1);
        backpointer[n-1] = n;
        inFrontier[n-1] = 1;
      }
      if (!(eastWall|| visited[n+ 1] == 1 || inFrontier[n+1] )){
        Serial.print("add pos: ");
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        inFrontier[n+1] = 1;
      }
      if (!(northWall || robotDetected || visited[n-mazeWidth] == 1 || inFrontier[n - mazeWidth])){
        Serial.print("add pos: ");
        Serial.println(n - mazeWidth);
        frontier.push(n-mazeWidth);
        backpointer[n-mazeWidth]=n;
        inFrontier[n - mazeWidth] = 1;
      }
      break;
    case (1):
      //facing East
      Serial.println("facing E");
      if (!(northWall || visited[n - mazeWidth] == 1 || inFrontier[n - mazeWidth])){
        Serial.print("add pos : ");
        Serial.println(n - mazeWidth);
        frontier.push(n-mazeWidth); 
        backpointer[n-mazeWidth]=n;
        inFrontier[n - mazeWidth] = 1;
      }
      if (!(southWall || visited[n + mazeWidth] == 1 || inFrontier[n + mazeWidth] )){
        Serial.print("add pos : ");
        Serial.println(n + mazeWidth);
        frontier.push(n + mazeWidth);
        backpointer[n+mazeWidth]=n;
        inFrontier[n + mazeWidth] = 1;
      }
      if (!(eastWall || robotDetected || visited[n + 1] == 1 || inFrontier[n + 1] )){
        Serial.print("add pos : ");
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        inFrontier[n +1] = 1;
      }
      break;

    case (2):
      //facing South
      Serial.println("facing S");
      if (!(westWall || visited[n - 1] == 1 || inFrontier[n - 1] )){
        Serial.print("add pos: ");
        Serial.println(n -1);        
        frontier.push(n -1);
        backpointer[n - 1]=n;
        inFrontier[n - 1] =1;
      }
      if (!(eastWall || visited[n + 1] == 1 || inFrontier[n + 1])){
        Serial.print("add pos: ");
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        inFrontier[n + 1] = 1;
      }

      if (!(southWall || robotDetected || visited[n+mazeWidth] == 1 || inFrontier[n + mazeWidth])){
        Serial.print("add pos: ");
        Serial.println(n + mazeWidth);        
        frontier.push(n+mazeWidth);
        backpointer[n+mazeWidth]=n;
        inFrontier[n + mazeWidth] = 1;
      }
      break;
      case (3):
      //facing West
      Serial.println("facing W");
      if (!(southWall || visited[n + mazeWidth] == 1 || inFrontier[n + mazeWidth])){
        Serial.print("add pos : ");
        Serial.println(n + mazeWidth);        
        frontier.push(n + mazeWidth); 
        backpointer[n+mazeWidth]=n;
        inFrontier[n + mazeWidth] = 1;
      }
      if (!(northWall || visited[n - mazeWidth] == 1 || inFrontier[n - mazeWidth])){
        Serial.print("add pos : ");
        Serial.println(n - mazeWidth);        
        frontier.push(n-mazeWidth);
        backpointer[n-mazeWidth]=n;
        inFrontier[n - mazeWidth] = 1;
      }
      if (!(westWall || robotDetected || visited[n - 1] == 1 || inFrontier[n - 1])){
        Serial.print("add pos: ");
        Serial.println(n -1);        
        frontier.push(n -1);
        backpointer[n-1]=n;
        inFrontier[n - 1] = 1;
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


unsigned long moveToNode(byte curr, byte goal, int orient, int seen){
  posX = curr % mazeWidth;
  posY = curr/mazeWidth;
  switch (orient){
    case(0):
      //facing North
      Serial.println("-> node, facing N");
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
      Serial.println("-> node, facing E");
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
      Serial.println("-> node, facing S");
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
        //goal is to the SOuth
        turnAround();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        goStraight();
      }
      break;   
    case (3):
      //facing West
      Serial.println("-> node, facing W");
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
        //nextPosY= posY+1;
        break;
      case (1):
        //nextPosX= posX+1;
        nextPosX= posX-1;
        break;
      case (2):
        nextPosY= posY+1;
        //nextPosY= posY-1;
        break;
      case (3):
        //nextPosX= posX-1;
        nextPosX= posX+1;
        break;
      default:
        break;
  }
    
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
        if ( middleSensor < 800){
        //Serial.println("going straight");
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 850){
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
    //Serial.println("trying to move to intersection");
    //Serial.println("mti");
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
    else if ( middleSensor < 800){
      //Serial.println("going straight");
      leftSpeed = 95;
      rightSpeed = 85;
      left.write(leftSpeed);
      right.write(rightSpeed);
    }
    else {
      if (leftSensor < 850){
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
  }
}

int checkWalls(){
    int seen = B0000;
    //digitalWrite(leftLED, LOW);
    //digitalWrite(rightLED, LOW);
    //digitalWrite(frontLED, LOW);
    //digitalWrite(robotLED, LOW);
    if (IRDetected){  
      //Serial.println("IR detected");       
      seen |= B1000;
    }
    if (digitalRead(lw)){  
      Serial.println("left wall detected");  
      seen |= B0100;
    }
    if (digitalRead(fw)){  
      Serial.println("front wall detected");  
      seen |= B0010;
    }
    if (digitalRead(rw)){
      Serial.println("right wall detected");     
      seen |= B0001;
    }
    return seen;
}





