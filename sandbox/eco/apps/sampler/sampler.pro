#-------------------------------------------------
#
# Project created by QtCreator 2011-09-10T07:39:35
#
#-------------------------------------------------

# Build against the 10.5 SDK. Comment this out when building against
# Mac 10.6 or greater. Note that Qt 4.7 appears to be linked against the
# 10.5 SDK because it's missing the curious "extract" functions.
QMAKE_CXXFLAGS = \
    -mmacosx-version-min=10.5 \
    -isysroot /Developer/SDKs/MacOSX10.5.sdk \
    -universal

QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
CONFIG += x86 ppc

QT += core gui

TARGET = sampler
TEMPLATE = app

INCLUDEPATH += /Users/asutton/Code/boost/boost-1.47/

SOURCES += main.cpp \
        MainWindow.cpp

HEADERS += MainWindow.hpp \
    statistics.hpp \
    random.hpp \
    math.hpp \
    geometry.hpp \
    algorithm.hpp \
    util.hpp

FORMS   += MainWindow.ui
ICON    = sampler.icns

