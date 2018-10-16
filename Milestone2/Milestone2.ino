#include <Servo.h>
#define LOG_OUT 1 // use the log output function
//#define FFT_N 256 // set to 256 point fft LEGACY
#define FFT_N 128 // CHANGED 128 pt FFT

#include <FFT.h> // include the library

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
int lw = 6;
int fw = 7;
int rw = 8;

int rightLED = 12;
int frontLED = 2;
int leftLED =  3;
int robotLED = 4;
boolean IRDetected;
int defaultT = TIMSK0;
int defaultADC = ADCSRA;
int defaultADMUX = ADMUX;
int defaultD = DIDR0;

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
    //for (int i = 0 ; i < 512 ; i += 2) {  save 256 samples LEGACY
    
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

void goStraight() {
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
  delay(200);
  leftSpeed = 95;
  rightSpeed = 95;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(700);
}

void turnAround() {
 leftSpeed = 95;
  rightSpeed = 95;
  left.write(95);
  right.write(85);
  delay(200);
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(1400); 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // use the serial port
  Serial.println("starting");
  left.attach(10);
  right.attach(5);
  left.write(90);
  right.write(90);
  
  //wall sensors
  pinMode(lw, INPUT);
  pinMode(fw, INPUT);
  pinMode(rw, INPUT);

  //output LEDs
  pinMode(rightLED, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(frontLED, OUTPUT);
  pinMode(robotLED, OUTPUT);
  Serial.println("finished setup");
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
//void loop(){
//  checkWallSensors();
//}
//
////void loop(){
////  runFFT();
////}
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
        runFFT();

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
        digitalWrite(leftLED, LOW);
        digitalWrite(rightLED, LOW);
        digitalWrite(frontLED, LOW);
        digitalWrite(robotLED, LOW);
        if (IRDetected){  
          Serial.println("IR detected");       
          seen |= B1000;
          digitalWrite(robotLED, HIGH);
        }
        //left wall
        if (digitalRead(lw)){  
          Serial.println("left wall detected");  
          seen |= B0100;
          digitalWrite(leftLED, HIGH);
        }
        //front wall
        if (digitalRead(fw)){  
          Serial.println("front wall detected");  
          seen |= B0010;
          digitalWrite(frontLED, HIGH);
        }
        //right wall
        if (digitalRead(rw)){
          Serial.println("right wall detected");     
          seen |= B0001;
          digitalWrite(rightLED, HIGH);
        }
        
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
    //straight line following
    }else if ( middleSensor < 800){
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
         // Serial.println("turning right onto line");
          leftSpeed = 95;
          rightSpeed = 95;
        }
        left.write(leftSpeed);
        right.write(rightSpeed);
      }

}
