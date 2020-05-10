#include "Channel.h"

//  Constructor
Channel::Channel(int _numberChannel)
{
    setAccelarationRate (ACCELARATION_RATE_DEFAULT  );
    setAccelarationTime (ACCELARATION_TIME_DEFAULT  );
    setStrokeLenght     (STROKE_LENGHT_DEFAULT      );
    setMaxCurrent       (MAX_CURRENT_DEFAULT        );
    setModel            (MODEL_DEFAULT              );
    setChannelNumber    (_numberChannel             );
    setPulseRate        (PULSE_RATE_DEFAULT         );
    return;
}

//  Destructor
Channel::~Channel()
{
    return;
}

//  "Set" Methods
void Channel::setModel(string _model)
{
    model = _model;
}
void Channel::setMaxCurrent(float _maxCurrent)
{
    maxCurrent = _maxCurrent;
}
void Channel::setAccelarationRate(float _accelarationRate)
{
    accelarationRate = _accelarationRate;
}
void Channel::setChannelNumber(int _channelNumber)
{
    channelNumber = _channelNumber;
}
void Channel::setAccelarationTime(int _accelarationTime)
{
    accelarationTime = _accelarationTime;
}
void Channel::setStrokeLenght(int _strokeLenght)
{
    strokeLenght = _strokeLenght;
}
void Channel::setPulseRate(int _pulseRate)
{
    pulseRate = _pulseRate;
}

//  "Get" Methods

string  Channel::getModel            (void)
{
    return  model;
}
float   Channel::getMaxCurrent       (void)
{
    return  maxCurrent;
}
float   Channel::getAccelarationRate (void)
{
    return  accelarationRate;
}
int     Channel::getChannelNumber   (void)
{
    return  channelNumber;
}
int     Channel::getAccelarationTime (void)
{
    return  accelarationTime;
}
int Channel::getStrokeLenght(void)
{
    return  strokeLenght;
}
int Channel::getPulseRate(void)
{
    return  pulseRate;
}
