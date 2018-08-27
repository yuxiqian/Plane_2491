// game.h
// Created by 法好 on 2018/7/4.
//

#ifndef GAME_H
#define GAME_H
#include "const.cpp"
#include "data.hpp"
#include "object.hpp"
#include "bullet.hpp"
#include "item.hpp"
#include "plane.hpp"
#include "map.hpp"
#include "explosion.hpp"
#include <qmediaplayer.h>
#include <qmediaplaylist.h>
class Game {
  public:
    void startGame();
    int updateFrame();
    Plane &getPlayerPlane();
    void shootBullet();
    void generateEnemy();
    void generatePresent();
    void generateAssist();
    void doExplosion(int, int);
    Object *getObject(int, int);
    int getObjectCount(int);
    void putObject(int, void *);
    void startBulletRain();
    void stopBulletRain();
    QString showStats();
    void clearBomb();
    void clearElement();
    bool isPause = false;
    GameData gameData;

  private:
    void updateMove();
    bool bulletRainMode;
    std::vector<Map> gameMap;
    std::vector<Plane> playerPlane;
    std::vector<Plane> enemyPlane;
    std::vector<Bullet> playerBullet;
    std::vector<Plane> playerAssistPlane;
    std::vector<Bullet> enemyBullet;
    std::vector<Explosion> explosionEffect;
    std::vector<Item> specialGiftItem;
};
#endif // GAME_H
