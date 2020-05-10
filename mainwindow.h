#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Channel.h"

#define PIN_CHANNEL1     RPI_BPLUS_GPIO_J8_11
#define PIN_CHANNEL2     RPI_BPLUS_GPIO_J8_12
#define PIN_CHANNEL3     RPI_BPLUS_GPIO_J8_24
#define PIN_CHANNEL4     RPI_BPLUS_GPIO_J8_26
#define PIN_CHANNEL5     RPI_BPLUS_GPIO_J8_36

#define SPI_COMMAND_READ    0x00
#define SPI_COMMAND_WRITE   0x01

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
    void            setListOfChannels                                               (QList<Channel*> _listOfChannels);

    // "Get" methods
    QList<Channel*> getListOfChannels                                               (void                           );

    //  Init methods
    void            initListOfActuators                                             (void                           );

    //  Json File
    void            readJsonFile                                                    (void                           );
    void            updateJsonFile                                                  (void                           );

    //  Setup widgests
    void            setupTab                                                        (void                           );
    void            setupSlider                                                     (void                           );

    // SPI
    void            spiInt                                                          (void                           );
    void            spiEnableSlave                                                  (int _channel                   );
    void            spiDisableSlave                                                 (int _channel                   );
    void            spiSend                                                         (quint8 data                    );
    quint8          spiRead                                                         (void                           );
    void            spiClose                                                        (void                           );

private slots:
    // Update buttons
    void        on_pushButtonUpdateLinearActuatorSpecifications_channel1_clicked    (void                           );
    void        on_pushButtonUpdateStartStopSoft_channel1_clicked                   (void                           );
    void        on_pushButtonUpdateLinearActuatorSpecifications_channel2_clicked    (void                           );
    void        on_pushButtonUpdateStartStopSoft_channel2_clicked                   (void                           );
    void        on_pushButtonUpdateLinearActuatorSpecifications_channel3_clicked    (void                           );
    void        on_pushButtonUpdateStartStopSoft_channel3_clicked                   (void                           );
    void        on_pushButtonUpdateLinearActuatorSpecifications_channel4_clicked    (void                           );
    void        on_pushButtonUpdateStartStopSoft_channel4_clicked                   (void                           );
    void        on_pushButtonUpdateLinearActuatorSpecifications_channel5_clicked    (void                           );
    void        on_pushButtonUpdateStartStopSoft_channel5_clicked                   (void                           );

    //  Sliders
    void        on_horizontalSliderExtended_channel5_valueChanged                   (int value                      );
    void        on_horizontalSliderRefracted_channel5_valueChanged                  (int value                      );
    void        on_horizontalSliderExtended_channel4_valueChanged                   (int value                      );
    void        on_horizontalSliderRefracted_channel4_valueChanged                  (int value                      );
    void        on_horizontalSliderExtended_channel3_valueChanged                   (int value                      );
    void        on_horizontalSliderRefracted_channel3_valueChanged                  (int value                      );
    void        on_horizontalSliderExtended_channel2_valueChanged                   (int value                      );
    void        on_horizontalSliderRefracted_channel2_valueChanged                  (int value                      );
    void        on_horizontalSliderExtended_channel1_valueChanged                   (int value                      );
    void        on_horizontalSliderRefracted_channel1_valueChanged                  (int value                      );


private:
    Ui::MainWindow *ui;
    QList<Channel*>  listOfChannels;

};
#endif // MAINWINDOW_H
