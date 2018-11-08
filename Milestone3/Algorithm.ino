#include <StackArray.h>


//  StackArray <byte> frontier; 
//  frontier.push(0);
byte n;
int mazeWidth = 4;
byte currentPos;

void runDFS(int posX, int posY, int seen, int orient, int robotDetected){
  // seen = B1111, robot, left, front, right
  // orient = 0 -> North, orient = 1 -> East, orient = 2 -> South, orient = 3 -> West
  
  // append (0, 0) to frontier
  // while frontier is not empty
  //      n = frontier.pop()
  //      visit n using backpointer
  //      set n to be visited
  //      figure out which places we can visit based on walls and robot
  //      add these places to frontier if they have not been visited
  //  

    unsigned int leftWall = seen >> 2 & B01;
    unsigned int frontWall = seen >> 1 & B001;
    unsigned int rightWall = seen & B001;
    unsigned int northWall;
    unsigned int southWall;
    unsigned int eastWall;
    unsigned int westWall;
    
  switch (orient){
    case (0):
        westWall = leftWall;
        northWall = frontWall;
        eastWall = rightWall;
        break;
    case (1):
        northWall = leftWall;
        eastWall = frontWall;
        southWall = rightWall;
        break;
    case (2):
        eastWall = leftWall;
        southWall = frontWall;
        westWall = rightWall;
        break;
    case (3):
        southWall = leftWall;
        westWall = frontWall;
        northWall = rightWall;
        break;
    default:
        break;
  }
  n = frontier.pop();
  if (n != 0){
    // move robot to position n
    while (currentPos != n){
      currentPos = encodePos(posX, posY);
      if (backpointer[n] == currentPos){
        moveToNode(currentPos, n, orient, seen);
      }
      else{
        moveToNode(currentPos, backpointer[currentPos], orient, seen);
      }
    }
  }
  visited[n] = 1;
  switch (orient){
    case (0):
      //facing North
      if (!(southWall || visited[n- mazeWidth] == 0)){
        frontier.push(n - mazeWidth);
        backpointer[n - mazeWidth] = n;
      }
      if (!(westWall || visited[n-1] == 0)){
        frontier.push(n - 1);
        backpointer[n-1] = n;
      }
      if (!(eastWall|| visited[n+ 1] == 0)){
        frontier.push(n + 1);
        backpointer[n+1]=n;
      }
      if (!(northWall || robotDetected || visited[n+mazeWidth] == 0)){
        frontier.push(n+mazeWidth);
        backpointer[n+mazeWidth]=n;
      }
      break;
    case (1):
      //facing East
      if (!(westWall || visited[n - 1] == 0)){
        frontier.push(n -1);
        backpointer[n-1]=n;
      }
      if (!(northWall || visited[n + mazeWidth] == 0)){
        frontier.push(n+mazeWidth); 
        backpointer[n+mazeWidth]=n;
      }
      if (!(southWall || visited[n - mazeWidth] == 0)){
        frontier.push(n - mazeWidth);
        backpointer[n-mazeWidth]=n;
      }
      if (!(eastWall || robotDetected || visited[n + 1] == 0)){
        frontier.push(n + 1);
        backpointer[n+1]=n;
      }
      break;

    case (2):
      //facing South
      if (!(northWall || visited[n + mazeWidth] == 0)){
        frontier.push(n+mazeWidth);
        backpointer[n+mazeWidth]=n;
      }
      if (!(eastWall || visited[n + 1] == 0)){
        frontier.push(n + 1);
        backpointer[n+1]=n;
      }
      if (!(westWall || visited[n - 1] == 0)){
        frontier.push(n -1);
        backpointer[n - 1]=n;
      }
      if (!(southWall || robotDetected || visited[n-mazeWidth] == 0)){
        frontier.push(n-mazeWidth);
        backpointer[n-mazeWidth]=n;
      }
      break;
      case (3):
      //facing West
      if (!(eastWall || visited[n + 1] == 0)){
        frontier.push(n + 1);
        backpointer[n+1]=n;
      }
      if (!(southWall || visited[n - mazeWidth] == 0)){
        frontier.push(n - mazeWidth); 
        backpointer[n-mazeWidth]=n;
      }
      if (!(northWall || visited[n + mazeWidth] == 0)){
        frontier.push(n+mazeWidth);
        backpointer[n+mazeWidth]=n;
      }
      if (!(westWall || robotDetected || visited[n - 1] == 0)){
        frontier.push(n -1);
        backpointer[n-1]=n;
      }
      break;
    default:
      break;
    
  } 
    
}

