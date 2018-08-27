// mywidget.cpp
// Created by 法好 on 2018/7/4.
//

#include "game.cpp"
#include "mywidget.h"
#include <QLineEdit>
#include <QApplication>
#include <QWidget>
#include <QColor>
#include <QMessageBox>
#include <QEvent>
#include <QCompleter>
#include <QFocusEvent>
#include <QScreen>
#include <QKeyEvent>
#include <QLabel>
#include <QTimer>
#include <QObject>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QPixmap>
#include <QPushButton>
#include <QDebug>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QFile>
#include <QStyleFactory>
//#include <QQuickStyle>

myWidget::myWidget(QWidget *parent)
    : QWidget(parent) {
    // initialize objects
    scoreLabel = new QLabel(this);
    bloodLabel = new QLabel(this);
    pauseAndResume = new QPushButton(this);
    saveGameState = new QPushButton(this);
    pauseFrameLabel = new QLabel(this);

    instructionLabel = new QLabel(this);
    instructionLabel->setAlignment(Qt::AlignLeft);
    instructionLabel->setGeometry(250, 400, 500, 200);
    instructionLabel->setFont(QFont(loadFont(), 13 * DPI_SCALE_FIX));
    instructionLabel->setText(instructionString());
    pauseFrameLabel->setGeometry(0, 0, 800, 600);
    saveGameState->setText("Save current game data");
    saveGameState->setFont(
        QFont(loadFont(), 13 * DPI_SCALE_FIX, QFont::Normal));
    saveGameState->setGeometry(310, 45, 180, 25);

    QPixmap img;
    img.load(QString::fromStdString(":/res/gui/pause_frame_simple.png"));
    pauseFrameLabel->setPixmap(img);
    pauseFrameLabel->hide();
    pauseFrameLabel->stackUnder(this->pauseAndResume);
    effectMediaPlayer = new QMediaPlayer(this);

    playlist = new QMediaPlaylist;
    playlist->addMedia(QUrl("qrc:/res/media/solstice.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    playlist->setCurrentIndex(1);

    bgMediaPlayer = new QMediaPlayer(this);

    bgMediaPlayer->setPlaylist(playlist);
    bgMediaPlayer->setVolume(50);
    bgMediaPlayer->play();
    // initialize background music

    qtime = new QTimer(this);

    commandLine = new QLineEdit(this);
    commandLine->setMaxLength(20);
    QStringList word_list;
    word_list << "/suicide"
              << "/money"
              << "/stats"
              << "/screenshot"
              << "/heal"
              << "/help";
    compCommand = new QCompleter(word_list, this);
    compCommand->setFilterMode(Qt::MatchContains);
    compCommand->setCaseSensitivity(Qt::CaseInsensitive);
    commandLine->setCompleter(compCommand);
    commandLine->setStyleSheet("background: transparent; color: white");
    //    commandLine->setGeometry(0, 700, 800, 100);
    commandLine->setFont(QFont(loadFont(), 12 * DPI_SCALE_FIX));
    commandLine->setText("");
    commandLine->setAlignment(Qt::AlignBottom);
    commandLine->hide();

    this->grabKeyboard();
    this->grabGesture(Qt::SwipeGesture);
    scoreLabel->setGeometry(20, 20, 400, 100);
    bloodLabel->setGeometry(680, 20, 100, 100);
    pauseAndResume->setGeometry(380, 20, 40, 30);
    scoreLabel->setAlignment(Qt::AlignLeft);
    bloodLabel->setAlignment(Qt::AlignRight);
    // register GUI control

    pauseAndResume->setFont(QFont(loadFont(), 20 * DPI_SCALE_FIX, 0, false));
    pauseAndResume->setText("| |");
    pauseAndResume->hide();
    saveGameState->hide();

    scoreLabel->setText("SCORE_HERE");
    bloodLabel->setText("BLOOD_HERE");

    qtime->setInterval(TICK_TIME_MS);
    qtime->start();

    connect(pauseAndResume, &QPushButton::clicked, this, &myWidget::pausePush);
    connect(saveGameState, &QPushButton::clicked, this,
            &myWidget::saveButtonPush);
    connect(qtime, &QTimer::timeout, this, &myWidget::updateGUI);
    connect(qtime, &QTimer::timeout, this, &myWidget::drawObject);
    // connect signals and slots
}

void myWidget::keyPressEvent(QKeyEvent *event) {
    if (this->game.gameData.getGameFlag() == 1 && !this->game.isPause) {
        if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) {
            moveX(true);
        } else if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) {
            moveX(false);
        } else if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up) {
            moveY(true);
        } else if (event->key() == Qt::Key_S || event->key() == Qt::Key_Down) {
            moveY(false);
        } else if (event->key() == Qt::Key_R) {
            this->game.startBulletRain();
        }
        // for each pressing event
        // do relative movements
    }
    QWidget::keyPressEvent(event);
}

