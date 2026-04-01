#include <cmath>
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "game/core/object.h"
#include "game/core/bullet.h"
#include "game/core/enemy.h"
#include "game/app/qt/mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      engine_(60, 25),
      backgroundLayer1_(QString::fromUtf8(":/background_layer_1.png")),
      backgroundLayer2_(QString::fromUtf8(":/background_layer_2.png")),
      backgroundLayer3_(QString::fromUtf8(":/background_layer_3.png")),
      bulletOriginal_(QString::fromUtf8(":/bullet.png")),
      enemyOriginal_(QString::fromUtf8(":/enemy.png"))
{
    ui_->setupUi(this);
    ui_->background->setScaledContents(false);
    tileMapper_.loadFromJsonResource(QString::fromUtf8(":/levels/sample_level.json"));
    engine_.setSolidQuery([this](int x, int y) {
        return tileMapper_.isSolidAt(x, y);
    });
    connect(&timer_, &QTimer::timeout, this, &MainWindow::update);
    timer_.start(16);

    QAudioOutput* audioOutput = new QAudioOutput(&player_);
    player_.setAudioOutput(audioOutput);
    audioOutput->setVolume(50);
    player_.play();

    playerAttackFrames_ = loadFrames({
        QString::fromUtf8(":/player/attack/01.png"),
        QString::fromUtf8(":/player/attack/02.png"),
        QString::fromUtf8(":/player/attack/03.png"),
        QString::fromUtf8(":/player/attack/04.png"),
        QString::fromUtf8(":/player/attack/05.png"),
        QString::fromUtf8(":/player/attack/06.png"),
    });

    playerDamageFrames_ = loadFrames({
        QString::fromUtf8(":/player/damage/01.png"),
        QString::fromUtf8(":/player/damage/02.png"),
        QString::fromUtf8(":/player/damage/03.png"),
        QString::fromUtf8(":/player/damage/04.png"),
        QString::fromUtf8(":/player/damage/05.png"),
        QString::fromUtf8(":/player/damage/06.png"),
        QString::fromUtf8(":/player/damage/07.png"),
        QString::fromUtf8(":/player/damage/08.png"),
    });

    playerDeathFrames_ = loadFrames({
        QString::fromUtf8(":/player/death/01.png"),
        QString::fromUtf8(":/player/death/02.png"),
        QString::fromUtf8(":/player/death/03.png"),
        QString::fromUtf8(":/player/death/04.png"),
    });

    playerDodgeFrames_ = loadFrames({
        QString::fromUtf8(":/player/dodge/01.png"),
    });

    playerDodgeMoveFrames_ = loadFrames({
        QString::fromUtf8(":/player/dodge-move/01.png"),
        QString::fromUtf8(":/player/dodge-move/02.png"),
        QString::fromUtf8(":/player/dodge-move/03.png"),
    });

    playerFallFrames_ = loadFrames({
        QString::fromUtf8(":/player/fall/01.png"),
        QString::fromUtf8(":/player/fall/02.png"),
        QString::fromUtf8(":/player/fall/03.png"),
        QString::fromUtf8(":/player/fall/04.png"),
        QString::fromUtf8(":/player/fall/05.png"),
        QString::fromUtf8(":/player/fall/06.png"),
        QString::fromUtf8(":/player/fall/07.png"),
        QString::fromUtf8(":/player/fall/08.png"),
    });

    playerIdleFrames_ = loadFrames({
        QString::fromUtf8(":/player/idle/01.png"),
        QString::fromUtf8(":/player/idle/02.png"),
        QString::fromUtf8(":/player/idle/03.png"),
        QString::fromUtf8(":/player/idle/04.png"),
        QString::fromUtf8(":/player/idle/05.png"),
        QString::fromUtf8(":/player/idle/06.png"),
    });

    playerJumpFrames_ = loadFrames({
        QString::fromUtf8(":/player/jump/01.png"),
        QString::fromUtf8(":/player/jump/02.png"),
        QString::fromUtf8(":/player/jump/03.png"),
        QString::fromUtf8(":/player/jump/04.png"),
        QString::fromUtf8(":/player/jump/05.png"),
        QString::fromUtf8(":/player/jump/06.png"),
        QString::fromUtf8(":/player/jump/07.png"),
        QString::fromUtf8(":/player/jump/08.png"),
    });

    playerRunFrames_ = loadFrames({
        QString::fromUtf8(":/player/run/01.png"),
        QString::fromUtf8(":/player/run/02.png"),
        QString::fromUtf8(":/player/run/03.png"),
        QString::fromUtf8(":/player/run/04.png"),
        QString::fromUtf8(":/player/run/05.png"),
        QString::fromUtf8(":/player/run/06.png"),
        QString::fromUtf8(":/player/run/07.png"),
        QString::fromUtf8(":/player/run/08.png"),
    });
}

