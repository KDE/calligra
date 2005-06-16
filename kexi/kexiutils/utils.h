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

#ifndef __KEXIUTILS_UTILS_H__
#define __KEXIUTILS_UTILS_H__

#include "kexiutils_export.h"

#include <qguardedptr.h>
#include <qobjectlist.h>
#include <kmimetype.h>
class QColor;

// General Utils

namespace KexiUtils
{
	//! \return true if \a o has parent \a par.
	inline bool hasParent(QObject* par, QObject* o)
	{
		if (!o || !par)
			return false;
		while (o && o!=par)
			o = o->parent();
		return o==par;
	}

	//! \return parent object of \a o that inherits \a className or NULL if no such parent
	template<class type>
	inline type* findParent(QObject* o, const char* className)
	{
		if (!o || !className || className[0]=='\0')
			return 0;
		while ( ((o=o->parent())) && !o->inherits(className) )
			;
		return static_cast<type*>(o);
	}

	//! \return first found child of \a o, that inherit \a className.
	//! Returned pointer type is casted.
	template<class type>
	type* findFirstChild(QObject *o, const char* className)
	{
		if (!o || !className || className[0]=='\0')
			return 0;
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

	/*! Sets "wait" cursor with 1 second delay. 
	 Does nothing if GUI is not GUI-aware. (see KApplication::guiEnabled()) */
	KEXIUTILS_EXPORT void setWaitCursor();

	/*! Remove "wait" cursor previously set with \a setWaitCursor(), 
	 even if it's not yet visible.
	 Does nothing if GUI is not GUI-aware. (see KApplication::guiEnabled()) */
	KEXIUTILS_EXPORT void removeWaitCursor();

	/*! Helper class. Allocate it in yor code block as follows:
	 <code>
	 KexiUtils::WaitCursor wait;
	 </code>
	 .. and wait cursor will be visible (with a delay) until you're in this block. without 
	 a need to call removeWaitCursor() before exiting the block.
	 Does nothing if GUI is not GUI-aware. (see KApplication::guiEnabled()) */
	class KEXIUTILS_EXPORT WaitCursor
	{
		public:
			WaitCursor();
			~WaitCursor();
	};

	/*! \return filter string in QFileDialog format for a mime type pointed by \a mime
	 If \a kdeFormat is true, QFileDialog-compatible filter string is generated, 
	 eg. "Image files (*.png *.xpm *.jpg)", otherwise KFileDialog -compatible 
	 filter string is generated, eg. "*.png *.xpm *.jpg|Image files (*.png *.xpm *.jpg)".
	 "\\n" is appended if \a kdeFormat is true, otherwise ";;" is appended. */
	KEXIUTILS_EXPORT QString fileDialogFilterString(const KMimeType::Ptr& mime, bool kdeFormat = true);

	/*! @overload QString fileDialogFilterString(const KMimeType::Ptr& mime, bool kdeFormat = true) */
	KEXIUTILS_EXPORT QString fileDialogFilterString(const QString& mimeString, bool kdeFormat = true);

	/*! Like QString fileDialogFilterString(const KMimeType::Ptr& mime, bool kdeFormat = true)
	 but returns a list of filter strings. */
	KEXIUTILS_EXPORT QString fileDialogFilterStrings(const QStringList& mimeStrings, bool kdeFormat);

	/*! \return a color being a result of blending \a c1 with \a c2 with \a factor1 
	 and \a factor1 factors: (c1*factor1+c2*factor2)/(factor1+factor2). */
	KEXIUTILS_EXPORT QColor blendColors(const QColor& c1, const QColor& c2, int factor1, int factor2);
}

//! sometimes we leave a space in the form of empty QFrame and want to insert here
//! a widget that must be instantiated by hand.
//! This macro inserts a widget \a what into a frame \a where.
#define GLUE_WIDGET(what, where) \
	{ QVBoxLayout *lyr = new QVBoxLayout(where); \
	  lyr->addWidget(what); }

#endif //__KEXIUTILS_UTILS_H__
