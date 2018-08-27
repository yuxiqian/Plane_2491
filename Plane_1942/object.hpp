// object.hpp
// Created by 法好 on 2018/7/4.
//

#ifndef FIRSTFLIGHT_OBJECT_HPP
#define FIRSTFLIGHT_OBJECT_HPP

#include <QString>
#include <QDebug>
#include <QPixmap>

#define PLAYER_PLANE 0
#define ENEMY_PLANE 1
#define PLAYER_FIRE 2
#define ENEMY_FIRE 3

class Object {
  public:
    static const std::string RESOURCE_INDEX[14];
    friend bool judgeHitBox(const Object &, const Object &);
    int _x, _y;
    double Xvelocity, Yvelocity;
    QPixmap getPixmap() {
        QPixmap img;
        img.load(QString::fromStdString(RESOURCE_INDEX[resIndex]));
        //        qDebug() << "\t I'm object type" << this->objectType << "and
        //        loads picture" <<
        //        QString::fromStdString(RESOURCE_INDEX[resIndex]);
        //        if (this->objectType == 0) {
        return img;
        //        }
        //        QSize scaleSize(RES_PIXELS * (1 - (FRAME_HEIGHT - _y) /
        //        SCALE_CONSTANT),
        //                        RES_PIXELS *
        //                            (1 - (FRAME_HEIGHT - _y) /
        //                            SCALE_CONSTANT));
        //        QPixmap scaledImg = img.scaled(scaleSize);
        //        return scaledImg;
    }
    //    void setPngPath(std::string path) {
    //        this->pngPath = path;
    //    }
    bool judgeOverarea() {
        if (this->_x < -RES_PIXELS || this->_x > FRAME_WIDTH ||
            this->_y < -RES_PIXELS || this->_y > FRAME_HEIGHT) {
            return true;
        }
        return false;
    }

    // objectType = 0: game map
    // objectType = 1: player's plane
    // objectType = 2: enemy's planes
    // objectType = 3: player's bullets
    // objectType = 4: enemy's bullets
    // objectType = 51: explosion special frame 1
    // objectType = 52: explosion special frame 2
    // objectType = 53: explosion special frame 3
    // objectType = 6: player's assist plane
    // objectType = 71: score randomly add up
    // objectType = 72: blood cure
  protected:
    int hitBoxLeft, hitBoxTop, hitBoxWidth, hitBoxHeight;
    int resIndex;
    int objectType;
};

#endif // FIRSTFLIGHT_OBJECT_HPP
