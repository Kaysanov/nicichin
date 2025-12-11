#ifndef POINT_H
#define POINT_H

#include "i_element.h"
#include <QPoint>

class Point : public IElement {
public:
    Point(int id, const QPoint& position);
    
    int getId() const override;
    QPoint getPosition() const override;
    void setPosition(const QPoint& pos) override;

private:
    int m_id;
    QPoint m_position;
};

#endif // POINT_H