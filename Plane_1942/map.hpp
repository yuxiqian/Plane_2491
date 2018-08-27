#ifndef MAP_HPP
#define MAP_HPP
#define MAP_MOVE_SPEED 1
class Map : public Object {
  public:
    Map(int x, int y) {
        //           pngPath = ":/res/background/bg.png";
        resIndex = 0;
        hitBoxLeft = 0;
        hitBoxTop = 0;
        hitBoxWidth = 800;
        hitBoxHeight = 2000;
        Yvelocity = MAP_MOVE_SPEED;
        objectType = 0;
        _x = x;
        _y = y;
    }
};
#endif // MAP_HPP
