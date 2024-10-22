#include "GameView.h"
#include "GameMap.h"
#include "ConveyorBelt.h"
#include "GameTile.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QCursor>
#include <QApplication>
#include <iostream>

GameView::GameView(GameController *controller, GameMap *map, QWidget *parent) : QGraphicsView(parent) {
    placingConveyorBelt = false;
    gameMap = map;
    setScene(controller->getScene());

    // 设置视图属性
    setFocusPolicy(Qt::StrongFocus);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);  // 设置缩放锚点为鼠标位置
}

GameTile* GameView::getTileAt(const QPointF& point) {
    int x = static_cast<int>(point.x()) / 100; // 假设您知道每个地块的大小
    int y = static_cast<int>(point.y()) / 100;

    // 确保 x 和 y 在 tiles 的范围内
    if (x >= 0 && x < gameMap->tiles.size() && y >= 0 && y < gameMap->tiles[0].size()) {
        return gameMap->tiles[x][y];
    }

    return nullptr; // 如果坐标超出范围，则返回 nullptr
}

void GameView::enableConveyorBeltPlacement() {
    placingConveyorBelt = true;
    setcursor();
}

void GameView::disableConveyorBeltPlacement() {
    placingConveyorBelt = false;
}

void GameView::wheelEvent(QWheelEvent *event) {
    if (!placingConveyorBelt) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }
}

void GameView::mousePressEvent(QMouseEvent *event) {
    if (placingConveyorBelt) {
        conveyorBeltStartPoint = mapToScene(event->pos());
        int x = static_cast<int>(conveyorBeltStartPoint.x()/100);
        int y = static_cast<int>(conveyorBeltStartPoint.y()/100);
        conveyorBeltPath.clear();
        conveyorBeltPath.push_back(std::make_pair(x, y));
        press = true;
    } else if(placingMiner) {
        minerPoint = mapToScene(event->pos());
    } else if(placingDustbin) {
        dustbinPoint = mapToScene(event->pos());
    } else if(placingDouble) {
        doublePoint = mapToScene(event->pos());
    } else if(placingCut) {
        cutPoint = mapToScene(event->pos());
    }
    else {
        // 处理地图拖拽的逻辑
        QGraphicsView::mousePressEvent(event);
    }
}

void GameView::mouseMoveEvent(QMouseEvent *event) {
    if (placingConveyorBelt  && press) {
        QPointF currentPoint = mapToScene(event->pos());
        int x = static_cast<int>(currentPoint.x() / 100);
        int y = static_cast<int>(currentPoint.y() / 100);

        // 检查当前地块坐标是否已在 conveyorBeltPath 中
        std::pair<int, int> currentTile(x, y);
        if (std::find(conveyorBeltPath.begin(), conveyorBeltPath.end(), currentTile) == conveyorBeltPath.end()) {
            // 如果当前地块还没有被添加，加入到路径中
            conveyorBeltPath.push_back(currentTile);
        }
        placeConveyorBelt();
    } else {
        // 处理地图拖拽的逻辑
        QGraphicsView::mouseMoveEvent(event);
    }
}

void GameView::mouseReleaseEvent(QMouseEvent *event) {
    if (placingConveyorBelt) {
        placeConveyorBelt();
        press = false;
        disableConveyorBeltPlacement();
        QApplication::restoreOverrideCursor();
    }else if(placingMiner) {
        placeMiner();
        disableMinerPlacement();
        QApplication::restoreOverrideCursor();
    }else if(placingDustbin) {
        placeDustbin();
        disableDustbinPlacement();
        QApplication::restoreOverrideCursor();
    }else if(placingDouble) {
        placeDouble();
        disableDoublePlacement();
        QApplication::restoreOverrideCursor();
    }else if(placingCut) {
        placeCut();
        disableCutPlacement();
        QApplication::restoreOverrideCursor();
    }
    else {
        // 处理地图拖拽的逻辑
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void GameView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_R) {
            // 旋转开采器方向
        if (placingMiner){
            minerDirection = getNextMinerDirection(minerDirection);
            QApplication::restoreOverrideCursor();
            setcursor();
        }
        if (placingCut){
            cutDirection = getNextCutDirection(cutDirection);
            QApplication::restoreOverrideCursor();
            setcursor();
        }
    }
    else {
        // 在这里处理其他按键事件
    }

    QGraphicsView::keyPressEvent(event); // 调用基类的处理，确保其他功能不受影响
}

