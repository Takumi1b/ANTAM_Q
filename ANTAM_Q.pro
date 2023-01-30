QT       += core gui
QT       += serialport
QT       += widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += -std=c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dynamixel.cpp \
    image.cpp \
    main.cpp \
    mainwindow.cpp \
    serial.cpp

HEADERS += \
    dynamixel.h \
    image.h \
    mainwindow.h \
    serial.h

FORMS += \
    mainwindow.ui


INCLUDEPATH += /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/install/include/opencv4
LIBS += -L /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/install/lib -lopencv_world -ldxlib
#LIBS += -L /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/DXLIB_v4.6/DXLIB

#LIBS += -L/usr/local/lib

#LIBS += -L/opt/homebrew/Cellar/opencv/4.7.0/lib -lopencv_core -lopencv_imgproc -lopencv_features2d -lopencv_highgui -lopencv_calib3d -lopencv_imgcodecs -lopencv_videoio
QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0


#INCLUDEPATH += /opt/homebrew/Cellar/opencv/4.7.0/include/opencv4
#DEPENDPATH += /opt/homebrew/Cellar/opencv/4.7.0/include/opencv4

#QT_CONFIG -= no-pkg-config
#CONFIG += link_pkgconfig
#PKGCONFIG += opencv

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

macos{
    QMAKE_INFO_PLIST = Info.plist
    ICON = ./icon/icon.icns
}
