#ifndef SPI_H
#define SPI_H

#include <xc.h>
#define SPI_TIMEOUT_MAX 300
#define START_DELIMITOR 0xAA
#define END_DELIMITOR 0x55

void SpiInit(void);
void SpiWatchdog(void);

#endif /* SPI_H */