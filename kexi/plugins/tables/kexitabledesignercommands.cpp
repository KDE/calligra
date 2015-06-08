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

#include <QDebug>

#include <KLocalizedString>

#include <KProperty>
#include <kexi_global.h>

#include "kexitabledesignercommands.h"

using namespace KexiTableDesignerCommands;


Command::Command(const KUndo2MagicString &text, Command *parent, KexiTableDesignerView* view)
        : KUndo2Command(text, parent)
        , m_view(view)
        , m_redoEnabled(true)
{
}

Command::Command(Command* parent, KexiTableDesignerView* view)
        : KUndo2Command(KUndo2MagicString(), parent)
        , m_view(view)
        , m_redoEnabled(true)
{
}

Command::~Command()
{
}

void Command::setRedoEnabled(bool enabled)
{
    m_redoEnabled = enabled;
}

void Command::redo()
{
    if (m_redoEnabled) {
        redoInternal();
    }
}

void Command::undo()
{
    undoInternal();
}

void Command::redoInternal()
{
}

void Command::undoInternal()
{
}

//--------------------------------------------------------

ChangeFieldPropertyCommand::ChangeFieldPropertyCommand(
    Command* parent, KexiTableDesignerView* view,
    const KPropertySet& set, const QByteArray& propertyName,
    const QVariant& oldValue, const QVariant& newValue,
    KProperty::ListData* const oldListData,
    KProperty::ListData* const newListData)
        : Command(parent, view)
        , m_alterTableAction(
            propertyName == "name" ? oldValue.toString() : set.property("name").value().toString(),
            propertyName, newValue, set["uid"].value().toInt())
        , m_oldValue(oldValue)
        , m_oldListData(oldListData ? new KProperty::ListData(*oldListData) : 0)
        , m_listData(newListData ? new KProperty::ListData(*newListData) : 0)
{
    setText(kundo2_i18n("Change \"%1\" property for table field from \"%2\" to \"%3\"",
                        m_alterTableAction.propertyName(),
                        m_oldValue.toString(),
                        m_alterTableAction.newValue().toString()));

    qDebug() << debugString();
}

ChangeFieldPropertyCommand::~ChangeFieldPropertyCommand()
{
    delete m_oldListData;
    delete m_listData;
}

QString ChangeFieldPropertyCommand::debugString() const
{
    QString s(text().toString());
    if (m_oldListData || m_listData)
        s += QString("\nAnd list data from [%1]\n  to [%2]")
             .arg(m_oldListData ?
                  QString("%1 -> %2")
                  .arg(m_oldListData->keysAsStringList().join(",")).arg(m_oldListData->names.join(","))
                  : QString("<NONE>"))
             .arg(m_listData ?
                  QString("%1 -> %2")
                  .arg(m_listData->keysAsStringList().join(",")).arg(m_listData->names.join(","))
                  : QString("<NONE>"));
    return s + QString(" (UID=%1)").arg(m_alterTableAction.uid());
}

void ChangeFieldPropertyCommand::redoInternal()
{
    m_view->changeFieldProperty(
        m_alterTableAction.uid(),
        m_alterTableAction.propertyName().toLatin1(),
        m_alterTableAction.newValue(), m_listData);
}

void ChangeFieldPropertyCommand::undoInternal()
{
    m_view->changeFieldProperty(
        m_alterTableAction.uid(),
        m_alterTableAction.propertyName().toLatin1(),
        m_oldValue, m_oldListData);
}

KDbAlterTableHandler::ActionBase* ChangeFieldPropertyCommand::createAction() const
{
    if (m_alterTableAction.propertyName() == "subType") {//skip these properties
        return 0;
    }
    return new KDbAlterTableHandler::ChangeFieldPropertyAction(m_alterTableAction);
}

//--------------------------------------------------------

RemoveFieldCommand::RemoveFieldCommand(Command* parent, KexiTableDesignerView* view, int fieldIndex,
                                       const KPropertySet* set)
        : Command(parent, view)
        , m_alterTableAction(set ? (*set)["name"].value().toString() : QString(),
                             set ? (*set)["uid"].value().toInt() : -1)
        , m_set(set ? new KPropertySet(*set /*deep copy*/) : 0)
        , m_fieldIndex(fieldIndex)
{
    if (m_set)
        setText(kundo2_i18n("Remove table field \"%1\"", m_alterTableAction.fieldName()));
    else
        setText(kundo2_i18n("Remove empty row at position %1", m_fieldIndex));
}

RemoveFieldCommand::~RemoveFieldCommand()
{
    delete m_set;
}

