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


    if(QFile::exists(JSON_FILE_NAME))
        readJsonFile();
    else {
        initListOfActuators();
        updateJsonFile();
        readJsonFile();
    }

    spiInit();
    checkCommunications();
    setupTab();
}

MainWindow::~MainWindow()
{
    //  Update pulse rate value
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getCommunication() == "Enable")
            _channel->setPulseRate(disableCommunication(_channel->getPin()));
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
        _channel->setAccelarationTime   ( (uint16_t)_channelJson.toObject().value("Accelaration Time"  ).toString().toUInt()       );
        _channel->setPulseRate          ( (uint8_t) _channelJson.toObject().value("Pulse Rate"         ).toString().toUInt()       );
        _channel->setPin                ( (uint8_t) _channelJson.toObject().value("Pin"                ).toString().toUInt()       );
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
        _channelJson.insert("Accelaration Time" , QString::number       (_channel->getAccelarationTime()));
        _channelJson.insert("Pulse Rate"        , QString::number       (_channel->getPulseRate()       ));
        _channelJson.insert("Pin"               , QString::number       (_channel->getPin()             ));
        channelsJson.push_back(_channelJson);
    }

    QJsonDocument recordsOfChannels(channelsJson);
    jsonFile.write(recordsOfChannels.toJson());
    jsonFile.close();
}

/*  ------------ Functions for SPI communication   -----------------------------------------   */
void MainWindow::checkCommunications (void){
    for (Channel* _channel: getListOfChannels()){
        if (enableCommunication(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop()) == 1)
            _channel->setCommunication("Enable");
        else
            _channel->setCommunication("Disable");
    }
}

void MainWindow::restartCommunication(void){
    for (Channel* _channel: getListOfChannels()){
        _channel->setPulseRate(disableCommunication(_channel->getPin()));
        if (enableCommunication(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop()) == 1)
            _channel->setCommunication("Enable");
        else
            _channel->setCommunication("Disable");
    }
}

/*  ------------ Functions for Widgets   -----------------------------------------   */

void MainWindow::setupTab(void)
{
    for(Channel* _channel : getListOfChannels()){

        if(_channel->getChannelNumber() == 1){
            ui->lineEditModel_channel1                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel1             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel1           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->linePulseRate_channel1                  ->setText(QString::number       (_channel->getPulseRate()        ));
            ui->lineEditAccelarationTime_channel1       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel1       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel1      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationTime_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel1  ->setEnabled(ui->checkBoxPwm_channel1->isChecked());

            ui->labelRefractedAdvanced_channel1         ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
            ui->labelExtendedAdvanced_channel1          ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));

            ui->lineEditAccelarationTime_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel1  ->setEnabled(ui->checkBoxPwm_channel1->isChecked());

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
            ui->lineEditAccelarationTime_channel2       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel2       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel2      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationTime_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel2  ->setEnabled(ui->checkBoxPwm_channel2->isChecked());

            ui->labelRefractedAdvanced_channel2         ->setText(QString::number(ui->horizontalSliderRefracted_channel2->value()));
            ui->labelExtendedAdvanced_channel2          ->setText(QString::number(ui->horizontalSliderExtended_channel2 ->value()));

            ui->lineEditAccelarationTime_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel2  ->setEnabled(ui->checkBoxPwm_channel2->isChecked());

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
            ui->lineEditAccelarationTime_channel3       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel3       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel3      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationTime_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel3  ->setEnabled(ui->checkBoxPwm_channel3->isChecked());

            ui->labelRefractedAdvanced_channel3         ->setText(QString::number(ui->horizontalSliderRefracted_channel3->value()));
            ui->labelExtendedAdvanced_channel3          ->setText(QString::number(ui->horizontalSliderExtended_channel3 ->value()));

            ui->lineEditAccelarationTime_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel3  ->setEnabled(ui->checkBoxPwm_channel3->isChecked());

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
            ui->lineEditAccelarationTime_channel4       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel4       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel4      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationTime_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel4  ->setEnabled(ui->checkBoxPwm_channel4->isChecked());

            ui->labelRefractedAdvanced_channel4         ->setText(QString::number(ui->horizontalSliderRefracted_channel4->value()));
            ui->labelExtendedAdvanced_channel4          ->setText(QString::number(ui->horizontalSliderExtended_channel4 ->value()));

            ui->lineEditAccelarationTime_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel4  ->setEnabled(ui->checkBoxPwm_channel4->isChecked());

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
            ui->lineEditAccelarationTime_channel5       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel5       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel5      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationTime_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel5  ->setEnabled(ui->checkBoxPwm_channel5->isChecked());

            ui->labelRefractedAdvanced_channel5         ->setText(QString::number(ui->horizontalSliderRefracted_channel5->value()));
            ui->labelExtendedAdvanced_channel5          ->setText(QString::number(ui->horizontalSliderExtended_channel5 ->value()));

            ui->lineEditAccelarationTime_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel5  ->setEnabled(ui->checkBoxPwm_channel5->isChecked());

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
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 1) {
            _channel->setAccelarationTime( (uint16_t) ui->lineEditAccelarationTime_channel1->text().toUInt());
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL1));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel2_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 2){
            _channel->setAccelarationTime( (uint16_t) ui->lineEditAccelarationTime_channel2->text().toInt());
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL2));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel3_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 3){
            _channel->setAccelarationTime( (uint16_t) ui->lineEditAccelarationTime_channel3->text().toInt());
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL3));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel4_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 4){
            _channel->setAccelarationTime((uint16_t) ui->lineEditAccelarationTime_channel4->text().toInt());
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL4));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel5_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 5){
            _channel->setAccelarationTime( (uint16_t) ui->lineEditAccelarationTime_channel5->text().toInt());
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL5));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }
    updateJsonFile();
    setupTab();
}

