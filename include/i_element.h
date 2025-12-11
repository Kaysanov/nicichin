#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include <QPoint>

class IElement {
public:
    virtual ~IElement() = default;
    
    virtual int getId() const = 0;
    virtual QPoint getPosition() const = 0;
    virtual void setPosition(const QPoint& pos) = 0;
};

#endif // I_ELEMENT_H