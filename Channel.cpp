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
    setCommunication    (COMMUNICATION_DEFAULT      );
    setSoftStartStop    (SOFT_START_STOP_DEFAULT    );

    if (_numberChannel == 1)
       setPin(PIN_CHANNEL1);
    else if (_numberChannel == 2)
        setPin(PIN_CHANNEL2);
    else if (_numberChannel == 3)
        setPin(PIN_CHANNEL3);
    else if (_numberChannel == 4)
        setPin(PIN_CHANNEL4);
    else if (_numberChannel == 5)
        setPin(PIN_CHANNEL5);

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
void Channel::setCommunication(string _communication)
{
    communication = _communication;
}
void Channel::setMaxCurrent(float _maxCurrent)
{
    maxCurrent = _maxCurrent;
}
void Channel::setAccelarationRate(int _accelarationRate)
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
void Channel::setSoftStartStop(int _softStartStop)
{
    softStartStop = _softStartStop;
}
void Channel::setPin(int _pin)
{
    pin = _pin;
}

//  "Get" Methods
string  Channel::getModel(void)
{
    return  model;
}
string Channel::getCommunication(void)
{
    return  communication;
}
float   Channel::getMaxCurrent(void)
{
    return  maxCurrent;
}
int   Channel::getAccelarationRate(void)
{
    return  accelarationRate;
}
int     Channel::getChannelNumber(void)
{
    return  channelNumber;
}
int     Channel::getAccelarationTime(void)
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
int Channel::getSoftStartStop(void)
{
    return softStartStop;
}
int Channel::getPin(void)
{
    return pin;
}
