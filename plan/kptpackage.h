/*  This file is part of the KDE project
    Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef KPLATO_KPTPACKAGE_H
#define KPLATO_KPTPACKAGE_H

#include "kpttask.h"

#include "kptpackage.h" //Note! in parent dir

#include <KUrl>

#include <QString>

namespace KPlato {

class Project;

// temporary convinience class
class Package
{
public:
    Package();
    Project *project;
    QString ownerId;
    QString ownerName;

    WorkPackageSettings settings;

    Task *task;
    QMap<QString, KUrl> documents;
};

}

#endif // KPLATO_KPTPACKAGE_H
