#include "route_builder.h"
#include <limits>
#include <algorithm>
#include <QQueue>
#include <QHash>

RouteBuilder::RouteBuilder()
{
}

std::vector<QPoint> RouteBuilder::buildRoute(
    const QPoint& start, 
    const QPoint& end, 
    const std::vector<QRect>& obstacles)
{
    return buildRouteInternal(start, end, obstacles);
}

bool RouteBuilder::lineIntersectsRect(const QLineF& line, const QRect& rect) const
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

bool RouteBuilder::segmentIntersectsBlocked(const QLineF& seg, const std::vector<QRect>& obstacles) const
{
    for (const QRect& rc : obstacles) {
        if (lineIntersectsRect(seg, rc))
            return true;
    }
    return false;
}

std::vector<QPoint> RouteBuilder::buildRouteInternal(
    const QPoint& a,
    const QPoint& b,
    const std::vector<QRect>& obstacles,
    int maxOffsetMultiplier)
{
    const int step = 25;

    QPoint start(
        a.x() / step,
        a.y() / step
    );
    QPoint goal(
        b.x() / step,
        b.y() / step
    );

    auto isBlocked = [&](int gx, int gy) {
        QPoint real(gx * step, gy * step);
        for (const QRect& rc : obstacles)
            if (rc.contains(real))
                return true;
        return false;
    };

    QQueue<QPoint> q;
    QHash<QPoint, QPoint> parent;

    q.enqueue(start);
    parent[start] = start;

    const QPoint dirs[4] = {
        QPoint(1, 0),
        QPoint(-1, 0),
        QPoint(0, 1),
        QPoint(0, -1)
    };

    while (!q.isEmpty())
    {
        QPoint cur = q.dequeue();

        if (cur == goal)
            break;

        for (auto d : dirs)
        {
            QPoint nxt(cur.x() + d.x(), cur.y() + d.y());

            if (parent.contains(nxt)) continue;
            if (isBlocked(nxt.x(), nxt.y())) continue;

            parent[nxt] = cur;
            q.enqueue(nxt);
        }
    }

    // Если цель недостижима
    if (!parent.contains(goal))
        return { a, b };

    std::vector<QPoint> pathGrid;
    QPoint p = goal;

    while (p != start) {
        pathGrid.push_back(QPoint(p.x() * step, p.y() * step));
        p = parent[p];
    }
    pathGrid.push_back(QPoint(start.x() * step, start.y() * step));

    std::reverse(pathGrid.begin(), pathGrid.end());
    return pathGrid;
}