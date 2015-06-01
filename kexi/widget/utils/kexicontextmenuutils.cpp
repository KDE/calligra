/* This file is part of the KDE project
   Copyright (C) 2006-2010 Jarosław Staniek <staniek@kde.org>

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
#include <core/KexiMainWindowIface.h>

#include <KexiIcon.h>

#include <klocale.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kactioncollection.h>
#include <kaction.h>
#include <kurl.h>

#include <QApplication>

//! @internal
class KexiImageContextMenu::Private
{
public:
    explicit Private(QWidget *parent)
            : actionCollection(parent) {
    }

    KActionCollection actionCollection;
    KAction *insertFromFileAction, *saveAsAction, *cutAction, *copyAction, *pasteAction,
            *deleteAction, *propertiesAction;
};

//------------

KexiImageContextMenu::KexiImageContextMenu(QWidget* parent)
        : KMenu(parent)
        , d(new Private(this))
{
    addTitle(QString());

    d->actionCollection.addAction("insert",
                                  d->insertFromFileAction = new KAction(
        koIcon("document-open"), i18n("Insert From &File..."), this));
    connect(d->insertFromFileAction, SIGNAL(triggered()),
            this, SLOT(insertFromFile()));
    addAction(d->insertFromFileAction);
    d->saveAsAction = KStandardAction::saveAs(this, SLOT(saveAs()), &d->actionCollection);
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
        koIcon("edit-clear"), i18n("&Clear"), this));
    connect(d->deleteAction, SIGNAL(triggered()),
            this, SLOT(clear()));
    addAction(d->deleteAction);
#ifndef KEXI_SHOW_UNFINISHED
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
    KUrl url(KFileDialog::getImageOpenUrl(
                 KUrl("kfiledialog:///LastVisitedImagePath"), this, i18n("Insert Image From File")));
    //! @todo download the file if remote, then set fileName properly
    if (!url.isValid()) {
        //focus the app again to avoid annoying the user with unfocused main window
#ifndef KEXI_MOBILE
        if (KexiMainWindowIface::global()->thisWidget()) {
            KexiMainWindowIface::global()->thisWidget()->raise();
        }
#endif
        return;
    }
    //kDebug() << "fname=" << url.prettyUrl();

    emit insertFromFileRequested(url);
#ifndef KEXI_MOBILE
    if (KexiMainWindowIface::global()->thisWidget()) {
        KexiMainWindowIface::global()->thisWidget()->raise();
    }
#endif
}

void KexiImageContextMenu::saveAs()
{
    QString origFilename, fileExtension;
    bool dataIsEmpty = false;
    emit aboutToSaveAsRequested(origFilename, fileExtension, dataIsEmpty);

    if (dataIsEmpty) {
        kWarning() << "no data!";
        return;
    }
    if (!origFilename.isEmpty())
        origFilename = QString("/") + origFilename;

    if (fileExtension.isEmpty()) {
        // PNG data is the default
        fileExtension = "png";
    }

    //! @todo add originalFileName! (requires access to KRecentDirs)
    QString fileName = KFileDialog::getSaveFileName(
                           KUrl("kfiledialog:///LastVisitedImagePath"),
                           KImageIO::pattern(KImageIO::Writing), this, i18n("Save Image to File"));
    if (fileName.isEmpty())
        return;

    if (QFileInfo(fileName).completeSuffix().isEmpty())
        fileName += (QString(".") + fileExtension);
    kDebug() << fileName;
    KUrl url;
    url.setPath(fileName);

    QFile f(fileName);
    if (f.exists() && KMessageBox::Yes != KMessageBox::warningYesNo(this,
            i18n("<para>File <filename>%1</filename> already exists.</para>"
                 "<para>Do you want to replace it with a new one?</para>",
                 QDir::convertSeparators(fileName)), 0,
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
#ifndef KEXI_SHOW_UNFINISHED
    if (d->propertiesAction)
        d->propertiesAction->setEnabled(!valueIsNull);
#endif
}

KActionCollection* KexiImageContextMenu::actionCollection() const
{
    return &d->actionCollection;
}

//static
bool KexiImageContextMenu::updateTitle(KMenu *menu, const QString& title,
                                       const QString& iconName)
{
    return KexiContextMenuUtils::updateTitle(menu, title, i18n("Image"), iconName);
}

// -------------------------------------------

//static
bool KexiContextMenuUtils::updateTitle(KMenu *menu, const QString& objectName,
                                       const QString& objectTypeName, const QString& iconName)
{
    if (!menu || objectName.isEmpty() || objectTypeName.isEmpty())
        return false;
    //try to find title action
    QList<QAction *> actions = menu->actions();
    if (actions.isEmpty())
        return false;
    QAction * action = actions.first();

    /*! @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
     (see doc/dev/settings.txt) */
    QString realTitle(i18nc("Object name : Object type", "%1 : %2",
                            objectName[0].toUpper() + objectName.mid(1),
                            objectTypeName));

    menu->addTitle(QIcon::fromTheme(iconName), realTitle, action /*before old*/);
    if (dynamic_cast<QWidgetAction*>(action)
        && dynamic_cast<QWidgetAction*>(action)->defaultWidget())
    {
        menu->removeAction(action);
    }
    return true;
}

#include "kexicontextmenuutils.moc"
