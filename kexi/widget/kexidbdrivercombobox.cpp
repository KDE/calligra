/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

KexiDBDriverComboBox::KexiDBDriverComboBox(QWidget* parent,
        const KexiDB::Driver::InfoHash& driversInfo, Options options)
        : KComboBox(parent)
{
    //retrieve list of drivers and sort it: file-based first, then server-based
    QStringList captionsForFileBasedDrivers, captionsForServerBasedDrivers;
    QHash<QString, QString> fileBasedDriversDict, serverBasedDriversDict; //a map from caption to name
    foreach(const KexiDB::Driver::Info& info, driversInfo) {
        if (info.fileBased) {
            captionsForFileBasedDrivers += info.caption;
            fileBasedDriversDict[info.caption] = info.name.toLower();
        } else {
            captionsForServerBasedDrivers += info.caption;
            serverBasedDriversDict[info.caption] = info.name.toLower();
        }
    }
    captionsForFileBasedDrivers.sort();
    captionsForServerBasedDrivers.sort();
    //insert file-based
    if (options & ShowFileDrivers) {
        foreach(QString caption, captionsForFileBasedDrivers) {
            const KexiDB::Driver::Info& info = driversInfo[ fileBasedDriversDict[ caption ] ];
            //! @todo change this if better icon is available
            addItem(KIcon("application-x-executable"), info.caption);
            m_drivers.insert(info.caption, info.name.toLower());
        }
    }
    //insert server-based
    if (options & ShowServerDrivers) {
        foreach(QString caption, captionsForServerBasedDrivers) {
            const KexiDB::Driver::Info& info = driversInfo[ serverBasedDriversDict[ caption ] ];
            //! @todo change this if better icon is available
            addItem(KIcon("application-x-executable"), info.caption);
            m_drivers.insert(info.caption, info.name.toLower());
        }
    }

    // Build the names list after sorting
    for (int i = 0; i < count(); i++)
        m_driverNames += m_drivers[ text(i)];
}

KexiDBDriverComboBox::~KexiDBDriverComboBox()
{
}

QString KexiDBDriverComboBox::selectedDriverName() const
{
    QHash<QString, QString>::ConstIterator it = m_drivers.find(itemText(currentIndex()));
    if (it == m_drivers.constEnd())
        return QString();
    return it.value();
}

void KexiDBDriverComboBox::setDriverName(const QString& driverName)
{
    const int index = m_driverNames.indexOf(driverName.toLower());
    if (index == -1)
        return;
    setCurrentIndex(index);
}

#include "kexidbdrivercombobox.moc"
