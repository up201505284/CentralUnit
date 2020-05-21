#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <bcm2835.h>

//  Default values
#define ACCELARATION_TIME_DEFAULT   40
#define ACCELARATION_RATE_DEFAULT   2
#define MAX_CURRENT_DEFAULT         15
#define STROKE_LENGHT_DEFAULT       155
#define MODEL_DEFAULT               ""
#define PULSE_RATE_DEFAULT          150
#define COMMUNICATION_DEFAULT       "Disable"
#define SOFT_START_STOP_DEFAULT     0

//  Slave pins
#define PIN_CHANNEL1                RPI_BPLUS_GPIO_J8_11
#define PIN_CHANNEL2                RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL3                RPI_BPLUS_GPIO_J8_13
#define PIN_CHANNEL4                RPI_BPLUS_GPIO_J8_15
#define PIN_CHANNEL5                RPI_BPLUS_GPIO_J8_16


using namespace std;


class Channel
{
    private:
        string  model           ; //  TA31
        string  communication   ; //  Enable/Disable
        float   maxCurrent      ; //  4.5A
        int     accelarationRate; //  4mm/s²
        int     channelNumber   ; //  2
        int     accelarationTime; //  1400s
        int     strokeLenght    ; //  40mm
        int     pulseRate       ; //  123imp/mm
        int     softStartStop   ; //  Enable/Disable
        int     pin             ; //  PIN_CHANNEL1


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