void myWidget::keyReleaseEvent(QKeyEvent *event) {
    if (this->game.gameData.getGameFlag() == 1 && !this->game.isPause) {
        if (event->key() == Qt::Key_Space) {
            if (this->game.gameData.Score() >= BULLET_COST) {
                // judge the score validation
                shootBullet();
                playMediaEffect(SHOOT_BULLET);
                this->game.gameData.Score(-BULLET_COST);
            }
        } else if (event->key() == Qt::Key_F) {
            this->game.generateAssist();
        } else if (event->key() == Qt::Key_R) {
            this->game.stopBulletRain();
        } else if (event->key() == Qt::Key_C || event->key() == Qt::Key_Slash) {
            this->showConsole();
        } else if (event->key() == Qt::Key_X) {
            this->planeDodge();
        } else if (event->key() == Qt::Key_V) {
            this->game.clearBomb();
        }
    }
    QWidget::keyReleaseEvent(event);
}

void myWidget::focusOutEvent(QFocusEvent *event) {
    if (this->game.gameData.getGameFlag() == 1 && !this->game.isPause) {
        this->pausePush();
        // when the window lost its focus
        // it will pause automatically
    }
    QWidget::focusOutEvent(event);
}

void myWidget::readBytes(QDataStream &in, int i) {
    static unsigned objSize[8] = {
        sizeof(Map),    sizeof(Plane),     sizeof(Plane), sizeof(Bullet),
        sizeof(Bullet), sizeof(Explosion), sizeof(Plane), sizeof(Item)};
    if (i < 0 || i > 7) {
        DataError dtErr;
        throw dtErr;
        return;
    }
    //            qDebug() << "Attempted to read" << objSize[i] << "bits";
    char *tmpObj = (char *) calloc(1, objSize[i]);
    //        qDebug() << "Attempted to read" << objSize[i] << "bits";
    in.readBytes(tmpObj, objSize[i]);
    //    qDebug() << "Gotta" << objSize[i] << "bits";
    this->game.putObject(i, (void *) tmpObj);
}

void myWidget::updateGUI() {
    if (this->game.gameData.getGameFlag() == 1) {
        // if it's the game playing mode

        this->scoreLabel->setText(
            QString("%1\nSCORE").arg(this->game.gameData.Score()));
        this->bloodLabel->setText(
            QString("%1\nBLOOD").arg(this->game.gameData.Blood()));
        // update GUI display

        if (this->game.gameData.Score() >= BULLET_COST) {
            this->scoreLabel->setStyleSheet("color: white");
        } else {
            this->scoreLabel->setStyleSheet("color: red");
        }
        if (this->game.gameData.Blood() > LOW_BLOOD_WARNING) {
            this->bloodLabel->setStyleSheet("color: white");
        } else {
            this->bloodLabel->setStyleSheet("color: red");
        }
        // change font color to red as warnings

        if (getPossibility() < ENEMY_GENERATE_POSSIBILITY) {
            this->game.generateEnemy();
        }
        if (getPossibility() < PRESENT_POSSIBILITY) {
            this->game.generatePresent();
        }
        // randomly generate enemies and presents

    } else {
        // if it's not the game playing mode
        this->scoreLabel->setText("");
        this->bloodLabel->setText("");
        // clear the GUI display
    }
}

