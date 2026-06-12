#include <QAudioOutput>
#include <algorithm>
#include <cmath>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QPainter>
#include <QStandardPaths>
#include <QUrl>
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

    gameController_.setNewGameHandler([this](bool newCampaign) {
        if (newCampaign)
        {
            loadLevelById(campaignLevelIds_.front(), true);
        }
        else
        {
            resetCurrentLevel();
        }
    });
    gameController_.setNextLevelHandler([this]() { return loadNextLevel(); });
    gameController_.setSaveGameHandler([this]() { return saveGame(); });
    gameController_.setLoadGameHandler([this]() { return loadGame(); });
    gameController_.setLevelEditorHandler([this]() {
        if (QFile::exists(customLevelPath()))
        {
            tileMapper_.loadFromJsonFile(customLevelPath());
        }
        gameController_.engine().resetSession(
            tileMapper_.safePlayerStart(),
            {},
            {},
            std::nullopt);
        updateEditorStatus();
    });
    gameController_.setSaveLevelHandler([this]() { return saveCustomLevel(); });
    gameController_.setPlayEditedLevelHandler([this]() { resetCurrentLevel(); });
    gameController_.setSettingsChangedHandler([this](const GameSettings& settings) {
        settingsRepository_.save(settings);
        applySettings(settings);
    });
    gameController_.setSettings(settingsRepository_.load());

    if (!loadLevel(QString::fromUtf8(":/levels/level_01.json"), true))
    {
        qFatal("Could not load the initial level.");
    }

    if (!assets_.loadAll())
    {
        qWarning() << "Some assets were not loaded from resources.";
    }

    connect(&timer_, &QTimer::timeout, this, &MainWindow::update);
    timer_.start(16);

    player_.setAudioOutput(&audioOutput_);
    player_.setSource(QUrl(QString::fromUtf8("qrc:/music.mp3")));
    player_.setLoops(QMediaPlayer::Infinite);
    attackSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/attack.wav")));
    jumpSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/jump.wav")));
    coinSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/coin.wav")));
    hurtSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/hurt.wav")));
    applySettings(gameController_.settings());
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
        playerScale_,
        gameController_.mode() == GameMode::LevelEditor,
        gameController_.settings().language);
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
            if (gameController_.mode() == GameMode::LevelEditor &&
                !gameController_.menuView().visible)
            {
                handleEditorPointer(mouseEvent->position().toPoint(), mouseEvent->button());
            }
            else if (mouseEvent->button() == Qt::LeftButton)
            {
                handleMenuPointer(mouseEvent->position().toPoint(), true);
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (gameController_.mode() == GameMode::LevelEditor &&
        event->matches(QKeySequence::Save))
    {
        saveCustomLevel();
        event->accept();
        return;
    }
    if (gameController_.mode() == GameMode::LevelEditor && !event->isAutoRepeat())
    {
        const int tileCount = tileMapper_.tilesetColumns() * tileMapper_.tilesetRows();
        if ((event->key() == Qt::Key_Q || event->key() == Qt::Key_E) && tileCount > 0)
        {
            const int delta = event->key() == Qt::Key_Q ? -1 : 1;
            editorTileIndex_ = (editorTileIndex_ + delta + tileCount) % tileCount;
            updateEditorStatus();
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_F)
        {
            editorSolid_ = !editorSolid_;
            updateEditorStatus();
            event->accept();
            return;
        }
        if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_5)
        {
            editorTool_ = static_cast<EditorTool>(event->key() - Qt::Key_1);
            updateEditorStatus();
            event->accept();
            return;
        }
    }
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

    const QPoint mappedPoint = mapPointerToFrame(localPoint);

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

QPoint MainWindow::mapPointerToFrame(const QPoint& localPoint) const
{
    if (!displayedFrameRect_.isValid() ||
        sourceFrameSize_.isEmpty() ||
        !displayedFrameRect_.contains(localPoint))
    {
        return QPoint(-1, -1);
    }

    const int relativeX = localPoint.x() - displayedFrameRect_.x();
    const int relativeY = localPoint.y() - displayedFrameRect_.y();
    return QPoint(
        static_cast<int>(relativeX * (static_cast<double>(sourceFrameSize_.width()) / displayedFrameRect_.width())),
        static_cast<int>(relativeY * (static_cast<double>(sourceFrameSize_.height()) / displayedFrameRect_.height())));
}

void MainWindow::handleEditorPointer(const QPoint& localPoint, Qt::MouseButton button)
{
    const QPoint framePoint = mapPointerToFrame(localPoint);
    if (framePoint.x() < 0 || framePoint.y() < 0)
    {
        return;
    }

    const int worldX = framePoint.x() + static_cast<int>(std::lround(sceneRenderer_.cameraOffsetX()));
    const int gridX = worldX / tileSizePx_;
    const int gridY = framePoint.y() / tileSizePx_;
    if (button == Qt::RightButton)
    {
        tileMapper_.removeTile(gridX, gridY);
        tileMapper_.removeEntitiesAt(gridX, gridY);
    }
    else if (button == Qt::LeftButton)
    {
        switch (editorTool_)
        {
        case EditorTool::Tile:
        {
            const int columns = std::max(1, tileMapper_.tilesetColumns());
            tileMapper_.paintTile(
                gridX,
                gridY,
                editorTileIndex_ % columns,
                editorTileIndex_ / columns,
                editorSolid_);
            break;
        }
        case EditorTool::PlayerSpawn:
            tileMapper_.setPlayerStart(gridX, gridY);
            break;
        case EditorTool::Enemy:
            tileMapper_.toggleEnemySpawn(gridX, gridY);
            break;
        case EditorTool::Coin:
            tileMapper_.toggleCoinSpawn(gridX, gridY);
            break;
        case EditorTool::Exit:
            tileMapper_.setLevelExit(gridX, gridY);
            break;
        }
    }
}

