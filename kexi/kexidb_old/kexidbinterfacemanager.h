/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIDBINTERFACEMANAGER_H
#define KEXIDBINTERFACEMANAGER_H

#include <qobject.h>
#include <qdict.h>

#include <klibloader.h>

#include <kservice.h>


class KexiDBDriver;

typedef QDict<KexiDBDriver> DriverList;


/*
	this class holds information
	of the driver-interfaces (KexiDBDriver)
	it also loads them if it's needed

*/

class KexiDBInterfaceManager : public QObject
{
	friend class KexiDB;
	Q_OBJECT

	public:
		KexiDBInterfaceManager(const char *name=0);
		~KexiDBInterfaceManager();

		QStringList drivers() const;
		KService::Ptr driverInfo(const QString &driver);

		KexiDB* newDBInstance(const QString &driver);

		static  KexiDBInterfaceManager *self();
		void addRef();
		void remRef();

	protected:
		KexiDB *load(const QString &driver);
		void lookupDrivers();

		QMap<QString, KService::Ptr> m_driverList;
		static KexiDBInterfaceManager *s_kexidbinterfacemanager;
		int m_ref;
};

#endif
