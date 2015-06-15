/* This file is part of the KDE project
   Copyright (C) 2006-2012 Jarosław Staniek <staniek@kde.org>

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

#include <QPointer>

#include <kundo2command.h>

#include <KDbAlter>

#include <KPropertySet>

#include "kexitabledesignerview.h"

class QPoint;
class QByteArray;

namespace KexiTableDesignerCommands
{

//! @short Base class for all Table Designer's commands
class Command : public KUndo2Command
{
public:
    Command(const KUndo2MagicString &text, Command *parent, KexiTableDesignerView* view);
    Command(Command* parent, KexiTableDesignerView* view);
    virtual ~Command();

    //! Used to collect actions data for AlterTableHandler
    //! Can return 0 if the action should not be passed to AlterTableHandler
    virtual KDbAlterTableHandler::ActionBase* createAction() const {
        return 0;
    }

    virtual QString debugString() const {
        return text().toString();
    }

    virtual void redo();
    virtual void undo();

    //! Enables or disabled redo(). Needed for pushing action on stack without executing it.
    //! True by default.
    void setRedoEnabled(bool enabled);
protected:
    virtual void redoInternal();
    virtual void undoInternal();
    QPointer<KexiTableDesignerView> m_view;
    bool m_redoEnabled;
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
    ChangeFieldPropertyCommand(Command* parent, KexiTableDesignerView* view,
                               const KPropertySet& set, const QByteArray& propertyName,
                               const QVariant& oldValue, const QVariant& newValue,
                               KPropertyListData* const oldListData = 0,
                               KPropertyListData* const newListData = 0);

    virtual ~ChangeFieldPropertyCommand();

    virtual void redoInternal();
    virtual void undoInternal();
    virtual KDbAlterTableHandler::ActionBase* createAction() const;
    virtual QString debugString() const;

protected:
    KDbAlterTableHandler::ChangeFieldPropertyAction m_alterTableAction;
    QVariant m_oldValue;
    KPropertyListData* m_oldListData, *m_listData;
};

//! @short Undo/redo command used when a field is removed from a table
class RemoveFieldCommand : public Command
{
public:
    /*! Constructs RemoveFieldCommand object.
     If \a set is 0, the action only means removing empty row (internal). */
    RemoveFieldCommand(Command* parent, KexiTableDesignerView* view, int fieldIndex,
                       const KPropertySet* set);

    virtual ~RemoveFieldCommand();

    virtual void redoInternal();
    virtual void undoInternal();
    virtual KDbAlterTableHandler::ActionBase* createAction() const;

    virtual QString debugString() const;

protected:
    KDbAlterTableHandler::RemoveFieldAction m_alterTableAction;
    KPropertySet* m_set;
    int m_fieldIndex;
};

//! @short Undo/redo command used when a new field is inserted into a table
class InsertFieldCommand : public Command
{
public:
    InsertFieldCommand(Command* parent, KexiTableDesignerView* view,
                       int fieldIndex/*, const KDbField& field*/, const KPropertySet& set);
    virtual ~InsertFieldCommand();

    virtual void redoInternal();
    virtual void undoInternal();
    virtual KDbAlterTableHandler::ActionBase* createAction() const;

    virtual QString debugString() const;

protected:
    KDbAlterTableHandler::InsertFieldAction *m_alterTableAction;
    KPropertySet m_set;
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
    ChangePropertyVisibilityCommand(Command* parent, KexiTableDesignerView* view,
                                    const KPropertySet& set, const QByteArray& propertyName,
                                    bool visible);

    virtual ~ChangePropertyVisibilityCommand();

    virtual void redoInternal();
    virtual void undoInternal();

protected:
    KDbAlterTableHandler::ChangeFieldPropertyAction m_alterTableAction;
    bool m_oldVisibility;
};

//! @short Undo/redo command used when property visibility is changed
/*! Internal, only used in addition to property change. */
class InsertEmptyRowCommand : public Command
{
public:
    /*! Creates the InsertEmptyRowCommand object. */
    InsertEmptyRowCommand(Command* parent, KexiTableDesignerView* view, int row);
    virtual ~InsertEmptyRowCommand();

    virtual void redoInternal();
    virtual void undoInternal();

protected:
    KDbAlterTableHandler::ChangeFieldPropertyAction m_alterTableAction;
    int m_row;
};

}

#endif
