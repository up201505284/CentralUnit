#include <bcm2835.h>
#include <QDebug>


//  Slave pins
#define PIN_CHANNEL1                    (uint8_t)   RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL2                    (uint8_t)   RPI_BPLUS_GPIO_J8_13
#define PIN_CHANNEL3                    (uint8_t)   RPI_BPLUS_GPIO_J8_15
#define PIN_CHANNEL4                    (uint8_t)   RPI_BPLUS_GPIO_J8_16
#define PIN_CHANNEL5                    (uint8_t)   RPI_BPLUS_GPIO_J8_18


//  SPI Commands
#define ENABLE()                        (uint8_t)   0x01
#define DISABLE()                       (uint8_t)   0x02
#define INITIALSETUP()                  (uint8_t)   0x03
#define SAFERESET()                     (uint8_t)   0x04
#define BASICEXTENDED()                 (uint8_t)   0x05
#define BASICREFRACTED()                (uint8_t)   0X06
#define STOP()                          (uint8_t)   0x07
#define ADVANCEDEXTENDED()              (uint8_t)   0x08
#define ADVANCEDREFRACTED()             (uint8_t)   0x09
#define GETPOSITIONLOW()                (uint8_t)   0x0A
#define GETPOSITIONHIGH()               (uint8_t)   0x0B
#define GETPULSERATE()                  (uint8_t)   0x0C
#define SENDPOSITIONLOW()               (uint8_t)   0x0D
#define SENDPOSITIONHIGH()              (uint8_t)   0x0E
#define SENDPULSERATE()                 (uint8_t)   0x0F
#define SENDSTROKELENGHTLOW()           (uint8_t)   0x10
#define SENDSTROKELENGHTHIGH()          (uint8_t)   0x11
#define SENDSOFTSTART()                 (uint8_t)   0x12
#define SENDACCELARATIONRATE()          (uint8_t)   0x13

//  Auc Commands
#define START()                         (uint8_t)   0xFF
#define END()                           (uint8_t)   0XFE
#define READWITHSUCESS()                (uint8_t)   0XFD

//  SPI Modes
#define COMMUNICATION()                 (uint8_t)   0xFC
#define BASIC()                         (uint8_t)   0xFB
#define ADVANCED()                      (uint8_t)   0xFA
#define UPDATE()                        (uint8_t)   0xF9
#define SETUP()                         (uint8_t)   0xF8


//  SPI Masks
#define MODE                            (uint8_t)   1
#define CMD                             (uint8_t)   2
#define DATA                            (uint8_t)   3
#define SIZE_BUFFER_COMMUNCATION_MODE   (uint8_t)   4
#define SIZE_BUFFER_SETUP_MODE          (uint8_t)   5
#define SIZE_BUFFER_UPDATE_MODE         (uint8_t)   4
#define SIZE_BUFFER_BASIC_MODE          (uint8_t)   4
#define SIZE_BUFFER_ADVANCED_MODE       (uint8_t)   5




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

bool spiCmdCommunication(uint8_t _channel, uint8_t _cmd) {
    spiEnableSlave(_channel);

    char buffer [SIZE_BUFFER_COMMUNCATION_MODE] = {};
    buffer[0]                                   = START();
    buffer[MODE]                                = COMMUNICATION();
    buffer[CMD]                                 = _cmd;
    buffer[SIZE_BUFFER_COMMUNCATION_MODE-1]     = END();

    bcm2835_spi_transfern(buffer, SIZE_BUFFER_COMMUNCATION_MODE);

    spiDisableSlave(_channel);
    for (int i=MODE; i<SIZE_BUFFER_COMMUNCATION_MODE; i++){
        if (buffer[i] != READWITHSUCESS())
            return false;
    }

    return  true;
}


bool spiCmdUpdate(uint8_t _channel, uint8_t _cmd, uint8_t* data) {
    spiEnableSlave(_channel);

    char buffer [SIZE_BUFFER_UPDATE_MODE]   = {};
    buffer[0]                               = START();
    buffer[MODE]                            = UPDATE();
    buffer[CMD]                             = _cmd;
    buffer[SIZE_BUFFER_UPDATE_MODE-1]       = END();

    bcm2835_spi_transfern(buffer, SIZE_BUFFER_UPDATE_MODE);

    spiDisableSlave(_channel);

    for (int i=MODE; i<SIZE_BUFFER_UPDATE_MODE-1; i++){
        if (buffer[i] != READWITHSUCESS())
            return false;
    }

    *data = buffer[SIZE_BUFFER_UPDATE_MODE-1];

    return  true;

}

bool spiCmdBasic(uint8_t _channel, uint8_t _cmd)
{
    spiEnableSlave(_channel);

    char buffer [SIZE_BUFFER_BASIC_MODE]        = {};
    buffer[0]                                   = START();
    buffer[MODE]                                = BASIC();
    buffer[CMD]                                 = _cmd;
    buffer[SIZE_BUFFER_BASIC_MODE-1]            = END();

    bcm2835_spi_transfern(buffer, SIZE_BUFFER_BASIC_MODE);

    spiDisableSlave(_channel);

    for (int i=MODE; i<SIZE_BUFFER_BASIC_MODE; i++){
        if (buffer[i] != READWITHSUCESS())
            return false;
    }

    return  true;
}

bool spiCmdAdvanced(uint8_t _channel, uint8_t _cmd, uint8_t _shift)
{
    spiEnableSlave(_channel);

    char buffer [SIZE_BUFFER_ADVANCED_MODE]     = {};
    buffer[0]                                   = START();
    buffer[MODE]                                = ADVANCED();
    buffer[CMD]                                 = _cmd;
    buffer[DATA]                                = _shift;
    buffer[SIZE_BUFFER_ADVANCED_MODE-1]     = END();

    bcm2835_spi_transfern(buffer, SIZE_BUFFER_ADVANCED_MODE);

    spiDisableSlave(_channel);

    for (int i=MODE; i<SIZE_BUFFER_ADVANCED_MODE; i++){
        if (buffer[i] != READWITHSUCESS())
            return false;
    }

    return  true;
}

bool spiCmdSetup(uint8_t _channel, uint8_t _cmd, uint8_t _data)
{
    spiEnableSlave(_channel);

    char buffer [SIZE_BUFFER_SETUP_MODE]        = {};
    buffer[0]                                   = START();
    buffer[MODE]                                = SETUP();
    buffer[CMD]                                 = _cmd;
    buffer[DATA]                                = _data;
    buffer[SIZE_BUFFER_SETUP_MODE-1]            = END();

    bcm2835_spi_transfern(buffer, SIZE_BUFFER_SETUP_MODE);

    spiDisableSlave(_channel);

    for (int i=MODE; i<SIZE_BUFFER_SETUP_MODE; i++){
        if (buffer[i] != READWITHSUCESS())
            return false;
    }

    return  true;
}



