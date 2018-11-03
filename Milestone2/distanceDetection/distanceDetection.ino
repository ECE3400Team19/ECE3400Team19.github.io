#include <Servo.h>
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

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
boolean IRDetected;

//helper methods
void runFFT(){
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
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
    Serial.println("start");
    for (byte i = 0 ; i < FFT_N/2 ; i++) { 
      Serial.println(fft_log_out[i]); // send out the data
    }
}

void goStraight() {
  leftSpeed = 85;
  rightSpeed =85;
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(400);
}

void turnLeft() {
  leftSpeed = 85;
  rightSpeed = 85;
  left.write(95);
  right.write(85);
  delay(200);
  left.write(leftSpeed);
  right.write(rightSpeed);
  delay(700);
}

void turnRight() {
  leftSpeed = 95;
  rightSpeed = 95;
  left.write(95);
  right.write(85);
  delay(200);
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
  left.attach(10);
  right.attach(5);
  pinMode(lw, INPUT);
  pinMode(fw, INPUT);
  pinMode(rw, INPUT);
  //fft
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x43; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void loop() {

    //read line sensors
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);    
    Serial.println("new loop");
    Serial.println(leftSensor);
    Serial.println(middleSensor);
    Serial.println(rightSensor);
    //At Intersection, need to decide where to go
    if (leftSensor < 800  && middleSensor< 800 && rightSensor < 800){
        Serial.println("at intersection");
        //FFT IR
        ADMUX = 0x43;
        runFFT();
        for (int i = 38; i < 48; i++){
          if (fft_log_out[i] > 100){
            IRDetected = 1;
            break;
          }
        }
        
        //What can you see?
        int seen = B0000;
        digitalWrite(4, LOW);
        digitalWrite(3, LOW);
        digitalWrite(2, LOW);
        digitalWrite(1, LOW);
        if (IRDetected){         
          seen |= B1000;
          digitalWrite(4, HIGH);
        }
        //left wall
        if (digitalRead(lw)){    
          seen |= B0100;
          digitalWrite(3, HIGH);
        }
        //front wall
        if (digitalRead(fw)){    
          seen |= B0010;
          digitalWrite(2, HIGH);
        }
        //right wall
        if (digitalRead(rw)){    
          seen |= B0001;
          digitalWrite(1, HIGH);
        }
        
        switch (seen) {
          case B0110:
            //wall in front and left, turn right
            turnRight();
            break;
          case B0011:
            //wall in front and right, turn left
            turnLeft();
            break;
          case B0111:
            //wall in front, right and left, turn around
            turnAround();
            break;
          case B1100:
            //wall on left, robot in front, turn right
            turnRight();
            break;
          case B1001:
            //wall on right, robot in front, turn left
            turnLeft();
            break;
          case B1101:
            //wall on right and left, robot in front, turn around
            turnAround();
            break;
          //weird cases
          case B1110:
            //wall on left and front, robot in front?, turn right
            turnRight();
            break;
          case B1011:
            //wall on right and front, robot in front?, turn left
            turnLeft();
            break;
          case B1111:
            //wall on right and left and front, robot in front?, turn around
            turnAround();
            break;
          default:
            //go straight
            goStraight();
            break;
    }
    //straight line following
    }else if ( middleSensor < 800){
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

}
