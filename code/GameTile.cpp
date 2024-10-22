#include "GameTile.h"
#include "GameMap.h"
#include "Miner.h"
#include "global.h"
#include <QBrush>
#include <QPen>
#include <QString>
#include <QPixmap>
#include <iostream>

GameTile::GameTile(int x, int y, int tileSize) :
    xPos(x), yPos(y), is_center(false), mineralType(0), is_trans(false)
{
    setRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize);
    setBrush(QBrush(Qt::white));
    setPen(QPen(Qt::gray));
}

bool GameTile::isEmptyfortrans() {
    return !(is_center || bool(mineralType));
}

bool GameTile::isCenter() const {
    return is_center;
}

void GameTile::setCenter(bool center) {
    is_center = center;
}

void GameTile::setMineralType(int type) {
    mineralType = type;
    nums = 10000;
    updateAppearance();
}

int GameTile::getMineralType() const {
    return mineralType;
}

void GameTile::updateAppearance() {
    if (mineralType == 1) {
        QPixmap originalPixmap(":/resourses/images/mineral1.png");
        //QPixmap scaledPixmap = originalPixmap.scaled(10, 10);
        setBrush(originalPixmap);
    } else if (mineralType == 2) {
        QPixmap originalPixmap(":/resourses/images/mineral2.png");
        //QPixmap scaledPixmap = originalPixmap.scaled(10, 10);
        setBrush(originalPixmap);
    } else {
        setBrush(QBrush(Qt::white)); // 非矿区为白色
    }
}

void GameTile::setConveyorBelt(const std::shared_ptr<ConveyorBelt>& conveyorBelt) {
    this->conveyorBelt = conveyorBelt;
    is_trans = true;
    is_dustbin = false;
    if(is_cut1 || is_cut2) {
        removeCut();
    }
    updatetransAppearance();
}

bool GameTile::isTrans() const{
    return is_trans;
}

std::shared_ptr<ConveyorBelt> GameTile::getConveyorBelt() const {
    return conveyorBelt;
}

void GameTile::updatetransAppearance() {
    if (conveyorBelt) {
        ConveyorBelt::Direction dir = conveyorBelt->getDirection();
        QString imagePath = ":/resourses/images/";
        switch (dir) {
        case ConveyorBelt::trans_down_left: imagePath += "trans_down_left"; break;
        case ConveyorBelt::trans_down_right: imagePath += "trans_down_right"; break;
        case ConveyorBelt::trans_down_up: imagePath += "trans_down_up"; break;

        case ConveyorBelt::trans_up_left: imagePath += "trans_up_left"; break;
        case ConveyorBelt::trans_up_right: imagePath += "trans_up_right"; break;
        case ConveyorBelt::trans_up_down: imagePath += "trans_up_down"; break;

        case ConveyorBelt::trans_left_up: imagePath += "trans_left_up"; break;
        case ConveyorBelt::trans_left_down: imagePath += "trans_left_down"; break;
        case ConveyorBelt::trans_left_right: imagePath += "trans_left_right"; break;

        case ConveyorBelt::trans_right_up: imagePath += "trans_right_up"; break;
        case ConveyorBelt::trans_right_down: imagePath += "trans_right_down"; break;
        case ConveyorBelt::trans_right_left: imagePath += "trans_right_left"; break;
        default: break;
        }
        imagePath += ".png";
        QPixmap originalPixmap(imagePath);
        //QPixmap scaledPixmap = originalPixmap.scaled(10, 10);
        setBrush(originalPixmap);
    } else {
        setBrush(Qt::white); // 如果没有传送带，恢复默认样式
    }
}

bool GameTile::isEmptyformine() {
    // 地块被认为是空的，如果它不是矿区且没有开采器
    return mineralType != 0;
}

bool GameTile::isMiner() const {
    // 如果地块上有开采器，则返回 true
    return is_mine;
}

void GameTile::setMiner(const std::shared_ptr<Miner>& miner) {
    this->miner = miner;
    is_mine = true; // 示例坐标

    updateMinerAppearance();
}

