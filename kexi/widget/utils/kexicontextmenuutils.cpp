/* This file is part of the KDE project
   Copyright (C) 2006-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexicontextmenuutils.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <KActionCollection>
#include <KAction>

#include <qfiledialog.h>
#include <qapplication.h>

/*#ifdef Q_WS_WIN
#include <win32_utils.h>
#include <krecentdirs.h>
#endif*/

#ifdef __GNUC__
#warning KexiImageContextMenu ported to Qt4 but not tested
#else
#pragma WARNING( KexiImageContextMenu ported to Qt4 but not tested )
#endif

//! @internal
class KexiImageContextMenu::Private
{
public:
    Private(QWidget *parent)
            : actionCollection(parent) {
    }

    KActionCollection actionCollection;
    KAction *insertFromFileAction, *saveAsAction, *cutAction, *copyAction, *pasteAction,
    *deleteAction
#ifdef KEXI_NO_UNFINISHED
    , *propertiesAction
#endif
    ;
    QAction *titleAction;
};

//------------

KexiImageContextMenu::KexiImageContextMenu(QWidget* parent)
        : KMenu(parent)
        , d(new Private(this))
{
    d->titleAction = addTitle(QString());

    d->actionCollection.addAction("insert",
                                  d->insertFromFileAction = new KAction(
        KIcon("document-open"), i18n("Insert From &File..."), this));
    connect(d->insertFromFileAction, SIGNAL(triggered()),
            this, SLOT(insertFromFile()));
    addAction(d->insertFromFileAction);
    d->saveAsAction = KStandardAction::saveAs(this, SLOT(saveAs()), &d->actionCollection);
// d->saveAsAction->setText(i18n("&Save &As..."));
    addAction(d->saveAsAction);
    addSeparator();
    d->cutAction = KStandardAction::cut(this, SLOT(cut()), &d->actionCollection);
    addAction(d->cutAction);
    d->copyAction = KStandardAction::copy(this, SLOT(copy()), &d->actionCollection);
    addAction(d->copyAction);
    d->pasteAction = KStandardAction::paste(this, SLOT(paste()), &d->actionCollection);
    addAction(d->pasteAction);
    d->actionCollection.addAction("delete",
                                  d->deleteAction = new KAction(
        KIcon("edit-clear"), i18n("&Clear"), this));
    connect(d->deleteAction, SIGNAL(triggered()),
            this, SLOT(clear()));
    addAction(d->deleteAction);
#ifdef KEXI_NO_UNFINISHED
    d->propertiesAction = 0;
#else
    addSeparator();
    d->actionCollection.addAction("properties",
                                  d->propertiesAction = new KAction(i18n("Properties"), this));
    connect(d->propertiesAction, SIGNAL(triggered()),
            this, SLOT(showProperties()));
    addAction(d->propertiesAction);
#endif
    connect(this, SIGNAL(aboutToShow()), this, SLOT(updateActionsAvailability()));
}

KexiImageContextMenu::~KexiImageContextMenu()
{
    delete d;
}

void KexiImageContextMenu::insertFromFile()
{
// QWidget *focusWidget = qApp->focusWidget();
#ifdef __GNUC__
#warning TODO Q3FileDialog::getOpenFileName for win32
#else
#pragma WARNING( Q3FileDialog::getOpenFileName for win32 )
#endif
    /* TODO
    #ifdef Q_WS_WIN
      QString recentDir;
      QString fileName = Q3FileDialog::getOpenFileName(
        KFileDialog::getStartURL(
          KUrl("kfiledialog:///LastVisitedImagePath"), recentDir).path(),
        convertKFileDialogFilterToQ3FileDialogFilter(KImageIO::pattern(KImageIO::Reading)),
        this, 0, i18n("Insert Image From File"));
      KUurl url;
      if (!fileName.isEmpty())
        url.setPath( fileName );
    #else*/
    KUrl url(KFileDialog::getImageOpenUrl(
                 KUrl("kfiledialog:///LastVisitedImagePath"), this, i18n("Insert Image From File")));
// QString fileName = url.isLocalFile() ? url.toLocalFile() : url.prettyURL();

    //! @todo download the file if remote, then set fileName properly
//#endif
    if (!url.isValid()) {
        //focus the app again to avoid annoying the user with unfocused main window
        if (qApp->mainWidget()) {
            //focusWidget->raise();
            //focusWidget->setFocus();
            qApp->mainWidget()->raise();
        }
        return;
    }
    kDebug() << "fname=" << url.prettyUrl();

#ifdef __GNUC__
#warning TODO Q3FileDialog::getOpenFileName for win32
#else
#pragma WARNING( Q3FileDialog::getOpenFileName for win32 )
#endif
    /*#ifdef Q_WS_WIN
      //save last visited path
    // KUrl url(fileName);
      if (url.isLocalFile())
        KRecentDirs::add("kfiledialog:///LastVisitedImagePath", url.directory());
    #endif*/

    emit insertFromFileRequested(url);
    if (qApp->mainWidget()) {
//  focusWidget->raise();
//  focusWidget->setFocus();
        qApp->mainWidget()->raise();
    }
}

