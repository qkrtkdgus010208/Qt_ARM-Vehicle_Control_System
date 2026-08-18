QT += widgets
QT += core gui multimedia multimediawidgets
QT += serialport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    drivelog.cpp \
    main.cpp \
    mainwindow.cpp \
    camerathread.cpp \
    speeddial.cpp \
    steeringwheel.cpp

HEADERS += \
    drivelog.h \
    mainwindow.h \
    camerathread.h \
    speeddial.h \
    steeringwheel.h

FORMS += \
    drivelog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myresource.qrc

DISTFILES += \
    image/3.png
