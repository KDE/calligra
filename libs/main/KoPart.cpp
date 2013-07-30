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
#include "KoServiceProvider.h"
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
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kfileitem.h>
#include <kio/netaccess.h>
#include <kdirnotify.h>
#include <ktemporaryfile.h>
#include <kprotocolinfo.h>
#include <kfiledialog.h>

#include <unistd.h>

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

#ifndef QT_NO_DBUS
#include <QDBusConnection>
#include "KoPartAdaptor.h"
#endif

namespace {

class DocumentProgressProxy : public KoProgressProxy {
public:
    KoMainWindow *m_shell;
    DocumentProgressProxy(KoMainWindow *shell)
        : m_shell(shell)
    {
    }

    ~DocumentProgressProxy() {
        // signal that the job is done
        setValue(-1);
    }

    int maximum() const {
        return 100;
    }

    void setValue(int value) {
        if (m_shell) {
            m_shell->slotProgress(value);
        }
    }

    void setRange(int /*minimum*/, int /*maximum*/) {

    }

    void setFormat(const QString &/*format*/) {

    }
};
}


class KoPart::Private
{
public:
    Private(KoPart *_parent)
        : parent(_parent)
        , document(0)
        , canvasItem(0)
        , startUpWidget(0)
        , m_manager(0)
    {
        m_job = 0;
        m_statJob = 0;
        m_uploadJob = 0;
        m_saveOk = false;
        m_waitForSave = false;
        m_duringSaveAs = false;
        m_bTemp = false;
        m_bAutoDetectedMime = false;

        m_bModified = false;
        m_bReadWrite = true;
        m_bClosing = false;

    }

    ~Private()
    {
        delete canvasItem;
    }

    KoPart *parent;

    QList<KoView*> views;
    QList<KoMainWindow*> shells;
    KoDocument *document;
    QGraphicsItem *canvasItem;
    QPointer<KoOpenPane> startUpWidget;
    QString templateType;

    KoMainWindow * m_manager;

    KIO::FileCopyJob * m_job;
    KIO::StatJob * m_statJob;
    KIO::FileCopyJob * m_uploadJob;
    KUrl m_originalURL; // for saveAs
    QString m_originalFilePath; // for saveAs
    bool m_saveOk : 1;
    bool m_waitForSave : 1;
    bool m_duringSaveAs : 1;
    bool m_bTemp: 1;      // If @p true, @p m_file is a temporary file that needs to be deleted later.
    bool m_bAutoDetectedMime : 1; // whether the mimetype in the arguments was detected by the part itself
    KUrl m_url; // Remote (or local) url - the one displayed to the user.
    QString m_file; // Local file - the only one the part implementation should deal with.
    QString m_mimeType;

    bool m_bModified;
    bool m_bReadWrite;
    bool m_bClosing;
    QEventLoop m_eventLoop;

    bool openLocalFile()
    {
        m_bTemp = false;
        // set the mimetype only if it was not already set (for example, by the host application)
        if (m_mimeType.isEmpty()) {
            // get the mimetype of the file
            // using findByUrl() to avoid another string -> url conversion
            KMimeType::Ptr mime = KMimeType::findByUrl(m_url, 0, true /* local file*/);
            if (mime) {
                m_mimeType = mime->name();
                m_bAutoDetectedMime = true;
            }
        }
        const bool ret = parent->openFile();
        if (ret) {
            emit parent->completed();
        } else {
            emit parent->canceled(QString());
        }
        return ret;
    }

    void openRemoteFile()
    {
        m_bTemp = true;
        // Use same extension as remote file. This is important for mimetype-determination (e.g. koffice)
        QString fileName = m_url.fileName();
        QFileInfo fileInfo(fileName);
        QString ext = fileInfo.completeSuffix();
        QString extension;
        if (!ext.isEmpty() && m_url.query().isNull()) // not if the URL has a query, e.g. cgi.pl?something
            extension = '.'+ext; // keep the '.'
        KTemporaryFile tempFile;
        tempFile.setSuffix(extension);
        tempFile.setAutoRemove(false);
        tempFile.open();
        m_file = tempFile.fileName();

        KUrl destURL;
        destURL.setPath( m_file );
        KIO::JobFlags flags = KIO::DefaultFlags;
        flags |= KIO::Overwrite;
        m_job = KIO::file_copy(m_url, destURL, 0600, flags);
        m_job->ui()->setWindow(0);
        if (m_job->ui()) {
            m_job->ui()->setWindow(parent->currentShell());
        }
        QObject::connect(m_job, SIGNAL(result(KJob*)), parent, SLOT(_k_slotJobFinished(KJob*)));
        QObject::connect(m_job, SIGNAL(mimetype(KIO::Job*,QString)),
                         parent, SLOT(_k_slotGotMimeType(KIO::Job*,QString)));
    }

