#include "grid_view.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <algorithm>
#include "point.h"
#include "obstacle.h"
#include "route.h"

GridView::GridView(std::unique_ptr<IScene> scene, QWidget *parent) 
    : QWidget(parent)
    , m_scene(std::move(scene))
{
    setFocusPolicy(Qt::StrongFocus);
}

void GridView::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.fillRect(rect(), Qt::white);

    // Рисуем сетку
    p.setPen(QPen(Qt::lightGray, 1));
    for (int x = 0; x < width(); x += 25 * m_scale)
        p.drawLine(x, 0, x, height());

    for (int y = 0; y < height(); y += 25 * m_scale)
        p.drawLine(0, y, width(), y);

    p.save();
    p.scale(m_scale, m_scale);

    // Рисуем препятствия
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 100, 100));
    for (const QRect& rect : m_scene->getObstacles())
        p.drawRect(rect);

    // Рисуем маршруты
    p.setPen(QPen(Qt::black, 2));
    const auto& routes = m_scene->getRoutes();
    for (const auto& path : routes)
    {
        for (size_t i = 0; i + 1 < path.size(); ++i)
            p.drawLine(path[i], path[i + 1]);
    }

    // Рисуем точки
    auto elements = m_scene->getAllElementsPtr();
    for (int i = 0; i < static_cast<int>(elements.size()); i++)
    {
        Point* point = dynamic_cast<Point*>(elements[i]);
        if (point) {
            if (i == m_selectedPoint) 
                p.setBrush(Qt::red);
            else 
                p.setBrush(Qt::blue);

            p.drawEllipse(point->getPosition(), 5, 5);
        }
    }

    p.restore();
}

void GridView::mousePressEvent(QMouseEvent *e) {
    QPoint worldPos = screenToWorld(e->pos());
    
    if (e->button() == Qt::RightButton) {
        // Добавляем препятствие
        QPoint cellPos = m_scene->snapToGrid(worldPos);
        QRect filled(
            cellPos.x(),
            cellPos.y(),
            50,
            50
        );
        
        m_scene->addObstacle(filled);
        
        // Перестраиваем маршруты
        m_scene->rebuildRoutes();
        
        update();
        return;
    }

    if (e->button() == Qt::LeftButton) {
        // Проверяем, кликнули ли мы по существующей точке
        auto elements = m_scene->getAllElementsPtr();
        int clickedPointId = -1;
        
        for (int i = 0; i < static_cast<int>(elements.size()); i++) {
            Point* point = dynamic_cast<Point*>(elements[i]);
            if (point) {
                QPoint pos = point->getPosition();
                if (QLineF(worldPos, pos).length() <= 8) {
                    clickedPointId = point->getId();
                    break;
                }
            }
        }
        
        if (clickedPointId != -1) {
            if (m_selectedPoint == -1) {
                m_selectedPoint = clickedPointId;
            } else if (m_selectedPoint != clickedPointId) {
                // Пытаемся построить маршрут
                if (m_scene->buildRoute(m_selectedPoint, clickedPointId)) {
                    update();
                }
                m_selectedPoint = -1;
            }
            m_dragPoint = clickedPointId;
            m_isDragging = true;
        } else {
            // Добавляем новую точку
            if (!m_scene->isInsideBlockedCell(m_scene->snapToGrid(worldPos))) {
                m_scene->addPoint(m_scene->snapToGrid(worldPos));
                update();
            }
        }
    }
    update();
}

void GridView::mouseMoveEvent(QMouseEvent *e) {
    if (m_isDragging && m_dragPoint != -1) {
        QPoint world = screenToWorld(e->pos());
        QPoint newPos = m_scene->snapToGrid(world);
        
        // Обновляем позицию точки
        IElement* element = m_scene->getElement(m_dragPoint);
        Point* point = dynamic_cast<Point*>(element);
        if (point && !m_scene->isInsideBlockedCell(newPos)) {
            point->setPosition(newPos);
            
            // Перестраиваем маршруты, связанные с этой точкой
            m_scene->removeRoutesWithPoint(m_dragPoint);
            update();
        }
    }
}

void GridView::mouseReleaseEvent(QMouseEvent *) {
    m_isDragging = false;
    m_dragPoint = -1;
}

void GridView::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Delete) {
        if (m_selectedPoint != -1) {
            // Удаляем точку
            m_scene->removeElement(m_selectedPoint);
            
            // Удаляем связанные маршруты
            m_scene->removeRoutesWithPoint(m_selectedPoint);
            
            m_selectedPoint = -1;
            update();
        }
    }
}

void GridView::wheelEvent(QWheelEvent *e)
{
    double numDegrees = e->angleDelta().y() / 120.0;
    double factor = 1.0 + numDegrees * 0.1;
    m_scale *= factor;

    if (m_scale < 0.3) m_scale = 0.3;
    if (m_scale > 3.0) m_scale = 3.0;

    update();
}

QPoint GridView::screenToWorld(const QPoint &p)
{
    return QPoint(
        p.x() / m_scale,
        p.y() / m_scale
    );
}