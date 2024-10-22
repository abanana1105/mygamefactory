#ifndef MINERAL_H
#define MINERAL_H

#include <QPointF>
#include <vector>

class Mineral {
public:
    Mineral(int remaining, int value, int type);

    int getRemaining() const;
    void mine(); // 开采矿物
    int getValue() const;
    int getType() const;
    QPointF getPosition() const;
    static const std::vector<QPointF>& getMiningCenters();

private:
    int type; // 矿物类型
    int remaining; // 剩余矿物量
    int value; // 矿物价值
    static std::vector<QPointF> miningCenters; // 存储矿物地块的中心点
};

#endif // MINERAL_H
