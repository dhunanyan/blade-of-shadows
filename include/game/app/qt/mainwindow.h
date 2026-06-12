#ifndef GAME_APP_QT_MAINWINDOW_H
#define GAME_APP_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QRect>
#include <QSoundEffect>
#include <QStringList>
#include <QTimer>
#include "game/app/game_controller.h"
#include "game/app/qt/tilemapper.h"
#include "game/app/qt/save_game_repository.h"
#include "game/app/qt/settings_repository.h"
#include "game/renderer/asset_repository.h"
#include "game/renderer/player_presentation.h"
#include "game/renderer/scene_renderer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void redrawView();
    bool eventFilter(QObject* watched, QEvent* event) override;

public slots:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void update();

private:
    enum class EditorTool
    {
        Tile,
        PlayerSpawn,
        Enemy,
        Coin,
        Exit
    };

    void handleMenuPointer(const QPoint& localPoint, bool activate);
    void handleEditorPointer(const QPoint& localPoint, Qt::MouseButton button);
    QPoint mapPointerToFrame(const QPoint& localPoint) const;
    bool loadLevel(const QString& resourcePath, bool resetSession);
    bool loadLevelById(const QString& levelId, bool resetSession);
    bool loadNextLevel();
    void resetCurrentLevel();
    bool saveGame();
    bool loadGame();
    void applySettings(const GameSettings& settings);
    QString customLevelPath() const;
    bool saveCustomLevel();
    void updateEditorStatus();

private:
    Ui::MainWindow *ui_;
    TileMapper tileMapper_;
    GameController gameController_;
    AssetRepository assets_;
    SceneRenderer sceneRenderer_;
    PlayerPresentation playerPresentation_;
    QTimer timer_;
    QMediaPlayer player_;
    QAudioOutput audioOutput_;
    QSoundEffect attackSound_;
    QSoundEffect jumpSound_;
    QSoundEffect coinSound_;
    QSoundEffect hurtSound_;
    SettingsRepository settingsRepository_;
    SaveGameRepository saveGameRepository_;
    QRect displayedFrameRect_;
    QSize sourceFrameSize_;
    QStringList campaignLevelIds_{
        QString::fromUtf8("level_01"),
        QString::fromUtf8("level_02")};
    int editorTileIndex_ = 0;
    bool editorSolid_ = true;
    EditorTool editorTool_ = EditorTool::Tile;

    static constexpr int tileSizePx_ = 24;
    static constexpr int playerScale_ = 3;
};
#endif // GAME_APP_QT_MAINWINDOW_H
