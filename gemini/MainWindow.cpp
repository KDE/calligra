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
#include <QHBoxLayout>
#include "desktopviewproxy.h"

#include <QApplication>
#include <QResizeEvent>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include <QGraphicsObject>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QToolButton>
#include <QMenuBar>
#include <QAction>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QUrl>
#include <QStandardPaths>

#include <kactioncollection.h>
#include <ktoolbar.h>
#include <kmessagebox.h>
#include <KConfigGroup>
#include <KSharedConfig>

#include <gemini/ViewModeSwitchEvent.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoMainWindow.h>
#include <KoGlobal.h>
#include <KoDocumentInfo.h>
#include <KoView.h>
#include <KoPart.h>
#include <KoDocumentEntry.h>
#include <KoFilterManager.h>
#include <part/KWFactory.h>
#include <stage/part/KPrDocument.h>
#include <stage/part/KPrFactory.h>
#include <stage/part/KPrViewModePresentation.h>
#include <KoAbstractGradient.h>
#include <KoZoomController.h>
#include <KoFileDialog.h>
#include <KoDialog.h>
#include <KoIcon.h>
#include <KoConfig.h> // CALLIGRA_OLD_PLUGIN_METADATA

#include "PropertyContainer.h"
#include "RecentFileManager.h"
#include "DocumentManager.h"
#include "QmlGlobalEngine.h"
#include "Settings.h"
#include "Theme.h"
#include "DocumentListModel.h"
#include "Constants.h"
#include "SimpleTouchArea.h"
#include "ToolManager.h"
#include "ParagraphStylesModel.h"
#include "KeyboardModel.h"
#include "ScribbleArea.h"
#include "RecentImageImageProvider.h"
#include "RecentFilesModel.h"
#include "TemplatesModel.h"
#include "CloudAccountsModel.h"

#ifdef Q_OS_WIN
// Slate mode/docked detection stuff
#include <Shellapi.h>
#define SM_CONVERTIBLESLATEMODE 0x2003
#define SM_SYSTEMDOCKED         0x2004
#endif

class MainWindow::Private
{
public:
    Private(MainWindow* qq)
        : q(qq)
        , allowClose(true)
        , touchView(0)
        , desktopView(0)
        , currentView(0)
        , settings(0)
        , slateMode(false)
        , docked(false)
        , touchKoView(0)
        , touchEventReceiver(0)
        , desktopKoView(0)
        , desktopViewProxy(0)
        , forceDesktop(false)
        , forceTouch(false)
        , temporaryFile(false)
        , syncObject(0)
        , toDesktop(0)
        , toTouch(0)
        , switcher(0)
        , alternativeSaveAction(0)
    {
#ifdef Q_OS_WIN
        // slateMode = (GetSystemMetrics(SM_CONVERTIBLESLATEMODE) == 0);
        // docked = (GetSystemMetrics(SM_SYSTEMDOCKED) != 0);
#endif
        fullScreenThrottle = new QTimer(qq);
        fullScreenThrottle->setInterval(500);
        fullScreenThrottle->setSingleShot(true);
    }
    MainWindow* q;
    bool allowClose;
    QQuickWidget* touchView;
    QPointer<KoMainWindow> desktopView;
    QObject* currentView;
    Settings *settings;

    bool slateMode;
    bool docked;
    QString currentTouchPage;
    KoView* touchKoView;
    QObject* touchEventReceiver;
    KoView* desktopKoView;
    DesktopViewProxy* desktopViewProxy;

    bool forceDesktop;
    bool forceTouch;
    bool temporaryFile;
    ViewModeSynchronisationObject* syncObject;

    QAction* toDesktop;
    QAction* toTouch;
    QToolButton* switcher;
    QAction* alternativeSaveAction;
    QTimer* fullScreenThrottle;

    void shouldAcceptTouchEvents(QWidget* widget) {
        // See https://bugreports.qt.io/browse/QTBUG-66718
        static QVersionNumber qtVersion = QVersionNumber::fromString(qVersion());
        static bool shouldWidgetAcceptTouchEvents = qtVersion > QVersionNumber(5, 9, 3) && qtVersion.normalized() != QVersionNumber(5, 10);
        if(shouldWidgetAcceptTouchEvents)
        {
            widget->setAttribute(Qt::WA_AcceptTouchEvents, true);
        }
    }

