#include "grid_view.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <algorithm> 

GridView::GridView(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

void GridView::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.fillRect(rect(), Qt::white);

    p.setPen(QPen(Qt::lightGray, 1));
    for (int x = 0; x < width(); x += m_cellSize * m_scale)
        p.drawLine(x, 0, x, height());

    for (int y = 0; y < height(); y += m_cellSize * m_scale)
        p.drawLine(0, y, width(), y);

    p.save();
    p.scale(m_scale, m_scale);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 100, 100));
    for (const auto &cell : m_filledCells)
        p.drawRect(cell);

    p.setPen(QPen(Qt::black, 2));
    for (auto &route : m_routes)
    {
        for (int i = 0; i + 1 < (int)route.path.size(); ++i)
            p.drawLine(route.path[i], route.path[i + 1]);
    }

    for (int i = 0; i < (int)m_points.size(); i++)
    {
        if (i == m_selectedPoint) p.setBrush(Qt::red);
        else p.setBrush(Qt::blue);

        p.drawEllipse(m_points[i], 5, 5);
    }

    p.restore();
}

int GridView::findPoint(const QPoint &pos) {
    for (int i = 0; i < static_cast<int>(m_points.size()); i++) {
        if (QLineF(pos, m_points[i]).length() <= 8) return i;
    }
    return -1;
}

void GridView::mousePressEvent(QMouseEvent *e) {
QPoint worldPos = screenToWorld(e->pos());
int idx = findPoint(worldPos);

if (e->button() == Qt::RightButton) {

    QPoint world = screenToWorld(e->pos());
    QPoint cellPos = snapToGrid(world);

    QRect filled(
        cellPos.x(),
        cellPos.y(),
        m_cellSize * 2,
        m_cellSize * 2
    );

    bool exists = false;
    for (auto &c : m_filledCells)
        if (c == filled) exists = true;

    if (!exists)
        m_filledCells.push_back(filled);

    std::vector<int> pointsToRemove;
    for (int i = 0; i < static_cast<int>(m_points.size()); ++i) {
        if (filled.contains(m_points[i])) {
            pointsToRemove.push_back(i);
        }
    }
    if (!pointsToRemove.empty()) {
        std::sort(pointsToRemove.rbegin(), pointsToRemove.rend());
        for (int id : pointsToRemove) {
            // удалить маршруты, связанные с этой точкой
            m_routes.erase(
                std::remove_if(
                    m_routes.begin(), m_routes.end(),
                    [&](const Route &r) { return r.a == id || r.b == id; }
                ),
                m_routes.end()
            );
            m_points.erase(m_points.begin() + id);
            for (auto &route : m_routes) {
                if (route.a > id) --route.a;
                if (route.b > id) --route.b;
            }
        }
    } else {
        for (auto it = m_routes.begin(); it != m_routes.end(); ) {
            std::vector<QPoint> newPath;
            bool found = false;
            for (int mult = 1; mult <= MAX_OFFSET_MULTIPLIER; ++mult) {
                newPath = buildRoute(m_points[it->a], m_points[it->b], mult);
                if (!pathIntersectsBlocked(newPath)) {
                    found = true;
                    break;
                }
            }
            if (found) {
                it->path = std::move(newPath);
                ++it;
            } else {
                it = m_routes.erase(it);
            }
        }
    }

    update();
    return;
}

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
                std::vector<QPoint> path;
                bool found = false;
                for (int mult = 1; mult <= MAX_OFFSET_MULTIPLIER; ++mult) {
                    path = buildRoute(m_points[r.a], m_points[r.b], mult);
                    if (!pathIntersectsBlocked(path)) { found = true; break; }
                }
                r.path = std::move(path);
                if (!pathIntersectsBlocked(r.path))
                    m_routes.push_back(std::move(r));
            }

            m_selectedPoint = -1;
        }
        m_dragPoint = idx;
        m_isDragging = true;
    } else {
        if (isInsideBlockedCell(snapToGrid(worldPos)))
            return;
        m_points.push_back(snapToGrid(worldPos));
        m_selectedPoint = (int)m_points.size() - 1;
    }
}
update();
}

