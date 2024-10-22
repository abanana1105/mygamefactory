#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include "GameController.h"
#include "ConveyorBelt.h"
#include "GameTile.h"
#include "GameMap.h"
#include "Miner.h"
#include "Cut.h"

class GameView : public QGraphicsView {
    Q_OBJECT

public:
    GameView(GameController *controller, GameMap *map,  QWidget *parent = nullptr);

    // 添加用于启用和禁用传送带放置模式的方法
    void enableConveyorBeltPlacement();
    void disableConveyorBeltPlacement();

    void enableMinerPlacement();
    void disableMinerPlacement();

    void enableDustbinPlacement();
    void disableDustbinPlacement();

    void enableCutPlacement();
    void disableCutPlacement();

    void enableDoublePlacement();
    void disableDoublePlacement();

protected:
    void wheelEvent(QWheelEvent *event) override;
    GameTile* getTileAt(const QPointF& point);

    void setcursor();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void placeConveyorBelt();
    ConveyorBelt::Direction determineDirection(size_t index);

    void placeMiner();
    Miner::Direction determineMinerDirection();
    Miner::Direction getNextMinerDirection(Miner::Direction currentDirection);

    void placeDustbin();

    void placeDouble();

    void placeCut();
    Cut::Direction determineCutDirection();
    Cut::Direction getNextCutDirection(Cut::Direction currentDirection);

private:
    GameMap *gameMap;

    bool placingConveyorBelt;
    bool press = false;
    QPointF conveyorBeltStartPoint;
    std::vector<std::pair<int, int>> conveyorBeltPath;

    bool placingMiner = false;
    Miner::Direction minerDirection = Miner::Right;
    QPointF minerPoint;

    bool placingDustbin = false;
    QPointF dustbinPoint;

    bool placingDouble = false;
    QPointF doublePoint;

    bool placingCut = false;
    QPointF cutPoint;
    Cut::Direction cutDirection = Cut::Up_left;
};

#endif // GAMEVIEW_H