byte encodePos(int posX, int posY){
  //change from an (X, Y) position to a byte between 0 and 80
  return posX + posY*mazeWidth;
}

void moveToNode(byte curr, byte goal, int orient, int seen){
  switch (orient){
    case(0):
      //facing North
      if (goal - curr == 1){
        //goal is to the East
        turnRight();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnLeft();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        goStraight();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnAround();
      }
      break;
    case (1):
      //facing East
      if (goal - curr == 1){
        //goal is to the East
        goStraight();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnAround();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnLeft();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnRight();
      }
      break; 
    case (2):
      //facing South
      if (goal - curr == 1){
        //goal is to the East
        turnLeft();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnRight();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnAround();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        goStraight();
      }
      break;   
    case (3):
      //facing West
      if (goal - curr == 1){
        //goal is to the East
        turnAround();
      }
      if (curr - goal == 1){
        //goal is to the West
        goStraight();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnRight();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnLeft();
      }
      break; 
  }

  //figure out where we moved
   switch (nextOrient) {
      case (0):
        nextPosY= posY-1;
        break;
      case (1):
        nextPosX= posX+1;
        break;
      case (2):
        nextPosY= posY+1;
        break;
      case (3):
        nextPosX= posX-1;
        break;
      default:
        break;
  }

    transmission = 0;
    transmission |= posY << 12;
    transmission |= posX << 8;
    //the rest is treasure stuff we haven't gotten to yet
    transmission |= IRDetected << 4;
    unsigned int leftWall = seen >> 2 & B01;
    unsigned int frontWall = seen >> 1 & B001;
    unsigned int rightWall = seen & B001;
    unsigned int northWall;
    unsigned int southWall;
    unsigned int eastWall;
    unsigned int westWall;
    
  switch (orient){
    case (0):
        westWall = leftWall;
        northWall = frontWall;
        eastWall = rightWall;
        break;
    case (1):
        northWall = leftWall;
        eastWall = frontWall;
        southWall = rightWall;
        break;
    case (2):
        eastWall = leftWall;
        southWall = frontWall;
        westWall = rightWall;
        break;
    case (3):
        southWall = leftWall;
        westWall = frontWall;
        northWall = rightWall;
        break;
    default:
        break;
  }
    transmission |= northWall << 3;
    transmission |= southWall << 2;
    transmission |= eastWall << 1;
    transmission |= westWall;


    
    //send over Radio and wait for a response
    radio.stopListening();
    
    bool ok = radio.write( &transmission, sizeof(unsigned long) );
    if (ok) Serial.print("ok...");
    else Serial.println("failed");

    //-------------------------------------------------------------------------------------------------------------------------
    //response waiting loop, may change it / remove it if it hampers performance too much or line following gets thrown off
    radio.startListening();
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout ) {
      if (millis() - started_waiting_at > 200 ) timeout = true;

        //--------------------------------------------
        if ( middleSensor < 800){
        //Serial.println("going straight");
        leftSpeed = 95;
        rightSpeed = 85;
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      else {
        if (leftSensor < 800){
          //TURN LEFT
          //Serial.println("turning left onto line");
          leftSpeed = 85;
          rightSpeed = 85;
        }
        if (rightSensor < 800){
          //TURN RIGHT
         // Serial.println("turning right onto line")
          leftSpeed = 95;
          rightSpeed = 95;
        }
        left.write(leftSpeed);
        right.write(rightSpeed);
      }
      //-----------------------------------------------
    }
    // Describe the results
    if ( timeout )
    {
      Serial.println("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      Serial.print("Got response");
      Serial.println(got_time, BIN);
    }
    //--------------------------------------------------------------------------------------------------------------------
    
    //lastly, update the position and orientation accordingly
    Serial.print("orient ");
    Serial.print(orient);
    orient = nextOrient;
    Serial.print(" nextOrient ");
    Serial.print(orient);
    Serial.print(" posX ");
    Serial.print(posX);
    Serial.print(" posY ");
    Serial.print(posY);
    posY = nextPosY;
    posX = nextPosX;
    Serial.print(" nextPosX ");
    Serial.print(posX);
    Serial.print(" nextPosY ");
    Serial.print(posY);
    Serial.println();
    
        
}


