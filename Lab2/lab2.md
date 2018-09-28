# Lab 2: Analog Circuitry and FFTs
[Home](https://ece3400team19.github.io/)

## Team Tasks
* Acoustic Team: Asena, Laasya, Nonso
* Optical Team: Cynthia, Robert

## Lab Description

In this lab, we added sensors and circuitry to our robot. The first was a microphone circuit that detects a 660Hz signal, this signal will be used to signify the start of the final competition. The second was an IR circuit that detects a 6.08kHz signal, a frequency that will be emitted by other robots during the competition. For both circuits, we need to be able to distinguish the desired frequency from decoys.

# Part 1: Acoustic

## Team Materials

* 1 Arduino Uno
* Electret microphone
* Capacitors and resistors of our choosing

We started the lab by assembling the microphone circuit suggested by the lab handout with a 1 µF capacitor and a 3.3 kΩ resistor:

 <img src="basicMicrophoneCircuit.png" width="180" height="156" alt="microphone-circuit">

We then used a tone generator app to play a 660Hz tone directly into the microphone at maximum volume, then checked the output of the circuit using the oscilloscope.

We were able to see our 660Hz signal on our oscilloscope with great difficulty, because the microphone also picked up all other noise in the room.
Then, we started looking at how to amplify our desired signal and squash other signals. We decided to feed the output of our microphone into an active band pass filter.
We started by building Team Alpha's circuit, and then adding an additional capacitor for more filtering.

Here is a picture of our circuit:

<img src="Acoustic Circuit.JPG" width="336" height="252" alt="acoustic-circuit">

<img src="mic_schematic.png" width="430" height="252" alt="mic-circuit" img align = "right">

After building our circuit, we modified the example sketch from the FFT library, fft_adc_serial, to use analogRead(), the built-in method for reading an analog input, instead of running the ADC in free-running mode. The frequency 660Hz is in the 5th bin using the ADC, but in the 20th bin using analogRead() because analogRead() runs at a much slower sampling rate.

We first used the signal generator to put a 660Hz, 50mVpp with 25mV offset sine wave into our active band pass filter. We fed the output into pin A1 and ran our modified code for FFT. We got the output from the serial monitor and plotted it in Excel. We then played a 660Hz tone from the app Tone Generator directly into our microphone. We got the output and plotted in in Excel. The two plots are shown below:

 <img src="FFT1.PNG" width="706" height="410" alt="fft-graph">

 There is a peak of similar amplitude in the fifth pin from the 660Hz from the signal generator and the 660Hz from the tone generator app. This shows that our circuit is able to detect a 660Hz tone.

 We then played the 660Hz tone into our microphone again and checked the output using the oscilloscope.
 The setup and oscilloscope looked as follows:

<img src="Acoustic Test.JPG" width="240" height="320" alt="tone generator setup" img align="left">
<img src="oscilloscope_audio.JPG" width="371.91" height="320" alt="tone generator setup" img align="center">

The frequency of the output measured by the oscilloscope was 660Hz and the voltage was 1.42 Vpp.







# Part 2: Optical

## Team Materials
* 1 Arduino Uno
* IR transistor (OP598)
* 1 IR hat (given by TAs)
* 1 IR decoy
* Capacitors and resistors of our choosing

## FFT Analysis and Arduino Code

## Circuit Design and Construction
[Figure: Circuit Diagram]

The circuit consists of three distinct stages.  The photodetector, a low-pass filter, and a non-inverting amplifier.  Each stage is discussed below.  

The photodetector’s configuration is largely identical to the configuration listed on the lab sheet with the exception of a resistive network to lower the bias voltage at the top.  We chose to lower this bias voltage from 5V to about 250 mV after considerable difficulty with the photodetector itself.  With a 5V pull up, the detector could only pull down the voltage in pulses by about half a volt, resulting in a pulse ranging from ~4.5 to 5V at maximum intensity (i.e. with the IR hat right next to the detector).  

[Figure: 5V bias waveform]

Because of the poor quality of the photodetector, we decided to amplify this signal, but doing so in a 4.5-5V range would be difficult while keeping it in a safe range for the Arduino.  Our solution was to decrease the bias voltage such that we would still see a comparable-size pulse while still allowing for later amplification.  With a bias voltage of 250 mV, the input pulse to the amplifier would be across most of the range from 250mV to 0V and, with a gain of about 10, would produce a pulse of 2.5V peak to peak.  

[Figure: 250mV Bias Waveform]

Another solution would be to reverse the resistor and photodetector positions, having the photodetector pull up the signal rather than pull it down.  

The second stage is a simple RC low-pass filter.  With resistor and capacitor values as shown, it has a corner frequency at roughly 10 kHz.  These values were chosen so the filter would pass 6 kHz frequencies while rejecting frequencies at 18 kHz.  With this configuration, we were able to cut the amplitude of 18kHz signals to less than 50% while keeping 6kHz signals within 10% of their initial amplitude.  This filter, along with software on the board not checking bins at 18 kHz, allows us to reject the decoy signal.  

[Figure(s): 6 kHz Pass and 18 kHz Rejection]

The third stage is a non-inverting amplifier with a gain of 11x.  With our signal biased at 250 mV, the maximum value we would expect would be around 2.5 V.  In practice, the pulse ends up smaller than this would expect, and our final signal had a peak-to-peak of about 1V.  This means we could increase the gain of our amplifier or the bias voltage on the detector to increase our detectable range.  

[Figure: Input and Output Waveforms for op-amp in detection operation
