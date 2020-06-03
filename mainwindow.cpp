#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Utils.h"

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
    setupSequenceTab();
}

MainWindow::~MainWindow()
{
    Update();
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getCommunication() == "Enable") {
                spiCmdCommunication(_channel->getPin(), DISABLE());
            }
    }
    updateJsonFile();
    spiClose();
    delete ui;
}

QList<Channel*> MainWindow::getListOfChannels(void)
{
    return  listOfChannels;
}

int MainWindow::getLastMoveInSequence(void)
{
    return lastMoveInSequence;
}

void MainWindow::setListOfChannels(QList<Channel*> _listOfChannels)
{
    listOfChannels = _listOfChannels;
}

void MainWindow::setLastMoveInSequence(int _move)
{
    lastMoveInSequence = _move;
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
        _channel->setMaxSpeed           ( (uint8_t) _channelJson.toObject().value("Speed"              ).toString().toUInt()       );
        _channel->setMaxCurrent         ( (float)   _channelJson.toObject().value("Maximum Current"    ).toString().toDouble()     );
        _channel->setModel              ( (string)  _channelJson.toObject().value("Model"              ).toString().toStdString()  );
        _channel->setCommunication      ( (string)  _channelJson.toObject().value("Communication"      ).toString().toStdString()  );
        _listOfChannels.append(_channel);
    }

    setListOfChannels(_listOfChannels);
}

void MainWindow::updateJsonFile (void)
{
    QFile _jsonFile(JSON_FILE_NAME);
    _jsonFile.open(QFile::WriteOnly);
    QJsonArray _channelsJson;

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
        _channelJson.insert("Speed"             , QString::number       (_channel->getMaxSpeed()        ));
        _channelJson.insert("Position"          , QString::number       (_channel->getPosition()        ));
        _channelsJson.push_back(_channelJson);
    }

    QJsonDocument recordsOfChannels(_channelsJson);
    _jsonFile.write(recordsOfChannels.toJson());
    _jsonFile.close();
}

void MainWindow::createJsonMoves(QString _jsonFileName)
{
    QFile _jsonFile(_jsonFileName);
    _jsonFile.open(QFile::WriteOnly);
    QJsonArray _movesJson;

    for (int i=0; i < getLastMoveInSequence() + 1; i++) {
        QJsonObject _moveJson;

        QLineEdit*  _name       = ui->tab->widget(TAB_SEQUENCE)->findChild<QLineEdit*>("editName"                                   );
        QComboBox*  _type       = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxSynchronous_"    + QString::number(i));
        QComboBox*  _master     = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxMasterChannel_"  + QString::number(i));
        QComboBox*  _slave      = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxSlaveChannel_"   + QString::number(i));
        QCheckBox*  _extended   = ui->tab->widget(TAB_SEQUENCE)->findChild<QCheckBox*>("checkBoxExtended_"      + QString::number(i));
        QCheckBox*  _refracted  = ui->tab->widget(TAB_SEQUENCE)->findChild<QCheckBox*>("checkBoxRefracted_"     + QString::number(i));
        QSlider*    _slider     = ui->tab->widget(TAB_SEQUENCE)->findChild<QSlider  *>("slider_"                + QString::number(i));

        _moveJson.insert("Name",    _name->text()                       );
        _moveJson.insert("Order",   QString::number(i)                  );
        _moveJson.insert("Type",    _type->currentText()                );
        _moveJson.insert("Master",  _master->currentText()              );
        _moveJson.insert("Slave",   _slave->currentText()               );
        _moveJson.insert("Shift",   QString::number(_slider->value())   );


        if (_extended->isChecked() == true)
            _moveJson.insert("Direction", "Extended");
        else if (_refracted->isChecked() == true)
            _moveJson.insert("Direction", "Refracted");

        _movesJson.push_back(_moveJson);
    }

    QJsonDocument recordsOfMoves(_movesJson);
    _jsonFile.write(recordsOfMoves.toJson());
    _jsonFile.close();
}

/*  ------------ Functions for SPI communication   -----------------------------------------   */
void MainWindow::checkCommunications (void){
    for (Channel* _ch: getListOfChannels()){

        if (spiCmdCommunication(_ch->getPin(), ENABLE())) {
            if (spiCmdSetup(_ch->getPin(), SENDSOFTSTART(), _ch->getSoftStartStop())) {
                if (spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTLOW(), _ch->getStrokeLenght() & 0xFF)) {
                    if (spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTHIGH(), (_ch->getStrokeLenght() >> 8) & 0xFF)) {
                        if (spiCmdSetup(_ch->getPin(), SENDACCELARATIONTIMELOW(), _ch->getAccelarationRate() & 0xFF )) {
                            if (spiCmdSetup(_ch->getPin(), SENDACCELARATIONTIMEHIGH(), (_ch->getAccelarationRate() >> 8) & 0xFF )) {
                                if (spiCmdSetup(_ch->getPin(), SENDPULSERATE(), _ch->getPulseRate())) {
                                    if (spiCmdSetup(_ch->getPin(), SENDPULSERATE(), _ch->getPulseRate())) {
                                        if (spiCmdSetup(_ch->getPin(), SENDMAXSPEED(), _ch->getMaxSpeed()))
                                            _ch->setCommunication("Enable");
                                    }
                                }
                            }
                        }
                    }
                }
            }
       }

        else
            _ch->setCommunication("Disable");
    }
}

void MainWindow::restartCommunication(uint8_t _channel)
{
    Channel* _ch = getChannel(_channel);
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

    if (spiCmdCommunication(_ch->getPin(), ENABLE()))
        _ch->setCommunication("Enable");
    else
        _ch->setCommunication("Disable");

    spiCmdSetup(_ch->getPin(), SENDSOFTSTART()              ,_ch->getSoftStartStop()                    );
    spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTLOW()        ,_ch->getStrokeLenght() & 0xFF              );
    spiCmdSetup(_ch->getPin(), SENDSTROKELENGHTHIGH()       ,(_ch->getStrokeLenght() >> 8) & 0xFF       );
    spiCmdSetup(_ch->getPin(), SENDACCELARATIONTIMELOW()    ,_ch->getAccelarationRate() & 0xFF          );
    spiCmdSetup(_ch->getPin(), SENDACCELARATIONTIMEHIGH()   ,(_ch->getAccelarationRate() >> 8) & 0xFF   );
    spiCmdSetup(_ch->getPin(), SENDPULSERATE()              ,_ch->getPulseRate()                        );
    spiCmdSetup(_ch->getPin(), SENDMAXSPEED()               , _ch->getMaxSpeed()                        );

}