MainWindow::~MainWindow()
{
    delete ui_;
}


void MainWindow::redrawView()
{
    const QSize targetSize(
        static_cast<int>(engine_.stageWidthCells()) * tileSizePx_,
        static_cast<int>(engine_.stageHeightCells()) * tileSizePx_);

    QPixmap composedBackground = backgroundLayer1_.scaled(
        targetSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );
    QPixmap backgroundLayer2 = backgroundLayer2_.scaled(
        targetSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );
    QPixmap backgroundLayer3 = backgroundLayer3_.scaled(
        targetSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );

    QPainter painter(&composedBackground);
    painter.drawPixmap(0, 0, backgroundLayer2);
    painter.drawPixmap(0, 0, backgroundLayer3);

    if (!engine_.isPlayerAlive())
    {
        painter.drawRect(composedBackground.rect());
    }

    tileMapper_.render(painter, tileSizePx_);
    drawPlayer(painter);
    drawShoots(painter);
    drawEnemies(painter);

    ui_->background->setPixmap(composedBackground);
}

void MainWindow::drawPlayer(QPainter& painter)
{
  if (!engine_.isPlayerAlive()) return;

  QPixmap frame = getRotatedPlayerImage();
  if (frame.isNull()) return;


  auto [topLeft, bottomRight] = calculatePlayerPosition();
  const QPoint cellSize = bottomRight - topLeft + QPoint(1, 1);

  const QSize targetSize(cellSize.x() * playerScale_, cellSize.y() * playerScale_);

  const int playerX = static_cast<int>(std::lround(engine_.playerPixelX()));
  const int playerY = static_cast<int>(std::lround(engine_.playerPixelY()));
  const int drawX = playerX - (targetSize.width() - cellSize.x()) / 2;
  const int drawY = playerY + cellSize.y() - targetSize.height();
  QPoint drawTopLeft(drawX, drawY);
  QRect targetRect(drawTopLeft, targetSize);
  painter.drawPixmap(targetRect, frame);
}

void MainWindow::drawShoots(QPainter& painter)
{
    for (const Bullet& shoot : engine_.bullets())
    {
        auto [shootPositionTopLeft, shootPositionBottomRight] = position2PairOfQPoints(shoot.position());
        auto cellSize = shootPositionBottomRight - shootPositionTopLeft;
        auto shootSize = cellSize / 3;
        shootPositionTopLeft += shootSize;
        shootPositionBottomRight -= shootSize;
        QRect shootPosition(shootPositionTopLeft, shootPositionBottomRight);
        painter.drawPixmap(shootPosition, bulletOriginal_);
    }
}

void MainWindow::drawEnemies(QPainter& painter)
{
    for (const auto& enemy : engine_.enemies())
    {
        auto [enemyPositionTopLeft, enemyPositionBottomRight] = position2PairOfQPoints(enemy->position());
        QRect enemyPosition(enemyPositionTopLeft, enemyPositionBottomRight);
        painter.drawPixmap(enemyPosition, enemyOriginal_);

        drawLifeBarAboveEnemy(painter, *enemy);
    }
}

