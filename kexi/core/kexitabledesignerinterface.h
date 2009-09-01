/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXITABLEDESIGNERINTERFACE_H
#define KEXITABLEDESIGNERINTERFACE_H

#include <kexi_export.h>
#include <koproperty/Property.h>
#include <kexiutils/tristate.h>
#include <QVariant>

class QByteArray;

namespace KoProperty
{
class Set;
}

//! Interface for main Table Designer's commands
/*! This interface has been specified to enable invoking Table Designer's commands
 at application's level. This is used in the "altertable" test suite, available in
 kexi/tests/altertable Kexi source code directory.
 KexiTableDesignerInterface is implemented by KexiTableDesignerView, so it's enough
 to use dynamic_cast:
 \code
 KexiWindow *window = KexiMainWindow::self()->currentWindow();
 if (window) {
   KexiTableDesignerInterface* designerIface
     = dynamic_cast<KexiTableDesignerInterface*>( window->selectedView() );
   if (designerIface) {
     //for example, delete row #3
     designerIface->deleteRow( 3, true );
   }
 }
 \endcode
 Methods of KexiTableDesignerInterface are also used by classes of KexiTableDesignerCommands
 namespace (KCommand derivatives) for implementing commands execution and unexecution.

 All the methods contain addCommand argument. Set if to true to get the command added
 to the undo/redo buffer, what will look like real user's action. This is also needed
 to poperly generate arguments for committing the "alter table" operation.
*/
class KEXICORE_EXPORT KexiTableDesignerInterface
{
public:
    KexiTableDesignerInterface();

    virtual ~KexiTableDesignerInterface();

    /*! Clears field information entered for row.
     This is performed by removing values from caption and data type columns. */
    virtual void clearRow(int row, bool addCommand = false) = 0;

    /*! Inserts a new field with \a caption for \a row.
     Property set is also created.
     Existing field will be overwritten, so use insertEmptyRow()
     is you want to move subsequent fields down. */
    virtual void insertField(int row, const QString& caption, bool addCommand = false) = 0;

    /*! Inserts a new \a field for \a row.
     Property set is also created. \a set will be deeply-copied into the new set.
     Existing field will be overwritten, so use insertEmptyRow()
     is you want to move subsequent fields down. */
    virtual void insertField(int row, KoProperty::Set& set, bool addCommand = false) = 0;

    /*! Inserts a new empty row at position \a row. */
    virtual void insertEmptyRow(int row, bool addCommand = false) = 0;

    /*! Deletes \a row from the table view. Property set is also deleted.
     All the subsequent fields are moved up. */
    virtual void deleteRow(int row, bool addCommand = false) = 0;

    /*! Changes property \a propertyName to \a newValue for a field pointed by \a fieldUID.
     If \a listData is not NULL and not empty, a deep copy of it is passed to Property::setListData().
     If \a listData \a nlist if not NULL but empty, Property::setListData(0) is called. */
    virtual void changeFieldPropertyForRow(int fieldUID, const QByteArray& propertyName,
                                           const QVariant& newValue, KoProperty::Property::ListData* const listData = 0,
                                           bool addCommand = false) = 0;

    /*! Creates temporary table for the current design and returns debug string for it. */
    virtual QString debugStringForCurrentTableSchema(tristate& result) = 0;

    /*! Simulates execution of alter table, and puts debug into \a debugTarget.
     A case when debugTarget is not 0 is true for the alter table test suite. */
    virtual tristate simulateAlterTableExecution(QString *debugTarget) = 0;

    /*! Real execution of the Alter Table. For debugging of the real alter table.
     \return true on success, false on failure and cancelled if user has cancelled
     execution. */
    virtual tristate executeRealAlterTable() = 0;
};

#endif
