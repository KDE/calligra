/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXITABLEFILTERMANAGER_H
#define KEXITABLEFILTERMANAGER_H

#include <qobject.h>
#include <qptrlist.h>
#include <kservice.h>

class KexiTableFilter;
class KexiTableFilterMeta;

typedef QPtrList<KexiTableFilterMeta> Filters;

class KexiTableFilterManager : public QObject
{
	Q_OBJECT

	public:
		KexiTableFilterManager(QObject *parent, const char *name=0);
		~KexiTableFilterManager();

		void		lookupFilters();
		KexiTableFilter	*getFilter(KexiTableFilterMeta *m);

		Filters		importFilters() { return m_import; }
		Filters		exportFilters() { return m_export; }

	private:
		Filters		m_import;
		Filters		m_export;
};

class KexiTableFilterMeta
{
	public:
		KexiTableFilterMeta(KService::Ptr &s);
		~KexiTableFilterMeta();

		QString		lib() { return m_lib; }
		QString		comment() { return m_comment; }

		KService::Ptr	service() { return m_service; }

	private:
		QString		m_lib;
		QString		m_comment;
		KService::Ptr	m_service;
};

#endif
