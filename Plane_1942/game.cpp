// game.hpp
// Created by 法好 on 2018/7/4.
//
#ifndef GAME_HPP
#define GAME_HPP

#include "game.h"
#include "helper.cpp"

#include <QMediaPlayer>

const std::string Object::RESOURCE_INDEX[14] = {
    ":/res/background/bg.png",
    ":/res/aircraft/player_plane_normal.png",
    ":/res/aircraft/player_assist_plane.png",
    ":/res/aircraft/enemy_plane_a.png",
    ":/res/aircraft/enemy_plane_b.png",
    ":/res/aircraft/enemy_plane_c.png",
    ":/res/effects/enemy_bullet_fire.png",
    ":/res/effects/player_bullet_fire.png",
    ":/res/explosion/explosion_frame_1.png",
    ":/res/explosion/explosion_frame_2.png",
    ":/res/explosion/explosion_frame_3.png",
    ":/res/gift/gray_gift.png",
    ":/res/gift/heal.png",
    ":/res/aircraft/player_plane_blackpow.png"};

void Game::startGame() {
    randomize();
    Map bg(0, 0);
    this->gameMap.push_back(bg);
    Plane plne(PLAYER_PLANE_NORMAL, 200, 500);
    this->playerPlane.push_back(plne);
}

int Game::updateFrame() {
    int mediaFlag = 0;
    if (this->gameData.Score() > MAX_SCORE) {
        this->gameData.setGameFlag(2);
    }
    updateMove();
    if (this->gameData.getGameFlag() == 1) {
        this->gameData.Score(19);
    }
    if (this->gameData.Blood() <= 0) {
        this->gameData.setGameFlag(2);
        return mediaFlag;
    }
    if (bulletRainMode) {
        if (gameData.Score() >= BULLET_COST) {
            Bullet rainBlt(PLAYER_BULLET, this->getPlayerPlane()._x,
                           this->getPlayerPlane()._y,
                           20 * getPossibility() - 10);
            this->playerBullet.push_back(rainBlt);
            this->gameData.Score(-BULLET_COST);
        } else {
            bulletRainMode = false;
            this->getPlayerPlane().setDefaultSkin();
        }
    }
    for (auto &i : enemyPlane) {
        if (getPossibility() <
            i.shootPossibility *
                (1 - static_cast<double>(abs(i._x - playerPlane.at(0)._x)) /
                         FRAME_WIDTH)) {
            if (abs(i._x - this->getPlayerPlane()._x) < RES_PIXELS) {
                i.Xvelocity *= 1.5;
            }
            Bullet newEnemyBullet(ENEMY_BULLET, i._x, i._y);
            enemyBullet.push_back(newEnemyBullet);
        }
    }
    {

        std::vector<Bullet>::iterator enemyBulletItor;
        for (enemyBulletItor = enemyBullet.begin();
             enemyBulletItor != enemyBullet.end(); ++enemyBulletItor) {
            std::vector<Plane>::iterator playerPlaneItor;
            if (enemyBulletItor->_y > FRAME_HEIGHT) {
                enemyBullet.erase(enemyBulletItor--);
                break;
            } else if (judgeHitBox(*enemyBulletItor, playerPlane.at(0))) {
                enemyBullet.erase(enemyBulletItor--);
                mediaFlag = PLANE_GOTHIT;
                // BLOOD_DEDUCTION_PER_HIT
                gameData.Blood(-BLOOD_DEDUCTION_PER_HIT);
                // SCORE_PUNISH_PER_HIT
                gameData.Score(-SCORE_PUNISH_PER_HIT);
            } else {
                for (playerPlaneItor = playerAssistPlane.begin();
                     playerPlaneItor != playerAssistPlane.end();
                     ++playerPlaneItor) {
                    if (judgeHitBox(*playerPlaneItor, *enemyBulletItor)) {
                        int expPosX = playerPlaneItor->_x,
                            expPosY = playerPlaneItor->_y;
                        enemyBullet.erase(enemyBulletItor--);
                        playerAssistPlane.erase(playerPlaneItor--);
                        mediaFlag = PLANE_EXPLOSION;
                        doExplosion(expPosX, expPosY);
                    }
                }
            }
        }
        // check if the player's plane got hit
    }

    // judge the hit between the player's plane and the enemies' plane
    {
        std::vector<Plane>::iterator enemyPlaneItor;
        for (enemyPlaneItor = enemyPlane.begin();
             enemyPlaneItor != enemyPlane.end(); ++enemyPlaneItor) {
            if (judgeHitBox(*enemyPlaneItor, this->playerPlane.at(0))) {
                doExplosion(enemyPlaneItor->_x, enemyPlaneItor->_y);
                enemyPlane.erase(enemyPlaneItor--);
                mediaFlag = PLANE_EXPLOSION;
                this->gameData.Blood(-BLOOD_DEDUCTION_PER_HIT);
                this->playerPlane.at(0).Xvelocity = 0;
                break;
            }
            std::vector<Plane>::iterator playerPlaneItor;
            for (playerPlaneItor = playerAssistPlane.begin();
                 playerPlaneItor != playerAssistPlane.end();
                 ++playerPlaneItor) {
                if (judgeHitBox(*playerPlaneItor, *enemyPlaneItor)) {
                    doExplosion(playerPlaneItor->_x, playerPlaneItor->_y);
                    mediaFlag = PLANE_EXPLOSION;
                    enemyPlane.erase(enemyPlaneItor--);
                    playerAssistPlane.erase(playerPlaneItor--);
                }
            }
        }
    }

    {
        std::vector<Plane>::iterator enemyPlaneItor;
        for (enemyPlaneItor = enemyPlane.begin();
             enemyPlaneItor != enemyPlane.end(); ++enemyPlaneItor) {
            std::vector<Bullet>::iterator playerBulletItor;
            for (playerBulletItor = playerBullet.begin();
                 playerBulletItor != playerBullet.end(); ++playerBulletItor) {
                if (judgeHitBox(*playerBulletItor, *enemyPlaneItor)) {
                    //                    doExplosion(enemyPlaneItor->_x,
                    //                    enemyPlaneItor->_y);
                    //                    enemyPlane.erase(enemyPlaneItor--);
                    enemyPlaneItor->blood -= 10;
                    playerBullet.erase(playerBulletItor--);
                    this->gameData.Score(BONUS_SCORE / 5);
                    mediaFlag = PLANE_GOTHIT;
                }
            }
            if (enemyPlaneItor->blood <= 0) {
                doExplosion(enemyPlaneItor->_x, enemyPlaneItor->_y);
                if (enemyPlaneItor->isBossFlag()) {
                    Item fallGift(GIFT_ITEM, enemyPlaneItor->_x,
                                  enemyPlaneItor->_y);
                    specialGiftItem.push_back(fallGift);
                }
                enemyPlane.erase(enemyPlaneItor--);
                mediaFlag = PLANE_EXPLOSION;
                this->gameData.Score(BONUS_SCORE);
            }
        }
        // check if the enemies' plane got hit

        // check the gift/cure situation
        {
            std::vector<Item>::iterator giftItor;
            for (giftItor = specialGiftItem.begin();
                 giftItor != specialGiftItem.end(); ++giftItor) {
                if (judgeHitBox(*giftItor, this->playerPlane.at(0))) {
                    switch (giftItor->giftType) {
                    case GIFT_ITEM:
                        this->gameData.Score(getRandom(10000, 100000));
                        break;
                    case HEAL_ITEM:
                        this->gameData.Blood(100);
                        break;
                    }
                    mediaFlag = GOT_GIFT;
                    specialGiftItem.erase(giftItor--);
                    break;
                }
            }
        }
    }
    return mediaFlag;
}

