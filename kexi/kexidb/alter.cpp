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
#include <kexiutils/utils.h>

#include <qmap.h>

#include <kstaticdeleter.h>

namespace KexiDB {
class AlterTableHandler::Private
{
	public:
		Private()
		{}
		~Private()
		{}
		ActionList actions;
		QGuardedPtr<Connection> conn;
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
 : m_alteringRequirements(0)
 , m_order(-1)
 , m_null(null)
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

void AlterTableHandler::ActionBase::simplifyActions(ActionDictDict &fieldActions)
{
	Q_UNUSED(fieldActions);
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

static KStaticDeleter< QMap<QCString,int> > KexiDB_alteringTypeForProperty_deleter;
QMap<QCString,int> *KexiDB_alteringTypeForProperty = 0;

//! @internal
static int alteringTypeForProperty(const char *propertyName)
{
	if (!KexiDB_alteringTypeForProperty) {
		KexiDB_alteringTypeForProperty_deleter.setObject( KexiDB_alteringTypeForProperty, 
			new QMap<QCString,int>() );
#define I(name, type) \
	KexiDB_alteringTypeForProperty->insert(name, AlterTableHandler::type)
#define I2(name, type1, type2) \
	KexiDB_alteringTypeForProperty->insert(name, AlterTableHandler::type1|AlterTableHandler::type2)

	/* useful links: 
		http://dev.mysql.com/doc/refman/5.0/en/create-table.html
	*/
		I("visibleDecimalPlaces", ExtendedSchemaAlteringRequired);
		I("name", PhysicalAlteringRequired);
		I2("type", PhysicalAlteringRequired, DataConversionRequired);
		I("caption", ExtendedSchemaAlteringRequired);
		I("description", ExtendedSchemaAlteringRequired);
		I2("unsigned", PhysicalAlteringRequired, DataConversionRequired); // always?
		I2("length", PhysicalAlteringRequired, DataConversionRequired); // always?
		I2("precision", PhysicalAlteringRequired, DataConversionRequired); // always?
		I("width", ExtendedSchemaAlteringRequired);
		// defaultValue: depends on backend, for mysql it can only by a constant or now()...
		// -- should we look at Driver here?
		I2("defaultValue", PhysicalAlteringRequired, ExtendedSchemaAlteringRequired);
		I2("primaryKey", PhysicalAlteringRequired, DataConversionRequired);
		I2("unique", PhysicalAlteringRequired, DataConversionRequired); // we may want to add an Index here
		I2("notNull", PhysicalAlteringRequired, DataConversionRequired); // we may want to add an Index here
		// allowEmpty: only support it just at kexi level? maybe there is a backend that supports this?
		I("allowEmpty", ExtendedSchemaAlteringRequired); 
		I2("autoIncrement", PhysicalAlteringRequired, DataConversionRequired); // data conversion may be hard here
		I2("indexed", PhysicalAlteringRequired, DataConversionRequired); // we may want to add an Index here

		// easier cases follow...
		I("visibleDecimalPlaces", ExtendedSchemaAlteringRequired);
		//more to come...
#undef I
#undef I2
	}
	return (*KexiDB_alteringTypeForProperty)[propertyName]; 
}

//---

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

void AlterTableHandler::ChangeFieldPropertyAction::updateAlteringRequirements()
{
//	m_alteringRequirements = ???;
	setAlteringRequirements( alteringTypeForProperty( m_propertyName.lower().latin1() ) );
}

QString AlterTableHandler::ChangeFieldPropertyAction::debugString()
{
	return QString("Set \"%1\" property for table field \"%2\" to \"%3\"")
		.arg(m_propertyName).arg(fieldName()).arg(m_newValue.toString());
}

static AlterTableHandler::ActionDict* createActionDict( 
	AlterTableHandler::ActionDictDict &fieldActions, const QString& forFieldName )
{
	AlterTableHandler::ActionDict* dict = new AlterTableHandler::ActionDict(101, false);
	dict->setAutoDelete(true);
	fieldActions.insert( forFieldName, dict );
	return dict;
}

static void debugActionDict(AlterTableHandler::ActionDict *dict, const char* fieldName, bool simulate)
{
	QString dbg = QString("Action dict for field \"%1\" (%2):").arg(fieldName).arg(dict->count());
	KexiDBDbg << dbg << endl;
#ifdef KEXI_DEBUG_GUI
	if (simulate)
		KexiUtils::addAlterTableActionDebug(dbg, 1);
#endif
	for (AlterTableHandler::ActionDictIterator it(*dict); it.current(); ++it) {
		it.current()->debug();
#ifdef KEXI_DEBUG_GUI
		if (simulate)
			KexiUtils::addAlterTableActionDebug(it.current()->debugString(), 2);
#endif
	}
}

static void debugFieldActions(const AlterTableHandler::ActionDictDict &fieldActions, bool simulate)
{
#ifdef KEXI_DEBUG_GUI
	if (simulate)
		KexiUtils::addAlterTableActionDebug("** Simplified Field Actions:");
#endif
	for (AlterTableHandler::ActionDictDictIterator it(fieldActions); it.current(); ++it) {
		debugActionDict(it.current(), it.currentKey(), simulate);
	}
}

void AlterTableHandler::ChangeFieldPropertyAction::simplifyActions(ActionDictDict &fieldActions)
{
//	ActionDict *actionsForThisField = fieldActions[ newName ];
	if (m_propertyName=="name") {
		// special case: name1 -> name2, i.e. rename action
		QString newName( newValue().toString() );
		// try to find rename(newName, otherName) action
		ActionDict *actionsLikeThis = fieldActions[ newName ];
		ActionBase *renameActionLikeThis = actionsLikeThis ? actionsLikeThis->find( "name" ) : 0;
		if (dynamic_cast<ChangeFieldPropertyAction*>(renameActionLikeThis)) {
			// 1. instead of having rename(fieldName(), newValue()) action,
			// create rename(fieldName(), otherName) action
			AlterTableHandler::ChangeFieldPropertyAction* newRenameAction 
				= new AlterTableHandler::ChangeFieldPropertyAction( *this );
			newRenameAction->m_newValue = dynamic_cast<ChangeFieldPropertyAction*>(renameActionLikeThis)->m_newValue;
			// 2. (m_order is the same as in newAction)
			// 3. replace prev. rename action (if any)
			actionsLikeThis->remove( "name" );
			ActionDict *adict = fieldActions[ fieldName() ];
			if (!adict)
				adict = createActionDict( fieldActions, fieldName() );
			adict->insert(m_propertyName.latin1(), newRenameAction);
		}
		else {
			//just insert a copy of the rename action
			if (!actionsLikeThis)
				actionsLikeThis = createActionDict( fieldActions, fieldName() );
			AlterTableHandler::ChangeFieldPropertyAction* newRenameAction = new AlterTableHandler::ChangeFieldPropertyAction( *this );
			KexiDBDbg << "ChangeFieldPropertyAction::simplifyActions(): insert into '"<< fieldName() << "' dict:"  << newRenameAction->debugString() << endl;
			actionsLikeThis->insert( m_propertyName.latin1(), newRenameAction );
			return;
		}
		if (actionsLikeThis) {
			// 4. change "field name" information to fieldName() in any action that 
			//    is related to newName
			//    e.g. if there is setCaption("B", "captionA") action after rename("A","B"),
			//    replace setCaption action with setCaption("A", "captionA")
			foreach_dict (ActionDictIterator, it, *actionsLikeThis) {
				dynamic_cast<ChangeFieldPropertyAction*>(it.current())->m_fieldName = fieldName();
			}
		}
		return;
	}
	// other cases: just give up with adding this "intermediate" action
	// e.g. [ setCaption(A, "captionA"), setCaption(A, "captionB") ]
	//  becomes: [ setCaption(A, "captionB") ]
	// because adding this action does nothing
	ActionDict *nextActionsLikeThis = fieldActions[ fieldName() ];
	if (!nextActionsLikeThis || !nextActionsLikeThis->find( m_propertyName )) { //no such action, add this
		AlterTableHandler::ChangeFieldPropertyAction* newAction 
			= new AlterTableHandler::ChangeFieldPropertyAction( *this );
		if (!nextActionsLikeThis)
			nextActionsLikeThis = createActionDict( fieldActions, fieldName() );
		nextActionsLikeThis->insert( m_propertyName.latin1(), newAction );
	}
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

void AlterTableHandler::RemoveFieldAction::updateAlteringRequirements()
{
//! @todo sometimes add DataConversionRequired (e.g. when relationships require removing orphaned records) ?

	setAlteringRequirements( PhysicalAlteringRequired );
	//todo
}

QString AlterTableHandler::RemoveFieldAction::debugString()
{
	return QString("Remove table field \"%1\"").arg(m_fieldName);
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

void AlterTableHandler::InsertFieldAction::updateAlteringRequirements()
{
//! @todo sometimes add DataConversionRequired (e.g. when relationships require removing orphaned records) ?

	setAlteringRequirements( PhysicalAlteringRequired );
	//todo
}

QString AlterTableHandler::InsertFieldAction::debugString()
{
	return QString("Insert table field \"%1\" at position %1")
		.arg(m_field->name()).arg(m_index);
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

void AlterTableHandler::MoveFieldPositionAction::updateAlteringRequirements()
{
	setAlteringRequirements( ExtendedSchemaAlteringRequired );
	//todo
}

QString AlterTableHandler::MoveFieldPositionAction::debugString()
{
	return QString("Move table field \"%1\" to position %1")
		.arg(m_fieldName).arg(m_index);
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

void AlterTableHandler::addAction(ActionBase* action)
{
	d->actions.append(action);
}

AlterTableHandler& AlterTableHandler::operator<< ( ActionBase* action )
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

void AlterTableHandler::setActions(const ActionList& actions)
{
	d->actions = actions;
}

void AlterTableHandler::debug()
{
	KexiDBDbg << "AlterTableHandler's actions:" << endl;
	foreach_list (ActionListIterator, it, d->actions)
		it.current()->debug();
}

bool AlterTableHandler::execute(const QString& tableName, bool simulate)
{
	if (!d->conn) {
		//err msg?
		return false;
	}
	if (d->conn->isReadOnly()) {
		//err msg?
		return false;
	}
	if (!d->conn->isDatabaseUsed()) {
		//err msg?
		return false;
	}
	TableSchema *table = d->conn->tableSchema(tableName);
	if (!table) {
		//err msg?
		return false;
	}

	debug();

	// Find a sum of requirements...
	int req = 0;
	int order = 0;
	for(ActionListIterator it(d->actions); it.current(); ++it, order++) {
		it.current()->updateAlteringRequirements();
		it.current()->m_order = order;
		req |= it.current()->alteringRequirements();
	}

	QString dbg = QString("AlterTableHandler::execute(): overall altering requirements: %1").arg(req);
	KexiDBDbg << dbg << endl;
#ifdef KEXI_DEBUG_GUI
		if (simulate)
			KexiUtils::addAlterTableActionDebug(dbg, 0);
#endif
	if (req == 0) {
		return true;
	}

	/* Simplify actions list if possible and check for errors

	How to do it?
	- track property changes/deletions in reversed order
	- reduce intermediate actions

	Trivial example 1:
	 *action1: "rename field a to b"
	 *action2: "rename field b to c"
	 *action3: "rename field c to d"

	 After reduction:
	 *action1: "rename field a to d" 
	 Summing up: we have tracked what happens to field curently named "d"
	 and eventually discovered that it was originally named "a".

	Trivial example 2:
	 *action1: "rename field a to b"
	 *action2: "rename field b to c"
	 *action3: "remove field b"
	 After reduction:
	 *action3: "remove field b"
	 Summing up: we have noticed that field "b" has beed eventually removed
	 so we needed to find all actions related to this field and remove them.
	 This is good optimization, as some of the eventually removed actions would 
	 be difficult to perform and/or costly, what would be a waste of resources
	 and a source of unwanted questions sent to the user.
	*/

	//todo
	ActionListIterator it(d->actions);

	// Fields-related actions. 
	ActionDictDict fieldActions(3001);
	fieldActions.setAutoDelete(true);
	ActionBase* action;
	for(it.toLast(); (action = it.current()); --it) {
		action->simplifyActions( fieldActions );
//		simplifyAction(it.current(), fieldActions);
	}

	debugFieldActions(fieldActions, simulate);

	if (simulate) //?
		return true;

	return true;
}

/*
void simplifyAction(ActionBase* action, QDict<ActionBase> &fieldActions)
{
	if (dynamic_cast<ChangeFieldPropertyAction*>(action) {
}
*/