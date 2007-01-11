/* This file is part of the KDE project
   Copyright (C) 2006-2007 Jaroslaw Staniek <js@iidea.pl>

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
#include "utils.h"
#include <kexiutils/utils.h>

#include <qmap.h>

#include <kstaticdeleter.h>

#include <stdlib.h>

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

AlterTableHandler::FieldActionBase::FieldActionBase(const QString& fieldName, int uid)
 : ActionBase()
 , m_fieldUID(uid)
 , m_fieldName(fieldName)
{
}

AlterTableHandler::FieldActionBase::FieldActionBase(bool)
 : ActionBase(true)
 , m_fieldUID(-1)
{
}

AlterTableHandler::FieldActionBase::~FieldActionBase()
{
}

//--------------------------------------------------------

static KStaticDeleter< QMap<Q3CString,int> > KexiDB_alteringTypeForProperty_deleter;
QMap<Q3CString,int> *KexiDB_alteringTypeForProperty = 0;

//! @internal
int AlterTableHandler::alteringTypeForProperty(const Q3CString& propertyName)
{
	if (!KexiDB_alteringTypeForProperty) {
		KexiDB_alteringTypeForProperty_deleter.setObject( KexiDB_alteringTypeForProperty, 
			new QMap<Q3CString,int>() );
#define I(name, type) \
	KexiDB_alteringTypeForProperty->insert(Q3CString(name).toLower(), (int)AlterTableHandler::type)
#define I2(name, type1, type2) \
	flag = (int)AlterTableHandler::type1|(int)AlterTableHandler::type2; \
	if (flag & AlterTableHandler::PhysicalAlteringRequired) \
		flag |= AlterTableHandler::MainSchemaAlteringRequired; \
	KexiDB_alteringTypeForProperty->insert(Q3CString(name).toLower(), flag)

	/* useful links: 
		http://dev.mysql.com/doc/refman/5.0/en/create-table.html
	*/
		// ExtendedSchemaAlteringRequired is here because when the field is renamed, 
		// we need to do the same rename in extended table schema: <field name="...">
		int flag;
		I2("name", PhysicalAlteringRequired, MainSchemaAlteringRequired);
		I2("type", PhysicalAlteringRequired, DataConversionRequired);
		I("caption", MainSchemaAlteringRequired);
		I("description", MainSchemaAlteringRequired);
		I2("unsigned", PhysicalAlteringRequired, DataConversionRequired); // always?
		I2("length", PhysicalAlteringRequired, DataConversionRequired); // always?
		I2("precision", PhysicalAlteringRequired, DataConversionRequired); // always?
		I("width", MainSchemaAlteringRequired);
		// defaultValue: depends on backend, for mysql it can only by a constant or now()...
		// -- should we look at Driver here?
#ifdef KEXI_NO_UNFINISHED
//! @todo reenable
		I("defaultValue", MainSchemaAlteringRequired);
#else
		I2("defaultValue", PhysicalAlteringRequired, MainSchemaAlteringRequired);
#endif
		I2("primaryKey", PhysicalAlteringRequired, DataConversionRequired);
		I2("unique", PhysicalAlteringRequired, DataConversionRequired); // we may want to add an Index here
		I2("notNull", PhysicalAlteringRequired, DataConversionRequired); // we may want to add an Index here
		// allowEmpty: only support it just at kexi level? maybe there is a backend that supports this?
		I2("allowEmpty", PhysicalAlteringRequired, MainSchemaAlteringRequired); 
		I2("autoIncrement", PhysicalAlteringRequired, DataConversionRequired); // data conversion may be hard here
		I2("indexed", PhysicalAlteringRequired, DataConversionRequired); // we may want to add an Index here

		// easier cases follow...
		I("visibleDecimalPlaces", ExtendedSchemaAlteringRequired);
		//more to come...
#undef I
#undef I2
	}
	return (*KexiDB_alteringTypeForProperty)[propertyName.toLower()]; 
}

//---

AlterTableHandler::ChangeFieldPropertyAction::ChangeFieldPropertyAction(
	const QString& fieldName, const QString& propertyName, const QVariant& newValue, int uid)
 : FieldActionBase(fieldName, uid)
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
	setAlteringRequirements( alteringTypeForProperty( m_propertyName.latin1() ) );
}

