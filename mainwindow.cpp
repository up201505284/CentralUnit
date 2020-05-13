#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Utils.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(QFile::exists("./RecordsOfChannels.js"))
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
    restartCommunication();
    for (Channel* _channel : getListOfChannels())
        disableCommunication(_channel->getPin());
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
    for (int i=0; i<5; i++){
        Channel* _channel = new Channel(i+1);
        _listOfActuators.append(_channel);
    }
    setListOfChannels(_listOfActuators);
}


//  Json File

void MainWindow::readJsonFile(void)
{
    QFile jsonFile(JSON_FILE_NAME);
    jsonFile.open(QFile::ReadOnly);
    QJsonArray _channelsJson = QJsonDocument().fromJson(jsonFile.readAll()).array();
    jsonFile.close();

    QList<Channel*> _listOfChannels;

    foreach (const QJsonValue & _channelJson, _channelsJson){
        Channel* _channel = new Channel(_channelJson.toObject().value("Channel Number").toString().toInt());

        _channel->setModel              (_channelJson.toObject().value("Model"              ).toString().toStdString()  );
        _channel->setCommunication      (_channelJson.toObject().value("Communication"      ).toString().toStdString()  );
        _channel->setStrokeLenght       (_channelJson.toObject().value("Stroke Lenght"      ).toString().toInt()        );
        _channel->setMaxCurrent         (_channelJson.toObject().value("Maximum Current"    ).toString().toDouble()     );
        _channel->setAccelarationTime   (_channelJson.toObject().value("Accelaration Time"  ).toString().toInt()        );
        _channel->setAccelarationRate   (_channelJson.toObject().value("Accelaration Rate"  ).toString().toDouble()     );
        _channel->setPulseRate          (_channelJson.toObject().value("Pulse Rate"         ).toString().toInt()        );
        _channel->setPin                (_channelJson.toObject().value("Pin"                ).toString().toInt()        );

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
        _channelJson.insert("Model"             , QString::fromStdString(_channel->getModel()           ));
        _channelJson.insert("Communication"     , QString::fromStdString(_channel->getCommunication()   ));
        _channelJson.insert("Stroke Lenght"     , QString::number       (_channel->getStrokeLenght()    ));
        _channelJson.insert("Maximum Current"   , QString::number       (_channel->getMaxCurrent()      ));
        _channelJson.insert("Accelaration Time" , QString::number       (_channel->getAccelarationTime()));
        _channelJson.insert("Accelaration Rate" , QString::number       (_channel->getAccelarationRate()));
        _channelJson.insert("Channel Number"    , QString::number       (_channel->getChannelNumber()   ));
        _channelJson.insert("Pulse Rate"        , QString::number       (_channel->getPulseRate()       ));
        _channelJson.insert("Pin"               , QString::number       (_channel->getPin()             ));

        channelsJson.push_back(_channelJson);
    }

    QJsonDocument recordsOfChannels(channelsJson);
    jsonFile.write(recordsOfChannels.toJson());
    jsonFile.close();
}

void MainWindow::checkCommunications (void) {
    bool controlEnbale[5];
    controlEnbale[0] = ui->checkBoxPwm_channel1->isChecked();
    controlEnbale[1] = ui->checkBoxPwm_channel2->isChecked();
    controlEnbale[2] = ui->checkBoxPwm_channel3->isChecked();
    controlEnbale[3] = ui->checkBoxPwm_channel4->isChecked();
    controlEnbale[4] = ui->checkBoxPwm_channel5->isChecked();

    for (Channel* _channel: getListOfChannels()) {
        if (enableCommunication(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationRate(), _channel->getAccelarationTime(), controlEnbale[_channel->getChannelNumber()-1]) == 1)
            _channel->setCommunication("Enable");
        else
            _channel->setCommunication("Disable");
    }
}

void MainWindow::restartCommunication(void) {
    for (Channel* _channel: getListOfChannels()) {
        uint8_t pulseRate = disableCommunication(_channel->getPin());
        if (pulseRate != _channel->getPulseRate())
            _channel->setPulseRate(pulseRate);
    }

    checkCommunications();
}

