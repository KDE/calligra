/* This file is part of the KDE project
   Copyright (C) 2005-2015 Jarosław Staniek <staniek@kde.org>

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

#include <KexiIcon.h>

#include <KDbDriverManager>
#include <KDbDriverMetaData>
#include <KDbMessageHandler>

class KexiDBDriverComboBox::Private
{
public:
    Private() {}

    QList<const KDbDriverMetaData*> drivers;
    QStringList driverIds;
};

//! Used to sort driver items by visible name
bool compareByName(const KDbDriverMetaData* d1, const KDbDriverMetaData* d2)
{
    return d1->name() < d2->name();
}

// ---

KexiDBDriverComboBox::KexiDBDriverComboBox(QWidget* parent, Options options)
        : KComboBox(parent)
        , d(new Private)
{
    // retrieve list of drivers and sort it: file-based first, then server-based
    KDbDriverManager manager;
    QList<const KDbDriverMetaData*> fileBasedDrivers, serverBasedDrivers;
    foreach(const QString &id, manager.driverIds()) {
        const KDbDriverMetaData* driverMetaData = manager.driverMetaData(id);
        KDbMessageGuard mg(manager.resultable());
        if (!driverMetaData) {
            return;
        }
        if (driverMetaData->isFileBased()) {
            fileBasedDrivers.append(driverMetaData);
        } else {
            serverBasedDrivers.append(driverMetaData);
        }
    }
    //insert file-based
    if (options & ShowFileDrivers) {
        std::sort(fileBasedDrivers.begin(), fileBasedDrivers.end(), compareByName);
        d->drivers += fileBasedDrivers;
    }
    if (options & ShowServerDrivers) {
        std::sort(serverBasedDrivers.begin(), serverBasedDrivers.end(), compareByName);
        d->drivers += serverBasedDrivers;
    }
    foreach(const KDbDriverMetaData* driverMetaData, d->drivers) {
        //! @todo change this if better icon is available
        addItem(koIcon("application-x-executable"), driverMetaData->name());
        d->driverIds.append(driverMetaData->id());
    }
}

KexiDBDriverComboBox::~KexiDBDriverComboBox()
{
    delete d;
}

QString KexiDBDriverComboBox::currentDriverId() const
{
    const KDbDriverMetaData* driverMetaData = d->drivers.value(currentIndex());
    return driverMetaData ? driverMetaData->id() : QString();
}

void KexiDBDriverComboBox::setCurrentDriverId(const QString& driverId)
{
    const int index = d->driverIds.indexOf(driverId);
    if (index == -1)
        return;
    setCurrentIndex(index);
}