Plane &Game::getPlayerPlane() { return this->playerPlane.at(0); }

void Game::shootBullet() {
    auto i = this->getPlayerPlane();
    Bullet pyrBlt(PLAYER_BULLET, i._x, i._y,
                  this->getPlayerPlane().Xvelocity / 4);
    this->playerBullet.push_back(pyrBlt);
    // shoot a bullet from the main plane

    for (auto &i : this->playerAssistPlane) {
        Bullet pyrBlt(PLAYER_BULLET, i._x, i._y);
        this->playerBullet.push_back(pyrBlt);
        // shoot a bullet from every assist plane
    }
}

void Game::generateEnemy() {
    int enemyFactor = getRandom(1, 100);
    if (enemyFactor > 95) {
        Plane planeTypeA(ENEMY_PLANE_A, getRandom(0, FRAME_WIDTH - RES_PIXELS),
                         -RES_PIXELS);
        this->enemyPlane.push_back(planeTypeA);
    } else if (enemyFactor > 80) {
        for (int i = 0; i < getRandom(1, 5); ++i) {
            Plane planeTypeB(ENEMY_PLANE_B,
                             getRandom(0, FRAME_WIDTH - RES_PIXELS),
                             -RES_PIXELS - i * RES_PIXELS);
            this->enemyPlane.push_back(planeTypeB);
        }
    } else if (enemyFactor > 70) {
        Plane planeTypeC(ENEMY_PLANE_C, getRandom(0, FRAME_WIDTH - RES_PIXELS),
                         -RES_PIXELS);
        this->enemyPlane.push_back(planeTypeC);
    }
}