void GameTile::updateMinerAppearance() {
    if (is_mine && miner) {
        // 根据开采器的方向设置图像
        Miner::Direction dir = miner->getDirection();
        QString imagePath = ":/resourses/images/";
        switch (dir) {
        case Miner::Up: imagePath += "mineUp"; break;
        case Miner::Down: imagePath += "mineDown"; break;
        case Miner::Left: imagePath += "mineLeft"; break;
        case Miner::Right: imagePath += "mineRight"; break;
        default: break;
        }
        imagePath += ".png";
        QPixmap originalPixmap(imagePath);
        //QPixmap scaledPixmap = originalPixmap.scaled(10, 10);
        setBrush(originalPixmap);
    } else {
        setBrush(Qt::white); // 如果没有传送带，恢复默认样式
    }
}

std::shared_ptr<Miner> GameTile::getMiner() const {
    return miner;
}

void GameTile::updateMine() {
    if(is_mine){
        extractMineral(); // 开采矿产
    }
}

void GameTile::updateTrans() {
    if(is_trans && hasMineral){
        transferMineral();
    }
}

void GameTile::updateCut() {
    if(is_cut1 && hasMineral){
        cutMineral();
        cuttransferMineral();
    }
}

void GameTile::extractMineral() {
    // 如果没有矿物且有开采器，重置地块属性
    if (nums == 0 && isMiner()) {
        mineralType = 0;
        is_mine = false;
        updateAppearance(); // 更新地块外观
    }
    // 如果有矿物且有开采器
    else if (hasMineral && isMiner()) {
        transferMineral(); // 尝试传输矿物
    }
    // 如果是矿物地块且有开采器且还有矿物
    else if (isMiner() && mineralType > 0 && nums > 0) {
        hasMineral = mineralType; // 设置有矿物等待传输的标志
        transferMineral(); // 尝试传输矿物
    }
}

void GameTile::transferMineral() {
    // 尝试将矿物传输到下一个地块
    GameTile* targetTile = getNextTileDirection();
    if (targetTile && targetTile->canReceiveMineral(this)) {

        // 隐藏阻塞图标（如果有）
        hideMineralBlockedIcon();
        // 动画
        GameController* controller = globalGameController;
        QString imagePath;
        switch (hasMineral) {
        case 1: imagePath = ":/resourses/images/mineral1_trans.png"; break;
        case 2: imagePath = ":/resourses/images/mineral2_trans.png"; break;
        case 3: imagePath = ":/resourses/images/mineral2_left.png"; break;
        case 4: imagePath = ":/resourses/images/mineral2_right.png"; break;
        default: break;
        }
        controller->animateMineral(xPos*100+25, yPos*100+25, targetTile->xPos*100+25, targetTile->yPos*100+25, imagePath);
        targetTile->receiveMineral(this);
        dispatchMineral();
    } else {
        // 显示阻塞图标
        showMineralBlockedIcon();
    }
}

void GameTile::cuttransferMineral() {
    GameTile* targetTile1 = getNextTileDirection();
    GameTile* targetTile2 = cut2 -> getNextTileDirection();
    if (targetTile2->canReceiveMineral(cut2) && targetTile1->canReceiveMineral(this)) {

        // 隐藏阻塞图标（如果有）
        hideMineralBlockedIcon();
        cut2 -> hideMineralBlockedIcon();
        // 动画
        GameController* controller = globalGameController;
        QString imagePath1 = ":/resourses/images/mineral2_left.png";
        QString imagePath2 = ":/resourses/images/mineral2_right.png";
        controller->animateMineral(xPos*100+25, yPos*100+25, (targetTile1->xPos)*100+25, (targetTile1->yPos)*100+25,imagePath1);
        controller->animateMineral((cut2->xPos)*100+25, (cut2->yPos)*100+25, (targetTile2->xPos)*100+25, (targetTile2->yPos)*100+25, imagePath2);
        targetTile1->receiveMineral(this);
        targetTile2->receiveMineral(cut2);
        dispatchMineral();
    } else {
        // 显示阻塞图标
        showMineralBlockedIcon();
        cut2 -> showMineralBlockedIcon();
    }
}

void GameTile::cutMineral() {
    if(hasMineral == 2) {
        hasMineral = 3;
        cut2 -> hasMineral = 4;
    }
}

