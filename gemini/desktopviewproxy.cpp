/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "desktopviewproxy.h"

#include <QProcess>
#include <QDir>
#include <QApplication>
#include <QUrl>

#include <klocalizedstring.h>
#include <krecentfilesaction.h>
#include <kactioncollection.h>
#include <KConfigGroup>
#include <KSharedConfig>

#include <KoMainWindow.h>
#include <KoFilterManager.h>
#include <KoFileDialog.h>
#include <KoDocumentEntry.h>
#include <KoConfig.h> // CALLIGRA_OLD_PLUGIN_METADATA

#include "MainWindow.h"
#include <DocumentManager.h>
#include <RecentFileManager.h>
#include <Settings.h>
#include <KoDocument.h>
#include <KWView.h>

class DesktopViewProxy::Private
{
public:
    Private(MainWindow* mainWindow, KoMainWindow* desktopView)
        : mainWindow(mainWindow)
        , desktopView(desktopView)
        , isImporting(false)
    {}
    MainWindow* mainWindow;
    KoMainWindow* desktopView;
    bool isImporting;
};

DesktopViewProxy::DesktopViewProxy(MainWindow* mainWindow, KoMainWindow* parent)
    : QObject(parent)
    , d(new Private(mainWindow, parent))
{
    Q_ASSERT(parent); // "There MUST be a KoMainWindow assigned, otherwise everything will blow up");

    // Hide this one... as it doesn't work at all well and release happens :P
    QAction* closeAction = d->desktopView->actionCollection()->action("file_close");
    closeAction->setVisible(false);

    // Concept is simple - simply steal all the actions we require to work differently, and reconnect them to local functions
    QAction* newAction = d->desktopView->actionCollection()->action("file_new");
    newAction->disconnect(d->desktopView);
    connect(newAction, SIGNAL(triggered(bool)), this, SLOT(fileNew()));
    QAction* openAction = d->desktopView->actionCollection()->action("file_open");
    openAction->disconnect(d->desktopView);
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(fileOpen()));
    QAction* saveAction = d->desktopView->actionCollection()->action("file_save");
    saveAction->disconnect(d->desktopView);
    connect(saveAction, SIGNAL(triggered(bool)), this, SLOT(fileSave()));
    QAction* saveasAction = d->desktopView->actionCollection()->action("file_save_as");
    saveasAction->disconnect(d->desktopView);
    connect(saveasAction, SIGNAL(triggered(bool)), this, SLOT(fileSaveAs()));
    QAction* reloadAction = d->desktopView->actionCollection()->action("file_reload_file");
    reloadAction->disconnect(d->desktopView);
    connect(reloadAction, SIGNAL(triggered(bool)), this, SLOT(reload()));
    QAction* loadExistingAsNewAction = d->desktopView->actionCollection()->action("file_import_file");
    loadExistingAsNewAction->disconnect(d->desktopView);
    connect(loadExistingAsNewAction, SIGNAL(triggered(bool)), this, SLOT(loadExistingAsNew()));

    // Recent files need a touch more work, as they aren't simply an action.
    KRecentFilesAction* recent = qobject_cast<KRecentFilesAction*>(d->desktopView->actionCollection()->action("file_open_recent"));
    recent->disconnect(d->desktopView);
    connect(recent, SIGNAL(urlSelected(QUrl)), this, SLOT(slotFileOpenRecent(QUrl)));
    recent->clear();
    recent->loadEntries(KSharedConfig::openConfig()->group("RecentFiles"));

    connect(d->desktopView, SIGNAL(documentSaved()), this, SIGNAL(documentSaved()));
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
#ifdef CALLIGRA_OLD_PLUGIN_METADATA
        QStringList mimeTypes = json.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
#else
        QStringList mimeTypes = json.value("X-KDE-ExtraNativeMimeTypes").toVariant().toStringList();
#endif

        mimeFilter << KoFilterManager::mimeFilter(DocumentManager::instance()->settingsManager()->currentFileClass().toLatin1(),
                                                               KoFilterManager::Import,
                                                               mimeTypes);
    }

    KoFileDialog dialog(d->desktopView, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setCaption(i18n("Open Document"));
    dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dialog.setMimeTypeFilters(mimeFilter);
    QString filename = dialog.filename();
    if (filename.isEmpty()) return;

    DocumentManager::instance()->recentFileManager()->addRecent(filename);

    QProcess::startDetached(qApp->applicationFilePath(), QStringList() << filename, QDir::currentPath());
}

void DesktopViewProxy::fileSave()
{
    if(DocumentManager::instance()->isTemporaryFile()) {
        if(d->desktopView->saveDocument(true)) {
            if (KoDocument* document = DocumentManager::instance()->document()) {
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
    KoDocument* document = DocumentManager::instance()->document();
    if (!document)
        return false;

    if(d->desktopView->saveDocument(true)) {
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

void DesktopViewProxy::slotFileOpenRecent(const QUrl& url)
{
    QProcess::startDetached(qApp->applicationFilePath(), QStringList() << url.toLocalFile(), QDir::currentPath());
}
