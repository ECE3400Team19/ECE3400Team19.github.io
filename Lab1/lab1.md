# Lab 1

[Home](https://ece3400team19.github.io/)


## Description

In this lab, we used the various functionalities of the Arduino Uno and the Arduino IDE, as well as the GitHub repository.
We brainstormed and constructed a simple functional Arduino program using multiple external components and the Arduino Uno.
We then put together our robot and had it perform a [simple autonomous task](#robot-demo).

## Materials

* 1 Arduino Uno (in the box)
* 1 USB A/B cable (in the box)
* 1 Continuous rotation servos
* 1 Pushbutton
* 1 LED (any color except IR!)
* 1 Potentiometer
* Several resistors (kΩ range)
* 1 Solderless breadboard

## Part 1 - Modifying the Blink sketch

```
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
```

Video of LED blinking.

<video width="800" height="600" controls>
  <source src="blinking.MOV" type="video/mp4">
</video>

Photo of the Arduino setup with external LED

<img src="arduino.JPG" width="800" height="600" alt="arduino!!!">

Video of external LED blinking

<video width="800" height="600" controls>
  <source src="externalLED.MOV" type="video/mp4">
</video>

## Part 2 - Using the analog output


Video of potentiometer-controlled LED output

<video width="800" height="600" controls>
  <source src="externalLEDPOT.MOV" type="video/mp4">
</video>


## Part 3 - Using the Parallax servos

Video of Servo control

<video width="800" height="600" controls>
  <source src="servo.MOV" type="video/mp4">
</video>

Video of potentiometer-controlled Servo control

<video width="800" height="600" controls>
  <source src="servoPot.MOV" type="video/mp4">
</video>


## Part 4 - Assembling the robot!

## Robot demo!!!

what a good boi
<video width="800" height="600" controls>
  <source src="robot.mov" type="video/mp4">
</video>

