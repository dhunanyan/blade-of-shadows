#include <QAudioOutput>
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QPainter>
#include "game/core/position.h"
#include "game/app/qt/mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      gameController_(60, 25)
{
    ui_->setupUi(this);
    ui_->background->setScaledContents(false);
    setMouseTracking(true);
    ui_->background->setMouseTracking(true);
    ui_->background->installEventFilter(this);

    tileMapper_.loadFromJsonResource(QString::fromUtf8(":/levels/sample_level.json"));
    gameController_.engine().setSolidQuery([this](int x, int y) {
        return tileMapper_.isSolidAt(x, y);
    });
    if (tileMapper_.hasPlayerStart())
    {
        const int startX = tileMapper_.playerStartX();
        int startY = tileMapper_.playerStartY();

        // Resolve spawn to a valid standing cell:
        // 1) move up if configured cell is solid,
        // 2) then drop down until standing on top of solid ground.
        while (startY > 0 && tileMapper_.isSolidAt(startX, startY))
        {
            --startY;
        }
        while (startY + 1 < tileMapper_.levelHeight() && !tileMapper_.isSolidAt(startX, startY + 1))
        {
            ++startY;
        }
        if (tileMapper_.isSolidAt(startX, startY))
        {
            --startY;
        }
        if (startY < 0)
        {
            startY = 0;
        }

        const Position startPosition(
            static_cast<std::size_t>(startX),
            static_cast<std::size_t>(startY));
        gameController_.engine().setPlayerPosition(startPosition);
        gameController_.engine().setPlayerPixelX(static_cast<float>(startX * tileSizePx_));
        gameController_.engine().setPlayerPixelY(static_cast<float>(startY * tileSizePx_));
    }

    if (!assets_.loadAll())
    {
        qWarning() << "Some assets were not loaded from resources.";
    }

    connect(&timer_, &QTimer::timeout, this, &MainWindow::update);
    timer_.start(16);

    QAudioOutput* audioOutput = new QAudioOutput(&player_);
    player_.setAudioOutput(audioOutput);
    audioOutput->setVolume(50);
    player_.play();
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::redrawView()
{
    const QPixmap frame = sceneRenderer_.renderFrame(
        gameController_.engine(),
        tileMapper_,
        assets_,
        playerPresentation_,
        gameController_.menuView(),
        tileSizePx_,
        playerScale_);
    sourceFrameSize_ = frame.size();

    const QSize viewportSize = ui_->background->size();
    const QPixmap scaledFrame = frame.scaled(
        viewportSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    QPixmap canvas(viewportSize);
    canvas.fill(Qt::black);

    const int offsetX = (viewportSize.width() - scaledFrame.width()) / 2;
    const int offsetY = (viewportSize.height() - scaledFrame.height()) / 2;
    displayedFrameRect_ = QRect(offsetX, offsetY, scaledFrame.width(), scaledFrame.height());

    {
        QPainter painter(&canvas);
        painter.drawPixmap(displayedFrameRect_.topLeft(), scaledFrame);
    }
    ui_->background->setPixmap(canvas);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui_->background)
    {
        if (event->type() == QEvent::MouseMove)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            handleMenuPointer(mouseEvent->position().toPoint(), false);
        }
        else if (event->type() == QEvent::MouseButtonPress)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                handleMenuPointer(mouseEvent->position().toPoint(), true);
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    gameController_.onKeyEvent(event->key(), true, event->isAutoRepeat());
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    gameController_.onKeyEvent(event->key(), false, event->isAutoRepeat());
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    QMainWindow::mousePressEvent(event);
}

void MainWindow::handleMenuPointer(const QPoint& localPoint, bool activate)
{
    const MenuView menuView = gameController_.menuView();
    if (!menuView.visible)
    {
        return;
    }

    if (!displayedFrameRect_.isValid() || sourceFrameSize_.isEmpty())
    {
        return;
    }

    if (!displayedFrameRect_.contains(localPoint))
    {
        if (!activate)
        {
            gameController_.onMenuHover(-1);
        }
        return;
    }

    const int relativeX = localPoint.x() - displayedFrameRect_.x();
    const int relativeY = localPoint.y() - displayedFrameRect_.y();
    const int mappedX = static_cast<int>(relativeX * (static_cast<double>(sourceFrameSize_.width()) / displayedFrameRect_.width()));
    const int mappedY = static_cast<int>(relativeY * (static_cast<double>(sourceFrameSize_.height()) / displayedFrameRect_.height()));
    const QPoint mappedPoint(mappedX, mappedY);

    const int menuIndex = sceneRenderer_.menuItemAtPoint(menuView, sourceFrameSize_, mappedPoint);
    if (activate)
    {
        gameController_.onMenuClick(menuIndex);
    }
    else
    {
        gameController_.onMenuHover(menuIndex);
    }
}

void MainWindow::update()
{
    gameController_.tick();
    playerPresentation_.update(gameController_.input(), gameController_.engine(), assets_);
    redrawView();

    if (gameController_.shouldQuit())
    {
        qDebug() << "Quit requested from menu.";
        close();
        qApp->quit();
    }
}
