/* This file is part of the KDE project
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 * Copyright (C) 2000-2005 David Faure <faure@kde.org>
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010-2012 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Inge Wallin <ingwa@kogmbh.com>
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

#include "KoPart.h"

#include "KoApplication.h"
#include "KoMainWindow.h"
#include "KoDocument.h"
#include "KoView.h"
#include "KoOpenPane.h"
#include "KoProgressProxy.h"
#include "KoFilterManager.h"
#include <KoDocumentInfoDlg.h>

#include <KoCanvasController.h>
#include <KoCanvasControllerWidget.h>

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kxmlguifactory.h>
#include <kdeprintdialog.h>
#include <knotification.h>
#include <kdialog.h>
#include <kdesktopfile.h>
#include <kmessagebox.h>
#include <kmimetype.h>

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

#ifndef QT_NO_DBUS
#include <QDBusConnection>
#include "KoPartAdaptor.h"
#endif


QList<QPointer<KoPart> > KoPart::s_partList;

class KoPart::Private
{
public:
    Private(KoPart *_parent)
        : parent(_parent)
        , canvasItem(0)
        , startupWidget(0)
        , m_componentData(KGlobal::mainComponent())
    {
    }

    ~Private()
    {
        delete canvasItem;
    }

    KoPart *parent;

    QList<QPointer<KoView> > views;
    QList<QPointer<KoMainWindow> > mainWindows;
    QList<QPointer<KoDocument> > documents;
    QGraphicsItem *canvasItem;
    QString templateType;
    KoOpenPane *startupWidget;
    KComponentData m_componentData;

};


KoPart::KoPart(QObject *parent)
        : QObject(parent)
        , d(new Private(this))
{
#ifndef QT_NO_DBUS
    new KoPartAdaptor(this);
    QDBusConnection::sessionBus().registerObject('/' + objectName(), this);
#endif

    s_partList.append(this);
}

KoPart::~KoPart()
{
    //qDebug() << "Deleting KoPart" << this << kBacktrace();

    while (!d->documents.isEmpty()) {
        delete d->documents.takeFirst();
    }

    while (!d->views.isEmpty()) {
        delete d->views.takeFirst();
    }

    while (!d->mainWindows.isEmpty()) {
        delete d->mainWindows.takeFirst();
    }

    s_partList.removeAll(this);

    delete d;
}

QList<QPointer<KoPart> > KoPart::partList()
{
    return s_partList;
}

KComponentData KoPart::componentData() const
{
    return d->m_componentData;
}

void KoPart::addDocument(KoDocument *document)
{
    Q_ASSERT(document);
    if (!d->documents.contains(document)) {
        d->documents.append(document);
    }
}

QList<QPointer<KoDocument> > KoPart::documents() const
{
    return d->documents;
}

int KoPart::documentCount() const
{
    return d->documents.size();
}

void KoPart::removeDocument(KoDocument *document)
{
    d->documents.removeAll(document);
    document->deleteLater();
}

KoView *KoPart::createView(KoDocument *document, QWidget *parent)
{
    KoView *view = createViewInstance(document, parent);
    addView(view, document);
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

    connect(view, SIGNAL(destroyed()), this, SLOT(viewDestroyed()));

    view->updateReadWrite(document->isReadWrite());

    if (d->views.size() == 1) {
        KoApplication *app = qobject_cast<KoApplication*>(KApplication::kApplication());
        if (0 != app) {
            emit app->documentOpened('/'+objectName());
        }
    }
}

void KoPart::removeView(KoView *view)
{
    if (!view) return;
    QPointer<KoDocument> doc = view->document();
    d->views.removeAll(view);

    if (doc) {
        bool found = false;
        foreach(QPointer<KoView> view, d->views) {
            if (view && view->document() == doc) {
                found = true;
                break;
            }
        }
        if (!found) {
            removeDocument(doc);
        }
    }

    if (d->views.isEmpty()) {
        KoApplication *app = qobject_cast<KoApplication*>(KApplication::kApplication());
        if (0 != app) {
            emit app->documentClosed('/'+objectName());
        }
    }
}

QList<QPointer<KoView> > KoPart::views() const
{
    return d->views;
}

int KoPart::viewCount() const
{
    return d->views.count();
}

QGraphicsItem *KoPart::canvasItem(KoDocument *document, bool create)
{
    if (create && !d->canvasItem) {
        d->canvasItem = createCanvasItem(document);
    }
    return d->canvasItem;
}

QGraphicsItem *KoPart::createCanvasItem(KoDocument *document)
{
    if (!document) return 0;

    KoView *view = createView(document, 0);
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
    QWidget *canvasController = view->findChild<KoCanvasControllerWidget*>();
    proxy->setWidget(canvasController);
    return proxy;
}

void KoPart::addMainWindow(KoMainWindow *mainWindow)
{
    if (!mainWindow) return;
    if (d->mainWindows.contains(mainWindow)) return;

    kDebug(30003) <<"mainWindow" << (void*)mainWindow <<"added to doc" << this;
    d->mainWindows.append(mainWindow);

}

void KoPart::removeMainWindow(KoMainWindow *mainWindow)
{
    kDebug(30003) <<"mainWindow" << (void*)mainWindow <<"removed from doc" << this;
    if (mainWindow) {
        d->mainWindows.removeAll(mainWindow);
    }
}

const QList<QPointer<KoMainWindow> > &KoPart::mainWindows() const
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
    KoMainWindow *mainWindow = qobject_cast<KoMainWindow*>(widget);
    while (!mainWindow && widget) {
        widget = widget->parentWidget();
        mainWindow = qobject_cast<KoMainWindow*>(widget);
    }

    if (!mainWindow && mainWindows().size() > 0) {
        mainWindow = mainWindows().first();
    }
    return mainWindow;

}

void KoPart::openExistingFile(const KUrl& url)
{
    qApp->setOverrideCursor(Qt::BusyCursor);
    KoDocument *document = createDocument();
    document->openUrl(url);
    document->setModified(false);
    addDocument(document);

    KoMainWindow *mw;
    if (d->startupWidget) {
        KoMainWindow *mw = qobject_cast<KoMainWindow*>(d->startupWidget->parent());
    }
    if (!mw) mw = currentMainwindow();
    KoView *view = createView(document, mw);
    mw->addView(view);

    if (d->startupWidget) {
        d->startupWidget->setParent(0);
        d->startupWidget->hide();
    }
    qApp->restoreOverrideCursor();
}

void KoPart::openTemplate(const KUrl& url)
{
    qApp->setOverrideCursor(Qt::BusyCursor);
    KoDocument *document = createDocument();

    bool ok = document->loadNativeFormat(url.toLocalFile());
    document->setModified(false);
    document->undoStack()->clear();

    if (ok) {
        QString mimeType = KMimeType::findByUrl( url, 0, true )->name();
        // in case this is a open document template remove the -template from the end
        mimeType.remove( QRegExp( "-template$" ) );
        document->setMimeTypeAfterLoading(mimeType);
        document->resetURL();
        document->setEmpty();
    } else {
        document->showLoadingErrorDialog();
        document->initEmpty();
    }
    addDocument(document);

    KoMainWindow *mw = qobject_cast<KoMainWindow*>(d->startupWidget->parent());
    if (!mw) mw = currentMainwindow();
    KoView *view = createView(document, mw);
    mw->addView(view);

    d->startupWidget->setParent(0);
    d->startupWidget->hide();
    qApp->restoreOverrideCursor();
}

void KoPart::viewDestroyed()
{
    KoView *view = qobject_cast<KoView*>(sender());
    if (view) {
        view->factory()->removeClient(view);
        removeView(view);
    }
}

void KoPart::addRecentURLToAllMainWindows(KUrl url)
{
    // Add to recent actions list in our mainWindows
    foreach(KoMainWindow *mainWindow, d->mainWindows) {
        mainWindow->addRecentURL(url);
    }
}

void KoPart::showStartUpWidget(KoMainWindow *mainWindow, bool alwaysShow)
{
#ifndef NDEBUG
    if (d->templateType.isEmpty())
        kDebug(30003) << "showStartUpWidget called, but setTemplateType() never called. This will not show a lot";
#endif

    if (!alwaysShow) {
        KConfigGroup cfgGrp(componentData().config(), "TemplateChooserDialog");
        QString fullTemplateName = cfgGrp.readPathEntry("AlwaysUseTemplate", QString());
        if (!fullTemplateName.isEmpty()) {
            KUrl url(fullTemplateName);
            QFileInfo fi(url.toLocalFile());
            if (!fi.exists()) {
                QString appName = KGlobal::mainComponent().componentName();
                QString desktopfile = KGlobal::dirs()->findResource("data", appName + "/templates/*/" + fullTemplateName);
                if (desktopfile.isEmpty()) {
                    desktopfile = KGlobal::dirs()->findResource("data", appName + "/templates/" + fullTemplateName);
                }
                if (desktopfile.isEmpty()) {
                    fullTemplateName.clear();
                } else {
                    KUrl templateURL;
                    KDesktopFile f(desktopfile);
                    templateURL.setPath(KUrl(desktopfile).directory() + '/' + f.readUrl());
                    fullTemplateName = templateURL.toLocalFile();
                }
            }
            if (!fullTemplateName.isEmpty()) {
                openTemplate(fullTemplateName);
                return;
            }
        }
    }

    if (!d->startupWidget) {
        const QStringList mimeFilter = koApp->mimeFilter(KoFilterManager::Import);

        d->startupWidget = new KoOpenPane(0, componentData(), mimeFilter, d->templateType);
        d->startupWidget->setWindowModality(Qt::WindowModal);
        QList<CustomDocumentWidgetItem> widgetList = createCustomDocumentWidgets(d->startupWidget);
        foreach(const CustomDocumentWidgetItem & item, widgetList) {
            d->startupWidget->addCustomDocumentWidget(item.widget, item.title, item.icon);
            connect(item.widget, SIGNAL(documentSelected(KoDocument*)), this, SLOT(startCustomDocument(KoDocument*)));
        }

        connect(d->startupWidget, SIGNAL(openExistingFile(const KUrl&)), this, SLOT(openExistingFile(const KUrl&)));
        connect(d->startupWidget, SIGNAL(openTemplate(const KUrl&)), this, SLOT(openTemplate(const KUrl&)));

    }

    d->startupWidget->setParent(mainWindow);
    d->startupWidget->setWindowFlags(Qt::Dialog);
    d->startupWidget->exec();
}

QList<KoPart::CustomDocumentWidgetItem> KoPart::createCustomDocumentWidgets(QWidget * /*parent*/)
{
    return QList<CustomDocumentWidgetItem>();
}

void KoPart::setTemplateType(const QString& _templateType)
{
    d->templateType = _templateType;
}

QString KoPart::templateType() const
{
    return d->templateType;
}

void KoPart::startCustomDocument(KoDocument* doc)
{
    addDocument(doc);
    KoMainWindow *mw = qobject_cast<KoMainWindow*>(d->startupWidget->parent());
    if (!mw) mw = currentMainwindow();
    KoView *view = createView(doc, mw);
    mw->addView(view);

    d->startupWidget->setParent(0);
    d->startupWidget->hide();
}

void KoPart::setComponentData(const KComponentData &componentData)
{
    d->m_componentData = componentData;

    KGlobal::locale()->insertCatalog(componentData.catalogName());
    // install 'instancename'data resource type
    KGlobal::dirs()->addResourceType(QString(componentData.componentName() + "data").toUtf8(),
                                     "data", componentData.componentName());
}




#include <KoPart.moc>