/*  ------------ Functions for Widgets   -----------------------------------------   */

void MainWindow::setupTab(void)
{
    ui->comboBoxSlave->clear();
    ui->comboBoxMaster->clear();

    for(Channel* _channel : getListOfChannels()){

        if(_channel->getChannelNumber() == 1){
            ui->lineEditModel_channel1                  ->setText(QString::fromStdString(_channel->getModel()           ));
            ui->lineEditMaxCurrent_channel1             ->setText(QString::number       (_channel->getMaxCurrent()      ));
            ui->lineEditStrokeLenght_channel1           ->setText(QString::number       (_channel->getStrokeLenght()    ));
            ui->linePulseRate_channel1                  ->setText(QString::number       (_channel->getPulseRate()       ));
            ui->linePosition_channel1                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel1       ->setText(QString::number       (_channel->getAccelarationRate()));
            ui->spinBox_channel1                        ->setValue(_channel->getMaxSpeed()                                );

            ui->horizontalSliderExtended_channel1       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel1      ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderExtended_channel1       ->setValue(0);
            ui->horizontalSliderRefracted_channel1      ->setValue(0);
            ui->labelRefractedAdvanced_channel1         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel1          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            ui->lineEditAccelarationRate_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel1  ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
            ui->spinBox_channel1                        ->setEnabled(ui->checkBoxPwm_channel1->isChecked());


            if (_channel->getCommunication() == "Enable") {
                ui->channel1                            ->setEnabled(true);
                ui->comboBoxSlave                       ->addItem(QString::number (_channel->getChannelNumber()));
                ui->comboBoxMaster                      ->addItem(QString::number (_channel->getChannelNumber()));
                ui->horizontalLayout_channel1           ->setEnabled(true);
            }
            else {
                ui->channel1                            ->setEnabled(false);
                ui->pushButtonExtendedCommand_channel1  ->setEnabled(false);
                ui->pushButtonRefractedCommand_channel1 ->setEnabled(false);
           }
        }

        else if(_channel->getChannelNumber() == 2){
            ui->lineEditModel_channel2                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel2             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel2           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel2                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->linePosition_channel2                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel2       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->spinBox_channel2                        ->setValue(_channel->getMaxSpeed()                                );

            ui->horizontalSliderExtended_channel2       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel2      ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderExtended_channel2       ->setValue(0);
            ui->horizontalSliderRefracted_channel2      ->setValue(0);

            ui->lineEditAccelarationRate_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel2  ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->spinBox_channel2                        ->setEnabled(ui->checkBoxPwm_channel2->isChecked());

            ui->labelRefractedAdvanced_channel2         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel2          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            if (_channel->getCommunication() == "Enable") {
                ui->channel2                            ->setEnabled(true);
                ui->comboBoxSlave                       ->addItem(QString::number (_channel->getChannelNumber()));
                ui->comboBoxMaster                      ->addItem(QString::number (_channel->getChannelNumber()));
                ui->horizontalLayout_channel2           ->setEnabled(true);
            }
            else {
                ui->channel2                            ->setEnabled(false);
                ui->pushButtonExtendedCommand_channel2  ->setEnabled(false);
                ui->pushButtonRefractedCommand_channel2 ->setEnabled(false);               }
        }

        else if(_channel->getChannelNumber() == 3){
            ui->lineEditModel_channel3                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel3             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel3           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel3                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->linePosition_channel3                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel3       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->spinBox_channel3                        ->setValue(_channel->getMaxSpeed()                                );

            ui->horizontalSliderExtended_channel3       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel3      ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderExtended_channel3       ->setValue(0);
            ui->horizontalSliderRefracted_channel3      ->setValue(0);

            ui->lineEditAccelarationRate_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel3  ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->spinBox_channel3                        ->setEnabled(ui->checkBoxPwm_channel3->isChecked());

            ui->labelRefractedAdvanced_channel3         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel3          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));


            if (_channel->getCommunication() == "Enable") {
                ui->channel3                            ->setEnabled(true);
                ui->comboBoxSlave                       ->addItem(QString::number (_channel->getChannelNumber()));
                ui->comboBoxMaster                      ->addItem(QString::number (_channel->getChannelNumber()));
                ui->horizontalLayout_channel3           ->setEnabled(true);
            }
            else {
                ui->channel3                            ->setEnabled(false);
                ui->pushButtonExtendedCommand_channel3  ->setEnabled(false);
                ui->pushButtonRefractedCommand_channel3 ->setEnabled(false);               }
        }

        else if(_channel->getChannelNumber() == 4){
            ui->lineEditModel_channel4                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel4             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel4           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel4                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->linePosition_channel4                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel4       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->spinBox_channel4                        ->setValue(_channel->getMaxSpeed()                                );

            ui->horizontalSliderExtended_channel4       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel4      ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderExtended_channel4       ->setValue(0);
            ui->horizontalSliderRefracted_channel4      ->setValue(0);

            ui->lineEditAccelarationRate_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel4  ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->spinBox_channel4                        ->setEnabled(ui->checkBoxPwm_channel4->isChecked());

            ui->labelRefractedAdvanced_channel4         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel4          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            if (_channel->getCommunication() == "Enable") {
                ui->channel4                            ->setEnabled(true);
                ui->comboBoxSlave                       ->addItem(QString::number (_channel->getChannelNumber()));
                ui->comboBoxMaster                      ->addItem(QString::number (_channel->getChannelNumber()));
                ui->horizontalLayout_channel4           ->setEnabled(true);
            }
            else {
                ui->channel4                            ->setEnabled(false);
                ui->pushButtonExtendedCommand_channel4  ->setEnabled(false);
                ui->pushButtonRefractedCommand_channel4 ->setEnabled(false);               }
        }

        else if(_channel->getChannelNumber() == 5){
            ui->lineEditModel_channel5                  ->setText(QString::fromStdString(_channel->getModel()           ));
            ui->lineEditMaxCurrent_channel5             ->setText(QString::number       (_channel->getMaxCurrent()      ));
            ui->lineEditStrokeLenght_channel5           ->setText(QString::number       (_channel->getStrokeLenght()    ));
            ui->linePulseRate_channel5                  ->setText(QString::number       (_channel->getPulseRate()       ));
            ui->linePosition_channel5                   ->setText(QString::number       (_channel->getPosition()        ));
            ui->lineEditAccelarationRate_channel5       ->setText(QString::number       (_channel->getAccelarationRate()));
            ui->spinBox_channel5                        ->setValue(_channel->getMaxSpeed()                               );

            ui->horizontalSliderExtended_channel5       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel5      ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderExtended_channel5       ->setValue(0);
            ui->horizontalSliderRefracted_channel5      ->setValue(0);

            ui->lineEditAccelarationRate_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel5  ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->spinBox_channel5                        ->setEnabled(ui->checkBoxPwm_channel5->isChecked());

            ui->labelRefractedAdvanced_channel5         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel5          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));


            if (_channel->getCommunication() == "Enable") {
                ui->channel5                            ->setEnabled(true);
                ui->comboBoxSlave                       ->addItem(QString::number (_channel->getChannelNumber()));
                ui->comboBoxMaster                      ->addItem(QString::number (_channel->getChannelNumber()));
                ui->horizontalLayout_channel5           ->setEnabled(true);
            }
            else {
                ui->channel5                            ->setEnabled(false);
                ui->pushButtonExtendedCommand_channel5  ->setEnabled(false);
                ui->pushButtonRefractedCommand_channel5 ->setEnabled(false);
            }
        }
    }

}


