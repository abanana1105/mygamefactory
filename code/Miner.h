#ifndef MINER_H
#define MINER_H

#include <QGraphicsPixmapItem>
#include <QPointF>

class Miner : public QGraphicsPixmapItem {
public:
    enum Direction {
        Up, Down, Left, Right
    };

    Miner(Direction direction, const QPointF &position);

    Direction getDirection() const;

private:
    Direction direction;
};

#endif // MINER_H
