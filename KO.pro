LIBS += /home/frode/Sync/Source/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -lSDL \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

#QMAKE_CXXFLAGS += -std=c++11



INCLUDEPATH += \
    /home/frode/Sync/Source/Urho3D/include \
    /home/frode/Sync/Source/Urho3D/include/Urho3D/ThirdParty \
    #/home/frode/Sync/Source/Urho3D/Source/Samples

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mastercontrol.cpp \
    player.cpp \
    sceneobject.cpp \
    tile.cpp \
    dungeon.cpp \
    kocam.cpp \
    inputmaster.cpp \
    frop.cpp \
    firepit.cpp \
    deco.cpp \
    wallcollider.cpp \
    floatingeye.cpp \
    npc.cpp

HEADERS += \
    mastercontrol.h \
    player.h \
    sceneobject.h \
    tile.h \
    dungeon.h \
    kocam.h \
    inputmaster.h \
    frop.h \
    firepit.h \
    deco.h \
    wallcollider.h \
    floatingeye.h \
    npc.h
