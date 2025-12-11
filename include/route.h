#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <QPoint>

class Route {
public:
    Route(int id, int startId, int endId);
    
    int getId() const;
    int getStartId() const;
    int getEndId() const;
    const std::vector<QPoint>& getPath() const;
    void setPath(const std::vector<QPoint>& path);
    
private:
    int m_id;
    int m_startId;
    int m_endId;
    std::vector<QPoint> m_path;
};

#endif // ROUTE_H