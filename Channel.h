#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <bcm2835.h>

//  Default values
#define ACCELARATION_RATE_DEFAULT   (uint8_t)   0
#define STROKE_LENGHT_DEFAULT       (uint16_t)  0
#define SOFT_START_STOP_DEFAULT     (uint8_t)   0
#define PULSE_RATE_DEFAULT          (uint8_t)   0xFF
#define COMMUNICATION_DEFAULT       (string)    "Disable"
#define MODEL_DEFAULT               (string)    ""
#define POSITION_DEFAULT            (uint16_t)  0
#define MAX_CURRENT_DEFAULT         (float)     0

//  Slave pins
#define PIN_CHANNEL1                (uint8_t)   RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL2                (uint8_t)   RPI_BPLUS_GPIO_J8_13
#define PIN_CHANNEL3                (uint8_t)   RPI_BPLUS_GPIO_J8_15
#define PIN_CHANNEL4                (uint8_t)   RPI_BPLUS_GPIO_J8_16
#define PIN_CHANNEL5                (uint8_t)   RPI_BPLUS_GPIO_J8_18



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
        uint8_t     accelarationRate; //  12mm/s²
        uint16_t    strokeLenght    ; //  40mm
        uint16_t    position        ; //  2mm


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
        void setAccelarationRate    (uint16_t _accelarationRate );
        void setStrokeLenght        (uint16_t _strokeLenght     );
        void setPosition            (uint16_t _position         );



        //  "Get" Methods

        string      getModel            (void);
        string      getCommunication    (void);
        float       getMaxCurrent       (void);
        uint8_t     getChannelNumber    (void);
        uint8_t     getPulseRate        (void);
        uint8_t     getSoftStartStop    (void);
        uint8_t     getPin              (void);
        uint8_t     getAccelarationRate (void);
        uint16_t    getStrokeLenght     (void);
        uint16_t    getPosition         (void);

};

#endif // CHANNEL_H
