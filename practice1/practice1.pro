TEMPLATE = app
TARGET = practice1
INCLUDEPATH += .

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += practice1.h
SOURCES += practice1.cpp

# Add widgets (e.g. text)
QT += widgets
