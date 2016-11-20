TARGET = ko

LIBS += ../KO/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

QMAKE_CXXFLAGS += -std=c++1y

INCLUDEPATH += \
    ../KO/Urho3D/include \
    ../KO/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mastercontrol.cpp \
    sceneobject.cpp \
    player.cpp \
    ko.cpp \
    tile.cpp \
    dungeon.cpp \
    kocam.cpp \
    controllable.cpp \
    inputmaster.cpp \
    frop.cpp \
    firepit.cpp \
    deco.cpp \
    wallcollider.cpp \
    floatingeye.cpp \
    npc.cpp \
    luckey.cpp

HEADERS += \
    mastercontrol.h \
    sceneobject.h \
    player.h \
    ko.h \
    tile.h \
    dungeon.h \
    kocam.h \
    controllable.h \
    inputmaster.h \
    frop.h \
    firepit.h \
    deco.h \
    wallcollider.h \
    floatingeye.h \
    npc.h \
    luckey.h
