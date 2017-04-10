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

unix {
    isEmpty(DATADIR) {
        DATADIR = ~/.local/share
    }
    DEFINES += DATADIR=\\\"$${DATADIR}/ko\\\"

    target.path = /usr/games/
    INSTALLS += target

    resources.path = $$DATADIR/luckey/ko/
    resources.files = Resources/*
    INSTALLS += resources

    icon.path = $$DATADIR/icons/
    icon.files = ko.svg
    INSTALLS += icon

    desktop.path = $$DATADIR/applications/
    desktop.files = ko.desktop
    INSTALLS += desktop
}
