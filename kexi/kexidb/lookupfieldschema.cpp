/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "lookupfieldschema.h"
#include "utils.h"

#include <qdom.h>
#include <qvariant.h>


using namespace KexiDB;

LookupFieldSchema::LookupFieldSchema()
 : m_rowSourceType(NoType)
 , m_boundColumn(-1)
 , m_visibleColumn(-1)
 , m_maximumListRows(KEXIDB_LOOKUP_FIELD_DEFAULT_LIST_ROWS)
 , m_displayWidget(ComboBox)
 , m_columnHeadersVisible(false)
 , m_limitToList(true)
{
}

LookupFieldSchema::~LookupFieldSchema()
{
}

void LookupFieldSchema::setMaximumListRows(uint rows)
{
	if (rows==0)
		m_maximumListRows = KEXIDB_LOOKUP_FIELD_DEFAULT_LIST_ROWS;
	else if (rows>KEXIDB_LOOKUP_FIELD_MAX_LIST_ROWS)
		m_maximumListRows = KEXIDB_LOOKUP_FIELD_MAX_LIST_ROWS;
	else
		m_maximumListRows = rows;
}

QString LookupFieldSchema::debugString() const
{
	QString rowSourceTypeStr;
	switch (m_rowSourceType) {
	case NoType: rowSourceTypeStr="NoType"; break;
	case Table: rowSourceTypeStr="Table"; break;
	case Query: rowSourceTypeStr="Query"; break;
	case SQLStatement: rowSourceTypeStr="SQLStatement"; break;
	case ValueList: rowSourceTypeStr="ValueList"; break;
	case FieldList: rowSourceTypeStr="FieldList"; break;
	}
	QString columnWidthsStr;
	bool first=true;
	foreach (QValueList<int>::ConstIterator, it, m_columnWidths) {
		if (first)
			first = false;
		else
			columnWidthsStr.append(";");
		columnWidthsStr.append( QString::number(*it) );
	}

	return QString("LookupFieldSchema( rowSourceType:%1\n rowSource:%2\n rowSourceValues:%3\n"
		" boundColumn:%4 visibleColumn:%5 maximumListRows:%6 displayWidget:%7\n"
		" columnHeadersVisible:%8 limitToList:%9\n"
		" columnWidths:%10 )")
		.arg(rowSourceTypeStr).arg(m_rowSource).arg(m_rowSourceValues.join("|"))
		.arg(m_boundColumn).arg(m_visibleColumn).arg(m_maximumListRows)
		.arg( m_displayWidget==ComboBox ? "ComboBox" : "ListBox")
		.arg(m_columnHeadersVisible).arg(m_limitToList)
		.arg(columnWidthsStr);
}

/* static */
LookupFieldSchema *LookupFieldSchema::loadFromXML(const QDomElement& lookupEl)
{
	LookupFieldSchema *lookupFieldSchema = new LookupFieldSchema();
	for (QDomNode node = lookupEl.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement el = node.toElement();
		QString name( el.tagName() );
		if (name=="row-source") {
			/*<row-source>
			   empty
			   | <type>table|query|sql</type>  #required because there can be table and query with the same name
									  #TODO1: support "valuelist"
									  #TODO2: support "fieldlist" (basically a list of column names of a table/query,
											  "Field List" as in MSA)
			   |<name>string</name> #table/query name, etc. or KEXISQL SELECT QUERY
			 </row-source> */
			
			for (el = el.firstChild().toElement(); !el.isNull(); el=el.nextSibling().toElement()) {
				if (el.tagName()=="type") {
					if (el.text()=="table")
						lookupFieldSchema->setRowSourceType( LookupFieldSchema::Table );
					else if (el.text()=="query")
						lookupFieldSchema->setRowSourceType( LookupFieldSchema::Query );
					else if (el.text()=="sql")
						lookupFieldSchema->setRowSourceType( LookupFieldSchema::SQLStatement );
					else if (el.text()=="valuelist")
						lookupFieldSchema->setRowSourceType( LookupFieldSchema::ValueList );
					else if (el.text()=="fieldlist")
						lookupFieldSchema->setRowSourceType( LookupFieldSchema::FieldList );
					else 
						lookupFieldSchema->setRowSourceType( LookupFieldSchema::NoType );
				}
				else if (el.tagName()=="name") {
					lookupFieldSchema->setRowSource( el.text() );
				}
//! @todo handle fieldlist (retrieve from external table or so?), use lookupFieldSchema.setRowSourceValues()
			}
		}
		else if (name=="bound-column") {
			/* <bound-column>
			    <number>number</number> #in later implementation there can be more columns
			   </bound-column> */
			QVariant val = KexiDB::loadPropertyValueFromXML( el.firstChild() );
			if (val.type()==QVariant::Int)
				lookupFieldSchema->setBoundColumn( val.toInt() );
		}
		else if (name=="visible-column") {
			/* <visible-column> #a column that has to be visible in the combo box
				<number>number</number> #in later implementation there can be more columns
			   </visible-column> */
			QVariant val = KexiDB::loadPropertyValueFromXML( el.firstChild() );
			if (val.type()==QVariant::Int)
				lookupFieldSchema->setVisibleColumn( val.toInt() );
		}
		else if (name=="column-widths") {
			/* <column-widths> #column widths, -1 means 'default'
			    <number>int</number>
			    ...
			    <number>int</number>
			   </column-widths> */
			QVariant val;
			QValueList<int> columnWidths;
			for (el = el.firstChild().toElement(); !el.isNull(); el=el.nextSibling().toElement()) {
				QVariant val = KexiDB::loadPropertyValueFromXML( el );
				if (val.type()==QVariant::Int)
					columnWidths.append(val.toInt());
			}
			lookupFieldSchema->setColumnWidths( columnWidths );
		}
		else if (name=="show-column-headers") {
			/* <show-column-headers>
			    <bool>true/false</bool>
			   </show-column-headers> */
			QVariant val = KexiDB::loadPropertyValueFromXML( el.firstChild() );
			if (val.type()==QVariant::Bool)
				lookupFieldSchema->setColumnHeadersVisible( val.toBool() );
		}
		else if (name=="list-rows") {
			/* <list-rows>
			    <number>1..100</number>
			   </list-rows> */
			QVariant val = KexiDB::loadPropertyValueFromXML( el.firstChild() );
			if (val.type()==QVariant::Int)
				lookupFieldSchema->setMaximumListRows( val.toUInt() );
		}
		else if (name=="limit-to-list") {
			/* <limit-to-list>
			    <bool>true/false</bool>
			   </limit-to-list> */
			QVariant val = KexiDB::loadPropertyValueFromXML( el.firstChild() );
			if (val.type()==QVariant::Bool)
				lookupFieldSchema->setLimitToList( val.toBool() );
		}
		else if (name=="display-widget") {
			if (el.text()=="combobox")
				lookupFieldSchema->setDisplayWidget( LookupFieldSchema::ComboBox );
			else if (el.text()=="listbox")
				lookupFieldSchema->setDisplayWidget( LookupFieldSchema::ListBox );
		}
	}
	return lookupFieldSchema;
}

/* static */
void LookupFieldSchema::saveToXML(LookupFieldSchema& lookupSchema, QDomElement& parentEl)
{
	Q_UNUSED(lookupSchema);
	Q_UNUSED(parentEl);
	//todo
}