GameTile* GameTile::getNextTileDirection() {
    // 需要访问整个地图的tiles数组
    // 假设有一个全局或父对象提供对它的访问
    // 假设 tiles 的类型为 std::vector<std::vector<GameTile*>>
    GameMap* gameMap = globalGameMap;
    // 获取当前地块的坐标
    int x = xPos;  // 假设 GameTile 有 x 和 y 属性
    int y = yPos;

    if (isMiner()) {
        // 如果是开采器，根据其方向返回相邻地块
        switch (miner->getDirection()) {
        case Miner::Up:    return gameMap->tiles[x][y - 1];
        case Miner::Down:  return gameMap->tiles[x][y + 1];
        case Miner::Left:  return gameMap->tiles[x - 1][y];
        case Miner::Right: return gameMap->tiles[x + 1][y];
        }
    } else if (is_trans) {
        // 如果是传送带，根据其方向返回相邻地块
        ConveyorBelt::Direction dir = conveyorBelt->getDirection();
        switch (dir) {
        case ConveyorBelt::trans_down_left:
        case ConveyorBelt::trans_up_left:
        case ConveyorBelt::trans_right_left:
            return gameMap->tiles[x - 1][y];  // 左侧的地块

        case ConveyorBelt::trans_down_right:
        case ConveyorBelt::trans_up_right:
        case ConveyorBelt::trans_left_right:
            return gameMap->tiles[x + 1][y];  // 右侧的地块

        case ConveyorBelt::trans_left_up:
        case ConveyorBelt::trans_right_up:
        case ConveyorBelt::trans_down_up:
            return gameMap->tiles[x][y - 1];  // 上方的地块

        case ConveyorBelt::trans_left_down:
        case ConveyorBelt::trans_right_down:
        case ConveyorBelt::trans_up_down:
            return gameMap->tiles[x][y + 1];  // 下方的地块
        }
    } else if(is_cut1 || is_cut2) {

        Cut::Direction dir;
        if(is_cut1) {
            dir = cut->getDirection();
        } else {
            dir = cut1 -> cut ->getDirection();
        }
        switch (dir) {
        case Cut::Up_left:
        case Cut::Up_Right:
            return gameMap->tiles[x][y - 1]; // 上方的地块

        case Cut::Down_left:
        case Cut::Down_Right:
            return gameMap->tiles[x][y + 1]; // 下方的地块

        case Cut::Left_up:
        case Cut::Left_Down:
            return gameMap->tiles[x - 1][y]; // 左侧的地块

        case Cut::Right_up:
        case Cut::Right_Down:
            return gameMap->tiles[x + 1][y]; // 右侧的地块

        default:
            break;

        }
    }

    // 如果不是开采器或传送带，或者越界，返回nullptr
    return nullptr;
}

bool GameTile::canReceiveMineral(GameTile* sourceTile) {
    // 确保这是一个传送带
    if (is_trans) {
        // 确保传送带当前没有矿物
        if (!hasMineral) {
            // 检查矿物来源的方向是否与传送带的起始方向一致
            ConveyorBelt::Direction dir = conveyorBelt->getDirection();
            switch (dir) {
            case ConveyorBelt::trans_down_left:
            case ConveyorBelt::trans_down_right:
            case ConveyorBelt::trans_down_up:
                return sourceTile->yPos > this->yPos; // 矿物应从下方来

            case ConveyorBelt::trans_up_left:
            case ConveyorBelt::trans_up_right:
            case ConveyorBelt::trans_up_down:
                return sourceTile->yPos < this->yPos; // 矿物应从上方来

            case ConveyorBelt::trans_left_up:
            case ConveyorBelt::trans_left_down:
            case ConveyorBelt::trans_left_right:
                return sourceTile->xPos < this->xPos; // 矿物应从左方来

            case ConveyorBelt::trans_right_up:
            case ConveyorBelt::trans_right_down:
            case ConveyorBelt::trans_right_left:
                return sourceTile->xPos > this->xPos; // 矿物应从右方来
            }
        }
    }
    if (is_cut1) {
        // 确保当前没有矿物
        if (!hasMineral && !(cut2 -> hasMineral) && sourceTile -> hasMineral == 2) {
            // 检查矿物来源的方向是否与切割机的朝向一致
            Cut::Direction dir = cut->getDirection();
            switch (dir) {
            case Cut::Up_left:
            case Cut::Up_Right:
                return sourceTile->yPos > this->yPos; // 矿物应从下方来

            case Cut::Down_left:
            case Cut::Down_Right:
                return sourceTile->yPos < this->yPos; // 矿物应从上方来

            case Cut::Left_up:
            case Cut::Left_Down:
                return sourceTile->xPos > this->xPos; // 矿物应从右方来

            case Cut::Right_up:
            case Cut::Right_Down:
                return sourceTile->xPos < this->xPos; // 矿物应从左方来

            default:
                break;
            }
        }
    }
    if (is_center) {
        return true;
    }
    if (is_dustbin) {
        return true;
    }
    if (is_cut2) {
        return false;
    }
    return false;
}