    void initTouchView(QObject* parent)
    {
        touchView = new QQuickWidget();
        shouldAcceptTouchEvents(touchView);
        QmlGlobalEngine::instance()->setEngine(touchView->engine());
        touchView->engine()->addImageProvider(QLatin1String("recentimage"), new RecentImageImageProvider);
        touchView->engine()->rootContext()->setContextProperty("mainWindow", parent);

        settings = new Settings( q );
        DocumentManager::instance()->setSettingsManager( settings );
        touchView->engine()->rootContext()->setContextProperty("DocumentManager", DocumentManager::instance());
        touchView->engine()->rootContext()->setContextProperty("Settings", settings);
        touchView->engine()->rootContext()->setContextProperty("Constants", new Constants( q ));
        touchView->engine()->rootContext()->setContextProperty("RecentFileManager", DocumentManager::instance()->recentFileManager());
        touchView->engine()->rootContext()->setContextProperty("WORDS_MIME_TYPE", QString(WORDS_MIME_TYPE));
        touchView->engine()->rootContext()->setContextProperty("STAGE_MIME_TYPE", QString(STAGE_MIME_TYPE));

#ifdef Q_OS_WIN
        QDir appdir(qApp->applicationDirPath());

        // Corrects for mismatched case errors in path (qtdeclarative fails to load)
        wchar_t buffer[1024];
        QString absolute = appdir.absolutePath();
        DWORD rv = ::GetShortPathName((wchar_t*)absolute.utf16(), buffer, 1024);
        rv = ::GetLongPathName(buffer, buffer, 1024);
        QString correctedPath((QChar *)buffer);
        appdir.setPath(correctedPath);

        // for now, the app in bin/ and we still use the env.bat script
        appdir.cdUp();

        // QT5TODO: adapt to QML_IMPORT_PATH usage and install to ${QML_INSTALL_DIR}
        touchView->engine()->addImportPath(appdir.canonicalPath() + "/imports");
        touchView->engine()->addImportPath(appdir.canonicalPath() + "/lib/calligra/imports");
        touchView->engine()->addImportPath(appdir.canonicalPath() + "/lib64/calligra/imports");
        touchView->engine()->addImportPath(appdir.canonicalPath() + "/bin/data/calligragemini");
        QString mainqml = appdir.canonicalPath() + "/bin/data/calligragemini/calligragemini.qml";
#else
        QString mainqml = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("calligragemini/calligragemini.qml"));
#endif

        Q_ASSERT(QFile::exists(mainqml));
        if (!QFile::exists(mainqml)) {
            QMessageBox::warning(0, "No QML found", mainqml + " doesn't exist.");
        }
        QFileInfo fi(mainqml);

        touchView->setSource(QUrl::fromLocalFile(fi.canonicalFilePath()));
        touchView->setResizeMode( QQuickWidget::SizeRootObjectToView );

