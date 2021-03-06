# Milestone 3: Maze Exploration
[Home](https://ece3400team19.github.io/)

## Milestone Description
The goal of this milestone was to get our robot to explore the maze using DFS, while correctly updating the GUI. 

## Algorithm
pseudocode:
```
StackArray<byte> frontier
if at an intersection{
 checkForWallsandIR()
 runDFS(current position, what it sees, its orientation)
}
else not at an intersection{
  lineFollow()
}
runDFS{
  n = frontier.pop() //(goal state)
  visit n- meaning move our robot to n
    if n is within one square of where currently are, then we just move to that square
    while n is farther away than one square{
       we move back to the parent of our current node
    }
  transmit the details of node n over radio
  figure out what places we can visit based on what the robot can see and its orientation
  add these places to the frontier stack
  return current position of robot and its orientation
}

```
[actual code](https://github.com/ECE3400Team19/ECE3400Team19.github.io/blob/master/Milestone3/Final3400/Final3400.ino)

Here is a video of our robot navigating a 4x5 maze!
<iframe width="560" height="315" src="https://www.youtube.com/embed/m5x3R3dMZAY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

Our robot is able to navigate different maze configurations!!
<iframe width="560" height="315" src="https://www.youtube.com/embed/qX5fpouX1v8" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

Our robot navigated the two mazes above as expected, using DFS for navigation and backtracking. We had difficultly transmitting to the GUI, so even when our robot navigated the maze properly, the incorrect walls or positions would show up on the GUI. 

Our robot starts on a 660Hz tone and recognizes an IR hat, waits for the IR hat to move, then resumes exploring the maze:
<iframe width="560" height="315" src="https://www.youtube.com/embed/8MXpFRq8EAk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


