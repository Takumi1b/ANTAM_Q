#include "serial.h"

Serial::Serial(QObject *parent, bool b)
    : QThread{parent}, stop(b)
{
    port = new QSerialPort(this);
    message_flag = false;
    receive_start = false;
    save_flag = false;
    foreach( const QSerialPortInfo info, QSerialPortInfo::availablePorts() ){
        port_list.append(info.portName());
    }
    foreach( const qint32 info, QSerialPortInfo::standardBaudRates()){
        baudrates_list.append(QString::number(info));
    }
    for(int i=5; i<9; ++i){
        data_bits_list.append(QString::number(i) + " Bits");
    }

    stop_bits_list.append("1 Bit");
    stop_bits_list.append("1.5 Bits");
    stop_bits_list.append("2 Bits");

    parities_list.append("No Parity");
    parities_list.append("Even Parity");
    parities_list.append("Odd Parity");
    parities_list.append("Mark Parity");
    parities_list.append("Space Parity");

    flow_control_list.append("No Flow Control");
    flow_control_list.append("Hardware Flow Control");
    flow_control_list.append("Software Flow Control");
}

Serial::~Serial(){
    portClose();
    stop = true;
}

bool Serial::initSerial(){
    bool ret = true;
    return ret;
}

void Serial::run(void){
    while(1){
        thread_running_mutex_.lock();
        if(this->stop){
            thread_running_mutex_.unlock();
            break;
        }
        thread_running_mutex_.unlock();
        emit valueChangedSerial();
        this->msleep(2); //500Hz 2ms sleep
    }
}

void Serial::portSetting(QString port_name,QString baudrate,QString databits,
                 QString stopbits,QString parity,QString flow_control){
    port->setPortName(port_name);
    port->open(QIODevice::ReadWrite);
    if(port->isOpen()){
        port->setBaudRate(baudrate.toInt());
        if(databits == "5 Bits") port->setDataBits(QSerialPort::Data5);
        else if(databits == "6 Bits") port->setDataBits(QSerialPort::Data6);
        else if(databits == "7 Bits") port->setDataBits(QSerialPort::Data7);
        else if(databits == "8 Bits") port->setDataBits(QSerialPort::Data8);

        if(stopbits == "1 Bit") port->setStopBits(QSerialPort::OneStop);
        else if(stopbits == "1.5 Bits") port->setStopBits(QSerialPort::OneAndHalfStop);
        else if(stopbits == "2 Bits") port->setStopBits(QSerialPort::TwoStop);

        if(parity == "No Parity") port->setParity(QSerialPort::NoParity);
        else if(parity == "Even Parity") port->setParity(QSerialPort::EvenParity);
        else if(parity == "Odd Parity") port->setParity(QSerialPort::OddParity);
        else if(parity == "Mark Parity") port->setParity(QSerialPort::MarkParity);
        else if(parity == "Space Parity") port->setParity(QSerialPort::SpaceParity);

        if(flow_control == "No Flow Control") port->setFlowControl(QSerialPort::NoFlowControl);
        else if(flow_control == "Hardware Flow Control") port->setFlowControl(QSerialPort::HardwareControl);
        else if(flow_control == "Software Flow Control") port->setFlowControl(QSerialPort::SoftwareControl);

        receive_mutex_.lock();
        receive_start = true;
        receive_mutex_.unlock();

    }else{
        message_mutex_.lock();
        message_flag = true;
        receive_start = false;
        message_mutex_.unlock();

    }
}

void Serial::portClose(void){
    if(port->isOpen()){
        message_mutex_.lock();
        message_flag = true;
        receive_start = false;
        message_mutex_.unlock();

        port->close();
    }
}

void Serial::callfunc(){
    receive_dataBA.remove(0, receive_dataBA.size());
    receive_dataBA.squeeze();
    receive_data_for_ui = "";
    connect(port,SIGNAL(readyRead()),this,SLOT(receiveData()));
}

void Serial::receiveData(void){
    receive_dataBA.append(port->readAll());
    receiveDataProcessing();
}

QByteArray Serial::readBuffer(int8_t buff_size){
    QByteArray read_BA;
    for(int i=0; i<buff_size; ++i){
        read_BA.append(receive_dataBA.at(i));
    }
    receive_dataBA.remove(0,buff_size);
    return read_BA;
}

void Serial::receiveDataProcessing(void){
    TimeData time_data = {0};
    MouseData dx_data = {0};
    MouseData dy_data = {0};

    QFileInfo fileInfo(save_file_name);
    QFile savefile(save_file_name);
    if (fileInfo.exists()){
        savefile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    while(receive_dataBA.size() > 15){
        head = cobsDecoder(readBuffer(3));
        if(head == ">"){
            interval = cobsDecoder(readBuffer(6));
            dx = cobsDecoder(readBuffer(3));
            dy = cobsDecoder(readBuffer(3));
            for(int i=0; i<(int)interval.size(); ++i){
                time_data.b[i] = interval.at(i);
                if(i == 0){
                    dx_data.b[i] = dx.at(i);
                    dy_data.b[i] = dy.at(i);
                }
            }
//            qDebug() << "time:" << time_data.interval << " dx:" << dx_data.val << " dy:" << dy_data.val;
            receive_mutex_.lock();
            receive_data_for_ui = "time:"+QString::number(time_data.interval) + " dx:" + QString::number(dx_data.val) + " dy:" + QString::number(dy_data.val);
            if(save_flag){
                QTextStream out(&savefile);
                out << QString::number(time_data.interval) + ","
                    + QString::number(dx_data.val) + ","
                    + QString::number(dy_data.val) + "\n";
            }
            receive_mutex_.unlock();
        }
        receive_dataBA.squeeze();
    }
    savefile.close();
}

QByteArray Serial::cobsDecoder(QByteArray encoding_data){
    QByteArray decode_data;
    decode_data.resize(encoding_data.size());
    int encode_idx = 0;
    int decode_idx = 0;
    unsigned char next_0x00 = 0x00;
    bool next_is_overhead = true;

    while(encode_idx < encoding_data.size()){
        if(next_0x00 != 0x00){
            decode_data[decode_idx] = encoding_data[encode_idx];
            ++encode_idx;
            ++decode_idx;
        }else{
            if(encoding_data.at(encode_idx) == 0x00){
                break;
            }
            if(next_is_overhead){
            }else{
                decode_data[decode_idx] = 0x00;
                ++decode_idx;
            }
            next_0x00 = encoding_data[encode_idx];
            ++encode_idx;

            if(next_0x00 == 0xff){
                next_is_overhead = true;
            }else{
                next_is_overhead = false;
            }
        }
        next_0x00 -= 0x01;
    }
    return decode_data.sliced(0,decode_idx);
}

void Serial::writeData(QString message){
    if(port->isOpen()){
        port->write(message.toUtf8().data());
    }
}

void Serial::createSaveFile(QString file_path){
    time_t now = time(NULL);
    struct tm *pnow = localtime(&now);
    char now_time[128]="";
    sprintf(now_time,"%04d_%02d%02d_%02d_%02d_%02d_",pnow->tm_year+1900,pnow->tm_mon+1,pnow->tm_mday,pnow->tm_hour,pnow->tm_min,pnow->tm_sec);

    save_file_name = file_path + now_time +"mouse_sensor_data.csv";
    QFile create_file(save_file_name);
    if (create_file.open(QIODevice::WriteOnly)) {
        QTextStream out(&create_file);
        out << "time,dx,dy\n";
        create_file.close();
    }else{
        qDebug() << "cannot open";
    }
}


