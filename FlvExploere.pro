TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ScriptDataTag.cpp \
    Header.cpp \
    AudioDataTag.cpp \
    VideoDataTag.cpp \
    MarkdownPrinter.cpp

QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    helper.h \
    ScriptDataTag.h \
    Header.h \
    AudioDataTag.h \
    VideoDataTag.h \
    MarkdownPrinter.h