/*  ------------ Functions for soft start button   -----------------------------------------   */
void MainWindow::on_pushButtonUpdateStartStopSoft_channel1_clicked()
{
    uint16_t _accelarationTime = (uint16_t) ui->lineEditAccelarationRate_channel1->text().toUInt();
    uint8_t _speed = (uint8_t) ui->spinBox_channel1->value();
    ButtonUpdateSoftStartStop(CHANNEL1, _accelarationTime, _speed);
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel2_clicked()
{
    uint16_t _accelarationTime = (uint16_t) ui->lineEditAccelarationRate_channel2->text().toUInt();
    uint8_t _speed = (uint8_t) ui->spinBox_channel2->value();
    ButtonUpdateSoftStartStop(CHANNEL2, _accelarationTime, _speed);
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel3_clicked()
{
    uint16_t _accelarationTime = (uint16_t) ui->lineEditAccelarationRate_channel3->text().toUInt();
    uint8_t _speed = (uint8_t) ui->spinBox_channel3->value();
    ButtonUpdateSoftStartStop(CHANNEL3, _accelarationTime, _speed);
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel4_clicked()
{
    uint16_t _accelarationTime = (uint16_t) ui->lineEditAccelarationRate_channel4->text().toUInt();
    uint8_t _speed = (uint8_t) ui->spinBox_channel4->value();
    ButtonUpdateSoftStartStop(CHANNEL4, _accelarationTime, _speed);
}
void MainWindow::on_pushButtonUpdateStartStopSoft_channel5_clicked()
{
    uint16_t _accelarationTime = (uint16_t) ui->lineEditAccelarationRate_channel5->text().toUInt();
    uint8_t _speed = (uint8_t) ui->spinBox_channel5->value();
    ButtonUpdateSoftStartStop(CHANNEL5, _accelarationTime, _speed);
}

/*  ------------ Functions for linear actuataor specifications button   -----------------------------------------   */
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel1_clicked()
{
    uint16_t    _strokeLenght   = (uint16_t) ui->lineEditStrokeLenght_channel1  ->text().toUInt();
    string      _model          = ui->lineEditModel_channel1                    ->text().toStdString();
    float       _maxCurrent     = ui->lineEditMaxCurrent_channel1               ->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL1, _strokeLenght, _model, _maxCurrent);
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel2_clicked()
{
    uint16_t _strokeLenght  = (uint16_t) ui->lineEditStrokeLenght_channel2  ->text().toUInt();
    string _model           = ui->lineEditModel_channel2                    ->text().toStdString();
    float _maxCurrent       = ui->lineEditMaxCurrent_channel2               ->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL2, _strokeLenght, _model, _maxCurrent);
}
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel3_clicked()
{
    uint16_t _strokeLenght  = (uint16_t) ui->lineEditStrokeLenght_channel3  ->text().toUInt();
    string _model           = ui->lineEditModel_channel3                    ->text().toStdString();
    float _maxCurrent       = ui->lineEditMaxCurrent_channel3               ->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL3, _strokeLenght, _model, _maxCurrent);
}
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel4_clicked()
{
    uint16_t _strokeLenght  = (uint16_t) ui->lineEditStrokeLenght_channel4  ->text().toUInt();
    string _model           = ui->lineEditModel_channel4                    ->text().toStdString();
    float _maxCurrent       = ui->lineEditMaxCurrent_channel4               ->text().toDouble();
    ButtonUpdateLinearActuator(CHANNEL4, _strokeLenght, _model, _maxCurrent);
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel5_clicked()
{
    uint16_t _strokeLenght  = (uint16_t) ui->lineEditStrokeLenght_channel5  ->text().toUInt();
    string _model           = ui->lineEditModel_channel5                    ->text().toStdString();
    float _maxCurrent   = ui->lineEditMaxCurrent_channel5                   ->text().toDouble();
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

void MainWindow::ButtonUpdateLinearActuator(uint8_t _ch, uint16_t _strokeLenght, string _model, float _maxCurrent)
{
    Channel* _channel = getChannel(_ch);
    _channel->setModel      (_model     );
    _channel->setMaxCurrent (_maxCurrent);

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


void MainWindow::ButtonUpdateSoftStartStop(uint8_t _ch, uint16_t _accelarationRate, uint8_t _speed)
{
    Channel* _channel = getChannel(_ch);

    if (spiCmdSetup(_channel->getPin(), SENDACCELARATIONTIMELOW(), _accelarationRate & 0xFF)) {
        if (spiCmdSetup(_channel->getPin(), SENDACCELARATIONTIMEHIGH(), (_accelarationRate >> 8) & 0xFF)) {
            _channel->setAccelarationRate(_accelarationRate);
            if (spiCmdSetup(_channel->getPin(), SENDMAXSPEED(), _speed))
                _channel->setMaxSpeed(_speed);
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

void MainWindow::CheckBoxSoftStartStop(uint8_t _ch, uint8_t _softStartStop)
{
    Channel* _channel = getChannel(_ch);

    if (spiCmdSetup(_channel->getPin(), SENDSOFTSTART(), _softStartStop))
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
                    else
                        _ch->setCommunication("Disable");
                }
                else
                    _ch->setCommunication("Disable");
            }
            else
                _ch->setCommunication("Disable");
        }

    }

    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonExtendedCommand_channel1_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL1, BASICEXTENDED());
}

void MainWindow::on_pushButtonExtendedCommand_channel2_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL2, BASICEXTENDED());
}

