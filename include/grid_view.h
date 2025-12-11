#ifndef GRID_VIEW_H
#define GRID_VIEW_H

#include <QWidget>
#include <QPoint>
#include <vector>
#include <memory>
#include "i_scene.h"

class GridView : public QWidget {
    Q_OBJECT
public:
    explicit GridView(std::unique_ptr<IScene> scene, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    QPoint screenToWorld(const QPoint &p);

private:
    std::unique_ptr<IScene> m_scene;
    int m_selectedPoint = -1;
    int m_dragPoint = -1;
    bool m_isDragging = false;
    double m_scale = 1.0;
    
    // Для создания препятствий
    bool m_creatingObstacle = false;
    QPoint m_obstacleStart;
    QPoint m_obstacleEnd;
};

#endif // GRID_VIEW_H