        toDesktop = new QAction(q);
        toDesktop->setEnabled(true);
        toDesktop->setText(tr("Switch to Desktop"));
        // useful for monkey-testing to crash...
        //toDesktop->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_D);
        //q->addAction(toDesktop);
        //connect(toDesktop, SIGNAL(triggered(bool)), q, SLOT(switchDesktopForced()));
        connect(toDesktop, SIGNAL(triggered(bool)), q, SLOT(switchToDesktop()));
        touchView->engine()->rootContext()->setContextProperty("switchToDesktopAction", toDesktop);
    }

    void initDesktopView()
    {
        if(settings->currentFile().isEmpty()) {
            return;
        }
        // Initialize all Calligra directories etc.
        KoGlobal::initialize();

        // The default theme is not what we want for Gemini
        KConfigGroup group(KSharedConfig::openConfig(), "theme");
        if(group.readEntry("Theme", "no-theme-is-set") == QLatin1String("no-theme-is-set")) {
            group.writeEntry("Theme", "Krita-dark");
        }

        if(settings->currentFileClass() == WORDS_MIME_TYPE) {
            qApp->setApplicationName("calligrawords");
            desktopView = new KoMainWindow(WORDS_MIME_TYPE, KWFactory::componentData());
        } else if(settings->currentFileClass() == STAGE_MIME_TYPE) {
            qApp->setApplicationName("calligrastage");
            desktopView = new KoMainWindow(STAGE_MIME_TYPE, KPrFactory::componentData());
        } else {
            desktopView = 0;
            qDebug() << "Big trouble, things gonna break. desktopView is not created." << settings->currentFileClass();
            return;
        }

        toTouch = new QAction(desktopView);
        toTouch->setEnabled(false);
        toTouch->setText(tr("Switch to Touch"));
        toTouch->setIcon(koIcon("system-reboot"));
        toTouch->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
        //connect(toTouch, SIGNAL(triggered(Qt::MouseButtons,Qt::KeyboardModifiers)), q, SLOT(switchTouchForced()));
        connect(toTouch, &QAction::triggered, q, &MainWindow::switchToTouch);
        desktopView->actionCollection()->addAction("SwitchToTouchView", toTouch);
        switcher = new QToolButton();
        switcher->setEnabled(false);
        switcher->setText(tr("Switch to Touch"));
        switcher->setIcon(koIcon("system-reboot"));
        switcher->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //connect(switcher, SIGNAL(clicked(bool)), q, SLOT(switchDesktopForced()));
        connect(switcher, SIGNAL(clicked(bool)), q, SLOT(switchToTouch()));
        desktopView->menuBar()->setCornerWidget(switcher);

        // DesktopViewProxy connects itself up to everything appropriate on construction,
        // and destroys itself again when the view is removed
        desktopViewProxy = new DesktopViewProxy(q, desktopView);
        connect(desktopViewProxy, SIGNAL(documentSaved()), q, SIGNAL(documentSaved()));
        connect(desktopViewProxy, SIGNAL(documentSaved()), q, SLOT(resetWindowTitle()));
        connect(desktopViewProxy, SIGNAL(documentSaved()), q, SLOT(enableAltSaveAction()));
    }

    void notifySlateModeChange();
    void notifyDockingModeChange();
    bool queryClose();
    void altSaveQuery();
};

MainWindow::MainWindow(QStringList fileNames, QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ), d( new Private(this) )
{
    qmlRegisterUncreatableType<PropertyContainer>("org.calligra", 1, 0, "PropertyContainer", "Contains properties and naively extends QML to support dynamic properties");
    qmlRegisterType<Theme>("org.calligra", 1, 0, "Theme");
    qmlRegisterType<DocumentListModel>("org.calligra", 1, 0, "DocumentListModel");
    qmlRegisterType<SimpleTouchArea>("org.calligra", 1, 0, "SimpleTouchArea");
    qmlRegisterType<ToolManager>("org.calligra", 1, 0, "ToolManager");
    qmlRegisterType<ParagraphStylesModel>("org.calligra", 1, 0, "ParagraphStylesModel");
    qmlRegisterType<KeyboardModel>("org.calligra", 1, 0, "KeyboardModel");
    qmlRegisterType<ScribbleArea>("org.calligra", 1, 0, "ScribbleArea");
    qmlRegisterType<RecentFilesModel>("org.calligra", 1, 0, "RecentFilesModel");
    qmlRegisterType<TemplatesModel>("org.calligra", 1, 0, "TemplatesModel");
    qmlRegisterType<CloudAccountsModel>("org.calligra", 1, 0, "CloudAccountsModel");
    qmlRegisterType<KPrViewModePresentation>();
    qRegisterMetaType<QAction*>();

    qApp->setActiveWindow( this );
    setWindowTitle(i18n("Calligra Gemini"));
    setWindowIcon(koIcon("calligragemini"));//gemini"));
    resize(QApplication::desktop()->availableGeometry().size() * 3/4);
    d->shouldAcceptTouchEvents(this);

    foreach(const QString &fileName, fileNames) {
        DocumentManager::instance()->recentFileManager()->addRecent( QDir::current().absoluteFilePath( fileName ) );
    }

    connect(DocumentManager::instance(), SIGNAL(documentChanged()), SLOT(documentChanged()));
    connect(DocumentManager::instance(), SIGNAL(documentChanged()), SLOT(resetWindowTitle()));
    connect(DocumentManager::instance(), SIGNAL(documentSaved()), SLOT(resetWindowTitle()));
    connect(DocumentManager::instance(), SIGNAL(documentSaved()), SLOT(enableAltSaveAction()));
    connect(DocumentManager::instance(), SIGNAL(aboutToDeleteDocument()), SLOT(closeWindow()));

    d->initTouchView(this);

    // Set the initial view to touch... because reasons.
    // Really, this allows us to show the pleasant welcome screen from Touch
    switchToTouch();

    if(!fileNames.isEmpty()) {
        //It feels a little hacky, but call a QML function to open files.
        //This saves a lot of hassle required to change state for loading dialogs etc.
        QMetaObject::invokeMethod(d->touchView->rootObject(), "openFile", Q_ARG(QVariant, fileNames.at(0)));
    }
}