void MainWindow::on_pushButtonExtendedCommand_channel3_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL3, BASICEXTENDED());
}

void MainWindow::on_pushButtonExtendedCommand_channel4_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL4, BASICEXTENDED());
}

void MainWindow::on_pushButtonExtendedCommand_channel5_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL5, BASICEXTENDED());
}

void MainWindow::on_pushButtonExtendedSynchronous_pressed(void)
{
    uint8_t masterChannel   = ui->comboBoxMaster->currentText().toUInt();
    uint8_t slaveChannel    = ui->comboBoxSlave ->currentText().toUInt();

    if (masterChannel == CHANNEL1 || masterChannel == CHANNEL2 || masterChannel == CHANNEL3 || masterChannel  == CHANNEL4 || masterChannel == CHANNEL5) {
        Channel* _masterCh = getChannel(masterChannel);
        if ((slaveChannel == CHANNEL1 || slaveChannel == CHANNEL2 || slaveChannel == CHANNEL3 || slaveChannel  == CHANNEL4 || slaveChannel == CHANNEL5) ) {
            Channel* _slaveCh = getChannel(slaveChannel);
            if (masterChannel != slaveChannel) {
                if (ui->checkBoxReverse->isChecked() == true){
                    spiCmdBasic(_masterCh->getPin(), BASICEXTENDED());
                    spiCmdBasic(_slaveCh->getPin(), BASICREFRACTED());
                }
                else {
                    spiCmdBasic(_masterCh->getPin(), BASICEXTENDED());
                    spiCmdBasic(_slaveCh->getPin(), BASICEXTENDED());
                }
            }
        }
    }
}


void MainWindow::on_pushButtonExtendedCommand_channel1_released(void)
{
    spiCmdBasic(PIN_CHANNEL1, STOP());
}

void MainWindow::on_pushButtonExtendedCommand_channel2_released(void)
{
    spiCmdBasic(PIN_CHANNEL2, STOP());
}

void MainWindow::on_pushButtonExtendedCommand_channel3_released(void)
{
    spiCmdBasic(PIN_CHANNEL3, STOP());
}

void MainWindow::on_pushButtonExtendedCommand_channel4_released(void)
{
    spiCmdBasic(PIN_CHANNEL4, STOP());
}

void MainWindow::on_pushButtonExtendedCommand_channel5_released(void)
{
    spiCmdBasic(PIN_CHANNEL5, STOP());
}

void MainWindow::on_pushButtonExtendedSynchronous_released(void)
{
    int8_t masterChannel   = ui->comboBoxMaster->currentText().toUInt();
    uint8_t slaveChannel   = ui->comboBoxSlave ->currentText().toUInt();

     if (masterChannel == CHANNEL1 || masterChannel == CHANNEL2 || masterChannel == CHANNEL3 || masterChannel  == CHANNEL4 || masterChannel == CHANNEL5) {
        Channel* _masterCh = getChannel(masterChannel);
        if ((slaveChannel == CHANNEL1 || slaveChannel == CHANNEL2 || slaveChannel == CHANNEL3 || slaveChannel  == CHANNEL4 || slaveChannel == CHANNEL5) ) {
            Channel* _slaveCh = getChannel(slaveChannel);
            if (slaveChannel != masterChannel) {
                spiCmdBasic(_masterCh->getPin(), STOP());
                spiCmdBasic(_slaveCh->getPin(), STOP());
            }
        }
    }
}

void MainWindow::on_pushButtonRefractedCommand_channel1_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL1, BASICREFRACTED());
}

void MainWindow::on_pushButtonRefractedCommand_channel2_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL2, BASICREFRACTED());
}

void MainWindow::on_pushButtonRefractedCommand_channel3_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL3, BASICREFRACTED());
}

void MainWindow::on_pushButtonRefractedCommand_channel4_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL4, BASICREFRACTED());
}

void MainWindow::on_pushButtonRefractedCommand_channel5_pressed(void)
{
    spiCmdBasic(PIN_CHANNEL5, BASICREFRACTED());
}

void MainWindow::on_pushButtonRefractedSynchronous_pressed(void)
{
    uint8_t masterChannel   = ui->comboBoxMaster->currentText().toUInt();
    uint8_t slaveChannel    = ui->comboBoxSlave ->currentText().toUInt();

    if (masterChannel == CHANNEL1 || masterChannel == CHANNEL2 || masterChannel == CHANNEL3 || masterChannel  == CHANNEL4 || masterChannel == CHANNEL5) {
        Channel* _masterCh = getChannel(masterChannel);
        if ((slaveChannel == CHANNEL1 || slaveChannel == CHANNEL2 || slaveChannel == CHANNEL3 || slaveChannel  == CHANNEL4 || slaveChannel == CHANNEL5) ) {
            Channel* _slaveCh = getChannel(slaveChannel);
            if (masterChannel != slaveChannel) {
                if (ui->checkBoxReverse->isChecked() == true){
                    spiCmdBasic(_masterCh->getPin(), BASICREFRACTED());
                    spiCmdBasic(_slaveCh->getPin(), BASICEXTENDED());
                }
                else {
                    spiCmdBasic(_masterCh->getPin(), BASICREFRACTED());
                    spiCmdBasic(_slaveCh->getPin(), BASICREFRACTED());
                }
            }
        }
    }
}

void MainWindow::on_pushButtonRefractedCommand_channel1_released(void)
{
    spiCmdBasic(PIN_CHANNEL1, STOP());
}

void MainWindow::on_pushButtonRefractedCommand_channel2_released(void)
{
    spiCmdBasic(PIN_CHANNEL2, STOP());
}

void MainWindow::on_pushButtonRefractedCommand_channel3_released(void)
{
    spiCmdBasic(PIN_CHANNEL3, STOP());
}

void MainWindow::on_pushButtonRefractedCommand_channel4_released()
{
    spiCmdBasic(PIN_CHANNEL4, STOP());
}

