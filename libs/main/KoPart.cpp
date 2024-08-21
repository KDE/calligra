/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
 * SPDX-FileCopyrightText: 2000-2005 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2010-2012 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 Inge Wallin <ingwa@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPart.h"

#include "KoApplication.h"
#include "KoDocument.h"
#include "KoFilterManager.h"
#include "KoMainWindow.h"
#include "KoOpenPane.h"
#include "KoView.h"
#include <KoComponentData.h>

#include <KoCanvasController.h>
#include <KoCanvasControllerWidget.h>
#include <KoResourcePaths.h>

#include <KConfigGroup>
#include <KSharedConfig>
#include <MainDebug.h>
#include <kdesktopfile.h>
#include <kxmlguifactory.h>

#include <QFileInfo>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QMimeDatabase>

#ifdef WITH_QTDBUS
#include "KoPartAdaptor.h"
#include <QDBusConnection>
#endif

class Q_DECL_HIDDEN KoPart::Private
{
public:
    Private(const KoComponentData &componentData_, KoPart *_parent)
        : parent(_parent)
        , document(nullptr)
        , proxyWidget(nullptr)
        , startUpWidget(nullptr)
        , componentData(componentData_)
    {
    }

    ~Private()
    {
        delete proxyWidget;
    }

    KoPart *parent;

    QList<KoView *> views;
    QList<KoMainWindow *> mainWindows;
    KoDocument *document;
    QList<KoDocument *> documents;
    QPointer<QGraphicsProxyWidget> proxyWidget;
    QPointer<KoOpenPane> startUpWidget;
    QString templatesResourcePath;

    KoComponentData componentData;
};

KoPart::KoPart(const KoComponentData &componentData, QObject *parent)
    : QObject(parent)
    , d(new Private(componentData, this))
{
#ifdef WITH_QTDBUS
    new KoPartAdaptor(this);
    QDBusConnection::sessionBus().registerObject('/' + objectName(), this);
#endif
}

KoPart::~KoPart()
{
    // Tell our views that the document is already destroyed and
    // that they shouldn't try to access it.
    foreach (KoView *view, views()) {
        view->setDocumentDeleted();
    }

    while (!d->mainWindows.isEmpty()) {
        delete d->mainWindows.takeFirst();
    }

    delete d->startUpWidget;
    d->startUpWidget = nullptr;

    delete d;
}

KoComponentData KoPart::componentData() const
{
    return d->componentData;
}

void KoPart::setDocument(KoDocument *document)
{
    Q_ASSERT(document);
    d->document = document;
}

KoDocument *KoPart::document() const
{
    return d->document;
}

KoView *KoPart::createView(KoDocument *document, QWidget *parent)
{
    KoView *view = createViewInstance(document, parent);
    addView(view, document);
    if (!d->documents.contains(document)) {
        d->documents.append(document);
    }
    return view;
}

void KoPart::addView(KoView *view, KoDocument *document)
{
    if (!view)
        return;

    if (!d->views.contains(view)) {
        d->views.append(view);
    }
    if (!d->documents.contains(document)) {
        d->documents.append(document);
    }

    view->updateReadWrite(document->isReadWrite());

    if (d->views.size() == 1) {
        KoApplication *app = qobject_cast<KoApplication *>(qApp);
        if (nullptr != app) {
            Q_EMIT app->documentOpened('/' + objectName());
        }
    }
}

void KoPart::removeView(KoView *view)
{
    d->views.removeAll(view);

    if (d->views.isEmpty()) {
        KoApplication *app = qobject_cast<KoApplication *>(qApp);
        if (nullptr != app) {
            Q_EMIT app->documentClosed('/' + objectName());
        }
    }
}

QList<KoView *> KoPart::views() const
{
    return d->views;
}

int KoPart::viewCount() const
{
    return d->views.count();
}

QGraphicsItem *KoPart::canvasItem(KoDocument *document, bool create)
{
    if (create && !d->proxyWidget) {
        return createCanvasItem(document);
    }
    return d->proxyWidget;
}

QGraphicsItem *KoPart::createCanvasItem(KoDocument *document)
{
    KoView *view = createView(document);
    d->proxyWidget = new QGraphicsProxyWidget();
    QWidget *canvasController = view->findChild<KoCanvasControllerWidget *>();
    d->proxyWidget->setWidget(canvasController);
    return d->proxyWidget;
}

void KoPart::addMainWindow(KoMainWindow *mainWindow)
{
    if (d->mainWindows.indexOf(mainWindow) == -1) {
        debugMain << "mainWindow" << (void *)mainWindow << "added to doc" << this;
        d->mainWindows.append(mainWindow);
    }
}

void KoPart::removeMainWindow(KoMainWindow *mainWindow)
{
    debugMain << "mainWindow" << (void *)mainWindow << "removed from doc" << this;
    if (mainWindow) {
        d->mainWindows.removeAll(mainWindow);
    }
}

const QList<KoMainWindow *> &KoPart::mainWindows() const
{
    return d->mainWindows;
}

int KoPart::mainwindowCount() const
{
    return d->mainWindows.count();
}

