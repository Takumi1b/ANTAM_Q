#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <QThread>
#include <QObject>
#include "QtCore/qdebug.h"
#include <QMutex>

#include "image.h"

#include <./DXLIB_v4.6/DXLIB/dxlib.h>

#define _COMPORT "/dev/cu.usbserial-FT3FSMJZ" //"/dev/cu.usbserial-FT3FQ2CS"
#define _BAUDRATE 57600

class Dynamixel : public QThread
{
    Q_OBJECT
public:
    explicit Dynamixel(QObject *parent = 0, bool b = false);
    ~Dynamixel();
    bool initDynamixel(void);


    bool stop;
    bool calib_flag;
    bool motor_num;
    int pos_x;
    int pos_y;
    int offset_x;
    int offset_y;

    float kp_gain;
    float ki_gain;
    float kd_gain;


signals:
    void valueChangedDynamixel(void);

private:
    void run(void) override;

    TDeviceID dev;
    TErrorCode err;
    QMutex thread_running_mutex_;
    QMutex moment_mutex_;
    QMutex motor_num_mutex_;
    QMutex mode_mutex_;

    int state;
    int speed_x;
    int speed_y;

    double integ_error_x;
    double integ_error_y;
    double diff_error_x;
    double diff_error_y;
    double pre_error_x;
    double pre_error_y;

    void calcSpeed();
    void moveMotor();

    void endDynamixel();

};

#endif // DYNAMIXEL_H
