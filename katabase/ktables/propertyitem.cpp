/***************************************************************************
                          propertyitem.cpp  -  description                              
                             -------------------                                         
    begin                : Sun Jul 11 1999                                           
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

#include <klocale.h>

#include <kdb.h>
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>

#include "queryselect.h"
#include "propertyeditor.h"
#include "propertyitem.h"

PropertyItem::PropertyItem(QuerySelect *p_par, const QString& p_s1, const QString& p_s2)
 : QListViewItem(p_par,p_s1,p_s2)
{
	buildProperty( p_s1,p_s2 );
}

PropertyItem::PropertyItem(PropertyItem *p_par, const QString& p_s1, const QString& p_s2)
 : QListViewItem(p_par,p_s1,p_s2)
{
	buildProperty( p_s1,p_s2 );
}

PropertyItem::~PropertyItem()
{
	if ( _ped )
		delete _ped;
}

void
PropertyItem::buildProperty(const QString& p_name,const QString& p_type)
{
	kdbDataSet *ds = 0;
	QStringList sl;
	QString s;
	
	_ped = new PropertyEditor( 0,p_name );
	_ped->hide();
	if ( p_type == i18n("table") ) {
		ds = Kdb::dataSet("pg_class");
		sl = p_name;
		for( uint i=0;i<ds->records();i++ ) {
			s = ds->field("relname").text();
			if ( s != p_name )
				sl += s;
		}
		set( i18n("Name"),sl );
		set( i18n("Label"),QString::null );
		set( i18n("Description"),QString::null );
	} else if ( p_type == i18n("field") ) {
		QListViewItem *p = parent();
		if ( p && p->text(-1) == "PropertyItem" ) {
			ds = Kdb::dataSet( p->text(0) );
			sl.clear();
			if ( ds )
				for( uint i=0;i<ds->fieldSet()->count();i++ ) {
					s = ds->field( i ).name();
					if ( s != p_name )
						sl += s;
				}
		}
		set( i18n("Name"),sl );
		set( i18n("Label"),QString::null );
		set( i18n("Description"),QString::null );
		if ( ds && ds->field( p_name ).type() == "date" ) {
			sl.clear();
			sl << i18n("Locale") << i18n("Database");
			set( i18n("Date format"),sl );
		}
	} else if ( p_type == i18n("join") ) {
		set( i18n("Table"),QString::null );
		set( i18n("Field"),QString::null );
	} else if ( p_type == i18n("criteria") ) {
		set( i18n("Value"),QString::null );
		sl << i18n("Greater than") << i18n("Less than") << i18n("Equal to") << i18n("Like");
		set( i18n("Condition"),sl );
		sl.clear();
		sl << i18n("AND") << i18n("OR") << i18n("AND NOT") << i18n("OR NOT");
		set( i18n("Operator"),sl );
	}
}

void
PropertyItem::set(const QString& p_name, const QString& p_val)
{
	if ( _ped )
		_ped->set(p_name,p_val);
}

void
PropertyItem::set(const QString& p_name, bool p_val)
{
	if ( _ped )
		_ped->set(p_name,p_val);
}

void
PropertyItem::set(const QString& p_name, const QStringList& p_sl)
{
	if ( _ped )
		_ped->set(p_name,p_sl);
}

QString
PropertyItem::getProperty(const QString& p_nam)
{
	if ( _ped )
		return _ped->setting(p_nam);
	return "";
}

QString
PropertyItem::text(int p_idx) const
{
	if ( p_idx == -1 )
		return "PropertyItem";
	return QListViewItem::text( p_idx );
}

void
PropertyItem::edit()
{
	if ( _ped )
		_ped->show();
}






