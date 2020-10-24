/*!
 * @author  Syed Asad Amin
 * @date    Oct 24th, 2020
 * @file    main.h
 * @version v1.0.0
 *          | v1.0.0 -> Added GPIO, SPI initialization and a readADC function
 *                      to read data from MCP3008
 * 
 * @note    This is a program writen in C to interface MCP3008, a adc IC having
 *          8 channel inputs / 10 bits / 200Ksps data rate.
 * 
 *          This program uses MCP3008 to sample data at 40 - 48KHz sample rate
 *          and collects the data from channels into a buffer.
 *          Also, a logging operation is added as well to be able to view the 
 *          sampled data using a file.
 * 
 *          As per datasheet,
 *          Fclk = 18 * Fsample = 18 * 48000 = 864000 Hz
 * 
 *          1MHz clock is used in our application
 *          
 *          Some calculations:
 *          Time for 1 sample = 18 / 1e6 = 18uSec
 *          Total time for 1 channel = Time for 1 sample + capacitor charge time
 *                                   = 18 uS + 20 uS
 *                                   = 38 uS
 *          Total time for 6 channel = Total time for 1 channel * 6
 *                                   = 38 uS * 6
 *                                   = 228 uS
 * 
 *          Best practise is to use a separate thread for spi and adc. To
 *          isolate the computation time consumption
 * 
 * A project for fiverr client. (symphonyrob)
 * 
 */

//===== INCLUDES =============================================================//

#pragma once

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <errno.h>
#include <unistd.h>

//===== DEFINITIONS ==========================================================//

// SPI Channel
#define SPI_CHANNEL_0   0
#define SPI_CHANNEL_1   1

// SPI Clock Speed
#define SPI_CLK_MIN     5e5     // For our application 1MHz clock rate is more
#define SPI_CLK_MAX     32e6    // than enough to sample data at 48KHz.
#define SPI_CLK_SPD     1e6     // Fclk = 18 * Fsample 

// SPI Mode
#define SPI_MODE_0      0
#define SPI_MODE_1      1
#define SPI_MODE_2      2
#define SPI_MODE_3      3

// MCP3008 Mode
#define CHAN_CONF_SING  8
#define CHAN_CONF_DIFF  0

//============================================================================//

//===== FUNCTION DECLARATIONS ================================================//

/*!
 * @brief This function initializes the GPIO using wiring pi library for RPi
 * @param None
 * @return None
 */
void initGPIO(void);

/*!
 * @brief This function initializes the SPI using wiring pi library for RPi
 * @param None
 * @return None
 */
void initSPI(void);

/*!
 * @brief This function reads the adc chip MCP3008 via SPI
 * @param [uint8_t] spiChannel : SPI channel used
 * @param [uint8_t] confMode : Either use in single more or differential mode
 * @param [uint8_t] adcChannel : Channel selection from 0 to 7
 * @return [int16_t] Digital data value of converted analog signal
 */
int16_t readADC(uint8_t spiChannel, uint8_t confMode, uint8_t adcChannel);

/*!
 * @brief This function write data to a file
 * @param [string] file: Name of file to write data on
 * @param [string] data: Data string to write on file
 * @return None
 */
void writeFile(const char *file, char *data);

//============================================================================//
