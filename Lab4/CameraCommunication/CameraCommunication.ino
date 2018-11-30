int incomingByte;
int colorPin =1;
int shapePin1 = 8;
int shapePin0 = 7;
int redLED = 11; //debug
int blueLED = 12; //debug
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("start");
  pinMode(shapePin1, INPUT);
  pinMode(shapePin0, INPUT);
  pinMode(colorPin, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
}

//  KEY: MSB = color; bits[0:1] = shape
//  000 = nothing detected
//  001 = red triangle
//  010 = red square
//  011 = red diamond
//  100 = nothing detected
//  101 = blue triangle
//  110 = blue square
//  111 = blue diamond

void loop() {
    Serial.print("state: ");
    Serial.print(digitalRead(6));
    Serial.print(digitalRead(5));
    Serial.print(digitalRead(4));
    Serial.print(digitalRead(3));
    Serial.println(digitalRead(2));

    Serial.print("colorpin: ");
    Serial.println(digitalRead(colorPin));

    Serial.print("shape pins: ");
    Serial.print(digitalRead(shapePin1));
    Serial.println(digitalRead(shapePin0));
    
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
    Serial.println(" square");
  }
  if (digitalRead(shapePin1) && digitalRead(shapePin0) ) {
    Serial.println(" diamond");
  }

  
}







