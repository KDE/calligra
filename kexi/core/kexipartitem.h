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


/** Project Part Item stores 
	- KexiProjectHandler
	- identifier ident (low-level name, for example: table name)
	- mime type name
	- title (visible hight leve name, eg. table or query title)
*/

namespace KexiDB
{
	class Connection;
}

namespace KexiPart
{

class Info;

class KEXICORE_EXPORT Item
{
	public:
		Item();
		~Item();

		int		identifier() { return m_id; }
		QString		mime() { return m_mime; }
		QString		name() { return m_name; }
		QString		caption() { return m_caption; }



		void		setIdentifier(int id) { m_id = id; }
		void		setMime(const QString &mime) { m_mime = mime; }
		void		setName(const QString &name) { m_name = name; }
		void		setCaption(const QString &c) { m_caption = c; }

	private:
		QString		m_mime;
		QString 	m_caption;
		QString		m_name;
		int		m_id;
};

}

#endif

