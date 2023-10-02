#include "xc.h"
#include "spi.h"

// size of byte packet
#define SPI_PACKET_BYTE_SIZE 5
// packet to send over spi
uint8_t bytes[SPI_PACKET_BYTE_SIZE] = {1,2,3,4,5};
/**
 * if we have not reached the end of our data transmission in 
 * the time frame defined by SPI_TIMOUT_MAX, we need to reset
 * our iterator to the first position. This function is defined
 * in the function called SpiWatchdog()
 */
uint16_t spiTimeoutCount = 0;
// global spi buffer (slave in)
uint8_t spiDataMosi[SPI_PACKET_BYTE_SIZE] = { 0 };
// iterator
uint8_t it = 0;
// a checksum
uint8_t checksum = 0;
// spi states
enum SpiTask { START, SEND, END };
enum SpiTask spiState = START;

// spi interrupt on full buffer
void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void) {
    // get the data from the master out
    spiDataMosi[it] = SPI1BUF;
    // state machine
    switch(spiState){
        // start
        case START:
            // load hello
            SPI1BUF = START_DELIMITOR;
            // reset it
            it = 0;
            // reset checksum
            checksum = 0;
            // pet the dog
            spiTimeoutCount = 0;
            // new state
            spiState = SEND;
            break;
        // get all the bytes
        case SEND:
            // check for state change
            if (it >= SPI_PACKET_BYTE_SIZE-1) {
                // load checksum
                SPI1BUF = checksum;
                // after last byte sent next state is the end
                spiState = END; 
                // end early
                break;
            }
            // add to the checksum. wraps around.
            checksum += bytes[it];
            // load next val and inc
            SPI1BUF = bytes[it];
            // inc the it
            it++;
            // pet the dog
            spiTimeoutCount = 0;
            break;
        // end
        case END:
            // say bye bye
            SPI1BUF = END_DELIMITOR;
            // reset it
            it = 0;
            // reset checksum
            checksum = 0;
            // pet the dog
            spiTimeoutCount = 0;
            // reset missed data counter (implement here)
            // state back to top
            spiState = START;
            break;
    }   
    IFS0bits.SPI1IF = 0; // clear the interrupt
}

void SpiInit(void){
    IFS0bits.SPI1IF = 0; // clear the interrupt
    IEC0bits.SPI1IE = 0; // disable the interrupt
    SPI1STATbits.SPIEN = 0; // disable spi
    // SPI1STATbits.SISEL = 3 // 011 = interrupt when the SPIx receive buffer is full (the SPIRBF bit set)
    SPI1STATbits.SPIROV = 0; // no overflow    
    SPI1CON1bits.MODE16 = 0; // byte wide.
    SPI1CON1bits.SMP = 0; // data sampled at mid of data output time
    SPI1CON1bits.DISSCK = 0; // internal clock is enabled
    SPI1CON1bits.DISSDO = 0; // SDO is enabled (data out)
    SPI1CON1bits.SSEN = 1; // config as slave
    SPI1CON1bits.CKP = 0; // idle clock is low and active is high
    SPI1CON1bits.CKE = 0; // data changes from idle to high
    SPI1CON1bits.MSTEN = 0; // not master
    SPI1CON2bits.FRMEN = 0; // not framed (we are slave anyways)
    // SPI1CON1bits.SPIEN = 1; // enhanced buffer mode ??
    IFS0bits.SPI1IF = 0; // clear the interrupt
    IEC0bits.SPI1IE = 1; // enable the interrupt

    // set the buffer to the start
    SPI1BUF = START_DELIMITOR;
    // enable spi
    SPI1STATbits.SPIEN = 1;
}

// keeps code readable
void SpiWatchdog (void){
    // check the time passed from last full buff isn't the max timeout
    if(spiTimeoutCount > SPI_TIMEOUT_MAX && it != 0 ){
        // reset the it
        it = 0;
        // reset the count
        spiTimeoutCount = 0; 
        // mark missed information (implement here)
        // reset state
        spiState = 1;
        // reset the spi
        SpiInit();
        // return
        return;
    }
    
    // counter which stops adding once the threshold is reached
    // (protect from int overflow)
    if (spiTimeoutCount < SPI_TIMEOUT_MAX + 1){
        spiTimeoutCount++;
    }

}
