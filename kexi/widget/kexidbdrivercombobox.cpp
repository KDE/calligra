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

#include <KoIcon.h>

class KexiDBDriverComboBox::Private
{
public:
    Private() {};

    QHash<QString, QString> drivers; //!< a map: driver caption -> driver name
    QStringList driverNames;
};

KexiDBDriverComboBox::KexiDBDriverComboBox(QWidget* parent,
        const KexiDB::Driver::InfoHash& driversInfo, Options options)
        : KComboBox(parent)
	, d(new Private)
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
            addItem(koIcon("application-x-executable"), info.caption);
            d->drivers.insert(info.caption, info.name.toLower());
        }
    }
    //insert server-based
    if (options & ShowServerDrivers) {
        foreach(QString caption, captionsForServerBasedDrivers) {
            const KexiDB::Driver::Info& info = driversInfo[ serverBasedDriversDict[ caption ] ];
            //! @todo change this if better icon is available
            addItem(koIcon("application-x-executable"), info.caption);
            d->drivers.insert(info.caption, info.name.toLower());
        }
    }

    // Build the names list after sorting
    for (int i = 0; i < count(); i++)
        d->driverNames += d->drivers[ itemText(i) ];
}

KexiDBDriverComboBox::~KexiDBDriverComboBox()
{
    delete d;
}

QStringList KexiDBDriverComboBox::driverNames() const
{
    return d->driverNames;
}

QString KexiDBDriverComboBox::selectedDriverName() const
{
    return d->drivers.value(itemText(currentIndex()));
}

void KexiDBDriverComboBox::setDriverName(const QString& driverName)
{
    const int index = d->driverNames.indexOf(driverName.toLower());
    if (index == -1)
        return;
    setCurrentIndex(index);
}

#include "kexidbdrivercombobox.moc"