void Game::generatePresent() {
    if (getRandom(1, 2) == 1) {
        Item scoreGift(GIFT_ITEM, getRandom(0, FRAME_WIDTH - RES_PIXELS),
                       -RES_PIXELS);
        this->specialGiftItem.push_back(scoreGift);
    } else {
        Item healGift(HEAL_ITEM, getRandom(0, FRAME_WIDTH - RES_PIXELS),
                      -RES_PIXELS);
        this->specialGiftItem.push_back(healGift);
    }
}

void Game::generateAssist() {
    if (this->gameData.Score() > ASSIST_PLANE_COST) {
        this->gameData.Score(-ASSIST_PLANE_COST);
        Plane assPln(PLAYER_ASSIST_PLANE,
                     getRandom(0, FRAME_WIDTH - RES_PIXELS),
                     FRAME_HEIGHT - RES_PIXELS * 1.5);
        this->playerAssistPlane.push_back(assPln);
    }
}

void Game::doExplosion(int x, int y) {
    Explosion tempExp(x, y);
    this->explosionEffect.push_back(tempExp);
}

Object *Game::getObject(int type, int index = 0) {
    switch (type) {
    case 0:
        return (Object *) &gameMap[index];
        break;
    case 1:
        return (Object *) &playerPlane[index];
        break;
    case 2:
        return (Object *) &enemyPlane[index];
        break;
    case 3:
        return (Object *) &playerBullet[index];
        break;
    case 4:
        return (Object *) &enemyBullet[index];
        break;
    case 5:
        return (Object *) &explosionEffect[index];
        break;
    case 6:
        return (Object *) &playerAssistPlane[index];
        break;
    case 7:
        return (Object *) &specialGiftItem[index];
        break;
    }
    return NULL;
}

int Game::getObjectCount(int type) {
    switch (type) {
    case 0:
        return gameMap.size();
        break;
    case 1:
        return playerPlane.size();
        break;
    case 2:
        return enemyPlane.size();
        break;
    case 3:
        return playerBullet.size();
        break;
    case 4:
        return enemyBullet.size();
        break;
    case 5:
        return explosionEffect.size();
        break;
    case 6:
        return playerAssistPlane.size();
        break;
    case 7:
        return specialGiftItem.size();
        break;
    }
    return 0;
}

