#-------------------------------------------------
#
# Project created by QtCreator 2014-01-31T10:01:07
# For MPI version needs to write `qmake PartsEngine.pro "MODES=mpi"` in console.
#
#-------------------------------------------------

TARGET = PartsEngine
QT       += core

SUBDIRS += tests

QT       -= gui

SOURCES += Vect.cpp \
    config.cpp \
    Part.cpp \
    distributionLaw.cpp \
    statemachinebase.cpp \
    PartArray.cpp \
    StateMachine.cpp \
    StateMachineFree.cpp \
    squarespinicearray.cpp \
    squarespinicecell.cpp \
    wanglandau.cpp \
    wanglandauparallel.cpp \
    wanglandauparallelwalker.cpp \
    random.cpp \
    honeycombspinicearray.cpp \
    honeycombspinicecell.cpp \
    squarelattice.cpp \
    amorphousarray.cpp \
    loadhelper.cpp \
    savehelper.cpp \
    dos.cpp \
    squareisinglattice.cpp

HEADERS += \
    Vect.h \
    random.h \
    Part.h \
    statemachinebase.h \
    StateMachine.h \
    StateMachineFree.h \
    PartArray.h \
    config.h \
    squarespinicecell.h \
    squarespinicearray.h \
    honeycombspinicecell.h \
    honeycombspinicearray.h \
    squarelattice.h \
    distributionLaw.h \
    wanglandau.h \
    wanglandauparallelwalker.h \
    wanglandauparallel.h \
    amorphousarray.h \
    loadhelper.h \
    savehelper.h \
    dos.h \
    squareisinglattice.h


CONFIG(debug,debug|release) {
    SOURCES += main.cpp
    TEMPLATE = app
    CONFIG += console
}

CONFIG(release,debug|release){
    TEMPLATE = lib
    CONFIG += staticlib
    DESTDIR = $$PWD
}

OTHER_FILES += \
    README.md \
    .gitignore

CONFIG += c++11

