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
      playerPixmapOrginal_(QString::fromUtf8(":/player.png")),
      backgroundOrginal_(QString::fromUtf8(":/background.png")),
      bulletOriginal_(QString::fromUtf8(":/bullet.png")),
      enemyOriginal_(QString::fromUtf8(":/enemy.png"))
{
    ui_->setupUi(this);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::update);
    timer_.start(/*msec=*/100);

    QAudioOutput* audioOutput = new QAudioOutput(&player_);
    player_.setAudioOutput(audioOutput);
    player_.setSource(QUrl("qrc:/music.mp3"));
    audioOutput->setVolume(50);
    player_.play();

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
    if (engine_.isPlayerAlive())
    {
        auto playerPixmap = getRotatedPlayerImage();
        auto [playerPositionTopLeft, playerPositionBottomRight] = calculatePlayerPosition();
        QRect playerPosition(playerPositionTopLeft, playerPositionBottomRight);
        painter.drawPixmap(playerPosition, playerPixmap);
    }
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
    engine_.movePlayerDown();
}

void MainWindow::onPressDown()
{
    engine_.movePlayerUp();
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
    else if (event->key() == Qt::Key_Up)
    {
        onPressUp();
    }
    else if (event->key() == Qt::Key_Down)
    {
        onPressDown();
    }
    else if (event->key() == Qt::Key_Left)
    {
        onPressLeft();
    }
    else if (event->key() == Qt::Key_Right)
    {
        onPressRight();
    }
    else if (event->key() == Qt::Key_Space)
    {
        onPressShoot();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::update()
{
    engine_.update();
    redrawView();
}

QPixmap MainWindow::getRotatedPlayerImage() const
{
    int rotationAngle = 0;
    switch(engine_.playerDirection())
    {
    case Direction::UP:
        rotationAngle = 90;
        break;
    case Direction::DOWN:
        rotationAngle = 270;
        break;
    case Direction::LEFT:
        rotationAngle = 180;
        break;
    case Direction::RIGHT:
        break;
    case Direction::UPPER_LEFT:
        break;
    case Direction::UPPER_RIGHT:
        break;
    case Direction::DOWNER_LEFT:
        break;
    case Direction::DOWNER_RIGHT:
        break;
    case Direction::CENTER:
        break;
    case Direction::INMOVABLE:
        break;
    case Direction::UNKNOWN:
        break;
    }
    QPixmap playerPixmapCopy(playerPixmapOrginal_);
    QTransform trans = QTransform().rotate(rotationAngle);
    return playerPixmapCopy.transformed(trans);
}
