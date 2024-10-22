#ifndef ANIMATEDPIXMAPITEM_H
#define ANIMATEDPIXMAPITEM_H

#include "qgraphicsitem.h"
class AnimatedPixmapItem : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    AnimatedPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr)
        : QGraphicsObject(parent), pixmapItem(new QGraphicsPixmapItem(pixmap, this)) {}

    QRectF boundingRect() const override {
        return pixmapItem->boundingRect();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
        // 绘制是由 QGraphicsPixmapItem 处理的
    }
private:
    QGraphicsPixmapItem *pixmapItem;
};


#endif // ANIMATEDPIXMAPITEM_H