void MainWindow::on_pushButtonRefractedCommand_channel5_released(void)
{
    spiCmdBasic(PIN_CHANNEL5, STOP());
}


void MainWindow::on_pushButtonRefractedSynchronous_released(void)
{
    int8_t masterChannel   = ui->comboBoxMaster->currentText().toUInt();
    uint8_t slaveChannel    = ui->comboBoxSlave ->currentText().toUInt();

     if (masterChannel == CHANNEL1 || masterChannel == CHANNEL2 || masterChannel == CHANNEL3 || masterChannel  == CHANNEL4 || masterChannel == CHANNEL5) {
        Channel* _masterCh = getChannel(masterChannel);
        if ((slaveChannel == CHANNEL1 || slaveChannel == CHANNEL2 || slaveChannel == CHANNEL3 || slaveChannel  == CHANNEL4 || slaveChannel == CHANNEL5) ) {
            Channel* _slaveCh = getChannel(slaveChannel);
            if (masterChannel != slaveChannel) {
                spiCmdBasic(_masterCh->getPin(), STOP());
                spiCmdBasic(_slaveCh->getPin(), STOP());
            }
        }
    }
}

void MainWindow::on_tab_currentChanged(int index)
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
    else if (index == TAB_COMMAND || index == TAB_SEQUENCE) {
        restartCommunication(CHANNEL1);
        restartCommunication(CHANNEL2);
        restartCommunication(CHANNEL3);
        restartCommunication(CHANNEL4);
        restartCommunication(CHANNEL5);
    }

    updateJsonFile();
    setupTab();
}

