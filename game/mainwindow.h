#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <utility>
#include <QMainWindow>
#include <QTimer>
#include <QMediaPlayer>
#include "../engine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void redrawView();

public slots:
    void keyPressEvent(QKeyEvent* event) override;

    void onPressShoot();
    void onPressUp();
    void onPressDown();
    void onPressLeft();
    void onPressRight();

    void update();

private:  // methods:
    void drawPlayer(QPainter& painter);
    void drawShoots(QPainter& painter);
    void drawEnemies(QPainter& painter);

    QPixmap getRotatedPlayerImage() const;
    std::pair<QPoint,QPoint> calculatePlayerPosition() const;
    QPoint position2QPoint(Position position) const;
    std::pair<QPoint,QPoint> position2PairOfQPoints(Position position) const;
    void drawLifeBarAboveEnemy(QPainter &painter, const Enemy &enemy);

private:  // fields:
    Ui::MainWindow *ui_;

    Engine engine_;

    QPixmap playerPixmapOrginal_;
    QPixmap backgroundOrginal_;
    QPixmap bulletOriginal_;
    QPixmap enemyOriginal_;

    QTimer timer_;

    QMediaPlayer player_;
};
#endif // MAINWINDOW_H
