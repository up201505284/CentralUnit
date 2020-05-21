#include <bcm2835.h>
#include <QDebug>


//  Slave pins
#define PIN_CHANNEL1                RPI_BPLUS_GPIO_J8_11
#define PIN_CHANNEL2                RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL3                RPI_BPLUS_GPIO_J8_13
#define PIN_CHANNEL4                RPI_BPLUS_GPIO_J8_15
#define PIN_CHANNEL5                RPI_BPLUS_GPIO_J8_16

// Json File
#define JSON_FILE_NAME              "RecordsOfChannels.js"

//  SPI Commands
#define INITIALSETUP()              0x01
#define SAFERESET()                 0x02
#define BASICEXTENDED()             0x03
#define BASICREFRACTED()            0X04
#define ADVANCEDEXTENDED()          0x05
#define ADVANCEDREFRACTED()         0x06
#define STOP()                      0x07
#define ENABLESPI()                 0x08
#define DISABLESPI()                0x09
#define SPICOMMANDINITIAL()         0xF0
#define SPICOMMANDFINISHED()        0xF1
#define SPIREADWITHSUCESS()         0xF2

//  SPI Masks
#define INITIAL                     0
#define MODE                        1
#define SHIFT                       2
#define PULSE_RATE_DISABLE          2
#define STROKE_LENGHT               2
#define PULSE_RATE_ENABLE           3
#define ACCELARATION_RATE           4
#define ACCELARATION_TIME           5
#define SOFT_START_STOP             6
#define SIZE_BUFFER_ENABLE_SPI      8
#define SIZE_BUFFER_DISABLE_SPI     3
#define SIZE_BUFFER_BASIC           3
#define SIZE_BUFFER_ADVANCED        4

void spiInit(void)
{
    if (bcm2835_init()) {

        //  Set slave pins as digital outputs
        bcm2835_gpio_fsel(PIN_CHANNEL1, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL2, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL3, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL4, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL5, BCM2835_GPIO_FSEL_OUTP);

        if (bcm2835_spi_begin()) {
            bcm2835_spi_setBitOrder             (BCM2835_SPI_BIT_ORDER_MSBFIRST );  // The default
            bcm2835_spi_setDataMode             (BCM2835_SPI_MODE0              );  // The default
            bcm2835_spi_setClockDivider         (BCM2835_SPI_CLOCK_DIVIDER_65536);  // cdiv = 65536; speed = 3.814 kHz
            bcm2835_spi_chipSelect              (BCM2835_SPI_CS_NONE            );  // Itself control
            bcm2835_spi_setChipSelectPolarity   (BCM2835_SPI_CS_NONE, LOW       );

            //  Disable all slaves
            bcm2835_gpio_write(PIN_CHANNEL1, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL2, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL3, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL4, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL5, HIGH);
        }

    }

}

void spiEnableSlave(int _channel)
{
    bcm2835_gpio_write(_channel, LOW);
    bcm2835_delayMicroseconds(800); //  Wait 3  clock tickets to complete initialization
}
void spiDisableSlave(int _channel)
{
    bcm2835_gpio_write(_channel, HIGH);
    bcm2835_delayMicroseconds(267); //  Wait 1  clock tickets to complete
}
void spiClose(void)
{
    bcm2835_spi_end();

    bcm2835_gpio_write(PIN_CHANNEL1, LOW);
    bcm2835_gpio_write(PIN_CHANNEL2, LOW);
    bcm2835_gpio_write(PIN_CHANNEL3, LOW);
    bcm2835_gpio_write(PIN_CHANNEL4, LOW);
    bcm2835_gpio_write(PIN_CHANNEL5, LOW);

    bcm2835_close();
}

int disableCommunication(int channel) {

    spiEnableSlave(channel);

    char bufferR [SIZE_BUFFER_DISABLE_SPI]  = {};
    bufferR[INITIAL]                        = SPICOMMANDINITIAL();
    bufferR[MODE]                           = DISABLESPI();
    bufferR[SIZE_BUFFER_DISABLE_SPI-1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferR, SIZE_BUFFER_DISABLE_SPI);

    spiDisableSlave(channel);
    qDebug()<<(int)bufferR[PULSE_RATE_DISABLE];

    return bufferR[PULSE_RATE_DISABLE];
}

int enableCommunication(int channel, uint8_t strokeLenght, uint8_t pulseRate, uint8_t accelarationRate, uint8_t accelarationTime, uint8_t enableSoftStartStop) {
    spiEnableSlave(channel);

    char bufferT [SIZE_BUFFER_ENABLE_SPI]   ={};
    bufferT[INITIAL]                        = SPICOMMANDINITIAL();
    bufferT[MODE]                           = ENABLESPI();
    bufferT[STROKE_LENGHT]                  = strokeLenght;
    bufferT[PULSE_RATE_ENABLE]              = pulseRate;
    bufferT[ACCELARATION_RATE]              = accelarationRate;
    bufferT[ACCELARATION_TIME]              = accelarationTime;
    bufferT[SOFT_START_STOP]                = enableSoftStartStop;
    bufferT[SIZE_BUFFER_ENABLE_SPI-1]       = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ENABLE_SPI);

    spiDisableSlave(channel);

    for (int i = 0; i < SIZE_BUFFER_DISABLE_SPI; i++) {
        if ( (bufferT[i] != SPIREADWITHSUCESS()) && (i != 0) )
            return -1;
    }

    return 1;
}



