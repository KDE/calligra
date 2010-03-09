#include "combinedview.h"
#include <QtGui/QApplication>
#include <QtGui/QPixmap>

int
main(int argc, char** argv) {
    //QApplication::setGraphicsSystem("opengl");
    QApplication app(argc, argv);
//    QPixmapCache::setCacheLimit(100000);

    CombinedView cv;
    cv.resize(630, 410);
    cv.show();
    if (argc > 1) {
        cv.openFile(argv[1]);
    }
    return app.exec();
}