    // Set m_file correctly for m_url
    void prepareSaving()
    {
        // Local file
        if ( m_url.isLocalFile() )
        {
            if ( m_bTemp ) // get rid of a possible temp file first
            {              // (happens if previous url was remote)
                QFile::remove( m_file );
                m_bTemp = false;
            }
            m_file = m_url.toLocalFile();
        }
        else
        { // Remote file
            // We haven't saved yet, or we did but locally - provide a temp file
            if ( m_file.isEmpty() || !m_bTemp )
            {
                KTemporaryFile tempFile;
                tempFile.setAutoRemove(false);
                tempFile.open();
                m_file = tempFile.fileName();
                m_bTemp = true;
            }
            // otherwise, we already had a temp file
        }
    }


    void _k_slotJobFinished( KJob * job )
    {
        Q_ASSERT( job == m_job );
        m_job = 0;
        if (job->error())
            emit parent->canceled( job->errorString() );
        else {
            if ( parent->openFile() ) {
                emit parent->completed();
            }
            else {
                emit parent->canceled(QString());
            }
        }
    }

    void _k_slotStatJobFinished(KJob * job)
    {
        Q_ASSERT(job == m_statJob);
        m_statJob = 0;

        // this could maybe confuse some apps? So for now we'll just fallback to KIO::get
        // and error again. Well, maybe this even helps with wrong stat results.
        if (!job->error()) {
            const KUrl localUrl = static_cast<KIO::StatJob*>(job)->mostLocalUrl();
            if (localUrl.isLocalFile()) {
                m_file = localUrl.toLocalFile();
                openLocalFile();
                return;
            }
        }
        openRemoteFile();
    }


    void _k_slotGotMimeType(KIO::Job *job, const QString &mime)
    {
        kDebug(1000) << mime;
        Q_ASSERT(job == m_job); Q_UNUSED(job);
        // set the mimetype only if it was not already set (for example, by the host application)
        if (m_mimeType.isEmpty()) {
            m_mimeType = mime;
            m_bAutoDetectedMime = true;
        }
    }

    void _k_slotUploadFinished( KJob * )
    {
        if (m_uploadJob->error())
        {
            QFile::remove(m_uploadJob->srcUrl().toLocalFile());
            m_uploadJob = 0;
            if (m_duringSaveAs) {
                parent->setUrl(m_originalURL);
                m_file = m_originalFilePath;
            }
        }
        else
        {
            KUrl dirUrl( m_url );
            dirUrl.setPath( dirUrl.directory() );
            ::org::kde::KDirNotify::emitFilesAdded( dirUrl.url() );

            m_uploadJob = 0;
            parent->setModified( false );
            emit parent->completed();
            m_saveOk = true;
        }
        m_duringSaveAs = false;
        m_originalURL = KUrl();
        m_originalFilePath.clear();
        if (m_waitForSave) {
            m_eventLoop.quit();
        }
    }



};


KoPart::KoPart(QObject *parent)
        : QObject(parent)
        , d(new Private(this))
{
#ifndef QT_NO_DBUS
    new KoPartAdaptor(this);
    QDBusConnection::sessionBus().registerObject('/' + objectName(), this);
#endif
}

KoPart::~KoPart()
{
    // Tell our views that the document is already destroyed and
    // that they shouldn't try to access it.
    foreach(KoView *view, views()) {
        view->setDocumentDeleted();
    }

    while (!d->shells.isEmpty()) {
        delete d->shells.takeFirst();
    }


    delete d->startUpWidget;
    d->startUpWidget = 0;


    delete d;
}

