int incomingByte;
int colorPin =1;
int shapePin1 = 8;
int shapePin0 = 7;
int redLED = 11; //debug
int blueLED = 12; //debug
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("start");
  pinMode(ackPin, INPUT);
  pinMode(colorPin, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
}

//  KEY: MSB = color; bits[0:1] = shape
//  000 = nothing detected
//  001 = red triangle
//  010 = red circle
//  011 = red diamond
//  100 = nothing detected
//  101 = red triangle
//  110 = red circle
//  111 = red diamond

void loop() {
    
    if (digitalRead(shapePin1) || digitalRead(shapePin0)  ){
      if (digitalRead(colorPin)) {
        digitalWrite(blueLED, HIGH); //debug
        Serial.print("blue"); 
        printShape();
      }
      else {
        digitalWrite(redLED, HIGH); //debug
        Serial.print("red"); 
        printShape();
        
      }
      delay(100);
      digitalWrite(redLED, LOW); //debug
      digitalWrite(blueLED, LOW); //debug
    }
    
}

void printShape() {
  if (!digitalRead(shapePin1) && digitalRead(shapePin0) ) {
    Serial.println(" triangle");
  }
  if (digitalRead(shapePin1) && !digitalRead(shapePin0) ) {
    Serial.println(" circle");
  }
  if (digitalRead(shapePin1) && digitalRead(shapePin0) ) {
    Serial.println(" diamond");
  }

  
}







