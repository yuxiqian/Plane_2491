// mywidget.h
// Created by 法好 on 2018/7/4.
//

#ifndef MYWIDGET_H
#define MYWIDGET_H

#include "game.h"
#include "qcompleter.h"
#include <qdatastream.h>
#include <qlineedit.h>
#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

class myWidget : public QWidget {
    Q_OBJECT
  public:
    explicit myWidget(QWidget *parent = nullptr);
    // For Frame 0:
    QPushButton *startGame, *hallOfFame, *aboutGameBtn, *exitGame;
    QLabel *instructionLabel;
    QLineEdit *commandLine;
    // For Frame 1:
    QLabel *scoreLabel, *bloodLabel, *pauseFrameLabel;
    QPushButton *pauseAndResume, *saveGameState;
    QMediaPlayer *bgMediaPlayer, *effectMediaPlayer;
    QMediaPlaylist *playlist;
    QCompleter *compCommand;
    // QPushButton *updateLog, *placeEnemyButton, *onFire;
    // the very first 3 gaming buttons...
    Game game;
    QTimer *qtime;
    std::vector<QObject *> bufObjPtr;

  protected:
    void initializeComponent();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void readBytes(QDataStream &, int);
  signals:

  public slots:
    void updateGUI();
    void drawObject();

    // game related slots
    void shootBullet();
    void moveX(bool);
    void moveY(bool);
    void planeDodge();
    void placeEnemy();

    // media effect slots
    void playMediaEffect(int);

    // some button events
    void start();
    void highScore();
    void aboutGame();
    void pausePush();
    void loadButtonPush();
    void saveButtonPush();
    void showConsole();
    void runCommand();
    void goHome();
};

#endif // MYWIDGET_H
