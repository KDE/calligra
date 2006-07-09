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

#include <qlistbox.h>

#include <kiconloader.h>

KexiDBDriverComboBox::KexiDBDriverComboBox(const KexiDB::Driver::InfoMap& driversInfo, 
	bool includeFileBasedDrivers, QWidget* parent, const char* name)
 : KComboBox(parent, name)
{
	foreach(KexiDB::Driver::InfoMap::ConstIterator, it, driversInfo) {
		if (!includeFileBasedDrivers && it.data().fileBased)
			continue;
		//! @todo change this if better icon is available
		insertItem( SmallIcon("gear"), it.data().caption );
		m_driversMap.insert(it.data().caption, it.data().name.lower());
	}
	if (listBox())
		listBox()->sort();

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
		return QString::null;
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
