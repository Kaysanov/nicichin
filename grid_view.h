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
    void wheelEvent(QWheelEvent *) override;
    QPoint screenToWorld(const QPoint &p);

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

    std::vector<QPoint> buildRoute(const QPoint &a, const QPoint &b, int maxOffsetMultiplier = 1);
    bool lineIntersectsRect(const QLineF &line, const QRect &rect) const;
    bool segmentIntersectsBlocked(const QLineF &seg) const;
    bool isInsideBlockedCell(const QPoint &pt);
    bool pathIntersectsBlocked(const std::vector<QPoint> &path) const;


    int m_selectedPoint = -1;
    int m_dragPoint = -1;
    bool m_isDragging = false;

    int findPoint(const QPoint &pos);
    
    std::vector<QRect> m_filledCells;
    
    double m_scale = 1.0;

    static constexpr int MAX_OFFSET_MULTIPLIER = 5;
};

#endif // GRID_VIEW_H

