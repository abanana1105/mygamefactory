#ifndef GAMETILE_H
#define GAMETILE_H

#include "Miner.h"
#include "Mineral.h"
#include "Cut.h"
#include "ConveyorBelt.h"
#include <memory>
#include <QGraphicsRectItem>

class GameTile : public QGraphicsRectItem {
public:
    GameTile(int x, int y, int tileSize = 100);
    int xPos, yPos;
    bool is_cut1 = false;
    bool is_cut2 = false;
    bool is_dustbin = false;
    bool is_center;
    GameTile *cut2 = nullptr;
    GameTile *cut1 = nullptr;
    QGraphicsPixmapItem* cutp = nullptr;

    bool isCenter() const;
    void setCenter(bool center);

    void setMineralType(int type);
    int getMineralType() const;
    void updateAppearance(); // 根据矿物类型更新地块的外观

    bool isEmptyfortrans();
    bool isTrans() const;
    void setConveyorBelt(const std::shared_ptr<ConveyorBelt>& conveyorBelt);
    void updatetransAppearance();
    std::shared_ptr<ConveyorBelt> getConveyorBelt() const;

    bool isEmptyformine();
    bool isMiner() const;
    void setMiner(const std::shared_ptr<Miner>& miner);
    void updateMinerAppearance();
    std::shared_ptr<Miner> getMiner() const;

    bool isEmptyfordustbin();
    void setDustbin();

    bool isEmptyfordouble();
    void setDouble();

    bool isEmptyforcut();
    bool isCut() const;
    void setCut(const std::shared_ptr<Cut>& cut);
    void updateCutAppearance();
    std::shared_ptr<Cut> getCut() const;
    void removeCut();

    void updateMine();
    void updateTrans();
    void updateCut();
    void extractMineral();
    void transferMineral();
    void cuttransferMineral();
    void cutMineral();
    GameTile* getNextTileDirection();
    bool canReceiveMineral(GameTile* sourceTile);
    void receiveMineral(GameTile* sourceTile);
    void dispatchMineral();
    void showMineralBlockedIcon();
    void hideMineralBlockedIcon();

    int mineralType; // 矿物类型：0为非矿区，1为矿物1，2为矿物2
    int nums = 0;
    bool is_trans = false;
    bool is_mine = false;
    bool is_double = false;

    QGraphicsPixmapItem* blockedIcon = nullptr;
    int hasMineral = 0; // 是否有矿物等待传输

    std::shared_ptr<ConveyorBelt> conveyorBelt;
    std::shared_ptr<Miner> miner;
    std::shared_ptr<Cut> cut;
};

#endif // GAMETILE_H
