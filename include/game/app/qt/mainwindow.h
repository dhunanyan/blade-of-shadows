#ifndef GAME_APP_QT_MAINWINDOW_H
#define GAME_APP_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QRect>
#include <QTimer>
#include "game/app/game_controller.h"
#include "game/app/qt/tilemapper.h"
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
    void handleMenuPointer(const QPoint& localPoint, bool activate);

private:
    Ui::MainWindow *ui_;
    TileMapper tileMapper_;
    GameController gameController_;
    AssetRepository assets_;
    SceneRenderer sceneRenderer_;
    PlayerPresentation playerPresentation_;
    QTimer timer_;
    QMediaPlayer player_;
    QRect displayedFrameRect_;
    QSize sourceFrameSize_;

    static constexpr int tileSizePx_ = 24;
    static constexpr int playerScale_ = 3;
};
#endif // GAME_APP_QT_MAINWINDOW_H
