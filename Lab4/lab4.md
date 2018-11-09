# Lab 4: FPGA and Vision
[Home](https://ece3400team19.github.io/)

## Team Tasks
For this lab, we split into two teams (Team Arduino and Team FPGA).  One to develop an FPGA module capable of detecting basic shapes from our OV7670 camera input, and another to pass this information onto our Arduino.

* Arduino Team: Asena, Laasya
* FPGA Team: Cynthia, Robert, Nonso

## Lab Description
 This lab was the first step in adding a treasure detection system to our robot capable of recognizing basic shapes.  In this lab we became familiar with the camera and working on FPGA, reading the camera and creating a basic image processor capable of detecting colors.  

## Arduino
2 points: Arduino-Camera communication (writing the correct registers)
3 points: Arduino-FPGA communication (communicating treasure/no treasure + shape and color)

## FPGA
On the FPGA side, we first wrote an image into memory, then spit this image back out to the VGA display this required writing specific colors to memory at given times and hooking the output of the memory up to the VGA module.  We created a flag.  
<img src="Lab04_Flag.jpeg" width="620" height="330" alt="SIgnal-Flag">
The code to generate this flag was written in verilog as shown below.
```
always @ (WRITE_ADDRESS) begin
		//write memory
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
			W_EN = 1'b0;
			pixel_data_RGB332 = 8'd0;
		end
		else begin
			if ( VGA_PIXEL_X  >= 0 && VGA_PIXEL_X  <= 58) begin
				pixel_data_RGB332 = RED;
			end
			else if ( VGA_PIXEL_X  >= 59 && VGA_PIXEL_X  <= 58*2) begin
				pixel_data_RGB332 = 8'b11111111;
			end
			else begin
				pixel_data_RGB332 = GREEN;
			end
			W_EN = 1'b1;
		end
end
```
Once we verified the memory block was updating correctly, we next sought about interfacing with the camera.  The camera sends 2 bytes off color data at a time, which our friends on the Arduino team told us was in an RGB565 configuration.  We had to reduce this to an RGB 332 configuration which was compatible with our VGA driver and storable in a single byte.  The final implementation we settled on was in the form of a finite state machine clocked by the incoming clock of the camera as shown below.  
<img src="FSM_Diagram.PNG" width="400" height="400" alt="SIgnal-Flag">
The FSM waits in idle until a row transmission begins.  Once there, it alternates between Byte 1 and Byte 2 until a row transmission finishes, when it transitions to line done.  When the entire image has been received, vsync will go high, at which point the FSM resets.  In Byte 1, the down sampler reads the first byte of camera data from GPIO and stores it in a register file.  In Byte 2, the down sampler reads the second byte from GPIO and concatenates bits from this new data and the data in the register file to create an 8-bit RGB332 value which is sent to memory.  

## Integration
With the two halves done, all that was left was putting them together (and a considerable amount of debugging).  We hooked the 8 data pins, HREF, PCLK, and VSYNC up to the FPGA as GPIO inputs and a single 24 MHz clock signal as an output to the camera to drive it.  [Discuss camera register stuff here?] With the Arduino setting up the camera's registers to display a color bar test image, we observed correct operation, the camera updating a monitor with the color bar test image over VGA.  

From here, we needed only set a few registers to get live video feed from the camera.  To detect colors, we wrote a rudimentary verilog program to increment counters based on values of red or blue in excess of a threshold. This program sent 2 bits of output over GPIO to the Arduino, which updated correspondingly colored LEDs accordingly.  