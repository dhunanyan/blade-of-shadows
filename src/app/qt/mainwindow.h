#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <utility>
#include <QMainWindow>
#include <QTimer>
#include <QMediaPlayer>
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

private:  // methods:
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
private:  // fields:
    Ui::MainWindow *ui_;

    Engine engine_;

    QPixmap playerAttackSheet_;
    int playerAttackFrameIndex_ = 0;
    int playerAttackFrameCount_ = 7;
    int playerAttackFrameWidth_ = 96;
    int playerAttackFrameHeight_ = 96;
    double playerAttackFrameAccumulator_ = 0.0;
    double playerAttackFramesPerTick_ = 2;
    bool attackRequested_ = false;
    bool attackInProgress_ = false;

    QPixmap playerDamageSheet_;
    int playerDamageFrameIndex_ = 0;
    int playerDamageFrameCount_ = 4;
    int playerDamageFrameWidth_ = 96;
    int playerDamageFrameHeight_ = 96;
    double playerDamageFrameAccumulator_ = 0.0;
    double playerDamageFramesPerTick_ = 1.2;

    QPixmap playerIdleSheet_;
    int playerIdleFrameIndex_ = 0;
    int playerIdleFrameCount_ = 10;
    int playerIdleFrameWidth_ = 96;
    int playerIdleFrameHeight_ = 96;
    double playerIdleFrameAccumulator_ = 0.0;
    double playerIdleFramesPerTick_ = 0.4;

    QPixmap playerRunSheet_;
    int playerRunFrameIndex_ = 0;
    int playerRunFrameCount_ = 16;
    int playerRunFrameWidth_ = 96;
    int playerRunFrameHeight_ = 96;
    double playerRunFrameAccumulator_ = 0.0;
    double playerRunFramesPerTick_ = 0.9;
    
    QPixmap backgroundOrginal_;
    QPixmap bulletOriginal_;
    QPixmap enemyOriginal_;

    QTimer timer_;

    QMediaPlayer player_;

    bool isUpPressed_ = false;
    bool isDownPressed_ = false;
    bool isLeftPressed_ = false;
    bool isRightPressed_ = false;
    bool isShootPressed_ = false;

    PlayerState currentPlayerState_ = PlayerState::Idle;
};
#endif // MAINWINDOW_H
