#include <QtGui/QApplication>
#include "KexiMobileMainWindow.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    KexiMobileMainWindow foo;
    foo.show();
    return app.exec();
}
