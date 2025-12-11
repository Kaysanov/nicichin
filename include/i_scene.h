#ifndef I_SCENE_H
#define I_SCENE_H

#include <vector>
#include <QPoint>
#include <QRect>
#include <memory>
#include "i_element.h"

class IScene {
public:
    virtual ~IScene() = default;
    
    // Управление элементами
    virtual int addPoint(const QPoint& position) = 0;
    virtual void addObstacle(const QRect& bounds) = 0;
    virtual void removeElement(int id) = 0;
    
    // Получение элементов
    virtual IElement* getElement(int id) = 0;
    virtual const std::vector<std::unique_ptr<IElement>>& getAllElements() const = 0;
    virtual std::vector<IElement*> getAllElementsPtr() = 0;
    
    // Работа с препятствиями
    virtual const std::vector<QRect>& getObstacles() const = 0;
    
    // Работа с маршрутами
    virtual bool buildRoute(int startId, int endId) = 0;
    virtual const std::vector<std::vector<QPoint>>& getRoutes() const = 0;
    virtual void removeRoutesWithPoint(int pointId) = 0;
    virtual void rebuildRoutes() = 0;
    
    // Вспомогательные функции
    virtual QPoint snapToGrid(const QPoint& p) const = 0;
    virtual bool isInsideBlockedCell(const QPoint& pt) const = 0;
};

#endif // I_SCENE_H