void GameTile::receiveMineral(GameTile* sourceTile) {
    // 如果这个地块是传送带
    if (is_trans) {
        // 标记这个传送带上现在有矿物
        hasMineral = sourceTile -> hasMineral;
        // 启动矿物的移动逻辑，可能涉及到定时器或游戏循环中的更新
    }
    // 如果这个地块是交付中心
    else if(is_center){
        if(!is_double){
            globalDeliveryCenter -> receive(sourceTile -> hasMineral);
        }
        else {
            globalDeliveryCenter -> receive(sourceTile -> hasMineral, 2);
        }
    }
    else if(is_dustbin){

    }
    else if(is_cut1) {
        hasMineral = sourceTile -> hasMineral;
    }
}

void GameTile::dispatchMineral() {
    if (is_trans) {
        hasMineral = 0;
    }
    if (is_mine) {
        hasMineral = 0;
        nums--;
    }
    if (is_cut1) {
        hasMineral = 0;
        cut2 -> hasMineral = 0;
    }
}

void GameTile::showMineralBlockedIcon() {
    if (!blockedIcon) {
        QString imagePath;
        switch (hasMineral) {
        case 1: imagePath = ":/resourses/images/mineral1_trans.png"; break;
        case 2: imagePath = ":/resourses/images/mineral2_trans.png"; break;
        case 3: imagePath = ":/resourses/images/mineral2_left.png"; break;
        case 4: imagePath = ":/resourses/images/mineral2_right.png"; break;
        default: break;
        }
        blockedIcon = new QGraphicsPixmapItem(QPixmap(imagePath));
        blockedIcon->setPos(xPos * 100 + 25, yPos * 100 + 25);
        globalGameController->getMap()->getScene()->addItem(blockedIcon);
    }
}

void GameTile::hideMineralBlockedIcon() {
    if (blockedIcon) {
        globalGameController->getMap()->getScene()->removeItem(blockedIcon);
        delete blockedIcon;
        blockedIcon = nullptr;
    }
}

bool GameTile::isEmptyfordustbin(){
    return !(is_center || bool(mineralType));
}

void GameTile::setDustbin() {
    is_dustbin = true;
    is_trans = false;
    if(is_cut1 || is_cut2) {
        removeCut();
    }
    QPixmap originalPixmap(":/resourses/images/dustbin.png");
    setBrush(originalPixmap);
}

bool GameTile::isEmptyfordouble() {
    return is_center;
}

void GameTile::setDouble() {
    is_double = true;
    QPixmap originalPixmap(":/resourses/images/double.png");
    setBrush(originalPixmap);
}

bool GameTile::isEmptyforcut() {
    return !(is_center || bool(mineralType));
}

bool GameTile::isCut() const {
    return is_cut1; // 假设 is_cut 是一个表示地块上有切割机的成员变量
}

void GameTile::setCut(const std::shared_ptr<Cut>& cut) {
    this->cut = cut;
    updateCutAppearance();
}

std::shared_ptr<Cut> GameTile::getCut() const {
    return cut; // 假设 cut 是一个指向切割机对象的智能指针
}

