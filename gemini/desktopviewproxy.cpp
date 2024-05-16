/*
 * <one line to give the library's name and an idea of what it does.>
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "desktopviewproxy.h"

#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QUrl>

#include <KActionCollection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <krecentfilesaction.h>

#include <KoDocumentEntry.h>
#include <KoFileDialog.h>
#include <KoFilterManager.h>
#include <KoMainWindow.h>

#include "MainWindow.h"
#include <DocumentManager.h>
#include <KWView.h>
#include <KoDocument.h>
#include <RecentFileManager.h>
#include <Settings.h>

class DesktopViewProxy::Private
{
public:
    Private(MainWindow *mainWindow, KoMainWindow *desktopView)
        : mainWindow(mainWindow)
        , desktopView(desktopView)
        , isImporting(false)
    {
    }
    MainWindow *mainWindow;
    KoMainWindow *desktopView;
    bool isImporting;
};

DesktopViewProxy::DesktopViewProxy(MainWindow *mainWindow, KoMainWindow *parent)
    : QObject(parent)
    , d(new Private(mainWindow, parent))
{
    Q_ASSERT(parent); // "There MUST be a KoMainWindow assigned, otherwise everything will blow up");

    // Hide this one... as it doesn't work at all well and release happens :P
    QAction *closeAction = d->desktopView->actionCollection()->action("file_close");
    closeAction->setVisible(false);

    // Concept is simple - simply steal all the actions we require to work differently, and reconnect them to local functions
    QAction *newAction = d->desktopView->actionCollection()->action("file_new");
    newAction->disconnect(d->desktopView);
    connect(newAction, &QAction::triggered, this, &DesktopViewProxy::fileNew);
    QAction *openAction = d->desktopView->actionCollection()->action("file_open");
    openAction->disconnect(d->desktopView);
    connect(openAction, &QAction::triggered, this, &DesktopViewProxy::fileOpen);
    QAction *saveAction = d->desktopView->actionCollection()->action("file_save");
    saveAction->disconnect(d->desktopView);
    connect(saveAction, &QAction::triggered, this, &DesktopViewProxy::fileSave);
    QAction *saveasAction = d->desktopView->actionCollection()->action("file_save_as");
    saveasAction->disconnect(d->desktopView);
    connect(saveasAction, &QAction::triggered, this, &DesktopViewProxy::fileSaveAs);
    QAction *reloadAction = d->desktopView->actionCollection()->action("file_reload_file");
    reloadAction->disconnect(d->desktopView);
    connect(reloadAction, &QAction::triggered, this, &DesktopViewProxy::reload);
    QAction *loadExistingAsNewAction = d->desktopView->actionCollection()->action("file_import_file");
    loadExistingAsNewAction->disconnect(d->desktopView);
    connect(loadExistingAsNewAction, &QAction::triggered, this, &DesktopViewProxy::loadExistingAsNew);

    // Recent files need a touch more work, as they aren't simply an action.
    KRecentFilesAction *recent = qobject_cast<KRecentFilesAction *>(d->desktopView->actionCollection()->action("file_open_recent"));
    recent->disconnect(d->desktopView);
    connect(recent, &KRecentFilesAction::urlSelected, this, &DesktopViewProxy::slotFileOpenRecent);
    recent->clear();
    recent->loadEntries(KSharedConfig::openConfig()->group("RecentFiles"));

    connect(d->desktopView, &KoMainWindow::documentSaved, this, &DesktopViewProxy::documentSaved);
}

DesktopViewProxy::~DesktopViewProxy()
{
    delete d;
}

void DesktopViewProxy::fileNew()
{
    QProcess::startDetached(qApp->applicationFilePath(), QStringList(), QDir::currentPath());
}

void DesktopViewProxy::fileOpen()
{
    QStringList mimeFilter;
    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType(DocumentManager::instance()->settingsManager()->currentFileClass().toLatin1());
    if (!entry.isEmpty()) {
        QJsonObject json = entry.metaData();
        QStringList mimeTypes = json.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
        mimeFilter << KoFilterManager::mimeFilter(DocumentManager::instance()->settingsManager()->currentFileClass().toLatin1(),
                                                  KoFilterManager::Import,
                                                  mimeTypes);
    }

    KoFileDialog dialog(d->desktopView, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setCaption(i18n("Open Document"));
    dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dialog.setMimeTypeFilters(mimeFilter);
    QString filename = dialog.filename();
    if (filename.isEmpty())
        return;

    DocumentManager::instance()->recentFileManager()->addRecent(filename);

    QProcess::startDetached(qApp->applicationFilePath(), QStringList() << filename, QDir::currentPath());
}

void DesktopViewProxy::fileSave()
{
    if (DocumentManager::instance()->isTemporaryFile()) {
        if (d->desktopView->saveDocument(true)) {
            if (KoDocument *document = DocumentManager::instance()->document()) {
                DocumentManager::instance()->recentFileManager()->addRecent(document->url().toLocalFile());
                DocumentManager::instance()->settingsManager()->setCurrentFile(document->url().toLocalFile());
                DocumentManager::instance()->setTemporaryFile(false);
                emit documentSaved();
            }
        }
    } else {
        DocumentManager::instance()->save();
        emit documentSaved();
    }
}

bool DesktopViewProxy::fileSaveAs()
{
    KoDocument *document = DocumentManager::instance()->document();
    if (!document)
        return false;

    if (d->desktopView->saveDocument(true)) {
        DocumentManager::instance()->recentFileManager()->addRecent(document->url().toLocalFile());
        DocumentManager::instance()->settingsManager()->setCurrentFile(document->url().toLocalFile());
        DocumentManager::instance()->setTemporaryFile(false);
        emit documentSaved();
        return true;
    }

    DocumentManager::instance()->settingsManager()->setCurrentFile(document->url().toLocalFile());
    return false;
}

void DesktopViewProxy::reload()
{
    DocumentManager::instance()->reload();
}

void DesktopViewProxy::loadExistingAsNew()
{
    d->isImporting = true;
    fileOpen();
    d->isImporting = false;
}

void DesktopViewProxy::slotFileOpenRecent(const QUrl &url)
{
    QProcess::startDetached(qApp->applicationFilePath(), QStringList() << url.toLocalFile(), QDir::currentPath());
}
