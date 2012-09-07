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
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>

#include <kcmdlineargs.h>
#include <kurl.h>
#include <kstandarddirs.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorSpace.h>

#include "kis_doc2.h"
#include "kis_view2.h"
#include "kis_clipboard.h"

#include "ColorSelectorItem.h"
#include "IconImageProvider.h"
#include "DocumentListModel.h"
#include "KisSketchView.h"
#include "ColorImageProvider.h"
#include "LayerModel.h"
#include "LayerCompositeDetails.h"
#include "PaletteColorsModel.h"
#include "RecentImagesModel.h"
#include "PaletteModel.h"
#include "PresetModel.h"
#include "PresetImageProvider.h"
#include "RecentImageImageProvider.h"
#include "RecentFileManager.h"
#include "MultiFeedRSSModel.h"
#include "FileSystemModel.h"

#include "Constants.h"
#include "Settings.h"

class MainWindow::Private
{
public:
    QDeclarativeView* view;
    Constants* constants;
    QObject* settings;
    RecentFileManager *recentFileManager;
};

MainWindow::MainWindow(QStringList fileNames, QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ), d( new Private )
{
    qApp->setActiveWindow( this );

    setWindowTitle(i18n("Krita Sketch"));

    d->constants = new Constants( this );
    d->settings = new Settings( this );
    d->recentFileManager = new RecentFileManager(this);
    foreach(QString fileName, fileNames) {
        d->recentFileManager->addRecent(fileName);
    }

    qmlRegisterType<ColorSelectorItem>("org.krita.sketch", 1, 0, "ColorSelectorItem");
    qmlRegisterType<DocumentListModel>("org.krita.sketch", 1, 0, "DocumentListModel");
    qmlRegisterType<PaletteModel>("org.krita.sketch", 1, 0, "PaletteModel");
    qmlRegisterType<PaletteColorsModel>("org.krita.sketch", 1, 0, "PaletteColorsModel");
    qmlRegisterType<PresetModel>("org.krita.sketch", 1, 0, "PresetModel");
    qmlRegisterType<KisSketchView>("org.krita.sketch", 1, 0, "SketchView");
    qmlRegisterType<LayerModel>("org.krita.sketch", 1, 0, "LayerModel");
    qmlRegisterType<RecentImagesModel>("org.krita.sketch", 1, 0, "RecentImagesModel");
    qmlRegisterType<FileSystemModel>("org.krita.sketch", 1, 0, "FileSystemModel");

    qmlRegisterUncreatableType<LayerCompositeDetails>("org.krita.sketch", 1, 0, "LayerCompositeDetails", "This type is returned by the LayerModel class");

    d->view = new QDeclarativeView();
    d->view->setAttribute(Qt::WA_AcceptTouchEvents);

    d->view->engine()->addImageProvider(QLatin1String("presetthumb"), new PresetImageProvider);
    d->view->engine()->addImageProvider(QLatin1String("color"), new ColorImageProvider);
    d->view->engine()->addImageProvider(QLatin1String("recentimage"), new RecentImageImageProvider);
    d->view->engine()->addImageProvider("icon", new IconImageProvider);

    d->view->rootContext()->setContextProperty("Constants", d->constants);
    d->view->rootContext()->setContextProperty("Settings", d->settings);
    d->view->rootContext()->setContextProperty("RecentFileManager", d->recentFileManager);
    d->view->rootContext()->setContextProperty("KisClipBoard", KisClipboard::instance());
    d->view->rootContext()->setContextProperty("QMLEngine", d->view->engine());

    Welcome::MultiFeedRssModel *rssModel = new Welcome::MultiFeedRssModel(this);
    rssModel->addFeed(QLatin1String("http://feeds.feedburner.com/krita/news"));
    d->view->rootContext()->setContextProperty("aggregatedFeedsModel", rssModel);


    // This is needed because OpenGL viewport doesn't support partial updates.
    d->view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    QGLWidget* glWidget = new QGLWidget(this, KisGL2Canvas::shareWidget());
    glWidget->grabGesture(Qt::PanGesture);
    d->view->setViewport(glWidget);

    QDir appdir(qApp->applicationDirPath());
    // for now, the app in bin/ and we still use the env.bat script
    appdir.cdUp();

    d->view->engine()->addImportPath(appdir.canonicalPath() + "/share/apps/kritasketch/qml");
    QString mainqml = appdir.canonicalPath() + "/share/apps/kritasketch/qml/main.qml";

    Q_ASSERT(QFile::exists(mainqml));
    if (!QFile::exists(mainqml)) {
        QMessageBox::warning(0, "No QML found", mainqml + " doesn't exist.");
    }
    QFileInfo fi(mainqml);

    d->view->setSource(QUrl::fromLocalFile(fi.canonicalFilePath()));
    d->view->setResizeMode( QDeclarativeView::SizeRootObjectToView );

    //     if (d->view->errors().count() > 0) {
    //         foreach(const QDeclarativeError &error, d->view->errors()) {
    //             qDebug() << error.toString();
    //         }
    //     }



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
