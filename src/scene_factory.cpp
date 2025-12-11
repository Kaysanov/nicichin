#include "scene_factory.h"
#include "scene.h"
#include "element_manager.h"
#include "route_builder.h"

std::unique_ptr<IScene> SceneFactory::createScene()
{
    auto elementManager = std::make_unique<ElementManager>();
    auto routeBuilder = std::make_unique<RouteBuilder>();
    
    return std::make_unique<Scene>(std::move(elementManager), std::move(routeBuilder));
}