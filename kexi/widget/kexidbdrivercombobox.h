/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDBDRIVERCOMBOBOX_H
#define KEXIDBDRIVERCOMBOBOX_H

#include <qwidget.h>
#include <qmap.h>

#include <kcombobox.h>

#include <kexidb/driver.h>

//! \brief Combo box widget for selecting a database driver
class KEXIEXTWIDGETS_EXPORT KexiDBDriverComboBox : public KComboBox
{
	Q_OBJECT

	public:
		KexiDBDriverComboBox(const KexiDB::Driver::InfoMap& driversInfo, 
			bool includeFileBasedDrivers = true, QWidget* parent = 0, const char* name = 0);
		~KexiDBDriverComboBox();

		QStringList driverNames() const { return m_driverNames; }
		QString selectedDriverName() const;
		void setDriverName(const QString& driverName);

	protected:
		QMap<QString,QString> m_driversMap;
		QStringList m_driverNames;
};

#endif

