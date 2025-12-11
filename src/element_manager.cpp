#include "element_manager.h"
#include <algorithm>

ElementManager::ElementManager()
    : m_nextId(0)
{
}

void ElementManager::addElement(std::unique_ptr<IElement> element)
{
    m_elementMap[element->getId()] = element.get();
    m_elements.push_back(std::move(element));
}

void ElementManager::removeElement(int id)
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(),
        [id](const std::unique_ptr<IElement>& element) {
            return element->getId() == id;
        });
    
    if (it != m_elements.end()) {
        m_elementMap.erase(id);
        m_elements.erase(it);
    }
}

IElement* ElementManager::getElement(int id)
{
    auto it = m_elementMap.find(id);
    if (it != m_elementMap.end()) {
        return it->second;
    }
    return nullptr;
}

const std::vector<std::unique_ptr<IElement>>& ElementManager::getAllElements() const
{
    return m_elements;
}

std::vector<IElement*> ElementManager::getAllElementsPtr()
{
    std::vector<IElement*> elements;
    elements.reserve(m_elements.size());
    
    for (const auto& element : m_elements) {
        elements.push_back(element.get());
    }
    
    return elements;
}