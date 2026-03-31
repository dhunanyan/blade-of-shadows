#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <utility>
#include <vector>
#include <QMainWindow>
#include <QTimer>
#include <QMediaPlayer>
#include "game/core/engine.h"
#include "tilemapper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void redrawView();

public slots:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void onPressShoot();
    void onPressUp();
    void onPressDown();
    void onPressLeft();
    void onPressRight();

    void update();

private:
    enum class PlayerState
    {
        Idle,
        Run,
        Attack,
        Damage
    };

    void processInput();
    void updateMovement();
    void setKeyState(int key, bool isPressed);
    PlayerState resolvePlayerState() const;
    void setPlayerState(PlayerState nextState);
    void updatePlayerAnimationFrame();
    void drawPlayer(QPainter& painter);
    void drawShoots(QPainter& painter);
    void drawEnemies(QPainter& painter);

    QPixmap getRotatedPlayerImage() const;
    std::pair<QPoint,QPoint> calculatePlayerPosition() const;
    QPoint position2QPoint(Position position) const;
    std::pair<QPoint,QPoint> position2PairOfQPoints(Position position) const;
    void drawLifeBarAboveEnemy(QPainter &painter, const Enemy &enemy);
    QPixmap getCurrentPlayerIdleFrame() const;
    QPixmap getCurrentPlayerDamageFrame() const;
    QPixmap getCurrentPlayerAttackFrame() const;
    QPixmap getCurrentPlayerRunFrame() const;
    QPixmap getCurrentPlayerFrame() const;
    std::vector<QPixmap> loadFrames(const std::vector<QString>& resourcePaths) const;
private:  // fields:
    Ui::MainWindow *ui_;

    TileMapper tileMapper_;
    Engine engine_;

    std::vector<QPixmap> playerAttackFrames_;
    int playerAttackFrameIndex_ = 0;
    double playerAttackFrameAccumulator_ = 0.0;
    double playerAttackFramesPerTick_ = 2;
    bool attackRequested_ = false;
    bool attackInProgress_ = false;

    std::vector<QPixmap> playerDamageFrames_;
    int playerDamageFrameIndex_ = 0;
    double playerDamageFrameAccumulator_ = 0.0;
    double playerDamageFramesPerTick_ = 1.2;

    std::vector<QPixmap> playerIdleFrames_;
    int playerIdleFrameIndex_ = 0;
    double playerIdleFrameAccumulator_ = 0.0;
    double playerIdleFramesPerTick_ = 0.4;

    std::vector<QPixmap> playerRunFrames_;
    int playerRunFrameIndex_ = 0;
    double playerRunFrameAccumulator_ = 0.0;
    double playerRunFramesPerTick_ = 0.9;
    
    QPixmap backgroundLayer1_;
    QPixmap backgroundLayer2_;
    QPixmap backgroundLayer3_;
    QPixmap bulletOriginal_;
    QPixmap enemyOriginal_;

    QTimer timer_;

    QMediaPlayer player_;
    static constexpr int tileSizePx_ = 24;
    const int playerScale_ = 3;

    bool isUpPressed_ = false;
    bool isDownPressed_ = false;
    bool isLeftPressed_ = false;
    bool isRightPressed_ = false;
    bool isShootPressed_ = false;

    PlayerState currentPlayerState_ = PlayerState::Idle;
};
#endif // MAINWINDOW_H
