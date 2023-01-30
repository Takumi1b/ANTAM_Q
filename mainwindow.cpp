#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    img_state = 0;
    parameter_num = 22;
    save_file_path = QDir::currentPath() + "/";

    img_process = new Image;
    motor_process = new Dynamixel;
    serial_process1 = new Serial;
    serial_process2 = new Serial;

    if(img_process->initCam()){
        connect(img_process, SIGNAL(valueChangedCam()), this, SLOT(onValueChangedCam()));
        img_process->start();
    }else{
        qDebug() << "cannot open cam";
    }

    if(motor_process->initDynamixel()){
        connect(motor_process, SIGNAL(valueChangedDynamixel()), this, SLOT(onValueChangedDynamixel()));
        motor_process->start();
    }else{
        qDebug() << "cannot open dynamixel port";
    }

    connect(serial_process1, SIGNAL(valueChangedSerial()), this, SLOT(onValueChangedSerial1()));
    serial_process1->start();
    connect(serial_process2, SIGNAL(valueChangedSerial()), this, SLOT(onValueChangedSerial2()));
    serial_process2->start();





    ui->setupUi(this);
    ui->comboBox_port1->addItems(serial_process1->port_list);
    ui->comboBox_baudrate1->addItems(serial_process1->baudrates_list);
    ui->comboBox_databits1->addItems(serial_process1->data_bits_list);
    ui->comboBox_stopbits1->addItems(serial_process1->stop_bits_list);
    ui->comboBox_parity1->addItems(serial_process1->parities_list);
    ui->comboBox_flowControl1->addItems(serial_process1->flow_control_list);

    ui->comboBox_port2->addItems(serial_process2->port_list);
    ui->comboBox_baudrate2->addItems(serial_process2->baudrates_list);
    ui->comboBox_databits2->addItems(serial_process2->data_bits_list);
    ui->comboBox_stopbits2->addItems(serial_process2->stop_bits_list);
    ui->comboBox_parity2->addItems(serial_process2->parities_list);
    ui->comboBox_flowControl2->addItems(serial_process2->flow_control_list);

    ui->lineEdit_save_file_path->setText(save_file_path);
}

MainWindow::~MainWindow()
{
    img_process->stop = true;
    img_process->exit();
    img_process->wait();

    motor_process->stop = true;
    motor_process->exit();
    motor_process->wait();

    serial_process1->stop = true;
    serial_process1->exit();
    serial_process1->wait();

    serial_process2->stop = true;
    serial_process2->exit();
    serial_process2->wait();

    delete img_process;
    delete motor_process;
    delete serial_process1;
    delete serial_process2;
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint p = ui->back_view->mapFromGlobal(QCursor::pos());
    event->accept();
    if(p.x() >= 0 && p.x() <= CAP_W && p.y() >= 0 && p.y() <= CAP_H){
        ui->label_ImageX->setText(QString::number(p.x()));
        ui->label_ImageY->setText(QString::number(p.y()));
    }

}



QImage MainWindow::MatToQImage(const cv::Mat& mat){
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img;
    }
    return QImage();
}

