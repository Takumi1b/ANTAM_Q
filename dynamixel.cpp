#include "dynamixel.h"

Dynamixel::Dynamixel(QObject *parent, bool b)
    : QThread(parent), stop(b)
{
    pos_x = CAP_W / 2;
    pos_y = CAP_H / 2;
    offset_x = CAP_W / 2;
    offset_y = CAP_H / 2;
    calib_flag = false;
    motor_num = false;

    kp_gain = 1;
    ki_gain = 0*CAP_FPS;
    kd_gain = 0/CAP_FPS;

    state = 0;
    speed_x = 0;
    speed_y = 0;

    integ_error_x = 0;
    integ_error_y = 0;
    diff_error_x = 0;
    diff_error_y = 0;
    pre_error_x = CAP_W / 2;
    pre_error_y = CAP_H / 2;


}

Dynamixel::~Dynamixel(){
    stop = true;
}

bool Dynamixel::initDynamixel(void){
    bool ret = true;
    dev = DX_OpenPort(_COMPORT, _BAUDRATE);
    if(dev){
        qDebug() << "Successful opening of " << _COMPORT;
    }else{
        ret = false;
    }

    return ret;
}

void Dynamixel::run(void){
    while(1){
        thread_running_mutex_.lock();
        if(this->stop){
            endDynamixel();
            thread_running_mutex_.unlock();
            break;
        }
        thread_running_mutex_.unlock();

        if(dev){
            mode_mutex_.lock();
            if(!calib_flag){
                calcSpeed();
                moveMotor();
            }else{
                state = 0;
                moveMotor();
            }
            mode_mutex_.unlock();
        }

        emit valueChangedDynamixel();
    }
}

void Dynamixel::calcSpeed(){
    int abs_x, abs_y;
    int error_x, error_y;

    //画面中心からのずれを求める
    moment_mutex_.lock();

    error_x = pos_x - offset_x;
    error_y = pos_y - offset_y;

    //PID制御の比例項、積分項、微分項を決定
    abs_x = abs(error_x);
    abs_y = abs(error_y);
    integ_error_x = integ_error_x + error_x;
    integ_error_y = integ_error_y + error_y;
    diff_error_x = pre_error_x - error_x;
    diff_error_y = pre_error_y - error_y;

    //PID制御によりモータ速度の決定
    speed_x = kp_gain * abs_x + ki_gain * integ_error_x + kd_gain*diff_error_x;
    speed_y = kp_gain * abs_y + ki_gain * integ_error_y + kd_gain*diff_error_y;

    pre_error_x = error_x;
    pre_error_y = error_y;

    if (speed_x > 1023) speed_x = 1023;
    if (speed_y > 1023) speed_y = 1023;

    if (pos_x >= CAP_W / 2 && pos_y >= CAP_H / 2) { //右下
        state = 1;
    }
    else if (pos_x < CAP_W / 2 && pos_y > CAP_H / 2) { //左下
        state = 2;
    }
    else if (pos_x <= CAP_W / 2 && pos_y <= CAP_H / 2) { //左上
        state = 3;
    }
    else if (pos_x > CAP_W / 2 && pos_y < CAP_H / 2) { //右上
        state = 4;
    }
    else {
        state = 0;
    }

    moment_mutex_.unlock();

}

void Dynamixel::moveMotor(){
    motor_num_mutex_.lock();
    if(!motor_num){ //3
        switch (state)
        {
        case 1: //左上方向
            DX_WriteWordData(dev, 0, 32, speed_x + 1024, &err);
            DX_WriteWordData(dev, 1, 32, (speed_x/2 + sqrt(3)*speed_y/2), &err);
            DX_WriteWordData(dev, 2, 32, (speed_x/2 + sqrt(3)*speed_y/2) + 1024, &err);
            break;
        case 2: //右上方向
            DX_WriteWordData(dev, 0, 32, speed_x, &err);
            DX_WriteWordData(dev, 1, 32, (speed_x/2 + sqrt(3)*speed_y/2) + 1024, &err);
            DX_WriteWordData(dev, 2, 32, (speed_x/2 + sqrt(3)*speed_y/2) + 1024, &err);
            break;
        case 3: //右下方向
            DX_WriteWordData(dev, 0, 32, speed_x, &err);
            DX_WriteWordData(dev, 1, 32, (speed_x/2 + sqrt(3)*speed_y/2) + 1024, &err);
            DX_WriteWordData(dev, 2, 32, (speed_x/2 + sqrt(3)*speed_y/2), &err);
            break;
        case 4: //左下方向
            DX_WriteWordData(dev, 0, 32, speed_x + 1024, &err);
            DX_WriteWordData(dev, 1, 32, (speed_x/2 + sqrt(3)*speed_y/2) + 1024, &err);
            DX_WriteWordData(dev, 2, 32, (speed_x/2 + sqrt(3)*speed_y/2), &err);
            break;
        default:
            for (int i = 0; i<4; i++)
                DX_WriteWordData(dev, i, 32, 0, &err);
            break;
        }
    }else{ //4
        switch (state)
        {
        case 1:
            DX_WriteWordData(dev, 0, 32, speed_y + 1024, &err);
            DX_WriteWordData(dev, 1, 32, speed_y, &err);
            DX_WriteWordData(dev, 2, 32, speed_x + 1024, &err);
            DX_WriteWordData(dev, 3, 32, speed_x, &err);

            break;
        case 2:
            DX_WriteWordData(dev, 0, 32, speed_y + 1024, &err);
            DX_WriteWordData(dev, 1, 32, speed_y, &err);
            DX_WriteWordData(dev, 2, 32, speed_x, &err);
            DX_WriteWordData(dev, 3, 32, speed_x + 1024, &err);
            break;
        case 3:
            DX_WriteWordData(dev, 0, 32, speed_y, &err);
            DX_WriteWordData(dev, 1, 32, speed_y + 1024, &err);
            DX_WriteWordData(dev, 2, 32, speed_x, &err);
            DX_WriteWordData(dev, 3, 32, speed_x + 1024, &err);
            break;
        case 4:
            DX_WriteWordData(dev, 0, 32, speed_y, &err);
            DX_WriteWordData(dev, 1, 32, speed_y + 1024, &err);
            DX_WriteWordData(dev, 2, 32, speed_x + 1024, &err);
            DX_WriteWordData(dev, 3, 32, speed_x, &err);

            break;
        default:
            for (int i = 0; i<4; i++)
                DX_WriteWordData(dev, i, 32, 0, &err);
            break;
        }
    }
    motor_num_mutex_.unlock();
}


void Dynamixel::endDynamixel(){
    motor_num_mutex_.lock();
    for (int i = 0; i < motor_num; i++) {
        DX_WriteWordData(dev, i, 32, 0, &err);
    }
    motor_num_mutex_.unlock();
    DX_ClosePort (dev);
}
