# Lab 3: System Integration and Radio Communication
[Home](https://ece3400team19.github.io/)

## Team Tasks
* Radio Team: Cynthia, Robert, Laasya
* Integration Team: Asena, Nonso

## Lab Description
In this lab, we integrated the work from previous labs and milestones. We integrated the microphone circuit into the robot so that the robot could start on a 660 Hz tone. We then had out robot navigate a maze on its own, and send the maze information wirelessly to a base station. The base station displayed the updates on a screen.

## Radio Communication

- store maze info in 81 long (9x9) byte array of Square data struct
- each square contains 8 bits of information packed into a struct
  - 4 bits (1 for each wall)
    - change from left-right-fwd to N-E-S-W
  - 1 bit for detecting robot
  - 3 bits for treasure
    - 1 bit for color, 2 for shape
  - shape(0,0) == no treasure
- update fields accordingly, transmit data in packets
  - if at position 4,5 update 4,5 entry in array
  - send raw maze info or decoded?

## System Integration
