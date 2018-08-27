// helper.cpp
// Created by 法好 on 2018/7/4.
//

#include <vector>
#include <random>
#include <ctime>
#include <QFile>
#include <QFontDatabase>
#include <QIODevice>
#include <QDataStream>
#include <exception>

class DataError : public std::exception {};

void randomize() { srand((int) time(NULL)); }

int getRandom(int minVal, int maxVal) {
    return random() % (maxVal - minVal + 1) + minVal;
}

double getPossibility() { return static_cast<double>(random()) / RAND_MAX; }

bool judgeHitBox(const Object &objA, const Object &objB) {
    if (objA._x + objA.hitBoxLeft >
            objB._x + objB.hitBoxLeft + objB.hitBoxWidth ||
        objB._x + objB.hitBoxLeft >
            objA._x + objA.hitBoxLeft + objA.hitBoxWidth ||
        objA._y + objA.hitBoxTop >
            objB._y + objB.hitBoxTop + objB.hitBoxHeight ||
        objB._y + objB.hitBoxTop >
            objA._y + objA.hitBoxTop + objA.hitBoxHeight) {
        return false;
    } else {
        return true;
    }
}

void deaccelerate(double &velocity) {
    if (velocity > MOVE_DEACC) {
        velocity -= MOVE_DEACC;
    } else if (velocity < -MOVE_DEACC) {
        velocity += MOVE_DEACC;
    } else {
        velocity = 0;
    }
}

QString copyrightInfo() {
    QString copyRightLines;
    copyRightLines.append(
        "「Solstice」music used here.\n"
        "Copyright © 2015 K-391, in Album 「Solstice」.\n"
        "\n"
        "「Avenir Next」 font used here.\n"
        "Copyright © 2014, 2015 Adobe Systems Incorporated "
        "(http://www.adobe.com/), "
        "with Reserved Font Name 'Source'.\n"
        "Source is a trademark of Adobe Systems Incorporated in the United "
        "States and/or other countries.\n"
        "Dr. Ken Lunde (project architect, glyph set definition & overall "
        "production);\n"
        "Masataka HATTORI 服部正貴 (production & ideograph elements)\n"
        "This Font Software is licensed under the SIL Open Font License, "
        "Version 1.1.\n"
        "This Font Software is distributed on an \"AS IS\" BASIS, WITHOUT "
        "WARRANTIES OR CONDITIONS OF ANY KIND,"
        "either express or implied. See the SIL Open Font License for the "
        "specific language, "
        "permissions and limitations governing your use of this Font "
        "Software.\n"
        "\n");
    copyRightLines.append(QString("「Game Plane_2491」 version %1 %2")
                              .arg(APPLICATION_VERSION, OS_TYPE));
    copyRightLines.append("\nCopyright © 2018 yuxiqian. All rights reserved.\n"
                          "\nBased on 「Qt」 5.11.1\n"
                          "Copyright © 2008-2018 The Qt Company Ltd.\nAll "
                          "rights reserved.");
    return copyRightLines;
}

QString instructionString() {
    QString instStr;
    instStr.append("<W> / <↑>\t Move upwards\n"
                   "<S> / <↓>\t Move downwards\n"
                   "<A> / <←>\t Move leftwards\n"
                   "<D> / <→>\t Move rightwards\n"
                   "<X>\t\t Dodge\n"
                   "<V>\t\t Clear enemy bullet\n"
                   "<Space>\t\t Shoot bullet\n"
                   "<F>\t\t Place assist plane\n"
                   "Hold <R>\t\t Activate Serial Bullet Mode\n"
                   "<C> / </>\t\t Open command window");
    return instStr;
}

QString loadFont() {
    // Avenir Next font used here.

    // Copyright © 2014, 2015 Adobe Systems Incorporated
    // (http://www.adobe.com/), with Reserved Font Name 'Source'. Source is a
    // trademark of Adobe Systems Incorporated in the United States and/or other
    // countries. Dr. Ken Lunde (project architect, glyph set definition &
    // overall production); Masataka HATTORI 服部正貴 (production & ideograph
    // elements) This Font Software is licensed under the SIL Open Font License,
    // Version 1.1. This Font Software is distributed on an "AS IS" BASIS,
    // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    // See the SIL Open Font License for the specific language, permissions and
    // limitations governing your use of this Font Software.

    QString fontPath = ":/res/font/avenir_next.ttc";
    QFile fontFile(fontPath);
    if (!fontFile.open(QIODevice::ReadOnly)) {
        // qDebug() << "Open font file error";
        return fontPath;
    }
    int loadedFontID =
        QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    QStringList loadedFontFamilies =
        QFontDatabase::applicationFontFamilies(loadedFontID);
    if (!loadedFontFamilies.empty()) {
        fontPath = loadedFontFamilies.at(0);
    }
    fontFile.close();
    return fontPath;
}
