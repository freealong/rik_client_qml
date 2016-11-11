TEMPLATE = app

QT += qml quick
CONFIG += c++11

INCLUDEPATH += Eigen

SOURCES += main.cpp \
    tcpclient.cpp \
    tcpcommon.cpp \
    udpclient.cpp \
    rik_client.cpp \
    robotinfo.cpp

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
    robotinfo.h
