/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDB_DRIVER_P_H
#define KEXIDB_DRIVER_P_H

#ifndef __KEXIDB__
# error "Do not include: this is KexiDB internal file"
#endif

#include <qstring.h>

namespace KexiDB {

/*! Detailed definition of driver's default behaviour.
 Note for driver developers: 
 Change these defaults in you Driver subclass
 constructor, if needed.
*/
class DriverBehaviour
{
	public:
		DriverBehaviour();
			
	//! "UNSIGNED" by default
	QString UNSIGNED_TYPE_KEYWORD;
	//! "AUTO_INCREMENT" by default, used as add-in word to field definition
	//! May be also used as full definition if SPECIAL_AUTO_INCREMENT_DEF is true.
	QString AUTO_INCREMENT_FIELD_OPTION; 
	/*! True if autoincrement field need has special definition 
	 e.g. like "INTEGER PRIMARY KEY" for SQLite.
	 Special definition string should be stored in AUTO_INCREMENT_FIELD_OPTION.
	 False by default. */
	bool SPECIAL_AUTO_INCREMENT_DEF : 1;
};

}

#endif