void KoPart::setDocument(KoDocument *document)
{
    Q_ASSERT(document);
    d->document = document;
    connect(d->document, SIGNAL(titleModified(QString,bool)), SLOT(setTitleModified(QString,bool)));
}

KoDocument *KoPart::document() const
{
    Q_ASSERT(d->document);
    return d->document;
}

void KoPart::setReadWrite(bool readwrite)
{
    d->m_bReadWrite = readwrite;

    foreach(KoView *view, d->views) {
        view->updateReadWrite(readwrite);
    }

    foreach(KoMainWindow *mainWindow, d->shells) {
        mainWindow->setReadWrite(readwrite);
    }
}

bool KoPart::openFile()
{
    DocumentProgressProxy *progressProxy = 0;
    if (!d->document->progressProxy()) {
        KoMainWindow *shell = 0;
        if (shellCount() > 0) {
            shell = shells()[0];
        }
        progressProxy = new DocumentProgressProxy(shell);
        d->document->setProgressProxy(progressProxy);
    }
    d->document->setUrl(url());

    // THIS IS WRONG! KoDocument::openFile should move here, and whoever subclassed KoDocument to
    // reimplement openFile shold now subclass KoPart.
    bool ok = d->document->openFile();

    if (progressProxy) {
        d->document->setProgressProxy(0);
        delete progressProxy;
    }
    return ok;
}

bool KoPart::saveFile()
{
    DocumentProgressProxy *progressProxy = 0;
    if (!d->document->progressProxy()) {
        KoMainWindow *shell = 0;
        if (shellCount() > 0) {
            shell = shells()[0];
        }
        progressProxy = new DocumentProgressProxy(shell);
        d->document->setProgressProxy(progressProxy);
    }
    d->document->setUrl(url());

    // THIS IS WRONG! KoDocument::saveFile should move here, and whoever subclassed KoDocument to
    // reimplement saveFile shold now subclass KoPart.
    bool ok = d->document->saveFile();

    if (progressProxy) {
        d->document->setProgressProxy(0);
        delete progressProxy;
    }
    return ok;
}

KoView *KoPart::createView(QWidget *parent)
{
    KoView *view = createViewInstance(parent);
    addView(view);
    return view;
}

void KoPart::addView(KoView *view)
{
    if (!view)
        return;

    d->views.append(view);
    view->updateReadWrite(isReadWrite());

    if (d->views.size() == 1) {
        KoApplication *app = qobject_cast<KoApplication*>(KApplication::kApplication());
        if (0 != app) {
            emit app->documentOpened('/'+objectName());
        }
    }
}

void KoPart::removeView(KoView *view)
{
    d->views.removeAll(view);

    if (d->views.isEmpty()) {
        KoApplication *app = qobject_cast<KoApplication*>(KApplication::kApplication());
        if (0 != app) {
            emit app->documentClosed('/'+objectName());
        }
    }
}

QList<KoView*> KoPart::views() const
{
    return d->views;
}

int KoPart::viewCount() const
{
    return d->views.count();
}

QGraphicsItem *KoPart::canvasItem(bool create)
{
    if (create && !d->canvasItem) {
        d->canvasItem = createCanvasItem();
    }
    return d->canvasItem;
}

QGraphicsItem *KoPart::createCanvasItem()
{
    KoView *view = createView();
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
    QWidget *canvasController = view->findChild<KoCanvasControllerWidget*>();
    proxy->setWidget(canvasController);
    return proxy;
}

void KoPart::addShell(KoMainWindow *shell)
{
    if (d->shells.indexOf(shell) == -1) {
        kDebug(30003) <<"shell" << (void*)shell <<"added to doc" << this;
        d->shells.append(shell);
        // XXX!!!
        //connect(shell, SIGNAL(documentSaved()), d->undoStack, SLOT(setClean()));
    }
}

void KoPart::removeShell(KoMainWindow *shell)
{
    kDebug(30003) <<"shell" << (void*)shell <<"removed from doc" << this;
    if (shell) {
        disconnect(shell, SIGNAL(documentSaved()), d->document->undoStack(), SLOT(setClean()));
        d->shells.removeAll(shell);
    }
}

