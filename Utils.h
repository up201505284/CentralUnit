#include <bcm2835.h>
#include <QDebug>


//  SPI Commands TX (from central init to control unit)
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


//  Slave pins
#define PIN_CHANNEL1                RPI_BPLUS_GPIO_J8_11
#define PIN_CHANNEL2                RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL3                RPI_BPLUS_GPIO_J8_24
#define PIN_CHANNEL4                RPI_BPLUS_GPIO_J8_26
#define PIN_CHANNEL5                RPI_BPLUS_GPIO_J8_36

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
    char bufferR [3] = {SPICOMMANDINITIAL(), DISABLESPI(), SPICOMMANDFINISHED()};
    bcm2835_spi_transfern(bufferR, 3);
    spiDisableSlave(channel);
    return bufferR[2];
}

int enableCommunication(int channel, uint8_t strokeLenght, uint8_t pulseRate, uint8_t accelarationRate, uint8_t accelarationTime, uint8_t enableSoftStartStop) {
    spiEnableSlave(channel);
    char bufferT [8] ={SPICOMMANDINITIAL(), ENABLESPI(), strokeLenght, pulseRate, accelarationRate, accelarationTime, enableSoftStartStop, SPICOMMANDFINISHED()};
    bcm2835_spi_transfern(bufferT, 8);
    spiDisableSlave(channel);
    for (int i=0; i<8;i++) {
        if (bufferT[i] != SPIREADWITHSUCESS())
            return -1;
    }
    return 1;
}

