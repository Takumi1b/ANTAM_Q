#include "image.h"

Image::Image(QObject *parent, bool b)
    : QThread(parent), stop(b)
{
    calib_flag = false;
    save_flag = false;

    h_min = 20;
    s_min = 58;
    v_min = 96;

    h_max = 30;
    s_max = 255;
    v_max = 255;

    offset_x = CAP_W / 2;
    offset_y = CAP_H / 2;

    threshold = 60;

    rect_left_top.x = 0;
    rect_left_top.y = 0;
    rect_right_top.x = 0;
    rect_right_top.y = 0;
    rect_left_bottom.x = 0;
    rect_left_bottom.y = 0;
    rect_right_bottom.x = 0;
    rect_right_bottom.y = 0;

    for(int i=0; i<(int)(sizeof(pts_dst)/sizeof(pts_dst[0])); ++i){
        switch(i)
        {
        case 0: //左上
            pts_dst[i].x = 0;
            pts_dst[i].y = 0;
            break;
        case 1: //左下
            pts_dst[i].x = 0;
            pts_dst[i].y = CAP_H;
            break;
        case 2: //右下
            pts_dst[i].x = CAP_W;
            pts_dst[i].y = CAP_H;
            break;
        default://右上
            pts_dst[i].x = CAP_W;
            pts_dst[i].y = 0;
            break;
        }
    }

//    calib_img = cv::Mat::zeros(cv::Size(CAP_W, CAP_H), CV_8UC3);
}

Image::~Image(){
    stop = true;
}

bool Image::initCam(void){
    bool ret = true;

    cap.open(CAP_ID);
    if(cap.isOpened()){
        cap.set(cv::CAP_PROP_FRAME_WIDTH, CAP_W);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAP_H);
        cap.set(cv::CAP_PROP_POS_FRAMES, CAP_FPS);
    }else{
        ret = false;
    }

    return ret;
}

void Image::run(void){
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');

    while(1){

        thread_running_mutex_.lock();
        if(this->stop){
            thread_running_mutex_.unlock();
            break;
        }
        thread_running_mutex_.unlock();

        cap >> frame;
        cv::cvtColor(frame, dst, cv::COLOR_BGR2RGB);
        processingProTrans(dst);
        pro_trans_img = getProTransImage(dst);
        binalized_img = binalizeBackView(pro_trans_img);

        mode_mutex_.lock();
        if(calib_flag){
            calibModeImageAdd(dst);
            calibModeImageAdd(pro_trans_img);
        }
        mode_mutex_.unlock();

        mode_mutex_.lock();
        if(save_flag){
            time_t now = time(NULL);
            struct tm *pnow = localtime(&now);
            char now_time[128]="";
            sprintf(now_time,"%04d_%02d%02d_%02d_%02d_%02d_",pnow->tm_year+1900,pnow->tm_mon+1,pnow->tm_mday,pnow->tm_hour,pnow->tm_min,pnow->tm_sec);
            save_file_name = save_file_path + now_time;
            frame_writer.open(save_file_name + "raw_video.mp4", fourcc, CAP_FPS, cv::Size(CAP_W, CAP_H));
            protrans_writer.open(save_file_name + "protrans_video.mp4", fourcc, CAP_FPS, cv::Size(CAP_W, CAP_H));
            t0 = cv::getTickCount();
            save_flag = false;
        }
        mode_mutex_.unlock();

        if(frame_writer.isOpened()){
            frame_writer << frame;
            t1 = cv::getTickCount();
            time_log = (int64)((t1 - t0) * 1000 / cv::getTickFrequency());
        }
        if(protrans_writer.isOpened()){
            cv::cvtColor(pro_trans_img, save_protrans_img, cv::COLOR_RGB2BGR);
            protrans_writer << save_protrans_img;
        }

        emit valueChangedCam();
//        this->msleep(2);
    }
    cap.release();
    frame.release();
    dst.release();
    process_img.release();
    binalized_img.release();
    pro_trans_img.release();
    frame_writer.release();
    protrans_writer.release();

}

void Image::getImage(cv::Mat *image){
    if(frame_writer.isOpened()){
        char message[64]="";
        sprintf(message, "%02lld:%02lld:%02lld", (time_log / 1000) / 3600, ((time_log / 1000) / 60) % 60, (time_log / 1000) % 60);
        cv::circle(dst, cv::Point(550,30), 10, cv::Scalar(255,0,0), cv::FILLED);
        cv::putText(dst, "REC", cv::Point(570,40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,0,0), 2);
        cv::putText(dst, message, cv::Point(10,460), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,0,0), 2);
    }
    *image = dst.clone();
}

void Image::getSettingImage(cv::Mat *image){
    *image = process_img.clone();
}