void KexiImageContextMenu::saveAs()
{
    QString origFilename, fileExtension;
    bool dataIsEmpty = false;
    emit aboutToSaveAsRequested(origFilename, fileExtension, dataIsEmpty);

    if (dataIsEmpty) {
        kWarning() << "KexiImageContextMenu::saveAs(): no data!";
        return;
    }
    if (!origFilename.isEmpty())
        origFilename = QString("/") + origFilename;

    if (fileExtension.isEmpty()) {
        // PNG data is the default
        fileExtension = "png";
    }

#ifdef __GNUC__
#warning TODO Q3FileDialog::getOpenFileName for win32
#else
#pragma WARNING( Q3FileDialog::getOpenFileName for win32 )
#endif
    /*TODO
    #ifdef Q_WS_WIN
      QString recentDir;
      QString fileName = Q3FileDialog::getSaveFileName(
        KFileDialog::getStartURL(KUrl("kfiledialog:///LastVisitedImagePath"), recentDir).path()
         + origFilename,
        convertKFileDialogFilterToQ3FileDialogFilter(KImageIO::pattern(KImageIO::Writing)),
        this, 0, i18n("Save Image to File"));
    #else*/
    //! @todo add originalFileName! (requires access to KRecentDirs)
    QString fileName = KFileDialog::getSaveFileName(
                           KUrl("kfiledialog:///LastVisitedImagePath"),
                           KImageIO::pattern(KImageIO::Writing), this, i18n("Save Image to File"));
//#endif
    if (fileName.isEmpty())
        return;

    if (QFileInfo(fileName).completeSuffix().isEmpty())
        fileName += (QString(".") + fileExtension);
    kDebug() << fileName;
    KUrl url;
    url.setPath(fileName);

#ifdef __GNUC__
#warning TODO Q3FileDialog::getOpenFileName for win32
#else
#pragma WARNING( Q3FileDialog::getOpenFileName for win32 )
#endif
    /*#ifdef Q_WS_WIN
      //save last visited path
      if (url.isLocalFile())
        KRecentDirs::add("kfiledialog:///LastVisitedImagePath", url.directory());
    #endif*/

    QFile f(fileName);
    if (f.exists() && KMessageBox::Yes != KMessageBox::warningYesNo(this,
            "<qt>" +
            i18n("File \"%1\" already exists.<p>Do you want to replace it with a new one?",
                 QDir::convertSeparators(fileName)) + "</qt>", 0,
            KGuiItem(i18n("&Replace")), KGuiItem(i18n("&Don't Replace")))) {
        return;
    }

//! @todo use KUrl?
    emit saveAsRequested(fileName);
}

void KexiImageContextMenu::cut()
{
    emit cutRequested();
}

void KexiImageContextMenu::copy()
{
    emit copyRequested();
}

void KexiImageContextMenu::paste()
{
    emit pasteRequested();
}

void KexiImageContextMenu::clear()
{
    emit clearRequested();
}

void KexiImageContextMenu::showProperties()
{
    emit showPropertiesRequested();
}

void KexiImageContextMenu::updateActionsAvailability()
{
    bool valueIsNull = true;
    bool valueIsReadOnly = true;
    emit updateActionsAvailabilityRequested(valueIsNull, valueIsReadOnly);

    d->insertFromFileAction->setEnabled(!valueIsReadOnly);
    d->saveAsAction->setEnabled(!valueIsNull);
    d->cutAction->setEnabled(!valueIsNull && !valueIsReadOnly);
    d->copyAction->setEnabled(!valueIsNull);
    d->pasteAction->setEnabled(!valueIsReadOnly);
    d->deleteAction->setEnabled(!valueIsNull && !valueIsReadOnly);
#ifdef KEXI_NO_UNFINISHED
    if (d->propertiesAction)
        d->propertiesAction->setEnabled(!valueIsNull);
#endif
}

KActionCollection* KexiImageContextMenu::actionCollection() const
{
    return &d->actionCollection;
}

//static
bool KexiImageContextMenu::updateTitle(QMenu *menu, const QString& title,
                                       const QString& iconName)
{
    return KexiContextMenuUtils::updateTitle(menu, title, i18n("Image"), iconName);
}

// -------------------------------------------

//static
bool KexiContextMenuUtils::updateTitle(QMenu *menu, const QString& objectName,
                                       const QString& objectTypeName, const QString& iconName)
{
    if (!menu || objectName.isEmpty() || objectTypeName.isEmpty())
        return false;
    //try to find title action
    QList<QAction *> actions = menu->actions();
    if (actions.isEmpty())
        return false;
    QWidgetAction * action = dynamic_cast<QWidgetAction*>(actions.first());
    if (!action || !action->defaultWidget())
        return false;

// const int id = menu->idAt(0);
// QMenuItem *item = menu->findItem(id);
// if (!item)
//  return false;
// KPopupTitle *title = dynamic_cast<KPopupTitle *>(item->widget());
// if (!title)
//  return false;

    /*! @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
     (see doc/dev/settings.txt) */
    QString realTitle(i18nc("Object name : Object type", "%1 : %2",
                            objectName[0].toUpper() + objectName.mid(1),
                            objectTypeName));

    action->setIcon(KIcon(iconName));
    action->setText(realTitle);
    /*if (iconName.isEmpty())
      title->setTitle(realTitle);
    else {
      QPixmap pixmap(SmallIcon( iconName ));
      title->setTitle(realTitle, &pixmap);
    }*/
    return true;
}

#include "kexicontextmenuutils.moc"