void MainWindow::loadSettings(){
    QFile settings_file(QFileDialog::getOpenFileName());
    settings_file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&settings_file);
    QString line;
    std::vector<QString> items(parameter_num);
    uint8_t line_num = 0;
    uint8_t item_num = 0;
    while(!in.atEnd()){
        if(line_num == 0){
            line = in.readLine();
            ++line_num;
            continue;
        }
        line = in.readLine();
        item_num = 0;
        for(const QString &item: line.split(",")){
            items.at(item_num) = item;
            ++item_num;
        }
        ++line_num;
    }
    settings_file.close();

    //mode
    motor_process->calib_flag = items.at(0).toInt();
    img_process->calib_flag = items.at(0).toInt();
    if(items.at(0).toInt() == 0) ui->checkBox_mode->setCheckState(Qt::Unchecked);
    else ui->checkBox_mode->setCheckState(Qt::Checked);

    //hsv
    img_process->h_min = items.at(1).toInt();
    img_process->h_max = items.at(2).toInt();
    img_process->s_min = items.at(3).toInt();
    img_process->s_max = items.at(4).toInt();
    img_process->v_min = items.at(5).toInt();
    img_process->v_max = items.at(6).toInt();
    ui->h_min_bar->setValue(items.at(1).toInt());
    ui->h_max_bar->setValue(items.at(2).toInt());
    ui->s_min_bar->setValue(items.at(3).toInt());
    ui->s_max_bar->setValue(items.at(4).toInt());
    ui->v_min_bar->setValue(items.at(5).toInt());
    ui->v_max_bar->setValue(items.at(6).toInt());

    //rect
    img_process->rect_left_top.x = items.at(7).toInt();
    img_process->rect_left_top.y = items.at(8).toInt();
    img_process->rect_left_bottom.x = items.at(9).toInt();
    img_process->rect_left_bottom.y = items.at(10).toInt();
    img_process->rect_right_top.x = items.at(11).toInt();
    img_process->rect_right_top.y = items.at(12).toInt();
    img_process->rect_right_bottom.x = items.at(13).toInt();
    img_process->rect_right_bottom.y = items.at(14).toInt();
    ui->spinBox_LT_x->setValue(items.at(7).toInt());
    ui->spinBox_LT_y->setValue(items.at(8).toInt());
    ui->spinBox_LB_x->setValue(items.at(9).toInt());
    ui->spinBox_LB_y->setValue(items.at(10).toInt());
    ui->spinBox_RT_x->setValue(items.at(11).toInt());
    ui->spinBox_RT_y->setValue(items.at(12).toInt());
    ui->spinBox_RB_x->setValue(items.at(13).toInt());
    ui->spinBox_RB_y->setValue(items.at(14).toInt());

    //threshold
    img_process->threshold = items.at(15).toInt();
    ui->threshold_bar->setValue(items.at(15).toInt());

    //motor num
    motor_process->motor_num = items.at(16).toInt();
    if(items.at(16).toInt() == 0) ui->checkBox_motorNum->setCheckState(Qt::Unchecked);
    else ui->checkBox_motorNum->setCheckState(Qt::Checked);

    //gain
    motor_process->kp_gain = items.at(17).toDouble();
    motor_process->ki_gain = items.at(18).toDouble();
    motor_process->kd_gain = items.at(19).toDouble();
    ui->doubleSpinBox_pgain->setValue(items.at(17).toDouble());
    ui->doubleSpinBox_igain->setValue(items.at(18).toDouble());
    ui->doubleSpinBox_dgain->setValue(items.at(19).toDouble());

    //offset
    img_process->offset_x = items.at(20).toInt();
    img_process->offset_y = items.at(21).toInt();
    motor_process->offset_x = items.at(20).toInt();
    motor_process->offset_y = items.at(21).toInt();
    ui->horizontalSlider_momOffset_x->setValue(items.at(20).toInt());
    ui->horizontalSlider_momOffset_y->setValue(items.at(21).toInt());


}

