/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Daniel Molkentin <molkentin@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDBRESULT_H
#define KEXIDBRESULT_H

#include <qobject.h>

class KexiDBField;

class KexiDBResult : public QObject
{
	Q_OBJECT
	
	public:
		/*! you mostly won't construct a field by your self
		 *  even drivers use their own (mostly)
		 */
		KexiDBResult(QObject *parent);
		~KexiDBResult();

		/*! returns the value of "field" */
		virtual QVariant	value(unsigned int field);
		/*! returns the value of "field" */
		virtual QVariant	value(QString);

		/*! returns usefull information about a field/column
		 * @ref KexiDBField. arg: column
		 */
		virtual KexiDBField	*fieldInfo(unsigned int field);
		/*! returns usefull information about a field/column
		 *  @ref KexiDBField. agr: columnName
		 */
		virtual KexiDBField	*fiedlInfo(QString field);

		/*! returns true as long as the table is not at the end
		 *  use it in a while @code while(result->next())
		 */
		virtual bool		next();

		/*! returns the numbers of rows in the result */
		virtual unsigned int	numRows();
};

#endif
