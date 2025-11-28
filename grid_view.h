#ifndef GRID_VIEW_H
#define GRID_VIEW_H

#include <QWidget>
#include <QPoint>
#include <vector>

class GridView : public QWidget {
    Q_OBJECT
public:
    explicit GridView(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void keyPressEvent(QKeyEvent *) override;

    QPoint snapToGrid(const QPoint &p);

private:
    int m_cellSize = 25;
    std::vector<QPoint> m_points;

    struct Route {
        int a = -1;
        int b = -1;
        std::vector<QPoint> path;
    };
    std::vector<Route> m_routes;

    std::vector<QPoint> buildRoute(const QPoint &a, const QPoint &b);

    int m_selectedPoint = -1;
    int m_dragPoint = -1;
    bool m_isDragging = false;

    int findPoint(const QPoint &pos);
};

#endif // GRID_VIEW_H