void MainWindow::resetWindowTitle()
{
    KoDocument* document = DocumentManager::instance()->document();
    if (!document)
        return;
    QUrl url = document->url();
    QString fileName = url.fileName();
    if(url.scheme() == "temp" || url.isEmpty())
        fileName = i18n("Untitled");

    KoDialog::CaptionFlags flags = KoDialog::HIGCompliantCaption;
    if ( document->isModified() ) {
        flags |= KoDialog::ModifiedCaption;
    }

    setWindowTitle( KoDialog::makeStandardCaption(fileName, this, flags) );
}

void MainWindow::switchDesktopForced()
{
    if (d->slateMode)
        d->forceDesktop = true;
    d->forceTouch = false;
}

void MainWindow::switchTouchForced()
{
    if (!d->slateMode)
        d->forceTouch = true;
    d->forceDesktop = false;
}

void MainWindow::switchToTouch()
{
    QTime timer;
    timer.start();
    qDebug() << "Switching to touch";

    if (d->toTouch)
    {
        d->toTouch->setEnabled(false);
        d->switcher->setEnabled(false);
    }

    d->syncObject = new ViewModeSynchronisationObject;

    if (d->desktopView && centralWidget() == d->desktopView) {
        if (KoView* view = d->desktopView->rootView()) {
            //Notify the view we are switching away from that we are about to switch away from it
            //giving it the possibility to set up the synchronisation object.
            ViewModeSwitchEvent aboutToSwitchEvent(ViewModeSwitchEvent::AboutToSwitchViewModeEvent, view, d->touchView, d->syncObject);
            QApplication::sendEvent(view, &aboutToSwitchEvent);
        }
        d->desktopView->setParent(0);
    }

    setCentralWidget(d->touchView);
    qApp->processEvents();
    d->touchView->setVisible(true);
    resize(size());
    emit switchedToTouch();

    if (d->slateMode) {
        if (d->syncObject->initialized)
            QTimer::singleShot(50, this, SLOT(touchChange()));
    }
    else
        QTimer::singleShot(50, this, SLOT(touchChange()));

    //qDebug() << "milliseconds to switch to touch:" << timer.elapsed();
}

void MainWindow::touchChange()
{
    if (centralWidget() != d->touchView || !d->syncObject)
        return;

    if (d->desktopView)
    {
        //if (/*!d->touchKoView ||*/ !d->touchView->canvasWidget())
        //{
        //    QTimer::singleShot(100, this, SLOT(touchChange()));
        //    return;
        //}
        qApp->processEvents();
        KoView* view = d->desktopView->rootView();
        //Notify the new view that we just switched to it, passing our synchronisation object
        //so it can use those values to sync with the old view.
        ViewModeSwitchEvent switchedEvent(ViewModeSwitchEvent::SwitchedToTouchModeEvent, view, d->touchView, d->syncObject);
        QApplication::sendEvent(d->touchEventReceiver, &switchedEvent);
        d->syncObject = 0;
        qApp->processEvents();
    }
    if (d->toDesktop)
    {
        qApp->processEvents();
        d->toDesktop->setEnabled(true);
    }
}

