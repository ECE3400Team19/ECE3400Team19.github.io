#include <StackArray.h>


//  StackArray <byte> frontier; 
//  frontier.push(0);
byte n;
byte currentPos;
//declared these as externs here bc they're declared in final3400
extern unsigned int posX;
extern unsigned int nextPosX;
extern unsigned int posY;
extern unsigned int nextPosY;
extern byte nextOrient; 
extern byte orient;
// end

unsigned long runDFS(int posX, int posY, int seen, int orient, int robotDetected){
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
  Serial.println("----- entering DFS-----");
  left.write(90); //stop the robot 
  right.write(90);
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
  inFrontier[n] = 0;
  Serial.print("fpos: ");
  Serial.println(n);
//  Serial.print("northWall ");
//  Serial.print(northWall);
//  Serial.print("  eastWall ");
//  Serial.print(eastWall);
//  Serial.print("  southWall ");
//  Serial.print(southWall);
//  Serial.print("  westWall ");
//  Serial.print(westWall);
  
  if (n != 0){
    // move robot to position n
    currentPos = encodePos(posX, posY);
    unsigned long orientPosition;
    while (currentPos != n){
      Serial.print("currpos: ");
      Serial.println(currentPos);
      if (backpointer[n] == currentPos){
        orientPosition = moveToNode(currentPos, n, orient, seen);
        orient = orientPosition >> 8;
        currentPos = orientPosition & B11111111;
      }
      else{
        orientPosition = moveToNode(currentPos, backpointer[currentPos], orient, seen);
        orient = orientPosition >> 8;
        currentPos = orientPosition & B11111111;        
      }
    }
  }
  seen = checkWalls();
  leftWall = seen >> 2 & B01;
  frontWall = seen >> 1 & B001;
  rightWall = seen & B001;
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
  visited[n] = 1;
  //Serial.println("adding to frontier");
  switch (orient){
    case (0):
      //facing North
      Serial.println("facing N");
      
      if (!(westWall || visited[n-1] == 1 || inFrontier[n-1] )){
        Serial.print("add pos: ");
        Serial.println(n - mazeWidth);
        frontier.push(n - 1);
        backpointer[n-1] = n;
        inFrontier[n-1] = 1;
      }
      if (!(eastWall|| visited[n+ 1] == 1 || inFrontier[n+1] )){
        Serial.print("add pos: ");
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        inFrontier[n+1] = 1;
      }
      if (!(northWall || robotDetected || visited[n-mazeWidth] == 1 || inFrontier[n - mazeWidth])){
        Serial.print("add pos: ");
        Serial.println(n - mazeWidth);
        frontier.push(n-mazeWidth);
        backpointer[n-mazeWidth]=n;
        inFrontier[n - mazeWidth] = 1;
      }
      break;
    case (1):
      //facing East
      Serial.println("facing E");
      if (!(northWall || visited[n - mazeWidth] == 1 || inFrontier[n - mazeWidth])){
        Serial.print("add pos : ");
        Serial.println(n - mazeWidth);
        frontier.push(n-mazeWidth); 
        backpointer[n-mazeWidth]=n;
        inFrontier[n - mazeWidth] = 1;
      }
      if (!(southWall || visited[n + mazeWidth] == 1 || inFrontier[n + mazeWidth] )){
        Serial.print("add pos : ");
        Serial.println(n + mazeWidth);
        frontier.push(n + mazeWidth);
        backpointer[n+mazeWidth]=n;
        inFrontier[n + mazeWidth] = 1;
      }
      if (!(eastWall || robotDetected || visited[n + 1] == 1 || inFrontier[n + 1] )){
        Serial.print("add pos : ");
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        inFrontier[n +1] = 1;
      }
      break;

    case (2):
      //facing South
      Serial.println("facing S");
      if (!(westWall || visited[n - 1] == 1 || inFrontier[n - 1] )){
        Serial.print("add pos: ");
        Serial.println(n -1);        
        frontier.push(n -1);
        backpointer[n - 1]=n;
        inFrontier[n - 1] =1;
      }
      if (!(eastWall || visited[n + 1] == 1 || inFrontier[n + 1])){
        Serial.print("add pos: ");
        Serial.println(n + 1);
        frontier.push(n + 1);
        backpointer[n+1]=n;
        inFrontier[n + 1] = 1;
      }

      if (!(southWall || robotDetected || visited[n+mazeWidth] == 1 || inFrontier[n + mazeWidth])){
        Serial.print("add pos: ");
        Serial.println(n + mazeWidth);        
        frontier.push(n+mazeWidth);
        backpointer[n+mazeWidth]=n;
        inFrontier[n + mazeWidth] = 1;
      }
      break;
      case (3):
      //facing West
      Serial.println("facing W");
      if (!(southWall || visited[n + mazeWidth] == 1 || inFrontier[n + mazeWidth])){
        Serial.print("add pos : ");
        Serial.println(n + mazeWidth);        
        frontier.push(n + mazeWidth); 
        backpointer[n+mazeWidth]=n;
        inFrontier[n + mazeWidth] = 1;
      }
      if (!(northWall || visited[n - mazeWidth] == 1 || inFrontier[n - mazeWidth])){
        Serial.print("add pos : ");
        Serial.println(n - mazeWidth);        
        frontier.push(n-mazeWidth);
        backpointer[n-mazeWidth]=n;
        inFrontier[n - mazeWidth] = 1;
      }
      if (!(westWall || robotDetected || visited[n - 1] == 1 || inFrontier[n - 1])){
        Serial.print("add pos: ");
        Serial.println(n -1);        
        frontier.push(n -1);
        backpointer[n-1]=n;
        inFrontier[n - 1] = 1;
      }
      break;
    default:
      break;
    
  } 
    unsigned long value = currentPos;
    value |= orient << 8; 
    //Serial.println("exiting DFS");
    left.write(90); //stop the robot 
    right.write(90);
    return value; 
    
}

