int incomingByte;
int colorPin =8;
int shapePin1 = 9;
int shapePin0 = 10;
int redLED = 11;
int blueLED = 12;
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
        digitalWrite(blueLED, HIGH);
        Serial.print("blue"); 
        printShape();
      }
      else {
        digitalWrite(redLED, HIGH);
        Serial.print("red"); 
        printShape();
        
      }
      delay(100);
      digitalWrite(redLED, LOW);
      digitalWrite(blueLED, LOW);
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







