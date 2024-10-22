#include "GameController.h"
#include "GameMap.h"
#include "AnimatedPixmapItem.h"
#include "global.h"
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QString>
#include <QJsonArray>
#include <QFile>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <iostream>

GameController::GameController(QObject *parent) : QObject(parent) {
    GameMap *map = new GameMap();
    gameMap = map;
    scene = map ->getScene();
}

QGraphicsScene *GameController::getScene() const {
    return scene;
}

GameMap *GameController::getMap(){
    return gameMap;
}

void GameController::animateMineral(int startX, int startY, int endX, int endY, QString imagepath) {
    // 创建矿物的图形表示
    AnimatedPixmapItem* mineralItem = new AnimatedPixmapItem(QPixmap(imagepath));
    mineralItem->setPos(startX, startY);
    scene->addItem(mineralItem);  // 添加到场景

    // 创建动画并设置属性
    QPropertyAnimation* animation = new QPropertyAnimation(mineralItem, "pos");
    animation->setDuration(speed);  // 动画持续时间，单位为毫秒
    animation->setStartValue(QPointF(startX, startY));
    animation->setEndValue(QPointF(endX, endY));

    // 连接动画结束信号
    QObject::connect(animation, &QAbstractAnimation::finished, [this, mineralItem]() {
        scene->removeItem(mineralItem);
        delete mineralItem;
    });

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameController::upspeed() {
    speed = 500;
}

void GameController::saveGame() {
    QJsonObject saveGame;

    // 保存 GameMap 中的 GameTiles
    QJsonArray tilesArray;
    GameMap* gameMap = globalGameMap;
    for (auto& row : gameMap->tiles) {
        for (auto& tile : row) {
            QJsonObject tileObject;
            tileObject["xPos"] = tile->xPos;
            tileObject["yPos"] = tile->yPos;
            tileObject["mineralType"] = tile->mineralType;
            tileObject["nums"] = tile->nums;
            tileObject["is_center"] =  tile->is_center;
            tileObject["is_double"] =  tile->is_double;
            tileObject["is_mine"] =  tile->is_mine;
            if (tile->is_mine) {
                tileObject["miner"] = static_cast<int>(tile->miner->getDirection());
            }
            tileObject["is_trans"] =  tile->is_trans;
            if (tile->is_trans) {
                tileObject["conveyorBelt"] = static_cast<int>(tile->conveyorBelt->getDirection());
            }
            tileObject["is_cut1"] =  tile->is_cut1;
            if (tile->is_cut1) {
                tileObject["cut"] = static_cast<int>(tile->cut->getDirection());
            }
            tileObject["hasMineral"] =  tile->hasMineral;
            tileObject["is_dustbin"] =  tile->is_dustbin;
            tilesArray.append(tileObject);
        }
    }

    QJsonObject center;
    center["upcount"] = globalDeliveryCenter -> upcount;
    center["count"] = globalDeliveryCenter -> count;
    center["size"] = globalDeliveryCenter -> size;
    center["step"] = globalDeliveryCenter -> step;
    center["mineral1"] = globalDeliveryCenter -> mineral1;
    center["mineral2"] = globalDeliveryCenter -> mineral2;
    center["mineral3"] = globalDeliveryCenter -> mineral3;
    center["mineral4"] = globalDeliveryCenter -> mineral4;

    QJsonObject control;
    control["speed"] = globalGameController -> speed;
    control["speedmine"] = globalGameController -> speedmine;
    control["speedcut"] = globalGameController -> speedcut;

    saveGame["control"] = control;
    saveGame["center"] = center;
    saveGame["tiles"] = tilesArray;

    // 写入 JSON 文件
    QFile saveFile(QCoreApplication::applicationDirPath() + "/savegame.json");
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(saveGame);
        saveFile.write(saveDoc.toJson());
    }
}

