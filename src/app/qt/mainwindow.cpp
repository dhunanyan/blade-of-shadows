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
#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      engine_(38, 25),
      backgroundOrginal_(QString::fromUtf8(":/background.png")),
      bulletOriginal_(QString::fromUtf8(":/bullet.png")),
      enemyOriginal_(QString::fromUtf8(":/enemy.png"))
{
    ui_->setupUi(this);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::update);
    timer_.start(/*msec=*/100);

    QAudioOutput* audioOutput = new QAudioOutput(&player_);
    player_.setAudioOutput(audioOutput);
    // player_.setSource(QUrl("qrc:/music.mp3"));
    audioOutput->setVolume(50);
    player_.play();

    playerIdleFrames_ = loadFrames({
        QString::fromUtf8(":/player/idle/01.png"),
        QString::fromUtf8(":/player/idle/02.png"),
        QString::fromUtf8(":/player/idle/03.png"),
        QString::fromUtf8(":/player/idle/04.png"),
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

    playerAttackFrames_ = loadFrames({
        QString::fromUtf8(":/player/attack/01.png"),
        QString::fromUtf8(":/player/attack/02.png"),
        QString::fromUtf8(":/player/attack/03.png"),
        QString::fromUtf8(":/player/attack/04.png"),
        QString::fromUtf8(":/player/attack/05.png"),
        QString::fromUtf8(":/player/attack/06.png"),
        QString::fromUtf8(":/player/attack/07.png"),
        QString::fromUtf8(":/player/attack/08.png"),
    });

    playerDamageFrames_ = loadFrames({
        QString::fromUtf8(":/player/damage/01.png"),
        QString::fromUtf8(":/player/damage/02.png"),
        QString::fromUtf8(":/player/damage/03.png"),
        QString::fromUtf8(":/player/damage/04.png"),
    });
}

MainWindow::~MainWindow()
{
    delete ui_;
}


void MainWindow::redrawView()
{
    QSize targetSize = ui_->background->size();
    if (targetSize.isEmpty())
    {
        targetSize = backgroundOrginal_.size();
    }

    QPixmap newBackground = backgroundOrginal_.scaled(
        targetSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );
    QPainter painter(&newBackground);
    if (!engine_.isPlayerAlive())
    {
        painter.drawRect(newBackground.rect());
    }

    drawPlayer(painter);
    drawShoots(painter);
    drawEnemies(painter);

    ui_->background->setPixmap(newBackground);
}

void MainWindow::drawPlayer(QPainter& painter)
{
  if (!engine_.isPlayerAlive()) return;

  QPixmap frame = getRotatedPlayerImage();
  if (frame.isNull()) return;


  auto [topLeft, bottomRight] = calculatePlayerPosition();
  const QPoint cellSize = bottomRight - topLeft + QPoint(1, 1);

  const QSize targetSize(cellSize.x() * playerScale_, cellSize.y() * playerScale_);

  QPoint drawTopLeft = topLeft - QPoint(
    (targetSize.width() - cellSize.x()) / 2,
    (targetSize.height() - cellSize.y()) / 2
  );
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

QPixmap MainWindow::getCurrentPlayerIdleFrame() const
{
    if (playerIdleFrames_.empty()) return QPixmap();
    const std::size_t idx =
        static_cast<std::size_t>(playerIdleFrameIndex_ % static_cast<int>(playerIdleFrames_.size()));
    return playerIdleFrames_[idx];
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
    const int renderWidth = std::max(1, ui_->background->width());
    const int renderHeight = std::max(1, ui_->background->height());

    auto newX = position.x_ * renderWidth / engine_.stageWidthCells();
    auto newY = position.y_ * renderHeight / engine_.stageHeightCells();

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
    // Side-scroller mode: vertical movement disabled until jump/gravity is implemented.
}

void MainWindow::onPressDown()
{
    // Side-scroller mode: vertical movement disabled until jump/gravity is implemented.
}
void MainWindow::onPressLeft()
{
    engine_.movePlayerLeft();
}
void MainWindow::onPressRight()
{
    engine_.movePlayerRight();
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
    if (isLeftPressed_)
    {
        engine_.movePlayerLeft();
        return;
    }
    if (isRightPressed_)
    {
        engine_.movePlayerRight();
        return;
    }
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
    case PlayerState::Idle:
        playerIdleFrameIndex_ = 0;
        playerIdleFrameAccumulator_ = 0.0;
        break;
    case PlayerState::Run:
        playerRunFrameIndex_ = 0;
        playerRunFrameAccumulator_ = 0.0;
        break;
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
  case PlayerState::Idle:
    advanceLooping(
      playerIdleFrameIndex_,
      static_cast<int>(playerIdleFrames_.size()),
      playerIdleFrameAccumulator_,
      playerIdleFramesPerTick_);
    break;
  case PlayerState::Run:
    advanceLooping(
      playerRunFrameIndex_,
      static_cast<int>(playerRunFrames_.size()),
      playerRunFrameAccumulator_,
      playerRunFramesPerTick_);
    break;
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
      playerDamageFramesPerTick_);
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
    case PlayerState::Run:
        return getCurrentPlayerRunFrame();
    case PlayerState::Damage:
        return getCurrentPlayerDamageFrame();
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
