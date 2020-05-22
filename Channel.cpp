#include "Channel.h"

//  Constructor
Channel::Channel(uint8_t _numberChannel)
{
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

void Channel::setChannelNumber(uint8_t _channelNumber)
{
    channelNumber = _channelNumber;
}

void Channel::setAccelarationTime(uint16_t _accelarationTime)
{
    accelarationTime = _accelarationTime;
}

void Channel::setStrokeLenght(uint16_t _strokeLenght)
{
    strokeLenght = _strokeLenght;
}

void Channel::setPulseRate(uint8_t _pulseRate)
{
    pulseRate = _pulseRate;
}

void Channel::setSoftStartStop(uint8_t _softStartStop)
{
    softStartStop = _softStartStop;
}

void Channel::setPin(uint8_t _pin)
{
    pin = _pin;
}

//  "Get" Methods
string Channel::getModel(void)
{
    return  model;
}
string Channel::getCommunication(void)
{
    return  communication;
}
float Channel::getMaxCurrent(void)
{
    return  maxCurrent;
}

uint8_t Channel::getChannelNumber(void)
{
    return  channelNumber;
}
uint16_t Channel::getAccelarationTime(void)
{
    return  accelarationTime;
}
uint16_t Channel::getStrokeLenght(void)
{
    return  strokeLenght;
}
uint8_t Channel::getPulseRate(void)
{
    return  pulseRate;
}
uint8_t Channel::getSoftStartStop(void)
{
    return softStartStop;
}
uint8_t Channel::getPin(void)
{
    return pin;
}
