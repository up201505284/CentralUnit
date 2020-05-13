#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Channel.h"

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
    void    setListOfChannels   (QList<Channel*> _listOfChannels);

    // "Get" methods
    QList<Channel*> getListOfChannels   (void);

    //  Init methods
    void    initListOfActuators (void);
    void    checkCommunications (void);

    //  Json File
    void    readJsonFile    (void);
    void    updateJsonFile  (void);

    //  Setup widgests
    void    setupTab    (void);
    void    setupSlider (void);


private slots:
    // Update buttons
    void    on_pushButtonUpdateLinearActuatorSpecifications_channel1_clicked    (void);
    void    on_pushButtonUpdateStartStopSoft_channel1_clicked                   (void);
    void    on_pushButtonUpdateLinearActuatorSpecifications_channel2_clicked    (void);
    void    on_pushButtonUpdateStartStopSoft_channel2_clicked                   (void);
    void    on_pushButtonUpdateLinearActuatorSpecifications_channel3_clicked    (void);
    void    on_pushButtonUpdateStartStopSoft_channel3_clicked                   (void);
    void    on_pushButtonUpdateLinearActuatorSpecifications_channel4_clicked    (void);
    void    on_pushButtonUpdateStartStopSoft_channel4_clicked                   (void);
    void    on_pushButtonUpdateLinearActuatorSpecifications_channel5_clicked    (void);
    void    on_pushButtonUpdateStartStopSoft_channel5_clicked                   (void);

    //  Sliders
    void    on_horizontalSliderExtended_channel5_valueChanged   (int value);
    void    on_horizontalSliderRefracted_channel5_valueChanged  (int value);
    void    on_horizontalSliderExtended_channel4_valueChanged   (int value);
    void    on_horizontalSliderRefracted_channel4_valueChanged  (int value);
    void    on_horizontalSliderExtended_channel3_valueChanged   (int value);
    void    on_horizontalSliderRefracted_channel3_valueChanged  (int value);
    void    on_horizontalSliderExtended_channel2_valueChanged   (int value);
    void    on_horizontalSliderRefracted_channel2_valueChanged  (int value);
    void    on_horizontalSliderExtended_channel1_valueChanged   (int value);
    void    on_horizontalSliderRefracted_channel1_valueChanged  (int value);

    //  Basic extended buttons
    void    on_pushButtonBasicExtended_channel1_clicked (void);
    void    on_pushButtonBasicExtended_channel2_clicked (void);
    void    on_pushButtonBasicExtended_channel3_clicked (void);
    void    on_pushButtonBasicExtended_channel4_clicked (void);
    void    on_pushButtonBasicExtended_channel5_clicked (void);

    //  Basic refracted buttons
    void    on_pushButtonBasicRefracted_channel5_clicked(void);
    void    on_pushButtonBasicRefracted_channel4_clicked(void);
    void    on_pushButtonBasicRefracted_channel3_clicked(void);
    void    on_pushButtonBasicRefracted_channel1_clicked(void);
    void    on_pushButtonBasicRefracted_channel2_clicked(void);

    //  Stop buttons
    void    on_pushButtonStop_channel1_clicked          (void);
    void    on_pushButtonStop_channel2_clicked          (void);
    void    on_pushButtonStop_channel3_clicked          (void);
    void    on_pushButtonStop_channel4_clicked          (void);
    void    on_pushButtonStop_channel5_clicked          (void);

    //  Safe reset buttons
    void    on_pushButtonSafeReset_channel1_clicked     (void);
    void    on_pushButtonSafeReset_channel2_clicked     (void);
    void    on_pushButtonSafeReset_channel3_clicked     (void);
    void    on_pushButtonSafeReset_channel4_clicked     (void);
    void    on_pushButtonSafeReset_channel5_clicked     (void);

    //  Initial setup buttons
    void    on_pushButtonInitialSetup_channel1_clicked  (void);
    void    on_pushButtonInitialSetup_channel2_clicked  (void);
    void    on_pushButtonInitialSetup_channel3_clicked  (void);
    void    on_pushButtonInitialSetup_channel4_clicked  (void);
    void    on_pushButtonInitialSetup_channel5_clicked  (void);

    //  Advanced extended buttons
    void  on_pushButtonExtended_channel1_clicked        (void);
    void  on_pushButtonExtended_channel2_clicked        (void);
    void  on_pushButtonExtended_channel3_clicked        (void);
    void  on_pushButtonExtended_channel4_clicked        (void);
    void  on_pushButtonExtended_channel5_clicked        (void);
    void  on_pushButtonRefracted_channel1_clicked       (void);
    void  on_pushButtonRefracted_channel2_clicked       (void);
    void  on_pushButtonRefracted_channel3_clicked       (void);
    void  on_pushButtonRefracted_channel4_clicked       (void);
    void  on_pushButtonRefracted_channel5_clicked       (void);

private:
    Ui::MainWindow* ui;
    QList<Channel*> listOfChannels;

};
#endif // MAINWINDOW_H
