#ifndef CHANNEL_H
#define CHANNEL_H


#include <iostream>

using namespace std;

#define ACCELARATION_TIME_DEFAULT   140
#define ACCELARATION_RATE_DEFAULT   2.1
#define MAX_CURRENT_DEFAULT         0
#define STROKE_LENGHT_DEFAULT       0
#define MODEL_DEFAULT               ""
#define PULSE_RATE_DEFAULT          0

class Channel
{
private:
    string  model           ; //  TA31
    float   maxCurrent      ; //  4.5A
    float   accelarationRate; //  4.1mm/s²
    int     channelNumber   ; //  2
    int     accelarationTime; //  1400s
    int     strokeLenght    ; //  40mm
    int     pulseRate       ; //  123imp/mm


public:
    //  Constructor
    Channel(int _numberChannel);
    //  Destructor
    ~Channel();

    //  "Set" Methods
    void setModel           (string _model          );
    void setMaxCurrent      (float _maxCurrent      );
    void setAccelarationRate(float _accelarationRate);
    void setChannelNumber   (int _channelNumber     );
    void setAccelarationTime(int _accelarationTime  );
    void setStrokeLenght    (int _strokeLenght      );
    void setPulseRate       (int _pulseRate         );


    //  "Get" Methods

    string  getModel            (void);
    float   getMaxCurrent       (void);
    float   getAccelarationRate (void);
    int     getChannelNumber    (void);
    int     getAccelarationTime (void);
    int     getStrokeLenght     (void);
    int     getPulseRate        (void);

};

#endif // CHANNEL_H
