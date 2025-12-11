#ifndef SCENE_FACTORY_H
#define SCENE_FACTORY_H

#include "i_scene.h"
#include <memory>

class SceneFactory {
public:
    static std::unique_ptr<IScene> createScene();
};

#endif // SCENE_FACTORY_H