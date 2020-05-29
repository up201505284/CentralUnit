#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Utils.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

/*  ------------ Basic Functions for mainwindow class   -----------------------------------------   */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!QFile::exists(JSON_FILE_NAME)) {
        initListOfActuators();
        updateJsonFile();
    }

    readJsonFile();
    spiInit();
    checkCommunications();
    setupTab();
}

MainWindow::~MainWindow()
{
    Update();
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getCommunication() == "Enable")
                spiCmdCommunication(_channel->getPin(), DISABLE());
    }

    updateJsonFile();
    spiClose();
    delete ui;
}

QList<Channel*> MainWindow::getListOfChannels(void)
{
    return  listOfChannels;
}

void MainWindow::setListOfChannels(QList<Channel*> _listOfChannels)
{
    listOfChannels = _listOfChannels;
}

void MainWindow::initListOfActuators(void)
{
    QList<Channel*> _listOfActuators;

    for (uint8_t i=0; i<MAX_NUMBER_CHANNELS; i++){
        Channel* _channel = new Channel(i+1);
        _listOfActuators.append(_channel);
    }

    setListOfChannels(_listOfActuators);
}

Channel* MainWindow::getChannel(uint8_t _channel)
{
    for (Channel* _ch: getListOfChannels()){
        if (_ch->getChannelNumber() == _channel)
            return  _ch;
    }

    return  NULL;
}


/*  ------------ Functions for json file   -----------------------------------------   */

void MainWindow::readJsonFile(void)
{
    QFile jsonFile(JSON_FILE_NAME);
    jsonFile.open(QFile::ReadOnly);
    QJsonArray _channelsJson = QJsonDocument().fromJson(jsonFile.readAll()).array();
    jsonFile.close();

    QList<Channel*> _listOfChannels;

    foreach (const QJsonValue & _channelJson, _channelsJson){
        Channel* _channel = new Channel ( (uint8_t) _channelJson.toObject().value("Channel Number"     ).toString().toUInt()       );
        _channel->setStrokeLenght       ( (uint16_t)_channelJson.toObject().value("Stroke Lenght"      ).toString().toUInt()       );
        _channel->setAccelarationRate   ( (uint16_t)_channelJson.toObject().value("Accelaration Rate"  ).toString().toUInt()       );
        _channel->setPulseRate          ( (uint8_t) _channelJson.toObject().value("Pulse Rate"         ).toString().toUInt()       );
        _channel->setPin                ( (uint8_t) _channelJson.toObject().value("Pin"                ).toString().toUInt()       );
        _channel->setPosition           ( (uint8_t) _channelJson.toObject().value("Position"           ).toString().toUInt()       );
        _channel->setMaxCurrent         ( (float)   _channelJson.toObject().value("Maximum Current"    ).toString().toDouble()     );
        _channel->setModel              ( (string)  _channelJson.toObject().value("Model"              ).toString().toStdString()  );
        _channel->setCommunication      ( (string)  _channelJson.toObject().value("Communication"      ).toString().toStdString()  );
        _listOfChannels.append(_channel);
    }

    setListOfChannels(_listOfChannels);
}

void MainWindow::updateJsonFile (void)
{
    QFile jsonFile(JSON_FILE_NAME);
    jsonFile.open(QFile::WriteOnly);
    QJsonArray channelsJson;

    for (Channel* _channel:getListOfChannels()){
        QJsonObject _channelJson;
        _channelJson.insert("Channel Number"    , QString::number       (_channel->getChannelNumber()   ));
        _channelJson.insert("Model"             , QString::fromStdString(_channel->getModel()           ));
        _channelJson.insert("Communication"     , QString::fromStdString(_channel->getCommunication()   ));
        _channelJson.insert("Stroke Lenght"     , QString::number       (_channel->getStrokeLenght()    ));
        _channelJson.insert("Maximum Current"   , QString::number       (_channel->getMaxCurrent()      ));
        _channelJson.insert("Accelaration Rate" , QString::number       (_channel->getAccelarationRate()));
        _channelJson.insert("Pulse Rate"        , QString::number       (_channel->getPulseRate()       ));
        _channelJson.insert("Pin"               , QString::number       (_channel->getPin()             ));
        _channelJson.insert("Position"          , QString::number       (_channel->getPosition()        ));
        channelsJson.push_back(_channelJson);
    }

    QJsonDocument recordsOfChannels(channelsJson);
    jsonFile.write(recordsOfChannels.toJson());
    jsonFile.close();
}

