/***************************************************************************
                          kdbFieldSet.cpp  -  description
                             -------------------                                         
    begin                : Sat Jun 5 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include <iostream.h>

#include <kdb.h>
#include <kdbdataset.h>
#include "kdbfieldset.h"

kdbFieldSet::kdbFieldSet(kdbFieldSet& p_fset)
{
	uint i;
	
	_parent = p_fset.parent();
	for(i=0;i<p_fset.count();i++)
		addField( new kdbDataField(p_fset[i]) );
}

kdbFieldSet::kdbFieldSet(kdbDataSet *p_par)
{
	_parent = p_par;
	_fields.clear();
}

kdbFieldSet::~kdbFieldSet()
{
}

int
kdbFieldSet::locate(const QString& p_name)
{
	kdbDataField *field;

	for(field=_fields.first();field;field=_fields.next())
		if (p_name == field->name())
			break;
	return _fields.at();
}

QString
kdbFieldSet::fieldList(const QChar& p_sep)
{
	kdbDataField *field;
	bool first  = true;
	QString tmp = "";
	
	for( field=_fields.first();field;field=_fields.next() ) {
		if ( !first )
			tmp += p_sep;
		else
			first = false;
		tmp += field->name();
	}
	return tmp;
}

QString
kdbFieldSet::fieldList(const QString& p_table, const QChar& p_sep)
{
	kdbDataField *field;
	bool first  = true;
	QString tmp = "";

	p_table.stripWhiteSpace();
	for( field=_fields.first();field;field=_fields.next() ) {
		if ( !first )
			tmp += p_sep;
		else
			first = false;
		if ( p_table.length() )
			tmp += p_table + p_sep;
		tmp += field->name();
	}
	return tmp;
}

QString
kdbFieldSet::valueList(const QChar& p_sep, const QChar& p_quote)
{
	kdbDataField *field;
	bool first  = true;
	QString tmp = "";
	
	for( field=_fields.first();field;field=_fields.next() ) {
		if ( !first )
			tmp += ',';
		else
			first = false;
		tmp += p_quote + field->text() + p_quote;
	}
	return tmp;
}

uint
kdbFieldSet::fieldNo(const QString& p_name)
{
	uint fno = locate( p_name );
	
	if ( fno < _fields.count() )
		return fno;
	return 0;
}

void
kdbFieldSet::addField(const QString& p_name)
{
	if ( has(p_name) )
		return;
	_fields.append( new kdbDataField(_parent,p_name) );
}

void
kdbFieldSet::addField(kdbDataField *p_df)
{
	if ( !p_df || p_df->name() == "" )
		return;
	removeField( p_df->name() );
	_fields.append( p_df );
}

void
kdbFieldSet::removeField(const QString& p_name)
{
	int i = locate( p_name );

	if ( i >= 0 )
		_fields.remove( i );
}

bool
kdbFieldSet::has(const QString& p_name)
{
	return ( locate(p_name) >= 0 );
}

kdbDataField&
kdbFieldSet::field(const QString& p_name)
{
	int i = locate( p_name );

	if (i < 0)
		throw Kdb::NoField;
	return *_fields.at(i);
}

kdbDataField&
kdbFieldSet::field(uint p_idx)
{
	if (p_idx >= _fields.count())
		throw Kdb::NoField;
	return *_fields.at(p_idx);
}

























