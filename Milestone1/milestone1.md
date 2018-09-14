# Milestone 1: Following a Line
[Home](https://ece3400team19.github.io/)

## Team Tasks

## Milestone Description
The goal of this milestone was to get our robot to follow a line and traverse a grid in a figure eight

## Milestone Materials
3 line sensors (QRE1113)

## Part 1 - Hooking up the Line Sensors

There were two tasks included in milestone 1; following a line and doing a figure eight.  Implicit in the second is the ability to turn either left or right.  We used three line sensors to start.  They were mounted on straight vertical pieces attached to the main chassis such that the bottom of the pieces were as close to the ground as possible without dragging them along the floor.  This proximity was necessary to get the most contrast we could between white and black.  

At first we had all three sensors facing parallel to the line side-by-side to use multiple low outputs (multiple sensors 'on the line') to aid in following down a straight line.  However, we later settled on a method using a single sensor low to do line following, using the other two sensors only for course correction and detecting intersections.  To go along with this alternate implementation, we flared out the two sensors on the sides, aligning them away from the central sensor such that the distance between them was roughly equal to the with of the line for easy course correction.  


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