void MainWindow::setupTab(void)
{
    for(Channel* _channel : getListOfChannels()){

        if(_channel->getChannelNumber() == 1){
            ui->lineEditModel_channel1                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel1             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel1           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel1       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel1       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel1       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel1      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
            ui->lineEditAccelarationTime_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel1  ->setEnabled(ui->checkBoxPwm_channel1->isChecked());

            setupSlider();

            if (_channel->getCommunication() == "Enable")
                ui->channel1->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel1->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 2){
            ui->lineEditModel_channel2                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel2             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel2           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel2       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel2       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel2       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel2      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->lineEditAccelarationTime_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel2  ->setEnabled(ui->checkBoxPwm_channel2->isChecked());

            setupSlider();

            if (_channel->getCommunication() == "Enable")
                ui->channel2->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel2->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 3){
            ui->lineEditModel_channel3                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel3             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel3           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel3       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel3       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel3       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel3      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->lineEditAccelarationTime_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel3  ->setEnabled(ui->checkBoxPwm_channel3->isChecked());

            setupSlider();

            if (_channel->getCommunication() == "Enable")
                ui->channel3->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel3->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 4){
            ui->lineEditModel_channel4                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel4             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel4           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel4       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel4       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel4       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel4      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->lineEditAccelarationTime_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel4  ->setEnabled(ui->checkBoxPwm_channel4->isChecked());

            setupSlider();

            if (_channel->getCommunication() == "Enable")
                ui->channel4->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel4->setEnabled(false);
        }

        else if(_channel->getChannelNumber() == 5){
            ui->lineEditModel_channel5                  ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel5             ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel5           ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel5       ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel5       ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel5       ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel5      ->setRange(0, _channel->getStrokeLenght());

            ui->lineEditAccelarationRate_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->lineEditAccelarationTime_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
            ui->pushButtonUpdateStartStopSoft_channel5  ->setEnabled(ui->checkBoxPwm_channel5->isChecked());

            setupSlider();

            if (_channel->getCommunication() == "Enable")
                ui->channel5->setEnabled(true);
            else if(_channel->getCommunication() == "Disable")
                ui->channel5->setEnabled(false);
        }
    }
}

void MainWindow::setupSlider(void)
{
    ui->labelRefractedAdvanced_channel1 ->setText(QString::number(ui->horizontalSliderRefracted_channel1->value()));
    ui->labelExtendedAdvanced_channel1  ->setText(QString::number(ui->horizontalSliderExtended_channel1 ->value()));
    ui->labelRefractedAdvanced_channel2 ->setText(QString::number(ui->horizontalSliderRefracted_channel2->value()));
    ui->labelExtendedAdvanced_channel2  ->setText(QString::number(ui->horizontalSliderExtended_channel2 ->value()));
    ui->labelRefractedAdvanced_channel3 ->setText(QString::number(ui->horizontalSliderRefracted_channel3->value()));
    ui->labelExtendedAdvanced_channel3  ->setText(QString::number(ui->horizontalSliderExtended_channel3 ->value()));
    ui->labelRefractedAdvanced_channel4 ->setText(QString::number(ui->horizontalSliderRefracted_channel4->value()));
    ui->labelExtendedAdvanced_channel4  ->setText(QString::number(ui->horizontalSliderExtended_channel4 ->value()));
    ui->labelRefractedAdvanced_channel5 ->setText(QString::number(ui->horizontalSliderRefracted_channel5->value()));
    ui->labelExtendedAdvanced_channel5  ->setText(QString::number(ui->horizontalSliderExtended_channel5 ->value()));
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel1_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 1){
            _channel->setModel       (ui->lineEditModel_channel1         ->text().toStdString()  );
            _channel->setMaxCurrent  (ui->lineEditMaxCurrent_channel1    ->text().toDouble()     );
            _channel->setStrokeLenght(ui->lineEditStrokeLenght_channel1  ->text().toInt()        );
       }
    }
    updateJsonFile();
    restartCommunication();
    setupTab();
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel1_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 1){
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel1->text().toInt()  );
            _channel->setAccelarationTime(ui->lineEditAccelarationTime_channel1->text().toInt()     );
        }
    }

    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel2_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 2){
            _channel->setModel       (ui->lineEditModel_channel2         ->text().toStdString()  );
            _channel->setMaxCurrent  (ui->lineEditMaxCurrent_channel2    ->text().toDouble()     );
            _channel->setStrokeLenght(ui->lineEditStrokeLenght_channel2  ->text().toInt()        );
       }
    }
    updateJsonFile();
    setupTab();
}
void MainWindow::on_pushButtonUpdateStartStopSoft_channel2_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 2){
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel2->text().toInt()  );
            _channel->setAccelarationTime(ui->lineEditAccelarationTime_channel2->text().toInt()     );
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel3_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 3){
            _channel->setModel       (ui->lineEditModel_channel3        ->text().toStdString()  );
            _channel->setMaxCurrent  (ui->lineEditMaxCurrent_channel3   ->text().toDouble()     );
            _channel->setStrokeLenght(ui->lineEditStrokeLenght_channel3 ->text().toInt()        );
       }
    }
    updateJsonFile();
    setupTab();
}
void MainWindow::on_pushButtonUpdateStartStopSoft_channel3_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 3){
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel3->text().toInt()  );
            _channel->setAccelarationTime(ui->lineEditAccelarationTime_channel3->text().toInt()  );
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel4_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 4){
            _channel->setModel       (ui->lineEditModel_channel4         ->text().toStdString()  );
            _channel->setMaxCurrent  (ui->lineEditMaxCurrent_channel4    ->text().toDouble()     );
            _channel->setStrokeLenght(ui->lineEditStrokeLenght_channel4  ->text().toInt()        );
       }
    }
    updateJsonFile();
    setupTab();
}
void MainWindow::on_pushButtonUpdateStartStopSoft_channel4_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 4){
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel4->text().toInt()  );
            _channel->setAccelarationTime(ui->lineEditAccelarationTime_channel4->text().toInt()     );
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_pushButtonUpdateLinearActuatorSpecifications_channel5_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 5){
            _channel->setModel       (ui->lineEditModel_channel5         ->text().toStdString()  );
            _channel->setMaxCurrent  (ui->lineEditMaxCurrent_channel5    ->text().toDouble()     );
            _channel->setStrokeLenght(ui->lineEditStrokeLenght_channel5  ->text().toInt()        );
       }
    }
    updateJsonFile();
    setupTab();
}
void MainWindow::on_pushButtonUpdateStartStopSoft_channel5_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 5){
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel5->text().toInt()  );
            _channel->setAccelarationTime(ui->lineEditAccelarationTime_channel5->text().toInt()  );
        }
    }
    updateJsonFile();
    setupTab();
}

