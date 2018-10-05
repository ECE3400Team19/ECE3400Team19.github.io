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
