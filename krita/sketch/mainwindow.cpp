/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "mainwindow.h"

#include <QApplication>
#include <QResizeEvent>
#include <QDeclarativeView>
#include <QDeclarativeContext>

#include "constants.h"
#include "settings.h"

#include "kis_doc2.h"
#include "kis_view2.h"
#include "klibloader.h"

#include <KoColorSpaceRegistry.h>
#include <KoColorSpace.h>

class MainWindow::Private
{
    public:
        QDeclarativeView* view;
        Constants* constants;
        QObject* settings;
};

MainWindow::MainWindow( QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ), d( new Private )
{
    qApp->setActiveWindow( this );

    setAttribute( Qt::WA_AcceptTouchEvents );

    d->constants = new Constants( this );
    d->settings = new Settings( this );

    QDeclarativeView* view = new QDeclarativeView();
    view->rootContext()->setContextProperty( "Constants", d->constants );
    view->rootContext()->setContextProperty( "Settings", d->settings );
    view->setSource( QUrl( "qrc:/main.qml" ) );
    view->setResizeMode( QDeclarativeView::SizeRootObjectToView );


    KPluginFactory* factory = KLibLoader::self()->factory("kritapart");
    KisDoc2 *doc = static_cast<KisDoc2*>(factory->create(0, "KritaPart"));
    qDebug() << "doc:" << doc << "views" << doc->viewCount();

    doc->newImage("test", 1000, 100, KoColorSpaceRegistry::instance()->rgb8());
    KoView *kisView = qobject_cast<KisView2*>(doc->createView(this));
    //Q_ASSERT(doc->viewCount() > 0);

    //view->setViewport(kisView->canvas());

    setCentralWidget( view );
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    d->constants->setGridWidth( event->size().width() / d->constants->gridColumns() );
    d->constants->setGridHeight( event->size().height() / d->constants->gridRows() );
    QWidget::resizeEvent(event);
}

MainWindow::~MainWindow()
{
    delete d;
}

#include "mainwindow.moc"
