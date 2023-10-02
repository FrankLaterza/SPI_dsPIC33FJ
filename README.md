# SPI dsPIC33FJ Slave
This is a SPI slave routine for the dsPIC33FJ128GP204 MCU and compiled with the XC16. The purpose of this code is to sent bytes through spi on the interrupt from the master.

To get started include the header file and run the SpiInit. After that you need to make sure that you're calling SpiWatchdog to check on the state of the Spi transmission. Setup the intervals in the spi.h file. Currently i've created an array called bytes which is filled with numbers 1-5 so make sure to change it to whatever data you want to send over. Happy coding
