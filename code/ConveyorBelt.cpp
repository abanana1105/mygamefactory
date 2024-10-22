#include "ConveyorBelt.h"
#include "GameView.h"
#include <QPixmap>

ConveyorBelt::ConveyorBelt(Direction direction, const QPointF &position)
    : direction(direction) {
    setPos(position);
    // 根据方向设置不同的图像
    QString imagePath = ":/resourses/images/";
    switch (direction) {
    case trans_down_left: imagePath += "trans_down_left"; break;
    case trans_down_right: imagePath += "trans_down_right"; break;
    case trans_down_up: imagePath += "trans_down_up"; break;

    case trans_up_left: imagePath += "trans_up_left"; break;
    case trans_up_right: imagePath += "trans_up_right"; break;
    case trans_up_down: imagePath += "trans_up_down"; break;

    case trans_left_up: imagePath += "trans_left_up"; break;
    case trans_left_down: imagePath += "trans_left_down"; break;
    case trans_left_right: imagePath += "trans_left_right"; break;

    case trans_right_up: imagePath += "trans_right_up"; break;
    case trans_right_down: imagePath += "trans_right_down"; break;
    case trans_right_left: imagePath += "trans_right_left"; break;
    default: break;
    }
    imagePath += ".png";
    setPixmap(QPixmap(imagePath));
}

ConveyorBelt::Direction ConveyorBelt::getDirection() const {
    return direction;
}



