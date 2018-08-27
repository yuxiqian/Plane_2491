#ifndef EFFECT_HPP
#define EFFECT_HPP

class Explosion : public Object {
  public:
    Explosion(int x, int y) {
        resIndex = 8;
        objectType = 51;
        hitBoxLeft = 0;
        hitBoxTop = 0;
        hitBoxWidth = 100;
        hitBoxHeight = 100;
        objectType = 51;
        _x = x;
        _y = y;
    }

    bool updateExplosionFrame() {
        if (this->resIndex < 10) {
            ++this->objectType;
            ++this->resIndex;
            return false;
        } else {
            return true;
        }
    }
};
#endif // EFFECT_HPP