QString AlterTableHandler::ChangeFieldPropertyAction::debugString(const DebugOptions& debugOptions)
{
	QString s = QString("Set \"%1\" property for table field \"%2\" to \"%3\"")
		.arg(m_propertyName).arg(fieldName()).arg(m_newValue.toString());
	if (debugOptions.showUID)
		s.append(QString(" (UID=%1)").arg(m_fieldUID));
	return s;
}

static AlterTableHandler::ActionDict* createActionDict( 
	AlterTableHandler::ActionDictDict &fieldActions, int forFieldUID )
{
	AlterTableHandler::ActionDict* dict = new AlterTableHandler::ActionDict(101, false);
	dict->setAutoDelete(true);
	fieldActions.insert( forFieldUID, dict );
	return dict;
}

static void debugAction(AlterTableHandler::ActionBase *action, int nestingLevel, 
  bool simulate, const QString& prependString = QString::null, QString* debugTarget = 0)
{
	QString debugString;
	if (!debugTarget)
		debugString = prependString;
	if (action) {
		AlterTableHandler::ActionBase::DebugOptions debugOptions;
		debugOptions.showUID = debugTarget==0;
		debugOptions.showFieldDebug = debugTarget!=0;
		debugString += action->debugString( debugOptions );
	}
	else {
		if (!debugTarget)
			debugString += "[No action]"; //hmm
	}
	if (debugTarget) {
		if (!debugString.isEmpty())
			*debugTarget += debugString + '\n';
	}
	else {
		KexiDBDbg << debugString << endl;
#ifdef KEXI_DEBUG_GUI
		if (simulate)
			KexiUtils::addAlterTableActionDebug(debugString, nestingLevel);
#endif
	}
}

