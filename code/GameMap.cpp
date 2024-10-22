#include "GameMap.h"
#include "GameTile.h"
#include "Mineral.h"
#include "DeliveryCenter.h"
#include "global.h"
#include <QGraphicsScene>

GameMap::GameMap(int width, int height, int tileSize) : width(width), height(height), tileSize(tileSize){
    scene = new QGraphicsScene();
    generateTiles();
    createDeliveryCenter();
    createmineral();
}

GameMap::~GameMap() {
    // 清理 GameTile 对象
    for (auto &row : tiles) {
        for (auto *tile : row) {
            delete tile;
        }
    }
    delete scene; // 清理 QGraphicsScene 对象
}

void GameMap::generateTiles() {
    tiles.resize(width, std::vector<GameTile*>(height, nullptr));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            GameTile *tile = new GameTile(x, y);
            tiles[x][y] = tile;
            scene->addItem(tile); // 将地块添加到场景
        }
    }
    scene->setSceneRect(0, 0, width * tileSize, height * tileSize);
}

void GameMap::createDeliveryCenter() {
    int centerSize = 4; // 交付中心的大小

    // 计算交付中心的位置
    QPointF centerPosition((width / 2 - centerSize / 2) * tileSize, (height / 2 - 2) * tileSize);
    QPixmap centerPixmap(":/resourses/images/center.png");

    // 创建交付中心
    globalDeliveryCenter = new DeliveryCenter(centerPixmap, centerPosition, centerSize);
    scene->addItem(globalDeliveryCenter);
    for (int x = width / 2 - 2; x < width / 2 + 2; ++x) {
        for (int y = height / 2 - 2; y < height / 2 + 2; ++y) {
            tiles[x][y]->setCenter(true);
        }
    }
}

void GameMap::createmineral() {
    const auto& centers = Mineral::getMiningCenters();
    for (size_t i = 0; i < centers.size(); ++i) {
        int xCenter = centers[i].x();
        int yCenter = centers[i].y();
        int mineralType = (i % 2) + 1; // 交替使用矿物类型1和2

        for (int dx = -2; dx <= 2; ++dx) {
            for (int dy = -2; dy <= 2; ++dy) {
                int x = xCenter + dx;
                int y = yCenter + dy;
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    tiles[x][y]->setMineralType(mineralType);
                }
            }
        }
    }
}

QGraphicsScene* GameMap::getScene() const {
    return scene;
}