KoMainWindow *KoPart::currentMainwindow() const
{
    QWidget *widget = qApp->activeWindow();
    KoMainWindow *mainWindow = qobject_cast<KoMainWindow *>(widget);
    while (!mainWindow && widget) {
        widget = widget->parentWidget();
        mainWindow = qobject_cast<KoMainWindow *>(widget);
    }

    if (!mainWindow && mainWindows().size() > 0) {
        mainWindow = mainWindows().first();
    }
    return mainWindow;
}

void KoPart::openExistingFile(const QUrl &url)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    d->document->openUrl(url);
    d->document->setModified(false);
    QApplication::restoreOverrideCursor();
}

void KoPart::openTemplate(const QUrl &url)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    bool ok = d->document->loadNativeFormat(url.toLocalFile());
    d->document->setModified(false);
    d->document->undoStack()->clear();

    if (ok) {
        QString mimeType = QMimeDatabase().mimeTypeForUrl(url).name();
        // in case this is a open document template remove the -template from the end
        mimeType.remove(QRegularExpression("-template$"));
        d->document->setMimeTypeAfterLoading(mimeType);
        deleteOpenPane();
        d->document->resetURL();
        d->document->setEmpty();
    } else {
        d->document->showLoadingErrorDialog();
        d->document->initEmpty();
    }
    QApplication::restoreOverrideCursor();
}

void KoPart::addRecentURLToAllMainWindows(const QUrl &url)
{
    // Add to recent actions list in our mainWindows
    foreach (KoMainWindow *mainWindow, d->mainWindows) {
        mainWindow->addRecentURL(url);
    }
}

void KoPart::showStartUpWidget(KoMainWindow *mainWindow, bool alwaysShow)
{
#ifndef NDEBUG
    if (d->templatesResourcePath.isEmpty())
        debugMain << "showStartUpWidget called, but setTemplatesResourcePath() never called. This will not show a lot";
#endif
    if (!alwaysShow) {
        KConfigGroup cfgGrp(componentData().config(), "TemplateChooserDialog");
        QString fullTemplateName = cfgGrp.readPathEntry("AlwaysUseTemplate", QString());
        if (!fullTemplateName.isEmpty()) {
            QFileInfo fi(fullTemplateName);
            if (!fi.exists()) {
                const QString templatesResourcePath = this->templatesResourcePath();
                QString desktopfile = KoResourcePaths::findResource("data", templatesResourcePath + "*/" + fullTemplateName);
                if (desktopfile.isEmpty()) {
                    desktopfile = KoResourcePaths::findResource("data", templatesResourcePath + fullTemplateName);
                }
                if (desktopfile.isEmpty()) {
                    fullTemplateName.clear();
                } else {
                    QUrl templateURL;
                    KDesktopFile f(desktopfile);
                    templateURL.setPath(QFileInfo(desktopfile).absolutePath() + '/' + f.readUrl());
                    fullTemplateName = templateURL.toLocalFile();
                }
            }
            if (!fullTemplateName.isEmpty()) {
                openTemplate(QUrl::fromUserInput(fullTemplateName));
                mainWindows().first()->setRootDocument(d->document, this);
                return;
            }
        }
    }

    mainWindow->factory()->container("mainToolBar", mainWindow)->hide();

    if (d->startUpWidget) {
        d->startUpWidget->show();
    } else {
        d->startUpWidget = createOpenPane(mainWindow, d->templatesResourcePath);
        mainWindow->setCentralWidget(d->startUpWidget);
    }

    mainWindow->setPartToOpen(this);
}

void KoPart::deleteOpenPane(bool closing)
{
    if (d->startUpWidget) {
        d->startUpWidget->hide();
        d->startUpWidget->deleteLater();

        if (!closing) {
            mainWindows().first()->setRootDocument(d->document, this);
            KoPart::mainWindows().first()->factory()->container("mainToolBar", mainWindows().first())->show();
        }
    }
}

QList<KoPart::CustomDocumentWidgetItem> KoPart::createCustomDocumentWidgets(QWidget * /*parent*/)
{
    return QList<CustomDocumentWidgetItem>();
}

void KoPart::setTemplatesResourcePath(const QString &templatesResourcePath)
{
    Q_ASSERT(!templatesResourcePath.isEmpty());
    Q_ASSERT(templatesResourcePath.endsWith(QLatin1Char('/')));

    d->templatesResourcePath = templatesResourcePath;
}

QString KoPart::templatesResourcePath() const
{
    return d->templatesResourcePath;
}

void KoPart::startCustomDocument()
{
    deleteOpenPane();
}

KoOpenPane *KoPart::createOpenPane(QWidget *parent, const QString &templatesResourcePath)
{
    const QStringList mimeFilter = koApp->mimeFilter(KoFilterManager::Import);

    KoOpenPane *openPane = new KoOpenPane(parent, mimeFilter, templatesResourcePath);
    QList<CustomDocumentWidgetItem> widgetList = createCustomDocumentWidgets(openPane);
    foreach (const CustomDocumentWidgetItem &item, widgetList) {
        openPane->addCustomDocumentWidget(item.widget, item.title, item.icon);
        connect(item.widget, SIGNAL(documentSelected()), this, SLOT(startCustomDocument()));
    }
    openPane->show();

    connect(openPane, &KoOpenPane::openExistingFile, this, &KoPart::openExistingFile);
    connect(openPane, &KoOpenPane::openTemplate, this, &KoPart::openTemplate);

    return openPane;
}
