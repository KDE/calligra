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
#include <qdom.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qsplitter.h>
#include <qmetaobject.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <koproperty/property.h>
#include <kexi_global.h>

#include "kexitabledesignercommands.h"

using namespace KexiTableDesignerCommands;


Command::Command(KexiTableDesignerView* view)
 : K3Command()
 , m_view(view)
{
}

Command::~Command()
{
}

//--------------------------------------------------------

ChangeFieldPropertyCommand::ChangeFieldPropertyCommand( 
	KexiTableDesignerView* view,
	const KoProperty::Set& set, const Q3CString& propertyName, 
	const QVariant& oldValue, const QVariant& newValue,
	KoProperty::Property::ListData* const oldListData, 
	KoProperty::Property::ListData* const newListData)
 : Command(view)
 , m_alterTableAction(
		propertyName=="name" ? oldValue.toString() : set.property("name").value().toString(), 
		propertyName, newValue, set["uid"].value().toInt())
 , m_oldValue(oldValue)
// , m_fieldUID(set["uid"].value().toInt())
 , m_oldListData( oldListData ? new KoProperty::Property::ListData(*oldListData) : 0 )
 , m_listData( newListData ? new KoProperty::Property::ListData(*newListData) : 0 )
{
	kexipluginsdbg << "ChangeFieldPropertyCommand: " << debugString() << endl;
}

ChangeFieldPropertyCommand::~ChangeFieldPropertyCommand()
{
	delete m_oldListData;
	delete m_listData;
}

QString ChangeFieldPropertyCommand::name() const
{
	return i18n(
		"Change \"%1\" property for table field from \"%2\" to \"%3\"",
		m_alterTableAction.propertyName(),
		m_oldValue.toString(),
		m_alterTableAction.newValue().toString());
}

QString ChangeFieldPropertyCommand::debugString()
{
	QString s( name() );
	if (m_oldListData || m_listData)
		s += QString("\nAnd list data from [%1]\n  to [%2]")
			.arg( m_oldListData ? 
				QString("%1 -> %2")
				.arg(m_oldListData->keysAsStringList().join(",")).arg(m_oldListData->names.join(","))
				: QString("<NONE>"))
			.arg( m_listData ?
				QString("%1 -> %2")
				.arg(m_listData->keysAsStringList().join(",")).arg(m_listData->names.join(","))
				: QString("<NONE>"));
	return s + QString(" (UID=%1)").arg(m_alterTableAction.uid());
}

void ChangeFieldPropertyCommand::execute()
{
	m_view->changeFieldProperty( 
		m_alterTableAction.uid(),
		m_alterTableAction.propertyName().toLatin1(),
		m_alterTableAction.newValue(), m_listData );
}

void ChangeFieldPropertyCommand::unexecute()
{
	m_view->changeFieldProperty( 
		m_alterTableAction.uid(),
		m_alterTableAction.propertyName().toLatin1(),
		m_oldValue, m_oldListData );
}

KexiDB::AlterTableHandler::ActionBase* ChangeFieldPropertyCommand::createAction()
{
	if (m_alterTableAction.propertyName()=="subType") {//skip these properties
		return 0;
	}
	return new KexiDB::AlterTableHandler::ChangeFieldPropertyAction( m_alterTableAction );
}

//--------------------------------------------------------

RemoveFieldCommand::RemoveFieldCommand( KexiTableDesignerView* view, int fieldIndex, 
	const KoProperty::Set* set)
 : Command(view)
 , m_alterTableAction( set ? (*set)["name"].value().toString() : QString(), 
	set ? (*set)["uid"].value().toInt() : -1 )
 , m_set( set ? new KoProperty::Set(*set /*deep copy*/) : 0 )
 , m_fieldIndex(fieldIndex)
{
}

RemoveFieldCommand::~RemoveFieldCommand()
{
	delete m_set;
}

QString RemoveFieldCommand::name() const
{
	if (m_set)
		return i18n("Remove table field \"%1\"", m_alterTableAction.fieldName());

	return QString("Remove empty row at position %1").arg(m_fieldIndex);
}

void RemoveFieldCommand::execute()
{
//	m_view->deleteField( m_fieldIndex );
	m_view->deleteRow( m_fieldIndex );
}

