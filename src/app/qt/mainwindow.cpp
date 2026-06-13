#include <algorithm>
#include <cmath>
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QPainter>
#include <QUrl>
#include <QWheelEvent>
#include "game/core/position.h"
#include "game/app/qt/mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      gameController_(60, 25),
      session_(gameController_.engine())
{
    ui_->setupUi(this);
    ui_->background->setScaledContents(false);
    setMouseTracking(true);
    ui_->background->setMouseTracking(true);
    ui_->background->installEventFilter(this);

    gameController_.setNewGameHandler([this](bool newCampaign) {
        const bool loaded =
            newCampaign ? session_.startNewCampaign() : session_.restartLevel();
        if (loaded)
        {
            playerPresentation_ = PlayerPresentation();
            sceneRenderer_.resetCamera();
        }
        return loaded;
    });
    gameController_.setNextLevelHandler([this]() {
        const bool loaded = session_.loadNextLevel();
        if (loaded)
        {
            playerPresentation_ = PlayerPresentation();
            sceneRenderer_.resetCamera();
        }
        return loaded;
    });
    gameController_.setSaveGameHandler([this]() { return session_.saveGame(); });
    gameController_.setLoadGameHandler([this]() {
        const bool loaded = session_.loadGame();
        if (loaded)
        {
            playerPresentation_ = PlayerPresentation();
            sceneRenderer_.resetCamera();
        }
        return loaded;
    });
    gameController_.setLevelEditorHandler([this]() {
        const bool loaded = session_.enterEditor();
        if (loaded)
        {
            playerPresentation_ = PlayerPresentation();
            sceneRenderer_.resetCamera();
            updateEditorStatus();
        }
        return loaded;
    });
    gameController_.setSaveLevelHandler([this]() { return saveCustomLevel(); });
    gameController_.setPlayEditedLevelHandler([this]() {
        const bool loaded = session_.playEditedLevel();
        if (loaded)
        {
            playerPresentation_ = PlayerPresentation();
            sceneRenderer_.resetCamera();
        }
        return loaded;
    });
    gameController_.setLevelSelectionHandlers(
        [this]()
        {
            return session_.levelSelections();
        },
        [this](const std::string& levelId)
        {
            const bool loaded = session_.loadSelectableLevel(levelId);
            if (loaded)
            {
                playerPresentation_ = PlayerPresentation();
                sceneRenderer_.resetCamera();
            }
            return loaded;
        });
    gameController_.setSettingsChangedHandler([this](const GameSettings& settings) {
        settingsRepository_.save(settings);
        applySettings(settings);
    });
    gameController_.setMusicNavigationHandlers(
        [this]() { musicPlayer_.previous(); },
        [this]() { musicPlayer_.next(); });
    connect(
        &musicPlayer_,
        &MusicPlayer::currentTrackChanged,
        this,
        [this](const QString& name, int)
        {
            gameController_.setCurrentMusicTrack(name.toStdString());
        });
    gameController_.setCurrentMusicTrack(musicPlayer_.currentTrackName().toStdString());
    gameController_.setSettings(settingsRepository_.load());

    if (!session_.loadInitialLevel())
    {
        qFatal("Could not load the initial level.");
    }
    sceneRenderer_.resetCamera();

    if (!assets_.loadAll())
    {
        qWarning() << "Some assets were not loaded from resources.";
    }

    connect(&timer_, &QTimer::timeout, this, &MainWindow::update);
    timer_.start(16);

    attackSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/sfx/shoot-2.wav")));
    jumpSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/sfx/jump-1.wav")));
    doubleJumpSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/sfx/powerup-1.wav")));
    coinSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/sfx/coin-1.wav")));
    hurtSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/sfx/damage-1.wav")));
    deathSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/sfx/die-1.wav")));
    levelCompleteSound_.setSource(QUrl(QString::fromUtf8("qrc:/audio/sfx/level-complete-1.wav")));
    applySettings(gameController_.settings());
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::redrawView()
{
    QPixmap frame = sceneRenderer_.renderFrame(
        gameController_.engine(),
        session_.level(),
        assets_,
        playerPresentation_,
        gameController_.menuView(),
        session_.level().tileSizePx(),
        playerScale_,
        gameController_.mode() == GameMode::LevelEditor,
        gameController_.settings().language);
    if (gameController_.mode() == GameMode::LevelEditor &&
        !gameController_.menuView().visible)
    {
        QPainter palettePainter(&frame);
        editorPalette_.render(palettePainter, frame.size(), session_.level());
    }
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
            if (gameController_.mode() == GameMode::LevelEditor &&
                !gameController_.menuView().visible &&
                (mouseEvent->buttons() & (Qt::LeftButton | Qt::RightButton)))
            {
                const Qt::MouseButton button =
                    mouseEvent->buttons().testFlag(Qt::RightButton)
                        ? Qt::RightButton
                        : Qt::LeftButton;
                handleEditorPointer(mouseEvent->position().toPoint(), button);
            }
            else
            {
                handleMenuPointer(mouseEvent->position().toPoint(), false);
            }
        }
        else if (event->type() == QEvent::MouseButtonPress)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (gameController_.mode() == GameMode::LevelEditor &&
                !gameController_.menuView().visible)
            {
                lastEditedCell_ = QPoint(-1, -1);
                handleEditorPointer(mouseEvent->position().toPoint(), mouseEvent->button());
            }
            else if (mouseEvent->button() == Qt::LeftButton)
            {
                handleMenuPointer(mouseEvent->position().toPoint(), true);
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            lastEditedCell_ = QPoint(-1, -1);
        }
        else if (event->type() == QEvent::Wheel &&
                 gameController_.menuView().visible)
        {
            auto* wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->angleDelta().y() != 0)
            {
                gameController_.onMenuScroll(
                    wheelEvent->angleDelta().y() > 0 ? -1 : 1);
                wheelEvent->accept();
                return true;
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
    if (gameController_.mode() == GameMode::LevelEditor &&
        !gameController_.menuView().visible)
    {
        switch (event->key())
        {
        case Qt::Key_Left:
        case Qt::Key_A:
            editorCameraLeft_ = true;
            event->accept();
            return;
        case Qt::Key_Right:
        case Qt::Key_D:
            editorCameraRight_ = true;
            event->accept();
            return;
        case Qt::Key_Up:
        case Qt::Key_W:
            editorCameraUp_ = true;
            event->accept();
            return;
        case Qt::Key_Down:
        case Qt::Key_S:
            editorCameraDown_ = true;
            event->accept();
            return;
        default:
            break;
        }
    }
    if (gameController_.mode() == GameMode::LevelEditor && !event->isAutoRepeat())
    {
        if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_6)
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
    switch (event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_A:
        editorCameraLeft_ = false;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        editorCameraRight_ = false;
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        editorCameraUp_ = false;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        editorCameraDown_ = false;
        break;
    default:
        break;
    }
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

void MainWindow::wheelEvent(QWheelEvent* event)
{
    if (gameController_.menuView().visible && event->angleDelta().y() != 0)
    {
        gameController_.onMenuScroll(event->angleDelta().y() > 0 ? -1 : 1);
        event->accept();
        return;
    }
    QMainWindow::wheelEvent(event);
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

    if (editorPalette_.contains(framePoint, sourceFrameSize_))
    {
        if (button == Qt::LeftButton)
        {
            editorPalette_.handleClick(
                framePoint,
                sourceFrameSize_,
                session_.level());
            editorTool_ =
                editorPalette_.mode() == EditorPalette::Mode::Terrain
                    ? EditorTool::Tile
                    : EditorTool::Decoration;
        }
        lastEditedCell_ = QPoint(-1, -1);
        updateEditorStatus();
        return;
    }

    const int worldX = framePoint.x() + static_cast<int>(std::lround(sceneRenderer_.cameraOffsetX()));
    const int gridX = worldX / session_.level().tileSizePx();
    const int worldY = framePoint.y() + static_cast<int>(std::lround(sceneRenderer_.cameraOffsetY()));
    const int gridY = worldY / session_.level().tileSizePx();
    const QPoint editedCell(gridX, gridY);
    if (editedCell == lastEditedCell_)
    {
        return;
    }
    lastEditedCell_ = editedCell;

    if (button == Qt::RightButton)
    {
        session_.level().removeTile(gridX, gridY);
        session_.level().removeEntitiesAt(gridX, gridY);
    }
    else if (button == Qt::LeftButton)
    {
        switch (editorTool_)
        {
        case EditorTool::Tile:
            session_.level().paintTile(
                gridX,
                gridY,
                editorPalette_.tileX(),
                editorPalette_.tileY(),
                true);
            break;
        case EditorTool::Decoration:
            session_.level().placeDecoration(
                editorPalette_.decorationId(),
                gridX,
                gridY);
            break;
        case EditorTool::PlayerSpawn:
            session_.level().setPlayerStart(gridX, gridY);
            break;
        case EditorTool::Enemy:
            session_.level().toggleEnemySpawn(gridX, gridY);
            break;
        case EditorTool::Coin:
            session_.level().toggleCoinSpawn(gridX, gridY);
            break;
        case EditorTool::Exit:
            session_.level().setLevelExit(gridX, gridY);
            break;
        }
    }
}

void MainWindow::applySettings(const GameSettings& settings)
{
    musicPlayer_.setVolume(settings.musicVolume);
    musicPlayer_.setEnabled(settings.musicEnabled);
    const bool muted = !settings.soundEnabled;
    const float volume = static_cast<float>(settings.soundVolume) / 100.0f;
    for (QSoundEffect* effect : {
             &attackSound_,
             &jumpSound_,
             &doubleJumpSound_,
             &coinSound_,
             &hurtSound_,
             &deathSound_,
             &levelCompleteSound_})
    {
        effect->setMuted(muted);
        effect->setVolume(volume);
    }
}

bool MainWindow::saveCustomLevel()
{
    const bool saved = session_.saveCustomLevel();
    qDebug() << (saved ? "Custom level saved to" : "Could not save custom level to")
             << session_.customLevelPath();
    ui_->statusbar->showMessage(
        saved ? QString::fromUtf8("Custom level saved")
              : QString::fromUtf8("Could not save custom level"),
        3000);
    return saved;
}

void MainWindow::updateEditorStatus()
{
    const QString toolNames[] = {
        QString::fromUtf8("Tile"),
        QString::fromUtf8("Decoration"),
        QString::fromUtf8("Player Spawn"),
        QString::fromUtf8("Enemy"),
        QString::fromUtf8("Coin"),
        QString::fromUtf8("Exit")};
    ui_->statusbar->showMessage(
        QString::fromUtf8("Tool: %1 | %2 | Drag left: paint | Drag right: erase | WASD/arrows: camera | 1-6 tools | Ctrl+S save")
            .arg(toolNames[static_cast<int>(editorTool_)])
            .arg(editorPalette_.selectionLabel()));
}

void MainWindow::update()
{
    if (gameController_.mode() == GameMode::LevelEditor &&
        !gameController_.menuView().visible)
    {
        constexpr float editorCameraSpeed = 8.0f;
        const float deltaX =
            static_cast<float>(editorCameraRight_ - editorCameraLeft_) *
            editorCameraSpeed;
        const float deltaY =
            static_cast<float>(editorCameraDown_ - editorCameraUp_) *
            editorCameraSpeed;
        sceneRenderer_.panEditorCamera(deltaX, deltaY);
    }
    gameController_.tick();

    for (const GameEvent event : gameController_.engine().takeEvents())
    {
        switch (event)
        {
        case GameEvent::AttackStarted:
            attackSound_.play();
            break;
        case GameEvent::JumpStarted:
            jumpSound_.play();
            break;
        case GameEvent::DoubleJumpStarted:
            doubleJumpSound_.play();
            break;
        case GameEvent::CoinCollected:
            coinSound_.play();
            break;
        case GameEvent::PlayerDamaged:
            hurtSound_.play();
            break;
        case GameEvent::PlayerDefeated:
            deathSound_.play();
            break;
        case GameEvent::LevelCompleted:
            session_.recordCurrentLevelCompleted();
            levelCompleteSound_.play();
            break;
        }
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
