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

#ifndef KEXIDBINTERFACEMANAGER_H
#define KEXIDBINTERFACEMANAGER_H

#include <qobject.h>
#include <qdict.h>

#include <klibloader.h>

#include "kexidbdriver.h"


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
		KexiDBInterfaceManager(QObject *parent=0, const char *name=0);
		~KexiDBInterfaceManager();

		QStringList getDrivers() const;
		KexiDBDriver *getDriverInfo(QString driver);
		
	protected:
		void load(QString driver);
		void lookupDrivers();
	
		DriverList	m_driverList;
//		KLibLoader	m_libLoader;
};

#endif