void GameController::loadGame() {
    QFile loadFile(QCoreApplication::applicationDirPath() + "/savegame.json");
    if (!loadFile.open(QIODevice::ReadOnly)) {
        // 处理文件无法打开的情况
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject loadGame = loadDoc.object();

    // 读取并设置控制参数
    QJsonObject control = loadGame["control"].toObject();
    this->speed = control["speed"].toInt();
    this->speedmine = control["speedmine"].toInt();
    this->speedcut = control["speedcut"].toInt();

    // 读取并恢复 DeliveryCenter 状态
    QJsonObject center = loadGame["center"].toObject();
    globalDeliveryCenter->upcount = center["upcount"].toInt();
    globalDeliveryCenter->count = center["count"].toInt();
    globalDeliveryCenter->size = center["size"].toInt();
    globalDeliveryCenter->step = center["step"].toInt();
    globalDeliveryCenter->mineral1 = center["mineral1"].toInt();
    globalDeliveryCenter->mineral2 = center["mineral2"].toInt();
    globalDeliveryCenter->mineral3 = center["mineral3"].toInt();
    globalDeliveryCenter->mineral4 = center["mineral4"].toInt();


    // 读取并恢复 GameTiles 状态
    QJsonArray tilesArray = loadGame["tiles"].toArray();
    for (const QJsonValue& tileVal : tilesArray) {
        QJsonObject tileObject = tileVal.toObject();
        int xPos = tileObject["xPos"].toInt();
        int yPos = tileObject["yPos"].toInt();
        GameTile* tile = gameMap->tiles[xPos][yPos]; // 假设这个函数可以根据位置找到对应的 GameTile

        tile->mineralType = tileObject["mineralType"].toInt();
        tile->nums = tileObject["nums"].toInt();
        tile->is_center = tileObject["is_center"].toBool();
        tile->is_double = tileObject["is_double"].toBool();
        tile->is_mine = tileObject["is_mine"].toBool();
        tile->hasMineral = tileObject["hasMineral"].toInt();
        tile->is_dustbin = tileObject["is_dustbin"].toBool();

        if (tile->mineralType) {
            tile->updateAppearance();
        }
        if (tileObject.contains("miner")) {
            Miner::Direction minerDir = static_cast<Miner::Direction>(tileObject["miner"].toInt());
            QPointF minerPosition((tile->xPos)*100+50, (tile->yPos)*100+50);
            tile->miner = std::make_shared<Miner>(minerDir, minerPosition);
            tile->setMiner(tile->miner);
        }

        if (tileObject.contains("conveyorBelt")) {
            ConveyorBelt::Direction beltDir = static_cast<ConveyorBelt::Direction>(tileObject["conveyorBelt"].toInt());
            QPointF beltPosition((tile->xPos)*100+50, (tile->yPos)*100+50);
            tile->conveyorBelt = std::make_shared<ConveyorBelt>(beltDir, beltPosition);
            tile->setConveyorBelt(tile->conveyorBelt);
        }

        if (tileObject.contains("cut")) {
            Cut::Direction cutDir = static_cast<Cut::Direction>(tileObject["cut"].toInt());
            QPointF cutPosition((tile->xPos)*100+50, (tile->yPos)*100+50);
            tile->cut = std::make_shared<Cut>(cutDir, cutPosition);
            tile->setCut(tile->cut);
        }

        if (tile->is_double) {
            tile->setDouble();
        }
        if (tile->is_dustbin) {
            tile->setDustbin();
        }
    }
}

void GameController::saveallGame() {
    QString savePath = QCoreApplication::applicationDirPath() + "/allup.json";

    // 存储 DeliveryCenter
    QJsonObject allup;
    allup["upcenter"] = globalDeliveryCenter -> upcenter;
    allup["upmineral"] = globalDeliveryCenter -> upmineral;
    allup["upvalue"] = globalDeliveryCenter -> upvalue;

    // 创建总的存档对象
    QJsonObject allall;
    allall["allups"] = allup;

    QFile saveFile(savePath);
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(allall);
        saveFile.write(saveDoc.toJson());
    }

}

void GameController::loadallGame() {
    QString savePath = QCoreApplication::applicationDirPath() + "/allup.json";
    QJsonObject loadGame;
    QFile loadFile(savePath);
    if (loadFile.open(QIODevice::ReadOnly)) {
        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        loadGame = loadDoc.object();
    }

    QJsonObject allup = loadGame["allups"].toObject();
    globalDeliveryCenter -> upcenter = allup["upcenter"].toInt();
    globalDeliveryCenter -> upmineral = allup["upmineral"].toInt();
    globalDeliveryCenter -> upvalue = allup["upvalue"].toInt();
    if(globalDeliveryCenter -> upcenter) {
        QPixmap newPixmap(":/resourses/images/upcenter.png");
        QPointF newPosition(9700, 4700); // 更新位置
        GameTile *up;
        for(int x = 97; x <= 102; x++) {
            for(int y = 47; y <= 52; y++) {
                up = globalGameMap -> tiles[x][y];
                up -> setDustbin();
                up -> is_dustbin = 0;
                up -> setBrush(Qt::white);
                up -> is_center = true;
            }
        }
        globalDeliveryCenter ->size = 6;
        globalDeliveryCenter ->setPixmap(newPixmap);
        globalDeliveryCenter ->setPos(newPosition);
    }
    if(globalDeliveryCenter -> upmineral) {
        GameTile *up;

        for(int x = 94; x <= 95; x++) {
            for(int y = 49; y <= 50; y++) {
                up = globalGameMap -> tiles[x][y];
                up -> setDustbin();
                up -> is_dustbin = 0;
                up -> setBrush(Qt::white);
                up -> setMineralType(1);
            }
        }
    }
    if(globalDeliveryCenter -> upvalue) {
        globalDeliveryCenter -> mul ++;
    }
}