void Image::getProTransImage(cv::Mat *image){
    if(protrans_writer.isOpened()){
        char message[64]="";
        sprintf(message, "%02lld:%02lld:%02lld", (time_log / 1000) / 3600, ((time_log / 1000) / 60) % 60, (time_log / 1000) % 60);
        cv::circle(pro_trans_img, cv::Point(550,30), 10, cv::Scalar(255,0,0), cv::FILLED);
        cv::putText(pro_trans_img, "REC", cv::Point(570,40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,0,0), 2);
        cv::putText(pro_trans_img, message, cv::Point(10,460), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,0,0), 2);
    }
    *image = pro_trans_img.clone();
}

void Image::getBinalizeBackImage(cv::Mat *image){
    *image = binalized_img.clone();
}

cv::Mat Image::maskImage(cv::Mat img){
    cv::Mat masked_img, hsv_img;
    cv::cvtColor(img, hsv_img, cv::COLOR_RGB2HSV);
    hsv_mutex_.lock();
    cv::Scalar scalar_min = cv::Scalar(h_min, s_min, v_min);
    cv::Scalar scalar_max = cv::Scalar(h_max, s_max, v_max);
    inRange(hsv_img, scalar_min, scalar_max, masked_img);
    hsv_mutex_.unlock();
    hsv_img.release();
    return masked_img;
}

cv::Mat Image::noiseReduction(cv::Mat img){
    cv::medianBlur(img, img, 15);
    const int opening_size = 1;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_OPEN,
        cv::Size(2 * opening_size + 1, 2 * opening_size + 1),
        cv::Point(opening_size, opening_size));

    cv::morphologyEx(img, img, cv::MORPH_OPEN, element);
    element.release();
    return img;
}

cv::Mat Image::getEdge(cv::Mat img){
    cv::Canny(img, img, 10, 255);
    return img;
}

cv::Mat Image::getContour(cv::Mat img){
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    cv::Scalar color(255, 255, 0);

    for(int i=0; i<(int)contours.size(); ++i){
        cv::approxPolyDP(contours.at(i), contours.at(i), 100, 1);
    }

    if(contours.size() == 0){
        return img;
    }
    std::vector<double> contour_area(contours.size());
    double max_area = 0.0;

    int max_area_idx = 0;

    for(int i=0; i<(int)contours.size(); ++i){
        contour_area.at(i) = cv::contourArea(contours.at(i));
        if(max_area < contour_area.at(i)){
            max_area = contour_area.at(i);
            max_area_idx = i;
        }
    }

    int n = 0;
    for(cv::Point k:contours.at(max_area_idx)){
        cv::putText(img, std::to_string(n), k, cv::FONT_HERSHEY_SIMPLEX, 1, color, 3);
        cv::circle(img, k, 5, color, cv::FILLED);
        n++;
    }

//    if(contours.at(max_area_idx).size() == 4){
//        int max_x = -1;
//        int max_y = -1;
//        int min_x = 10000;
//        int min_y = 10000;

//        for(int i=0; i<(int)contours.at(max_area_idx).size(); ++i){
//            if(max_x < contours.at(max_area_idx).at(i).x) max_x = contours.at(max_area_idx).at(i).x;
//            if(max_y < contours.at(max_area_idx).at(i).y) max_y = contours.at(max_area_idx).at(i).y;
//            if(min_x > contours.at(max_area_idx).at(i).x) min_x = contours.at(max_area_idx).at(i).x;
//            if(min_y > contours.at(max_area_idx).at(i).y) min_y = contours.at(max_area_idx).at(i).y;
//        }
//        rect_left_top.x = min_x;
//        rect_left_top.y = min_y;
//        rect_right_bottom.x = max_x;
//        rect_right_bottom.y = max_y;

//        int temp_x = 0;
//        int temp_idx = 0;

//        for(int i=0; i<(int)contours.at(max_area_idx).size(); ++i){
//            if(contours.at(max_area_idx).at(i).x != max_x && contours.at(max_area_idx).at(i).x != min_x){
//                temp_x = max_x - contours.at(max_area_idx).at(i).x;
//                if(temp_x > max_x - contours.at(max_area_idx).at(temp_idx).x){
//                    rect_right_top.x = contours.at(max_area_idx).at(temp_idx).x;
//                    rect_right_top.y = contours.at(max_area_idx).at(temp_idx).y;
//                    rect_left_bottom.x = contours.at(max_area_idx).at(i).x;
//                    rect_left_bottom.y = contours.at(max_area_idx).at(i).y;
//                    break;
//                }else if(temp_x < max_x - contours.at(max_area_idx).at(temp_idx).x){
//                    rect_right_top.x = contours.at(max_area_idx).at(i).x;
//                    rect_right_top.y = contours.at(max_area_idx).at(i).y;
//                    rect_left_bottom.x = contours.at(max_area_idx).at(temp_idx).x;
//                    rect_left_bottom.y = contours.at(max_area_idx).at(temp_idx).y;
//                    break;
//                }else{

//                }

//                temp_idx = i;
//            }
//        }

//        cv::line(img, rect_left_top,rect_left_bottom, color,2,cv::LINE_4);
//        cv::line(img, rect_left_top,rect_right_top, color,2,cv::LINE_4);
//        cv::line(img, rect_left_bottom,rect_right_bottom, color,2,cv::LINE_4);
//        cv::line(img, rect_right_top,rect_right_bottom, color,2,cv::LINE_4);
//    }

    return img;
}

