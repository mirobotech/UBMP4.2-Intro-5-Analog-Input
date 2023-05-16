/*==============================================================================
 Project: Intro-5-Analog-Input          Activity: mirobo.tech/ubmp4-intro-5
 Date:    May 16, 2023
 
 This introductory programming activity for the mirobo.tech UBMP4 demonstrates
 analog input, number base conversion, and the use of a simple, external serial
 output function library. (Serial output is a useful technique for debugging
 using a logic analyzer, or digital oscilloscope with a serial decode function.)
 
 Additional program analysis activities investigate the use and operation of the
 analog constants and input functions in the included files, as well as explore
 bit-wise AND operations for selectively clearing and testing bits.
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP420.h"         // Include UBMP4.2 constants and functions
#include    "Simple-Serial.h"   // Include simple serial functions

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// ASCII character code definitions
#define LF      10              // ASCII line feed character code
#define CR      13              // ASCII carriage return character code

// Program variable definitions
unsigned char rawADC;           // Raw ADC conversion result

// Decimal digit variables used by binary to decimal conversion function
unsigned char dec0;             // Decimal digit 0 - ones digit
unsigned char dec1;             // Decimal digit 1 - tens digit
unsigned char dec2;             // Decimal digit 2 - hundreds digit

// Convert an 8-bit binary number to 3 decimal digits
void bin_to_dec(unsigned char bin)
{
    dec0 = bin;             // Store number in ones digit
    dec1 = 0;               // Clear tens digit
    dec2 = 0;               // Clear hundreds digit
    
    // Count hundreds digits in dec2 variable
    while(dec0 >= 100)
    {
        dec2 ++;
        dec0 = dec0 - 100;
    }
    // Count tens digits in dec1 variable, dec0 will contain remaining ones
    while(dec0 >= 10)
    {
        dec1 ++;
        dec0 = dec0 - 10;
    }
}

int main(void)
{
    // Set up ports and ADC
    OSC_config();               // Configure oscillator for 48 MHz
    UBMP4_config();             // Configure I/O for on-board UBMP4 devices
    ADC_config();               // Configure ADC and enable input on Q1
    H1_serial_config();         // Prepare for serial output on H1
        
    // Enable PORTC output except for phototransistor Q1 and IR receiver U2 pins
    TRISC = 0b00001100;
    
    // If Q1 and U2 are not installed, all PORTC outputs can be enabled by
    // uncommenting the line below:
    // TRISC = 0b00000000;
    
    // Enable on-die temperature sensor and set high operating Vdd range
    FVRCON = FVRCON | 0b00110000;
    
    // Select the on-die temperature indicator module as ADC input and wait
    // for the recommended acquisition time before A-D conversion
    ADC_select_channel(ANTIM);
    __delay_us(200);
    
    // Select Q1 phototransistor as ADC input (can be selected if Q1 is installed)
    // ADC_select_channel(ANQ1);
    
    while(1)
    {
        // Read selected ADC channel and display the analog result on the LEDs
        rawADC = ADC_read();
        LATC = rawADC;
        
        // Add serial write code from the program analysis activities here:
        
        __delay_ms(100);
        
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}

/* Learn More -- Program Analysis Activities
 * 
 * 1.   The TRISC register is originally configured by the UBMP4_config()
 *      function, and controls the data direction (input or output) of the PORTC
 *      I/O pins. A value of 0 in any TRIS register bit position configures the
 *      pin to be an output (0 is like 'o', in output), while a value of 1 sets
 *      a pin as an input (1 is like 'i', in input).
 * 
 *      What is the original value of TRISC set in UBMP4_config? How does the
 *      statement:
 * 
 *      TRISC = 0b00001100;
 * 
 *      in the main() function change the function of the PORTC I/O pins?
 * 
 * 2.   This program converts an analog input into an 8-bit digital value
 *      representing the input, and then outputs each bit of the digital value
 *      to the UBMP4 PORTC header pins. A voltmeter can be used to read the
 *      digital result on the H1-H8 header pins, except for pins H3 and H4.
 *      This is because H3 and H4 have been set as inputs in this program since
 *      they are usually connected to input devices Q1 (an ambient light sensor
 *      or phototransistor) and U2 (the IR demodulator) on the UBMP4 circuit.
 * 
 *      If your UBMP4 has Q1 and U2 installed leave the TRISC register as set,
 *      above. If Q1 and U2 are not installed, the TRISC register can be set to
 *      output digital data on all of the header pins by clearing it instead:
 
    TRISC = 0b00000000;
 
 *      Read the conversion result by connecting a voltmeter between ground (the
 *      header pin closest to the circuit board edge, and marked by a ground
 *      symbol near the H1 legend) and each header signal pin (the header pin
 *      closest to the resistors, marked by a pulse symbol).
 * 
 *      H8 is the most significant bit of the result, and H1 is the least
 *      significant bit. Starting at H8, record all 8 bits of the analog
 *      value, representing voltages close 5V as 1, and voltages near 0V as 0.
 *      What was your binary result? What is it when converted to decimal?
 * 
 * 3.   The statement that outputs the digital value of the analog input to the
 *      header pins is: LATC = rawADC;
 * 
 *      Since the upper 4 bits of PORTC are also physically connected to LEDs
 *      D2-D5, the LEDs will light to represent these first four data bits as
 *      well. Do the lit LEDs match your measured result, above?
 * 
 *      The lower 4 bits of the result can be displayed instead of the upper 4
 *      bits by using a bitshift operator to move the contents of the ADC
 *      conversion result four bits to the left before outputting the value to
 *      LATC. This essentially slides each bit four positions to its left. The
 *      upper 4 bits will be lost during this shift operation, as they are
 *      replaced by the lower 4 bits of the result, and the lowest four bits
 *      will be 'empty' or 0.
 * 
 *      Update the LATC expression to add the bitshift operator, below. Rebuild
 *      the program and run it again.
 * 
        LATC = rawADC << 4;     // Shift result 4 bits left to display low nybble
 * 
 *      The least significant bits of a conversion result will change the most
 *      quickly as an analog input voltage changes. If you are using the
 *      built-in temperature sensor as the input, try heating the
 *      microcontroller with your touch, or by cradling the circuit board in
 *      your hands until you see one or more bits changing on the LEDs.
 * 
 * 4.   The PIC16F1459 microcontroller has 9 analog inputs shared with its
 *      digital I/O pins. Pins used for analog input must be explicity enabled
 *      as both input pins, and also as analog pins. Examine the ADC_config()
 *      function and explain how it configures the Q1 pin for analog input.
 * 
 *      (Amusingly, the microcontroller starts up from a reset with these pins
 *      configured as analog inputs already, and this programs's UBMP4_config()
 *      function reconfigures them as digital I/O. You can explore how this is
 *      done in the function.)
 * 
 * 5.   The 9 analog inputs in the PIC16F1459 can be connected into the single
 *      ADC (analog-to-digital coverter) one at a time through an analog
 *      multiplexer (or mux). After changing to a different input, a short delay
 *      is necessary to allow the input potential to 'settle' (by charging an
 *      internal capacitor to the new input voltage) before the conversion
 *      starts. The function call:
 * 
        ADC_select_channel(ANTIM);
 * 
 *      selects the microcontroller's on-board Temperature Indicator Module
 *      (ANTIM) as the ADC input, but does not add a settling delay. What other
 *      analog inputs are available on UBMP4? (Hint: see the UBMP420.h file)
 * 
 *      The ADC_select_channel() function does not add a delay after switching
 *      a new input to the ADC, but there is another function that switches
 *      inputs, adds a delay, and performs the analog to digital conversion.
 *      What is this function called? Why do you think it was not used in this
 *      program?
 * 
 * 6.   This program contains the bin_to_dec() function which demonstrates a
 *      simple method of converting an 8-bit binary value to three decimal
 *      digits. Can you figure out how it works? Explain, or use a flow chart
 *      to describe its operation.
 * 
 * 7.   Two serial data functions are included in the Simple-Serial.c file.
 *      These functions enable the program to output RS-232 formatted serial
 *      data on the H1 header pin. This allows a logic analyser or oscilloscope
 *      to read the value of the serial data more conveniently and quickly than
 *      by successivley probing each of the header pins using a voltmeter.
 * 
 *      The format of the serial data is straightforward to understand. When no
 *      data is being transmitted, the serial line is 'idle', represented by a
 *      high voltage, or 1 logic level. (This is usually true at the TTL logic
 *      level, before an RS-232 level shifter ?? idle is usually a low voltage
 *      on the actual RS-232 data cable, after the level shifter.)
 * 
 *      Every data transmission is preceeded by a low voltage, or logic 0 level
 *      Start bit. Next, the data bits themselves are transmitted in order from
 *      LSB (least-significant bit) to MSB (most-significant bit). The
 *      transmission is ended with a logic 1 level Stop bit.
 * 
 *      Each data bit, 0 or 1, is transmitted at a low or high voltage level,
 *      respectively, and each bit remains at its logic level for the entire bit
 *      duration (this is known as NRZ, or non-return-to-zero format). The bit
 *      duration is a reciprocal of the data rate. For example at the commonly-
 *      used 9600 bps (bits-per-second) data rate, each bit is 1/9600s, or
 *      roughly 104us (microseconds) in length.
 * 
 *      If you have access to an oscilloscope or logic analyzer, add the
 *      following code to the program (below the LATC output statement) to
 *      enable serial output. Then, probe H1 to view the serial data:
 
        // Write ADC result to H1 as binary serial data
        H1_serial_write(rawADC);
        
 *      Can you record or capture the digital serial data? What is the digital 
 *      equivalent of the analogue value that was transmitted?
 * 
 * 8.   Rather than interpreting or converting the analogue value yourself, this
 *      next block of code can be added to your program to convert the analog
 *      result to three decimal digits, transmit each of the digits serially as
 *      ASCII characters, and add the character codes signifying the end of a
 *      line of text (CR) and the start of a new line of text (LF). Logic
 *      analysers or digital oscilloscopes may be able to decode these digits,
 *      or the output can also be connected to a serial terminal to display the
 *      text on screen.
 
        // Convert ADC result to decimal and write each digit serially in ASCII
        bin_to_dec(rawADC);
        H1_serial_write(dec2 + 0x30);   // Convert each digit to ASCII
        H1_serial_write(dec1 + 0x30);
        H1_serial_write(dec0 + 0x30);
        H1_serial_write(CR);            // Carriage return character
        H1_serial_write(LF);            // Line feed character
                
 * 
 * 9.   Both the H1_serial_config() function and the H1_serial_write() function
 *      make use of bit-wise AND (&) operators. Below is the AND operator used
 *      by the H1_serial_config() function:
 * 
        TRISC = TRISC & 0b11111110;
 * 
 *      Can you determine what this statement is doing, and why an AND operation
 *      is being used instead of just over-writing the TRISC value? Explain.
 * 
 * 10.  The data transmission loop of the H1_serial_write() function is shown
 *      below:
 * 
    // Shift 8 data bits out LSB first
    for(unsigned char bits = 8; bits != 0; bits--)
    {
        if((data & 0b00000001) == 0)    // Check the least significant bit state
        {
            H1OUT = 0;
        }
        else
        {
            H1OUT = 1;
        }
        __delay_us(103);    // Shorter delay to account for 'for' loop overhead
        data = data >> 1;   // Prepare next bit by shifting data right 1 bit
    }
 *  
 *      Explain how the AND operation used in the if condition can determine
 *      the state of the least significant data bit.
 * 
 *      After the least significant data bit is transmitted, how are the other
 *      seven bits in the data byte isolated and transmitted?
 *      
 * 
 * Programming Activities
 * 
 * 1.   The bin_to_dec() function converts a single byte into three decimal
 *      digits. Step 8 in the program analysis activities, above, converted
 *      each of these numeric digits to the ASCII code representing the digit.
 *      Can you make a bin_to_ASCII() function that would eliminate the need
 *      for you to offset the values to ASCII separately, as was done here?
 * 
 * 2.   Does your UBMP4 have Q1 installed? If Q1 is installed, it will be easier
 *      to use Q1 as an input device instead of the temperature module. Q1 can
 *      either be an ambient light sensor, which is sensitive to visible light,
 *      or a phototransistor, sensitive to infrared (IR) wavelengths. Try using
 *      your phone's flashlight (for an ambient light sensor) or an infrared LED
 *      (for a phototransistor) to illuminate Q1.
 * 
 *      Create a program to implement threshold detection and to light an LED
 *      when that threshold is crossed. Start by determining the analog level
 *      corresponding to low intensities of light and the level representing
 *      bright light. Use conditional instructions to light an LED when the
 *      light level rises above the bright threshold and drops below the dark
 *      threshold set in your program. Can you think of a reason why two 
 *      different light thresholds would be better than one? What other types
 *      of electronic devices or circuits work this way?
 * 
 * 3.   Create a program that produces a PWM output proportional to an analog
 *      input, or a program that creates a tone having a pitch proportional
 *      to an analog input value.
 * 
 * 4.   If you have an oscilloscope available, investigate how fast you can get
 *      the serial output function to transmit data. Try setting the bit delays
 *      inside the serial write function to 1 microsecond of delay, instead of
 *      104 and 103 as used in the current function, and try the program. Does
 *      it work the way it should? Is each bit actually 1us in duration? What do
 *      you think would make them different if the bits are not 1us long?
 * 
 * 5.   Creating a serial data transmission function is relatively straight-
 *      forward. Receiving serial data can be done in a very similar way.
 *      Think about how you would create a function to receive serial data. How
 *      could you ensure that the data is received correctly even if there are
 *      slight clock timing differences between the microcontrollers in the
 *      transmitting and receiving devices? Create the pseudocode or a flowchart
 *      describing a serial receive function.
 * 
 * 6.   In this program, transmitting serial data involves isolating one bit
 *      of a variable using a bit-wise logical operator, and bit-shifting the
 *      data into place within a loop (and doing it at the proper bit timing,
 *      of course). Received serial data bits can be re-assembled into an 8-bit
 *      variable using a similar technique. Try to create a function, or just
 *      its main code loop, that would successively read serial data bits from
 *      an input pin and assemble the values into an 8-bit variable.
 */
