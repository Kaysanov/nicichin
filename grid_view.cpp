#include "grid_view.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <algorithm> 

GridView::GridView(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

void GridView::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    p.setPen(QPen(Qt::lightGray, 1));
    for (int x = 0; x < width(); x += m_cellSize)
        p.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += m_cellSize)
        p.drawLine(0, y, width(), y);

    p.setPen(QPen(Qt::black, 2));
    for (auto &route : m_routes) {
        for (int i = 0; i + 1 < static_cast<int>(route.path.size()); ++i)
            p.drawLine(route.path[i], route.path[i + 1]);
    }

    for (int i = 0; i < static_cast<int>(m_points.size()); i++) {
        if (i == m_selectedPoint) p.setBrush(Qt::red);
        else p.setBrush(Qt::blue);
        p.drawEllipse(m_points[i], 5, 5);
    }
}

int GridView::findPoint(const QPoint &pos) {
    for (int i = 0; i < static_cast<int>(m_points.size()); i++) {
        if (QLineF(pos, m_points[i]).length() <= 8) return i;
    }
    return -1;
}

void GridView::mousePressEvent(QMouseEvent *e) {
    int idx = findPoint(e->pos());
    if (e->button() == Qt::LeftButton) {
        if (idx != -1) {
            if (m_selectedPoint == -1) {
                m_selectedPoint = idx;
            } else if (m_selectedPoint != idx) {

                bool exists = false;
                for (const auto &route : m_routes) {
                    if ((route.a == m_selectedPoint && route.b == idx) ||
                        (route.a == idx && route.b == m_selectedPoint)) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    Route r;
                    r.a = m_selectedPoint;
                    r.b = idx;
                    r.path = buildRoute(m_points[r.a], m_points[r.b]);
                    m_routes.push_back(std::move(r));
                }

                m_selectedPoint = -1;
            }
            m_dragPoint = idx;
            m_isDragging = true;
        } else {
            m_points.push_back(snapToGrid(e->pos()));
            m_selectedPoint = (int)m_points.size() - 1;
        }
    }
    update();
}


void GridView::mouseMoveEvent(QMouseEvent *e) {
    if (m_isDragging && m_dragPoint != -1) {
        QPoint oldPos = m_points[m_dragPoint];
        QPoint newPos = snapToGrid(e->pos());
        if (newPos == oldPos) return; 

        m_points[m_dragPoint] = newPos;

        for (auto &route : m_routes) {
            bool updated = false;
            if (route.a == m_dragPoint || route.b == m_dragPoint) {
                if (route.a >= 0 && route.a < static_cast<int>(m_points.size()) &&
                    route.b >= 0 && route.b < static_cast<int>(m_points.size())) {
                    route.path = buildRoute(m_points[route.a], m_points[route.b]);
                    updated = true;
                }
            }
            (void)updated; 
        }
        update();
    }
}

void GridView::mouseReleaseEvent(QMouseEvent *) {
    m_isDragging = false;
    m_dragPoint = -1;
}

void GridView::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Delete) {
        if (m_selectedPoint != -1) {
            int removedIdx = m_selectedPoint;

            m_routes.erase(
                std::remove_if(
                    m_routes.begin(), m_routes.end(),
                    [&](const Route &r) { return r.a == removedIdx || r.b == removedIdx; }
                ),
                m_routes.end()
            );

            m_points.erase(m_points.begin() + removedIdx);

            for (auto &route : m_routes) {
                if (route.a > removedIdx) --route.a;
                if (route.b > removedIdx) --route.b;
                if (route.a >= 0 && route.a < static_cast<int>(m_points.size()) &&
                    route.b >= 0 && route.b < static_cast<int>(m_points.size())) {
                    route.path = buildRoute(m_points[route.a], m_points[route.b]);
                }
            }

            m_selectedPoint = -1;
            update();
        }
    }
}

QPoint GridView::snapToGrid(const QPoint &p) {
    int x = (p.x() + m_cellSize / 2) / m_cellSize * m_cellSize;
    int y = (p.y() + m_cellSize / 2) / m_cellSize * m_cellSize;
    return QPoint(x, y);
}

std::vector<QPoint> GridView::buildRoute(const QPoint &a, const QPoint &b) {
    std::vector<QPoint> route;
    route.push_back(a);
    QPoint mid(b.x(), a.y());
    route.push_back(mid);
    route.push_back(b);
    return route;
}
