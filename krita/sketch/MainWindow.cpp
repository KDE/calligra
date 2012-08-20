/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 * Copyright (C) 2012 KO GmbH. Contact: Boudewijn Rempt <boud@kogmbh.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "MainWindow.h"

#include <opengl2/kis_gl2_canvas.h>

#include <QApplication>
#include <QResizeEvent>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDir>
#include <QGLWidget>

#include <kcmdlineargs.h>
#include <kurl.h>
#include <kstandarddirs.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorSpace.h>

#include "kis_doc2.h"
#include "kis_view2.h"
#include "klibloader.h"

#include "IconImageProvider.h"
#include "CanvasControllerDeclarative.h"
#include "DocumentListModel.h"
#include "KisSketchView.h"
#include "ColorImageProvider.h"
#include "PaletteColorsModel.h"
#include "PaletteModel.h"
#include "PresetModel.h"
#include "PresetImageProvider.h"

#include "Constants.h"
#include "Settings.h"

class MainWindow::Private
{
    public:
        QDeclarativeView* view;
        Constants* constants;
        QObject* settings;
};

MainWindow::MainWindow(QStringList fileNames, QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ), d( new Private )
{
    qApp->setActiveWindow( this );

    setWindowTitle(i18n("Krita Sketch"));

    d->constants = new Constants( this );
    d->settings = new Settings( this );

    qmlRegisterType<DocumentListModel>("org.krita.sketch", 1, 0, "DocumentListModel");
    qmlRegisterType<KisSketchView>("org.krita.sketch", 1, 0, "SketchView");

    d->view = new QDeclarativeView();
    d->view->setAttribute(Qt::WA_AcceptTouchEvents);

    d->view->engine()->addImageProvider(QLatin1String("presetthumb"), new PresetImageProvider);
    d->view->engine()->addImageProvider(QLatin1String("color"), new ColorImageProvider);
    d->view->rootContext()->setContextProperty( "PaletteModel", new PaletteModel( this ) );
    d->view->rootContext()->setContextProperty( "PaletteColorsModel", new PaletteColorsModel( this ) );
    d->view->rootContext()->setContextProperty( "PresetsModel", new PresetModel( this ) );
    d->view->rootContext()->setContextProperty("Constants", d->constants);
    d->view->rootContext()->setContextProperty("Settings", d->settings);

    // This is needed because OpenGL viewport doesn't support partial updates.
    d->view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    QGLWidget* glWidget = new QGLWidget(this, KisGL2Canvas::shareWidget());
    d->view->setViewport(glWidget);

    d->view->engine()->addImageProvider("icon", new IconImageProvider);
    QStringList dataPaths = KGlobal::dirs()->findDirs("appdata", "qml");
    foreach(const QString& path, dataPaths) {
        d->view->engine()->addImportPath(path);
    }

    d->view->setSource(QUrl::fromLocalFile(KStandardDirs::locate("appdata", "qml/main.qml")));
    d->view->setResizeMode( QDeclarativeView::SizeRootObjectToView );

//     if(d->view->errors().count() > 0) {
//         foreach(const QDeclarativeError &error, d->view->errors()) {
//             qDebug() << error.toString();
//         }
//     }

    // Use these to populate the list of recent files, open the last one automatically
    Q_UNUSED(fileNames);

//    d->view->setViewport(kisView->canvas());

    setCentralWidget( d->view );
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

#include "MainWindow.moc"