void RemoveFieldCommand::redoInternal()
{
// m_view->deleteField( m_fieldIndex );
    m_view->deleteRow(m_fieldIndex);
}

void RemoveFieldCommand::undoInternal()
{
    m_view->insertEmptyRow(m_fieldIndex);
    if (m_set)
        m_view->insertField(m_fieldIndex, *m_set);
}

QString RemoveFieldCommand::debugString() const
{
    if (!m_set)
        return text().toString();

    return text().toString() + "\nAT ROW " + QString::number(m_fieldIndex)
           + ", FIELD: " + (*m_set)["caption"].value().toString()
           + QString(" (UID=%1)").arg(m_alterTableAction.uid());
}

KDbAlterTableHandler::ActionBase* RemoveFieldCommand::createAction() const
{
    return new KDbAlterTableHandler::RemoveFieldAction(m_alterTableAction);
}

//--------------------------------------------------------

InsertFieldCommand::InsertFieldCommand(Command* parent, KexiTableDesignerView* view,
                                       int fieldIndex/*, const KDbField& field*/, const KPropertySet& set)
        : Command(parent, view)
        , m_alterTableAction(0) //fieldIndex, new KDbField(field) /*deep copy*/)
        , m_set(set)   //? new KPropertySet(*set) : 0 )
{
    KDbField *f = view->buildField(m_set);
    if (f)
        m_alterTableAction = new KDbAlterTableHandler::InsertFieldAction(
            fieldIndex, f, set["uid"].value().toInt());
    else //null action
        m_alterTableAction = new KDbAlterTableHandler::InsertFieldAction(true);

    setText(kundo2_i18n("Insert table field \"%1\"", m_set["caption"].value().toString()));
}

InsertFieldCommand::~InsertFieldCommand()
{
    delete m_alterTableAction;
}

void InsertFieldCommand::redoInternal()
{
    m_view->insertField(m_alterTableAction->index(), /*m_alterTableAction.field(),*/ m_set);
}

void InsertFieldCommand::undoInternal()
{
    m_view->clearRow(m_alterTableAction->index());  //m_alterTableAction.index() );
}

KDbAlterTableHandler::ActionBase* InsertFieldCommand::createAction() const
{
    return new KDbAlterTableHandler::InsertFieldAction(*m_alterTableAction);
}

QString InsertFieldCommand::debugString() const
{
    return text().toString() + "\nAT ROW " + QString::number(m_alterTableAction->index())
           + ", FIELD: " + m_set["caption"].value().toString();
}

//--------------------------------------------------------

ChangePropertyVisibilityCommand::ChangePropertyVisibilityCommand(Command* parent, KexiTableDesignerView* view,
        const KPropertySet& set, const QByteArray& propertyName, bool visible)
        : Command(parent, view)
        , m_alterTableAction(set.property("name").value().toString(), propertyName, visible, set["uid"].value().toInt())
        , m_oldVisibility(set.property(propertyName).isVisible())
{
    setText(kundo2_noi18n("[internal] Change \"%1\" visibility from \"%2\" to \"%3\"",
                          m_alterTableAction.propertyName(),
                          m_oldVisibility ? "true" : "false",
                          m_alterTableAction.newValue().toBool() ? "true" : "false"));

    qDebug() << debugString();
}

ChangePropertyVisibilityCommand::~ChangePropertyVisibilityCommand()
{
}

void ChangePropertyVisibilityCommand::redoInternal()
{
    m_view->changePropertyVisibility(
        m_alterTableAction.uid(),
        m_alterTableAction.propertyName().toLatin1(),
        m_alterTableAction.newValue().toBool());
}

void ChangePropertyVisibilityCommand::undoInternal()
{
    m_view->changePropertyVisibility(
        m_alterTableAction.uid(),
        m_alterTableAction.propertyName().toLatin1(),
        m_oldVisibility);
}

//--------------------------------------------------------

InsertEmptyRowCommand::InsertEmptyRowCommand(Command* parent, KexiTableDesignerView* view, int row)
        : Command(parent, view)
        , m_alterTableAction(true) //unused, null action
        , m_row(row)
{
    setText(kundo2_noi18n("Insert empty row at position %1", m_row));
}

InsertEmptyRowCommand::~InsertEmptyRowCommand()
{
}

void InsertEmptyRowCommand::redoInternal()
{
    m_view->insertEmptyRow(m_row);
}

void InsertEmptyRowCommand::undoInternal()
{
    // let's assume the row is empty...
    m_view->deleteRow(m_row);
}

