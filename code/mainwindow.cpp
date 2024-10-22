#include "MainWindow.h"
#include "GameView.h"
#include "GameController.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QCursor>
#include <QApplication>
#include "global.h"
#include "DeliveryCenter.h"

GameController* globalGameController = nullptr;
GameMap* globalGameMap = nullptr;
DeliveryCenter* globalDeliveryCenter = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QMediaPlayer *bgm = new QMediaPlayer();
    QAudioOutput *audioOutput = new QAudioOutput();

    bgm->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0);  // 设置音量

    // 设置音频源
    bgm->setSource(QUrl("qrc:/resourses/music/bgm2.mp3"));

    // 连接播放完成信号到一个槽，用于重新开始播放
    QObject::connect(bgm, &QMediaPlayer::playbackStateChanged, [bgm](QMediaPlayer::PlaybackState state) {
        if (state == QMediaPlayer::StoppedState) {
            bgm->play();
        }
    });

    // 开始播放
    bgm->play();
    setupStartScreen();
}

void MainWindow::setupStartScreen() {
    QFont buttonFont; // 字体对象
    buttonFont.setPointSize(20); // 设置字体大小

    newGameButton = new QPushButton("新的游戏", this);
    newGameButton->setFont(buttonFont); // 应用字体
    newGameButton->setFixedSize(1000, 100); // 设置固定尺寸
    connect(newGameButton, &QPushButton::clicked, this, &MainWindow::newGame);

    useGlobalEnhancementsButton = new QPushButton("使用全局强化", this);
    useGlobalEnhancementsButton->setFont(buttonFont);
    useGlobalEnhancementsButton->setFixedSize(1000, 100);
    connect(useGlobalEnhancementsButton, &QPushButton::clicked, this, &MainWindow::useGlobalEnhancements);

    loadGameButton = new QPushButton("读取存档", this);
    loadGameButton->setFont(buttonFont);
    loadGameButton->setFixedSize(1000, 100); // 设置固定尺寸
    connect(loadGameButton, &QPushButton::clicked, this, &MainWindow::loadGame);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(newGameButton);
    layout->addWidget(useGlobalEnhancementsButton);
    layout->addWidget(loadGameButton);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::newGame() {
    initializeGameView();
    // 新游戏的初始化逻辑
    setupUi(); // 设置游戏视图和其余 UI
}

void MainWindow::useGlobalEnhancements() {
    initializeGameView();
    // 全局强化的逻辑
    globalGameController -> loadallGame();
    setupUi(); // 设置游戏视图和其余 UI
}

void MainWindow::loadGame() {
    initializeGameView();
    // 读取存档的逻辑
    globalGameController -> loadallGame();
    globalGameController -> loadGame();
    setupUi(); // 设置游戏视图和其余 UI
}

void MainWindow::initializeGameView() {
    globalGameController = new GameController(this);
    globalGameMap = globalGameController->getMap();
    gameView = new GameView(globalGameController, globalGameMap);

    globalDeliveryCenter -> createText();

    // 隐藏启动界面的按钮
    newGameButton->setVisible(false);
    useGlobalEnhancementsButton->setVisible(false);
    loadGameButton->setVisible(false);
}

/*MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    globalGameController = new GameController(this);
    globalGameMap = globalGameController->getMap();
    gameView = new GameView(globalGameController, globalGameMap);

    setupUi();

    globalDeliveryCenter -> createText();

    updatetextTimer = new QTimer(this);
    connect(updatetextTimer, &QTimer::timeout, this, &MainWindow::updateText);
    updatetextTimer->start(1);

    updatemineTimer = new QTimer(this);
    connect(updatemineTimer, &QTimer::timeout, this, &MainWindow::updateMine);
    updatemineTimer->start(1000);

    updatetransTimer = new QTimer(this);
    connect(updatetransTimer, &QTimer::timeout, this, &MainWindow::updateTrans);
    updatetransTimer->start(1000);

    updatecutTimer = new QTimer(this);
    connect(updatecutTimer, &QTimer::timeout, this, &MainWindow::updateCut);
    updatecutTimer->start(1000);
}*/

MainWindow::~MainWindow() {
    // 如果有需要，清理资源
}

void MainWindow::setupUi() {
    // 创建传送带按钮
    transButton = new QPushButton(QIcon(":/resourses/images/menu_trans.png"), "", this);
    connect(transButton, &QPushButton::clicked, this, &MainWindow::enableConveyorBeltPlacement);

    // 创建开采器按钮
    minerButton = new QPushButton(QIcon(":/resourses/images/menu_mine.png"), "", this);
    connect(minerButton, &QPushButton::clicked, this, &MainWindow::enableMinerPlacement);

    // 创建垃圾桶按钮
    dustbinButton = new QPushButton(QIcon(":/resourses/images/menu_dustbin.png"), "", this);
    connect(dustbinButton, &QPushButton::clicked, this, &MainWindow::enableDustbinPlacement);

    // 创建切割器按钮
    cutButton = new QPushButton(QIcon(":/resourses/images/menu_cut.png"), "", this);
    connect(cutButton, &QPushButton::clicked, this, &MainWindow::enableCutPlacement);

    // 创建加倍器按钮
    doubleButton = new QPushButton(QIcon(":/resourses/images/menu_double.png"), "", this);
    connect(doubleButton, &QPushButton::clicked, this, &MainWindow::enableDoublePlacement);

    upmineButton = new QPushButton(QIcon(":/resourses/images/upmine.png"), "", this);
    connect(upmineButton, &QPushButton::clicked, this, &MainWindow::setUpdateMineInterval);

    uptransButton = new QPushButton(QIcon(":/resourses/images/uptrans.png"), "", this);
    connect(uptransButton, &QPushButton::clicked, this, &MainWindow::setUpdateTransInterval);

    upcutButton = new QPushButton(QIcon(":/resourses/images/upcut.png"), "", this);
    connect(upcutButton, &QPushButton::clicked, this, &MainWindow::setUpdateCutInterval);

    allupcenterButton = new QPushButton(QIcon(":/resourses/images/allupcenter.png"), "", this);
    connect(allupcenterButton, &QPushButton::clicked, this, &MainWindow::allupcenter);

    allupmineralButton = new QPushButton(QIcon(":/resourses/images/allupmineral.png"), "", this);
    connect(allupmineralButton, &QPushButton::clicked, this, &MainWindow::allupmineral);

    allupvalueButton = new QPushButton(QIcon(":/resourses/images/allupvalue.png"), "", this);
    connect(allupvalueButton, &QPushButton::clicked, this, &MainWindow::allupvalue);

    save = new QPushButton(QIcon(":/resourses/images/save.jpg"), "", this);
    connect(save, &QPushButton::clicked, this, &MainWindow::saveg);
    // 创建水平布局并添加按钮
    QHBoxLayout *menuLayout = new QHBoxLayout;
    menuLayout->addWidget(transButton);  // 添加传送带按钮
    menuLayout->addWidget(minerButton);  // 添加开采器按钮
    menuLayout->addWidget(dustbinButton);  // 添加垃圾桶按钮
    menuLayout->addWidget(cutButton);  // 添加切割器按钮
    menuLayout->addWidget(doubleButton);  // 添加加倍器按钮
    menuLayout->addWidget(upmineButton);
    menuLayout->addWidget(uptransButton);
    menuLayout->addWidget(upcutButton);
    menuLayout->addWidget(allupcenterButton);
    menuLayout->addWidget(allupmineralButton);
    menuLayout->addWidget(allupvalueButton);
    menuLayout->addWidget(save);
    menuLayout->setAlignment(Qt::AlignCenter); // 将按钮居中对齐

    // 创建一个包含布局的小部件
    QWidget *menuWidget = new QWidget;
    menuWidget->setLayout(menuLayout);

    // 将包含按钮的小部件添加到状态栏中
    statusBar()->addPermanentWidget(menuWidget, 1);

    // 设置 GameView 为中心小部件
    setCentralWidget(gameView);

    updatetextTimer = new QTimer(this);
    connect(updatetextTimer, &QTimer::timeout, this, &MainWindow::updateText);
    updatetextTimer->start(1);

    updatemineTimer = new QTimer(this);
    connect(updatemineTimer, &QTimer::timeout, this, &MainWindow::updateMine);
    updatemineTimer->start(globalGameController->speedmine);

    updatetransTimer = new QTimer(this);
    connect(updatetransTimer, &QTimer::timeout, this, &MainWindow::updateTrans);
    updatetransTimer->start(globalGameController -> speed);

    updatecutTimer = new QTimer(this);
    connect(updatecutTimer, &QTimer::timeout, this, &MainWindow::updateCut);
    updatecutTimer->start(globalGameController -> speedcut);
}

void MainWindow::enableConveyorBeltPlacement() {
    gameView->setFocus();  // 确保 GameView 获得焦点
    gameView->enableConveyorBeltPlacement();
}

void MainWindow::enableMinerPlacement() {
    gameView->setFocus();  // 确保 GameView 获得焦点
    gameView->enableMinerPlacement();
}

void MainWindow::enableDustbinPlacement() {
    gameView->setFocus();  // 确保 GameView 获得焦点
    gameView->enableDustbinPlacement();
}

void MainWindow::enableCutPlacement() {
    gameView->setFocus();  // 确保 GameView 获得焦点
    gameView->enableCutPlacement();

}

void MainWindow::enableDoublePlacement() {
    gameView->setFocus();  // 确保 GameView 获得焦点
    gameView->enableDoublePlacement();

}

void MainWindow::updateMine() {
    GameMap* gameMap = globalGameMap;
    for (auto& row : gameMap->tiles) {
        for (auto& tile : row) {
            tile->updateMine();
        }
    }
}

void MainWindow::updateTrans() {
    GameMap* gameMap = globalGameMap;
    for (auto& row : gameMap->tiles) {
        for (auto& tile : row) {
            tile->updateTrans();
        }
    }
}

void MainWindow::updateCut() {
    GameMap* gameMap = globalGameMap;
    for (auto& row : gameMap->tiles) {
        for (auto& tile : row) {
            tile->updateCut();
        }
    }
}

void MainWindow::updateText() {
    globalDeliveryCenter -> updateText();
}

void MainWindow::setUpdateMineInterval() {
    if(globalDeliveryCenter -> upcount >= 1 && globalGameController -> speedmine == 1000) {
        globalGameController -> speedmine = 500;
        updatemineTimer->setInterval(globalGameController -> speedmine);
        globalDeliveryCenter -> upcount --;
    }
}

void MainWindow::setUpdateTransInterval() {
    if(globalDeliveryCenter -> upcount >= 1 && globalGameController -> speed == 1000) {
        globalGameController -> speed = 500;
        updatetransTimer->setInterval(globalGameController -> speed);
        globalDeliveryCenter -> upcount --;
    }
}

void MainWindow::setUpdateCutInterval() {
    if(globalDeliveryCenter -> upcount >= 1 &&  globalGameController -> speedcut == 1000) {
        globalGameController -> speedcut = 500;
        updatecutTimer->setInterval(globalGameController -> speedcut);
        globalDeliveryCenter -> upcount --;
    }
}

void MainWindow::allupcenter(){
    globalDeliveryCenter -> up_center();
}

void MainWindow::allupmineral(){
    globalDeliveryCenter -> up_mineral();
}

void MainWindow::allupvalue(){
    globalDeliveryCenter -> up_value();
}

void MainWindow::saveg() {
    globalGameController -> saveGame();
}