void myWidget::drawObject() {
    // clean the screen, release memory
    for (auto &i : bufObjPtr) {
        delete i;
    }
    bufObjPtr.clear();
    switch (this->game.gameData.getGameFlag()) {
    case 0: {
        pauseAndResume->hide();
        qtime->stop();
        this->game.gameData.readHighScore();
        QLabel *titleLabel = new QLabel(this);
        titleLabel->setText("THIS IS A FAKE 2491");
        titleLabel->setFont(QFont(loadFont(), 54 * DPI_SCALE_FIX, -1, true));
        titleLabel->setGeometry(0, 50, 800, 60);
        titleLabel->setAlignment(Qt::AlignCenter);
        instructionLabel->show();
        titleLabel->show();
        bufObjPtr.push_back(titleLabel);
        // generate title label

        QPushButton *startGame = new QPushButton(this);
        startGame->setText("Start Game");
        startGame->setGeometry(200, 150, 400, 40);
        connect(startGame, &QPushButton::clicked, this, &myWidget::start);
        connect(commandLine, SIGNAL(returnPressed()), this, SLOT(runCommand()));
        startGame->show();
        bufObjPtr.push_back(startGame);
        // start game button

        QPushButton *loadState = new QPushButton(this);
        loadState->setText("Load Game");
        loadState->setGeometry(200, 200, 400, 40);
        connect(loadState, &QPushButton::clicked, this,
                &myWidget::loadButtonPush);
        loadState->show();
        bufObjPtr.push_back(loadState);
        // start game button

        QPushButton *highScore = new QPushButton(this);
        highScore->setText("High Score");
        highScore->setGeometry(200, 250, 400, 40);
        highScore->show();
        bufObjPtr.push_back(highScore);
        connect(highScore, &QPushButton::clicked, this, &myWidget::highScore);
        // high score button

        QPushButton *aboutGameBtn = new QPushButton(this);
        aboutGameBtn->setText("About Game");
        aboutGameBtn->setGeometry(200, 300, 400, 40);
        aboutGameBtn->show();
        bufObjPtr.push_back(aboutGameBtn);
        connect(aboutGameBtn, &QPushButton::clicked, this,
                &myWidget::aboutGame);
        // about game button

        QPushButton *exitGame = new QPushButton(this);
        exitGame->setText("Exit Game");
        exitGame->setGeometry(200, 350, 400, 40);
        exitGame->show();
        bufObjPtr.push_back(exitGame);
        connect(exitGame, &QPushButton::clicked, this, &myWidget::close);
        // exit game button
    } break;
    case 2:
        instructionLabel->hide();
        pauseAndResume->hide();
        // hide irrelevant UI controls
        {
            game.clearElement();
            QMessageBox *qm = new QMessageBox(this);
            qm->setWindowTitle("2491");
            if (this->game.gameData.Score() > this->game.gameData.highScore) {
                qm->setText(QString("New Highscore!\nYou gained %1 scores.")
                                .arg(this->game.gameData.Score()));
                QFile file("highscore.dat");
                file.open(QIODevice::WriteOnly);
                QDataStream out(&file);
                out << this->game.gameData.Score();
                file.close();
                // judge the new highscore and write it into the hard drive
            } else {
                qm->setText(QString("Game Over.\nYou gained %1 scores.")
                                .arg(this->game.gameData.Score()));
            }
            qm->show();
            bufObjPtr.push_back(qm);

            QPushButton *restart = new QPushButton(this);
            restart->setText("Replay");
            restart->setGeometry(200, 200, 400, 50);
            connect(restart, &QPushButton::clicked, this, &myWidget::goHome);
            restart->show();
            bufObjPtr.push_back(restart);
            // replay button here

            QPushButton *exitGame = new QPushButton(this);
            exitGame->setText("Exit Game");
            exitGame->setGeometry(200, 270, 400, 50);
            exitGame->show();
            bufObjPtr.push_back(exitGame);
            connect(exitGame, &QPushButton::clicked, this, &myWidget::close);
            // exit game button here
        }
        qtime->stop();
        break;
    case 1: {
        instructionLabel->hide();
        pauseAndResume->show();
        // hide irrelevant GUI controls
        if (this->game.isPause) {
            break;
        }
        //          int index = 0;
        for (int index = 0; index < 8; ++index) {
            //                auto i = game[index];
            int size = this->game.getObjectCount(index);
            // search the gameElement vector
            //                qDebug() << QString("SEE VECTOR[%1]").arg(index);
            for (int i = 0; i < size; ++i) {
                auto j = this->game.getObject(index, i);
                QPixmap img = j->getPixmap();
                QLabel *pic = new QLabel(this);
                pic->stackUnder(this->scoreLabel);
                //                     qDebug() << "\tGotta an object";
                //                     qDebug() << "\tREAD PICTURE AT" <<
                //                     QString::fromStdString(j->pngPath);
                pic->clear();
                if (index == 1 || index == 2 || index == 3 || index == 6) {
                    pic->setGeometry(j->_x, j->_y, RES_PIXELS, RES_PIXELS);
                    QMatrix rotMat;
                    rotMat.rotate(int(j->Xvelocity / 2));
                    //                       qDebug() << "\tROTATE PICTURE" <<
                    //                       int(j.Xvelocity) ;
                    pic->setPixmap(
                        img.transformed(rotMat, Qt::SmoothTransformation));
                    // use matrix to rotate the element, when it's a plane
                } else if (index == 0) {
                    pic->setGeometry(j->_x, j->_y, FRAME_WIDTH, MAP_HEIGHT);
                    pic->setPixmap(img);
                } else {
                    pic->setGeometry(j->_x, j->_y, RES_PIXELS, RES_PIXELS);
                    pic->setPixmap(img);
                }
                //                     qDebug() << "\t有一个Obj";
                //                    qDebug() << "\tPLACE PICTURE AT x: " <<
                //                    j->_x << "y: " << j->_y;
                pic->show();
                bufObjPtr.push_back((QObject *) pic);
            }
        }
        this->playMediaEffect(this->game.updateFrame());
        // call game to update its situation
    } break;
    }
}