void RemoveFieldCommand::unexecute()
{
	m_view->insertEmptyRow(m_fieldIndex);
	if (m_set)
		m_view->insertField( m_fieldIndex, *m_set );
}

QString RemoveFieldCommand::debugString()
{
	if (!m_set)
		return name();

	return name() + "\nAT ROW " + QString::number(m_fieldIndex)
		+ ", FIELD: " + (*m_set)["caption"].value().toString()
		+ QString(" (UID=%1)").arg(m_alterTableAction.uid());
}

KexiDB::AlterTableHandler::ActionBase* RemoveFieldCommand::createAction()
{
	return new KexiDB::AlterTableHandler::RemoveFieldAction( m_alterTableAction );
}

//--------------------------------------------------------

InsertFieldCommand::InsertFieldCommand( KexiTableDesignerView* view,
 int fieldIndex/*, const KexiDB::Field& field*/, const KoProperty::Set& set )
 : Command(view)
 , m_alterTableAction(0) //fieldIndex, new KexiDB::Field(field) /*deep copy*/)
 , m_set( set ) //? new KoProperty::Set(*set) : 0 )
{
	KexiDB::Field *f = view->buildField( m_set );
	if (f)
		m_alterTableAction = new KexiDB::AlterTableHandler::InsertFieldAction(
			fieldIndex, f, set["uid"].value().toInt());
	else //null action
		m_alterTableAction = new KexiDB::AlterTableHandler::InsertFieldAction(true);
}

InsertFieldCommand::~InsertFieldCommand()
{
	delete m_alterTableAction;
}

QString InsertFieldCommand::name() const
{
	return i18n("Insert table field \"%1\"", m_set["caption"].value().toString());
}

void InsertFieldCommand::execute()
{
	m_view->insertField( m_alterTableAction->index(), /*m_alterTableAction.field(),*/ m_set );
}

void InsertFieldCommand::unexecute()
{
	m_view->clearRow( m_alterTableAction->index() );//m_alterTableAction.index() );
}

KexiDB::AlterTableHandler::ActionBase* InsertFieldCommand::createAction()
{
	return new KexiDB::AlterTableHandler::InsertFieldAction(*m_alterTableAction);
}

//--------------------------------------------------------

ChangePropertyVisibilityCommand::ChangePropertyVisibilityCommand( KexiTableDesignerView* view,
	const KoProperty::Set& set, const Q3CString& propertyName, bool visible)
 : Command(view)
 , m_alterTableAction(set.property("name").value().toString(), propertyName, visible, set["uid"].value().toInt())
// , m_fieldUID(set["uid"].value().toInt())
 , m_oldVisibility( set.property(propertyName).isVisible() )
{
	kexipluginsdbg << "ChangePropertyVisibilityCommand: " << debugString() << endl;
}

ChangePropertyVisibilityCommand::~ChangePropertyVisibilityCommand()
{
}

QString ChangePropertyVisibilityCommand::name() const
{
	return QString("[internal] Change \"%1\" visibility from \"%2\" to \"%3\"")
		.arg(m_alterTableAction.propertyName())
		.arg(m_oldVisibility ? "true" : "false")
		.arg(m_alterTableAction.newValue().toBool() ? "true" : "false");
}

void ChangePropertyVisibilityCommand::execute()
{
	m_view->changePropertyVisibility( 
		m_alterTableAction.uid(),
		m_alterTableAction.propertyName().toLatin1(),
		m_alterTableAction.newValue().toBool() );
}

void ChangePropertyVisibilityCommand::unexecute()
{
	m_view->changePropertyVisibility( 
		m_alterTableAction.uid(),
		m_alterTableAction.propertyName().toLatin1(),
		m_oldVisibility );
}

//--------------------------------------------------------

InsertEmptyRowCommand::InsertEmptyRowCommand( KexiTableDesignerView* view, int row )
 : Command(view)
 , m_alterTableAction(true) //unused, null action
 , m_row(row)
{
}

InsertEmptyRowCommand::~InsertEmptyRowCommand()
{
}

QString InsertEmptyRowCommand::name() const
{
	return QString("Insert empty row at position %1").arg(m_row);
}

void InsertEmptyRowCommand::execute()
{
	m_view->insertEmptyRow( m_row );
}

void InsertEmptyRowCommand::unexecute()
{
	// let's assume the row is empty...
	m_view->deleteRow( m_row );
}

