/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIPROJECTPARTITEM_H
#define KEXIPROJECTPARTITEM_H

#include <qobject.h>
#include <qintdict.h>

namespace KexiDB
{
	class Connection;
}

namespace KexiPart
{

class Info;

/** Project Part Item stores:
	- identifier ident (low-level name, for example: table name)
	- mime type name, eg. "kexi/table"
	- caption (visible, i18n'd hight level name, eg. table or query title)
*/
//! Data that identifies a single part object (not necessary instantiated)
class KEXICORE_EXPORT Item
{
	public:

		Item();
		~Item();

		int identifier() const { return m_id; }
		void setIdentifier(int id) { m_id = id; }

		QCString mime() const { return m_mime; }
		void setMime(const QCString &mime) { m_mime = mime; }

		QString name() const { return m_name; }
		void setName(const QString &name) { m_name = name; }

		QString caption() const { return m_caption; }
		void setCaption(const QString &c) { m_caption = c; }

		QString description() const { return m_desc; }
		void setDescription(const QString &d) { m_desc = d; }

		/*! \return "neverSaved" flag for this item what mean 
		 that is used when new item is created in-memory-only,
		 so we need to indicate for KexiProject about that state. 
		 By default this flag is false. 
		 Used by KexiMainWindowImpl::newObject(). */
		bool neverSaved() const { return m_neverSaved; }

		/*! \sa neverSaved().
		 Used by KexiMainWindowImpl::newObject(). */
		void setNeverSaved(bool set) { m_neverSaved = set; }

		bool isNull() const { return m_id==0; }

	private:
		QCString		m_mime;
		QString		m_name;
		QString 	m_caption;
		QString 	m_desc;
		int		m_id;
		bool m_neverSaved : 1;
};

//typedef QValueList<Item> ItemList;
typedef QIntDict<KexiPart::Item> ItemDict;
typedef QIntDictIterator<KexiPart::Item> ItemDictIterator;

}

#endif

