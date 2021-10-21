TEMPLATE = app
TARGET = keylightcontrol
INCLUDEPATH += .

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += KeyLightControlWindow.h
SOURCES += main.cpp KeyLightControlWindow.cpp

# Add widgets (e.g. text)
QT += widgets network
