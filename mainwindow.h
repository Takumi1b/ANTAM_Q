#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMutex>
#include <QDir>
#include "image.h"
#include "serial.h"
#include "dynamixel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Image *img_process;
    Serial *serial_process1;
    Serial *serial_process2;
    Dynamixel *motor_process;


public slots:
    void onValueChangedCam(void);
    void onValueChangedSerial1(void);
    void onValueChangedSerial2(void);
    void onValueChangedDynamixel(void);

private slots:
    void on_h_min_bar_valueChanged(int value);

    void on_h_max_bar_valueChanged(int value);

    void on_s_min_bar_valueChanged(int value);

    void on_s_max_bar_valueChanged(int value);

    void on_v_min_bar_valueChanged(int value);

    void on_v_max_bar_valueChanged(int value);

    void on_threshold_bar_valueChanged(int value);

    void on_pushButton_rawImageShow_released();

    void on_pushButton_backViewShow_released();

    void on_pushButton_imageSetting_released();

    void on_pushButton_LT_released();

    void on_pushButton_RT_released();

    void on_pushButton_LB_released();

    void on_pushButton_RB_released();

    void on_pushButton_setProTransImage_released();

    void on_doubleSpinBox_pgain_valueChanged(double arg1);

    void on_doubleSpinBox_igain_valueChanged(double arg1);

    void on_doubleSpinBox_dgain_valueChanged(double arg1);

    void on_checkBox_mode_released();

    void on_checkBox_motorNum_released();

    void on_horizontalSlider_momOffset_x_valueChanged(int value);

    void on_horizontalSlider_momOffset_y_valueChanged(int value);

    void on_actionload_settings_triggered();

    void on_actionRaw_Image_triggered();

    void on_actionBack_View_triggered();

    void on_actionSet_Image_triggered();



    void on_pushButton_connect_port1_released();

    void on_pushButton_disconnect_port1_released();

    void on_pushButton_send_port1_released();

    void on_pushButton_connect_port2_released();

    void on_pushButton_disconnect_port2_released();

    void on_pushButton_send_port2_released();

    void on_pushButton_save_released();

    void on_toolButton_save_file_path_released();

    void on_lineEdit_save_file_path_editingFinished();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QMutex serial_mutex_;
    QImage qt_img;
    int img_state;
    int parameter_num;
    QString save_file_path;



    QImage MatToQImage(const cv::Mat& mat);
    void loadSettings();
    void saveSettings();


};
#endif // MAINWINDOW_H
