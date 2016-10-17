QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = edit
TEMPLATE = app


SOURCES += main.cpp \
    main_window.cpp \
    editor.cpp \
    controls/label.cpp \
    editor/cursor.cpp \
    editor/coordinates.cpp

HEADERS  += \
    _includes/main_window.h \
    _includes/editor.h \
    _includes/controls.h

INCLUDEPATH += _includes