void MainWindow::on_horizontalSliderExtended_channel5_valueChanged(int value)
{
     ui->labelExtendedAdvanced_channel5  ->setText(QString::number(value));
}

void MainWindow::on_horizontalSliderRefracted_channel5_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel5  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel4_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel5  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel4_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel4  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel3_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel3  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel3_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel3  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel2_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel5  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel2_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel2  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderExtended_channel1_valueChanged(int value)
{
    ui->labelExtendedAdvanced_channel1  ->setText(QString::number(value));

}

void MainWindow::on_horizontalSliderRefracted_channel1_valueChanged(int value)
{
    ui->labelRefractedAdvanced_channel1  ->setText(QString::number(value));

}

void MainWindow::on_pushButtonBasicExtended_channel1_clicked()
{
    spiEnableSlave(PIN_CHANNEL1);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICEXTENDED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL1);
}
void MainWindow::on_pushButtonBasicExtended_channel2_clicked()
{
    spiEnableSlave(PIN_CHANNEL2);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICEXTENDED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL2);
}
void MainWindow::on_pushButtonBasicExtended_channel3_clicked()
{
    spiEnableSlave(PIN_CHANNEL3);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICEXTENDED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL3);
}
void MainWindow::on_pushButtonBasicExtended_channel4_clicked()
{
    spiEnableSlave(PIN_CHANNEL4);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICEXTENDED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL4);
}
void MainWindow::on_pushButtonBasicExtended_channel5_clicked()
{
    spiEnableSlave(PIN_CHANNEL5);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICEXTENDED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL5);
}

void MainWindow::on_pushButtonBasicRefracted_channel1_clicked()
{
    spiEnableSlave(PIN_CHANNEL1);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICREFRACTED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL1);
}
void MainWindow::on_pushButtonBasicRefracted_channel2_clicked()
{
    spiEnableSlave(PIN_CHANNEL2);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICREFRACTED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL2);
}
void MainWindow::on_pushButtonBasicRefracted_channel3_clicked()
{
    spiEnableSlave(PIN_CHANNEL3);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICREFRACTED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL3);
}
void MainWindow::on_pushButtonBasicRefracted_channel4_clicked()
{
    spiEnableSlave(PIN_CHANNEL4);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICREFRACTED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL4);;
}

void MainWindow::on_pushButtonBasicRefracted_channel5_clicked()
{
    spiEnableSlave(PIN_CHANNEL5);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = BASICREFRACTED();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL5);
}

