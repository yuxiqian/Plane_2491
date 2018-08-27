// data.hpp
// Created by 法好 on 2018/7/4.
//

#ifndef FIRSTFLIGHT_DATA_HPP
#define FIRSTFLIGHT_DATA_HPP

#include <QFile>
#include <QDataStream>

class GameData {
  public:
    GameData() { readHighScore(); }

    int getGameFlag() { return this->gameFlag; }

    void setGameFlag(int flag) {
        if (flag >= 0 && flag <= 2) {
            this->gameFlag = flag;
            return;
        }
    }

    int Score(int value = 0) {
        this->score += value;
        score = (score > 0 ? score : 0);
        return this->score;
    }

    int Blood(int value = 0) {
        this->blood += value;
        blood = (blood > 100 ? 100 : blood);
        return this->blood;
    }

    void resetData() {
        this->blood = 0;
        this->score = 0;
    }

    int highScore;

    void readHighScore() {
        QFile file("highscore.dat");
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            in >> this->highScore;
            file.close();
        } else {
            file.close();
            file.open(QIODevice::WriteOnly);
            QDataStream out(&file);
            out << 0;
            file.close();
        }
    }

  private:
    int gameFlag = 0;
    // gameFlag = 0: before starting a game
    // gameFlag = 1: game going on
    // gameFlag = 2: game over

    int score = 50000;
    int blood = 100;
};

#endif // FIRSTFLIGHT_DATA_HPP
