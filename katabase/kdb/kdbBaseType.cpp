/***************************************************************************
                          kdbBaseType.cpp  -  description                              
                             -------------------                                         
    begin                : Wed Jun 30 1999                                           
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

#include "kdbBaseType.h"

kdbBaseType::kdbBaseType(kdbBaseType& p_old)
{
	_label = p_old.label();
	_type  = p_old.type();
	_len   = p_old.length();
	_value = p_old.text();
	_descr = p_old.descr();
}

kdbBaseType::kdbBaseType(const QString& p_type)
{
	_label = "";
	_type  = p_type;
	_len   = 0;
	_value = "";
	_descr = "";
}

kdbBaseType::~kdbBaseType()
{
}

const QString&
kdbBaseType::text()
{
	return _value;
}

void
kdbBaseType::setText(const QString& p_val)
{
	_value = p_val;
}

const QString&
kdbBaseType::type()
{
	return _type;
}

void
kdbBaseType::setType(const QString& p_typ)
{
	_type = p_typ;
}

uint
kdbBaseType::length() const
{
	return _len;
}

void
kdbBaseType::setLength(uint p_len)
{
	_len = p_len;
}