void MainWindow::saveSettings(){
    QString setting_path = save_file_path + "Settings/";
    QDir dir;
    if (!dir.exists(setting_path)) dir.mkdir(setting_path);

    QString file_name = setting_path + "settings.csv";
    QString top = "mode,hmin,hmax,smin,smax,vmin,vmax,lefttopx,lefttopy,leftbottomx,leftbottomy,righttopx,righttopy,rightbottomx,rightbottomy,threshold,motornum,pgain,igain,dgain,offsetx,offsety\n";
    std::vector<QString> items(parameter_num);
    QFile file(file_name);

    items.at(0) = QString::number(motor_process->calib_flag) + ",";
    items.at(1) = QString::number(img_process->h_min) + ",";
    items.at(2) = QString::number(img_process->h_max) + ",";
    items.at(3) = QString::number(img_process->s_min) + ",";
    items.at(4) = QString::number(img_process->s_max) + ",";
    items.at(5) = QString::number(img_process->v_min) + ",";
    items.at(6) = QString::number(img_process->v_max) + ",";
    items.at(7) = QString::number(img_process->rect_left_top.x) + ",";
    items.at(8) = QString::number(img_process->rect_left_top.y) + ",";
    items.at(9) = QString::number(img_process->rect_left_bottom.x) + ",";
    items.at(10) = QString::number(img_process->rect_left_bottom.y) + ",";
    items.at(11) = QString::number(img_process->rect_right_top.x) + ",";
    items.at(12) = QString::number(img_process->rect_right_top.y) + ",";
    items.at(13) = QString::number(img_process->rect_right_bottom.x) + ",";
    items.at(14) = QString::number(img_process->rect_right_bottom.y) + ",";
    items.at(15) = QString::number(img_process->threshold) + ",";
    items.at(16) = QString::number(motor_process->motor_num) + ",";
    items.at(17) = QString::number(motor_process->kp_gain) + ",";
    items.at(18) = QString::number(motor_process->ki_gain) + ",";
    items.at(19) = QString::number(motor_process->kd_gain) + ",";
    items.at(20) = QString::number(img_process->offset_x) + ",";
    items.at(21) = QString::number(img_process->offset_y);

    file.open(QFile::WriteOnly);
    QTextStream out(&file);
    out << top;
    for(int i=0; i<(int)items.size(); ++i){
        out << items.at(i);
    }
}





void MainWindow::onValueChangedCam(void){
    cv::Mat copy_frame;
    switch(img_state){
        case 0:
            img_process->getImage(&copy_frame);
            break;
        case 1:
            img_process->getProTransImage(&copy_frame);
            break;
        case 2:
            img_process->getSettingImage(&copy_frame);
            break;
        default:
            break;
    }


    qt_img = MatToQImage(copy_frame);
    ui->back_view->setPixmap(QPixmap::fromImage(qt_img));

    img_process->getBinalizeBackImage(&copy_frame);
    qt_img = MatToQImage(copy_frame);
    ui->binary_view->setPixmap(QPixmap::fromImage(qt_img));

    copy_frame.release();
}

void MainWindow::onValueChangedSerial1(void){
    if(serial_process1->receive_start){
        ui->textBrowser_readSerial1->setTextColor(Qt::blue);
        ui->textBrowser_readSerial1->append(serial_process1->receive_data_for_ui);
    }
}

void MainWindow::onValueChangedSerial2(void){
    if(serial_process2->receive_start){
        ui->textBrowser_readSerial2->setTextColor(Qt::blue);
        ui->textBrowser_readSerial2->append(serial_process2->receive_data_for_ui);
    }
}

void MainWindow::onValueChangedDynamixel(void){
    cv::Point pos;
    pos = img_process->calc_moment(img_process->mom);
    motor_process->pos_x = pos.x;
    motor_process->pos_y = pos.y;
}

void MainWindow::on_h_min_bar_valueChanged(int value)
{
    img_process->h_min = value;
}


void MainWindow::on_h_max_bar_valueChanged(int value)
{
    img_process->h_max = value;
}


void MainWindow::on_s_min_bar_valueChanged(int value)
{
    img_process->s_min = value;
}


void MainWindow::on_s_max_bar_valueChanged(int value)
{
    img_process->s_max = value;
}


void MainWindow::on_v_min_bar_valueChanged(int value)
{
    img_process->v_min = value;
}


void MainWindow::on_v_max_bar_valueChanged(int value)
{
    img_process->v_max = value;
}

void MainWindow::on_threshold_bar_valueChanged(int value)
{
    img_process->threshold = value;
}


void MainWindow::on_pushButton_rawImageShow_released()
{
    img_state = 0;
}



