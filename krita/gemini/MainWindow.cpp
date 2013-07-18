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

#include "opengl/kis_opengl.h"

#include <QApplication>
#include <QResizeEvent>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>
#include <QGLWidget>

#include <kcmdlineargs.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kaboutdata.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorSpace.h>
#include <KoMainWindow.h>

#include "kis_config.h"
#include <kis_factory2.h>
#include <kis_doc2.h>

#include "cpuid.h"
#include "sketch/SketchDeclarativeView.h"
#include "sketch/RecentFileManager.h"
#include "sketch/DocumentManager.h"
#include <sketch/KisSketchPart.h>

class MainWindow::Private
{
public:
    Private() 
        : allowClose(true)
        , sketchView(0)
        , desktopView(0)
        , currentView(0)
    { }
    bool allowClose;
    SketchDeclarativeView* sketchView;
    KoMainWindow* desktopView;
    QObject* currentView;

    void initSketchView(QObject* parent)
    {
        sketchView = new SketchDeclarativeView();
        sketchView->engine()->rootContext()->setContextProperty("mainWindow", parent);

        QDir appdir(qApp->applicationDirPath());
        // for now, the app in bin/ and we still use the env.bat script
        appdir.cdUp();

        sketchView->engine()->addImportPath(appdir.canonicalPath() + "/lib/calligra/imports");
        sketchView->engine()->addImportPath(appdir.canonicalPath() + "/lib64/calligra/imports");
        QString mainqml = appdir.canonicalPath() + "/share/apps/kritagemini/kritagemini.qml";

        Q_ASSERT(QFile::exists(mainqml));
        if (!QFile::exists(mainqml)) {
            QMessageBox::warning(0, "No QML found", mainqml + " doesn't exist.");
        }
        QFileInfo fi(mainqml);

        sketchView->setSource(QUrl::fromLocalFile(fi.canonicalFilePath()));
        sketchView->setResizeMode( QDeclarativeView::SizeRootObjectToView );
    }

    void initDesktopView()
    {
        desktopView = new KoMainWindow(KisFactory2::componentData());
    }
};

MainWindow::MainWindow(QStringList fileNames, QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ), d( new Private )
{
    qApp->setActiveWindow( this );

    setWindowTitle(i18n("Krita Gemini"));

    KisConfig cfg;
    cfg.setCursorStyle(CURSOR_STYLE_NO_CURSOR);
    cfg.setUseOpenGL(true);

    foreach(QString fileName, fileNames) {
        DocumentManager::instance()->recentFileManager()->addRecent(fileName);
    }

    connect(DocumentManager::instance(), SIGNAL(documentChanged()), SLOT(documentChanged()));

    d->initSketchView(this);
    d->initDesktopView();

    KAction* toDesktop = new KAction(this);
    toDesktop->setText(tr("Switch to Desktop"));
    connect(toDesktop, SIGNAL(triggered(Qt::MouseButtons,Qt::KeyboardModifiers)), SLOT(switchToDesktop()));
    d->sketchView->engine()->rootContext()->setContextProperty("switchToDesktopAction", toDesktop);

    KAction* toSketch = new KAction(this);
    toSketch->setText(tr("Switch to Sketch"));
    toSketch->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    connect(toSketch, SIGNAL(triggered(Qt::MouseButtons,Qt::KeyboardModifiers)), SLOT(switchToSketch()));
    d->desktopView->actionCollection()->addAction("SwitchToSketchView", toSketch);

    // Set the initial view to sketch... because reasons.
    switchToSketch();
}

void MainWindow::switchToSketch()
{
    QTime timer;
    timer.start();
    d->desktopView->setParent(0);
    setCentralWidget(d->sketchView);
    emit switchedToSketch();
    qApp->processEvents();
    qDebug() << "milliseconds to switch to sketch:" << timer.elapsed();
}

void MainWindow::switchToDesktop()
{
    QTime timer;
    timer.start();
    d->sketchView->setParent(0);
    setCentralWidget(d->desktopView);
    qApp->processEvents();
    qDebug() << "milliseconds to switch to desktop:" << timer.elapsed();
}

void MainWindow::documentChanged()
{
    d->desktopView->setRootDocument(DocumentManager::instance()->document(), DocumentManager::instance()->part());
}

bool MainWindow::allowClose() const
{
    return d->allowClose;
}

void MainWindow::setAllowClose(bool allow)
{
    d->allowClose = allow;
}

void MainWindow::minimize()
{
    setWindowState(windowState() ^ Qt::WindowMinimized);
}

void MainWindow::closeWindow()
{
    //For some reason, close() does not work even if setAllowClose(true) was called just before this method.
    //So instead just completely quit the application, since we are using a single window anyway.
    QApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(!d->allowClose) {
        event->ignore();
        emit closeRequested();
    } else {
        event->accept();
        d->desktopView->close();
    }
}

MainWindow::~MainWindow()
{
    delete d;
}

#include "MainWindow.moc"
