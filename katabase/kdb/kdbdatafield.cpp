/***************************************************************************
                          kdbDataField.cpp  -  description
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

#include "kdb.h"
#include "kdbpickdate.h"
#include "kdbdatafield.h"

#include "kdbdatafield.moc"

kdbDataField::kdbDataField(kdbDataField& p_field)
 : QObject(p_field.parent(),p_field.name()), kdbBaseType(p_field)
{
}

kdbDataField::kdbDataField(QObject *p_par, const QString& p_name, const QString& p_typ)
 : QObject(p_par,p_name), kdbBaseType(p_typ)
{
}

kdbDataField::~kdbDataField()
{
}

void kdbDataField::set(int p_val)
{
	set( QString("%1").arg(p_val) );
}

void kdbDataField::set(double p_val)
{
	set( QString("%1").arg(p_val) );
}

void
kdbDataField::set(const QString& p_val)
{
	bool verified = true;;
	QString txt = p_val;
	
	emit preChange( verified );
	if ( !verified )
		return;
	emit validateText( verified,txt );
	if ( verified ) {
		kdbBaseType::setText( txt );
		emit postChange( _value );
	}
}

void
kdbDataField::set(const kdbDataField& p_field)
{
	set( p_field.text() );
}

void
kdbDataField::set(const QDate& p_date)
{
	QString txt;
	if ( !p_date.isValid() )
		return;
	txt = kdbPickDate(p_date).format(Kdb::dateFormat());
	set( txt );
}

void
kdbDataField::setText(const QString& p_txt)
{
	bool ok;
	QString txt = p_txt;
	
	if ( !sender() )
		set( txt );
	else {
		emit preChange( ok );
		if ( !ok )
			return;
		emit validateText( ok,txt );
		if ( ok )
			kdbBaseType::setText( txt );
	}
}

void
kdbDataField::setDate(const QDate& p_date)
{
	bool ok = true;
	QString txt;
	
	if ( !p_date.isValid() )
		return;
	txt = kdbPickDate(p_date).format(Kdb::dateFormat());
	if ( !sender() )
		set( txt );
	else {
		preChange( ok );
		if ( ok ) {
			validateText( ok,txt );
			if ( ok )
				kdbBaseType::setText( txt );
		}
	}
}

		















