#include "Mineral.h"

Mineral::Mineral(int remaining, int value, int type)
    : remaining(remaining), value(value), type(type) {}

std::vector<QPointF> Mineral::miningCenters = {
    QPointF(10, 10), QPointF(40, 10), QPointF(70, 10), QPointF(100, 10), QPointF(130, 10),
    QPointF(10, 30), QPointF(40, 30), QPointF(70, 30), QPointF(100, 30), QPointF(130, 30),
    QPointF(10, 50), QPointF(40, 50), QPointF(70, 50), QPointF(100, 65), QPointF(130, 50),
    QPointF(10, 70), QPointF(40, 70), QPointF(70, 70), QPointF(100, 70), QPointF(130, 70)
};


int Mineral::getType() const {
    return type;
}

int Mineral::getRemaining() const {
    return remaining;
}

void Mineral::mine() {
    if (remaining > 0) {
        remaining--;
    }
}

int Mineral::getValue() const {
    return value;
}

const std::vector<QPointF>& Mineral::getMiningCenters() {
    return miningCenters;
}