void Game::putObject(int type, void *objPtr) {
    qDebug() << "Gotta a pointer points to" << objPtr;
    qDebug() << "current type: " << type;
    switch (type) {
    case 0:
        //        qDebug() << "resolving an object 0";
        this->gameMap.push_back(*((Map *) objPtr));
        break;
    case 1:
        this->playerPlane.push_back(*((Plane *) objPtr));
        break;
    case 2:
        this->enemyPlane.push_back(*((Plane *) objPtr));
        break;
    case 3:
        this->playerBullet.push_back(*((Bullet *) objPtr));
        break;
    case 4:
        this->enemyBullet.push_back(*((Bullet *) objPtr));
        break;
    case 5:
        this->explosionEffect.push_back(*((Explosion *) objPtr));
        break;
    case 6:
        this->playerAssistPlane.push_back(*((Plane *) objPtr));
        break;
    case 7:
        this->specialGiftItem.push_back(*((Item *) objPtr));
        break;
    }
}

void Game::startBulletRain() {
    if (this->gameData.Score() <= BULLET_COST) {
        return;
    }
    this->bulletRainMode = true;
    this->getPlayerPlane().setBulletRainSkin();
}

void Game::stopBulletRain() {
    this->bulletRainMode = false;
    this->getPlayerPlane().setDefaultSkin();
}

QString Game::showStats() {
    QString description;
    for (int i = 0; i < 8; ++i) {
        description.append(QString("Index %1, contains %2 elements. \n")
                               .arg(i)
                               .arg(getObjectCount(i)));
        for (int j = 0; j < getObjectCount(i); ++j) {
            description.append(QString("\tLies at _x: %1, _y: %2\n")
                                   .arg(getObject(i, j)->_x)
                                   .arg(getObject(i, j)->_y));
        }
    }
    return description;
}

void Game::clearBomb() {
    std::vector<Bullet>::iterator enemyBulletItor;
    for (enemyBulletItor = enemyBullet.begin();
         enemyBulletItor != enemyBullet.end(); ++enemyBulletItor) {
        if (this->gameData.Score(-BULLET_COST) <= 0) {
            break;
        }
            this->doExplosion(enemyBulletItor->_x, enemyBulletItor->_y);
            this->enemyBullet.erase(enemyBulletItor--);
    }
}

void Game::clearElement() {
    this->gameMap.clear();
    this->playerPlane.clear();
    this->enemyPlane.clear();
    this->playerBullet.clear();
    this->playerAssistPlane.clear();
    this->enemyBullet.clear();
    this->explosionEffect.clear();
    this->specialGiftItem.clear();
}

