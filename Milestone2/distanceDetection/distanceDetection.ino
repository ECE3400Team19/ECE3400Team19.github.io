#include <Servo.h>

Servo left; //180 turns wheel backward
Servo right; //180 turns wheel forward

int leftPin = A0; //left
int middlePin = A5; //center
int rightPin = A1; //right
int IR = A3;
int leftSensor;
int rightSensor;
int middleSensor;
int leftSpeed;
int rightSpeed;
float dis=0;
boolean detected = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  left.attach(10);
  right.attach(5);
  float sensorValue = analogRead(A2); // Read sensor value for analog pin A2
  float dis = 2076/(sensorValue - 11); // convert to a distance
  while (1){
    float sensorValue = analogRead(A2); // Read sensor value for analog pin A2
    float dis = 2076/(sensorValue - 11); // convert to a distance
    if (dis < 20) break;
    Serial.println("looking for wall");
    //delay(300);
    Serial.println(dis);
    left.write(95);
    right.write(90);
    delay(100);
  }
  
}

void loop() {

    float sensorValue = analogRead(A2); // Read sensor value for analog pin A2
    float dis = 2076/(sensorValue - 11); // convert to a distance

    //read line sensors
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);    

    
    //At Intersection, need to decide where to go
    if (leftSensor < 800  && middleSensor< 800 && rightSensor < 800){
        //FFT IR
        //code here
    
        //left wall
        //code here
    
        //front wall
        //code here
    
        //right wall
        //code here
    
        //What can you see?
        int seen = 0'b0000
        if (IR detected)  seen |= 0'b1000;
        if (leftWall)     seen |= 0'b0100;
        if (frontWall)    seen |= 0'b0010;
        if (rightWall)    seen |= 0'b0001;
        
        switch (seen) {
          case 0'b0110:
            //wall in front and left, turn right
            turnRight()
            break;
          case 0'b0011:
            //wall in front and right, turn left
            turnLeft();
            break;
          case 0'b0111:
            //wall in front, right and left, turn around
            turnAround();
            break;
          case 0'b1100:
            //wall on left, robot in front, turn right
            turnRight()
            break;
          case 0'b1001:
            //wall on right, robot in front, turn left
            turnLeft();
            break;
          case 0'b1101:
            //wall on right and left, robot in front, turn around
            turnAround();
            break;
          //weird cases
          case 0'b1110:
            //wall on left and front, robot in front?, turn right
            turnRight()
            break;
          case 0'b1011:
            //wall on right and front, robot in front?, turn left
            turnLeft();
            break;
          case 0'b1111:
            //wall on right and left and front, robot in front?, turn around
            turnAround();
            break;
          default:
            //go straight
            goStraight()
            break;
    }
    //straight line following
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

    
    if (dis < 4 || dis >30) {
        Serial.print( "Out of Range.\n");
    }
    else {
        Serial.print(dis);
        Serial.print(" cm\n"); 
        
    }
    if (dis < 10){
      //wall is to the left, go forward
       left.write(95);
       right.write(85);
    }
    else{
        //turn left
      left.write(85);
      right.write(85);
      delay(400);
    }
    delay(500);
}

void goStraight() {
  leftSpeed = 85;
  rightSpeed =85;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(400);
}

void turnLeft() {
  
}

void turnRight() {
  
}

void turnAround() {
  
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
