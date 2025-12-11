#include "route_builder.h"
#include <limits>
#include <algorithm>

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
    std::vector<QPoint> route;

    if (a == b) {
        route.push_back(a);
        return route;
    }

    QPoint mid1(b.x(), a.y());
    QLineF s1(a, mid1);
    QLineF s2(mid1, b);

    if (!segmentIntersectsBlocked(s1, obstacles) && !segmentIntersectsBlocked(s2, obstacles)) {
        route.push_back(a);
        route.push_back(mid1);
        route.push_back(b);
        return route;
    }

    int baseOffset = 50; // фиксированный размер ячейки

    struct Candidate { 
        std::vector<QPoint> pts; 
        double len; 
        bool valid; 
    };
    
    auto evaluate = [&](const std::vector<QPoint>& pts) -> Candidate {
        Candidate c; 
        c.pts = pts; 
        c.valid = true; 
        c.len = 0.0;
        for (size_t i = 0; i + 1 < pts.size(); ++i) {
            QLineF seg(pts[i], pts[i+1]);
            if (segmentIntersectsBlocked(seg, obstacles)) {
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