/*  ------------ Functions for SPI communication   -----------------------------------------   */
void MainWindow::checkCommunications (void){
    for (Channel* _ch: getListOfChannels()){
        if (spiCmdCommunication(_ch->getPin(), ENABLE())) {
            _ch->setCommunication("Enable");
            spiCmdSetup(_ch->getPin(), SENDSOFTSTART(), _ch->getSoftStartStop());
            spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTLOW(), _ch->getStrokeLenght() & 0xFF);
            spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTHIGH(), (_ch->getStrokeLenght() >> 8) & 0xFF);
            spiCmdSetup(_ch->getPin(), SENDACCELARATIONRATE(), _ch->getAccelarationRate());
            spiCmdSetup(_ch->getPin(), SENDPULSERATE(), _ch->getPulseRate());
            spiCmdSetup(_ch->getPin(), SENDPOSITIONLOW(), _ch->getPosition() & 0xFF);
            spiCmdSetup(_ch->getPin(), SENDPOSITIONHIGH(), (_ch->getPosition() >> 8) & 0xFF);
        }
        else
            _ch->setCommunication("Disable");
    }
}

void MainWindow::restartCommunication(uint8_t _channel)
{
    for (Channel* _ch: getListOfChannels()){
        if (_ch->getCommunication() == "Enable") {
            uint8_t _positionLow;
            uint8_t _positionHigh;
            uint8_t _pulseRate;
            if (spiCmdUpdate(_ch->getPin(), GETPOSITIONLOW(), &_positionLow)) {
                if (spiCmdUpdate(_ch->getPin(), GETPOSITIONHIGH(), &_positionHigh)) {
                    if (spiCmdUpdate(_ch->getPin(), GETPULSERATE(), &_pulseRate)) {
                        _ch->setPosition((uint16_t)((_positionLow & 0xFF) | ((_positionHigh << 8) & 0xFF00)));
                        _ch->setPulseRate(_pulseRate);
                    }
                }
            }
        }
    }

    for (Channel* _ch: getListOfChannels()){
        if (spiCmdCommunication(_ch->getPin(), DISABLE()))
            _ch->setCommunication("Disable");
        else
            _ch->setCommunication("Disable");
    }

    Channel* _ch = getChannel(_channel);
    if (spiCmdCommunication(_ch->getPin(), ENABLE()))
        _ch->setCommunication("Enable");
    else
        _ch->setCommunication("Disable");

    spiCmdSetup(_ch->getPin(), SENDSOFTSTART(), _ch->getSoftStartStop());
    spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTLOW(), _ch->getStrokeLenght() & 0xFF);
    spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTHIGH(), (_ch->getStrokeLenght() >> 8) & 0xFF);
    spiCmdSetup(_ch->getPin(), SENDACCELARATIONRATE(), _ch->getAccelarationRate());
    spiCmdSetup(_ch->getPin(), SENDPULSERATE(), _ch->getPulseRate());
    spiCmdSetup(_ch->getPin(), SENDPOSITIONLOW(), _ch->getPosition() & 0xFF);
    spiCmdSetup(_ch->getPin(), SENDPOSITIONHIGH(), (_ch->getPosition() >> 8) & 0xFF);




}

/*  ------------ Functions for Widgets   -----------------------------------------   */

