#include "route.h"

Route::Route(int id, int startId, int endId)
    : m_id(id)
    , m_startId(startId)
    , m_endId(endId)
{
}

int Route::getId() const
{
    return m_id;
}

int Route::getStartId() const
{
    return m_startId;
}

int Route::getEndId() const
{
    return m_endId;
}

const std::vector<QPoint>& Route::getPath() const
{
    return m_path;
}

void Route::setPath(const std::vector<QPoint>& path)
{
    m_path = path;
}