/*  ------------ Functions for linear actuataor specifications button   -----------------------------------------   */
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel1_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 1){
            _channel->setModel       ( (string)     ui->lineEditModel_channel1         ->text().toStdString()  );
            _channel->setMaxCurrent  ( (float)      ui->lineEditMaxCurrent_channel1    ->text().toDouble()     );
            _channel->setStrokeLenght( (uint16_t)   ui->lineEditStrokeLenght_channel1  ->text().toUInt()       );
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL1));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
       }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel2_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 2){
            _channel->setModel       ( (string)     ui->lineEditModel_channel2         ->text().toStdString()  );
            _channel->setMaxCurrent  ( (float)      ui->lineEditMaxCurrent_channel2    ->text().toDouble()     );
            _channel->setStrokeLenght( (uint16_t)   ui->lineEditStrokeLenght_channel2  ->text().toInt()        );
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL2));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
       }
    }
    updateJsonFile();
    setupTab();
}
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel3_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 3){
            _channel->setModel       ( (string)     ui->lineEditModel_channel3        ->text().toStdString()  );
            _channel->setMaxCurrent  ( (float)      ui->lineEditMaxCurrent_channel3   ->text().toDouble()     );
            _channel->setStrokeLenght( (uint16_t)   ui->lineEditStrokeLenght_channel3 ->text().toInt()        );
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL3));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
       }
    }
    updateJsonFile();
    setupTab();
}
void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel4_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 4){
            _channel->setModel       ( (string)     ui->lineEditModel_channel4         ->text().toStdString()  );
            _channel->setMaxCurrent  ( (float)      ui->lineEditMaxCurrent_channel4    ->text().toDouble()     );
            _channel->setStrokeLenght( (uint16_t)   ui->lineEditStrokeLenght_channel4  ->text().toInt()        );
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL4));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
       }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel5_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 5){
            _channel->setModel       ( (string)     ui->lineEditModel_channel5         ->text().toStdString()  );
            _channel->setMaxCurrent  ( (float)      ui->lineEditMaxCurrent_channel5    ->text().toDouble()     );
            _channel->setStrokeLenght( (uint16_t)   ui->lineEditStrokeLenght_channel5  ->text().toInt()        );
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL5));
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
       }
    }
    updateJsonFile();
    setupTab();
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
    sendBasicCommand(PIN_CHANNEL1, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicExtended_channel2_clicked()
{
    sendBasicCommand(PIN_CHANNEL2, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicExtended_channel3_clicked()
{
    sendBasicCommand(PIN_CHANNEL3, BASICEXTENDED());

}

void MainWindow::on_pushButtonBasicExtended_channel4_clicked()
{
    sendBasicCommand(PIN_CHANNEL4, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicExtended_channel5_clicked()
{
    sendBasicCommand(PIN_CHANNEL5, BASICEXTENDED());
}

void MainWindow::on_pushButtonBasicRefracted_channel1_clicked()
{
    sendBasicCommand(PIN_CHANNEL1, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel2_clicked()
{
    sendBasicCommand(PIN_CHANNEL2, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel3_clicked()
{
    sendBasicCommand(PIN_CHANNEL3, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel4_clicked()
{
    sendBasicCommand(PIN_CHANNEL4, BASICREFRACTED());
}

void MainWindow::on_pushButtonBasicRefracted_channel5_clicked()
{
    sendBasicCommand(PIN_CHANNEL5, BASICREFRACTED());
}

void MainWindow::on_pushButtonStop_channel1_clicked()
{
    sendBasicCommand(PIN_CHANNEL1, STOP());
}

void MainWindow::on_pushButtonStop_channel2_clicked()
{
    sendBasicCommand(PIN_CHANNEL2, STOP());
}

void MainWindow::on_pushButtonStop_channel3_clicked()
{
    sendBasicCommand(PIN_CHANNEL3, STOP());
}

void MainWindow::on_pushButtonStop_channel4_clicked()
{
    sendBasicCommand(PIN_CHANNEL4, STOP());
}

void MainWindow::on_pushButtonStop_channel5_clicked()
{
    sendBasicCommand(PIN_CHANNEL5, STOP());
}

void MainWindow::on_pushButtonSafeReset_channel1_clicked()
{
    sendBasicCommand(PIN_CHANNEL1, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel2_clicked()
{
    sendBasicCommand(PIN_CHANNEL2, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel3_clicked()
{
    sendBasicCommand(PIN_CHANNEL3, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel4_clicked()
{
    sendBasicCommand(PIN_CHANNEL4, SAFERESET());
}

void MainWindow::on_pushButtonSafeReset_channel5_clicked()
{
    sendBasicCommand(PIN_CHANNEL5, SAFERESET());
}

void MainWindow::on_pushButtonInitialSetup_channel1_clicked()
{
    sendBasicCommand(PIN_CHANNEL1, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel2_clicked()
{
    sendBasicCommand(PIN_CHANNEL2, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel3_clicked()
{
    sendBasicCommand(PIN_CHANNEL3, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel4_clicked()
{
    sendBasicCommand(PIN_CHANNEL4, INITIALSETUP());
}

void MainWindow::on_pushButtonInitialSetup_channel5_clicked()
{
    sendBasicCommand(PIN_CHANNEL5, INITIALSETUP());
}

/*  ------------ Functions advanced buttons (Extended, Refracted)   -----------------------------------------   */

void MainWindow::on_pushButtonExtended_channel1_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL1, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel1->value());
}

void MainWindow::on_pushButtonExtended_channel2_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL2, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel2->value());
}

void MainWindow::on_pushButtonExtended_channel3_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL3, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel3->value());
}
void MainWindow::on_pushButtonExtended_channel4_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL4, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel4->value());
}

void MainWindow::on_pushButtonExtended_channel5_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL5, ADVANCEDEXTENDED(), (uint8_t) ui->horizontalSliderExtended_channel5->value());
}


void MainWindow::on_pushButtonRefracted_channel1_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL1, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel1->value());
}

void MainWindow::on_pushButtonRefracted_channel2_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL2, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel2->value());
}

void MainWindow::on_pushButtonRefracted_channel3_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL3, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel3->value());
}

void MainWindow::on_pushButtonRefracted_channel4_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL4, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel4->value());
}

void MainWindow::on_pushButtonRefracted_channel5_clicked()
{
    sendAdvancedCommand(PIN_CHANNEL5, ADVANCEDREFRACTED(), (uint8_t) ui->horizontalSliderRefracted_channel5->value());
}

void MainWindow::on_checkBoxPwm_channel1_stateChanged(int arg1)
{
    for (Channel* _channel: getListOfChannels()){
        if (_channel->getChannelNumber() == 1){
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL1));
            if (arg1 == Qt::Unchecked)
                _channel->setSoftStartStop( (uint8_t) 0);
            else if (arg1 == Qt::Checked)
                _channel->setSoftStartStop( (uint8_t) 1);
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }

    updateJsonFile();
    setupTab();
}

void MainWindow::on_checkBoxPwm_channel2_stateChanged(int arg1)
{
    for (Channel* _channel: getListOfChannels()){
        if (_channel->getChannelNumber() == 2){
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL2));
            if (arg1 == Qt::Unchecked)
                _channel->setSoftStartStop( (uint8_t) 0);
            else if (arg1 == Qt::Checked)
                _channel->setSoftStartStop( (uint8_t) 1);
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }

    updateJsonFile();
    setupTab();
}

void MainWindow::on_checkBoxPwm_channel3_stateChanged(int arg1)
{
    for (Channel* _channel: getListOfChannels()){
        if (_channel->getChannelNumber() == 3){
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL3));
            if (arg1 == Qt::Unchecked)
                _channel->setSoftStartStop( (uint8_t) 0);
            else if (arg1 == Qt::Checked)
                _channel->setSoftStartStop( (uint8_t) 1);
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }

    updateJsonFile();
    setupTab();
}

void MainWindow::on_checkBoxPwm_channel4_stateChanged(int arg1)
{
    for (Channel* _channel: getListOfChannels()){
        if (_channel->getChannelNumber() == 4){
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL4));
            if (arg1 == Qt::Unchecked)
                _channel->setSoftStartStop( (uint8_t) 0);
            else if (arg1 == Qt::Checked)
                _channel->setSoftStartStop( (uint8_t) 1);
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }

    updateJsonFile();
    setupTab();
}

void MainWindow::on_checkBoxPwm_channel5_stateChanged(int arg1)
{
    for (Channel* _channel: getListOfChannels()){
        if (_channel->getChannelNumber() == 5){
            _channel->setPulseRate(disableCommunication(PIN_CHANNEL5));
            if (arg1 == Qt::Unchecked)
                _channel->setSoftStartStop( (uint8_t) 0);
            else if (arg1 == Qt::Checked)
                _channel->setSoftStartStop( (uint8_t) 1);
            sendUpdateCommand(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationTime(), _channel->getSoftStartStop());
        }
    }

    updateJsonFile();
    setupTab();
}

void MainWindow::on_tabChannel_currentChanged(int index)
{
    restartCommunication();
}
