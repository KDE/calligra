/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "alter.h"

namespace KexiDB {
class AlterTableHandler::Private
{
	public:
		Private()
		{}
		~Private()
		{}
		ActionList actions;
		QPointer<Connection> conn;
};
}

using namespace KexiDB;

//! a global instance used to when returning null is needed
AlterTableHandler::ChangeFieldPropertyAction nullChangeFieldPropertyAction(true);
AlterTableHandler::RemoveFieldAction nullRemoveFieldAction(true);
AlterTableHandler::InsertFieldAction nullInsertFieldAction(true);
AlterTableHandler::MoveFieldPositionAction nullMoveFieldPositionAction(true);

//--------------------------------------------------------

AlterTableHandler::ActionBase::ActionBase(bool null)
 : m_null(null)
{
}

AlterTableHandler::ActionBase::~ActionBase()
{
}

AlterTableHandler::ChangeFieldPropertyAction& AlterTableHandler::ActionBase::toChangeFieldPropertyAction()
{
	if (dynamic_cast<ChangeFieldPropertyAction*>(this))
		return *dynamic_cast<ChangeFieldPropertyAction*>(this);
	return nullChangeFieldPropertyAction;
}

AlterTableHandler::RemoveFieldAction& AlterTableHandler::ActionBase::toRemoveFieldAction()
{
	if (dynamic_cast<RemoveFieldAction*>(this))
		return *dynamic_cast<RemoveFieldAction*>(this);
	return nullRemoveFieldAction;
}

AlterTableHandler::InsertFieldAction& AlterTableHandler::ActionBase::toInsertFieldAction()
{
	if (dynamic_cast<InsertFieldAction*>(this))
		return *dynamic_cast<InsertFieldAction*>(this);
	return nullInsertFieldAction;
}

AlterTableHandler::MoveFieldPositionAction& AlterTableHandler::ActionBase::toMoveFieldPositionAction()
{
	if (dynamic_cast<MoveFieldPositionAction*>(this))
		return *dynamic_cast<MoveFieldPositionAction*>(this);
	return nullMoveFieldPositionAction;
}

//--------------------------------------------------------

AlterTableHandler::FieldActionBase::FieldActionBase(const QString& fieldName)
 : ActionBase()
 , m_fieldName(fieldName)
{
}

AlterTableHandler::FieldActionBase::FieldActionBase(bool)
 : ActionBase(true)
{
}

AlterTableHandler::FieldActionBase::~FieldActionBase()
{
}

//--------------------------------------------------------

AlterTableHandler::ChangeFieldPropertyAction::ChangeFieldPropertyAction(
	const QString& fieldName, const QString& propertyName, const QVariant& newValue)
 : FieldActionBase(fieldName)
 , m_propertyName(propertyName)
 , m_newValue(newValue)
{
}

AlterTableHandler::ChangeFieldPropertyAction::ChangeFieldPropertyAction(bool)
 : FieldActionBase(true)
{
}

AlterTableHandler::ChangeFieldPropertyAction::~ChangeFieldPropertyAction()
{
}

//--------------------------------------------------------

AlterTableHandler::RemoveFieldAction::RemoveFieldAction(const QString& fieldName)
 : FieldActionBase(fieldName)
{
}

AlterTableHandler::RemoveFieldAction::RemoveFieldAction(bool)
 : FieldActionBase(true)
{
}

AlterTableHandler::RemoveFieldAction::~RemoveFieldAction()
{
}

//--------------------------------------------------------

AlterTableHandler::InsertFieldAction::InsertFieldAction(int fieldIndex, KexiDB::Field *field)
 : FieldActionBase(field->name())
 , m_index(fieldIndex)
 , m_field(field)
{
	Q_ASSERT(field);
}

AlterTableHandler::InsertFieldAction::InsertFieldAction(bool)
 : FieldActionBase(true)
 , m_index(0)
 , m_field(0)
{
}

AlterTableHandler::InsertFieldAction::~InsertFieldAction()
{
	delete m_field;
}

//--------------------------------------------------------

AlterTableHandler::MoveFieldPositionAction::MoveFieldPositionAction(
	int fieldIndex, const QString& fieldName)
 : FieldActionBase(fieldName)
 , m_index(fieldIndex)
{
}

AlterTableHandler::MoveFieldPositionAction::MoveFieldPositionAction(bool)
 : FieldActionBase(true)
{
}

AlterTableHandler::MoveFieldPositionAction::~MoveFieldPositionAction()
{
}

//--------------------------------------------------------

AlterTableHandler::AlterTableHandler(Connection &conn)
 : Object()
 , d( new Private() )
{
	d->conn = &conn;
}

AlterTableHandler::~AlterTableHandler()
{
	delete d;
}

void AlterTableHandler::addAction(const ActionBase& action)
{
	d->actions.append(action);
}

AlterTableHandler& AlterTableHandler::operator<< ( const ActionBase& action )
{
	d->actions.append(action);
	return *this;
}

const AlterTableHandler::ActionList& AlterTableHandler::actions() const
{
	return d->actions;
}

void AlterTableHandler::removeAction(int index)
{
	d->actions.remove( d->actions.at(index) );
}

void AlterTableHandler::clear()
{
	d->actions.clear();
}

bool AlterTableHandler::execute()
{
	// Simplify actions list if possible and check for errors

	return true;
}