void MainWindow::on_pushButton_backViewShow_released()
{
    img_state = 1;
}


void MainWindow::on_pushButton_imageSetting_released()
{
    img_state = 2;
}


void MainWindow::on_pushButton_LT_released()
{
    ui->spinBox_LT_x->setValue(ui->label_ImageX->text().toInt());
    ui->spinBox_LT_y->setValue(ui->label_ImageY->text().toInt());

}


void MainWindow::on_pushButton_RT_released()
{
    ui->spinBox_RT_x->setValue(ui->label_ImageX->text().toInt());
    ui->spinBox_RT_y->setValue(ui->label_ImageY->text().toInt());

}


void MainWindow::on_pushButton_LB_released()
{
    ui->spinBox_LB_x->setValue(ui->label_ImageX->text().toInt());
    ui->spinBox_LB_y->setValue(ui->label_ImageY->text().toInt());

}


void MainWindow::on_pushButton_RB_released()
{
    ui->spinBox_RB_x->setValue(ui->label_ImageX->text().toInt());
    ui->spinBox_RB_y->setValue(ui->label_ImageY->text().toInt());

}

void MainWindow::on_pushButton_setProTransImage_released()
{
    img_process->rect_left_top.x = ui->spinBox_LT_x->value();
    img_process->rect_left_top.y = ui->spinBox_LT_y->value();
    img_process->rect_right_top.x = ui->spinBox_RT_x->value();
    img_process->rect_right_top.y = ui->spinBox_RT_y->value();
    img_process->rect_left_bottom.x = ui->spinBox_LB_x->value();
    img_process->rect_left_bottom.y = ui->spinBox_LB_y->value();
    img_process->rect_right_bottom.x = ui->spinBox_RB_x->value();
    img_process->rect_right_bottom.y = ui->spinBox_RB_y->value();
}


void MainWindow::on_doubleSpinBox_pgain_valueChanged(double arg1)
{
    motor_process->kp_gain = arg1;
}


void MainWindow::on_doubleSpinBox_igain_valueChanged(double arg1)
{
    motor_process->ki_gain = arg1;
}


void MainWindow::on_doubleSpinBox_dgain_valueChanged(double arg1)
{
    motor_process->kd_gain = arg1;
}


void MainWindow::on_checkBox_mode_released()
{
    motor_process->calib_flag = !motor_process->calib_flag;
    img_process->calib_flag = !img_process->calib_flag;
}


void MainWindow::on_checkBox_motorNum_released()
{
    motor_process->motor_num = !motor_process->motor_num;

}


void MainWindow::on_horizontalSlider_momOffset_x_valueChanged(int value)
{
    img_process->offset_x = value;
    motor_process->offset_x = value;
}


void MainWindow::on_horizontalSlider_momOffset_y_valueChanged(int value)
{
    img_process->offset_y = value;
    motor_process->offset_y = value;
}


void MainWindow::on_actionload_settings_triggered()
{
    loadSettings();
}


void MainWindow::on_actionRaw_Image_triggered()
{
    img_state = 0;
}


void MainWindow::on_actionBack_View_triggered()
{
    img_state = 1;
}


void MainWindow::on_actionSet_Image_triggered()
{
    img_state = 2;
}





void MainWindow::on_pushButton_connect_port1_released()
{
    serial_process1->portSetting(ui->comboBox_port1->currentText(),
                                 ui->comboBox_baudrate1->currentText(),
                                 ui->comboBox_databits1->currentText(),
                                 ui->comboBox_stopbits1->currentText(),
                                 ui->comboBox_parity1->currentText(),
                                 ui->comboBox_flowControl1->currentText());

    if(serial_process1->message_flag){
        ui->textBrowser_readSerial1->setTextColor(Qt::red);
        ui->textBrowser_readSerial1->append("[ERROR:] cannot open serial port");
        serial_process1->message_flag = false;
    }else{
        ui->textBrowser_readSerial1->setTextColor(Qt::green);
        ui->textBrowser_readSerial1->append("Open serial port: " + ui->comboBox_port1->currentText());
        serial_process1->callfunc();
    }

}


