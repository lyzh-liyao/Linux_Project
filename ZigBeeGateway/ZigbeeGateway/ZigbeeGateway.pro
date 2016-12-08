TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CFLAGS += -std=c99
LIBS += -lpthread
SOURCES += main.c \
    UartDriver/UartDriver.c \
    Socket/SocketOperation.c

HEADERS += \
    UartDriver/UartDriver.h \
    Socket/SocketOperation.h

