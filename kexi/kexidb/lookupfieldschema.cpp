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
//Added by qt3to4:
#include <Q3ValueList>
#include <kdebug.h>

using namespace KexiDB;


LookupFieldSchema::RowSource::RowSource()
: m_type(NoType)
, m_values(0)
{
}

LookupFieldSchema::RowSource::~RowSource()
{
	delete m_values;
}

void LookupFieldSchema::RowSource::setName(const QString& name)
{
	m_name = name;
	if (m_values)
		m_values->clear();
}

QString LookupFieldSchema::RowSource::typeName() const
{
	switch (m_type) {
	case Table: return "table";
	case Query: return "query";
	case SQLStatement: return "sql";
	case ValueList: return "valuelist";
	case FieldList: return "fieldlist";
	default:;
	}
	return QString();
}

void LookupFieldSchema::RowSource::setTypeByName( const QString& typeName )
{
	if (typeName=="table")
		setType( Table );
	else if (typeName=="query")
		setType( Query );
	else if (typeName=="sql")
		setType( SQLStatement );
	else if (typeName=="valuelist")
		setType( ValueList );
	else if (typeName=="fieldlist")
		setType( FieldList );
	else
		setType( NoType );
}

QStringList LookupFieldSchema::RowSource::values() const
{
	return m_values ? *m_values : QStringList();
}

void LookupFieldSchema::RowSource::setValues(const QStringList& values)
{
	m_name.clear();
	if (m_values)
		*m_values = values;
	else
		m_values = new QStringList(values);
}

QString LookupFieldSchema::RowSource::debugString() const
{
	return QString("rowSourceType:'%1' rowSourceName:'%2' rowSourceValues:'%3'\n")
		.arg(typeName()).arg(name()).arg(m_values ? m_values->join("|") : QString::null);
}

void LookupFieldSchema::RowSource::debug() const
{
	KexiDBDbg << debugString() << endl;
}

//---------------------------------------

LookupFieldSchema::LookupFieldSchema()
 : m_boundColumn(-1)
 , m_visibleColumn(-1)
 , m_maximumListRows(KEXIDB_LOOKUP_FIELD_DEFAULT_LIST_ROWS)
 , m_displayWidget(KEXIDB_LOOKUP_FIELD_DEFAULT_DISPLAY_WIDGET)
 , m_columnHeadersVisible(KEXIDB_LOOKUP_FIELD_DEFAULT_HEADERS_VISIBLE)
 , m_limitToList(KEXIDB_LOOKUP_FIELD_DEFAULT_LIMIT_TO_LIST)
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
	QString columnWidthsStr;
	bool first=true;
	for (Q3ValueList<int>::ConstIterator it=m_columnWidths.constBegin();
		it!=m_columnWidths.constEnd();++it)
	{
		if (first)
			first = false;
		else
			columnWidthsStr.append(";");
		columnWidthsStr.append( QString::number(*it) );
	}

	return QString("LookupFieldSchema( %1\n"
		" boundColumn:%2 visibleColumn:%3 maximumListRows:%4 displayWidget:%5\n"
		" columnHeadersVisible:%6 limitToList:%7\n"
		" columnWidths:%8 )")
		.arg(m_rowSource.debugString())
		.arg(m_boundColumn).arg(m_visibleColumn).arg(m_maximumListRows)
		.arg( m_displayWidget==ComboBox ? "ComboBox" : "ListBox")
		.arg(m_columnHeadersVisible).arg(m_limitToList)
		.arg(columnWidthsStr);
}

void LookupFieldSchema::debug() const
{
	KexiDBDbg << debugString() << endl;
}

