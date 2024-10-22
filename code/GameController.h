#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "GameMap.h"
#include <QObject>
#include <QString>
#include <QGraphicsScene>

class GameController : public QObject {
    Q_OBJECT

public:
    GameMap *gameMap;
    GameController(QObject *parent = nullptr);
    QGraphicsScene *getScene() const;
    GameMap *getMap();

    void animateMineral(int startX, int startY, int endX, int endY, QString imagepath);
    void upspeed();

    void saveGame();
    void loadGame();
    void saveallGame();
    void loadallGame();

    int speed = 1000;
    int speedmine = 1000;
    int speedcut = 1000;

private:
    QGraphicsScene *scene;
};

#endif
