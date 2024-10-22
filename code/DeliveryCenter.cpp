#include "DeliveryCenter.h"
#include "global.h"

DeliveryCenter::DeliveryCenter(const QPixmap &pixmap, const QPointF &position, int size)
    : QGraphicsPixmapItem(pixmap), position(position), size(size) {
    setPos(position);
    // 根据需要调整 pixmap 的大小
}

QPointF DeliveryCenter::getPosition() const {
    return position;
}

void DeliveryCenter::setPosition(const QPointF &newPosition) {
    position = newPosition;
    setPos(position);
}

int DeliveryCenter::getSize() const {
    return size;
}

void DeliveryCenter::setSize(int newSize) {
    size = newSize;
    // 根据 newSize 调整交付中心的显示
}

void DeliveryCenter::createText() {
    setText(("现在的金币数量：%1\n需要情侣矿物: %2 / %3"), count, mineral2, 20);
}

void DeliveryCenter::setText(QString s , int a, int b, int c) {
    if(deliveryCenterText != nullptr) {
        delete deliveryCenterText;
    }
    deliveryCenterText = nullptr;
    deliveryCenterText = new QGraphicsTextItem();
    deliveryCenterText->setPlainText(s.arg(a).arg(b).arg(c));

    QFont font = deliveryCenterText->font();
    font.setPointSize(20);
    deliveryCenterText->setFont(font);

    deliveryCenterText->setPos(10000 - 140, 50 * 100 - 40);  // 设置合适的位置
    globalGameController->getMap()->getScene()->addItem(deliveryCenterText);
}

void DeliveryCenter::updateText() {
    if(step == 1) {
        setText(("现在的金币数量：%1\n需要情侣矿物: %2 / %3"), count, mineral2, 20);
        if(mineral2 >= 20) {
            step = 2;
            upcount++;
        }
    } else if(step == 2) {
        setText(("现在的金币数量：%1\n需要笑脸矿物: %2 / %3"), count, mineral1, 30);
        if(mineral1 >= 30) {
            step = 3;
            upcount++;
        }
    } else if(step == 3) {
        setText(("现在的金币数量：%1\n需要女孩矿物: %2 / %3"), count, mineral3, 50);
        if(mineral3 >= 50) {
            step = 4;
            upcount++;
        }
    }else {
        setText(("现在的金币数量：%1\n不需要任何矿物: %2 / %3"), count, 13, 14);
    }
}

void DeliveryCenter::receive(int type, int multiply) {
    if(type == 1) {
        mineral1 += multiply * 1;
        count += multiply * value1 * mul;
    } else if (type == 2){
        mineral2 += multiply * 1;
        count += multiply * value2 * mul;
    } else if (type == 3){
        mineral3 += multiply * 1;
        count += multiply * value3 * mul;
    } else if (type == 4){
        mineral4 += multiply * 1;
        count += multiply * value4 * mul;
    }
}

void DeliveryCenter::up_center() {
    if(upcenter == 0 && count >= 100) {
        QPixmap newPixmap(":/resourses/images/upcenter.png");
        QPointF newPosition(9700, 4700); // 更新位置
        GameTile *up;
        for(int x = 97; x <= 102; x++) {
            for(int y = 47; y <= 52; y++) {
                up = globalGameMap -> tiles[x][y];
                up -> setDustbin();
                up -> is_dustbin = 0;
                up -> setBrush(Qt::white);
                up -> is_center = true;
            }
        }
        size = 6;
        setPixmap(newPixmap);
        setPos(newPosition);
        count -= 100;
        upcenter = 1;
        globalGameController -> saveallGame();
        globalGameController -> saveGame();
    }
}

void DeliveryCenter::up_mineral() {
    if(upmineral == 0 && count >= 100) {
        GameTile *up;

        for(int x = 94; x <= 95; x++) {
            for(int y = 49; y <= 50; y++) {
                up = globalGameMap -> tiles[x][y];
                up -> setDustbin();
                up -> is_dustbin = 0;
                up -> setBrush(Qt::white);
                up -> setMineralType(1);
            }
        }
        count -= 100;
        upmineral = 1;
        globalGameController -> saveallGame();
        globalGameController -> saveGame();
    }
}

void DeliveryCenter::up_value() {
    if(upvalue == 0 && count >= 100) {
        mul ++;
        count -= 100;
        upvalue = 1;
        globalGameController -> saveallGame();
        globalGameController -> saveGame();
    }
}
