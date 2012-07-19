#include <QtDeclarative>
#include <QDeclarativeView>
#include <QDeclarativeError>
#include <QApplication>
#include <QDesktopServices>
#include <QDebug>

#include "constants.h"

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    QDeclarativeView* view = new QDeclarativeView();
    view->rootContext()->setContextProperty("Constants", new Constants( view ) );
    view->setSource( QUrl( "qrc:/main.qml" ) );
    view->setResizeMode( QDeclarativeView::SizeRootObjectToView );
    view->show();

    if( view->errors().size() > 0 )
    {
        return 1;
    }
    else
    {
        return app.exec();
    }
}