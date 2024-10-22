#include "Miner.h"
#include <QPixmap>

Miner::Miner(Direction direction, const QPointF &position)    : direction(direction) {
    setPos(position);
    // 根据方向设置不同的图像
    QString imagePath = ":/resourses/images/";
    switch (direction) {
    case Up: imagePath += "mineUp"; break;
    case Down: imagePath += "mineDown"; break;
    case Left: imagePath += "mineLeft"; break;
    case Right: imagePath += "mineRight"; break;
    default: break;
    }
    imagePath += ".png";
    setPixmap(QPixmap(imagePath));
}

Miner::Direction Miner::getDirection() const {
    return direction;
}
