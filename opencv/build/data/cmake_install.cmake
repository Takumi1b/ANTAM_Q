# Install script for directory: /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libs" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opencv4/haarcascades" TYPE FILE FILES
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_eye.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_frontalcatface.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_frontalcatface_extended.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_frontalface_alt.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_frontalface_alt2.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_frontalface_alt_tree.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_frontalface_default.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_fullbody.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_lefteye_2splits.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_license_plate_rus_16stages.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_lowerbody.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_profileface.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_righteye_2splits.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_russian_plate_number.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_smile.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/haarcascades/haarcascade_upperbody.xml"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libs" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opencv4/lbpcascades" TYPE FILE FILES
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/lbpcascades/lbpcascade_frontalcatface.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/lbpcascades/lbpcascade_frontalface.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/lbpcascades/lbpcascade_frontalface_improved.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/lbpcascades/lbpcascade_profileface.xml"
    "/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/data/lbpcascades/lbpcascade_silverware.xml"
    )
endif()

