/***************************************************************************
                          database.cpp  -  description                              
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


#include "kdbdatabase.h"

kdbDataBase::kdbDataBase(const char *p_base)
 : QObject( 0,p_base )
{
}

kdbDataBase::~kdbDataBase()
{
}

#include "kdbdatabase.moc"

