/*!
 * @author  Syed Asad Amin
 * @date    Oct 24th, 2020
 * @file    main.cpp
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

#include "main.h"
using namespace std;

//============================================================================//

//===== GLOBAL VARIABLES =====================================================//

static const char logPath[] = "/home/pi/Desktop/MCP3008.log";
char logMsg[64];

static int gpiofd;
static int spifd;

bool isRunning = true;
int16_t adcData[6] = {0};
uint8_t idx = 0;

//============================================================================//

int main() {
    fprintf(stdout, "Application started.\r\n");

    initGPIO();
    initSPI();

    fprintf(stdout, "Initialization(s) complete.\r\n");

    while(isRunning) {
        int16_t val = readADC(SPI_CHANNEL_1, CHAN_CONF_SING, idx);
        adcData[idx] = val;
        delayMicroseconds(20);                                                  // Sample capacitor charge time.
        
        sprintf(logMsg, "A%d: %d", idx, adcData[idx]);
        writeFile(logPath, logMsg);
        
        if(idx >= 6) { idx = 0; }
        idx++;
    }

    close(gpiofd);
    close(spifd);
    
    return EXIT_SUCCESS;
}

void initGPIO(void) {
    if((gpiofd = wiringPiSetupGpio()) < 0) {
        fprintf(stderr, "GPIO initialization failed.\r\n%s", strerror(errno));
        return;
    }
    fprintf(stdout, "GPIO initialization complete.\r\n");
}


void initSPI(void) {
    if((spifd = wiringPiSPISetup(SPI_CHANNEL_1, SPI_CLK_SPD)) < 0) {
        fprintf(stderr, "SPI initialization failed.\r\n%s", strerror(errno));
        return;
    }
    fprintf(stdout, "SPI initialization complete.\r\n");
}


int16_t readADC(uint8_t spiChannel, uint8_t confMode, uint8_t adcChannel) {
    int16_t res = 0;
    if(adcChannel < 0 || adcChannel > 7) return -1;
    
    uint8_t buf[3] = {1};                                                       // Start Bit
    buf[1] = (confMode + adcChannel) << 4;                                      // Config bit and channel
    wiringPiSPIDataRW(spiChannel, buf, sizeof(buf));                            // Single mode: [0x10, 0x20...] Diff mode: [0x90, 0xA0....]
    res = ((buf[1] & 0x03) << 8) + buf[2];                                      // Result byte received duing the cycle of mode writing as well as next cycle.
    return res;
}


void writeFile(const char *file, char *data) {
    ofstream mFile;
    mFile.open(file, ios::out | ios::app);
    mFile << data << endl;
    mFile.close();
}
