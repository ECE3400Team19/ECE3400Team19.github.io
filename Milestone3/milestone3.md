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
  n = frontier.pop()
  visit n- meaning move our robot to n
  transmit the details of node n over radio
  figure out what places we can visit based on what the robot can see and its orientation
  add these places to the frontier stack
  return current position of robot and its orientation
}

```
