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

#include "MainWindow.h"
#include <sketch/DocumentManager.h>

#include <KoMainWindow.h>
#include <KoFilterManager.h>
#include <KoServiceProvider.h>

#include <kactioncollection.h>

#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QApplication>
#include <KFileDialog>
#include <KLocalizedString>

class DesktopViewProxy::Private
{
public:
    Private(MainWindow* mainWindow, KoMainWindow* desktopView)
        : mainWindow(mainWindow)
        , desktopView(desktopView)
    {}
    MainWindow* mainWindow;
    KoMainWindow* desktopView;
};

DesktopViewProxy::DesktopViewProxy(MainWindow* mainWindow, KoMainWindow* parent)
    : QObject(parent)
    , d(new Private(mainWindow, parent))
{
    Q_ASSERT(parent); // "There MUST be a KoMainWindow assigned, otherwise everything will blow up");
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
}

DesktopViewProxy::~DesktopViewProxy()
{
    delete d;
}

void DesktopViewProxy::fileNew()
{
    qDebug() << Q_FUNC_INFO;
    QProcess::startDetached(qApp->applicationFilePath(), QStringList(), QDir::currentPath());
}

void DesktopViewProxy::fileOpen()
{
    qDebug() << Q_FUNC_INFO;
#ifdef Q_WS_WIN
    // "kfiledialog:///OpenDialog" forces KDE style open dialog in Windows
    // TODO provide support for "last visited" directory
    KFileDialog *dialog = new KFileDialog(KUrl(""), QString(), d->desktopView);
#else
    KFileDialog *dialog = new KFileDialog(KUrl("kfiledialog:///OpenDialog"), QString(), d->desktopView);
#endif
    dialog->setObjectName("file dialog");
    dialog->setMode(KFile::File);
    dialog->setCaption(i18n("Open Document"));

    const QStringList mimeFilter = KoFilterManager::mimeFilter(KoServiceProvider::readNativeFormatMimeType(),
                                   KoFilterManager::Import,
                                   KoServiceProvider::readExtraNativeMimeTypes());
    dialog->setMimeFilter(mimeFilter);
    if (dialog->exec() != QDialog::Accepted) {
        delete dialog;
        return;
    }
    KUrl url(dialog->selectedUrl());
    delete dialog;

    if (url.isEmpty())
        return;

    DocumentManager::instance()->openDocument(url.toLocalFile());
}

void DesktopViewProxy::fileSave()
{
    qDebug() << Q_FUNC_INFO;

}

void DesktopViewProxy::fileSaveAs()
{
    qDebug() << Q_FUNC_INFO;

}

#include "desktopviewproxy.moc"
