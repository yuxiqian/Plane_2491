#ifndef PLANE_HPP
#define PLANE_HPP

#define PLAYER_PLANE_NORMAL 0
#define PLAYER_ASSIST_PLANE 1
#define ENEMY_PLANE_A 2
#define ENEMY_PLANE_B 3
#define ENEMY_PLANE_C 4

class Plane : public Object {
  public:
    Plane(int planeType, int x, int y) {
        if (planeType == PLAYER_PLANE_NORMAL) {
            //            pngPath = ":/res/aircraft/player_plane_normal.png";
            resIndex = 1;
            hitBoxLeft = 16;
            hitBoxTop = 0;
            hitBoxWidth = 69;
            hitBoxHeight = 97;
            objectType = 1;
            blood = 100;
        } else if (planeType == PLAYER_ASSIST_PLANE) {
            //            pngPath = ":/res/aircraft/player_assist_plane.png";
            resIndex = 2;
            hitBoxLeft = 22;
            hitBoxTop = 13;
            hitBoxWidth = 54;
            hitBoxHeight = 74;
            objectType = 6;
        } else if (planeType == ENEMY_PLANE_A) {
            //            pngPath = ":/res/aircraft/enemy_plane_a.png";
            resIndex = 3;
            hitBoxLeft = 16;
            hitBoxTop = 7;
            hitBoxWidth = 66;
            hitBoxHeight = 88;
            objectType = 2;
            blood = 30;
            shootPossibility = 0.08;
            Yvelocity = 6;
        } else if (planeType == ENEMY_PLANE_B) {
            //            pngPath = ":/res/aircraft/enemy_plane_b.png";
            resIndex = 4;
            hitBoxLeft = 28;
            hitBoxTop = 22;
            hitBoxWidth = 42;
            hitBoxHeight = 57;
            objectType = 2;
            blood = 10;
            shootPossibility = 0.02;
            Yvelocity = 8;
        } else if (planeType == ENEMY_PLANE_C) {
            //            pngPath = ":/res/aircraft/enemy_plane_c.png";
            resIndex = 5;
            hitBoxLeft = 28;
            hitBoxTop = 22;
            hitBoxWidth = 42;
            hitBoxHeight = 57;
            objectType = 2;
            blood = 10;
            shootPossibility = 0.01;
            Yvelocity = 10;
        }
        _x = x;
        _y = y;
    }
    int blood;
    double shootPossibility;
    void setBulletRainSkin() { this->resIndex = 13; }
    void setDefaultSkin() { this->resIndex = 1; }
    bool isBossFlag() { return this->resIndex == 3; }
};
#endif // PLANE_HPP