void MainWindow::switchToDesktop()
{
    QTime timer;
    timer.start();
    qDebug() << "Switching to desktop";

    if (d->toDesktop)
        d->toDesktop->setEnabled(false);

    ViewModeSynchronisationObject* syncObject = new ViewModeSynchronisationObject;

    KoView* view = 0;
    if (d->desktopView) {
        view = d->desktopView->rootView();
    }

    if (!view) {
        return;
    }

    //Notify the view we are switching away from that we are about to switch away from it
    //giving it the possibility to set up the synchronisation object.
    ViewModeSwitchEvent aboutToSwitchEvent(ViewModeSwitchEvent::AboutToSwitchViewModeEvent, d->touchView, view, syncObject);
    QApplication::sendEvent(d->touchEventReceiver, &aboutToSwitchEvent);
    qApp->processEvents();

    if (d->currentTouchPage == "MainPage")
    {
        d->touchView->setParent(0);
        d->touchView->setVisible(false);
        setCentralWidget(d->desktopView);
    }

    //Notify the new view that we just switched to it, passing our synchronisation object
    //so it can use those values to sync with the old view.
    ViewModeSwitchEvent switchedEvent(ViewModeSwitchEvent::SwitchedToDesktopModeEvent, d->touchView, view, syncObject);
    QApplication::sendEvent(view, &switchedEvent);

    qApp->processEvents();
    d->toTouch->setEnabled(true);
    d->switcher->setEnabled(true);

    //qDebug() << "milliseconds to switch to desktop:" << timer.elapsed();
}

void MainWindow::setDocAndPart(QObject* document, QObject* part)
{
    if(DocumentManager::instance()->document()) {
        disconnect(DocumentManager::instance()->document(), SIGNAL(modified(bool)), this, SLOT(resetWindowTitle()));
    }
    qDebug() << "Attempting to set doc and part to" << document << "and" << part;
    d->touchEventReceiver = d->touchView->rootObject()->findChild<QQuickItem*>("controllerItem");
    DocumentManager::instance()->setDocAndPart(qobject_cast<KoDocument*>(document), qobject_cast<KoPart*>(part));
    if(DocumentManager::instance()->document()) {
        connect(DocumentManager::instance()->document(), SIGNAL(modified(bool)), this, SLOT(resetWindowTitle()));
    }
    if(document && part && !d->settings->currentFile().isEmpty()) {
        QAction* undo = qobject_cast<KoPart*>(part)->views().at(0)->action("edit_undo");
        d->touchView->rootContext()->setContextProperty("undoaction", undo);
        QAction* redo = qobject_cast<KoPart*>(part)->views().at(0)->action("edit_redo");
        d->touchView->rootContext()->setContextProperty("redoaction", redo);
    }
    resetWindowTitle();
}