/* static */
LookupFieldSchema *LookupFieldSchema::loadFromDom(const QDomElement& lookupEl)
{
	LookupFieldSchema *lookupFieldSchema = new LookupFieldSchema();
	for (QDomNode node = lookupEl.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement el = node.toElement();
		QString name( el.tagName() );
		if (name=="row-source") {
			/*<row-source>
				empty
				| <type>table|query|sql|valuelist|fieldlist</type>  #required because there can be table and query with the same name
									"fieldlist" (basically a list of column names of a table/query,
											  "Field List" as in MSA)
				<name>string</name> #table/query name, etc. or KEXISQL SELECT QUERY
				<values><value>...</value> #for "valuelist" type
					<value>...</value>
				</values>
			 </row-source> */
			for (el = el.firstChild().toElement(); !el.isNull(); el=el.nextSibling().toElement()) {
				if (el.tagName()=="type")
					lookupFieldSchema->rowSource().setTypeByName( el.text() );
				else if (el.tagName()=="name")
					lookupFieldSchema->rowSource().setName( el.text() );
//! @todo handle fieldlist (retrieve from external table or so?), use lookupFieldSchema.rowSource().setValues()
			}
		}
		else if (name=="bound-column") {
			/* <bound-column>
			    <number>number</number> #in later implementation there can be more columns
			   </bound-column> */
			QVariant val = KexiDB::loadPropertyValueFromDom( el.firstChild() );
			if (val.type()==QVariant::Int)
				lookupFieldSchema->setBoundColumn( val.toInt() );
		}
		else if (name=="visible-column") {
			/* <visible-column> #a column that has to be visible in the combo box
				<number>number</number> #in later implementation there can be more columns
			   </visible-column> */
			QVariant val = KexiDB::loadPropertyValueFromDom( el.firstChild() );
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
			Q3ValueList<int> columnWidths;
			for (el = el.firstChild().toElement(); !el.isNull(); el=el.nextSibling().toElement()) {
				QVariant val = KexiDB::loadPropertyValueFromDom( el );
				if (val.type()==QVariant::Int)
					columnWidths.append(val.toInt());
			}
			lookupFieldSchema->setColumnWidths( columnWidths );
		}
		else if (name=="show-column-headers") {
			/* <show-column-headers>
			    <bool>true/false</bool>
			   </show-column-headers> */
			QVariant val = KexiDB::loadPropertyValueFromDom( el.firstChild() );
			if (val.type()==QVariant::Bool)
				lookupFieldSchema->setColumnHeadersVisible( val.toBool() );
		}
		else if (name=="list-rows") {
			/* <list-rows>
			    <number>1..100</number>
			   </list-rows> */
			QVariant val = KexiDB::loadPropertyValueFromDom( el.firstChild() );
			if (val.type()==QVariant::Int)
				lookupFieldSchema->setMaximumListRows( val.toUInt() );
		}
		else if (name=="limit-to-list") {
			/* <limit-to-list>
			    <bool>true/false</bool>
			   </limit-to-list> */
			QVariant val = KexiDB::loadPropertyValueFromDom( el.firstChild() );
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
void LookupFieldSchema::saveToDom(LookupFieldSchema& lookupSchema, QDomDocument& doc, QDomElement& parentEl)
{
	QDomElement lookupColumnEl( doc.createElement("lookup-column") );
	parentEl.appendChild( lookupColumnEl );

	QDomElement rowSourceEl( doc.createElement("row-source") );
	lookupColumnEl.appendChild( rowSourceEl );

	QDomElement rowSourceTypeEl( doc.createElement("type") );
	rowSourceEl.appendChild( rowSourceTypeEl );
	rowSourceTypeEl.appendChild( doc.createTextNode(lookupSchema.rowSource().typeName()) ); //can be empty

	QDomElement nameEl( doc.createElement("name") );
	rowSourceEl.appendChild( nameEl );
	nameEl.appendChild( doc.createTextNode(lookupSchema.rowSource().name()) ); //can be empty

	const QStringList& values( lookupSchema.rowSource().values() );
	if (!values.isEmpty()) {
		QDomElement valuesEl( doc.createElement("values") );
		rowSourceEl.appendChild( valuesEl );
		for (QStringList::ConstIterator it = values.constBegin(); it!=values.constEnd(); ++it) {
			QDomElement valueEl( doc.createElement("value") );
			valuesEl.appendChild( valueEl );
			valueEl.appendChild( doc.createTextNode(*it) );
		}
	}

	if (lookupSchema.boundColumn()>=0)
		KexiDB::saveNumberElementToDom(doc, lookupColumnEl, "bound-column", lookupSchema.boundColumn());
	if (lookupSchema.visibleColumn()>=0)
		KexiDB::saveNumberElementToDom(doc, lookupColumnEl, "visible-column", lookupSchema.visibleColumn()); //can be -1

	const Q3ValueList<int> columnWidths = lookupSchema.columnWidths();
	if (!columnWidths.isEmpty()) {
		QDomElement columnWidthsEl( doc.createElement("column-widths") );
		lookupColumnEl.appendChild( columnWidthsEl );
		for (Q3ValueList<int>::ConstIterator it = columnWidths.constBegin(); it!=columnWidths.constEnd(); ++it) {
			QDomElement columnWidthEl( doc.createElement("number") );
			columnWidthsEl.appendChild( columnWidthEl );
			columnWidthEl.appendChild( doc.createTextNode( QString::number(*it) ) );
		}
	}

	if (lookupSchema.columnHeadersVisible()!=KEXIDB_LOOKUP_FIELD_DEFAULT_HEADERS_VISIBLE)
		KexiDB::saveBooleanElementToDom(doc, lookupColumnEl, "show-column-headers", lookupSchema.columnHeadersVisible());
	if (lookupSchema.maximumListRows()!=KEXIDB_LOOKUP_FIELD_DEFAULT_LIST_ROWS)
		KexiDB::saveNumberElementToDom(doc, lookupColumnEl, "list-rows", lookupSchema.maximumListRows());
	if (lookupSchema.limitToList()!=KEXIDB_LOOKUP_FIELD_DEFAULT_LIMIT_TO_LIST)
		KexiDB::saveBooleanElementToDom(doc, lookupColumnEl, "limit-to-list", lookupSchema.limitToList());
	
	if (lookupSchema.displayWidget()!=KEXIDB_LOOKUP_FIELD_DEFAULT_DISPLAY_WIDGET) {
		QDomElement displayWidgetEl( doc.createElement("display-widget") );
		lookupColumnEl.appendChild( displayWidgetEl );
		displayWidgetEl.appendChild( 
			doc.createTextNode( (lookupSchema.displayWidget()==ListBox) ? "listbox" : "combobox" ) );
	}
}
