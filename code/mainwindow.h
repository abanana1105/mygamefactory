#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include "GameView.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupUi();
    void setupStartScreen();
    void initializeGameView();

    void enableConveyorBeltPlacement();
    void enableMinerPlacement();
    void enableDustbinPlacement();
    void enableCutPlacement();
    void enableDoublePlacement();

    void updateText();
    void updateMine();
    void updateTrans();
    void updateCut();

    void setUpdateMineInterval();
    void setUpdateTransInterval();
    void setUpdateCutInterval();

    void allupcenter();
    void allupmineral();
    void allupvalue();

    void saveg();



private slots:
    void newGame();
    void useGlobalEnhancements();
    void loadGame();

private:

    GameView *gameView;

    QTimer* updatetextTimer;
    QTimer* updatemineTimer;
    QTimer* updatetransTimer;
    QTimer* updatecutTimer;

    QPushButton *transButton; // 传送带按钮
    QPushButton *minerButton;
    QPushButton *dustbinButton;
    QPushButton *cutButton;
    QPushButton *doubleButton;

    QPushButton *upmineButton;
    QPushButton *uptransButton;
    QPushButton *upcutButton;

    QPushButton *allupcenterButton;
    QPushButton *allupmineralButton;
    QPushButton *allupvalueButton;

    QPushButton *newGameButton;
    QPushButton *useGlobalEnhancementsButton;
    QPushButton *loadGameButton;

    QPushButton *save;
};

#endif // MAINWINDOW_H