const QList<KoMainWindow*>& KoPart::shells() const
{
    return d->shells;
}

int KoPart::shellCount() const
{
    return d->shells.count();
}


KoMainWindow *KoPart::currentShell() const
{
    QWidget *widget = qApp->activeWindow();
    KoMainWindow *shell = qobject_cast<KoMainWindow*>(widget);
    while (!shell && widget) {
        widget = widget->parentWidget();
        shell = qobject_cast<KoMainWindow*>(widget);
    }

    if (!shell && d->document && shells().size() > 0) {
        shell = shells().first();
    }
    return shell;

}

KoDocumentInfoDlg *KoPart::createDocumentInfoDialog(QWidget *parent, KoDocumentInfo *docInfo) const
{
    return new KoDocumentInfoDlg(parent, docInfo);
}

void KoPart::showSavingErrorDialog()
{
    if (d->document->errorMessage().isEmpty()) {
        KMessageBox::error(0, i18n("Could not save\n%1", localFilePath()));
    } else if (d->document->errorMessage() != "USER_CANCELED") {
        KMessageBox::error(0, i18n("Could not save %1\nReason: %2", localFilePath(), d->document->errorMessage()));
    }
}

void KoPart::showLoadingErrorDialog()
{
    if (d->document->errorMessage().isEmpty()) {
        KMessageBox::error(0, i18n("Could not open\n%1", localFilePath()));
    } else if (d->document->errorMessage() != "USER_CANCELED") {
        KMessageBox::error(0, i18n("Could not open %1\nReason: %2", localFilePath(), d->document->errorMessage()));
    }
}

void KoPart::openExistingFile(const KUrl& url)
{
    qApp->setOverrideCursor(Qt::BusyCursor);
    d->document->openUrl(url);
    setModified(false);
    qApp->restoreOverrideCursor();
}

void KoPart::openTemplate(const KUrl& url)
{
    qApp->setOverrideCursor(Qt::BusyCursor);
    bool ok = d->document->loadNativeFormat(url.toLocalFile());
    d->document->setModified(false);
    d->document->undoStack()->clear();

    if (ok) {
        QString mimeType = KMimeType::findByUrl( url, 0, true )->name();
        // in case this is a open document template remove the -template from the end
        mimeType.remove( QRegExp( "-template$" ) );
        d->document->setMimeTypeAfterLoading(mimeType);
        deleteOpenPane();
        d->document->resetURL();
        d->document->setEmpty();
    } else {
        showLoadingErrorDialog();
        d->document->initEmpty();
    }
    qApp->restoreOverrideCursor();
}

void KoPart::addRecentURLToAllShells(KUrl url)
{
    // Add to recent actions list in our shells
    foreach(KoMainWindow *mainWindow, d->shells) {
        mainWindow->addRecentURL(url);
    }

}

void KoPart::setTitleModified(const QString &caption, bool mod)
{
    // we must be root doc so update caption in all related windows
    foreach(KoMainWindow *mainWindow, d->shells) {
        mainWindow->updateCaption(caption, mod);
        mainWindow->updateReloadFileAction(d->document);
        mainWindow->updateVersionsFileAction(d->document);
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
                shells().first()->setRootDocument(d->document, this);
                return;
            }
        }
    }

    mainWindow->factory()->container("mainToolBar", mainWindow)->hide();

    if (d->startUpWidget) {
        d->startUpWidget->show();
    } else {
        d->startUpWidget = createOpenPane(mainWindow, componentData(), d->templateType);
        mainWindow->setCentralWidget(d->startUpWidget);
    }

    mainWindow->setPartToOpen(this);
}

