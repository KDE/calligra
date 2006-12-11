/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

#include <kexidb/object.h>

//#include "kexipartdatasource.h"

namespace KexiDB
{
	class Connection;
}

namespace KexiPart
{
	class Info;
	class Part;
	class StaticPart;

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
	typedef QPtrListIterator<Info> PartInfoListIterator;
	typedef QIntDict<Part> PartDict;
//	typedef QPtrList<DataSource> DataSourceList;

/**
 * @short KexiPart's manager: looks up and instantiates them
 *
 * It dlopens them when needed, they aren't dlopened at startup is not necessary.
 */
class KEXICORE_EXPORT Manager : public QObject, public KexiDB::Object
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
		void lookup();

		/**
		 * \return a part object for specified mime type. Dlopens a part using KexiPart::Info
		 * if needed. Return 0 if loading failed.
		 */
		Part *partForMimeType(const QString& mimeTypt);

		/**
		 * \return a part object for specified info. Dlopens a part using KexiPart::Info
		 * if needed. Return 0 if loading failed.
		 */
		Part *part(Info *);

		/**
		 * \return the info for a coresponding internal mime
		 */
		Info *infoForMimeType(const QString& mimeType);

		/**
		 * checks project's kexi__part table
		 * and checks if all parts used in a project are available locally
		 *
		 * use @ref missingParts() to get a list of missing parts
		 */
		bool checkProject(KexiDB::Connection *conn);

		/**
		 * @returns parts metioned in the project meta tables but not available locally
		 */
		MissingList missingParts() const { return m_missing; }


		/**
		 * @returns a list of the available KexiParts in well-defined order
		 */
		PartInfoList *partInfoList() { return &m_partlist; }

	signals:
		void partLoaded(KexiPart::Part*);

	protected:
		//! Used by StaticPart
		void insertStaticPart(KexiPart::StaticPart* part);

	private:
		PartDict m_parts;
		PartInfoList m_partlist;
		PartInfoDict m_partsByMime;
		MissingList m_missing;
		int m_nextTempProjectPartID;
		bool m_lookupDone : 1;

		friend class StaticPart;
};

}

#endif

