/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXITABLEDESIGNER_COMMANDS_H
#define KEXITABLEDESIGNER_COMMANDS_H

#include <qmap.h>
#include <q3dict.h>
#include <q3ptrlist.h>
#include <q3ptrdict.h>
#include <qvariant.h>
#include <qpointer.h>
//Added by qt3to4:
#include <Q3CString>

#include <k3command.h>
#include <kexidb/alter.h>
#include <koproperty/set.h>

#include "kexitabledesignerview.h"

class QPoint;
class Q3CString;

namespace KexiTableDesignerCommands
{

//! @short Base class for all Table Designer's commands
class Command : public K3Command
{
public:
    Command(KexiTableDesignerView* view);
    virtual ~Command();

    //! Used to collect actions data for AlterTableHandler
    //! Can return 0 if the action should not be passed to AlterTableHandler
    virtual KexiDB::AlterTableHandler::ActionBase* createAction() {
        return 0;
    }

    virtual QString debugString() {
        return name();
    }

protected:
    QPointer<KexiTableDesignerView> m_view;
};

//! @short Undo/redo command used for when changing a property for a table field
class ChangeFieldPropertyCommand : public Command
{
public:
    /*! Creates the ChangeFieldPropertyCommand object.
     Note: we use internal "uid" property of a field (set["uid"]) to avoid problems with looking
     for field by name when more than one field exists with the same name
     (it's invalid but allowed in design time).
     \a oldlistData and and \a newListData can be specified so Property::setListData() will be called
     on execute() and unexecute().
    */
    ChangeFieldPropertyCommand(KexiTableDesignerView* view,
                               const KoProperty::Set& set, const Q3CString& propertyName,
                               const QVariant& oldValue, const QVariant& newValue,
                               KoProperty::Property::ListData* const oldListData = 0,
                               KoProperty::Property::ListData* const newListData = 0);

    virtual ~ChangeFieldPropertyCommand();

    virtual QString name() const;
    virtual void execute();
    virtual void unexecute();
    virtual KexiDB::AlterTableHandler::ActionBase* createAction();
    virtual QString debugString();

protected:
    KexiDB::AlterTableHandler::ChangeFieldPropertyAction m_alterTableAction;
    QVariant m_oldValue;
//  int m_fieldUID;
    KoProperty::Property::ListData* m_oldListData, *m_listData;
};

//! @short Undo/redo command used when a field is removed from a table
class RemoveFieldCommand : public Command
{
public:
    /*! Constructs RemoveFieldCommand object.
     If \a set is 0, the action only means removing empty row (internal). */
    RemoveFieldCommand(KexiTableDesignerView* view, int fieldIndex,
                       const KoProperty::Set* set);

    virtual ~RemoveFieldCommand();

    virtual QString name() const;
    virtual void execute();
    virtual void unexecute();
    virtual KexiDB::AlterTableHandler::ActionBase* createAction();

    virtual QString debugString();

protected:
    KexiDB::AlterTableHandler::RemoveFieldAction m_alterTableAction;
    KoProperty::Set* m_set;
    int m_fieldIndex;
};

//! @short Undo/redo command used when a new field is inserted into a table
class InsertFieldCommand : public Command
{
public:
    InsertFieldCommand(KexiTableDesignerView* view,
                       int fieldIndex/*, const KexiDB::Field& field*/, const KoProperty::Set& set);
    virtual ~InsertFieldCommand();

    virtual QString name() const;
    virtual void execute();
    virtual void unexecute();
    virtual KexiDB::AlterTableHandler::ActionBase* createAction();

    virtual QString debugString() {
        return name() + "\nAT ROW " + QString::number(m_alterTableAction->index()) //m_alterTableAction.index())
               + ", FIELD: " + m_set["caption"].value().toString(); //m_alterTableAction.field().debugString();
    }

protected:
    KexiDB::AlterTableHandler::InsertFieldAction *m_alterTableAction;
    KoProperty::Set m_set;
};


/* ---- Internal commands follow (not used for building performing ALTER TABLE ---- */

//! @short Undo/redo command used when property visibility is changed
/*! Internal, only used in addition to property change. */
class ChangePropertyVisibilityCommand : public Command
{
public:
    /*! Creates the ChangePropertyVisibilityCommand object.
     Note: we use internal "uid" property of a field (set["uid"]) to avoid problems with looking
     for field by name when more than one field exists with the same name
     (it's invalid but allowed in design time).
    */
    ChangePropertyVisibilityCommand(KexiTableDesignerView* view,
                                    const KoProperty::Set& set, const Q3CString& propertyName,
                                    bool visible);

    virtual ~ChangePropertyVisibilityCommand();

    virtual QString name() const;
    virtual void execute();
    virtual void unexecute();

protected:
    KexiDB::AlterTableHandler::ChangeFieldPropertyAction m_alterTableAction;
//  int m_fieldUID;
    bool m_oldVisibility;
};

//! @short Undo/redo command used when property visibility is changed
/*! Internal, only used in addition to property change. */
class InsertEmptyRowCommand : public Command
{
public:
    /*! Creates the InsertEmptyRowCommand object. */
    InsertEmptyRowCommand(KexiTableDesignerView* view, int row);
    virtual ~InsertEmptyRowCommand();

    virtual QString name() const;
    virtual void execute();
    virtual void unexecute();

protected:
    KexiDB::AlterTableHandler::ChangeFieldPropertyAction m_alterTableAction;
    int m_row;
};

}

#endif