void MainWindow::on_pushButtonStop_channel1_clicked()
{
    spiEnableSlave(PIN_CHANNEL1);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = STOP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL1);
}
void MainWindow::on_pushButtonStop_channel2_clicked()
{
    spiEnableSlave(PIN_CHANNEL2);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = STOP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL2);
}
void MainWindow::on_pushButtonStop_channel3_clicked()
{
    spiEnableSlave(PIN_CHANNEL3);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = STOP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL3);
}
void MainWindow::on_pushButtonStop_channel4_clicked()
{
    spiEnableSlave(PIN_CHANNEL4);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = STOP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL4);
}
void MainWindow::on_pushButtonStop_channel5_clicked()
{
    spiEnableSlave(PIN_CHANNEL5);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = STOP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL5);
}

void MainWindow::on_pushButtonSafeReset_channel1_clicked()
{
    spiEnableSlave(PIN_CHANNEL1);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = SAFERESET();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL1);
}
void MainWindow::on_pushButtonSafeReset_channel2_clicked()
{
    spiEnableSlave(PIN_CHANNEL2);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = SAFERESET();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL2);
}
void MainWindow::on_pushButtonSafeReset_channel3_clicked()
{
    spiEnableSlave(PIN_CHANNEL3);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = SAFERESET();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL3);
}
void MainWindow::on_pushButtonSafeReset_channel4_clicked()
{
    spiEnableSlave(PIN_CHANNEL4);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = SAFERESET();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL4);
}
void MainWindow::on_pushButtonSafeReset_channel5_clicked()
{
    spiEnableSlave(PIN_CHANNEL5);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = SAFERESET();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL5);
}

void MainWindow::on_pushButtonInitialSetup_channel1_clicked()
{
    spiEnableSlave(PIN_CHANNEL1);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = INITIALSETUP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL1);
}
void MainWindow::on_pushButtonInitialSetup_channel2_clicked()
{
    spiEnableSlave(PIN_CHANNEL2);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = INITIALSETUP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL2);
}

void MainWindow::on_pushButtonInitialSetup_channel3_clicked()
{
    spiEnableSlave(PIN_CHANNEL3);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = INITIALSETUP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL3);
}
void MainWindow::on_pushButtonInitialSetup_channel4_clicked()
{
    spiEnableSlave(PIN_CHANNEL4);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = INITIALSETUP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL4);
}
void MainWindow::on_pushButtonInitialSetup_channel5_clicked()
{
    spiEnableSlave(PIN_CHANNEL5);
    char bufferT [SIZE_BUFFER_BASIC]    = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = INITIALSETUP();
    bufferT[SIZE_BUFFER_BASIC - 1]      = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_BASIC);

    spiDisableSlave(PIN_CHANNEL5);
}

void MainWindow::on_pushButtonExtended_channel1_clicked()
{
    spiEnableSlave(PIN_CHANNEL1);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDEXTENDED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderExtended_channel1->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL1);
}
void MainWindow::on_pushButtonExtended_channel2_clicked()
{
    spiEnableSlave(PIN_CHANNEL2);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDEXTENDED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderExtended_channel2->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL2);
}
void MainWindow::on_pushButtonExtended_channel3_clicked()
{
    spiEnableSlave(PIN_CHANNEL3);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDEXTENDED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderExtended_channel3->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL3);
}
void MainWindow::on_pushButtonExtended_channel4_clicked()
{
    spiEnableSlave(PIN_CHANNEL4);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDEXTENDED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderExtended_channel4->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL4);
}
void MainWindow::on_pushButtonExtended_channel5_clicked()
{
    spiEnableSlave(PIN_CHANNEL5);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDEXTENDED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderExtended_channel5->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL5);
}

void MainWindow::on_pushButtonRefracted_channel1_clicked()
{
    spiEnableSlave(PIN_CHANNEL1);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDREFRACTED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderRefracted_channel1->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL1);
}

void MainWindow::on_pushButtonRefracted_channel2_clicked()
{
    spiEnableSlave(PIN_CHANNEL2);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDREFRACTED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderRefracted_channel2->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL2);
}

void MainWindow::on_pushButtonRefracted_channel3_clicked()
{
    spiEnableSlave(PIN_CHANNEL3);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDREFRACTED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderRefracted_channel3->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL3);
}

void MainWindow::on_pushButtonRefracted_channel4_clicked()
{
    spiEnableSlave(PIN_CHANNEL4);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDREFRACTED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderRefracted_channel4->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL4);
}

