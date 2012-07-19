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

#ifndef KexiContextMenuUtils_H
#define KexiContextMenuUtils_H

#include <kexi_global.h>
#include <db/queryschema.h>
#include <KMenu>
#include <KUrl>

class KActionCollection;

//! @short A set of helpers for updating popup menu titles
/*! The functions set meaningful titles like "Emploee : Image".
*/
class KEXIGUIUTILS_EXPORT KexiContextMenuUtils
{
public:
    /*! Updates title for context menu.
     \return true if the title has been updated. */
    static bool updateTitle(KMenu *menu, const QString& objectName,
                            const QString& objectTypeName, const QString& iconName);
};

//! @short A context menu used for images within form and table views
/*! Used in KexiDBImageBox and KexiBlobTableEdit.
 Contains actions like insert, save, copy, paste, clear.

 Signals like insertFromFileRequested() are all connected to
 handlers in KexiDBImageBox and KexiBlobTableEdit so these objects can
 respond on requests for data handling.
*/
class KEXIGUIUTILS_EXPORT KexiImageContextMenu : public KMenu
{
    Q_OBJECT

public:
    KexiImageContextMenu(QWidget *parent = 0);
    virtual ~KexiImageContextMenu();

    KActionCollection* actionCollection() const;

    /*! Updates title for context menu.
     Used in KexiDBWidgetContextMenuExtender::createTitle(QMenu *menu) and KexiDBImageBox.
     \return true if the title has been updated. */
    static bool updateTitle(KMenu *menu, const QString& title,
                            const QString& iconName = QString());

public slots:
    void updateActionsAvailability();

    virtual void insertFromFile();
    virtual void saveAs();
    virtual void cut();
    virtual void copy();
    virtual void paste();
    virtual void clear();
    virtual void showProperties();

signals:
    //! Emitted when actions availability should be performed. Just connect this signal
    //! to a slot and set \a valueIsNull and \a valueIsReadOnly.
    void updateActionsAvailabilityRequested(bool& valueIsNull, bool& valueIsReadOnly);

    /*! Emitted before "insertFromFile" action was requested. */
    void insertFromFileRequested(const KUrl &url);

    /*! Emitted before "saveAs" action was requested.
     You should fill \a origFilename, \a fileExtension and \a dataIsEmpty values.
     If \a dataIsEmpty is false, saving will be cancelled. */
    void aboutToSaveAsRequested(QString& origFilename, QString& fileExtension, bool& dataIsEmpty);

    //! Emitted when "saveAs" action was requested
    void saveAsRequested(const QString& fileName);

    //! Emitted when "cut" action was requested
    void cutRequested();

    //! Emitted when "copy" action was requested
    void copyRequested();

    //! Emitted when "paste" action was requested
    void pasteRequested();

    //! Emitted when "clear" action was requested
    void clearRequested();

    //! Emitted when "showProperties" action was requested
    void showPropertiesRequested();

protected:
    class Private;
    Private * const d;
};

#endif
