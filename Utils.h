#include <bcm2835.h>
#include <QDebug>


//  Slave pins
#define PIN_CHANNEL1                (uint8_t)   RPI_BPLUS_GPIO_J8_11
#define PIN_CHANNEL2                (uint8_t)   RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL3                (uint8_t)   RPI_BPLUS_GPIO_J8_13
#define PIN_CHANNEL4                (uint8_t)   RPI_BPLUS_GPIO_J8_15
#define PIN_CHANNEL5                (uint8_t)   RPI_BPLUS_GPIO_J8_16



//  SPI Commands
#define INITIALSETUP()              (uint8_t)   0x01
#define SAFERESET()                 (uint8_t)   0x02
#define BASICEXTENDED()             (uint8_t)   0x03
#define BASICREFRACTED()            (uint8_t)   0X04
#define ADVANCEDEXTENDED()          (uint8_t)   0x05
#define ADVANCEDREFRACTED()         (uint8_t)   0x06
#define STOP()                      (uint8_t)   0x07
#define ENABLESPI()                 (uint8_t)   0x08
#define DISABLESPI()                (uint8_t)   0x09
#define UPDATE()                    (uint8_t)   0x0A
#define SPICOMMANDINITIAL()         (uint8_t)   0xF0
#define SPICOMMANDFINISHED()        (uint8_t)   0xF1
#define SPIREADWITHSUCESS()         (uint8_t)   0xF2

//  SPI Masks
#define INITIAL                     (uint8_t)   0
#define MODE                        (uint8_t)   1
#define SHIFT                       (uint8_t)   2
#define PULSE_RATE_DISABLE          (uint8_t)   2
#define STROKE_LENGHT_HIGH          (uint8_t)   2
#define STROKE_LENGHT_LOW           (uint8_t)   3
#define PULSE_RATE_ENABLE           (uint8_t)   4
#define ACCELARATION_TIME_HIGH      (uint8_t)   5
#define ACCELARATION_TIME_LOW       (uint8_t)   6
#define SOFT_START_STOP             (uint8_t)   7
#define SIZE_BUFFER_ENABLE_SPI      (uint8_t)   9
#define SIZE_BUFFER_UPDATE          (uint8_t)   9
#define SIZE_BUFFER_DISABLE_SPI     (uint8_t)   3
#define SIZE_BUFFER_BASIC           (uint8_t)   3
#define SIZE_BUFFER_ADVANCED        (uint8_t)   4

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
            bcm2835_spi_setBitOrder             (BCM2835_SPI_BIT_ORDER_MSBFIRST );
            bcm2835_spi_setDataMode             (BCM2835_SPI_MODE0              );
            //  Speed = 3.814 kHz
            bcm2835_spi_setClockDivider         (BCM2835_SPI_CLOCK_DIVIDER_65536);
            //  Itself control
            bcm2835_spi_chipSelect              (BCM2835_SPI_CS_NONE            );
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

void spiEnableSlave(uint8_t _channel)
{
    bcm2835_gpio_write(_channel, LOW);
    bcm2835_delayMicroseconds(800); //  Wait 3  clock tickets to complete initialization
}

void spiDisableSlave(uint8_t _channel)
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

uint8_t disableCommunication(uint8_t _channel) {

    spiEnableSlave(_channel);

    char bufferR [SIZE_BUFFER_DISABLE_SPI]  = {};
    bufferR[INITIAL]                        = SPICOMMANDINITIAL();
    bufferR[MODE]                           = DISABLESPI();
    bufferR[SIZE_BUFFER_DISABLE_SPI-1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferR, SIZE_BUFFER_DISABLE_SPI);

    spiDisableSlave(_channel);

    return bufferR[PULSE_RATE_DISABLE];
}

int enableCommunication(uint8_t _channel, uint16_t _strokeLenght, uint8_t _pulseRate, uint16_t _accelarationTime, uint8_t _enableSoftStartStop) {
    spiEnableSlave(_channel);

    char bufferT [SIZE_BUFFER_ENABLE_SPI]   = {};
    bufferT[INITIAL]                        = SPICOMMANDINITIAL();
    bufferT[MODE]                           = ENABLESPI();
    bufferT[STROKE_LENGHT_HIGH]             = (_strokeLenght >> 8) & 0xFF;
    bufferT[STROKE_LENGHT_LOW]              = _strokeLenght & 0xFF;
    bufferT[PULSE_RATE_ENABLE]              = _pulseRate;
    bufferT[ACCELARATION_TIME_HIGH]         = (_accelarationTime>>8) & 0xFF;
    bufferT[ACCELARATION_TIME_LOW]          = _accelarationTime & 0xFF;
    bufferT[SOFT_START_STOP]                = _enableSoftStartStop;
    bufferT[SIZE_BUFFER_ENABLE_SPI-1]       = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ENABLE_SPI);

    spiDisableSlave(_channel);

    for (int i = 0; i < SIZE_BUFFER_DISABLE_SPI; i++) {
        if ( (bufferT[i] != SPIREADWITHSUCESS()) && (i != 0) )
            return -1; //  Error
    }

    return 1;
}

void sendUpdateCommand(int _channel, uint16_t _strokeLenght, uint8_t _pulseRate, uint16_t _accelarationTime, uint8_t _enableSoftStartStop)
{
    spiEnableSlave(_channel);

    char bufferT [SIZE_BUFFER_ENABLE_SPI]   = {};
    bufferT[INITIAL]                        = SPICOMMANDINITIAL();
    bufferT[MODE]                           = ENABLESPI();
    bufferT[STROKE_LENGHT_HIGH]             = (_strokeLenght >> 8) & 0xFF;
    bufferT[STROKE_LENGHT_LOW]              = _strokeLenght & 0xFF;
    bufferT[PULSE_RATE_ENABLE]              = _pulseRate;
    bufferT[ACCELARATION_TIME_HIGH]         = (_accelarationTime>>8) & 0xFF;
    bufferT[ACCELARATION_TIME_LOW]          = _accelarationTime & 0xFF;
    bufferT[SOFT_START_STOP]                = _enableSoftStartStop;
    bufferT[SIZE_BUFFER_ENABLE_SPI-1]       = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ENABLE_SPI);

    spiDisableSlave(_channel);
}

void sendBasicCommand(uint8_t _channel, uint8_t _command)
{
    spiEnableSlave(_channel);

    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = _command;
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(_channel);
}

void sendAdvancedCommand(uint8_t _channel, uint8_t _command, uint8_t _shift)
{
    spiEnableSlave(_channel);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = _command;
    bufferT[SHIFT]                      = _shift;
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(_channel);
}



