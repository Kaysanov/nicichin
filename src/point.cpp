#include "point.h"

Point::Point(int id, const QPoint& position)
    : m_id(id)
    , m_position(position)
{
}

int Point::getId() const
{
    return m_id;
}

QPoint Point::getPosition() const
{
    return m_position;
}

void Point::setPosition(const QPoint& pos)
{
    m_position = pos;
}