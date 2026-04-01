#ifndef GAME_APP_QT_MAINWINDOW_H
#define GAME_APP_QT_MAINWINDOW_H

#include <utility>
#include <vector>
#include <QMainWindow>
#include <QTimer>
#include <QMediaPlayer>
#include "game/app/qt/tilemapper.h"
#include "game/core/engine.h"

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
        Attack,
        Damage,
        Death,
        Dodge,
        DodgeMove,
        Fall,
        Idle,
        Jump,
        Run,
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
    QPixmap getCurrentPlayerFrame() const;
    QPixmap getCurrentPlayerAttackFrame() const;
    QPixmap getCurrentPlayerDamageFrame() const;
    QPixmap getCurrentPlayerDeathFrame() const;
    QPixmap getCurrentPlayerDodgeFrame() const;
    QPixmap getCurrentPlayerDodgeMoveFrame() const;
    QPixmap getCurrentPlayerFallFrame() const;
    QPixmap getCurrentPlayerIdleFrame() const;
    QPixmap getCurrentPlayerJumpFrame() const;
    QPixmap getCurrentPlayerRunFrame() const;
    std::vector<QPixmap> loadFrames(const std::vector<QString>& resourcePaths) const;
private:
    Ui::MainWindow *ui_;

    TileMapper tileMapper_;
    Engine engine_;

    std::vector<QPixmap> playerAttackFrames_;
    int playerAttackFrameIndex_ = 0;
    double playerAttackFrameAccumulator_ = 0.0;
    double playerAttackFramesPerTick_ = 0.33;
    bool attackRequested_ = false;
    bool attackInProgress_ = false;

    std::vector<QPixmap> playerDamageFrames_;
    int playerDamageFrameIndex_ = 0;
    double playerDamageFrameAccumulator_ = 0.0;
    double playerDamageFramesPerTick_ = 0.20;
    bool damageRequested_ = false;
    bool damageInProgress_ = false;

    std::vector<QPixmap> playerDeathFrames_;
    int playerDeathFrameIndex_ = 0;
    double playerDeathFrameAccumulator_ = 0.0;
    double playerDeathFramesPerTick_ = 0.20;


    std::vector<QPixmap> playerDodgeMoveFrames_;
    int playerDodgeMoveFrameIndex_ = 0;
    double playerDodgeMoveFrameAccumulator_ = 0.0;
    double playerDodgeMoveFramesPerTick_ = 0.20;


    std::vector<QPixmap> playerDodgeFrames_;
    int playerDodgeFrameIndex_ = 0;
    double playerDodgeFrameAccumulator_ = 0.0;
    double playerDodgeFramesPerTick_ = 0.20;

    std::vector<QPixmap> playerFallFrames_;
    int playerFallFrameIndex_ = 0;
    double playerFallFrameAccumulator_ = 0.0;
    double playerFallFramesPerTick_ = 0.20;

    std::vector<QPixmap> playerIdleFrames_;
    int playerIdleFrameIndex_ = 0;
    double playerIdleFrameAccumulator_ = 0.0;
    double playerIdleFramesPerTick_ = 0.067;

    std::vector<QPixmap> playerJumpFrames_;
    int playerJumpFrameIndex_ = 0;
    double playerJumpFrameAccumulator_ = 0.0;
    double playerJumpFramesPerTick_ = 0.20;

    std::vector<QPixmap> playerRunFrames_;
    int playerRunFrameIndex_ = 0;
    double playerRunFrameAccumulator_ = 0.0;
    double playerRunFramesPerTick_ = 0.15;
    
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
#endif // GAME_APP_QT_MAINWINDOW_H