void MainWindow::setupTab(void)
{
    for(Channel* _channel : getListOfChannels()){

        if(_channel->getChannelNumber() == 1){
            ui->lineEditModel_channel1                  ->setText(QString::fromStdString(_channel->getModel()           ));
            ui->lineEditMaxCurrent_channel1             ->setText(QString::number       (_channel->getMaxCurrent()      ));
            ui->lineEditStrokeLenght_channel1           ->setText(QString::number       (_channel->getStrokeLenght()    ));
            ui->linePulseRate_channel1                  ->setText(QString::number       (_channel->getPulseRate()       ));
            ui->linePosition_channel1                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel1       ->setText(QString::number       (_channel->getAccelarationRate()));

            ui->horizontalSliderExtended_channel1       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel1      ->setRange(0, _channel->getStrokeLenght());
            ui->labelRefractedAdvanced_channel1         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel1          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            ui->lineEditAccelarationRate_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel1  ->setEnabled(ui->checkBoxPwm_channel1->isChecked());

            ui->labelRefractedAdvanced_channel1         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel1          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));


            if (_channel->getCommunication() == "Enable")
                ui->channel1->setEnabled(true);
            else
                ui->channel1->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 2){
            ui->lineEditModel_channel2                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel2             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel2           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel2                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->linePosition_channel2                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel2       ->setText(QString::number       (_channel->getAccelarationRate() ));

            ui->horizontalSliderExtended_channel2       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel2      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel2  ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->labelRefractedAdvanced_channel2        ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel2          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            ui->labelRefractedAdvanced_channel2         ->setText(QString::number(ui->horizontalSliderRefracted_channel2->value()));
            ui->labelExtendedAdvanced_channel2          ->setText(QString::number(ui->horizontalSliderExtended_channel2 ->value()));


            if (_channel->getCommunication() == "Enable")
                ui->channel2->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel2->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 3){
            ui->lineEditModel_channel3                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel3             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel3           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel3                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->linePosition_channel3                  ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel3       ->setText(QString::number       (_channel->getAccelarationRate() ));

            ui->horizontalSliderExtended_channel3       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel3      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel3  ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->labelRefractedAdvanced_channel3         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel3          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            ui->labelRefractedAdvanced_channel3         ->setText(QString::number(ui->horizontalSliderRefracted_channel3->value()));
            ui->labelExtendedAdvanced_channel3          ->setText(QString::number(ui->horizontalSliderExtended_channel3 ->value()));


            if (_channel->getCommunication() == "Enable")
                ui->channel3->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel3->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 4){
            ui->lineEditModel_channel4                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel4             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel4           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel4                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->linePosition_channel4                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel4       ->setText(QString::number       (_channel->getAccelarationRate() ));

            ui->horizontalSliderExtended_channel4       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel4      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel4  ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->labelRefractedAdvanced_channel4         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel4          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            ui->labelRefractedAdvanced_channel4         ->setText(QString::number(ui->horizontalSliderRefracted_channel4->value()));
            ui->labelExtendedAdvanced_channel4          ->setText(QString::number(ui->horizontalSliderExtended_channel4 ->value()));


            if (_channel->getCommunication() == "Enable")
                ui->channel4->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel4->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 5){
            ui->lineEditModel_channel5                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel5             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel5           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel5                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->linePosition_channel5                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel5       ->setText(QString::number       (_channel->getAccelarationRate() ));

            ui->horizontalSliderExtended_channel5       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel5      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel5  ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->labelRefractedAdvanced_channel5         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel5          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            if (_channel->getCommunication() == "Enable")
                ui->channel5->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel5->setEnabled(false);
        }
    }
}


