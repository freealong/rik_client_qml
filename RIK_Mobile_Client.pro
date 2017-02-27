TEMPLATE = app

QT += qml quick
CONFIG += c++11

INCLUDEPATH += Eigen \
               /home/wzf/Softwares/opencv/opencv-2.4.9/include/opencv

SOURCES += main.cpp \
    tcpclient.cpp \
    tcpcommon.cpp \
    udpclient.cpp \
    rik_client.cpp \
    robotinfo.cpp \
    cvSix/CCalibrator.cpp \
    cvSix/CLog.cpp \
    cvSix/CPosTable.cpp \
    cvSix/cvRun.cpp \
    cvSix/NFindCorners.cpp \
    cvSix/NFunc.cpp \
    cvSix/NImage.cpp \
    cvSix/NUtils.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    sharetypes.h \
    tcpclient.h \
    tcpcommon.h \
    udpclient.h \
    rik_client.h \
    robotinfo.h \
    cvSix/CCalibrator.h \
    cvSix/CLog.h \
    cvSix/CPosTable.h \
    cvSix/cvRun.h \
    cvSix/NFindCorners.h \
    cvSix/NFunc.h \
    cvSix/NImage.h \
    cvSix/NUtils.h


DISTFILES += \
    RIK_Mobile_Client.pro.user

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d
