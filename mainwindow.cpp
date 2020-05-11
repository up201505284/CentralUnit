#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Utils.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <bcm2835.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(QFile::exists("./RecordsOfChannels.js")){
        readJsonFile();
    }

    else{
        initListOfActuators();
        updateJsonFile();
        readJsonFile();
    }
    setupTab();
}

MainWindow::~MainWindow()
{
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
    QFile jsonFile("./RecordsOfChannels.js");
    jsonFile.open(QFile::ReadOnly);
    QJsonArray _channelsJson = QJsonDocument().fromJson(jsonFile.readAll()).array();
    jsonFile.close();

    QList<Channel*> _listOfChannels;

    foreach (const QJsonValue & _channelJson, _channelsJson){
        Channel* _channel = new Channel(_channelJson.toObject().value("Channel Number").toString().toInt());
        _channel->setModel(_channelJson.toObject().value("Model").toString().toStdString());
        _channel->setCommunication(_channelJson.toObject().value("Communication").toString().toStdString());
        _channel->setStrokeLenght(_channelJson.toObject().value("Stroke Lenght").toString().toInt());
        _channel->setMaxCurrent(_channelJson.toObject().value("Maximum Current").toString().toDouble());
        _channel->setAccelarationTime(_channelJson.toObject().value("Accelaration Time").toString().toInt());
        _channel->setAccelarationRate(_channelJson.toObject().value("Accelaration Rate").toString().toDouble());
        _channel->setPulseRate(_channelJson.toObject().value("Pulse Rate").toString().toInt());
        _listOfChannels.append(_channel);
    }

    setListOfChannels(_listOfChannels);
}

void MainWindow::updateJsonFile (void)
{
    QFile jsonFile("RecordsOfChannels.js");
    jsonFile.open(QFile::WriteOnly);
    QJsonArray channelsJson;

    for (Channel* _channel:getListOfChannels()){
        QJsonObject _channelJson;
        _channelJson.insert("Model", QString::fromStdString(_channel->getModel()));
        _channelJson.insert("Communication", QString::fromStdString(_channel->getCommunication()));
        _channelJson.insert("Stroke Lenght", QString::number(_channel->getStrokeLenght()));
        _channelJson.insert("Maximum Current", QString::number(_channel->getMaxCurrent()));
        _channelJson.insert("Accelaration Time", QString::number(_channel->getAccelarationTime()));
        _channelJson.insert("Accelaration Rate", QString::number(_channel->getAccelarationRate()));
        _channelJson.insert("Channel Number", QString::number(_channel->getChannelNumber()));
        _channelJson.insert("Pulse Rate", QString::number(_channel->getPulseRate()));

        channelsJson.push_back(_channelJson);
    }

    QJsonDocument recordsOfChannels(channelsJson);
    jsonFile.write(recordsOfChannels.toJson());
    jsonFile.close();
}

void MainWindow::setupTab(void)
{
    for(Channel* _channel : getListOfChannels()){

        if(_channel->getChannelNumber() == 1){
            ui->lineEditModel_channel1              ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel1         ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel1       ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel1   ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel1   ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel1   ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel1  ->setRange(0, _channel->getStrokeLenght());

            setupSlider();
        }
        else if(_channel->getChannelNumber() == 2){
            ui->lineEditModel_channel2              ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel2         ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel2       ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel2   ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel2   ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel2   ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel2  ->setRange(0, _channel->getStrokeLenght());

            setupSlider();
        }

        else if(_channel->getChannelNumber() == 3){
            ui->lineEditModel_channel3              ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel3         ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel3       ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel3   ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel3   ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel3   ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel3  ->setRange(0, _channel->getStrokeLenght());

            setupSlider();
        }
        else if(_channel->getChannelNumber() == 4){
            ui->lineEditModel_channel4              ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel4         ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel4       ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel4   ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel4   ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel4   ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel4  ->setRange(0, _channel->getStrokeLenght());

            setupSlider();

        }

        else if(_channel->getChannelNumber() == 5){
            ui->lineEditModel_channel5              ->setText(QString::fromStdString(_channel->getModel()            ));
            ui->lineEditMaxCurrent_channel5         ->setText(QString::number       (_channel->getMaxCurrent()       ));
            ui->lineEditStrokeLenght_channel5       ->setText(QString::number       (_channel->getStrokeLenght()     ));
            ui->lineEditAccelarationRate_channel5   ->setText(QString::number       (_channel->getAccelarationRate() ));
            ui->lineEditAccelarationTime_channel5   ->setText(QString::number       (_channel->getAccelarationTime() ));

            ui->horizontalSliderExtended_channel5   ->setRange(0, _channel->getStrokeLenght());
            ui->horizontalSliderRefracted_channel5  ->setRange(0, _channel->getStrokeLenght());

            setupSlider();
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
    setupTab();
}

void MainWindow::on_pushButtonUpdateStartStopSoft_channel1_clicked()
{
    for (Channel* _channel : getListOfChannels()){
        if (_channel->getChannelNumber() == 1){
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel1->text().toDouble()  );
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
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel2->text().toDouble()  );
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
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel3->text().toDouble()  );
            _channel->setAccelarationTime(ui->lineEditAccelarationTime_channel3->text().toInt()     );
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
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel4->text().toDouble()  );
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
            _channel->setAccelarationRate(ui->lineEditAccelarationRate_channel5->text().toDouble()  );
            _channel->setAccelarationTime(ui->lineEditAccelarationTime_channel5->text().toInt()     );
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

