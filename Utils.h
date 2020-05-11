#include <bcm2835.h>

//  Default values
#define ACCELARATION_TIME_DEFAULT   140
#define ACCELARATION_RATE_DEFAULT   2.1
#define MAX_CURRENT_DEFAULT         0
#define STROKE_LENGHT_DEFAULT       0
#define MODEL_DEFAULT               ""
#define PULSE_RATE_DEFAULT          0
#define COMMUNICATION_DEFAULT       "Disable"

//  Slave pins
#define PIN_CHANNEL1                RPI_BPLUS_GPIO_J8_11
#define PIN_CHANNEL2                RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL3                RPI_BPLUS_GPIO_J8_24
#define PIN_CHANNEL4                RPI_BPLUS_GPIO_J8_26
#define PIN_CHANNEL5                RPI_BPLUS_GPIO_J8_36

//  SPI Commands TX (from central init to control unit)
#define DISCONNECTED()              0x00
#define INITIALSETUP()              0x01
#define SAFERESET()                 0x02
#define BASICEXTENDED()             0x03
#define BASICREFRACTED()            0X04
#define ADVANCEDEXTENDED()          0x05
#define ADVANCEDREFRACTED()         0x06
#define STOP()                      0x07
#define SUCESS()                    0x08
#define SENDING_SHIFT()             0x09
#define SETUPCOMMUNICATION()        0x10
#define SENDINGSTROKELENGHT()       0x11
#define SENDINGPULSERATE()          0x12
#define SENDINGSOFTSTARTSTOP()      0x13
#define SENDINGACCELRATIONRATE()    0x14
#define SENDINGACCELRATIONTIME()    0x15
#define HELLO()                     0x16

//  SPI Communication
void    spiInt          (void           );
void    spiEnableSlave  (int _channel   );
void    spiDisableSlave (int _channel   );
void    spiSend         (uint8_t data   );
uint8_t spiRead         (void           );
void    spiClose        (void           );

//  SPI Commands
int     checkCommunication  (int channel);
int    setupCommunication  (int channel);

void spiInt(void)
{
    if (bcm2835_init()) {
        bcm2835_gpio_fsel(PIN_CHANNEL1, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL2, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL3, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL4, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(PIN_CHANNEL5, BCM2835_GPIO_FSEL_OUTP);

        bcm2835_gpio_write(PIN_CHANNEL1, HIGH);
        bcm2835_gpio_write(PIN_CHANNEL2, HIGH);
        bcm2835_gpio_write(PIN_CHANNEL3, HIGH);
        bcm2835_gpio_write(PIN_CHANNEL4, HIGH);
        bcm2835_gpio_write(PIN_CHANNEL5, HIGH);

        if (bcm2835_spi_begin()) {
            bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST      );  // The default
            bcm2835_spi_setDataMode(BCM2835_SPI_MODE0                   );  // The default
            bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536 );  // cdiv = 65536; speed = 3.814 kHz
            bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);                    // Itself control
            bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS_NONE, LOW);

            bcm2835_gpio_fsel(PIN_CHANNEL1, BCM2835_GPIO_FSEL_OUTP);
            bcm2835_gpio_fsel(PIN_CHANNEL2, BCM2835_GPIO_FSEL_OUTP);
            bcm2835_gpio_fsel(PIN_CHANNEL3, BCM2835_GPIO_FSEL_OUTP);
            bcm2835_gpio_fsel(PIN_CHANNEL4, BCM2835_GPIO_FSEL_OUTP);
            bcm2835_gpio_fsel(PIN_CHANNEL5, BCM2835_GPIO_FSEL_OUTP);


            bcm2835_gpio_write(PIN_CHANNEL1, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL2, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL3, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL4, HIGH);
            bcm2835_gpio_write(PIN_CHANNEL5, HIGH);
        }
        /*else
            qDebug()<<"Error1";*/
    }
    /*else
        qDebug()<<"Error2";*/

}

void spiSend(uint8_t data)
{
    bcm2835_spi_write(data);
}
uint8_t spiRead(void)
{
    return bcm2835_spi_transfer(0xFF);
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

int checkCommunication(int channel) {
    spiEnableSlave(channel);
    spiSend(HELLO());
    if (spiRead() != HELLO()) {
        spiDisableSlave(channel);
        return -1; //  Error
    }
    else {
        spiDisableSlave(channel);
        return 1;
    }
}

int setupCommunication(int channel, uint8_t strokeLenght, uint8_t pulseRate, uint8_t accelarationRate, uint8_t accelarationTime, uint8_t enableSoftStartStop) {
    if (checkCommunication(channel) != 1)
        return -1;

    spiEnableSlave(channel);

    spiSend(SETUPCOMMUNICATION());
    if (spiRead() != SETUPCOMMUNICATION())
        return -2;

    spiSend(strokeLenght);
    if (spiRead() != strokeLenght)
        return  -3;

    spiSend(pulseRate);
    if (spiRead() != strokeLenght)
        return  -4;

    spiSend(accelarationRate);
    if (spiRead() != strokeLenght)
        return  -5;

    spiSend(accelarationTime);
    if (spiRead() != strokeLenght)
        return  -6;

    spiSend(enableSoftStartStop);
    if (spiRead() != strokeLenght)
        return  -6;

    spiSend(DISCONNECTED());
    if (spiRead() != strokeLenght)
        return  -7;

    spiDisableSlave(channel);
    return 1;
}
