/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_UTILS_H
#define KEXI_UTILS_H

#include <qstring.h>
#include <qobjectlist.h>
#include <qvariant.h>
#include <qdatetime.h>

#include <kmessagebox.h>
#include <kaction.h>
#include <klocale.h>

#include "kexi_global.h"

//! displays information that feature "feature_name" is not availabe in the current application version
inline void KEXI_UNFINISHED(QString feature_name) {
	QString msg;
	if (feature_name.isEmpty())
			msg = i18n("This function is not available");
	else 
			msg = i18n("\"%1\" function is not available").arg(feature_name.replace("&","")); 

	KMessageBox::sorry(0, i18n("%1 for version %2 of %3 application.").arg(msg)
		.arg(KEXI_VERSION_STRING).arg(KEXI_APP_NAME) );
}

//! like above - for use inside KExiActionProxy subclass - reuses feature name from shared action's text
#define KEXI_UNFINISHED_SHARED_ACTION(action_name) \
	KEXI_UNFINISHED(sharedAction(action_name) ? sharedAction(action_name)->text() : QString::null)

namespace Kexi
{
	/*! \return true if \a s is a valid identifier, ie. starts with a letter or '_' 
	 and contains only letters, numbers and '_'. */
	inline bool isIdentifier(const QString& s) {
		uint i;
		for (i=0; i<s.length(); i++) {
			QChar c = s.at(i).lower();
			if (!(c=='_' || c>='a' && c<='z' || i>0 && c>='0' && c<='9'))
				break;
		}
		return i>0 && i==s.length();
	}

	//! \return true if \a o has parent \a par.
	inline bool hasParent(QObject* par, QObject* o)
	{
		while (o && o!=par)
			o = o->parent();
		return o==par;
	}

	//! \return parent object of \a o that inherits \a className or NULL if no such parent
	template<class type>
	inline type* findParent(QObject* o, const char* className)
	{
		while ( ((o=o->parent())) && !o->inherits(className) )
			;
		return static_cast<type*>(o);
	}

	//! \return first found child of \a o, that inherit \a className.
	//! Returned pointer type is casted.
	template<class type>
	type* findFirstChild(QObject *o, const char* className)
	{
		QObjectList *l = o->queryList( className );
		QObjectListIt it( *l );
		return static_cast<type*>(it.current());
	}

	//! QDateTime - a hack needed because QVariant(QTime) has broken isNull()
	inline QDateTime stringToHackedQTime(const QString& s)
	{
		if (s.isEmpty())
			return QDateTime();
//		kdDebug() << QDateTime( QDate(0,1,2), QTime::fromString( s, Qt::ISODate ) ).toString(Qt::ISODate) << endl;;
		return QDateTime( QDate(0,1,2), QTime::fromString( s, Qt::ISODate ) );
	}

} //namespace Kexi

#endif


