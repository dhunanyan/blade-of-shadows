#ifndef GAME_APP_QT_MAINWINDOW_H
#define GAME_APP_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include <QSoundEffect>
#include <QTimer>
#include "game/app/game_controller.h"
#include "game/app/qt/game_session.h"
#include "game/app/qt/editor_palette.h"
#include "game/app/qt/music_player.h"
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
    void wheelEvent(QWheelEvent* event) override;
    void update();

private:
    enum class EditorTool
    {
        Tile,
        Decoration,
        PlayerSpawn,
        Enemy,
        Coin,
        Exit
    };

    void handleMenuPointer(const QPoint& localPoint, bool activate);
    void handleEditorPointer(const QPoint& localPoint, Qt::MouseButton button);
    QPoint mapPointerToFrame(const QPoint& localPoint) const;
    void applySettings(const GameSettings& settings);
    bool saveCustomLevel();
    void updateEditorStatus();

private:
    Ui::MainWindow *ui_;
    GameController gameController_;
    GameSession session_;
    AssetRepository assets_;
    SceneRenderer sceneRenderer_;
    PlayerPresentation playerPresentation_;
    QTimer timer_;
    MusicPlayer musicPlayer_;
    QSoundEffect attackSound_;
    QSoundEffect jumpSound_;
    QSoundEffect doubleJumpSound_;
    QSoundEffect coinSound_;
    QSoundEffect hurtSound_;
    QSoundEffect deathSound_;
    QSoundEffect levelCompleteSound_;
    SettingsRepository settingsRepository_;
    QRect displayedFrameRect_;
    QSize sourceFrameSize_;
    EditorTool editorTool_ = EditorTool::Tile;
    EditorPalette editorPalette_;
    QPoint lastEditedCell_{-1, -1};
    bool editorCameraLeft_ = false;
    bool editorCameraRight_ = false;
    bool editorCameraUp_ = false;
    bool editorCameraDown_ = false;

    static constexpr int playerScale_ = 3;
};
#endif // GAME_APP_QT_MAINWINDOW_H
