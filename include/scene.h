#ifndef SCENE_H
#define SCENE_H

#include "i_scene.h"
#include "i_element_manager.h"
#include "i_route_builder.h"
#include "point.h"
#include "obstacle.h"
#include "route.h"
#include <memory>
#include <vector>

class Scene : public IScene {
public:
    Scene(std::unique_ptr<IElementManager> elementManager, 
          std::unique_ptr<IRouteBuilder> routeBuilder);
    
    // Управление элементами
    int addPoint(const QPoint& position) override;
    void addObstacle(const QRect& bounds) override;
    void removeElement(int id) override;
    
    // Получение элементов
    IElement* getElement(int id) override;
    const std::vector<std::unique_ptr<IElement>>& getAllElements() const override;
    std::vector<IElement*> getAllElementsPtr() override;
    
    // Работа с препятствиями
    const std::vector<QRect>& getObstacles() const override;
    
    // Работа с маршрутами
    bool buildRoute(int startId, int endId) override;
    const std::vector<std::vector<QPoint>>& getRoutes() const override;
    void removeRoutesWithPoint(int pointId) override;
    void rebuildRoutes() override;
    
    // Вспомогательные функции
    QPoint snapToGrid(const QPoint& p) const override;
    bool isInsideBlockedCell(const QPoint& pt) const override;

private:
    std::unique_ptr<IElementManager> m_elementManager;
    std::unique_ptr<IRouteBuilder> m_routeBuilder;
    std::vector<Route> m_routes;
    std::vector<QRect> m_obstacles;
    int m_cellSize;
    
    std::vector<Route> findRoutesWithPoint(int pointId);
};

#endif // SCENE_H