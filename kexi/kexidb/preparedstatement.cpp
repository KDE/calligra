/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "preparedstatement.h"

#include <kexidb/connection.h>
#include <kexidb/connection_p.h>
#include <kdebug.h>
//Added by qt3to4:
#include <Q3CString>

using namespace KexiDB;

PreparedStatement::PreparedStatement(StatementType type, ConnectionInternal& conn, 
	FieldList& fields, const QStringList& where)
 : KShared()
 , m_type(type)
 , m_fields(&fields)
 , m_where(where.isEmpty() ? new QStringList(where) : 0)
 , m_whereFields(0)
{
	Q_UNUSED(conn);
}

PreparedStatement::~PreparedStatement()
{
	delete m_where;
	delete m_whereFields;
}

Q3CString PreparedStatement::generateStatementString()
{
	Q3CString s(1024);
	if (m_type == SelectStatement) {
//! @todo only tables and trivial queries supported for select...
		s = "SELECT ";
		bool first = true;
//		for (uint i=0; i<m_fields->fieldCount(); i++) {
		for (Field::ListIterator it(m_fields->fieldsIterator()); it.current(); ++it) {
			if (first)
				first = false;
			else
				s.append(", ");
			s.append(it.current()->name().latin1());
		}
		first = true;
		s.append(" WHERE ");
//		for (uint i=0; i<m_fields->fieldCount(); i++) {

		m_whereFields = new Field::List();
		for (QStringList::ConstIterator it=m_where->constBegin(); it!=m_where->constEnd(); ++it) {
//		for (Field::ListIterator it(m_fields->fieldsIterator()); it.current(); ++it) {
			if (first)
				first = false;
			else
				s.append(" AND ");
			Field *f = m_fields->field(*it);
			if (!f) {
				KexiDBWarn << "PreparedStatement::generateStatementString(): no '" 
					<< *it << "' field found" << endl;
				continue;
			}
			m_whereFields->append(f);
			s.append((*it).latin1());
			s.append("=?");
		}
	}
	else if (m_type == InsertStatement /*&& dynamic_cast<TableSchema*>(m_fields)*/) {
  //! @todo only tables supported for insert; what about views?
		
		TableSchema *table = m_fields->fieldCount()>0 ? m_fields->field(0)->table() : 0;
		if (!table)
			return ""; //err

		QCString namesList;
		bool first = true;
		const bool allTableFieldsUsed = dynamic_cast<TableSchema*>(m_fields); //we are using a selection of fields only
//		for (Field::ListIterator it(m_fields->fieldsIterator()); it.current(); ++it) {
		Field::ListIterator it = m_fields->fieldsIterator();
		for (uint i=0; i<m_fields->fieldCount(); i++, ++it) {
			if (first) {
				s.append( "?" );
				if (!allTableFieldsUsed)
					namesList = it.current()->name().latin1();
				first = false;
			} else {
				s.append( ",?" );
				if (!allTableFieldsUsed)
					namesList.append(QCString(", ")+it.current()->name().latin1());
			}
		}
		s.append(")");
		s.prepend(QCString("INSERT INTO ")
			+ (allTableFieldsUsed ? "" : table->name().latin1()) 
			+ " (" + namesList + ") VALUES (");
	}
	return s;
}

PreparedStatement& PreparedStatement::operator<< ( const QVariant& value )
{
	m_args.append(value);
	return *this;
}

/*bool PreparedStatement::insert()
{
	const bool res = m_conn->drv_prepareStatement(this);
	const bool res = m_conn->drv_insertRecord(this);
	clearArguments();
	return res;
}

bool PreparedStatement::select()
{
	const bool res = m_conn->drv_bindArgumentForPreparedStatement(this, m_args.count()-1);
}*/

void PreparedStatement::clearArguments()
{
	m_args.clear();
}

