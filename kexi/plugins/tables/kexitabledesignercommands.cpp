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

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kaccelmanager.h>

#include <koproperty/property.h>

#include "kexitabledesignercommands.h"

using namespace KexiTableDesignerCommands;


Command::Command(KexiAlterTableDialog* view)
 : KCommand()
 , m_view(view)
{
}

Command::~Command()
{
}

//--------------------------------------------------------

ChangeFieldPropertyCommand::ChangeFieldPropertyCommand( KexiAlterTableDialog* view,
	const KoProperty::Set& set, const QCString& propertyName, const QVariant& oldValue, const QVariant& newValue,
	KoProperty::Property::ListData* const oldListData, KoProperty::Property::ListData* const newListData)
 : Command(view)
 , m_alterTableAction(set.property("name").value().toString(), propertyName, newValue)
 , m_oldValue(oldValue)
 , m_fieldUID(set["uid"].value().toInt())
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
	return i18n("Change \"%1\" property for table field from \"%2\" to \"%3\"")
		.arg(m_alterTableAction.propertyName()).arg(m_oldValue.toString())
		.arg(m_alterTableAction.newValue().toString());
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
	return s;
}

void ChangeFieldPropertyCommand::execute()
{
	m_view->changeFieldProperty( 
		m_fieldUID,
		m_alterTableAction.propertyName().latin1(),
		m_alterTableAction.newValue(), m_listData );
}

void ChangeFieldPropertyCommand::unexecute()
{
	m_view->changeFieldProperty( 
		m_fieldUID,
		m_alterTableAction.propertyName().latin1(),
		m_oldValue, m_oldListData );
}

//--------------------------------------------------------

RemoveFieldCommand::RemoveFieldCommand( KexiAlterTableDialog* view, int fieldIndex, 
	const KoProperty::Set& set)//, const KexiDB::Field& field )
 : Command(view)
 , m_alterTableAction(set["name"].value().toString())
 , m_set(set /*deep copy*/)
// , m_field(field /*deep copy*/)
 , m_fieldIndex(fieldIndex)
{
}

RemoveFieldCommand::~RemoveFieldCommand()
{
}

QString RemoveFieldCommand::name() const
{
	return i18n("Remove table field \"%1\"")
		.arg(m_alterTableAction.fieldName());
}

void RemoveFieldCommand::execute()
{
	m_view->deleteField( m_fieldIndex );
}

void RemoveFieldCommand::unexecute()
{
	m_view->insertEmptyRow(m_fieldIndex);
	m_view->insertField( m_fieldIndex, /*m_field,*/ m_set );
}

//--------------------------------------------------------

InsertFieldCommand::InsertFieldCommand( KexiAlterTableDialog* view,
 int fieldIndex/*, const KexiDB::Field& field*/, const KoProperty::Set& set )
 : Command(view)
 , m_fieldIndex(fieldIndex)
 , m_alterTableAction(0) //fieldIndex, new KexiDB::Field(field) /*deep copy*/)
 , m_set( set ) //? new KoProperty::Set(*set) : 0 )
{
}

InsertFieldCommand::~InsertFieldCommand()
{
	delete m_alterTableAction;
}

QString InsertFieldCommand::name() const
{
	return i18n("Insert table field \"%1\"").arg(m_set["caption"].value().toString());//m_alterTableAction.field().name());
}

void InsertFieldCommand::execute()
{
	m_view->insertField( m_fieldIndex, /*m_alterTableAction.field(),*/ m_set );
}

void InsertFieldCommand::unexecute()
{
	m_view->clearRow( m_fieldIndex );//m_alterTableAction.index() );
}

const KexiDB::AlterTableHandler::ActionBase& InsertFieldCommand::action()
{
	//todo
	return *m_alterTableAction;
}

//--------------------------------------------------------

ChangePropertyVisibilityCommand::ChangePropertyVisibilityCommand( KexiAlterTableDialog* view,
	const KoProperty::Set& set, const QCString& propertyName, bool visible)
 : Command(view)
 , m_alterTableAction(set.property("name").value().toString(), propertyName, visible)
 , m_fieldUID(set["uid"].value().toInt())
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
		m_fieldUID,
		m_alterTableAction.propertyName().latin1(),
		m_alterTableAction.newValue().toBool() );
}

void ChangePropertyVisibilityCommand::unexecute()
{
	m_view->changePropertyVisibility( 
		m_fieldUID,
		m_alterTableAction.propertyName().latin1(),
		m_oldVisibility );
}