cv::Mat Image::getProTransImage(cv::Mat img){
    rect_mutex_.lock();
    for(int i=0; i<(int)(sizeof(pts_src)/sizeof(pts_src[0])); ++i){
        switch(i)
        {
        case 0: //左上
            pts_src[i].x = rect_left_top.x;
            pts_src[i].y = rect_left_top.y;
            break;
        case 1: //左下
            pts_src[i].x = rect_left_bottom.x;
            pts_src[i].y = rect_left_bottom.y;
            break;
        case 2: //右下
            pts_src[i].x = rect_right_bottom.x;
            pts_src[i].y = rect_right_bottom.y;
            break;
        default://右上
            pts_src[i].x = rect_right_top.x;
            pts_src[i].y = rect_right_top.y;
            break;
        }
    }
    rect_mutex_.unlock();

    cv::Mat convert_mat = cv::getPerspectiveTransform(pts_src, pts_dst);
    cv::Mat out_img = cv::Mat::zeros(img.rows, img.cols, img.type());
    cv::warpPerspective(img, out_img, convert_mat, out_img.size());

    return out_img;
}


void Image::processingProTrans(cv::Mat img){
    process_img = maskImage(img);
    process_img = noiseReduction(process_img);
    process_img = getEdge(process_img);
    process_img = getContour(process_img);
}

cv::Mat Image::binalizeBackView(cv::Mat img){
    cv::Mat circleFrame = cv::Mat::zeros(cv::Size(CAP_W, CAP_H), CV_8UC1);
    cv::circle(circleFrame, cv::Point(CAP_W / 2, CAP_H / 2), CAP_W / 2 - CAP_H / 12 - 80, 255, -1); //ANTAM-Q用　マスク
    cv::bitwise_not(circleFrame, circleFrame);

    const int opening_size = 1;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_OPEN,
        cv::Size(2 * opening_size + 1, 2 * opening_size + 1),
        cv::Point(opening_size, opening_size));

    cv::Mat gray = cv::Mat(cv::Size(CAP_W, CAP_H), CV_8UC1);
    cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
    cv::bitwise_or(gray, circleFrame, gray);
    threshold_mutex_.lock();
    cv::threshold(gray, img, threshold, 255, cv::THRESH_BINARY_INV);
    threshold_mutex_.unlock();
    cv::morphologyEx(img, img, cv::MORPH_OPEN, element);

    mom = cv::moments(img);

    circleFrame.release();
    element.release();
    gray.release();

    return img;
}

cv::Point Image::calc_moment(cv::Moments mom){
    cv::Point return_pos;
    double dotnum_dbl, x_cm_dbl, y_cm_dbl;
    int dotnum;
    dotnum_dbl = mom.m00;
    dotnum = (int)dotnum_dbl;

    if (dotnum != 0) {
        x_cm_dbl = mom.m10 / dotnum_dbl;//重心のx座標
        y_cm_dbl = mom.m01 / dotnum_dbl;//重心のｙ座標
    }
    else {
        x_cm_dbl = CAP_W / 2;
        y_cm_dbl = CAP_H / 2;
    }
    return_pos.x = (int)x_cm_dbl;
    return_pos.y = (int)y_cm_dbl;
    return return_pos;

}

void Image::calibModeImageAdd(cv::Mat img){
    for(int y=0; y<CAP_H; y+=40){
        for(int x=0; x<CAP_W; x+=40){
            if(x != 0)
                cv::line(img, cv::Point(x, 0), cv::Point(x, CAP_H), cv::Scalar(255,0,0), 1, cv::LINE_4);
        }
        if(y != 0)
            cv::line(img, cv::Point(0,y), cv::Point(CAP_W, y), cv::Scalar(255,0,0), 1, cv::LINE_4);
    }

    cv::circle(img, cv::Point(offset_x, offset_y), 4, cv::Scalar(0,255,0), cv::FILLED);
}


