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
In this part of the lab, we worked on integrating everything together. We added a new level to our robot to house our microphone circuit. We put the microphone circuit and the three schmitt triggers onto a breadboard on this top level. After this lab, we will focus on tidying up our robot, moving things from breadboards to solderable breadboards or PCBs.

We added code in our setup function to keep running the FFT until it detects a 660 Hz signal. Once this signal is detected, the robot begins navigating the maze. In the video below, you can see the robot waiting for the 660 Hz tone, once it detects that, it starts navigating the maze, following lines, avoiding walls, and avoiding robots.

<iframe width="560" height="315" src="https://www.youtube.com/embed/zOxhwYFFuUA" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

The last line of our setup function calls a helper function called waitForMic() which continuously runs the FFT until it detects a 660 Hz signal.
We are running the FFT for 128 points instead of 256 points. We chose to do this in Milestone2 to save dynamic memory.
For the IR signal, we used the free running mode for the ADC.
We use analogRead() instead of running the ADC in free running mode for the microphone because free running mode for 128 points puts 660Hz in a small bin that is difficult to detect because the first few bins are always detect a high number.
analogRead() runs at a lower sampling frequency so 660Hz ends up in bin 10 for the 128 point FFT. 

```
void waitForMic(){
  while(1){
    runMICFFT();  
    Serial.println("waiting for 660 tone");
    if (fft_log_out[10] > 60){
      Serial.println("660 Hz signal detected");
      break;
      }
   delay(50);
  }
}

void runMICFFT(){
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 256 ; i += 2) { // save 256 samples
    fft_input[i] = analogRead(A2); // put analog input (pin A0) into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }

  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();

}
```


