QT       += core gui  serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#include($$PWD/TTKWidgetTools/TTKModule/TTKModule.pro)
include($$PWD/TTKWidgetTools-2.2.0.0/TTKModule/TTKModule.pro)
TEMPLATE = app
TARGET = DevilTools

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    serialprocess.cpp \
    zr_modbus.c

HEADERS += \
    mainwindow.h \
    serialprocess.h \
    settingconfig.h \
    zr_modbus.h

FORMS += \
    mainwindow.ui

#TRANSLATIONS += \    serial_test_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
win32:RC_FILE = SerialPort.rc

DISTFILES += \
    SerialPort.rc

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-serial_test-Desktop_Qt_5_12_12_MinGW_32_bit-Debug/release/libTTKCore.a
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-serial_test-Desktop_Qt_5_12_12_MinGW_32_bit-Debug/debug/libTTKCore.a
#else:unix: LIBS += -L$$PWD/../../build-serial_test-Desktop_Qt_5_12_12_MinGW_32_bit-Debug/ -lTTKCore

#INCLUDEPATH += $$PWD/../../build-serial_test-Desktop_Qt_5_12_12_MinGW_32_bit-Debug/debug
#DEPENDPATH += $$PWD/../../build-serial_test-Desktop_Qt_5_12_12_MinGW_32_bit-Debug/debug



