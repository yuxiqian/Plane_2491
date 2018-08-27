HEADERS += \
    mywidget.h \
    data.hpp \
    object.hpp \
    game.h \
    plane.hpp \
    bullet.hpp \
    item.hpp \
    explosion.hpp \
    map.hpp

SOURCES += \
    mywidget.cpp \
    helper.cpp \
    game.cpp \
    const.cpp

QT += widgets gui
QT += multimedia
QT += quickcontrols2
#QT += multimediawidgets

DISTFILES += \
    Resources/aircraft/enemy_plane_a.png \
    Resources/aircraft/enemy_plane_b.png \
    Resources/aircraft/enemy_plane_c.png \
    Resources/aircraft/player_assist_plane.png \
    Resources/aircraft/player_plane_blackpow.png \
    Resources/aircraft/player_plane_normal.png \
    Resources/effects/enemy_bullet_fire.png \
    Resources/effects/player_bullet_fire.png \
    Resources/explosion/explosion_frame_1.png \
    Resources/explosion/explosion_frame_2.png \
    Resources/explosion/explosion_frame_3.png \
    Resources/icon/icon.png \
    Resources/aircraft/enemy_plane_a.png \
    Resources/aircraft/enemy_plane_b.png \
    Resources/aircraft/enemy_plane_c.png \
    Resources/aircraft/player_assist_plane.png \
    Resources/aircraft/player_plane_blackpow.png \
    Resources/aircraft/player_plane_normal.png \
    updatelog

RESOURCES += \
    res.qrc


CONFIG += resources_big
