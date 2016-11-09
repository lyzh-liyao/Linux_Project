TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    V4L2Operation.c \
    main.c \
    SocketOperation.c

HEADERS += \
    V4L2Operation.h \
    SocketOperation.h