void GameTile::updateCutAppearance() {
    if (cut) {
        // 根据切割机的方向设置图像
        Cut::Direction dir = cut->getDirection();
        QString imagePath = ":/resourses/images/cut"; // 基础路径

        // 为不同方向添加不同的后缀
        switch (dir) {
        case Cut::Up_left: imagePath += "UpLeft"; break;
        case Cut::Down_left: imagePath += "DownLeft"; break;
        case Cut::Left_up: imagePath += "LeftUp"; break;
        case Cut::Right_up: imagePath += "RightUp"; break;
        case Cut::Up_Right: imagePath += "UpRight"; break;
        case Cut::Down_Right:imagePath += "DownRight"; break;
        case Cut::Left_Down: imagePath += "LeftDown"; break;
        case Cut::Right_Down: imagePath += "RightDown"; break;
        default:
            break;
        }

        imagePath += ".png"; // 完成图像路径
        /**/

        GameTile * cuttemp;

        switch (dir) {
        case Cut::Up_left:
            cuttemp = globalGameMap->tiles[xPos - 1][yPos];
            break;
        case Cut::Down_left:
            cuttemp = globalGameMap->tiles[xPos - 1][yPos];
            break;
        case Cut::Left_up:
            cuttemp = globalGameMap->tiles[xPos][yPos - 1];
            break;
        case Cut::Right_up:
            cuttemp = globalGameMap->tiles[xPos][yPos - 1];
            break;
        case Cut::Up_Right:
            cuttemp = globalGameMap->tiles[xPos + 1][yPos];
            break;
        case Cut::Down_Right:
            cuttemp = globalGameMap->tiles[xPos + 1][yPos];
            break;
        case Cut::Left_Down:
            cuttemp = globalGameMap->tiles[xPos][yPos + 1];
            break;
        case Cut::Right_Down:
            cuttemp = globalGameMap->tiles[xPos][yPos + 1];
            break;
        default:
            break;
        }
        if(cuttemp -> isEmptyforcut()){
            removeCut();
            cuttemp -> removeCut();
            cut2 = cuttemp;
            cut2->cut1 = this;
            is_cut1 = true; // 设置地块上有切割机的标志
            is_trans = false;
            is_dustbin = false;
            cut2 -> is_cut2 = true;
            cut2 -> is_trans = false;
            cut2 -> is_dustbin = false;
            cutp = new QGraphicsPixmapItem(QPixmap(imagePath));

            switch (dir) {
            case Cut::Up_left: cutp->setPos((xPos-1) * 100, yPos * 100); break;
            case Cut::Down_left: cutp->setPos((xPos-1) * 100, yPos * 100); break;
            case Cut::Left_up: cutp->setPos(xPos * 100, (yPos-1) * 100); break;
            case Cut::Right_up: cutp->setPos(xPos * 100, (yPos-1) * 100); break;
            case Cut::Up_Right: cutp->setPos(xPos * 100, yPos * 100); break;
            case Cut::Down_Right:cutp->setPos(xPos * 100, yPos * 100); break;
            case Cut::Left_Down: cutp->setPos(xPos * 100, yPos * 100); break;
            case Cut::Right_Down: cutp->setPos(xPos * 100, yPos * 100); break;
            default:
                break;
            }
            globalGameController->getMap()->getScene()->addItem(cutp);
        }
    }

}

void GameTile::removeCut() {
    if(is_cut1) {
        globalGameController->getMap()->getScene()->removeItem(cutp);
        delete cutp;
        cutp = nullptr;
        setBrush(Qt::white);
        cut2 -> setBrush(Qt::white);
        cut2 -> is_cut2 = false;
        cut2 -> cut1 = nullptr;
        is_cut1 = false;
        cut2 = nullptr;
    }
    else if(is_cut2){
        globalGameController->getMap()->getScene()->removeItem(cut1 -> cutp);
        delete cut1 -> cutp;
        cut1 -> cutp = nullptr;
        setBrush(Qt::white);
        cut1 -> setBrush(Qt::white);
        cut1 -> is_cut1 = false;
        cut1 -> cut2 = nullptr;
        is_cut2 = false;
        cut1 = nullptr;
    }
    else{

    }
}
