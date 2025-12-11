#include "obstacle.h"

Obstacle::Obstacle(int id, const QRect& bounds)
    : m_id(id)
    , m_bounds(bounds)
{
}

int Obstacle::getId() const
{
    return m_id;
}

QPoint Obstacle::getPosition() const
{
    return m_bounds.topLeft();
}

void Obstacle::setPosition(const QPoint& pos)
{
    m_bounds.moveTo(pos);
}

QRect Obstacle::getBounds() const
{
    return m_bounds;
}