/*  ------------ Functions for soft start button   -----------------------------------------   */
void MainWindow::on_pushButtonUpdateStartStopSoft_channel1_clicked()
{
    uint8_t _accelarationRate = (uint8_t) ui->lineEditAccelarationRate_channel1->text().toUInt();
    ButtonUpdateSoftStartStop(CHANNEL1, _accelarationRate);
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel2_clicked()
{
    uint8_t _accelarationRate = (uint8_t) ui->lineEditAccelarationRate_channel2->text().toUInt();
    ButtonUpdateSoftStartStop(CHANNEL2, _accelarationRate);
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel3_clicked()
{
    uint8_t _accelarationRate = (uint8_t) ui->lineEditAccelarationRate_channel3->text().toUInt();
    ButtonUpdateSoftStartStop(CHANNEL3, _accelarationRate);
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel4_clicked()
{
    uint8_t _accelarationRate = (uint8_t) ui->lineEditAccelarationRate_channel4->text().toUInt();
    ButtonUpdateSoftStartStop(CHANNEL4, _accelarationRate);
}
void MainWindow::on_pushButtonUpdateStartStopSoft_channel5_clicked()
{
    uint8_t _accelarationRate = (uint8_t) ui->lineEditAccelarationRate_channel5->text().toUInt();
    ButtonUpdateSoftStartStop(CHANNEL5, _accelarationRate);
}

/*  ------------ Functions for linear actuataor specifications button   -----------------------------------------   */
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel1_clicked()
{
    uint16_t _strokeLenght = (uint16_t) ui->lineEditStrokeLenght_channel1->text().toUInt();
    string _model = ui->lineEditModel_channel1->text().toStdString();
    float _maxCurrent = ui->lineEditMaxCurrent_channel1->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL1, _strokeLenght, _model, _maxCurrent);
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel2_clicked()
{
    uint16_t _strokeLenght = (uint16_t) ui->lineEditStrokeLenght_channel2->text().toUInt();
    string _model = ui->lineEditModel_channel2->text().toStdString();
    float _maxCurrent = ui->lineEditMaxCurrent_channel2->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL2, _strokeLenght, _model, _maxCurrent);
}
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel3_clicked()
{
    uint16_t _strokeLenght = (uint16_t) ui->lineEditStrokeLenght_channel3->text().toUInt();
    string _model = ui->lineEditModel_channel3->text().toStdString();
    float _maxCurrent = ui->lineEditMaxCurrent_channel3->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL3, _strokeLenght, _model, _maxCurrent);
}
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel4_clicked()
{
    uint16_t _strokeLenght = (uint16_t) ui->lineEditStrokeLenght_channel4->text().toUInt();
    string _model = ui->lineEditModel_channel4->text().toStdString();
    float _maxCurrent = ui->lineEditMaxCurrent_channel4->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL4, _strokeLenght, _model, _maxCurrent);
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel5_clicked()
{
    uint16_t _strokeLenght = (uint16_t) ui->lineEditStrokeLenght_channel5->text().toUInt();
    string _model = ui->lineEditModel_channel5->text().toStdString();
    float _maxCurrent = ui->lineEditMaxCurrent_channel5->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL5, _strokeLenght, _model, _maxCurrent);
}

/*  ------------ Functions for horizontal sliders   -----------------------------------------   */

void MainWindow::on_horizontalSliderExtended_channel1_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel1  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel1_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel1  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel2_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel2  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel2_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel2  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel3_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel3  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel3_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel3  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel4_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel4  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel4_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel4  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel5_valueChanged(int value)
{
     ui->labelExtendedAdvanced_channel5  ->setText(QString::number(value));
}

void MainWindow::on_horizontalSliderRefracted_channel5_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel5  ->setText(QString::number(value));

}

/*  ------------ Functions basic buttons (Initial Setup, Safe Reset, Extended, Refracted, Stop)   -----------------------------------------   */