bool MainWindow::loadLevel(const QString& resourcePath, bool resetSession)
{
    if (!tileMapper_.loadFromJsonResource(resourcePath))
    {
        qWarning() << "Could not load level:" << resourcePath;
        return false;
    }

    gameController_.engine().setSolidQuery([this](int x, int y) {
        return tileMapper_.isSolidAt(x, y);
    });
    if (resetSession)
    {
        resetCurrentLevel();
    }
    return true;
}

bool MainWindow::loadLevelById(const QString& levelId, bool resetSession)
{
    if (levelId == QString::fromUtf8("custom_level"))
    {
        if (!tileMapper_.loadFromJsonFile(customLevelPath()))
        {
            return false;
        }
        if (resetSession)
        {
            resetCurrentLevel();
        }
        return true;
    }
    return loadLevel(QString::fromUtf8(":/levels/%1.json").arg(levelId), resetSession);
}

bool MainWindow::loadNextLevel()
{
    const int currentIndex = campaignLevelIds_.indexOf(tileMapper_.levelId());
    if (currentIndex < 0 || currentIndex + 1 >= campaignLevelIds_.size())
    {
        return false;
    }
    return loadLevelById(campaignLevelIds_.at(currentIndex + 1), true);
}

void MainWindow::resetCurrentLevel()
{
    gameController_.engine().resetSession(
        tileMapper_.safePlayerStart(),
        tileMapper_.enemySpawns(),
        tileMapper_.coinSpawns(),
        tileMapper_.levelExit());
    playerPresentation_ = PlayerPresentation();
}

bool MainWindow::saveGame()
{
    const SaveGameData data{
        tileMapper_.levelId(),
        gameController_.engine().snapshot()};
    const bool saved = saveGameRepository_.save(data);
    qDebug() << (saved ? "Game saved to" : "Could not save game to")
             << saveGameRepository_.savePath();
    return saved;
}

bool MainWindow::loadGame()
{
    const auto saved = saveGameRepository_.load();
    if (!saved)
    {
        qWarning() << "No valid save game found.";
        return false;
    }

    if (!loadLevelById(saved->levelId, false))
    {
        return false;
    }
    gameController_.engine().restoreSnapshot(saved->snapshot);
    playerPresentation_ = PlayerPresentation();
    return true;
}

void MainWindow::applySettings(const GameSettings& settings)
{
    audioOutput_.setMuted(!settings.musicEnabled);
    audioOutput_.setVolume(static_cast<float>(settings.musicVolume) / 100.0f);
    const bool muted = !settings.soundEnabled;
    const float volume = static_cast<float>(settings.soundVolume) / 100.0f;
    for (QSoundEffect* effect : {&attackSound_, &jumpSound_, &coinSound_, &hurtSound_})
    {
        effect->setMuted(muted);
        effect->setVolume(volume);
    }
}

QString MainWindow::customLevelPath() const
{
    const QString root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(root).filePath(QString::fromUtf8("levels/custom_level.json"));
}

bool MainWindow::saveCustomLevel()
{
    const bool saved = tileMapper_.saveToJsonFile(customLevelPath());
    qDebug() << (saved ? "Custom level saved to" : "Could not save custom level to")
             << customLevelPath();
    ui_->statusbar->showMessage(
        saved ? QString::fromUtf8("Custom level saved")
              : QString::fromUtf8("Could not save custom level"),
        3000);
    return saved;
}

void MainWindow::updateEditorStatus()
{
    const int columns = std::max(1, tileMapper_.tilesetColumns());
    const QString toolNames[] = {
        QString::fromUtf8("Tile"),
        QString::fromUtf8("Player Spawn"),
        QString::fromUtf8("Enemy"),
        QString::fromUtf8("Coin"),
        QString::fromUtf8("Exit")};
    ui_->statusbar->showMessage(
        QString::fromUtf8("Tool: %1 | Tile (%2, %3) | %4 | 1-5 tools | Q/E tile | F collision | Ctrl+S save")
            .arg(toolNames[static_cast<int>(editorTool_)])
            .arg(editorTileIndex_ % columns)
            .arg(editorTileIndex_ / columns)
            .arg(editorSolid_ ? QString::fromUtf8("Solid") : QString::fromUtf8("Decorative")));
}

void MainWindow::update()
{
    const bool attackWasActive = gameController_.engine().isPlayerAttackInProgress();
    const float previousVelocityY = gameController_.engine().playerVelocityY();
    const int previousDoubleJumpTicks = gameController_.engine().playerDoubleJumpFxTicks();
    const int previousCoins = gameController_.engine().playerCoins();
    const int previousHealth = gameController_.engine().playerHealth();

    gameController_.tick();

    if (!attackWasActive && gameController_.engine().isPlayerAttackInProgress())
    {
        attackSound_.play();
    }
    if ((previousVelocityY >= 0.0f && gameController_.engine().playerVelocityY() < 0.0f) ||
        gameController_.engine().playerDoubleJumpFxTicks() > previousDoubleJumpTicks)
    {
        jumpSound_.play();
    }
    if (gameController_.engine().playerCoins() > previousCoins)
    {
        coinSound_.play();
    }
    if (gameController_.engine().playerHealth() < previousHealth)
    {
        hurtSound_.play();
    }

    playerPresentation_.update(gameController_.input(), gameController_.engine(), assets_);
    redrawView();

    if (gameController_.shouldQuit())
    {
        qDebug() << "Quit requested from menu.";
        close();
        qApp->quit();
    }
}
