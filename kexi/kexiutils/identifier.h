/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#ifndef __IDENTIFIER_H
#define __IDENTIFIER_H

#include <qstring.h>

namespace KexiUtils {
  //! @todo  Do these need to be KDE_EXPORT'ed?

	//! \return Valid filename based on \a s
	QString string2FileName(const QString &s);
	
	/*! Always returns valid identifier based on \a s.
	 Non alphanumeric chars (or spaces) are replaced with '_'.
	 If a number char is at the beginning, '_' is added at start.
	 Empty strings are not changed.
	*/
	QString string2Identifier(const QString &s);

}

#endif
