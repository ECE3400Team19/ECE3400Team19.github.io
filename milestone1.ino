#include <Servo.h>


//int pot = A0;
Servo left; //180 turns wheel forward
Servo right; //180 turns wheel backward
int line[3];
int line2 = A5; //left
int line1 = A1; //center
int line0 = A0; //right
int leftSpeed;
int rightSpeed;


void setup() {
  Serial.begin(9600);
  left.attach(5);
  right.attach(10);
  pinMode(line2, INPUT);
  
}

void loop() {

    line[2] = analogRead(line2);
    line[1] = analogRead(line1);
    line[0] = analogRead(line0);

//      if(line[2]>300){
//        rightSpeed = 45;
//        Serial.println("LEFT");
//        //Serial.println(line[2] + " " + line[1] + " " + line[0]);
//        leftSpeed =  ;
//      }
//      else if(line[0]>300){
//        leftSpeed =;
//        rightSpeed = ;
//        Serial.println("RIGHT");
//        //Serial.println(line[2] + " " + line[1] + " " + line[0]);
//      }
//      else if (line[0]<300 && line[2]<300){
//        Serial.println("STRAIGHT");
//        //Serial.println(line[2] + " " + line[1] + " " + line[0]);
//        leftSpeed = 100;
//        rightSpeed = 80;
//      }
      if (line[1] < 300){
        leftSpeed = 101;
        rightSpeed= 89;
      }
      else if( 
        
      left.write(leftSpeed);
      right.write(rightSpeed);
 
     Serial.print("Left: ");
     Serial.print(analogRead(line2));
     Serial.print(" Middle: ");
     Serial.print(line[1]);
     Serial.print(" Right: ");
     Serial.print(line[0]);
     Serial.println();
     delay(50);



}