void Game::updateMove() {
    for (int i = 0; i <= 7; ++i) {
        switch (i) {
        case 0:
            // update map moving
            for (Object &i : gameMap) {
                i._y += i.Yvelocity;
                if (i._y > 0) {
                    i._y -= (MAP_HEIGHT - FRAME_HEIGHT);
                }
            }
            break;
        case 1: {
            for (auto &i : playerPlane) {
                i._x += i.Xvelocity;
                deaccelerate(i.Xvelocity);
                if (i._x < 0) {
                    i.Xvelocity = 0;
                    i._x = 0;
                }
                if (i._x > FRAME_WIDTH - RES_PIXELS) {
                    i.Xvelocity = 0;
                    i._x = FRAME_WIDTH - RES_PIXELS;
                }

                i._y += i.Yvelocity;
                deaccelerate(i.Yvelocity);
                if (i._y < PLANE_Y_LIMIT) {
                    i.Yvelocity = 0;
                    i._y = PLANE_Y_LIMIT;
                }
                if (i._y > FRAME_HEIGHT - RES_PIXELS) {
                    i.Yvelocity = 0;
                    i._y = FRAME_HEIGHT - RES_PIXELS;
                }
            }
        } break;
        case 2: {
            int currPlayerX = this->getPlayerPlane()._x;
            std::vector<Plane>::iterator enemyPlaneItor;
            for (enemyPlaneItor = enemyPlane.begin();
                 enemyPlaneItor != enemyPlane.end(); ++enemyPlaneItor) {
                enemyPlaneItor->Yvelocity += (getPossibility() - 0.5);
                if (!enemyPlaneItor->isBossFlag()) {
                    if (enemyPlaneItor->_x < currPlayerX) {
                        enemyPlaneItor->Xvelocity += (getPossibility() - 0.3);
                        //                        qDebug() << "\t A plane
                        //                        attempted to get closer from
                        //                        left side, now with speed" <<
                        //                        enemyPlaneItor->Xvelocity;
                    } else if (enemyPlaneItor->_x > currPlayerX + RES_PIXELS) {
                        enemyPlaneItor->Xvelocity -= (getPossibility() - 0.3);
                        //                        qDebug() << "\t A plane
                        //                        attempted to get closer from
                        //                        right side, now with speed" <<
                        //                        enemyPlaneItor->Xvelocity;
                    } else {
                        deaccelerate(enemyPlaneItor->Xvelocity);
                    }
                } else {
                    enemyPlaneItor->Xvelocity += (getPossibility() - 0.5);
                }
                // randomly accelerate and deaccelerate
                if ((enemyPlaneItor->_y += enemyPlaneItor->Yvelocity) >
                    FRAME_HEIGHT) {
                    enemyPlane.erase(enemyPlaneItor--);
                    break;
                } else if (enemyPlaneItor->_y < -RES_PIXELS) {
                    enemyPlane.erase(enemyPlaneItor--);
                    break;
                } else if ((enemyPlaneItor->_x += enemyPlaneItor->Xvelocity) >
                           FRAME_WIDTH) {
                    enemyPlane.erase(enemyPlaneItor--);
                    break;
                } else if (enemyPlaneItor->_x < -RES_PIXELS) {
                    enemyPlane.erase(enemyPlaneItor--);
                    break;
                }
            }
        } break;
        case 3:
            // update players bullet moving
            {
                std::vector<Bullet>::iterator playerBulletItor;
                for (playerBulletItor = playerBullet.begin();
                     playerBulletItor != playerBullet.end();
                     ++playerBulletItor) {
                    playerBulletItor->_y -= playerBulletItor->Yvelocity;
                    playerBulletItor->_x += playerBulletItor->Xvelocity;
                    if (playerBulletItor->judgeOverarea()) {
                        playerBullet.erase(playerBulletItor--);
                    }
                }
            }

            break;
        case 4:
            // update enemies bullet moving
            {
                std::vector<Bullet>::iterator enemyBulletItor;
                for (enemyBulletItor = enemyBullet.begin();
                     enemyBulletItor != enemyBullet.end(); ++enemyBulletItor) {
                    if ((enemyBulletItor->_y += BULLET_MOVEMENT_SPEED) >
                        FRAME_HEIGHT) {
                        enemyBullet.erase(enemyBulletItor--);
                    }
                }
            }

            break;
        case 5: {
            std::vector<Explosion>::iterator explosionItor;
            for (explosionItor = explosionEffect.begin();
                 explosionItor != explosionEffect.end(); ++explosionItor) {
                if (explosionItor->updateExplosionFrame()) {
                    explosionEffect.erase(explosionItor--);
                }
            }
        } break;
        case 6:
            for (auto &i : playerAssistPlane) {
                i.Xvelocity = playerPlane.at(0).Xvelocity;
                i._x += i.Xvelocity;
                deaccelerate(i.Xvelocity);
                if (i._x < 0) {
                    i.Xvelocity = 0;
                    i._x = 0;
                }
                if (i._x > FRAME_WIDTH - RES_PIXELS) {
                    i.Xvelocity = 0;
                    i._x = FRAME_WIDTH - RES_PIXELS;
                }
            }
            break;
        case 7: {
            std::vector<Item>::iterator giftItor;
            for (giftItor = specialGiftItem.begin();
                 giftItor != specialGiftItem.end(); ++giftItor) {
                if ((giftItor->_y += giftItor->Yvelocity) > FRAME_HEIGHT) {
                    specialGiftItem.erase(giftItor--);
                }
            }
        } break;
        }
    }
}


#endif
