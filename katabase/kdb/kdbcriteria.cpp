/***************************************************************************
                          kdbCriteria.cpp  -  description
                             -------------------                                         
    begin                : Sun Jun 6 1999                                           
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


#include <kdbdataset.h>

#include "kdbcriteria.h"

kdbCriteria::kdbCriteria(kdbCriteria& p_crit)
 : QObject(p_crit.parent(),p_crit.field())
{
	_field     = p_crit.field();
	_relation  = (Rel)p_crit;
	_operation = (Oper)p_crit;
	_value     = p_crit.value();
}

kdbCriteria::kdbCriteria(kdbDataSet *p_set, const char *p_field)
 : QObject(p_set,p_field)
{
	_field     = p_field;
	_relation  = Kdb::Greater;
	_operation = Kdb::And;
	_value     = "";
}

kdbCriteria::~kdbCriteria()
{
}

#include "kdbcriteria.moc"