void MainWindow::setupSequenceTab(void)
{
        ui->tab->removeTab(TAB_SEQUENCE);

        QGridLayout*    grid = new QGridLayout();

        QGridLayout*    move[MAX_NUMBER_MOVES];
        QWidget*        moveWidget[MAX_NUMBER_MOVES];

        QLabel*         labelOrder[MAX_NUMBER_MOVES];

        QVBoxLayout*    synchronous[MAX_NUMBER_MOVES];
        QLabel*         labelSynchronous[MAX_NUMBER_MOVES];
        QComboBox*      combBoxSynchronous[MAX_NUMBER_MOVES];

        QVBoxLayout*    channelMaster[MAX_NUMBER_MOVES];
        QLabel*         labelMasterChannel[MAX_NUMBER_MOVES];
        QComboBox*      combBoxMasterChannel[MAX_NUMBER_MOVES];

        QVBoxLayout*    channelSlave[MAX_NUMBER_MOVES];
        QLabel*         labelSlaveChannel[MAX_NUMBER_MOVES];
        QComboBox*      combBoxSlaveChannel[MAX_NUMBER_MOVES];
        QCheckBox*      checkBoxRefracted[MAX_NUMBER_MOVES];
        QCheckBox*      checkBoxExtended[MAX_NUMBER_MOVES];

        QVBoxLayout*    range[MAX_NUMBER_MOVES];
        QLabel*         labelSlider[MAX_NUMBER_MOVES];
        QSlider*        slider[MAX_NUMBER_MOVES];

        QLabel*         labelName       = new QLabel();
        QLineEdit*      editName        = new QLineEdit();
        QLabel*         labelRepetion   = new QLabel();
        QLineEdit*      editRepetion    = new QLineEdit();
        QHBoxLayout*    name            = new QHBoxLayout();

        labelName       ->setObjectName("labelName"             );
        labelName       ->setText      ("Sequence Name: "       );
        editName        ->setObjectName("editName"              );
        labelRepetion   ->setObjectName("labelRepetion"         );
        editRepetion    ->setObjectName("editRepetion"          );
        labelRepetion   ->setText      ("Number of repetions: " );
        editRepetion    ->setText      ("1");
        name->addWidget         (labelName          );
        name->addWidget         (editName           );
        name->addWidget         (labelRepetion      );
        name->addWidget         (editRepetion       );
        grid->addLayout         (name, 0, 0         );


        for (int i=0; i<MAX_NUMBER_MOVES; i++) {
            move[i] = new QGridLayout();

            labelOrder[i] = new QLabel();
            labelOrder[i]->setObjectName("labelOrder_" + QString::number(i) );
            labelOrder[i]->setText      ("Move " + QString::number(i) + ":" );
            move[i]->addWidget(labelOrder[i], 0, 0                          );

            labelSynchronous[i] = new QLabel();
            labelSynchronous[i]->setObjectName  ("labelSynchronous_" + QString::number(i)   );
            labelSynchronous[i]->setText        ("Synchronous"                              );
            combBoxSynchronous[i] = new QComboBox();
            combBoxSynchronous[i]->setObjectName("combBoxSynchronous_" + QString::number(i));
            combBoxSynchronous[i]->insertItem(0, "Independend"          );
            combBoxSynchronous[i]->insertItem(1, "Synchronous"          );
            combBoxSynchronous[i]->insertItem(2, "Reverse Synchronous"  );
            connect(combBoxSynchronous[i], SIGNAL(currentTextChanged(QString)), this, SLOT(changed_combBoxSynchronous()));
            synchronous[i] = new QVBoxLayout();
            synchronous[i]->addWidget(labelSynchronous[i]   );
            synchronous[i]->addWidget(combBoxSynchronous[i] );
            move[i]->addLayout(synchronous[i], 0, 1         );

            labelMasterChannel[i]  = new QLabel();
            labelMasterChannel[i]->setObjectName("labelMasterChannel_" + QString::number(i) );
            labelMasterChannel[i]->setText      ("Master"                                   );
            combBoxMasterChannel[i] = new QComboBox();
            combBoxMasterChannel[i]->setObjectName("combBoxMasterChannel_" + QString::number(i));
            connect(combBoxMasterChannel[i], SIGNAL(currentTextChanged(QString)), this, SLOT(changed_combBoxMaster(QString)));
            channelMaster[i] = new QVBoxLayout();
            channelMaster[i]->addWidget(labelMasterChannel[i]   );
            channelMaster[i]->addWidget(combBoxMasterChannel[i] );
            move[i]->addLayout(channelMaster[i], 0, 2           );

            labelSlaveChannel[i]  = new QLabel();
            labelSlaveChannel[i]->setObjectName("labelSlaveChannel_" + QString::number(i));
            labelSlaveChannel[i]->setText("Slave");
            combBoxSlaveChannel[i] = new QComboBox();
            combBoxSlaveChannel[i]->setObjectName("combBoxSlaveChannel_" + QString::number(i));
            channelSlave[i] = new QVBoxLayout();
            channelSlave[i]->addWidget(labelSlaveChannel[i]     );
            channelSlave[i]->addWidget(combBoxSlaveChannel[i]   );
            move[i]->addLayout(channelSlave[i], 0, 3            );

            checkBoxRefracted[i] = new QCheckBox();
            checkBoxRefracted[i]->setObjectName("checkBoxRefracted_" + QString::number(i));
            checkBoxRefracted[i]->setText("Refracted"       );
            move[i]->addWidget(checkBoxRefracted[i], 0, 4   );

            checkBoxExtended[i] = new QCheckBox();
            checkBoxExtended[i]->setObjectName("checkBoxExtended_" + QString::number(i));
            checkBoxExtended[i]->setText("Extended"     );
            move[i]->addWidget(checkBoxExtended[i], 0, 5);

            labelSlider[i] = new QLabel();
            labelSlider[i]->setObjectName("labelSlider_" + QString::number(i));
            slider[i] = new QSlider();
            slider[i]->setObjectName("slider_" + QString::number(i) );
            slider[i]->setOrientation(Qt::Horizontal                );
            labelSlider[i]->setText("Shift: " + QString::number(slider[i]->tickPosition()) + " mm");
            connect(slider[i], SIGNAL(sliderReleased()), this, SLOT(changed_slider()));
            range[i] = new QVBoxLayout();
            range[i]->addWidget(labelSlider[i]      );
            range[i]->addWidget(slider[i]           );
            move[i]->addLayout(range[i], 0, 6       );

            moveWidget[i] = new QWidget();
            moveWidget[i]->setObjectName("moveWidget_" + QString::number(i));
            moveWidget[i]->setLayout(move[i]);
            moveWidget[i]->hide();

            grid->addWidget(moveWidget[i], i + 1, 0);
        }

        setLastMoveInSequence(-1);

        QHBoxLayout* buttonsSecondLine = new QHBoxLayout();

        QPushButton* startButton = new QPushButton();
        startButton->setText("Start Sequence");
        connect(startButton, SIGNAL (clicked()),this, SLOT (on_startButton()));
        grid->addWidget(startButton, MAX_NUMBER_MOVES + 1, 0);

        QPushButton* newButton = new QPushButton();
        newButton->setText("New Move");
        buttonsSecondLine->addWidget(newButton);
        connect(newButton, SIGNAL (clicked()),this, SLOT (on_newButton()));


        QPushButton* saveButton = new QPushButton();
        saveButton->setText("Save Sequence");
        buttonsSecondLine->addWidget(saveButton);
        connect(saveButton, SIGNAL (clicked()),this, SLOT (on_saveButton()));

        grid->addLayout(buttonsSecondLine, MAX_NUMBER_MOVES + 2, 0);

        QHBoxLayout* buttonsThirdLine = new QHBoxLayout();

        QPushButton* uploadButton = new QPushButton();
        uploadButton->setText("Upload Sequence");
        buttonsThirdLine->addWidget(uploadButton);
        connect(uploadButton, SIGNAL (clicked()),this, SLOT (on_uploadButton()));

        QComboBox* sequence = new QComboBox();
        sequence->setObjectName("comboBoxSequence");
        sequence->clear();
        buttonsThirdLine->addWidget(sequence);

        QDir dir;
        dir.setFilter(QDir::Files);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.fileName().contains(".json") && fileInfo.fileName().contains("RecordsMoves_")) {
                QFile jsonFile(fileInfo.fileName());
                jsonFile.open(QFile::ReadOnly);
                QJsonArray _movesJson = QJsonDocument().fromJson(jsonFile.readAll()).array();
                jsonFile.close();
                foreach (const QJsonValue & _moveJson, _movesJson){
                    if (sequence->findText(_moveJson.toObject().value("Name").toString()) == -1)
                        sequence->addItem(_moveJson.toObject().value("Name").toString());
                }
            }
        }

        grid->addLayout(buttonsThirdLine, MAX_NUMBER_MOVES + 3, 0);


        QWidget *w      = new QWidget();
        w->setLayout(grid);
        ui->tab->addTab(w, "Sequential");

        for (int i=0; i < MAX_NUMBER_MOVES; i++) {
            setupCombBoxMasterChannel(i);
            setupCombBoxSlaveChannel(i);

            slider[i]->setRange(0, getChannel(combBoxMasterChannel[i]->currentText().toUInt())->getStrokeLenght());
            slider[i]->setTickInterval(1);
            slider[i]->setValue(getChannel(combBoxMasterChannel[i]->currentText().toUInt())->getStrokeLenght());
            labelSlider[i]->setText( QString::number(slider[i]->value()) + " mm");

            if (combBoxSynchronous[i]->currentText() == "Independend") {
                labelSlaveChannel[i]    ->setEnabled(false);
                combBoxSlaveChannel[i]  ->setEnabled(false);
            }
            else {
                labelSlaveChannel[i]    ->setEnabled(true);
                combBoxSlaveChannel[i]  ->setEnabled(true);
            }
        }
}

void MainWindow::on_newButton(void) {
    QWidget* widget = ui->tab->widget(TAB_SEQUENCE)->findChild<QWidget *>("moveWidget_" +  QString::number(getLastMoveInSequence() + 1));
    widget->show();
    setLastMoveInSequence(getLastMoveInSequence() + 1);
}

void MainWindow::on_saveButton(void)
{
    QLineEdit* _name = ui->tab->widget(TAB_SEQUENCE)->findChild<QLineEdit *>("editName");
    QString jsonFileName = JSON_FILE_MOVES + _name->text() + ".json";
    createJsonMoves(jsonFileName);

}

