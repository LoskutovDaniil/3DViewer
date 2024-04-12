QT       += core gui

QT       += widgets

QT       += opengl

QT       += openglwidgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../GL/widget.cpp \
    ../gifLib/src/3rdParty/giflib/dgif_lib.c \
    ../gifLib/src/3rdParty/giflib/egif_lib.c \
    ../gifLib/src/3rdParty/giflib/gif_err.c \
    ../gifLib/src/3rdParty/giflib/gif_font.c \
    ../gifLib/src/3rdParty/giflib/gif_hash.c \
    ../gifLib/src/3rdParty/giflib/gifalloc.c \
    ../gifLib/src/3rdParty/giflib/quantize.c \
    ../gifLib/src/gifimage/qgifimage.cpp \
    ../s21_help.c \
    ../s21_validation.c \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../GL/widget.h \
    ../gifLib/src/3rdParty/giflib/gif_hash.h \
    ../gifLib/src/3rdParty/giflib/gif_lib.h \
    ../gifLib/src/3rdParty/giflib/gif_lib_private.h \
    ../gifLib/src/gifimage/qgifglobal.h \
    ../gifLib/src/gifimage/qgifimage.h \
    ../gifLib/src/gifimage/qgifimage_p.h \
    ../s21_viewer.h \
    mainwindow.h


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../GL/shaders.qrc

DISTFILES += \
    ../GL/fshader.fsh \
    ../GL/vshader.vsh \
    ../gifLib/src/3rdParty/giflib.pri \
    ../gifLib/src/3rdParty/giflib/AUTHORS \
    ../gifLib/src/3rdParty/giflib/COPYING \
    ../gifLib/src/3rdParty/giflib/README \
    ../gifLib/src/gifimage/doc/qtgifimage.qdocconf \
    ../gifLib/src/gifimage/doc/src/examples.qdoc \
    ../gifLib/src/gifimage/doc/src/index.qdoc \
    ../gifLib/src/gifimage/doc/src/qtgifimage.qdoc \
    ../gifLib/src/gifimage/doc/src/usage.qdoc \
    ../gifLib/src/gifimage/qtgifimage.pri

SUBDIRS += \
    ../gifLib/src/gifimage/doc/snippets/doc_src_qtgifimage.pro \
    ../gifLib/src/gifimage/gifimage.pro


