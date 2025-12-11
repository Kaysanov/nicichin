#include "scene.h"
#include <algorithm>
#include <memory>

Scene::Scene(std::unique_ptr<IElementManager> elementManager, 
             std::unique_ptr<IRouteBuilder> routeBuilder)
    : m_elementManager(std::move(elementManager))
    , m_routeBuilder(std::move(routeBuilder))
    , m_cellSize(25)
{
}

int Scene::addPoint(const QPoint& position)
{
    static int nextId = 0;
    int id = nextId++;
    
    auto point = std::make_unique<Point>(id, position);
    m_elementManager->addElement(std::move(point));
    
    return id;
}

void Scene::addObstacle(const QRect& bounds)
{
    static int nextId = 1000; // Отдельная нумерация для препятствий
    int id = nextId++;
    
    auto obstacle = std::make_unique<Obstacle>(id, bounds);
    m_elementManager->addElement(std::move(obstacle));
    
    m_obstacles.push_back(bounds);
}

void Scene::removeElement(int id)
{
    m_elementManager->removeElement(id);
    
    // Удалить препятствие, если это препятствие
    m_obstacles.erase(
        std::remove_if(m_obstacles.begin(), m_obstacles.end(),
            [id](const QRect& rect) {
                // Временно: упрощенная проверка
                return false;
            }),
        m_obstacles.end()
    );
}

IElement* Scene::getElement(int id)
{
    return m_elementManager->getElement(id);
}

const std::vector<std::unique_ptr<IElement>>& Scene::getAllElements() const
{
    return m_elementManager->getAllElements();
}

std::vector<IElement*> Scene::getAllElementsPtr()
{
    return m_elementManager->getAllElementsPtr();
}

const std::vector<QRect>& Scene::getObstacles() const
{
    return m_obstacles;
}

bool Scene::buildRoute(int startId, int endId)
{
    IElement* startElement = m_elementManager->getElement(startId);
    IElement* endElement = m_elementManager->getElement(endId);
    
    if (!startElement || !endElement) {
        return false;
    }
    
    // Проверяем, что оба элемента - точки
    Point* startPoint = dynamic_cast<Point*>(startElement);
    Point* endPoint = dynamic_cast<Point*>(endElement);
    
    if (!startPoint || !endPoint) {
        return false;
    }
    
    std::vector<QPoint> path = m_routeBuilder->buildRoute(
        startPoint->getPosition(),
        endPoint->getPosition(),
        m_obstacles
    );
    
    if (!path.empty()) {
        Route route(m_routes.size(), startId, endId);
        route.setPath(path);
        m_routes.push_back(route);
        return true;
    }
    
    return false;
}

const std::vector<std::vector<QPoint>>& Scene::getRoutes() const
{
    // Преобразуем маршруты в вектор путей
    static std::vector<std::vector<QPoint>> paths;
    paths.clear();
    
    for (const auto& route : m_routes) {
        paths.push_back(route.getPath());
    }
    
    return paths;
}

void Scene::removeRoutesWithPoint(int pointId)
{
    m_routes.erase(
        std::remove_if(m_routes.begin(), m_routes.end(),
            [pointId](const Route& route) {
                return route.getStartId() == pointId || route.getEndId() == pointId;
            }),
        m_routes.end()
    );
}

void Scene::rebuildRoutes()
{
    // Перестраиваем все маршруты
    std::vector<Route> newRoutes;
    
    for (const auto& route : m_routes) {
        IElement* startElement = m_elementManager->getElement(route.getStartId());
        IElement* endElement = m_elementManager->getElement(route.getEndId());
        
        if (startElement && endElement) {
            Point* startPoint = dynamic_cast<Point*>(startElement);
            Point* endPoint = dynamic_cast<Point*>(endElement);
            
            if (startPoint && endPoint) {
                std::vector<QPoint> path = m_routeBuilder->buildRoute(
                    startPoint->getPosition(),
                    endPoint->getPosition(),
                    m_obstacles
                );
                
                if (!path.empty()) {
                    Route newRoute(newRoutes.size(), route.getStartId(), route.getEndId());
                    newRoute.setPath(path);
                    newRoutes.push_back(newRoute);
                }
            }
        }
    }
    
    m_routes = std::move(newRoutes);
}

QPoint Scene::snapToGrid(const QPoint& p) const
{
    int x = (p.x() + m_cellSize / 2) / m_cellSize * m_cellSize;
    int y = (p.y() + m_cellSize / 2) / m_cellSize * m_cellSize;
    return QPoint(x, y);
}

bool Scene::isInsideBlockedCell(const QPoint& pt) const
{
    for (const QRect& rc : m_obstacles) {
        if (rc.contains(pt))
            return true;
    }
    return false;
}

std::vector<Route> Scene::findRoutesWithPoint(int pointId)
{
    std::vector<Route> result;
    
    for (const auto& route : m_routes) {
        if (route.getStartId() == pointId || route.getEndId() == pointId) {
            result.push_back(route);
        }
    }
    
    return result;
}