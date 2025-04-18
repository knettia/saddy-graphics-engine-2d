######################################################################
# Automatically generated by qmake (2.01a) ?? ????. 14 17:12:28 2013
######################################################################

QT += core gui opengl widgets
CONFIG += console opengl
TEMPLATE = app

TARGET =
DEPENDPATH += . \
              ../../include/
INCLUDEPATH += . \
               ../../include/

# Input
HEADERS += mainwindow.h

FORMS += mainwindow.ui

SOURCES += mainwindow.cpp \
    game.cpp \
    main.cpp

DESTDIR = ../../bin/

unix {
	DEFINES += "UNIX=1"
	DEFINES += "LINUX=1"
	DEFINES += "GCC=1"
}

win32 {
	DEFINES += "WIN32=1"
	DEFINES +=  "MINGW=1"
}

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/ -lsaddy-debug -lsaddy-phy52d-debug -lsaddy-qt-widget-debug
    TARGET = example-game-in-qt-debug
}

CONFIG(release, debug|release) {
    LIBS += -L../../lib/ -lsaddy-release -lsaddy-phy52d-release -lsaddy-qt-widget-release
    TARGET = example-game-in-qt-release
}

unix {
        LIBS += -lGL
        LIBS += -lGLU
        LIBS += -lX11
        LIBS += -lxcb
}

win32 {
        LIBS += -lglu32 -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool  -lshell32 -lcomdlg32 -ladvapi32
}

QMAKE_CXXFLAGS += -Wno-reorder -Wno-unused -Wno-sign-compare -w