static void debugActionDict(AlterTableHandler::ActionDict *dict, int fieldUID, bool simulate)
{
	QString fieldName;
	AlterTableHandler::ActionDictIterator it(*dict);
	if (dynamic_cast<AlterTableHandler::FieldActionBase*>(it.current())) //retrieve field name from the 1st related action
		fieldName = dynamic_cast<AlterTableHandler::FieldActionBase*>(it.current())->fieldName();
	else
		fieldName = "??";
	QString dbg = QString("Action dict for field \"%1\" (%2, UID=%3):")
		.arg(fieldName).arg(dict->count()).arg(fieldUID);
	KexiDBDbg << dbg << endl;
#ifdef KEXI_DEBUG_GUI
	if (simulate)
		KexiUtils::addAlterTableActionDebug(dbg, 1);
#endif
	for (;it.current(); ++it) {
		debugAction(it.current(), 2, simulate);
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

/*! 
 Legend: A,B==fields, P==property, [....]==action, (..,..,..) group of actions, <...> internal operation.
 Case 1. (special)
    when new action=[rename A to B]
    and exists=[rename B to C] 
    =>
    remove [rename B to C]
    and set result to new [rename A to C]
    and go to 1b.
 Case 1b. when new action=[rename A to B]
    and actions exist like [set property P to C in field B] 
    or like [delete field B] 
    or like [move field B] 
    =>
    change B to A for all these actions
 Case 2. when new action=[change property in field A] (property != name)
    and exists=[remove A] or exists=[change property in field A]
    =>
    do not add [change property in field A] because it will be removed anyway or the property will change
*/
void AlterTableHandler::ChangeFieldPropertyAction::simplifyActions(ActionDictDict &fieldActions)
{
	ActionDict *actionsLikeThis = fieldActions[ uid() ];
	if (m_propertyName=="name") {
		// Case 1. special: name1 -> name2, i.e. rename action
		QString newName( newValue().toString() );
		// try to find rename(newName, otherName) action
		ActionBase *renameActionLikeThis = actionsLikeThis ? actionsLikeThis->find( "name" ) : 0;
		if (dynamic_cast<ChangeFieldPropertyAction*>(renameActionLikeThis)) {
			// 1. instead of having rename(fieldName(), newValue()) action,
			// let's have rename(fieldName(), otherName) action
			dynamic_cast<ChangeFieldPropertyAction*>(renameActionLikeThis)->m_newValue 
				= dynamic_cast<ChangeFieldPropertyAction*>(renameActionLikeThis)->m_newValue;
/*			AlterTableHandler::ChangeFieldPropertyAction* newRenameAction 
				= new AlterTableHandler::ChangeFieldPropertyAction( *this );
			newRenameAction->m_newValue = dynamic_cast<ChangeFieldPropertyAction*>(renameActionLikeThis)->m_newValue;
			// (m_order is the same as in newAction)
			// replace prev. rename action (if any)
			actionsLikeThis->remove( "name" );
			ActionDict *adict = fieldActions[ fieldName().latin1() ];
			if (!adict)
				adict = createActionDict( fieldActions, fieldName() );
			adict->insert(m_propertyName.latin1(), newRenameAction);*/
		}
		else {
			ActionBase *removeActionForThisField = actionsLikeThis ? actionsLikeThis->find( ":remove:" ) : 0;
			if (removeActionForThisField) {
				//if this field is going to be removed, jsut change the action's field name 
				// and do not add a new action
			}
			else {
				//just insert a copy of the rename action
				if (!actionsLikeThis)
					actionsLikeThis = createActionDict( fieldActions, uid() ); //fieldName() );
				AlterTableHandler::ChangeFieldPropertyAction* newRenameAction 
					= new AlterTableHandler::ChangeFieldPropertyAction( *this );
				KexiDBDbg << "ChangeFieldPropertyAction::simplifyActions(): insert into '"
					<< fieldName() << "' dict:"  << newRenameAction->debugString() << endl;
				actionsLikeThis->insert( m_propertyName.latin1(), newRenameAction );
				return;
			}
		}
		if (actionsLikeThis) {
			// Case 1b. change "field name" information to fieldName() in any action that 
			//    is related to newName
			//    e.g. if there is setCaption("B", "captionA") action after rename("A","B"),
			//    replace setCaption action with setCaption("A", "captionA")
			foreach_dict (ActionDictIterator, it, *actionsLikeThis) {
				dynamic_cast<FieldActionBase*>(it.current())->setFieldName( fieldName() );
			}
		}
		return;
	}
	ActionBase *removeActionForThisField = actionsLikeThis ? actionsLikeThis->find( ":remove:" ) : 0;
	if (removeActionForThisField) {
		//if this field is going to be removed, do not add a new action
		return;
	}
	// Case 2. other cases: just give up with adding this "intermediate" action
	// so, e.g. [ setCaption(A, "captionA"), setCaption(A, "captionB") ]
	//  becomes: [ setCaption(A, "captionB") ]
	// because adding this action does nothing
	ActionDict *nextActionsLikeThis = fieldActions[ uid() ]; //fieldName().latin1() ];
	if (!nextActionsLikeThis || !nextActionsLikeThis->find( m_propertyName.latin1() )) { 
		//no such action, add this
		AlterTableHandler::ChangeFieldPropertyAction* newAction 
			= new AlterTableHandler::ChangeFieldPropertyAction( *this );
		if (!nextActionsLikeThis)
			nextActionsLikeThis = createActionDict( fieldActions, uid() );//fieldName() );
		nextActionsLikeThis->insert( m_propertyName.latin1(), newAction );
	}
}

bool AlterTableHandler::ChangeFieldPropertyAction::shouldBeRemoved(ActionDictDict &fieldActions)
{
	Q_UNUSED(fieldActions);
	return fieldName().lower() == m_newValue.toString().lower();
}

tristate AlterTableHandler::ChangeFieldPropertyAction::updateTableSchema(TableSchema &table, Field* field,
	QMap<QString, QString>& fieldMap)
{
	//1. Simpler cases first: changes that do not affect table schema at all
	// "caption", "description", "width", "visibleDecimalPlaces"
	if (SchemaAlteringRequired & alteringTypeForProperty(m_propertyName.latin1())) {
		bool result = KexiDB::setFieldProperty(*field, m_propertyName.latin1(), newValue());
		return result;
	}

	if (m_propertyName=="name") {
		if (fieldMap[ field->name() ] == field->name())
			fieldMap.remove( field->name() );
		fieldMap.insert( newValue().toString(), field->name() );
		table.renameField(field, newValue().toString());
		return true;
	}
	return cancelled;
}

/*! Many of the properties must be applied using a separate algorithm.
*/
tristate AlterTableHandler::ChangeFieldPropertyAction::execute(Connection &conn, TableSchema &table)
{
	Q_UNUSED(conn);
	Field *field = table.field( fieldName() );
	if (!field) {
		//! @todo errmsg
		return false;
	}
	bool result;
	//1. Simpler cases first: changes that do not affect table schema at all
	// "caption", "description", "width", "visibleDecimalPlaces"
	if (SchemaAlteringRequired & alteringTypeForProperty(m_propertyName.latin1())) {
		result = KexiDB::setFieldProperty(*field, m_propertyName.latin1(), newValue());
		return result;
	}

//todo
return true;

	//2. Harder cases, that often require special care
	if (m_propertyName=="name") {
		/*mysql:
		 A. Get real field type (it's safer): 
		    let <TYPE> be the 2nd "Type" column from result of "DESCRIBE tablename oldfieldname"
			( http://dev.mysql.com/doc/refman/5.0/en/describe.html )
		 B. Run "ALTER TABLE tablename CHANGE oldfieldname newfieldname <TYPE>";
			( http://dev.mysql.com/doc/refman/5.0/en/alter-table.html )
		*/
	}
	if (m_propertyName=="type") {
		/*mysql:
		 A. Like A. for "name" property above
		 B. Construct <TYPE> string, eg. "varchar(50)" using the driver
		 C. Like B. for "name" property above
		 (mysql then truncate the values for changes like varchar -> integer,
		 and properly convert the values for changes like integer -> varchar)

		 TODO: more cases to check
		*/
	}
	if (m_propertyName=="length") {
		//use "select max( length(o_name) ) from kexi__Objects"
		
	}
	if (m_propertyName=="primaryKey") {
//! @todo
	}

/*
		 "name", "unsigned", "precision", 
		 "defaultValue", "primaryKey", "unique", "notNull", "allowEmpty",
		 "autoIncrement", "indexed", 


	bool result = KexiDB::setFieldProperty(*field, m_propertyName.latin1(), newValue());
*/
	return result;
}

//--------------------------------------------------------

AlterTableHandler::RemoveFieldAction::RemoveFieldAction(const QString& fieldName, int uid)
 : FieldActionBase(fieldName, uid)
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
	//! @todo
}

QString AlterTableHandler::RemoveFieldAction::debugString(const DebugOptions& debugOptions)
{
	QString s = QString("Remove table field \"%1\"").arg(fieldName());
	if (debugOptions.showUID)
		s.append(QString(" (UID=%1)").arg(uid()));
	return s;
}

/*! 
 Legend: A,B==objects, P==property, [....]==action, (..,..,..) group of actions, <...> internal operation.
 Preconditions: we assume there cannot be such case encountered: ([remove A], [do something related on A])
  (except for [remove A], [insert A])
 General Case: it's safe to always insert a [remove A] action.
*/
void AlterTableHandler::RemoveFieldAction::simplifyActions(ActionDictDict &fieldActions)
{
	//! @todo not checked
	AlterTableHandler::RemoveFieldAction* newAction 
			= new AlterTableHandler::RemoveFieldAction( *this );
	ActionDict *actionsLikeThis = fieldActions[ uid() ]; //fieldName().latin1() ];
	if (!actionsLikeThis)
		actionsLikeThis = createActionDict( fieldActions, uid() ); //fieldName() );
	actionsLikeThis->insert( ":remove:", newAction ); //special
}

tristate AlterTableHandler::RemoveFieldAction::updateTableSchema(TableSchema &table, Field* field,
	QMap<QString, QString>& fieldMap)
{
	fieldMap.remove( field->name() );
	table.removeField(field);
	return true;
}

tristate AlterTableHandler::RemoveFieldAction::execute(Connection& conn, TableSchema& table)
{
	Q_UNUSED(conn);
	Q_UNUSED(table);
	//! @todo
	return true;
}

//--------------------------------------------------------

AlterTableHandler::InsertFieldAction::InsertFieldAction(int fieldIndex, KexiDB::Field *field, int uid)
 : FieldActionBase(field->name(), uid)
 , m_index(fieldIndex)
 , m_field(0)
{
	Q_ASSERT(field);
	setField(field);
}

AlterTableHandler::InsertFieldAction::InsertFieldAction(const InsertFieldAction& action)
 : FieldActionBase(action) //action.fieldName(), action.uid())
 , m_index(action.index())
{
	m_field = new KexiDB::Field( action.field() );
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

void AlterTableHandler::InsertFieldAction::setField(KexiDB::Field* field)
{
	if (m_field)
		delete m_field;
	m_field = field;
	setFieldName(m_field ? m_field->name() : QString::null);
}

void AlterTableHandler::InsertFieldAction::updateAlteringRequirements()
{
//! @todo sometimes add DataConversionRequired (e.g. when relationships require removing orphaned records) ?

	setAlteringRequirements( PhysicalAlteringRequired );
	//! @todo
}

QString AlterTableHandler::InsertFieldAction::debugString(const DebugOptions& debugOptions)
{
	QString s = QString("Insert table field \"%1\" at position %2")
		.arg(m_field->name()).arg(m_index);
	if (debugOptions.showUID)
		s.append(QString(" (UID=%1)").arg(m_fieldUID));
	if (debugOptions.showFieldDebug)
		s.append(QString(" (%1)").arg(m_field->debugString()));
	return s;
}

/*! 
 Legend: A,B==fields, P==property, [....]==action, (..,..,..) group of actions, <...> internal operation.


 Case 1: there are "change property" actions after the Insert action.
  -> change the properties in the Insert action itself and remove the "change property" actions.
 Examples:
   [Insert A] && [rename A to B] => [Insert B]
   [Insert A] && [change property P in field A] => [Insert A with P altered]
 Comment: we need to do this reduction because otherwise we'd need to do psyhical altering 
  right after [Insert A] if [rename A to B] follows.
*/
void AlterTableHandler::InsertFieldAction::simplifyActions(ActionDictDict &fieldActions)
{
	// Try to find actions related to this action
	ActionDict *actionsForThisField = fieldActions[ uid() ]; //m_field->name().latin1() ];

	ActionBase *removeActionForThisField = actionsForThisField ? actionsForThisField->find( ":remove:" ) : 0;
	if (removeActionForThisField) {
		//if this field is going to be removed, do not add a new action
		//and remove the "Remove" action
		actionsForThisField->remove(":remove:");
		return;
	}
	if (actionsForThisField) {
		//collect property values that have to be changed in this field
		QMap<Q3CString, QVariant> values;
		for (ActionDictIterator it(*actionsForThisField); it.current();) {
			ChangeFieldPropertyAction* changePropertyAction = dynamic_cast<ChangeFieldPropertyAction*>(it.current());
			if (changePropertyAction) {
				//if this field is going to be renamed, also update fieldName()
				if (changePropertyAction->propertyName()=="name") {
					setFieldName(changePropertyAction->newValue().toString());
				}
				values.insert( changePropertyAction->propertyName().latin1(), changePropertyAction->newValue() );
				//the subsequent "change property" action is no longer needed
				actionsForThisField->remove(changePropertyAction->propertyName().latin1());
			}
			else {
				++it;
			}
		}
		if (!values.isEmpty()) {
			//update field, so it will be created as one step
			KexiDB::Field f = field();
			if (KexiDB::setFieldProperties( f, values )) {
				field() = f;
				field().debug();
#ifdef KEXI_DEBUG_GUI
				KexiUtils::addAlterTableActionDebug(
					QString("** Property-set actions moved to field definition itself:\n")+field().debugString(), 0);
#endif
			}
			else {
#ifdef KEXI_DEBUG_GUI
				KexiUtils::addAlterTableActionDebug(
					QString("** Failed to set properties for field ")+field().debugString(), 0);
#endif
				KexiDBWarn << "AlterTableHandler::InsertFieldAction::simplifyActions(): KexiDB::setFieldProperties() failed!" << endl;
			}
		}
	}
	//ok, insert this action
	//! @todo not checked
	AlterTableHandler::InsertFieldAction* newAction 
			= new AlterTableHandler::InsertFieldAction( *this );
	if (!actionsForThisField)
		actionsForThisField = createActionDict( fieldActions, uid() );
	actionsForThisField->insert( ":insert:", newAction ); //special
}

tristate AlterTableHandler::InsertFieldAction::updateTableSchema(TableSchema &table, Field* field,
	QMap<QString, QString>& fieldMap)
{
	//in most cases we won't add the field to fieldMap
	Q_UNUSED(field);
//! @todo add it only when there should be fixed value (e.g. default) set for this new field...
	fieldMap.remove( this->field().name() );
	table.insertField(index(), new Field(this->field()));
	return true;
}

tristate AlterTableHandler::InsertFieldAction::execute(Connection& conn, TableSchema& table)
{
	Q_UNUSED(conn);
	Q_UNUSED(table);
	//! @todo
	return true;
}

//--------------------------------------------------------

AlterTableHandler::MoveFieldPositionAction::MoveFieldPositionAction(
	int fieldIndex, const QString& fieldName, int uid)
 : FieldActionBase(fieldName, uid)
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
	setAlteringRequirements( MainSchemaAlteringRequired );
	//! @todo
}

QString AlterTableHandler::MoveFieldPositionAction::debugString(const DebugOptions& debugOptions)
{
	QString s = QString("Move table field \"%1\" to position %2")
		.arg(fieldName()).arg(m_index);
	if (debugOptions.showUID)
		s.append(QString(" (UID=%1)").arg(uid()));
	return s;
}

void AlterTableHandler::MoveFieldPositionAction::simplifyActions(ActionDictDict &fieldActions)
{
	Q_UNUSED(fieldActions);
	//! @todo
}

tristate AlterTableHandler::MoveFieldPositionAction::execute(Connection& conn, TableSchema& table)
{
	Q_UNUSED(conn);
	Q_UNUSED(table);
	//! @todo
	return true;
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

TableSchema* AlterTableHandler::execute(const QString& tableName, ExecutionArguments& args)
{
	args.result = false;
	if (!d->conn) {
//! @todo err msg?
		return 0;
	}
	if (d->conn->isReadOnly()) {
//! @todo err msg?
		return 0;
	}
	if (!d->conn->isDatabaseUsed()) {
//! @todo err msg?
		return 0;
	}
	TableSchema *oldTable = d->conn->tableSchema(tableName);
	if (!oldTable) {
//! @todo err msg?
		return 0;
	}

	if (!args.debugString)
		debug();

	// Find a sum of requirements...
	int allActionsCount = 0;
	for(ActionListIterator it(d->actions); it.current(); ++it, allActionsCount++) {
		it.current()->updateAlteringRequirements();
		it.current()->m_order = allActionsCount;
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

	ActionListIterator it(d->actions);

	// Fields-related actions. 
	ActionDictDict fieldActions(3001);
	fieldActions.setAutoDelete(true);
	ActionBase* action;
	for(it.toLast(); (action = it.current()); --it) {
		action->simplifyActions( fieldActions );
	}

	if (!args.debugString)
		debugFieldActions(fieldActions, args.simulate);

	// Prepare actions for execution ----
	// - Sort actions by order
	ActionVector actionsVector(allActionsCount);
	int currentActionsCount = 0; //some actions may be removed
	args.requirements = 0;
	Q3Dict<char> fieldsWithChangedMainSchema(997); // Used to collect fields with changed main schema.
	                                              // This will be used when recreateTable is false to update kexi__fields
	for (ActionDictDictIterator it(fieldActions); it.current(); ++it) {
		for (AlterTableHandler::ActionDictIterator it2(*it.current());it2.current(); ++it2, currentActionsCount++) {
			if (it2.current()->shouldBeRemoved(fieldActions))
				continue;
			actionsVector.insert( it2.current()->m_order, it2.current() );
			// a sum of requirements...
			const int r = it2.current()->alteringRequirements();
			args.requirements |= r;
			if (r & MainSchemaAlteringRequired && dynamic_cast<ChangeFieldPropertyAction*>(it2.current())) {
				// Remember, this will be used when recreateTable is false to update kexi__fields, below.
				fieldsWithChangedMainSchema.insert( 
					dynamic_cast<ChangeFieldPropertyAction*>(it2.current())->fieldName(), (char*)1 );
			}
		}
	}
	// - Debug
	QString dbg = QString("** Overall altering requirements: %1").arg(args.requirements);
	KexiDBDbg << dbg << endl;

	if (args.onlyComputeRequirements) {
		args.result = true;
		return 0;
	}

	const bool recreateTable = (args.requirements & PhysicalAlteringRequired);

#ifdef KEXI_DEBUG_GUI
	if (args.simulate)
		KexiUtils::addAlterTableActionDebug(dbg, 0);
#endif
	dbg = QString("** Ordered, simplified actions (%1, was %2):").arg(currentActionsCount).arg(allActionsCount);
	KexiDBDbg << dbg << endl;
#ifdef KEXI_DEBUG_GUI
	if (args.simulate)
		KexiUtils::addAlterTableActionDebug(dbg, 0);
#endif
	for (int i=0; i<allActionsCount; i++) {
		debugAction(actionsVector[i], 1, args.simulate, QString("%1: ").arg(i+1), args.debugString);
	}

	if (args.requirements == 0) {//nothing to do
		args.result = true;
		return oldTable;
	}
	if (args.simulate) {//do not execute
		args.result = true;
		return oldTable;
	}
// @todo transaction!

	// Create new TableSchema
	TableSchema *newTable = recreateTable ? new TableSchema(*oldTable, false/*!copy id*/) : oldTable;
	// find nonexisting temp name for new table schema
	if (recreateTable) {
		QString tempDestTableName;
		while (true) {
			tempDestTableName = QString("%1_temp%2%3").arg(newTable->name()).arg(QString::number(rand(), 16)).arg(QString::number(rand(), 16));
			if (!d->conn->tableSchema(tempDestTableName))
				break;
		}
		newTable->setName( tempDestTableName );
	}
	oldTable->debug();
	if (recreateTable && !args.debugString)
		newTable->debug();

	// Update table schema in memory ----
	int lastUID = -1;
	Field *currentField = 0;
	QMap<QString, QString> fieldMap; // a map from new value to old value
	foreach_list( Field::ListIterator, it, newTable->fieldsIterator() ) {
		fieldMap.insert( it.current()->name(), it.current()->name() );
	}
	for (int i=0; i<allActionsCount; i++) {
		action = actionsVector[i];
		if (!action)
			continue;
		//remember the current Field object because soon we may be unable to find it by name:
		FieldActionBase *fieldAction = dynamic_cast<FieldActionBase*>(action);
		if (!fieldAction) {
			currentField = 0;
		}
		else {
			if (lastUID != fieldAction->uid()) {
				currentField = newTable->field( fieldAction->fieldName() );
				lastUID = currentField ? fieldAction->uid() : -1;
			}
			InsertFieldAction *insertFieldAction = dynamic_cast<InsertFieldAction*>(action);
			if (insertFieldAction && insertFieldAction->index()>(int)newTable->fieldCount()) {
				//update index: there can be empty rows
				insertFieldAction->setIndex(newTable->fieldCount());
			}
		}
		//if (!currentField)
		//	continue;
		args.result = action->updateTableSchema(*newTable, currentField, fieldMap);
		if (args.result!=true) {
			if (recreateTable)
				delete newTable;
			return 0;
		}
	}

	if (recreateTable) {
		// Create the destination table with temporary name
		if (!d->conn->createTable( newTable, false )) {
			setError(d->conn);
			delete newTable;
			args.result = false;
			return 0;
		}
	}

#if 0//todo
	// Execute actions ----
	for (int i=0; i<allActionsCount; i++) {
		action = actionsVector[i];
		if (!action)
			continue;
		args.result = action->execute(*d->conn, *newTable);
		if (!args.result || ~args.result) {
//! @todo delete newTable...
			args.result = false;
			return 0;
		}
	}
#endif

	// update extended table schema after executing the actions
	if (!d->conn->storeExtendedTableSchemaData(*newTable)) {
//! @todo better errmsg?
		setError(d->conn);
//! @todo delete newTable...
		args.result = false;
		return 0;
	}

	if (recreateTable) {
		// Copy the data:
		// Build "INSERT INTO ... SELECT FROM ..." SQL statement
		// The order is based on the order of the source table fields.
		// Notes:
		// -Some source fields can be skipped in case when there are deleted fields.
		// -Some destination fields can be skipped in case when there 
		//  are new empty fields without fixed/default value.
		QString sql = QString("INSERT INTO %1 (").arg(d->conn->escapeIdentifier(newTable->name()));
		//insert list of dest. fields
		bool first = true;
		QString sourceFields;
		foreach_list( Field::ListIterator, it, newTable->fieldsIterator() ) {
			Field * const f = it.current();
			QString renamedFieldName( fieldMap[ f->name() ] );
			QString sourceSQLString;
			if (!renamedFieldName.isEmpty()) {
				//this field should be renamed
				sourceSQLString = d->conn->escapeIdentifier(renamedFieldName);
			}
			else if (!f->defaultValue().isNull()) {
				//this field has a default value defined
//! @todo support expressions (eg. TODAY()) as a default value
//! @todo this field can be notNull or notEmpty - check whether the default is ok 
//!       (or do this checking also in the Table Designer?)
				sourceSQLString = d->conn->driver()->valueToSQL( f->type(), f->defaultValue() );
			}
			else if (f->isNotNull()) {
				//this field cannot be null
				sourceSQLString = d->conn->driver()->valueToSQL( 
					f->type(), KexiDB::emptyValueForType( f->type() ) );
			}
			else if (f->isNotEmpty()) {
				//this field cannot be empty - use any nonempty value..., e.g. " " for text or 0 for number
				sourceSQLString = d->conn->driver()->valueToSQL( 
					f->type(), KexiDB::notEmptyValueForType( f->type() ) );
			}
//! @todo support unique, validatationRule, unsigned flags...
//! @todo check for foreignKey values...

			if (!sourceSQLString.isEmpty()) {
				if (first) {
					first = false;
				}
				else {
					sql.append( ", " );
					sourceFields.append( ", " );
				}
				sql.append( d->conn->escapeIdentifier( f->name() ) );
				sourceFields.append( sourceSQLString );
			}
		}
		sql.append(QString(") SELECT ") + sourceFields + " FROM " + oldTable->name());
		KexiDBDbg << " ** " << sql << endl;
		if (!d->conn->executeSQL( sql )) {
			setError(d->conn);
//! @todo delete newTable...
			args.result = false;
			return 0;
		}

		const QString oldTableName = oldTable->name();
/*		args.result = d->conn->dropTable( oldTable );
		if (!args.result || ~args.result) {
			setError(d->conn);
//! @todo delete newTable...
			return 0;
		}
		oldTable = 0;*/

		// Replace the old table with the new one (oldTable will be destroyed)
		if (!d->conn->alterTableName(*newTable, oldTableName, true /*replace*/)) {
			setError(d->conn);
//! @todo delete newTable...
			args.result = false;
			return 0;
		}
		oldTable = 0;
	}

	if (!recreateTable) {
		if ((MainSchemaAlteringRequired & args.requirements) && !fieldsWithChangedMainSchema.isEmpty()) {
			//update main schema (kexi__fields) for changed fields
			foreach_list(Q3DictIterator<char>, it, fieldsWithChangedMainSchema) {
				Field *f = newTable->field( it.currentKey() );
				if (f) {
					if (!d->conn->storeMainFieldSchema(f)) {
						setError(d->conn);
			//! @todo delete newTable...
						args.result = false;
						return 0;
					}
				}
			}
		}
	}

	args.result = true;
	return newTable;
}

/*TableSchema* AlterTableHandler::execute(const QString& tableName, tristate &result, bool simulate)
{
	return executeInternal( tableName, result, simulate, 0 );
}

tristate AlterTableHandler::simulateExecution(const QString& tableName, QString& debugString)
{
	tristate result;
	(void)executeInternal( tableName, result, true//simulate
	, &debugString );
	return result;
}
*/