void GridView::mouseMoveEvent(QMouseEvent *e) {
    if (m_isDragging && m_dragPoint != -1) {
        QPoint oldPos = m_points[m_dragPoint];
        QPoint world = screenToWorld(e->pos());
        QPoint newPos = snapToGrid(world);

        if (newPos == oldPos) return; 

        if (isInsideBlockedCell(newPos))
            return;

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

bool GridView::isInsideBlockedCell(const QPoint &pt)
{
    for (const QRect &rc : m_filledCells) {
        if (rc.contains(pt))
            return true;
    }
    return false;
}

bool GridView::lineIntersectsRect(const QLineF &line, const QRect &rect) const
{
    QLineF edges[4] = {
        QLineF(rect.topLeft(), rect.topRight()),
        QLineF(rect.topRight(), rect.bottomRight()),
        QLineF(rect.bottomRight(), rect.bottomLeft()),
        QLineF(rect.bottomLeft(), rect.topLeft())
    };

    for (int i = 0; i < 4; ++i)
    {
        QPointF intersectPoint;
        if (line.intersects(edges[i], &intersectPoint) == QLineF::BoundedIntersection)
            return true;
    }
    return false;
}

bool GridView::segmentIntersectsBlocked(const QLineF &seg) const
{
    for (const QRect &rc : m_filledCells) {
        if (lineIntersectsRect(seg, rc))
            return true;
    }
    return false;
}

std::vector<QPoint> GridView::buildRoute(const QPoint &a, const QPoint &b, int maxOffsetMultiplier)
{
std::vector<QPoint> route;

if (a == b) {
    route.push_back(a);
    return route;
}

QPoint mid1(b.x(), a.y());
QLineF s1(a, mid1);
QLineF s2(mid1, b);

if (!segmentIntersectsBlocked(s1) && !segmentIntersectsBlocked(s2)) {
    route.push_back(a);
    route.push_back(mid1);
    route.push_back(b);
    return route;
}

int baseOffset = m_cellSize * 2;

struct Candidate { std::vector<QPoint> pts; double len; bool valid; };
auto evaluate = [&](const std::vector<QPoint> &pts) -> Candidate {
    Candidate c; c.pts = pts; c.valid = true; c.len = 0.0;
    for (size_t i = 0; i + 1 < pts.size(); ++i) {
        QLineF seg(pts[i], pts[i+1]);
        if (segmentIntersectsBlocked(seg)) {
            c.valid = false;
            return c;
        }
        c.len += seg.length();
    }
    return c;
};

for (int mult = 1; mult <= maxOffsetMultiplier; ++mult) {
    int offset = baseOffset * mult;
    std::vector<Candidate> cands;

    {
        std::vector<QPoint> pts = { a, QPoint(a.x(), a.y() - offset), QPoint(b.x(), a.y() - offset), b };
        cands.push_back(evaluate(pts));
    }

    {
        std::vector<QPoint> pts = { a, QPoint(a.x(), a.y() + offset), QPoint(b.x(), a.y() + offset), b };
        cands.push_back(evaluate(pts));
    }

    {
        std::vector<QPoint> pts = { a, QPoint(a.x() - offset, a.y()), QPoint(a.x() - offset, b.y()), b };
        cands.push_back(evaluate(pts));
    }

    {
        std::vector<QPoint> pts = { a, QPoint(a.x() + offset, a.y()), QPoint(a.x() + offset, b.y()), b };
        cands.push_back(evaluate(pts));
    }

    {
        QPoint altMid(a.x(), b.y());
        std::vector<QPoint> pts = { a, altMid, b };
        cands.push_back(evaluate(pts));
    }

    double bestLen = std::numeric_limits<double>::infinity();
    int bestIdx = -1;
    for (size_t i = 0; i < cands.size(); ++i) {
        if (cands[i].valid && cands[i].len < bestLen) {
            bestLen = cands[i].len;
            bestIdx = static_cast<int>(i);
        }
    }

    if (bestIdx != -1) {
        return cands[bestIdx].pts;
    }

    {
        QPoint midUp(b.x(), a.y() - offset);
        std::vector<QPoint> pts = { a, midUp, b };
        Candidate c = evaluate(pts);
        if (c.valid) return c.pts;
    }
}

route.push_back(a);
route.push_back(mid1);
route.push_back(b);
return route;

}

void GridView::wheelEvent(QWheelEvent *e)
{
    double numDegrees = e->angleDelta().y() / 120.0;

    double factor = 1.0 + numDegrees * 0.1;

    m_scale *= factor;

    if (m_scale < 0.3) m_scale = 0.3;
    if (m_scale > 3.0) m_scale = 3.0;

    update();
}

QPoint GridView::screenToWorld(const QPoint &p)
{
    return QPoint(
        p.x() / m_scale,
        p.y() / m_scale
    );
}

bool GridView::pathIntersectsBlocked(const std::vector<QPoint> &path) const
{
    if (path.size() < 2) return false;
        for (size_t i = 0; i + 1 < path.size(); ++i) {
        QLineF seg(path[i], path[i+1]);
            
            if (segmentIntersectsBlocked(seg)) return true;
        }
        return false;
}