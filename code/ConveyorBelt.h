#ifndef CONVEYORBELT_H
#define CONVEYORBELT_H

#include <QGraphicsPixmapItem>
#include <QPointF>

class ConveyorBelt : public QGraphicsPixmapItem {
public:
    enum Direction {
        trans_down_left, trans_down_right, trans_down_up,
        trans_up_left, trans_up_right, trans_up_down,
        trans_left_up, trans_left_down, trans_left_right,
        trans_right_up, trans_right_down, trans_right_left
    };

    ConveyorBelt(Direction direction, const QPointF &position);

    Direction getDirection() const;

private:
    Direction direction;
};

#endif // CONVEYORBELT_H