void MainWindow::on_uploadButton(void)
{
    QComboBox* _sequence = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox *>("comboBoxSequence");
    QString _fileName = JSON_FILE_MOVES + _sequence->currentText() + ".json";
    QFile jsonFile(_fileName);
    jsonFile.open(QFile::ReadOnly);
    QJsonArray _movesJson = QJsonDocument().fromJson(jsonFile.readAll()).array();
    jsonFile.close();

    foreach (const QJsonValue & _moveJson, _movesJson){
        int         _order      = _moveJson.toObject().value("Order").toString().toInt();
        QLineEdit*  _name       = ui->tab->widget(TAB_SEQUENCE)->findChild<QLineEdit*>("editName"                                        );
        QLabel*     _labelOrder = ui->tab->widget(TAB_SEQUENCE)->findChild<QLabel   *>("labelOrder_"            + QString::number(_order));
        QComboBox*  _type       = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxSynchronous_"    + QString::number(_order));
        QComboBox*  _master     = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxMasterChannel_"  + QString::number(_order));
        QComboBox*   _slave     = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxSlaveChannel_"   + QString::number(_order));
        QLabel*     _labelSlave = ui->tab->widget(TAB_SEQUENCE)->findChild<QLabel   *>("labelSlaveChannel_"     + QString::number(_order));
        QCheckBox*  _extended   = ui->tab->widget(TAB_SEQUENCE)->findChild<QCheckBox*>("checkBoxExtended_"      + QString::number(_order));
        QCheckBox*  _refracted  = ui->tab->widget(TAB_SEQUENCE)->findChild<QCheckBox*>("checkBoxRefracted_"     + QString::number(_order));
        QSlider*    _slider     = ui->tab->widget(TAB_SEQUENCE)->findChild<QSlider  *>("slider_"                + QString::number(_order));
        QLabel*     _labelSlider= ui->tab->widget(TAB_SEQUENCE)->findChild<QLabel   *>("labelSlider_"           + QString::number(_order));
        QWidget*    _move       = ui->tab->widget(TAB_SEQUENCE)->findChild<QWidget  *>("moveWidget_"            + QString::number(_order));

        _name       ->setText(_moveJson.toObject().value("Name").toString()         );
        _labelOrder ->setText("Move " + QString::number(_order) + ":"               );
        _type       ->setCurrentText(_moveJson.toObject().value("Type").toString()  );
        _master     ->setCurrentText(_moveJson.toObject().value("Master").toString());
        _slave      ->setCurrentText(_moveJson.toObject().value("Slave").toString() );

        if (_type->currentText() == "Independend") {
            _slave      ->setEnabled(false);
            _labelSlave ->setEnabled(false);
        }
        else {
            _slave      ->setEnabled(true);
            _labelSlave ->setEnabled(true);
        }

        if (_moveJson.toObject().value("Direction").toString() == "Extended") {
            _extended   ->setChecked(true   );
            _refracted  ->setChecked(false  );
        }
        else if (_moveJson.toObject().value("Direction").toString() == "Refracted") {
            _extended   ->setChecked(false  );
            _refracted  ->setChecked(true   );
        }

        _slider     ->setTickInterval(1                                                         );
        _slider     ->setRange(0, getChannel(_master->currentText().toInt())->getStrokeLenght() );
        _slider     ->setValue(_moveJson.toObject().value("Shift").toString().toInt()           );
        _labelSlider->setText("Shift: " + QString::number(_slider->value()) + " mm"             );
        _move       ->show(                                                                     );

        setLastMoveInSequence(_order);
    }

}
void MainWindow::on_startButton(void) {
    QLineEdit* editRepetion = ui->tab->widget(TAB_SEQUENCE)->findChild<QLineEdit*>("editRepetion");
    for (int j=0; j < editRepetion->text().toInt(); j++) {
        for (int i = 0; i < getLastMoveInSequence() + 1; i++) {
            QComboBox* checkSynchronous = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxSynchronous_"    + QString::number(i));
            QComboBox* master           = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxMasterChannel_"  + QString::number(i));
            QComboBox* slave            = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox*>("combBoxSlaveChannel_"   + QString::number(i));
            QCheckBox* extended         = ui->tab->widget(TAB_SEQUENCE)->findChild<QCheckBox*>("checkBoxExtended_"      + QString::number(i));
            QCheckBox* refracted        = ui->tab->widget(TAB_SEQUENCE)->findChild<QCheckBox*>("checkBoxRefracted_"     + QString::number(i));
            QSlider*   slider           = ui->tab->widget(TAB_SEQUENCE)->findChild<QSlider  *>("slider_"                + QString::number(i));

            uint8_t _flagAdvanced       = 0;
            uint8_t _flagAdvancedMaster = 0;
            uint8_t _flagAdvancedSlave  = 0;
            uint8_t _positionLowMaster  = 0;
            uint8_t _positionHighMaster = 0;
            uint8_t _positionLowSlave   = 0;
            uint8_t _positionHighSlave  = 0;
            uint8_t _positionLow        = 0;
            uint8_t _positionHigh       = 0;

            if (checkSynchronous->currentText() == "Independend") {
                if (extended->isChecked())
                    spiCmdAdvanced(getChannel(master->currentText().toUInt())->getPin(), ADVANCEDEXTENDED(), slider->value());

                else if (refracted->isChecked())
                    spiCmdAdvanced(getChannel(master->currentText().toUInt())->getPin(), ADVANCEDREFRACTED(), slider->value());

                while (_flagAdvanced == 0) {
                    QThread::sleep(DELAY);
                    spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin(),GETFLAGADVANCED(), &_flagAdvanced);
                }

                spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin(), GETPOSITIONLOW() , &_positionLow);
                spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin(), GETPOSITIONHIGH(), &_positionHigh);
                getChannel(master->currentText().toUInt())->setPosition((_positionLow & 0xFF) | ((_positionHigh << 8) & 0xFF));
            }

            else if (checkSynchronous->currentText() == "Synchronous") {
               if (extended->isChecked()) {
                    spiCmdAdvanced(getChannel(master->currentText().toUInt())->getPin() , ADVANCEDEXTENDED(), slider->tickPosition());
                    spiCmdAdvanced(getChannel(slave->currentText().toUInt())->getPin()  , ADVANCEDEXTENDED(), slider->tickPosition());
               }

               else if (refracted->isChecked()) {
                    spiCmdAdvanced(getChannel(master->currentText().toUInt())->getPin() , ADVANCEDREFRACTED(), slider->tickPosition());
                    spiCmdAdvanced(getChannel(slave->currentText().toUInt())->getPin()  , ADVANCEDREFRACTED(), slider->tickPosition());
               }

               while(_flagAdvancedMaster == 0 || _flagAdvancedSlave == 0) {
                QThread::sleep(DELAY);
                if (_flagAdvancedMaster == 0)
                    spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin()   ,GETFLAGADVANCED()  , &_flagAdvancedMaster);
                if (_flagAdvancedSlave == 0)
                    spiCmdUpdate(getChannel(slave->currentText().toUInt())->getPin()    ,GETFLAGADVANCED()  , &_flagAdvancedSlave);
               }

               spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin(), GETPOSITIONLOW()  , &_positionLowMaster);
               spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin(), GETPOSITIONHIGH() , &_positionHighMaster);
               getChannel(master->currentText().toUInt())->setPosition((_positionLowMaster & 0xFF) | ((_positionHighMaster << 8) & 0xFF));

               spiCmdUpdate(getChannel(slave->currentText().toUInt())->getPin(), GETPOSITIONLOW()   , &_positionLowSlave);
               spiCmdUpdate(getChannel(slave->currentText().toUInt())->getPin(), GETPOSITIONHIGH()  , &_positionHighSlave);
               getChannel(slave->currentText().toUInt())->setPosition((_positionLowSlave & 0xFF) | ((_positionHighSlave << 8) & 0xFF));
            }

            else if (checkSynchronous->currentText() == "Reverse Synchronous") {
               if (extended->isChecked()) {
                    spiCmdAdvanced(getChannel(master->currentText().toUInt())->getPin() , ADVANCEDEXTENDED()    , slider->tickPosition());
                    spiCmdAdvanced(getChannel(slave->currentText().toUInt())->getPin()  , ADVANCEDREFRACTED()   , slider->tickPosition());
               }

               else if (refracted->isChecked()) {
                    spiCmdAdvanced(getChannel(master->currentText().toUInt())->getPin() , ADVANCEDREFRACTED()   , slider->tickPosition());
                    spiCmdAdvanced(getChannel(slave->currentText().toUInt())->getPin()  , ADVANCEDEXTENDED()    , slider->tickPosition());
               }

               while(_flagAdvancedMaster == 0 || _flagAdvancedSlave == 0) {
                QThread::sleep(DELAY);
                if (_flagAdvancedMaster == 0)
                    spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin()   ,GETFLAGADVANCED(), &_flagAdvancedMaster);
                if (_flagAdvancedSlave == 0)
                    spiCmdUpdate(getChannel(slave->currentText().toUInt())->getPin()    ,GETFLAGADVANCED(), &_flagAdvancedSlave);
               }

               spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin(), GETPOSITIONLOW()  , &_positionLowMaster);
               spiCmdUpdate(getChannel(master->currentText().toUInt())->getPin(), GETPOSITIONHIGH() , &_positionHighMaster);
               getChannel(master->currentText().toUInt())->setPosition((_positionLowMaster & 0xFF) | ((_positionHighMaster << 8) & 0xFF));

               spiCmdUpdate(getChannel(slave->currentText().toUInt())->getPin(), GETPOSITIONLOW()   , &_positionLowSlave);
               spiCmdUpdate(getChannel(slave->currentText().toUInt())->getPin(), GETPOSITIONHIGH()  , &_positionHighSlave);
               getChannel(slave->currentText().toUInt())->setPosition((_positionLowSlave & 0xFF) | ((_positionHighSlave << 8) & 0xFF));
            }
        }
     }

    updateJsonFile();
    setupTab();
    setupSequenceTab();
    setLastMoveInSequence(-1);
}

