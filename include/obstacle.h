#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "i_element.h"
#include <QRect>

class Obstacle : public IElement {
public:
    Obstacle(int id, const QRect& bounds);
    
    int getId() const override;
    QPoint getPosition() const override;
    void setPosition(const QPoint& pos) override;
    
    QRect getBounds() const;

private:
    int m_id;
    QRect m_bounds;
};

#endif // OBSTACLE_H