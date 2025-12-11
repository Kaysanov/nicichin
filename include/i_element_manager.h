#ifndef I_ELEMENT_MANAGER_H
#define I_ELEMENT_MANAGER_H

#include <vector>
#include <memory>
#include "i_element.h"

class IElementManager {
public:
    virtual ~IElementManager() = default;
    
    virtual void addElement(std::unique_ptr<IElement> element) = 0;
    virtual void removeElement(int id) = 0;
    virtual IElement* getElement(int id) = 0;
    virtual const std::vector<std::unique_ptr<IElement>>& getAllElements() const = 0;
    virtual std::vector<IElement*> getAllElementsPtr() = 0;
};

#endif // I_ELEMENT_MANAGER_H