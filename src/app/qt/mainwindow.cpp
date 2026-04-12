#include <QAudioOutput>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMediaPlayer>
#include "game/app/qt/mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      gameController_(60, 25)
{
    ui_->setupUi(this);
    ui_->background->setScaledContents(false);

    tileMapper_.loadFromJsonResource(QString::fromUtf8(":/levels/sample_level.json"));
    gameController_.engine().setSolidQuery([this](int x, int y) {
        return tileMapper_.isSolidAt(x, y);
    });

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
        tileSizePx_,
        playerScale_);
    ui_->background->setPixmap(frame);
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

    gameController_.onKeyEvent(event->key(), true, event->isAutoRepeat());
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    gameController_.onKeyEvent(event->key(), false, event->isAutoRepeat());
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::update()
{
    playerPresentation_.update(gameController_.input(), assets_);
    gameController_.tick();
    redrawView();
}
