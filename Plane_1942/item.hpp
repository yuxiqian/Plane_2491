#ifndef ITEM_HPP
#define ITEM_HPP
#define GIFT_ITEM 0
#define HEAL_ITEM 1
class Item : public Object {
  public:
    Item(int itemType, int x, int y) {
        if (itemType == GIFT_ITEM) {
            //         pngPath = ":/res/gift/gray_gift.png";
            resIndex = 11;
            hitBoxLeft = 27;
            hitBoxTop = 27;
            hitBoxWidth = 46;
            hitBoxHeight = 46;
            Yvelocity = 9;
            objectType = 71;
            giftType = GIFT_ITEM;
        } else if (itemType == HEAL_ITEM) {
            //         pngPath = ":/res/gift/heal.png";
            resIndex = 12;
            hitBoxLeft = 28;
            hitBoxTop = 28;
            hitBoxWidth = 44;
            hitBoxHeight = 44;
            Yvelocity = 10;
            objectType = 72;
            giftType = HEAL_ITEM;
        }
        _x = x;
        _y = y;
    }
    int giftType;
};

#endif // ITEM_HPP
