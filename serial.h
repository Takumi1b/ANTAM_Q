#ifndef SERIAL_H
#define SERIAL_H
#include <iostream>

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFile>
#include <QFileInfo>

#include "QtCore/qdebug.h"

#define STACK_BUFFER_SIZE 100

class Serial : public QThread
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = 0, bool b = false);
    ~Serial();

    bool initSerial(void);
    void portSetting(QString,QString,QString,QString,QString,QString);
    void portClose(void);

    void writeData(QString);
    void callfunc(void);

    void createSaveFile(QString);
    bool stop;
    bool receive_start;
    bool message_flag;
    bool save_flag;
    QList<QString> port_list;
    QList<QString> baudrates_list;
    QList<QString> data_bits_list;
    QList<QString> stop_bits_list;
    QList<QString> parities_list;
    QList<QString> flow_control_list;

    QByteArray receive_dataBA;
    QString receive_data_for_ui;

    QString save_file_name;




signals:
    void valueChangedSerial(void);

private slots:
    void receiveData(void);

private:
    void run(void) override;

    QSerialPort *port = nullptr;
    QMutex thread_running_mutex_;
    QMutex receive_mutex_;
    QMutex message_mutex_;

    QByteArray head;
    QByteArray interval;
    QByteArray dx;
    QByteArray dy;

    union TimeData{
        unsigned long interval;
        uint8_t b[4];
    };

    union MouseData{
        int8_t val;
        uint8_t b[1];
    };






    void receiveDataProcessing(void);
    QByteArray readBuffer(int8_t);
    QByteArray cobsDecoder(QByteArray);

};

#endif // SERIAL_H