void KoPart::deleteOpenPane(bool closing)
{
    if (d->startUpWidget) {
        d->startUpWidget->hide();
        d->startUpWidget->deleteLater();

        if(!closing) {
            shells().first()->setRootDocument(d->document, this);
            KoPart::shells().first()->factory()->container("mainToolBar",
                                                                  shells().first())->show();
        }
    } else {
        emit closeEmbedInitDialog();
    }
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

void KoPart::startCustomDocument()
{
    deleteOpenPane();
}

KoOpenPane *KoPart::createOpenPane(QWidget *parent, const KComponentData &componentData,
                                       const QString& templateType)
{
    const QStringList mimeFilter = KoFilterManager::mimeFilter(KoServiceProvider::readNativeFormatMimeType(),
                                                               KoFilterManager::Import, KoServiceProvider::readExtraNativeMimeTypes());

    KoOpenPane *openPane = new KoOpenPane(parent, componentData, mimeFilter, templateType);
    QList<CustomDocumentWidgetItem> widgetList = createCustomDocumentWidgets(openPane);
    foreach(const CustomDocumentWidgetItem & item, widgetList) {
        openPane->addCustomDocumentWidget(item.widget, item.title, item.icon);
        connect(item.widget, SIGNAL(documentSelected()), this, SLOT(startCustomDocument()));
    }
    openPane->show();

    connect(openPane, SIGNAL(openExistingFile(const KUrl&)), this, SLOT(openExistingFile(const KUrl&)));
    connect(openPane, SIGNAL(openTemplate(const KUrl&)), this, SLOT(openTemplate(const KUrl&)));

    return openPane;
}

void KoPart::setManager( KoMainWindow *manager )
{
    d->m_manager = manager;
}

KUrl KoPart::url() const
{
    return d->m_url;
}

bool KoPart::closeUrl()
{
    abortLoad(); //just in case
    if ( isReadWrite() && isModified() ) {
        if (!queryClose())
            return false;
    }
    // Not modified => ok and delete temp file.
    d->m_mimeType = QString();

    if ( d->m_bTemp )
    {
        QFile::remove( d->m_file );
        d->m_bTemp = false;
    }
    // It always succeeds for a read-only part,
    // but the return value exists for reimplementations
    // (e.g. pressing cancel for a modified read-write part)
    return true;
}


QString KoPart::mimeType() const
{
    return d->m_mimeType;
}

bool KoPart::isReadWrite() const
{
    return d->m_bReadWrite;
}

bool KoPart::isModified() const
{
    return d->m_bModified;
}

void KoPart::setModified( bool modified )
{
    kDebug(1000) << "setModified(" << (modified ? "true" : "false") << ")";
    if ( !d->m_bReadWrite && modified ) {
        kError(1000) << "Can't set a read-only document to 'modified' !" << endl;
        return;
    }
    d->m_bModified = modified;
}

bool KoPart::saveAs( const KUrl & kurl )
{
    if (!kurl.isValid())
    {
        kError(1000) << "saveAs: Malformed URL " << kurl.url() << endl;
        return false;
    }
    d->m_duringSaveAs = true;
    d->m_originalURL = d->m_url;
    d->m_originalFilePath = d->m_file;
    d->m_url = kurl; // Store where to upload in saveToURL
    d->prepareSaving();
    bool result = save(); // Save local file and upload local file
    if (!result) {
        d->m_url = d->m_originalURL;
        d->m_file = d->m_originalFilePath;
        d->m_duringSaveAs = false;
        d->m_originalURL = KUrl();
        d->m_originalFilePath.clear();
    }

    return result;
}


bool KoPart::openUrl( const KUrl &url )
{
    if ( !url.isValid() )
        return false;
    if (d->m_bAutoDetectedMime) {
        d->m_mimeType = QString();
        d->m_bAutoDetectedMime = false;
    }
    QString mimetype = d->m_mimeType;
    if ( !closeUrl() )
        return false;
    d->m_mimeType = mimetype;
    setUrl(url);

    d->m_file.clear();

    if (d->m_url.isLocalFile()) {
        d->m_file = d->m_url.toLocalFile();
        return d->openLocalFile();
    } else if (KProtocolInfo::protocolClass(url.protocol()) == ":local") {
        // Maybe we can use a "local path", to avoid a temp copy?
        KIO::JobFlags flags = KIO::DefaultFlags;
        d->m_statJob = KIO::mostLocalUrl(d->m_url, flags);
        d->m_statJob->ui()->setWindow( 0 );
        connect(d->m_statJob, SIGNAL(result(KJob*)), this, SLOT(_k_slotStatJobFinished(KJob*)));
        return true;
    } else {
        d->openRemoteFile();
        return true;
    }
}


bool KoPart::save()
{
    d->m_saveOk = false;
    if ( d->m_file.isEmpty() ) // document was created empty
        d->prepareSaving();
    if (saveFile()) {
        return saveToUrl();
    }
    else {
        emit canceled(QString());
    }
    return false;
}


bool KoPart::waitSaveComplete()
{
    if (!d->m_uploadJob)
        return d->m_saveOk;

    d->m_waitForSave = true;

    d->m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    d->m_waitForSave = false;

    return d->m_saveOk;
}


void KoPart::setComponentData(const KComponentData &componentData)
{
    KXMLGUIClient::setComponentData(componentData);
    KGlobal::locale()->insertCatalog(componentData.catalogName());
    // install 'instancename'data resource type
    KGlobal::dirs()->addResourceType(QString(componentData.componentName() + "data").toUtf8(),
                                     "data", componentData.componentName());
}


void KoPart::abortLoad()
{
    if ( d->m_statJob ) {
        //kDebug(1000) << "Aborting job" << d->m_statJob;
        d->m_statJob->kill();
        d->m_statJob = 0;
    }
    if ( d->m_job ) {
        //kDebug(1000) << "Aborting job" << d->m_job;
        d->m_job->kill();
        d->m_job = 0;
    }
}


void KoPart::setUrl(const KUrl &url)
{
    d->m_url = url;
}

QString KoPart::localFilePath() const
{
    return d->m_file;
}


void KoPart::setLocalFilePath( const QString &localFilePath )
{
    d->m_file = localFilePath;
}

bool KoPart::queryClose()
{
    if ( !isReadWrite() || !isModified() )
        return true;

    QString docName = url().fileName();
    if (docName.isEmpty()) docName = i18n( "Untitled" );


    int res = KMessageBox::warningYesNoCancel( 0,
                                               i18n( "The document \"%1\" has been modified.\n"
                                                     "Do you want to save your changes or discard them?" ,  docName ),
                                               i18n( "Close Document" ), KStandardGuiItem::save(), KStandardGuiItem::discard() );

    bool abortClose=false;
    bool handled=false;

    switch(res) {
    case KMessageBox::Yes :
        if (!handled)
        {
            if (d->m_url.isEmpty())
            {
                KUrl url = KFileDialog::getSaveUrl(KUrl(), QString(), 0);
                if (url.isEmpty())
                    return false;

                saveAs( url );
            }
            else
            {
                save();
            }
        } else if (abortClose) return false;
        return waitSaveComplete();
    case KMessageBox::No :
        return true;
    default : // case KMessageBox::Cancel :
        return false;
    }
}


bool KoPart::saveToUrl()
{
    if ( d->m_url.isLocalFile() ) {
        setModified( false );
        emit completed();
        // if m_url is a local file there won't be a temp file -> nothing to remove
        Q_ASSERT( !d->m_bTemp );
        d->m_saveOk = true;
        d->m_duringSaveAs = false;
        d->m_originalURL = KUrl();
        d->m_originalFilePath.clear();
        return true; // Nothing to do
    }
    else {
        if (d->m_uploadJob) {
            QFile::remove(d->m_uploadJob->srcUrl().toLocalFile());
            d->m_uploadJob->kill();
            d->m_uploadJob = 0;
        }
        KTemporaryFile *tempFile = new KTemporaryFile();
        tempFile->open();
        QString uploadFile = tempFile->fileName();
        delete tempFile;
        KUrl uploadUrl;
        uploadUrl.setPath( uploadFile );
        // Create hardlink
        if (::link(QFile::encodeName(d->m_file), QFile::encodeName(uploadFile)) != 0) {
            // Uh oh, some error happened.
            return false;
        }
        d->m_uploadJob = KIO::file_move( uploadUrl, d->m_url, -1, KIO::Overwrite );
        d->m_uploadJob->ui()->setWindow( 0 );
        connect( d->m_uploadJob, SIGNAL(result(KJob*)), this, SLOT(_k_slotUploadFinished(KJob*)) );
        return true;
    }
}


#include <KoPart.moc>
