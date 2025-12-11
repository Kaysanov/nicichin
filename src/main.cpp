#include <QApplication>
#include "grid_view.h"
#include "scene_factory.h"
#include "i_scene.h"
#include <memory>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Создаем сцену через фабрику
    std::unique_ptr<IScene> scene = SceneFactory::createScene();
    
    GridView view(std::move(scene));
    view.resize(800, 600);
    view.show();

    return app.exec();
}