void myWidget::shootBullet() { this->game.shootBullet(); }

void myWidget::moveX(bool isLeft) {
    // isLeft = true: go leftside
    // isLeft = false: go rightside
    if (abs(this->game.getPlayerPlane().Xvelocity) <= MAX_SPEED) {
        if (isLeft) {
            this->game.getPlayerPlane().Xvelocity -= MOVE_ACC;
        } else {
            this->game.getPlayerPlane().Xvelocity += MOVE_ACC;
        }
    }
}

void myWidget::moveY(bool isUp) {
    // isUp = true: go upside
    // isUp = false: go downside
    if (abs(this->game.getPlayerPlane().Yvelocity) <= MAX_SPEED) {
        if (isUp) {
            this->game.getPlayerPlane().Yvelocity -= MOVE_ACC;
        } else {
            this->game.getPlayerPlane().Yvelocity += MOVE_ACC;
        }
    }
}

void myWidget::planeDodge() {
    this->game.getPlayerPlane().Xvelocity *= 2;
    if (this->game.getPlayerPlane().Xvelocity == 0) {
        if (getRandom(1, 2) == 1) {
            this->game.getPlayerPlane().Xvelocity += 10;
        } else {
            this->game.getPlayerPlane().Xvelocity -= 10;
        }
    }
    this->game.getPlayerPlane()._x += this->game.getPlayerPlane().Xvelocity;
}

void myWidget::placeEnemy() {
    this->game.generateEnemy();
    // operate between GUI and the game logic
}

void myWidget::playMediaEffect(int mediaType) {
    // from the parameter's value to play different media effects
    effectMediaPlayer->setVolume(30);
    switch (mediaType) {
    case PLANE_GOTHIT:
        effectMediaPlayer->setMedia(QUrl("qrc:/res/media/plane_hit.mp3"));
        break;
    case SHOOT_BULLET:
        effectMediaPlayer->setMedia(QUrl("qrc:/res/media/shoot_bullet.mp3"));
        break;
    case PLANE_EXPLOSION:
        effectMediaPlayer->setMedia(QUrl("qrc:/res/media/plane_explosion.mp3"));
        break;
    case GOT_GIFT:
        effectMediaPlayer->setMedia(QUrl("qrc:/res/media/gift_get.mp3"));
        break;
    default:
        return;
    }
    effectMediaPlayer->play();
}