void MainWindow::documentChanged()
{
    if (d->desktopView) {
        d->desktopView->deleteLater();
        d->desktopView = 0;
        qApp->processEvents();
    }
    d->initDesktopView();
    if(d->desktopView) {
        d->desktopView->setRootDocument(DocumentManager::instance()->document(), DocumentManager::instance()->part(), false);
        qApp->processEvents();
        d->desktopKoView = d->desktopView->rootView();
        emit desktopKoViewChanged();
    //    d->desktopKoView->setQtMainWindow(d->desktopView);
    //    connect(d->desktopKoView, SIGNAL(sigLoadingFinished()), d->centerer, SLOT(start()));
    //    connect(d->desktopKoView, SIGNAL(sigSavingFinished()), this, SLOT(resetWindowTitle()));
    //    KWView* wordsview = qobject_cast<KWView*>(d->desktopView->rootView());
    //    if(wordsview) {
    //        connect(wordsview->canvasBase()->resourceManager(), SIGNAL(canvasResourceChanged(int, const QVariant&)),
    //                this, SLOT(resourceChanged(int, const QVariant&)));
    //    }
        if (!d->forceTouch && !d->slateMode)
            switchToDesktop();
    }
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

QString MainWindow::currentTouchPage() const
{
    return d->currentTouchPage;
}

void MainWindow::setCurrentTouchPage(QString newPage)
{
    d->currentTouchPage = newPage;
    emit currentTouchPageChanged();

    if (newPage == "MainPage")
    {
        if (!d->forceTouch && !d->slateMode)
        {
            // Just loaded to desktop, do nothing
        }
        else
        {
            //QTimer::singleShot(3000, this, SLOT(adjustZoomOnDocumentChangedAndStuff()));
        }
    }
}

void MainWindow::setAlternativeSaveAction(QAction* altAction)
{
    // if mainwindow exists, and alt action exists, remove alt action from current mainwindow
    if(d->desktopView && d->alternativeSaveAction) {
        d->desktopView->actionCollection()->removeAction(d->alternativeSaveAction);
        d->desktopView->actionCollection()->action("file_save")->disconnect(d->alternativeSaveAction);
    }
    d->alternativeSaveAction = altAction;
    // if mainwindow exists, set alt action into current mainwindow
    if(d->desktopView && d->alternativeSaveAction) {
        QAction* cloudSave = d->desktopView->actionCollection()->addAction("cloud_save", d->alternativeSaveAction);
        KToolBar* tb = d->desktopView->toolBar("mainToolBar");
        if(tb) {
            tb->removeAction(cloudSave);
            // find the action /after/ the save action (because we want the alt save there, not before it)
            QAction* saveAction = d->desktopView->actionCollection()->action("file_save");
            QAction* afterSave = 0;
            bool useNext = false;
            Q_FOREACH(QAction* action, tb->actions()) {
                if(useNext) {
                    afterSave = action;
                    break;
                }
                if(action == saveAction) {
                    useNext = true;
                }
            }
            if(afterSave) {
                tb->insertAction(afterSave, cloudSave);
            } else {
                tb->addAction(cloudSave);
            }
        }
    }
    if(d->alternativeSaveAction) {
        // disabled for a start - this is called on load completion, so let's just assume we're not ready to reupload yet
        d->alternativeSaveAction->setEnabled(false);
    }
}

void MainWindow::enableAltSaveAction()
{
    if(d->alternativeSaveAction) {
        d->alternativeSaveAction->setEnabled(true);
    }
}

void MainWindow::openFile()
{
    QStringList mimeFilter;

    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType(WORDS_MIME_TYPE);
    if (!entry.isEmpty()) {
        QJsonObject json = entry.metaData();
#ifdef CALLIGRA_OLD_PLUGIN_METADATA
        QStringList mimeTypes = json.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
#else
        QStringList mimeTypes = json.value("X-KDE-ExtraNativeMimeTypes").toVariant().toStringList();
#endif

        mimeFilter << KoFilterManager::mimeFilter(WORDS_MIME_TYPE,
                                                  KoFilterManager::Import,
                                                  mimeTypes);
    }
    entry = KoDocumentEntry::queryByMimeType(STAGE_MIME_TYPE);
    if (!entry.isEmpty()) {
        QJsonObject json = entry.metaData();
#ifdef CALLIGRA_OLD_PLUGIN_METADATA
        QStringList mimeTypes = json.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
#else
        QStringList mimeTypes = json.value("X-KDE-ExtraNativeMimeTypes").toVariant().toStringList();
#endif
        mimeFilter << KoFilterManager::mimeFilter(STAGE_MIME_TYPE,
                                                  KoFilterManager::Import,
                                                  mimeTypes);
    }

    KoFileDialog dialog(d->desktopView, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setCaption(i18n("Open Document"));
    dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dialog.setMimeTypeFilters(mimeFilter);
    QString filename = dialog.filename();
    if(!filename.isEmpty()) {
        QMetaObject::invokeMethod(d->touchView->rootObject(), "openFile", Q_ARG(QVariant, filename), Q_ARG(QVariant, 0));
    }
}

bool MainWindow::temporaryFile() const
{
    return d->temporaryFile;
}

void MainWindow::setTemporaryFile(bool newValue)
{
    d->temporaryFile = newValue;
    emit temporaryFileChanged();
}

bool MainWindow::fullScreen() const
{
    return Qt::WindowFullScreen == (windowState() & Qt::WindowFullScreen);
}

void MainWindow::setFullScreen(bool newValue)
{
    if(newValue) {
        if(d->fullScreenThrottle->isActive()) {
            // not a good thing... you need to avoid this happening. This exists to avoid a death-loop,
            // such as what might happen if readermode is enabled when the window is not maximised
            // as this causes a resize loop which makes readermode switch between enabled and disabled,
            // which in turn makes fullScreen be set and reset all the time... very bad, so let's try
            // and avoid that.
        }
        else {
            setWindowState(windowState() | Qt::WindowFullScreen);
        }
    }
    else {
        // this is really unpleasant... however, fullscreen is very twitchy, and exiting it as below
        // will cause an inconsistent state, so we simply assume exiting fullscreen leaves you maximised.
        // It isn't optimal, but it is the best state for now, this has taken too long to work out.
        // setWindowState(windowState() & ~Qt::WindowFullScreen);
        // should really do it, but... it doesn't. So, we end up with what we have next:
        showMaximized();
    }
    d->fullScreenThrottle->start();
    emit fullScreenChanged();
}

QObject* MainWindow::desktopKoView() const
{
    return d->desktopKoView;
}

int MainWindow::lastScreen() const
{
    QDesktopWidget desktop;
    return desktop.screenCount() - 1;
}

void MainWindow::resourceChanged(int key, const QVariant& v)
{
    Q_UNUSED(key)
    Q_UNUSED(v)
    if(centralWidget() == d->touchView)
        return;
}

void MainWindow::resourceChangedTouch(int key, const QVariant& v)
{
    Q_UNUSED(key)
    Q_UNUSED(v)
    if(centralWidget() == d->desktopView)
        return;
}

void MainWindow::minimize()
{
    setWindowState(windowState() ^ Qt::WindowMinimized);
}

void MainWindow::closeWindow()
{
    if (d->desktopView) {
        d->desktopView->setNoCleanup(true);
        if (centralWidget() == d->desktopView)
            d->allowClose = d->queryClose();
    }

    if (d->allowClose)
    {
        d->altSaveQuery();
        d->settings->setCurrentFile("");
    }
    qApp->processEvents();
    qApp->quit();
}

bool MainWindow::Private::queryClose()
{
    desktopView->setNoCleanup(true);
    if (DocumentManager::instance()->document() == 0)
        return true;

    // main doc + internally stored child documents
    if (DocumentManager::instance()->document()->isModified()) {
        QString name;
        if (DocumentManager::instance()->document()->documentInfo()) {
            name = DocumentManager::instance()->document()->documentInfo()->aboutInfo("title");
        }
        if (name.isEmpty())
            name = DocumentManager::instance()->document()->url().fileName();

        if (name.isEmpty())
            name = i18n("Untitled");

        int res = KMessageBox::warningYesNoCancel(q,
                  i18n("<p>The document <b>'%1'</b> has been modified.</p><p>Do you want to save it?</p>", name),
                  QString(),
                  KStandardGuiItem::save(),
                  KStandardGuiItem::discard());

        switch (res) {
        case KMessageBox::Yes : {
            if (DocumentManager::instance()->isTemporaryFile() && !desktopViewProxy->fileSaveAs())
                return false;
            if (!DocumentManager::instance()->save())
                return false;
            break;
        }
        case KMessageBox::No :
            DocumentManager::instance()->document()->removeAutoSaveFiles();
            DocumentManager::instance()->document()->setModified(false);   // Now when queryClose() is called by closeEvent it won't do anything.
            break;
        default : // case KMessageBox::Cancel :
            return false;
        }
    }
    return true;
}

void MainWindow::Private::altSaveQuery()
{
    if(alternativeSaveAction && alternativeSaveAction->isEnabled())
    {
        int res = KMessageBox::warningYesNo(q, i18n("<p>The cloud copy of the document is out of date. Do you want to upload a new copy?</p>"));
        switch (res) {
        case KMessageBox::Yes : {
            alternativeSaveAction->trigger();
            while(alternativeSaveAction->isEnabled()) {
                qApp->processEvents();
            }
            break;
        }
        case KMessageBox::No :
        default:
            break;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (centralWidget() == d->desktopView)
    {
        KoDocument* document = DocumentManager::instance()->document();
        if (document && document->isLoading()) {
            event->ignore();
            return;
        }
    }

    event->accept();
    closeWindow();
}

MainWindow::~MainWindow()
{
    delete d;
}

#ifdef Q_OS_WIN
bool MainWindow::winEvent( MSG * message, long * result )
{
    if (message && message->message == WM_SETTINGCHANGE && message->lParam)
    {
        if (wcscmp(TEXT("ConvertibleSlateMode"), (TCHAR *) message->lParam) == 0)
            d->notifySlateModeChange();
        else if (wcscmp(TEXT("SystemDockMode"), (TCHAR *) message->lParam) == 0)
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
        if (forceTouch || (slateMode && !forceDesktop))
        {
            if (!toTouch || (toTouch && toTouch->isEnabled()))
                q->switchToTouch();
        }
        else
        {
                q->switchToDesktop();
        }
        //qDebug() << "Slate mode is now" << slateMode;
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
        //qDebug() << "Docking mode is now" << docked;
    }
#endif
}
