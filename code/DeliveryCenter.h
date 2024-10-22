#ifndef DELIVERYCENTER_H
#define DELIVERYCENTER_H

#include <QGraphicsPixmapItem>
#include <QPointF>

class DeliveryCenter : public QGraphicsPixmapItem {
public:
    DeliveryCenter(const QPixmap &pixmap, const QPointF &position, int size);

    QPointF getPosition() const;
    void setPosition(const QPointF &position);

    int getSize() const;
    void setSize(int size);

    void createText();
    void setText(QString s , int a, int b, int c);
    void updateText();
    void receive(int type, int multiply = 1);

    void up_center();
    void up_mineral();
    void up_value();

    int upcount = 0;
    int count = 0;
    int upcenter = 0;
    int upmineral = 0;
    int upvalue = 0;
    int mul = 1;
    int size;

    QGraphicsTextItem *deliveryCenterText = nullptr;
    QPointF position; // 交付中心的位置
    int step = 1;
    int mineral1 = 0;
    int mineral2 = 0;
    int mineral3 = 0;
    int mineral4 = 0;
    int value1 = 1;
    int value2 = 2;
    int value3 = 3;
    int value4 = 4;
};

#endif // DELIVERYCENTER_H
