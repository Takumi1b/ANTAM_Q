#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <algorithm>
#include <string>

#include <QObject>
#include <QThread>

#include <QMutex>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include "QtCore/qdebug.h"

#define CAP_ID 0
#define CAP_W 640
#define CAP_H 480
#define CAP_FPS 30

class Image : public QThread
{
    Q_OBJECT
public:
    explicit Image(QObject *parent = 0, bool b = false);
    ~Image();

    bool initCam(void);
    void getImage(cv::Mat*);
    void getSettingImage(cv::Mat*);
    void getProTransImage(cv::Mat*);
    void getBinalizeBackImage(cv::Mat*);
    cv::Point calc_moment(cv::Moments);
    bool stop;
    bool calib_flag;
    bool save_flag;

    std::string save_file_path;
    std::string save_file_name;

    int h_min;
    int s_min;
    int v_min;

    int h_max;
    int s_max;
    int v_max;

    int offset_x;
    int offset_y;

    int threshold;
    cv::Moments mom;
    cv::Point2f rect_left_top, rect_right_top, rect_left_bottom, rect_right_bottom;

signals:
    void valueChangedCam(void);

private:
    void run(void) override;
    cv::VideoCapture cap;
    cv::Mat frame, dst;
    cv::Mat process_img;
    cv::Mat binalized_img;
    cv::Mat pro_trans_img;
    cv::Mat save_protrans_img;

    cv::VideoWriter frame_writer;
    cv::VideoWriter protrans_writer;

    int64 time_log;
    int64 t0;
    int64 t1;

    cv::Mat maskImage(cv::Mat);
    cv::Mat noiseReduction(cv::Mat);
    cv::Mat getEdge(cv::Mat);
    cv::Mat getContour(cv::Mat);
    cv::Mat getProTransImage(cv::Mat);
    void processingProTrans(cv::Mat);
    cv::Mat binalizeBackView(cv::Mat);

    void calibModeImageAdd(cv::Mat);

    cv::Point2f pts_src[4];
    cv::Point2f pts_dst[4];

    QMutex thread_running_mutex_;
    QMutex hsv_mutex_;
    QMutex threshold_mutex_;
    QMutex rect_mutex_;
    QMutex mode_mutex_;

};

#endif // IMAGE_H
