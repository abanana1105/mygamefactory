#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "GameTile.h"
#include "DeliveryCenter.h"
#include <QGraphicsScene>
#include <vector>

class GameMap {
public:
    GameMap(int width = 200, int height = 100, int tileSize = 100);
    ~GameMap();


    void generateTiles();
    void createDeliveryCenter();
    void createmineral();
    QGraphicsScene* getScene() const;

public:
    QGraphicsScene *scene;
    int width, height;
    int tileSize;
    std::vector<std::vector<GameTile*>> tiles; // 使用 vector 管理地块
};

#endif // GAMEMAP_H