void myWidget::start() {
    // when the game should start:

    this->qtime->start();
    // start the time interval

    GameData newData;
    this->game.gameData = newData;
    this->game.gameData.setGameFlag(1);
    // create a clean gamedata object and set its gameflag as 1: playing

    this->game.startGame();
    // call game.startGame() method
}

void myWidget::highScore() {
    QMessageBox *qm = new QMessageBox(this);
    this->game.gameData.readHighScore();
    // read high score from local files
    qm->setText(QString("-- GAME 2491 --\n\nHighest score: %1")
                    .arg(this->game.gameData.highScore));
    qm->show();
    bufObjPtr.push_back(qm);
    // push qm* into the buffer object pointers vector
    // and release them when needed
}

void myWidget::aboutGame() {
    QMessageBox *qMsgAbout = new QMessageBox(this);
    qMsgAbout->setText(copyrightInfo());
    qMsgAbout->setFont(QFont(loadFont(), 12, QFont::Bold));
    qMsgAbout->show();
    bufObjPtr.push_back(qMsgAbout);
    // push qMsgAbout* into the buffer object pointers vector
    // and release them when needed
}

void myWidget::pausePush() {
    // switch state from pause and play
    if (this->game.isPause) {
        this->pauseFrameLabel->hide();
        this->game.stopBulletRain();
        this->game.isPause = false;
        this->pauseAndResume->setText("| |");
        this->qtime->start();
        this->saveGameState->hide();
        this->commandLine->hide();
        this->commandLine->clear();
        this->grabKeyboard();
    } else {
        this->pauseFrameLabel->show();
        this->game.stopBulletRain();
        this->game.isPause = true;
        this->pauseAndResume->setText("►");
        this->qtime->stop();
        this->saveGameState->show();
    }
}

void myWidget::loadButtonPush() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "Pick a saved game data", "", tr("Game Data files (*.dat)"));
    if (!fileName.isNull()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open file."),
                                 QMessageBox::Ok);
            return;
        } else {
            this->game.clearElement();
            this->game.gameData.resetData();
            try {
                QDataStream in(&file);
                {
                    //                    if (!in.Ok) {
                    //                        StreamError strErr;
                    //                        throw strErr;
                    //                    }
                    int score, blood;
                    in >> score >> blood;
                    this->game.gameData.Score(score);
                    this->game.gameData.Blood(blood);
                    qDebug() << "Read in" << score << blood;
                    if (blood == 0) {
                        DataError dtErr;
                        throw dtErr;
                    }
                }
                int vectorSize[8];
                for (int i = 0; i < 8; ++i) {
                    in >> vectorSize[i];
                    //                    qDebug() << "Read in" <<
                    //                    vectorSize[i];
                }
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < vectorSize[i]; ++j) {
                        //                        qDebug() << i << "type :
                        //                        count" << vectorSize[i];
                        this->readBytes(in, i);
                    }
                }
            } catch (...) {
                QMessageBox::warning(this, tr("Error"),
                                     tr("Failed to resolve data."),
                                     QMessageBox::Ok);
                this->game.clearElement();
                this->game.gameData.resetData();
                return;
            }
            file.close();
            this->game.gameData.setGameFlag(1);
            this->qtime->start();
        }
    }
}