void GameView::placeConveyorBelt() {
    for (size_t i = 0; i < conveyorBeltPath.size(); ++i) {
        GameTile* tile = gameMap->tiles[conveyorBeltPath[i].first][conveyorBeltPath[i].second];

            if (tile->isEmptyfortrans()) {
            ConveyorBelt::Direction direction = determineDirection(i);
            QPointF currentPoint(100*conveyorBeltPath[i].first + 50, 100*conveyorBeltPath[i].second + 50);
            std::shared_ptr<ConveyorBelt> conveyorBelt = std::make_shared<ConveyorBelt>(direction, currentPoint);
            tile->setConveyorBelt(conveyorBelt);
        }
    }
}

ConveyorBelt::Direction GameView::determineDirection(size_t index) {
    std::pair<int, int> prev, current, next;

    if(conveyorBeltPath.size() == 1){
        return ConveyorBelt::trans_left_right;
    }

    // 确保不会越界
    current = conveyorBeltPath[index];
    if (index > 0) {
        prev = conveyorBeltPath[index - 1];
    }
    if (index < conveyorBeltPath.size() - 1) {
        next = conveyorBeltPath[index + 1];
    }

    if (index == 0) {
        int x = current.first - next.first;
        int y = current.second - next.second;
        std::pair<int, int> temp = std::make_pair(current.first + x, current.second + y);
        prev = temp;
    }
    if (index == conveyorBeltPath.size() - 1) {
        int x = current.first - prev.first;
        int y = current.second - prev.second;
        std::pair<int, int> temp = std::make_pair(current.first + x, current.second + y);
        next = temp;
    }

    // 判断方向
    if(next.first == prev.first) {
        if(current.second > prev.second) {
            return ConveyorBelt::trans_up_down;
        } else {
            return ConveyorBelt::trans_down_up;
        }
    }
    else if(next.second == prev.second) {
        if(current.first > prev.first) {
            return ConveyorBelt::trans_left_right;
        } else {
            return ConveyorBelt::trans_right_left;
        }
    }
    else {
        if(current.second == prev.second) {
            if(current.first > prev.first) {
                if(current.second > next.second) {
                    return ConveyorBelt::trans_left_up;
                }
                else {
                    return ConveyorBelt::trans_left_down;
                }
            }
            else {
                if(current.second > next.second) {
                    return ConveyorBelt::trans_right_up;
                }
                else {
                    return ConveyorBelt::trans_right_down;
                }
            }
        }
        else {
            if(current.second > prev.second) {
                if(current.first > next.first) {
                    return ConveyorBelt::trans_up_left;
                }
                else {
                    return ConveyorBelt::trans_up_right;
                }
            }
            else {
                if(current.first > next.first) {
                    return ConveyorBelt::trans_down_left;
                }
                else {
                    return ConveyorBelt::trans_down_right;
                }
            }
        }
    }


    return ConveyorBelt::trans_left_right; // 默认方向
}

void GameView::enableMinerPlacement() {
    placingMiner = true;
    setcursor();
}

void GameView::disableMinerPlacement() {
    placingMiner = false;
}

void GameView::placeMiner() {
    if (placingMiner) {
        GameTile* tile = getTileAt(minerPoint.toPoint());// 获取 minerPoint 位置的 GameTile
            if (tile->isEmptyformine()) {
            std::shared_ptr<Miner> miner = std::make_shared<Miner>(minerDirection, minerPoint);
            tile->setMiner(miner);
        }
    }
}

Miner::Direction GameView::determineMinerDirection() {
    // 根据按键旋转开采器方向
    // 这个方法可能需要根据具体需求进行调整
    return minerDirection;
}

Miner::Direction GameView::getNextMinerDirection(Miner::Direction currentDirection) {
    // 根据当前方向计算下一个方向
    switch (currentDirection) {
    case Miner::Up:    return Miner::Right;
    case Miner::Right: return Miner::Down;
    case Miner::Down:  return Miner::Left;
    case Miner::Left:  return Miner::Up;
    default:           return Miner::Up; // 默认方向
    }
}

void GameView::enableDustbinPlacement() {
    placingDustbin = true;
    setcursor();
}

