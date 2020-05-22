#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <bcm2835.h>

//  Default values
#define ACCELARATION_TIME_DEFAULT   (uint16_t)  0
#define STROKE_LENGHT_DEFAULT       (uint16_t)  0
#define SOFT_START_STOP_DEFAULT     (uint8_t)   0
#define PULSE_RATE_DEFAULT          (uint8_t)   0
#define COMMUNICATION_DEFAULT       (string)    "Disable"
#define MODEL_DEFAULT               (string)    ""
#define MAX_CURRENT_DEFAULT         (float)     0

//  Slave pins
#define PIN_CHANNEL1                (uint8_t)   RPI_BPLUS_GPIO_J8_11
#define PIN_CHANNEL2                (uint8_t)   RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL3                (uint8_t)   RPI_BPLUS_GPIO_J8_13
#define PIN_CHANNEL4                (uint8_t)   RPI_BPLUS_GPIO_J8_15
#define PIN_CHANNEL5                (uint8_t)   RPI_BPLUS_GPIO_J8_16




using namespace std;


class Channel
{
    private:
        string      model           ; //  TA31
        string      communication   ; //  Disable
        float       maxCurrent      ; //  4.5A
        uint8_t     pulseRate       ; //  123imp/mm
        uint8_t     softStartStop   ; //  0
        uint8_t     pin             ; //  PIN_CHANNEL1
        uint8_t     channelNumber   ; //  2
        uint16_t    accelarationTime; //  1400s
        uint16_t    strokeLenght    ; //  40mm


    public:
        //  Constructor
        Channel(uint8_t _numberChannel);
        //  Destructor
        ~Channel();

        //  "Set" Methods
        void setModel               (string _model              );
        void setCommunication       (string _communication      );
        void setMaxCurrent          (float _maxCurrent          );
        void setChannelNumber       (uint8_t _channelNumber     );
        void setPulseRate           (uint8_t _pulseRate         );
        void setSoftStartStop       (uint8_t _softStartStop     );
        void setPin                 (uint8_t _pin               );
        void setAccelarationTime    (uint16_t _accelarationTime );
        void setStrokeLenght        (uint16_t _strokeLenght     );



        //  "Get" Methods

        string      getModel            (void);
        string      getCommunication    (void);
        float       getMaxCurrent       (void);
        uint8_t     getChannelNumber    (void);
        uint8_t     getPulseRate        (void);
        uint8_t     getSoftStartStop    (void);
        uint8_t     getPin              (void);
        uint16_t    getAccelarationTime (void);
        uint16_t    getStrokeLenght     (void);

};

#endif // CHANNEL_H