void myWidget::saveButtonPush() {
    int objSize[8] = {sizeof(Map),    sizeof(Plane),  sizeof(Plane),
                      sizeof(Bullet), sizeof(Bullet), sizeof(Explosion),
                      sizeof(Plane),  sizeof(Item)};
    QString fileName =
        QFileDialog::getSaveFileName(this, "Pick a path to save game data", "",
                                     tr("Game Data files (*.dat)"));
    if (!fileName.isNull()) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open file."),
                                 QMessageBox::Ok);
            return;
        } else {
            QDataStream out(&file);
            out << this->game.gameData.Score() << this->game.gameData.Blood();
            int vectorSize[8];
            for (int i = 0; i < 8; ++i) {
                vectorSize[i] = this->game.getObjectCount(i);
                out << vectorSize[i];
                //                qDebug() << "Save out" << vectorSize[i];
            }
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < vectorSize[i]; ++j) {
                    out.writeBytes((char *) this->game.getObject(i, j),
                                   objSize[i]);
                    //                    qDebug() << "Write" << objSize[i] <<
                    //                    "bits of type" << i;
                }
            }
            file.close();
            QMessageBox *qm = new QMessageBox(this);
            qm->setText(QString("Successfully saved to %1.").arg(fileName));
            qm->setFont(QFont(loadFont(), 14));
            qm->show();
            bufObjPtr.push_back(qm);
            //            this->game.clearElement();
            //            this->game.gameData.setGameFlag(0);
            //            this->pausePush();
        }
    }
}

void myWidget::showConsole() {
    if (this->game.gameData.getGameFlag() != 1) {
        return;
    }
    if (!this->game.isPause) {
        this->pausePush();
    }
    this->releaseKeyboard();
    //    bool isOK = false;
    //    QInputDialog qID(this);
    //    qID.setModal(true);
    commandLine->setText("/");
    commandLine->show();
    commandLine->setFocus();
}

void myWidget::runCommand() {
    QString command = this->commandLine->text();
    //    QString command = qID.getText(this, "You're in Plane 2491 Console",
    //    "Input one piece of command and press OK", QLineEdit::Normal,
    //    QString::null, &isOK);
    if (!command.isEmpty()) {
        command = command.toLower();
        if (command == "/suicide") {
            this->game.gameData.Blood(-100);
            this->pausePush();
        } else if (command == "/money") {
            this->game.gameData.Score(141592653);
            this->pausePush();
        } else if (command == "/stats") {
            QMessageBox *qm = new QMessageBox(this);
            qm->setFont(QFont(loadFont(), 15 * DPI_SCALE_FIX));
            qm->setText(this->game.showStats());
            qm->show();
            bufObjPtr.push_back(qm);
        } else if (command == "/screenshot") {
            this->pauseAndResume->hide();
            this->pauseFrameLabel->hide();
            this->saveGameState->hide();
            this->commandLine->hide();
            QPixmap p = this->grab(QRect(0, 0, FRAME_WIDTH, FRAME_HEIGHT));
            QString fileName = QFileDialog::getSaveFileName(
                this, "Pick a path to save game screenshot", "",
                tr("PNG Files (*.png)"));
            if (!fileName.isNull()) {
                p.save(fileName);
            }
            this->pausePush();
        } else if (command == "/heal") {
            this->game.gameData.Blood(100);
            this->pausePush();
        } else if (command == "/help") {
            QMessageBox *qm = new QMessageBox(this);
            qm->setFont(QFont(loadFont(), 13 * DPI_SCALE_FIX));
            qm->setText(instructionString());
            qm->show();
            bufObjPtr.push_back(qm);
        }
        else {
            this->pausePush();
        }
    } else {
        this->pausePush();
    }
    this->commandLine->clear();
    this->commandLine->hide();
    this->grabKeyboard();
}

void myWidget::goHome() {
    this->game.gameData.setGameFlag(0);
    // reset gameFlag

    this->qtime->start();
    // just start it once at this frame
    // and it will stop() automatically
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setStyle(QStyleFactory::create("fusion"));
    QFont globalFont(loadFont(), 18 * DPI_SCALE_FIX);
    QPixmap ico;
    //    QQuickStyle::setStyle("Material");
    app.setFont(globalFont);
    ico.load(":/res/icon/icon.png");
    app.setApplicationDisplayName("2491 Game");
    app.setApplicationName("2491 Game");
    app.setApplicationVersion(APPLICATION_VERSION);
    app.setWindowIcon(QIcon(ico));

    myWidget w;
    w.setWindowTitle("2491 Game");
    w.setWindowIcon(QIcon(ico));
    w.setFixedWidth(FRAME_WIDTH);
    w.setFixedHeight(FRAME_HEIGHT);
    w.show();
    // initialize widget
    return app.exec();
    // start mainloop
}
