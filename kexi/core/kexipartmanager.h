/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
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

#ifndef KEXIPARTMANAGER_H
#define KEXIPARTMANAGER_H

#include <qobject.h>
#include <qdict.h>
#include <qasciidict.h>
#include <qintdict.h>
#include <qvaluelist.h>
#include <qptrlist.h>
#include <kservice.h>

namespace KexiDB
{
	class Connection;
}

namespace KexiPart
{
	class Info;
	class Part;


	struct Missing
	{
		QString name;
		QCString mime;
		QString url;
	};

	typedef QAsciiDict<Info> PartInfoDict;
	typedef QDictIterator<Info> PartInfoDictIterator;
	typedef QValueList<Missing> MissingList;
	typedef QPtrList<Info> PartInfoList;
	typedef QIntDict<Part> PartDict;

/**
 * queries parts and dlopens them when needed, they aren't dlopened at startup tough
 */
class KEXICORE_EXPORT Manager : public QObject
{
	Q_OBJECT

	public:
		/**
		 * creates an empty instance
		 */
		Manager(QObject *parent = 0);
		~Manager();

		/**
		 * queries ktrader and creates a list of available parts
		 */
		void		lookup();

		/**
		 * \return a part object for specified mime type. Dlopens a part using KexiPart::Info
		 * if needed. Return 0 if loading failed.
		 */
		Part		*part(const QCString &mime);

		/**
		 * \return a part object for specified info. Dlopens a part using KexiPart::Info
		 * if needed. Return 0 if loading failed.
		 */
		Part		*part(Info *);

		/**
		 * \return the info for a coresponding internal mime
		 */
		Info		*info(const QCString &mime);

		/**
		 * checks project's kexi__part table, creates one if nessesary
		 * and checks if all parts used in a project are available locally
		 *
		 * use @ref missingParts() to get a list of missing parts
		 */
		bool		checkProject(KexiDB::Connection *conn);

		/**
		 * @returns parts metioned in the project meta tables but not available locally
		 */
		MissingList	missingParts() const { return m_missing; }


		/**
		 * @returns a list of the available KexiParts
		 */
		PartInfoList		*partInfoList() { return &m_partlist; }


#if 0
		void unloadPart(Info *i);
		void unloadAllParts();
#endif
	signals:
		void partLoaded(KexiPart::Part*);

	private:
		PartDict m_parts;
		PartInfoList	m_partlist;
		PartInfoDict	m_partsByMime;
		MissingList	m_missing;

		bool m_lookupDone : 1;
};

}

#endif

