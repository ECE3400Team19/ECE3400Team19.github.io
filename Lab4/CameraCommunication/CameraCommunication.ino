int incomingByte;
int ackPin = 9;
int colorPin =10;
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

void loop() {
  // put your main code here, to run repeatedly:
//  if (Serial.available()){
//    incomingByte = Serial.read();
//    // say what you got:
//    Serial.print("I received: ");
//    Serial.println(incomingByte, DEC); }

    Serial.println(digitalRead(ackPin));
    
    if (digitalRead(ackPin)){
      if (digitalRead(colorPin)) digitalWrite(blueLED, HIGH);
      else digitalWrite(redLED, HIGH);
      delay(100);
      digitalWrite(redLED, LOW);
      digitalWrite(blueLED, LOW);
    }


  
}
