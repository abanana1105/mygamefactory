#ifndef CUT_H
#define CUT_H


#include <QGraphicsPixmapItem>
#include <QPointF>

class Cut: public QGraphicsPixmapItem {
public:
    enum Direction {
        Up_left, Down_left, Left_up, Right_up,
        Up_Right, Down_Right, Left_Down, Right_Down
    };

    Cut(Direction direction, const QPointF &position);

    Direction getDirection() const;

private:
    Direction direction;
};
#endif // CUT_H
