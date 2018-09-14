#include <Servo.h>

Servo left; //180 turns wheel backward
Servo right; //180 turns wheel forward

int leftPin = A0; //left
int middlePin = A5; //center
int rightPin = A1; //right
int leftSensor;
int rightSensor;
int middleSensor;
int leftSpeed;
int rightSpeed;
int numIntersections = 0;


void setup() {
  Serial.begin(9600);
  left.attach(10);
  right.attach(5);

//  //turn left
//  leftSpeed = 95;
//  rightSpeed = 95;
//  left.write(95);
//  right.write(95);

//  //turn right
//  leftSpeed = 85;
//  rightSpeed = 85;
//  left.write(85);
//  right.write(85);

// //go forward
// left.write(95)
// right.write(85)
  


}

void loop() {
  //followLine();
  figureEight();



  
  
//    leftSensor = analogRead(leftPin);
//    middleSensor = analogRead(middlePin);
//    rightSensor = analogRead(rightPin);
//     Serial.print("Left: ");
//     Serial.print(leftSensor);
//     Serial.print(" Middle: ");
//     Serial.print(middleSensor);
//     Serial.print(" Right: ");
//     Serial.print(rightSensor);
//     Serial.println();

}

void followLine(){
  
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);


      if (leftSensor < 800  && middleSensor< 800 && rightSensor < 800){
        // reaches the INTERSECTION
        leftSpeed = 85;
        rightSpeed =85;
        left.write(leftSpeed);
        right.write(rightSpeed);
        delay(400);
//        while( leftSensor > 800);
      }
      else if ( middleSensor < 800){
        //go straight!!
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 800){
          //TURN LEFT
          leftSpeed = 85;
          rightSpeed = 85;
        }
        if (rightSensor < 800){
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
    
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);


      if (leftSensor < 800 && middleSensor< 800 && rightSensor < 800){
        // reaches the INTERSECTION
        if (numIntersections == 8) {numIntersections = 0;}
        if (numIntersections>3){
            //turn left
            Serial.println(numIntersections);
            //++numIntersections;
            leftSpeed = 85;
            rightSpeed = 85;
        }
        else{
          //turn right
          Serial.println(numIntersections);
          //++numIntersections;
          leftSpeed = 95;
          rightSpeed = 95;
        }
        ++numIntersections;
        left.write(95);
        right.write(85);
        delay(200);
        left.write(leftSpeed);
        right.write(rightSpeed);
        delay(700);
        //while( leftSensor > 800);
      }
      else if ( middleSensor < 800){
        //go straight!!
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 800){
          //TURN LEFT
          leftSpeed = 85;
          rightSpeed = 85;
        }
        if (rightSensor < 800){
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