byte encodePos(int posX, int posY){
  //change from an (X, Y) position to a byte between 0 and 80
  return posX + posY*mazeWidth;
}


unsigned long moveToNode(byte curr, byte goal, int orient, int seen){
  posX = curr % mazeWidth;
  posY = curr/mazeWidth;
  switch (orient){
    case(0):
      //facing North
      Serial.println("-> node, facing N");
      if (goal - curr == 1){
        //goal is to the East
        turnRight();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnLeft();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        goStraight();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnAround();
      }
      break;
    case (1):
      //facing East
      Serial.println("-> node, facing E");
      if (goal - curr == 1){
        //goal is to the East
        goStraight();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnAround();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnLeft();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        turnRight();
      }
      break; 
    case (2):
      Serial.println("-> node, facing S");
      //facing South
      if (goal - curr == 1){
        //goal is to the East
        turnLeft();
      }
      if (curr - goal == 1){
        //goal is to the West
        turnRight();
      }
      if (curr - goal == mazeWidth){
        //goal is to the SOuth
        turnAround();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
        goStraight();
      }
      break;   
    case (3):
      //facing West
      Serial.println("-> node, facing W");
      if (goal - curr == 1){
        //goal is to the East
        turnAround();
      }
      if (curr - goal == 1){
        //goal is to the West
        goStraight();
      }
      if (curr - goal == mazeWidth){
        //goal is to the South
        turnRight();
      }
      if (goal - curr == mazeWidth){
        //goal is to the North
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
    //if (ok) Serial.print("ok...");
    //else Serial.println("failed");

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
      //Serial.println("Failed, response timed out.\n\r");
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
    Serial.print(" posX,Y ");
    Serial.print(posX);
    Serial.print(",");
    Serial.print(posY);
    posY = nextPosY;
    posX = nextPosX;
    Serial.print(" nextPosX,Y ");
    Serial.print(posX);
    Serial.print(",");
    Serial.println(posY);

    // move to an intersection
    moveToIntersection();  
    unsigned long value = encodePos(posX, posY);
    value |= orient << 8; 
    return value; 
}

void moveToIntersection(){
  while (1){
    //Serial.println("trying to move to intersection");
    //Serial.println("mti");
    leftSensor = analogRead(leftPin);
    middleSensor = analogRead(middlePin);
    rightSensor = analogRead(rightPin);
    if (leftSensor < 850  && middleSensor < 800 && rightSensor < 800){
      
  //    runFFT(); //true indicates run FFT for IR
  //    //check if IR sensor detects another robot based on FFT
  //    IRDetected = 0;
  //    for (int i = 15; i < 25; i++){ //changed
  //      if (fft_log_out[i] > 125){
  //        //Serial.println(fft_log_out[i]);
  //        IRDetected = 1;
  //        break;
  //      }
  //    }  
  
    //TODO: figure out what to do if robot is in path
      return;
    }
    else if ( middleSensor < 800){
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
  }
}

int checkWalls(){
    int seen = B0000;
    //digitalWrite(leftLED, LOW);
    //digitalWrite(rightLED, LOW);
    //digitalWrite(frontLED, LOW);
    //digitalWrite(robotLED, LOW);
    if (IRDetected){  
      //Serial.println("IR detected");       
      seen |= B1000;
    }
    if (digitalRead(lw)){  
      Serial.println("left wall detected");  
      seen |= B0100;
    }
    if (digitalRead(fw)){  
      Serial.println("front wall detected");  
      seen |= B0010;
    }
    if (digitalRead(rw)){
      Serial.println("right wall detected");     
      seen |= B0001;
    }
    return seen;
}






