#ifndef CHANNEL_H
#define CHANNEL_H


#include <iostream>
#include <bcm2835.h>

using namespace std;

//  Default values
#define ACCELARATION_TIME_DEFAULT   140
#define ACCELARATION_RATE_DEFAULT   2.1
#define MAX_CURRENT_DEFAULT         0
#define STROKE_LENGHT_DEFAULT       0
#define MODEL_DEFAULT               ""
#define PULSE_RATE_DEFAULT          0
#define COMMUNICATION_DEFAULT       "Disable"
#define SOFT_START_STOP_DEFAULT     0
#define PIN_DEFAULT                 0

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

class Channel
{
private:
    string  model           ; //  TA31
    string  communication   ; //  Enable/Disable
    float   maxCurrent      ; //  4.5A
    int   accelarationRate; //  4.1mm/s²
    int     channelNumber   ; //  2
    int     accelarationTime; //  1400s
    int     strokeLenght    ; //  40mm
    int     pulseRate       ; //  123imp/mm
    int     softStartStop   ; //  Enable/Disable
    int     pin             ; // PIN_CHANNEL1


public:
    //  Constructor
    Channel(int _numberChannel);
    //  Destructor
    ~Channel();

    //  "Set" Methods
    void setModel           (string _model          );
    void setCommunication   (string _communication  );
    void setMaxCurrent      (float _maxCurrent      );
    void setAccelarationRate(int _accelarationRate);
    void setChannelNumber   (int _channelNumber     );
    void setAccelarationTime(int _accelarationTime  );
    void setStrokeLenght    (int _strokeLenght      );
    void setPulseRate       (int _pulseRate         );
    void setSoftStartStop   (int _softStartStop     );
    void setPin             (int _pin               );


    //  "Get" Methods

    string  getModel            (void);
    string  getCommunication    (void);
    float   getMaxCurrent       (void);
    int     getAccelarationRate (void);
    int     getChannelNumber    (void);
    int     getAccelarationTime (void);
    int     getStrokeLenght     (void);
    int     getPulseRate        (void);
    int     getSoftStartStop    (void);
    int     getPin              (void);


};

#endif // CHANNEL_H