void MainWindow::on_pushButton_disconnect_port1_released()
{
    serial_process1->portClose();
    if(serial_process1->message_flag){
        ui->textBrowser_readSerial1->setTextColor(Qt::green);
        ui->textBrowser_readSerial1->append("Close Port: " + ui->comboBox_port1->currentText());
        serial_process1->message_flag = false;
    }else{
        ui->textBrowser_readSerial1->setTextColor(Qt::red);
        ui->textBrowser_readSerial1->append("Not opne serial port: " + ui->comboBox_port1->currentText());
    }

}


void MainWindow::on_pushButton_send_port1_released()
{
    QString message = ui->textEdit_writeSerial1->toPlainText();
    ui->textBrowser_readSerial1->setTextColor(Qt::green);
    ui->textBrowser_readSerial1->append(message);
    serial_process1->writeData(message);
}

void MainWindow::on_pushButton_connect_port2_released(){
    serial_process2->portSetting(ui->comboBox_port2->currentText(),
                                 ui->comboBox_baudrate2->currentText(),
                                 ui->comboBox_databits2->currentText(),
                                 ui->comboBox_stopbits2->currentText(),
                                 ui->comboBox_parity2->currentText(),
                                 ui->comboBox_flowControl2->currentText());

    if(serial_process2->message_flag){
        ui->textBrowser_readSerial2->setTextColor(Qt::red);
        ui->textBrowser_readSerial2->append("[ERROR:] cannot open serial port");
        serial_process2->message_flag = false;
    }else{
        ui->textBrowser_readSerial2->setTextColor(Qt::green);
        ui->textBrowser_readSerial2->append("Open serial port: " + ui->comboBox_port2->currentText());
        serial_process2->callfunc();
    }
}

void MainWindow::on_pushButton_disconnect_port2_released(){
    serial_process2->portClose();
    if(serial_process2->message_flag){
        ui->textBrowser_readSerial2->setTextColor(Qt::green);
        ui->textBrowser_readSerial2->append("Close Port: " + ui->comboBox_port2->currentText());
        serial_process2->message_flag = false;
    }else{
        ui->textBrowser_readSerial2->setTextColor(Qt::red);
        ui->textBrowser_readSerial2->append("Not opne serial port: " + ui->comboBox_port2->currentText());
    }
}

void MainWindow::on_pushButton_send_port2_released(){
    QString message = ui->textEdit_writeSerial2->toPlainText();
    ui->textBrowser_readSerial2->setTextColor(Qt::green);
    ui->textBrowser_readSerial2->append(message);
    serial_process2->writeData(message);
}



void MainWindow::on_pushButton_save_released()
{
    QDir dir;
    QString sensor_path = save_file_path + "MouseSensorData/";
    QString video_path = save_file_path + "VideoData/";
    if (!dir.exists(sensor_path)) dir.mkdir(sensor_path);
    if (!dir.exists(video_path)) dir.mkdir(video_path);
    saveSettings();
    serial_process1->createSaveFile(sensor_path + ui->comboBox_port1->currentText() + "_");
    serial_process1->save_flag = true;
    serial_process2->createSaveFile(sensor_path + ui->comboBox_port2->currentText() + "_");
    serial_process2->save_flag = true;
    img_process->save_file_path = video_path.toStdString();
    img_process->save_flag = true;


}


void MainWindow::on_toolButton_save_file_path_released()
{
    save_file_path = QFileDialog::getExistingDirectory() + "/";
    ui->lineEdit_save_file_path->setText(save_file_path);
}


void MainWindow::on_lineEdit_save_file_path_editingFinished()
{
    save_file_path = ui->lineEdit_save_file_path->text();
}

