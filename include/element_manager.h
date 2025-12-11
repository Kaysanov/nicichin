#ifndef ELEMENT_MANAGER_H
#define ELEMENT_MANAGER_H

#include "i_element_manager.h"
#include <map>

class ElementManager : public IElementManager {
public:
    ElementManager();
    
    void addElement(std::unique_ptr<IElement> element) override;
    void removeElement(int id) override;
    IElement* getElement(int id) override;
    const std::vector<std::unique_ptr<IElement>>& getAllElements() const override;
    std::vector<IElement*> getAllElementsPtr() override;

private:
    std::vector<std::unique_ptr<IElement>> m_elements;
    std::map<int, IElement*> m_elementMap;
    int m_nextId;
};

#endif // ELEMENT_MANAGER_H