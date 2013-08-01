/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 * Copyright (C) 2012 KO GmbH. Contact: Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "sketch/SketchDeclarativeView.h"
#include "sketch/RecentFileManager.h"
#include "sketch/DocumentManager.h"
#include <sketch/KisSketchPart.h>

#ifdef Q_OS_WIN
// Slate mode/docked detection stuff
#include <shellapi.h>
#define SM_CONVERTIBLESLATEMODE 0x2003
#define SM_SYSTEMDOCKED         0x2004
#endif

class MainWindow::Private
{
public:
    Private(MainWindow* qq) 
        : q(qq)
		, allowClose(true)
        , sketchView(0)
        , desktopView(0)
        , currentView(0)
		, slateMode(false)
		, docked(false)
    {
#ifdef Q_OS_WIN
		slateMode = (GetSystemMetrics(SM_CONVERTIBLESLATEMODE) == 0);
		docked = (GetSystemMetrics(SM_SYSTEMDOCKED) != 0);
#endif
	}
	MainWindow* q;
    bool allowClose;
    SketchDeclarativeView* sketchView;
    KoMainWindow* desktopView;
    QObject* currentView;
	
	bool slateMode;
	bool docked;

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

        KAction* toDesktop = new KAction(q);
        toDesktop->setText(tr("Switch to Desktop"));
        connect(toDesktop, SIGNAL(triggered(Qt::MouseButtons,Qt::KeyboardModifiers)), q, SLOT(switchToDesktop()));
        sketchView->engine()->rootContext()->setContextProperty("switchToDesktopAction", toDesktop);
    }

    void initDesktopView()
    {
        desktopView = new KoMainWindow(KisFactory2::componentData());

        KAction* toSketch = new KAction(q);
        toSketch->setText(tr("Switch to Sketch"));
        toSketch->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
        connect(toSketch, SIGNAL(triggered(Qt::MouseButtons,Qt::KeyboardModifiers)), q, SLOT(switchToSketch()));
        desktopView->actionCollection()->addAction("SwitchToSketchView", toSketch);
    }

	void notifySlateModeChange();
	void notifyDockingModeChange();
};

MainWindow::MainWindow(QStringList fileNames, QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ), d( new Private(this) )
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

    // Set the initial view to sketch... because reasons.
    switchToSketch();
}

void MainWindow::switchToSketch()
{
    QTime timer;
    timer.start();
    if(d->desktopView) {
        d->desktopView->setParent(0);
    }
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
    if(d->desktopView) {
        d->desktopView->setNoCleanup(true);
        d->desktopView->deleteLater();
        d->desktopView = 0;
    }
    d->initDesktopView();
    d->desktopView->setRootDocument(DocumentManager::instance()->document(), DocumentManager::instance()->part(), false);
}

bool MainWindow::allowClose() const
{
    return d->allowClose;
}

void MainWindow::setAllowClose(bool allow)
{
    d->allowClose = allow;
}

bool MainWindow::slateMode() const
{
	return d->slateMode;
}

void MainWindow::minimize()
{
    setWindowState(windowState() ^ Qt::WindowMinimized);
}

void MainWindow::closeWindow()
{
    //For some reason, close() does not work even if setAllowClose(true) was called just before this method.
    //So instead just completely quit the application, since we are using a single window anyway.
    DocumentManager::instance()->closeDocument();
    DocumentManager::instance()->part()->deleteLater();
    qApp->processEvents();
    QApplication::instance()->quit();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(d->allowClose) {
        event->accept();
        d->desktopView->close();
    } else {
        event->ignore();
        emit closeRequested();
    }
}

MainWindow::~MainWindow()
{
    delete d;
}

#ifdef Q_OS_WIN
bool MainWindow::winEvent( MSG * message, long * result )
{
	if(message->message == WM_SETTINGCHANGE)
	{
		if(wcscmp(TEXT("ConvertibleSlateMode"), (TCHAR *) message->lParam) == 0)
			d->notifySlateModeChange();
		else if(wcscmp(TEXT("SystemDockMode"), (TCHAR *) message->lParam) == 0)
			d->notifyDockingModeChange();
		*result = 0;
		return true;
	}
	return false;
}
#endif

void MainWindow::Private::notifySlateModeChange()
{
#ifdef Q_OS_WIN
	bool bSlateMode = (GetSystemMetrics(SM_CONVERTIBLESLATEMODE) == 0);
	
	if (slateMode != bSlateMode)
	{
		slateMode = bSlateMode;
		emit q->slateModeChanged();
		if(slateMode)
			q->switchToSketch();
		else
			q->switchToDesktop();
		qDebug() << "Slate mode is now" << slateMode;
	} 
#endif
}

void MainWindow::Private::notifyDockingModeChange()
{
#ifdef Q_OS_WIN
	bool bDocked = (GetSystemMetrics(SM_SYSTEMDOCKED) != 0);

	if (docked != bDocked)
	{
		docked = bDocked;
		qDebug() << "Docking mode is now" << docked;
	}
#endif
}

#include "MainWindow.moc"
