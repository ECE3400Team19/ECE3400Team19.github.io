# Milestone 1: Wall Following
[Home](https://ece3400team19.github.io/)

## Team Tasks
  * Asena, Cynthia, Laasya:
  * Nonso, Robert:

## Milestone Description
The goal of this milestone was to get our robot to successfully circle an arbitrary set of walls, avoid other robots, and follow lines.

## Milestone Materials
[3 wall sensors](https://www.sparkfun.com/products/12728) (GP2Y0A41SK0F)

## Part 1 - Hooking up the Wall Sensors
We connected the line sensor's three pins to ground, power, and an analog input (A2).
The line sensor outputs a voltage
Using the following code, we were able to detect how far away the sensor was from a wall.

void setup() {
    Serial.begin(9600);
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
    delay(500);
}
Here is a video of the code in action. Laasya moves her hand farther away from the sensor until she is out of range, and then moves back into range.
<video width="640" height="360" controls muted>
  <source src="IMG_6671.MOV" type="video/mp4">
</video>

The Arduino only has 6 analog inputs. We were already using three analog inputs for the three line sensors we implemented in Milestone 1, and two analog inputs for the microphone detection and IR detection.
This only left us one analog input. We decided to change the an

## Part 2 - Avoiding Walls


## Part 3 - Avoiding Walls while Staying on the Line


## Part 3 - Avoiding Walls while Staying on the Line and Avoiding Robots 