void MainWindow::changed_combBoxSynchronous (void)
{
    for (int i = 0; i < getLastMoveInSequence() + 1; i++) {
        QComboBox*  comboBoxSynchronous = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox *> ("combBoxSynchronous_"  +  QString::number(i));
        QLabel*     labelSlave          = ui->tab->widget(TAB_SEQUENCE)->findChild<QLabel *>    ("labelSlaveChannel_"   +  QString::number(i));
        QComboBox*  comboBoxSlave       = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox *> ("combBoxSlaveChannel_" +  QString::number(i));

        if (comboBoxSynchronous->currentText() == "Independend") {
            labelSlave->setEnabled(false);
            comboBoxSlave->setEnabled(false);
        }

        else {
            labelSlave->setEnabled(true);
            comboBoxSlave->setEnabled(true);
        }
    }
}

void MainWindow::changed_combBoxMaster(QString)
{
    for (int i = 0; i < getLastMoveInSequence() + 1; i++) {
        QSlider*    _slider     = ui->tab->widget(TAB_SEQUENCE)->findChild<QSlider *>   ("slider_" +  QString::number(i)                );
        QComboBox*  _comboBox   = ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox *> ("combBoxMasterChannel_" +  QString::number(i)  );
        QLabel*     _label      = ui->tab->widget(TAB_SEQUENCE)->findChild<QLabel *>    ("labelSlider_" +  QString::number(i)           );
        Channel*    _ch         = getChannel(_comboBox->currentText().toUInt());

        _slider->setRange(0, (int) _ch->getStrokeLenght());
        _label->setText(QString::number(_slider->value()));
    }
}

void MainWindow::setupCombBoxMasterChannel (uint8_t _move)
{
    QComboBox* _combBoxMaster =  ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox *>("combBoxMasterChannel_" +  QString::number(_move));
    for (Channel* _ch : getListOfChannels()) {
        if (_ch->getCommunication() == "Enable")
            _combBoxMaster->addItem(QString::number(_ch->getChannelNumber()));
    }

}

void MainWindow::setupCombBoxSlaveChannel (uint8_t _move) {
    QComboBox* _combBoxSlave =  ui->tab->widget(TAB_SEQUENCE)->findChild<QComboBox *>("combBoxSlaveChannel_" +  QString::number(_move));
    for (Channel* _ch : getListOfChannels()) {
        if (_ch->getCommunication() == "Enable")
            _combBoxSlave->addItem(QString::number(_ch->getChannelNumber()));
    }
}

void MainWindow::changed_slider(void) {
    for (int i = 0; i < getLastMoveInSequence() + 1; i++) {
        QSlider*_slider = ui->tab->widget(TAB_SEQUENCE)->findChild<QSlider  *>("slider_"        +  QString::number(i));
        QLabel* _label  = ui->tab->widget(TAB_SEQUENCE)->findChild<QLabel   *>("labelSlider_"   +  QString::number(i));
        _label->setText(QString::number(_slider->value()) + " mm");
    }
}
