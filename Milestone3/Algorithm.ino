#include <StackArray.h>


//  StackArray <byte> frontier; 
//  frontier.push(0);
byte n;
int mazeWidth = 4;


int[] runDFS(int posX, int posY, int northWall, int southWall, int eastWall, int westWall, int orient, int robotDetected){
  // seen = B1111, robot, left, front, right
  // orient = 0 -> North, orient = 1 -> East, orient = 2 -> South, orient = 3 -> West
  
  // append (0, 0) to frontier, frontier.push(0)
  // while frontier is not empty
  //      n = frontier.pop()
  //      set n to be visited
  //      figure out which places we can visit based on walls and robot
  //      add these places to frontier if they have not been visited
  //    
  n = frontier.pop();
  if (n != 0){
    // move robot to position n
    
  }
  visited[n] = 1;
  switch (orient){
    case 0:
      //facing North
      if !(southWall || visited[n- mazeWidth] == 0) frontier.push(n - mazeWidth); //when switching to 81, (n = 9)
      if !(westWall || visited[n-1] == 0) frontier.push(n - 1);
      if !(eastWall|| visited[n+ 1] == 0) frontier.push(n + 1);
      if !(northWall || robotDetected || visited[n+mazeWidth] == 0) frontier.push(n+mazeWidth); //switch to n + 9
      break;
    case 1:
      //facing East
      if !(westWall || visited[n - 1] == 0) frontier.push(n -1);
      if !(northWall || visited[n + mazeWidth] == 0) frontier.push(n+mazeWidth); //switch to n + 9
      if !(southWall || visited[n - mazeWidth] == 0) frontier.push(n - mazeWidth); //when switching to 81, (n = 9)
      if !(eastWall || robotDetected || visited[n + 1] == 0) frontier.push(n + 1);
      break;
    case 2:
      //facing West
      if !(eastWall || visited[n + 1] == 0) frontier.push(n + 1);
      if !(southWall || visited[n - mazeWidth] == 0) frontier.push(n - mazeWidth); //when switching to 81, (n = 9)
      if !(northWall || visited[n + mazeWidth] == 0) frontier.push(n+mazeWidth); //switch to n + 9
      if !(westWall || robotDetected || visited[n - 1] == 0) frontier.push(n -1);
      break;
    case 3:
      //facing South
      if !(northWall || visited[n + mazeWidth] == 0) frontier.push(n+mazeWidth); //switch to n + 9
      if !(eastWall || visited[n + 1] == 0) frontier.push(n + 1);
      if !(westWall || visited[n - 1] == 0) frontier.push(n -1);
      if !(southWall || robotDetected || visited[n - mazeWidth] == 0) frontier.push(n - mazeWidth); //when switching to 81, (n = 9)
      break;
    default:
      break;
    
  }
    
  
  
    
}



