#include "Cut.h"
#include <QPixmap>

Cut::Cut(Direction direction, const QPointF &position)    : direction(direction) {
    setPos(position);
    // 根据方向设置不同的图像
}

Cut::Direction Cut::getDirection() const {
    return direction;
}
