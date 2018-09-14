# Milestone 1: Following a Line
[Home](https://ece3400team19.github.io/)

## Team Tasks

## Milestone Description
The goal of this milestone was to get our robot to follow a line and traverse a grid in a figure eight

## Milestone Materials
3 line sensors (QRE1113)

## Part 1 - Hooking up the Line Sensors

## Part 2 - Following a Line

Once we completed the setup for the Line Sensors, we developed an algorithm to best help the robot find the white line, stay on it, and turn right or left at the perpendicular intersections. The pseudo-code for this is as follows:

Loop:
  If the middle line sensor detects a white line:
    the robot should go straight

  If the middle sensor does not detect the white line, but the right sensor does:
    the robot should turn clockwise until the middle line sensor returns to the line

  If the middle sensor does not detect the white line, but the left sensor does:
    the robot should turn counterclockwise until the middle line sensor returns to the line

  If all 3 line sensors detect a white line:
    the robot acknowledges that it's at an intersection
    the robot will either turn left or right (to be determined by the code implementer)

  Else:
    the robot should go straight until the middle sensor detects a line

This allowed the robot to find and remain on a line, even if it's starting position was not perfectly parallel with any of the white lines. The Arduino implementation of this code is provided below:

```
```


## Part 3 - Moving in a Figure Eight
