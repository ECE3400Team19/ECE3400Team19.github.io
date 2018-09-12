#include <Servo.h>


//int pot = A0;
Servo left; //180 turns wheel forward
Servo right; //180 turns wheel backward
//int line[3];
int leftPin = A5; //left
int middlePin = A1; //center
int rightPin = A0; //right
int leftSensor;
int rightSensor;
int middleSensor;
int leftSpeed;
int rightSpeed;


void setup() {
  Serial.begin(9600);
  left.attach(5);
  right.attach(10);
  //pinMode(line2, INPUT);
  
}

void loop() {
  followLine();
}

void followLine(){
  
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);


      if (leftSensor < 300 && middleSensor< 300 && rightSensor < 300){
        // reaches the INTERSECTION
        leftSpeed = 85;
        rightSpeed =85;
        left.write(leftSpeed);
        right.write(rightSpeed);
        delay(400);
//        while( leftSensor > 300);
      }
      else if ( middleSensor < 300){
        //go straight!!
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 300){
          //TURN LEFT
          leftSpeed = 85;
          rightSpeed = 85;
        }
        if (rightSensor < 300){
          //TURN RIGHT
          leftSpeed = 95;
          rightSpeed = 95;
        }
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      
     
     //writing 100 to both turns it right
     Serial.print("Left: ");
     Serial.print(leftSensor);
     Serial.print(" Middle: ");
     Serial.print(middleSensor);
     Serial.print(" Right: ");
     Serial.print(rightSensor);
     Serial.println();
}

void figureEight(){
    int numIntersections;
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);


      if (leftSensor < 300 && middleSensor< 300 && rightSensor < 300){
        // reaches the INTERSECTION
        numIntersections++;
        if (numIntersections>4){
            leftSpeed = 85;
            rightSpeed = 85;
        }
        else{
          leftSpeed = 95;
          rightSpeed = 95;
        }
        left.write(leftSpeed);
        right.write(rightSpeed);
        delay(400);
        //while( leftSensor > 300);
      }
      else if ( middleSensor < 300){
        //go straight!!
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 300){
          //TURN LEFT
          leftSpeed = 85;
          rightSpeed = 85;
        }
        if (rightSensor < 300){
          //TURN RIGHT
          leftSpeed = 95;
          rightSpeed = 95;
        }
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      
     
     //writing 100 to both turns it right
     Serial.print("Left: ");
     Serial.print(leftSensor);
     Serial.print(" Middle: ");
     Serial.print(middleSensor);
     Serial.print(" Right: ");
     Serial.print(rightSensor);
     Serial.println();
  
}