void GameView::disableDustbinPlacement() {
    placingDustbin = false;
}

void GameView::placeDustbin() {
    if (placingDustbin) {
        GameTile* tile = getTileAt(dustbinPoint.toPoint());// 获取 minerPoint 位置的 GameTile
        if (tile->isEmptyfordustbin()) {
            tile->setDustbin();
        }
    }
}

void GameView::enableDoublePlacement() {
    placingDouble = true;
    setcursor();
}

void GameView::disableDoublePlacement() {
    placingDouble = false;
}

void GameView::placeDouble() {
    if (placingDouble) {
        GameTile* tile = getTileAt(doublePoint.toPoint());// 获取 minerPoint 位置的 GameTile
        if (tile->isEmptyfordouble()) {
            tile->setDouble();
        }
    }
}

void GameView::enableCutPlacement() {
    placingCut = true;
    setcursor();
}

void GameView::disableCutPlacement() {
    placingCut = false;
}

void GameView::placeCut(){
    if (placingCut) {
        GameTile* tile = getTileAt(cutPoint.toPoint());// 获取 minerPoint 位置的 GameTile
        if (tile->isEmptyforcut()) {
            std::shared_ptr<Cut> cut = std::make_shared<Cut>(cutDirection, cutPoint);
            tile->setCut(cut);
        }
    }
}

Cut::Direction GameView::determineCutDirection(){
    return cutDirection;
}

Cut::Direction GameView::getNextCutDirection(Cut::Direction currentDirection) {
    switch (currentDirection) {
    case Cut::Up_left:
        return Cut::Down_left;
    case Cut::Down_left:
        return Cut::Left_up;
    case Cut::Left_up:
        return Cut::Right_up;
    case Cut::Right_up:
        return Cut::Up_Right;
    case Cut::Up_Right:
        return Cut::Down_Right;
    case Cut::Down_Right:
        return Cut::Left_Down;
    case Cut::Left_Down:
        return Cut::Right_Down;
    case Cut::Right_Down:
        return Cut::Up_left;  // 循环回到起始方向
    default:
        return Cut::Up_left;  // 默认值
    }
}

void GameView::setcursor() {
    QString imagepath = ":/resourses/images/";
    QPixmap scaledPixmap;
    if(placingConveyorBelt) {
        imagepath += "trans_right_left";
        imagepath += ".png";
        QPixmap cursorPixmap(imagepath);
        scaledPixmap = cursorPixmap.scaled(40, 40);
    }
    else if(placingCut) {
        switch (cutDirection) {
        case Cut::Up_left: imagepath += "cutUpLeft"; break;
        case Cut::Down_left: imagepath += "cutDownLeft"; break;
        case Cut::Left_up: imagepath += "cutLeftUp"; break;
        case Cut::Right_up: imagepath += "cutRightUp"; break;
        case Cut::Up_Right: imagepath += "cutUpRight"; break;
        case Cut::Down_Right:imagepath += "cutDownRight"; break;
        case Cut::Left_Down: imagepath += "cutLeftDown"; break;
        case Cut::Right_Down: imagepath += "cutRightDown"; break;
        default:
            break;
        }
        QPixmap cursorPixmap(imagepath);
        scaledPixmap = cursorPixmap.scaled(40, 40);
    }
    else if(placingDouble) {
        imagepath += "double";
        imagepath += ".png";
        QPixmap cursorPixmap(imagepath);
        scaledPixmap = cursorPixmap.scaled(40, 40);
        }
    else if(placingDustbin) {
        imagepath += "dustbin";
        imagepath += ".png";
        QPixmap cursorPixmap(imagepath);
        scaledPixmap = cursorPixmap.scaled(40, 40);
    }
    else {
        switch (minerDirection) {
        case Miner::Up: imagepath += "mineUp"; break;
        case Miner::Down: imagepath += "mineDown"; break;
        case Miner::Left: imagepath += "mineLeft"; break;
        case Miner::Right: imagepath += "mineRight"; break;
        default: break;
        }
        imagepath += ".png";
        QPixmap cursorPixmap(imagepath);
        scaledPixmap = cursorPixmap.scaled(40, 40);
    }

    QCursor customCursor(scaledPixmap);
    // 设置应用程序的光标
    QApplication::setOverrideCursor(customCursor);
}