void MainWindow::drawLifeBarAboveEnemy(QPainter& painter, const Enemy& enemy)
{
    auto [enemyPositionTopLeft, enemyPositionBottomRight] = position2PairOfQPoints(enemy.position());

    auto cellSize = enemyPositionBottomRight - enemyPositionTopLeft;
    auto lifeBarLength = abs(cellSize.x() * enemy.lifePercent() / 100);
    QPoint lifeBarPositionBottomRight(enemyPositionTopLeft.x()+lifeBarLength,
                enemyPositionTopLeft.y()+cellSize.y()/10);

    painter.setBrush(Qt::red);
    painter.drawRect(QRect{enemyPositionTopLeft, lifeBarPositionBottomRight});
}

QPixmap MainWindow::getCurrentPlayerAttackFrame() const
{
    if (playerAttackFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerAttackFrameIndex_ % static_cast<int>(playerAttackFrames_.size()));
    return playerAttackFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerDamageFrame() const
{
    if (playerDamageFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerDamageFrameIndex_ % static_cast<int>(playerDamageFrames_.size()));
    return playerDamageFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerDeathFrame() const
{
    if (playerDeathFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerDeathFrameIndex_ % static_cast<int>(playerDeathFrames_.size()));
    return playerDeathFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerDodgeFrame() const
{
    if (playerDodgeFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerDodgeFrameIndex_ % static_cast<int>(playerDodgeFrames_.size()));
    return playerDodgeFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerDodgeMoveFrame() const
{
    if (playerDodgeMoveFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerDodgeMoveFrameIndex_ % static_cast<int>(playerDodgeMoveFrames_.size()));
    return playerDodgeMoveFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerFallFrame() const
{
    if (playerFallFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerFallFrameIndex_ % static_cast<int>(playerFallFrames_.size()));
    return playerFallFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerIdleFrame() const
{
    if (playerIdleFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerIdleFrameIndex_ % static_cast<int>(playerIdleFrames_.size()));
    return playerIdleFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerJumpFrame() const
{
    if (playerJumpFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerJumpFrameIndex_ % static_cast<int>(playerJumpFrames_.size()));
    return playerJumpFrames_[idx];
}

QPixmap MainWindow::getCurrentPlayerRunFrame() const
{
    if (playerRunFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerRunFrameIndex_ % static_cast<int>(playerRunFrames_.size()));
    return playerRunFrames_[idx];
}

std::pair<QPoint,QPoint> MainWindow::calculatePlayerPosition() const
{
    return position2PairOfQPoints(engine_.playerPosition());
}

QPoint MainWindow::position2QPoint(Position position) const
{
    const int newX = static_cast<int>(position.x_) * tileSizePx_;
    const int newY = static_cast<int>(position.y_) * tileSizePx_;
    return QPoint(newX, newY);
}
std::pair<QPoint,QPoint> MainWindow::position2PairOfQPoints(Position position) const
{
    auto positionFrom = position2QPoint(position);
    auto positionTo = position2QPoint(position.moveUpRight())-QPoint(1,1);
    return {positionFrom, positionTo};
}


void MainWindow::onPressUp()
{
    // TODO: Side-scroller mode: vertical movement disabled until jump/gravity is implemented.
}

void MainWindow::onPressDown()
{
    // TODO: Side-scroller mode: vertical movement disabled until jump/gravity is implemented.
}
void MainWindow::onPressLeft()
{
    isLeftPressed_ = true;
    updateMovement();
    isLeftPressed_ = false;
}
void MainWindow::onPressRight()
{
    isRightPressed_ = true;
    updateMovement();
    isRightPressed_ = false;
}

void MainWindow::onPressShoot()
{
    engine_.playerShoots();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        qDebug() << "Esc pressed, goodbye";
        close();
        qApp->quit();
        return;
    }

    setKeyState(event->key(), true);
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat())
    {
        attackRequested_ = true;
    }

    if (!event->isAutoRepeat())
    {
        processInput();
        redrawView();
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    setKeyState(event->key(), false);
    if (!event->isAutoRepeat())
    {
        processInput();
        redrawView();
    }

    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::update()
{
    processInput();
    setPlayerState(resolvePlayerState());
    updatePlayerAnimationFrame();

    engine_.update();
    redrawView();
}

void MainWindow::processInput()
{
    updateMovement();
}

void MainWindow::updateMovement()
{
    int moveIntentX = 0;
    if (isLeftPressed_ && !isRightPressed_)
    {
        moveIntentX = -1;
    }
    else if (isRightPressed_ && !isLeftPressed_)
    {
        moveIntentX = 1;
    }

    engine_.setPlayerMoveIntentX(moveIntentX);
}

void MainWindow::setKeyState(int key, bool isPressed)
{
    switch (key)
    {
    case Qt::Key_Up:
        isUpPressed_ = isPressed;
        break;
    case Qt::Key_Down:
        isDownPressed_ = isPressed;
        break;
    case Qt::Key_Left:
        isLeftPressed_ = isPressed;
        break;
    case Qt::Key_Right:
        isRightPressed_ = isPressed;
        break;
    case Qt::Key_Space:
        isShootPressed_ = isPressed;
        break;
    default:
        break;
    }
}

MainWindow::PlayerState MainWindow::resolvePlayerState() const
{
    if (attackInProgress_ || attackRequested_ || isShootPressed_) return PlayerState::Attack;

    if ((isLeftPressed_ || isRightPressed_) && isDownPressed_) return PlayerState::DodgeMove;

    if(isDownPressed_) return PlayerState::Dodge;

    if(isUpPressed_) return PlayerState::Jump;

    if (isLeftPressed_ || isRightPressed_) return PlayerState::Run;

    return PlayerState::Idle;
}

void MainWindow::setPlayerState(PlayerState nextState)
{
    if (currentPlayerState_ == nextState)
    {
        if (currentPlayerState_ == PlayerState::Attack && !attackInProgress_ &&
            (attackRequested_ || isShootPressed_))
        {
            playerAttackFrameIndex_ = 0;
            playerAttackFrameAccumulator_ = 0.0;
            attackInProgress_ = true;
            attackRequested_ = false;
            engine_.playerShoots();
        }
        return;
    }

    currentPlayerState_ = nextState;
    switch (currentPlayerState_)
    {
    case PlayerState::Attack:
        playerAttackFrameIndex_ = 0;
        playerAttackFrameAccumulator_ = 0.0;
        attackInProgress_ = true;
        attackRequested_ = false;
        engine_.playerShoots();
        break;
    case PlayerState::Damage:
        playerDamageFrameIndex_ = 0;
        playerDamageFrameAccumulator_ = 0.0;
        break;
    case PlayerState::Death:
        playerDeathFrameIndex_ = 0;
        playerDeathFrameAccumulator_ = 0.0;
        break;
    case PlayerState::Dodge:
        playerDodgeFrameIndex_ = 0;
        playerDodgeFrameAccumulator_ = 0.0;
        break;
    case PlayerState::DodgeMove:
        playerDodgeMoveFrameIndex_ = 0;
        playerDodgeMoveFrameAccumulator_ = 0.0;
        break;
    case PlayerState::Fall:
        playerFallFrameIndex_ = 0;
        playerFallFrameAccumulator_ = 0.0;
        break;
    case PlayerState::Idle:
        playerIdleFrameIndex_ = 0;
        playerIdleFrameAccumulator_ = 0.0;
        break;
    case PlayerState::Jump:
        playerJumpFrameIndex_ = 0;
        playerJumpFrameAccumulator_ = 0.0;
        break;
    case PlayerState::Run:
        playerRunFrameIndex_ = 0;
        playerRunFrameAccumulator_ = 0.0;
        break;
    }
}

void MainWindow::updatePlayerAnimationFrame()
{
  auto advanceLooping = [](int& frameIndex, int frameCount, double& accumulator, double framesPerTick)
  {
    if (frameCount <= 0 || framesPerTick <= 0.0) return;

    accumulator += framesPerTick;
    while (accumulator >= 1.0)
    {
        frameIndex = (frameIndex + 1) % frameCount;
        accumulator -= 1.0;
    }
  };

  switch (currentPlayerState_)
  {
  case PlayerState::Attack:
  {
    if (playerAttackFrames_.empty() || playerAttackFramesPerTick_ <= 0.0)
    {
      attackInProgress_ = false;
      break;
    }
    const int attackFrameCount = static_cast<int>(playerAttackFrames_.size());
    playerAttackFrameAccumulator_ += playerAttackFramesPerTick_;

    while (playerAttackFrameAccumulator_ >= 1.0)
    {
      playerAttackFrameAccumulator_ -= 1.0;
      if (playerAttackFrameIndex_ >= attackFrameCount - 1)
      {
        attackInProgress_ = false;
        break;
      }
      ++playerAttackFrameIndex_;
    }
    break;
  }
  case PlayerState::Damage:
    advanceLooping(
      playerDamageFrameIndex_,
      static_cast<int>(playerDamageFrames_.size()),
      playerDamageFrameAccumulator_,
      playerDamageFramesPerTick_
    );
    break;
  case PlayerState::Death:
    advanceLooping(
      playerDeathFrameIndex_,
      static_cast<int>(playerDeathFrames_.size()),
      playerDeathFrameAccumulator_,
      playerDeathFramesPerTick_
    );
    break;
  case PlayerState::Dodge:
    advanceLooping(
      playerDodgeFrameIndex_,
      static_cast<int>(playerDodgeFrames_.size()),
      playerDodgeFrameAccumulator_,
      playerDodgeFramesPerTick_
    );
    break;
  case PlayerState::DodgeMove:
    advanceLooping(
      playerDodgeMoveFrameIndex_,
      static_cast<int>(playerDodgeMoveFrames_.size()),
      playerDodgeMoveFrameAccumulator_,
      playerDodgeMoveFramesPerTick_
    );
    break;
  case PlayerState::Fall:
    advanceLooping(
      playerFallFrameIndex_,
      static_cast<int>(playerFallFrames_.size()),
      playerFallFrameAccumulator_,
      playerFallFramesPerTick_
    );
    break;
  case PlayerState::Idle:
    advanceLooping(
      playerIdleFrameIndex_,
      static_cast<int>(playerIdleFrames_.size()),
      playerIdleFrameAccumulator_,
      playerIdleFramesPerTick_
    );
    break;
  case PlayerState::Jump:
    advanceLooping(
      playerJumpFrameIndex_,
      static_cast<int>(playerJumpFrames_.size()),
      playerJumpFrameAccumulator_,
      playerJumpFramesPerTick_
    );
    break;
  case PlayerState::Run:
    advanceLooping(
      playerRunFrameIndex_,
      static_cast<int>(playerRunFrames_.size()),
      playerRunFrameAccumulator_,
      playerRunFramesPerTick_
    );
    break;
  }
}

std::vector<QPixmap> MainWindow::loadFrames(const std::vector<QString>& resourcePaths) const
{
    std::vector<QPixmap> frames;
    frames.reserve(resourcePaths.size());
    for (const auto& path : resourcePaths)
    {
        QPixmap frame(path);
        if (!frame.isNull())
        {
            frames.push_back(frame);
        }
    }
    return frames;
}

QPixmap MainWindow::getCurrentPlayerFrame() const
{
    switch (currentPlayerState_)
    {
    case PlayerState::Attack:
        return getCurrentPlayerAttackFrame();
    case PlayerState::Damage:
        return getCurrentPlayerDamageFrame();
    case PlayerState::Death:
        return getCurrentPlayerDeathFrame();
    case PlayerState::Dodge:
        return getCurrentPlayerDodgeFrame();
    case PlayerState::DodgeMove:
        return getCurrentPlayerDodgeMoveFrame();
    case PlayerState::Fall:
        return getCurrentPlayerFallFrame();
    case PlayerState::Jump:
        return getCurrentPlayerJumpFrame();
    case PlayerState::Run:
        return getCurrentPlayerRunFrame();
    case PlayerState::Idle:
    default:
        return getCurrentPlayerIdleFrame();
    }
}

QPixmap MainWindow::getRotatedPlayerImage() const
{
  QPixmap frame = getCurrentPlayerFrame();
  if (frame.isNull()) return QPixmap();

  const Direction dir = engine_.playerDirection();
  const bool faceLeft =
      dir == Direction::LEFT ||
      dir == Direction::UPPER_LEFT ||
      dir == Direction::DOWNER_LEFT;

  if (!faceLeft) return frame;
  return frame.transformed(QTransform().scale(-1, 1));
}
