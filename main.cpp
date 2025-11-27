#include <QApplication>
#include "grid_view.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GridView view;
    view.resize(800, 600);
    view.show();

    return app.exec();
}
