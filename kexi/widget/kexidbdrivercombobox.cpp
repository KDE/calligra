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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidbdrivercombobox.h"

#include <q3listbox.h>

#include <kiconloader.h>

KexiDBDriverComboBox::KexiDBDriverComboBox(QWidget* parent, const KexiDB::Driver::InfoMap& driversInfo, 
	Options options)
 : KComboBox(parent, "KexiDBDriverComboBox")
{
	//retrieve list of drivers and sort it: file-based first, then server-based
	QStringList captionsForFileBasedDrivers, captionsForServerBasedDrivers;
	QMap<QString,QString> fileBasedDriversDict, serverBasedDriversDict; //a map from caption to name
	foreach(KexiDB::Driver::InfoMap::ConstIterator, it, driversInfo) {
		if (it.data().fileBased) {
			captionsForFileBasedDrivers += it.data().caption;
			fileBasedDriversDict[it.data().caption] = it.data().name.lower();
		}
		else {
			captionsForServerBasedDrivers += it.data().caption;
			serverBasedDriversDict[it.data().caption] = it.data().name.lower();
		}
	}
	captionsForFileBasedDrivers.sort();
	captionsForServerBasedDrivers.sort();
	//insert file-based
	if (options & ShowFileDrivers) {
		foreach(QStringList::ConstIterator, it, captionsForFileBasedDrivers) {
			const KexiDB::Driver::Info& info = driversInfo[ fileBasedDriversDict[ *it ] ];
			//! @todo change this if better icon is available
			insertItem( SmallIcon("gear"), info.caption );
			m_driversMap.insert(info.caption, info.name.lower());
		}
	}
	//insert server-based
	if (options & ShowServerDrivers) {
		foreach(QStringList::ConstIterator, it, captionsForServerBasedDrivers) {
			const KexiDB::Driver::Info& info = driversInfo[ serverBasedDriversDict[ *it ] ];
			//! @todo change this if better icon is available
			insertItem( SmallIcon("gear"), info.caption );
			m_driversMap.insert(info.caption, info.name.lower());
		}
	}
//	if (listBox())
//		listBox()->sort();

	// Build the names list after sorting
	for (int i=0; i<count(); i++)
		m_driverNames += m_driversMap[ text(i) ];
}

KexiDBDriverComboBox::~KexiDBDriverComboBox()
{
}

QString KexiDBDriverComboBox::selectedDriverName() const
{
	QMapConstIterator<QString,QString> it( m_driversMap.find( text( currentItem() ) ) );
	if (it==m_driversMap.constEnd())
		return QString();
	return it.data();
}

void KexiDBDriverComboBox::setDriverName(const QString& driverName)
{
	int index = m_driverNames.findIndex( driverName.lower() );
	if (index==-1) {
		return;
	}
	setCurrentItem(index);
}

#include "kexidbdrivercombobox.moc"
