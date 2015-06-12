/* This file is part of the KDE project
   Copyright (C) 2006-2015 Jarosław Staniek <staniek@kde.org>

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
#include <kexiutils/utils.h>
#include <KexiIcon.h>
#include <config-kexi.h>

#include <KMessageBox>
#include <KActionCollection>
#include <KLocalizedString>

#include <QDir>
#include <QUrl>
#include <QAction>
#include <QFileInfo>

//! @internal
class KexiImageContextMenu::Private
{
public:
    explicit Private(QWidget *parent)
            : actionCollection(parent) {
    }

    KActionCollection actionCollection;
    QAction *insertFromFileAction, *saveAsAction, *cutAction, *copyAction, *pasteAction,
            *deleteAction, *propertiesAction;
};

//------------

KexiImageContextMenu::KexiImageContextMenu(QWidget* parent)
        : QMenu(parent)
        , d(new Private(this))
{
    // default title section
    addSection(QString());
    QString iconName = "pixmaplabel"; //!< @todo pixmaplabel icon is hardcoded...
    updateTitle(this, QString(), iconName);

    d->actionCollection.addAction("insert",
                                  d->insertFromFileAction = new QAction(
        koIcon("document-open"), xi18n("Insert From &File..."), this));
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
                                  d->deleteAction = new QAction(
        koIcon("edit-clear"), xi18n("&Clear"), this));
    connect(d->deleteAction, SIGNAL(triggered()),
            this, SLOT(clear()));
    addAction(d->deleteAction);
#ifndef KEXI_SHOW_UNFINISHED
    d->propertiesAction = 0;
#else
    addSeparator();
    d->actionCollection.addAction("properties",
                                  d->propertiesAction = new QAction(xi18n("Properties"), this));
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
    QUrl dir;
    //QUrl("kfiledialog:///LastVisitedImagePath"), this, ));
    QUrl url = KexiUtils::getOpenImageUrl(this, xi18nc("@title", "Insert Image From File"), dir);

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
    //qDebug() << "fname=" << url.toDisplayString();

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
    emit aboutToSaveAsRequested(&origFilename, &fileExtension, &dataIsEmpty);

    if (dataIsEmpty) {
        qWarning() << "no data!";
        return;
    }
    if (!origFilename.isEmpty())
        origFilename = QString("/") + origFilename;

    if (fileExtension.isEmpty()) {
        // PNG data is the default
        fileExtension = "png";
    }

    //! @todo add originalFileName! (requires access to KRecentDirs)
    //! @todo support remote URLs
    // QUrl("kfiledialog:///LastVisitedImagePath"),
    QUrl dir;
    QUrl url = KexiUtils::getSaveImageUrl(this, xi18nc("@title", "Save Image to File"), dir);
    if (!url.isValid()) {
        return;
    }

    if (QFileInfo(url.toLocalFile()).completeSuffix().isEmpty()) {
        url.setPath(url.toLocalFile() + QLatin1Char('.') + fileExtension);
    }
    qDebug() << url;
    QFile f(url.toLocalFile());
    if (f.exists() && KMessageBox::Yes != KMessageBox::warningYesNo(this,
            xi18n("<para>File <filename>%1</filename> already exists.</para>"
                 "<para>Do you want to replace it with a new one?</para>",
                 QDir::toNativeSeparators(url.toString())), 0,
            KGuiItem(xi18n("&Replace")), KGuiItem(xi18n("&Don't Replace")))) {
        return;
    }
    emit saveAsRequested(url);
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
    emit updateActionsAvailabilityRequested(&valueIsNull, &valueIsReadOnly);

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
bool KexiImageContextMenu::updateTitle(QMenu *menu, const QString& title,
                                       const QString& iconName)
{
    return KexiContextMenuUtils::updateTitle(menu, title, xi18n("Image"), iconName);
}

// -------------------------------------------

//static
bool KexiContextMenuUtils::updateTitle(QMenu *menu, const QString& objectName,
                                       const QString& objectTypeName, const QString& iconName)
{
    if (!menu || objectTypeName.isEmpty())
        return false;
    //try to find title action
    QList<QAction *> actions = menu->actions();
    if (actions.isEmpty())
        return false;
    QAction *titleAction = actions.first();
    //! @todo KEXI3 test this
    if (titleAction && titleAction->isSeparator()) {
        titleAction->setIcon(QIcon::fromTheme(iconName));
        /*! @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
         (see doc/dev/settings.txt) */
        QString realTitle;
        if (objectName.isEmpty()) {
            realTitle = objectTypeName;
        }
        else {
            realTitle = xi18nc("Object name : Object type", "%1 : %2",
                               objectName[0].toUpper() + objectName.mid(1),
                               objectTypeName);
        }
        titleAction->setText(realTitle);
    }
    return true;
}