void MainWindow::on_pushButtonRefracted_channel5_clicked()
{
    spiEnableSlave(PIN_CHANNEL5);

    char bufferT [SIZE_BUFFER_ADVANCED] = {};
    bufferT[INITIAL]                    = SPICOMMANDINITIAL();
    bufferT[MODE]                       = ADVANCEDREFRACTED();
    bufferT[SHIFT]                      = (char) ui->horizontalSliderRefracted_channel5->value();
    bufferT[SIZE_BUFFER_ADVANCED - 1]   = SPICOMMANDFINISHED();

    bcm2835_spi_transfern(bufferT, SIZE_BUFFER_ADVANCED);

    spiDisableSlave(PIN_CHANNEL5);
}

void MainWindow::on_checkBoxPwm_channel1_stateChanged(int arg1)
{
    ui->lineEditAccelarationRate_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
    ui->lineEditAccelarationTime_channel1       ->setEnabled(ui->checkBoxPwm_channel1->isChecked());
    ui->pushButtonUpdateStartStopSoft_channel1  ->setEnabled(ui->checkBoxPwm_channel1->isChecked());

    for (Channel* _channel : getListOfChannels()) {
        int pwmControl;
        if (ui->checkBoxPwm_channel1->isChecked() == false)
            pwmControl = 0;
        else
            pwmControl = 1;

        if (_channel->getChannelNumber() == 1 ) {
            if (enableCommunication(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationRate(), _channel->getAccelarationTime(), pwmControl)  == 1)
                _channel->setCommunication("Enable");
            else
                _channel->setCommunication("Disable");
        }
    }
}
void MainWindow::on_checkBoxPwm_channel2_stateChanged(int arg1)
{
    ui->lineEditAccelarationRate_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
    ui->lineEditAccelarationTime_channel2       ->setEnabled(ui->checkBoxPwm_channel2->isChecked());
    ui->pushButtonUpdateStartStopSoft_channel2  ->setEnabled(ui->checkBoxPwm_channel2->isChecked());

    disableCommunication(PIN_CHANNEL2);

    for (Channel* _channel : getListOfChannels()) {
        int pwmControl;
        if (ui->checkBoxPwm_channel2->isChecked() == false)
            pwmControl = 0;
        else
            pwmControl = 1;

        if (_channel->getChannelNumber() == 2 ) {
            if (enableCommunication(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationRate(), _channel->getAccelarationTime(), pwmControl)  == 1)
                _channel->setCommunication("Enable");
            else
                _channel->setCommunication("Disable");
        }
    }
}
void MainWindow::on_checkBoxPwm_channel3_stateChanged(int arg1)
{
    ui->lineEditAccelarationRate_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
    ui->lineEditAccelarationTime_channel3       ->setEnabled(ui->checkBoxPwm_channel3->isChecked());
    ui->pushButtonUpdateStartStopSoft_channel3  ->setEnabled(ui->checkBoxPwm_channel3->isChecked());

    for (Channel* _channel : getListOfChannels()) {
        int pwmControl;
        if (ui->checkBoxPwm_channel3->isChecked() == false)
            pwmControl = 0;
        else
            pwmControl = 1;

        if (_channel->getChannelNumber() == 3 ) {
            if (enableCommunication(_channel->getPin(), _channel->getStrokeLenght(), _channel->getPulseRate(), _channel->getAccelarationRate(), _channel->getAccelarationTime(), pwmControl)  == 1)
                _channel->setCommunication("Enable");
            else
                _channel->setCommunication("Disable");
        }
    }
}
void MainWindow::on_checkBoxPwm_channel4_stateChanged(int arg1)
{
    ui->lineEditAccelarationRate_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
    ui->lineEditAccelarationTime_channel4       ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
    ui->pushButtonUpdateStartStopSoft_channel4  ->setEnabled(ui->checkBoxPwm_channel4->isChecked());
}
void MainWindow::on_checkBoxPwm_channel5_stateChanged(int arg1)
{
    ui->lineEditAccelarationRate_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
    ui->lineEditAccelarationTime_channel5       ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
    ui->pushButtonUpdateStartStopSoft_channel5  ->setEnabled(ui->checkBoxPwm_channel5->isChecked());
}

void MainWindow::on_tabChannel_currentChanged(int index)
{
    if (index >= 1 && index <= 5) {
        restartCommunication();
        setupTab();
    }
}
