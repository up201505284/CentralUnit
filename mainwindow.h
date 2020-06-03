#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QFileDialog>
#include <QDir>
#include "Channel.h"

#define JSON_FILE_NAME              "RecordsOfChannels.js"
#define JSON_FILE_MOVES             "RecordsMoves_"

#define MAX_NUMBER_CHANNELS         5
#define MAX_NUMBER_MOVES            10
#define DELAY                       5

#define CHANNEL1                    1
#define CHANNEL2                    2
#define CHANNEL3                    3
#define CHANNEL4                    4
#define CHANNEL5                    5

#define TAB_CHANNEL1                0
#define TAB_CHANNEL2                1
#define TAB_CHANNEL3                2
#define TAB_CHANNEL4                3
#define TAB_CHANNEL5                4
#define TAB_COMMAND                 5
#define TAB_TESTS                   6
#define TAB_SEQUENCE                6

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        // "Set" methods
        void setListOfChannels      (QList<Channel*> _listOfChannels);
        void setLastMoveInSequence  (int _move);

        // "Get" methods
        QList<Channel*>  getListOfChannels      (void);
        int              getLastMoveInSequence  (void);

        //  Init methods
        void        initListOfActuators (void);
        void        checkCommunications (void);
        void        restartCommunication(uint8_t _channel);
        Channel*    getChannel          (uint8_t _channel);

        //  Json File
        void readJsonFile    (void);
        void updateJsonFile  (void);
        void createJsonMoves (QString _jsonFileName);

        //  Setup channels tab and command tab
        void setupTab                   (void);

        void setupSequenceTab           (void);

        //  Update linear actuator specifications
        void ButtonUpdateLinearActuator (uint8_t _ch, uint16_t _strokeLenght, string _model, float _maxCurrent);

        //  Update soft start stop
        void ButtonUpdateSoftStartStop  (uint8_t _ch, uint16_t _accelarationRate, uint8_t _speed);

        //  Enable/Disabçe soft start stop
        void CheckBoxSoftStartStop      (uint8_t _ch, uint8_t _softStartStop);

        //  Update position and pulse rate
        void Update                     (void);

        //  Setup comboBoxMaster/comboBoxSlave with all enable channeçs
        void setupCombBoxMasterChannel  (uint8_t _move);
        void setupCombBoxSlaveChannel   (uint8_t _move);


    private slots:
        //  Add new move widget to sequence tab
        void on_newButton               (void);

        //  Start new seuence
        void on_startButton             (void);

        //  Enable/Disable slave channels in sequence tab
        void changed_combBoxSynchronous (void);

        //  Update values of slider label in sequece tab
        void changed_slider             (void);

        //  Update max value of sliders in sequece tab
        void changed_combBoxMaster      (QString);

        //   Save sequence to json file
        void on_saveButton              (void);

        //  Upload sequence
        void on_uploadButton             (void);

        // Update button for linear specificaitons
        void    on_pushButtonUpdateLinearActuatorSpecifications_channel1_clicked (void);
        void    on_pushButtonUpdateLinearActuatorSpecifications_channel2_clicked (void);
        void    on_pushButtonUpdateLinearActuatorSpecifications_channel3_clicked (void);
        void    on_pushButtonUpdateLinearActuatorSpecifications_channel4_clicked (void);
        void    on_pushButtonUpdateLinearActuatorSpecifications_channel5_clicked (void);

        // Update button for start stop
        void    on_pushButtonUpdateStartStopSoft_channel1_clicked    (void);
        void    on_pushButtonUpdateStartStopSoft_channel2_clicked    (void);
        void    on_pushButtonUpdateStartStopSoft_channel3_clicked    (void);
        void    on_pushButtonUpdateStartStopSoft_channel4_clicked    (void);
        void    on_pushButtonUpdateStartStopSoft_channel5_clicked    (void);

        //  Horizontal Sliders for extended
        void    on_horizontalSliderExtended_channel1_valueChanged    (int value);
        void    on_horizontalSliderExtended_channel2_valueChanged    (int value);
        void    on_horizontalSliderExtended_channel3_valueChanged    (int value);
        void    on_horizontalSliderExtended_channel4_valueChanged    (int value);
        void    on_horizontalSliderExtended_channel5_valueChanged    (int value);

        //  Horizontal Sliders for refracted
        void    on_horizontalSliderRefracted_channel1_valueChanged   (int value);
        void    on_horizontalSliderRefracted_channel2_valueChanged   (int value);
        void    on_horizontalSliderRefracted_channel3_valueChanged   (int value);
        void    on_horizontalSliderRefracted_channel4_valueChanged   (int value);
        void    on_horizontalSliderRefracted_channel5_valueChanged   (int value);

        //  Basic extended buttons
        void    on_pushButtonBasicExtended_channel1_clicked  (void);
        void    on_pushButtonBasicExtended_channel2_clicked  (void);
        void    on_pushButtonBasicExtended_channel3_clicked  (void);
        void    on_pushButtonBasicExtended_channel4_clicked  (void);
        void    on_pushButtonBasicExtended_channel5_clicked  (void);

        //  Basic refracted buttons
        void    on_pushButtonBasicRefracted_channel5_clicked (void);
        void    on_pushButtonBasicRefracted_channel4_clicked (void);
        void    on_pushButtonBasicRefracted_channel3_clicked (void);
        void    on_pushButtonBasicRefracted_channel1_clicked (void);
        void    on_pushButtonBasicRefracted_channel2_clicked (void);

        //  Stop buttons
        void    on_pushButtonStop_channel1_clicked   (void);
        void    on_pushButtonStop_channel2_clicked   (void);
        void    on_pushButtonStop_channel3_clicked   (void);
        void    on_pushButtonStop_channel4_clicked   (void);
        void    on_pushButtonStop_channel5_clicked   (void);

        //  Safe reset buttons
        void    on_pushButtonSafeReset_channel1_clicked  (void);
        void    on_pushButtonSafeReset_channel2_clicked  (void);
        void    on_pushButtonSafeReset_channel3_clicked  (void);
        void    on_pushButtonSafeReset_channel4_clicked  (void);
        void    on_pushButtonSafeReset_channel5_clicked  (void);

        //  Initial setup buttons
        void    on_pushButtonInitialSetup_channel1_clicked   (void);
        void    on_pushButtonInitialSetup_channel2_clicked   (void);
        void    on_pushButtonInitialSetup_channel3_clicked   (void);
        void    on_pushButtonInitialSetup_channel4_clicked   (void);
        void    on_pushButtonInitialSetup_channel5_clicked   (void);

        //  Advanced extended buttons
        void  on_pushButtonExtended_channel1_clicked (void);
        void  on_pushButtonExtended_channel2_clicked (void);
        void  on_pushButtonExtended_channel3_clicked (void);
        void  on_pushButtonExtended_channel4_clicked (void);
        void  on_pushButtonExtended_channel5_clicked (void);

        //  Advanced refracted buttons
        void  on_pushButtonRefracted_channel1_clicked    (void);
        void  on_pushButtonRefracted_channel2_clicked    (void);
        void  on_pushButtonRefracted_channel3_clicked    (void);
        void  on_pushButtonRefracted_channel4_clicked    (void);
        void  on_pushButtonRefracted_channel5_clicked    (void);

        //  Check-box for soft start and stop
        void on_checkBoxPwm_channel1_stateChanged    (int arg1);
        void on_checkBoxPwm_channel2_stateChanged    (int arg1);
        void on_checkBoxPwm_channel3_stateChanged    (int arg1);
        void on_checkBoxPwm_channel4_stateChanged    (int arg1);
        void on_checkBoxPwm_channel5_stateChanged    (int arg1);

        //  Check which channel is active
        void on_tab_currentChanged(int index);


        //  Push buttons "Extended" pressed in tab command
        void on_pushButtonExtendedCommand_channel1_pressed   (void);
        void on_pushButtonExtendedCommand_channel2_pressed   (void);
        void on_pushButtonExtendedCommand_channel3_pressed   (void);
        void on_pushButtonExtendedCommand_channel4_pressed   (void);
        void on_pushButtonExtendedCommand_channel5_pressed   (void);
        void on_pushButtonExtendedSynchronous_pressed        (void);

        //  Push buttons "Extended" released in tab command
        void on_pushButtonExtendedCommand_channel1_released  (void);
        void on_pushButtonExtendedCommand_channel2_released  (void);
        void on_pushButtonExtendedCommand_channel3_released  (void);
        void on_pushButtonExtendedCommand_channel4_released  (void);
        void on_pushButtonExtendedCommand_channel5_released  (void);
        void on_pushButtonExtendedSynchronous_released       (void);

        //  Push buttons "Refracted" pressed in tab command
        void on_pushButtonRefractedCommand_channel1_pressed  (void);
        void on_pushButtonRefractedCommand_channel2_pressed  (void);
        void on_pushButtonRefractedCommand_channel3_pressed  (void);
        void on_pushButtonRefractedCommand_channel4_pressed  (void);
        void on_pushButtonRefractedCommand_channel5_pressed  (void);
        void on_pushButtonRefractedSynchronous_pressed       (void);

        void on_pushButtonRefractedCommand_channel1_released (void);
        void on_pushButtonRefractedCommand_channel2_released (void);
        void on_pushButtonRefractedCommand_channel3_released (void);
        void on_pushButtonRefractedCommand_channel4_released (void);
        void on_pushButtonRefractedCommand_channel5_released (void);
        void on_pushButtonRefractedSynchronous_released      (void);



private:
       Ui::MainWindow* ui;
       QList<Channel*> listOfChannels;
       int             lastMoveInSequence;

};

#endif // MAINWINDOW_H
