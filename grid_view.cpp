#include "grid_view.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>

GridView::GridView(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void GridView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    p.setPen(QPen(Qt::lightGray, 1));
    for (int x = 0; x < width(); x += m_cellSize)
        p.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += m_cellSize)
        p.drawLine(0, y, width(), y);

    p.setPen(QPen(Qt::black, 2));
    for (auto &route : m_routes)
    {
        for (int i = 0; i < route.size() - 1; i++)
            p.drawLine(route[i], route[i+1]);
    }   

    for (int i = 0; i < m_points.size(); i++) {
        if (i == m_selectedPoint)
            p.setBrush(Qt::red);
        else
            p.setBrush(Qt::blue);

        p.drawEllipse(m_points[i], 5, 5);
    }
}

int GridView::findPoint(const QPoint &pos)
{
    for (int i = 0; i < m_points.size(); i++) {
        if (QLineF(pos, m_points[i]).length() <= 8)
            return i;
    }
    return -1;
}

void GridView::mousePressEvent(QMouseEvent *e)
{
    int idx = findPoint(e->pos());

    if (e->button() == Qt::LeftButton) {

        if (idx != -1) {
            if (m_selectedPoint == -1) {
                m_selectedPoint = idx;
            } else if (m_selectedPoint != idx) {
                m_routes.push_back(buildRoute(m_points[m_selectedPoint], m_points[idx]));
                m_selectedPoint = -1;
            }

            m_dragPoint = idx;
            m_isDragging = true;
        } else {
            m_points.push_back(snapToGrid(e->pos()));
            m_selectedPoint = m_points.size() - 1;
        }
    }

    update();
}

void GridView::mouseMoveEvent(QMouseEvent *e)
{
    if (m_isDragging && m_dragPoint != -1) {

        QPoint oldPos = m_points[m_dragPoint];

        QPoint newPos = snapToGrid(e->pos());
        m_points[m_dragPoint] = newPos;

        for (auto &route : m_routes)
        {
            QPoint &start = route.front();
            QPoint &end   = route.back();

            bool updated = false;

            if (start == oldPos) {
                start = newPos;
                updated = true;
            }

            if (end == oldPos) {
                end = newPos;
                updated = true;
            }

            if (updated) {
                route = buildRoute(start, end);
            }
        }

        update();
    }
}


void GridView::mouseReleaseEvent(QMouseEvent *)
{
    m_isDragging = false;
    m_dragPoint = -1;
}

void GridView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete) {

        if (m_selectedPoint != -1) {

            QPoint removedPoint = m_points[m_selectedPoint];

            m_routes.erase(
                std::remove_if(
                    m_routes.begin(),
                    m_routes.end(),
                    [&](const auto &r) {
                        return r.front() == removedPoint || r.back() == removedPoint;
                    }
                ),
                m_routes.end()
            );

            m_points.erase(m_points.begin() + m_selectedPoint);

            m_selectedPoint = -1;
            update();
        }

    }
}

QPoint GridView::snapToGrid(const QPoint &p)
{
    int x = (p.x() + m_cellSize / 2) / m_cellSize * m_cellSize;
    int y = (p.y() + m_cellSize / 2) / m_cellSize * m_cellSize;
    return QPoint(x, y);
}

std::vector<QPoint> GridView::buildRoute(const QPoint &a, const QPoint &b)
{
    std::vector<QPoint> route;

    route.push_back(a);

    QPoint mid(b.x(), a.y());
    route.push_back(mid);

    route.push_back(b);

    return route;
}