void MainWindow::on_pushButtonBasicExtended_channel1_clicked()
{

    spiCmdBasic(PIN_CHANNEL1, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicExtended_channel2_clicked()
{

    spiCmdBasic(PIN_CHANNEL2, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicExtended_channel3_clicked()
{

    spiCmdBasic(PIN_CHANNEL3, BASICEXTENDED());

}

void MainWindow::on_pushButtonBasicExtended_channel4_clicked()
{

    spiCmdBasic(PIN_CHANNEL4, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicExtended_channel5_clicked()
{

    spiCmdBasic(PIN_CHANNEL5, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicRefracted_channel1_clicked()
{

    spiCmdBasic(PIN_CHANNEL1, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel2_clicked()
{

    spiCmdBasic(PIN_CHANNEL2, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel3_clicked()
{

    spiCmdBasic(PIN_CHANNEL3, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel4_clicked()
{

    spiCmdBasic(PIN_CHANNEL4, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel5_clicked()
{

    spiCmdBasic(PIN_CHANNEL5, BASICREFRACTED());
}

void MainWindow::on_pushButtonStop_channel1_clicked()
{

    spiCmdBasic(PIN_CHANNEL1, STOP());
}

void MainWindow::on_pushButtonStop_channel2_clicked()
{

    spiCmdBasic(PIN_CHANNEL2, STOP());
}

void MainWindow::on_pushButtonStop_channel3_clicked()
{

    spiCmdBasic(PIN_CHANNEL3, STOP());
}

void MainWindow::on_pushButtonStop_channel4_clicked()
{

    spiCmdBasic(PIN_CHANNEL4, STOP());
}

void MainWindow::on_pushButtonStop_channel5_clicked()
{

    spiCmdBasic(PIN_CHANNEL5, STOP());
}

void MainWindow::on_pushButtonSafeReset_channel1_clicked()
{

    spiCmdBasic(PIN_CHANNEL1, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel2_clicked()
{

    spiCmdBasic(PIN_CHANNEL2, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel3_clicked()
{

    spiCmdBasic(PIN_CHANNEL3, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel4_clicked()
{

    spiCmdBasic(PIN_CHANNEL4, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel5_clicked()
{

    spiCmdBasic(PIN_CHANNEL5, SAFERESET());
}

void MainWindow::on_pushButtonInitialSetup_channel1_clicked()
{

    spiCmdBasic(PIN_CHANNEL1, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel2_clicked()
{

    spiCmdBasic(PIN_CHANNEL2, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel3_clicked()
{

    spiCmdBasic(PIN_CHANNEL3, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel4_clicked()
{

    spiCmdBasic(PIN_CHANNEL4, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel5_clicked()
{

    spiCmdBasic(PIN_CHANNEL5, INITIALSETUP());
}

/*  ------------ Functions advanced buttons (Extended, Refracted)   -----------------------------------------   */

void MainWindow::on_pushButtonExtended_channel1_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL1, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel1->value());
}

void MainWindow::on_pushButtonExtended_channel2_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL2, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel2->value());
}

void MainWindow::on_pushButtonExtended_channel3_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL3, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel3->value());
}
void MainWindow::on_pushButtonExtended_channel4_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL4, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel4->value());
}

void MainWindow::on_pushButtonExtended_channel5_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL5, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel5->value());
}


void MainWindow::on_pushButtonRefracted_channel1_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL1, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel1->value());
}

void MainWindow::on_pushButtonRefracted_channel2_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL2, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel2->value());
}

void MainWindow::on_pushButtonRefracted_channel3_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL3, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel3->value());
}

void MainWindow::on_pushButtonRefracted_channel4_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL4, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel4->value());
}

void MainWindow::on_pushButtonRefracted_channel5_clicked()
{

    spiCmdAdvanced(PIN_CHANNEL5, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel5->value());
}

void MainWindow::on_checkBoxPwm_channel1_stateChanged(int arg1)
{

    uint8_t _softStartStop = 0;
    if (arg1 == Qt::Unchecked)
        _softStartStop = 0;
    else if (arg1 == Qt::Checked)
        _softStartStop = 1;

    CheckBoxSoftStartStop(CHANNEL1, _softStartStop);
}

void MainWindow::on_checkBoxPwm_channel2_stateChanged(int arg1)
{

    uint8_t _softStartStop = 0;
    if (arg1 == Qt::Unchecked)
        _softStartStop = 0;
    else if (arg1 == Qt::Checked)
        _softStartStop = 1;

    CheckBoxSoftStartStop(CHANNEL2, _softStartStop);
}

void MainWindow::on_checkBoxPwm_channel3_stateChanged(int arg1)
{

    uint8_t _softStartStop = 0;
    if (arg1 == Qt::Unchecked)
        _softStartStop = 0;
    else if (arg1 == Qt::Checked)
        _softStartStop = 1;

    CheckBoxSoftStartStop(CHANNEL3, _softStartStop);
}

void MainWindow::on_checkBoxPwm_channel4_stateChanged(int arg1)
{

    uint8_t _softStartStop = 0;
    if (arg1 == Qt::Unchecked)
        _softStartStop = 0;
    else if (arg1 == Qt::Checked)
        _softStartStop = 1;

    CheckBoxSoftStartStop(CHANNEL4, _softStartStop);
}

void MainWindow::on_checkBoxPwm_channel5_stateChanged(int arg1)
{
    uint8_t _softStartStop = 0;
    if (arg1 == Qt::Unchecked)
        _softStartStop = 0;
    else if (arg1 == Qt::Checked)
        _softStartStop = 1;

    CheckBoxSoftStartStop(CHANNEL5, _softStartStop);
}

void MainWindow::on_tabChannel_currentChanged(int index)
{

    if (index == TAB_CHANNEL1)
        restartCommunication(CHANNEL1);
    else if (index == TAB_CHANNEL2)
        restartCommunication(CHANNEL2);
    else if (index == TAB_CHANNEL3)
        restartCommunication(CHANNEL3);
    else if (index == TAB_CHANNEL4)
        restartCommunication(CHANNEL4);
    else if (index == TAB_CHANNEL5)
        restartCommunication(CHANNEL5);

    updateJsonFile();
    setupTab();
}

void MainWindow::ButtonUpdateLinearActuator(uint8_t _ch, uint16_t _strokeLenght, string _model, float _maxCurrent)
{
    Channel* _channel = getChannel(_ch);
    _channel->setModel(_model);
    _channel->setMaxCurrent(_maxCurrent);
    if (spiCmdSetup(_channel->getPin(), SENDSTROKELENGHTLOW(), (uint8_t) (_strokeLenght & 0xFF))) {
        if (spiCmdSetup(_channel->getPin(), SENDSTROKELENGHTHIGH(), (uint8_t) ((_strokeLenght >> 8) & 0xFF))) {
            _channel->setStrokeLenght(_strokeLenght);
            uint8_t _positionLow;
            uint8_t _positionHigh;
            uint8_t _pulseRate;
            if (spiCmdUpdate(_channel->getPin(), GETPOSITIONLOW(), &_positionLow)) {
                if (spiCmdUpdate(_channel->getPin(), GETPOSITIONHIGH(), &_positionHigh)) {
                    if (spiCmdUpdate(_channel->getPin(), GETPULSERATE(), &_pulseRate)) {
                        _channel->setPosition((uint16_t)((_positionLow & 0xFF) | ((_positionHigh << 8) & 0xFF00)));
                        _channel->setPulseRate(_pulseRate);
                    }
                    else
                        _channel->setCommunication("Disable");
                }
                else
                    _channel->setCommunication("Disable");
            }
            else
                _channel->setCommunication("Disable");
        }
        else
            _channel->setCommunication("Disable");
    }
    else
        _channel->setCommunication("Disable");

    updateJsonFile();
    setupTab();
}


void MainWindow::ButtonUpdateSoftStartStop(uint8_t _ch, uint8_t _accelarationRate)
{
    Channel* _channel = getChannel(_ch);

    if (spiCmdSetup(PIN_CHANNEL1, SENDSOFTSTART(), _accelarationRate))
        _channel->setAccelarationRate(_accelarationRate);
    else
        _channel->setCommunication("Disable");

    updateJsonFile();
    setupTab();
}

void MainWindow::CheckBoxSoftStartStop(uint8_t _ch, uint8_t _softStartStop)
{
    Channel* _channel = getChannel(_ch);
    if (spiCmdUpdate(_channel->getPin(), SENDSOFTSTART(), &_softStartStop))
        _channel->setSoftStartStop(_softStartStop);
    else
        _channel->setCommunication("Disable");

    updateJsonFile();
    setupTab();
}

void MainWindow::Update(void)
{
    for (Channel* _ch: getListOfChannels()){
        if (_ch->getCommunication() == "Enable") {
            uint8_t _positionLow;
            uint8_t _positionHigh;
            uint8_t _pulseRate;
            if (spiCmdUpdate(_ch->getPin(), GETPOSITIONLOW(), &_positionLow)) {
                if (spiCmdUpdate(_ch->getPin(), GETPOSITIONHIGH(), &_positionHigh)) {
                    if (spiCmdUpdate(_ch->getPin(), GETPULSERATE(), &_pulseRate)) {
                        _ch->setPosition((uint16_t)((_positionLow & 0xFF) | ((_positionHigh << 8) & 0xFF00)));
                        _ch->setPulseRate(_pulseRate);
                    }
                }
            }
        }
    }

    updateJsonFile();
    setupTab();
}
