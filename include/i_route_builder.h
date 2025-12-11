#ifndef I_ROUTE_BUILDER_H
#define I_ROUTE_BUILDER_H

#include <vector>
#include <QPoint>
#include <QRect>
#include <memory>

class IRouteBuilder {
public:
    virtual ~IRouteBuilder() = default;
    
    virtual std::vector<QPoint> buildRoute(
        const QPoint& start, 
        const QPoint& end, 
        const std::vector<QRect>& obstacles
    ) = 0;
};

#endif // I_ROUTE_BUILDER_H