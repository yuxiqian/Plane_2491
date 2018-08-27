#ifndef BULLET_HPP
#define BULLET_HPP

#define BULLET_MOVEMENT_SPEED 20

#define PLAYER_BULLET 0
#define ENEMY_BULLET 1
class Bullet : public Object {
  public:
    Bullet(int bulletType, int x, int y, double xv = 0,
           double yv = BULLET_MOVEMENT_SPEED) {
        if (bulletType == ENEMY_BULLET) {
            //             pngPath = ":/res/effects/enemy_bullet_fire.png";
            resIndex = 6;
            hitBoxLeft = 45;
            hitBoxTop = 17;
            hitBoxWidth = 12;
            hitBoxHeight = 69;
            objectType = 4;
            this->Yvelocity = BULLET_MOVEMENT_SPEED;
            this->Xvelocity = 0.0;
        } else if (bulletType == PLAYER_BULLET) {
            //             pngPath = ":/res/effects/player_bullet_fire.png";
            resIndex = 7;
            hitBoxLeft = 47;
            hitBoxTop = 16;
            hitBoxWidth = 11;
            hitBoxHeight = 70;
            objectType = 3;
            this->Yvelocity = yv;
            this->Xvelocity = xv;
        }
        _x = x;
        _y = y;
    }
};
#endif // BULLET_HPP
