#ifndef ROUTE_BUILDER_H
#define ROUTE_BUILDER_H

#include "i_route_builder.h"
#include <QLineF>

class RouteBuilder : public IRouteBuilder {
public:
    RouteBuilder();
    
    std::vector<QPoint> buildRoute(
        const QPoint& start, 
        const QPoint& end, 
        const std::vector<QRect>& obstacles
    ) override;

private:
    bool lineIntersectsRect(const QLineF& line, const QRect& rect) const;
    bool segmentIntersectsBlocked(const QLineF& seg, const std::vector<QRect>& obstacles) const;
    std::vector<QPoint> buildRouteInternal(
        const QPoint& a, 
        const QPoint& b, 
        const std::vector<QRect>& obstacles,
        int maxOffsetMultiplier = 5
    );
};

#endif // ROUTE_BUILDER_H