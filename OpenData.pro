TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        datacollector.cpp \
        datamanager.cpp \
        histogram.cpp \
        main.cpp \
        parking.cpp

HEADERS += \
    datacollector.h \
    datamanager.h \
    histogram.h \
    parking.h
