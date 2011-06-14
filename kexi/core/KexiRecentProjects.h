/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIRECENTPROJECTS_H
#define KEXIRECENTPROJECTS_H

#include "kexiprojectset.h"

//! @short Stores information about recent projects opened by Kexi.
/*! The location is share/apps/kexi/recent_projects directory.
 Information about one project is store in one .kexis file.
 The loading process is deferred.
*/
class KEXICORE_EXPORT KexiRecentProjects : protected KexiProjectSet
{
public:
    ~KexiRecentProjects();

    /*! Adds @a data as project data.
     @a data will be owned by this object. 
     While saved, data will be merged with earlier saved data. */
    void addProjectData(KexiProjectData *data);

    //! @return list of data items.
    KexiProjectData::List list() const;

protected:
    //! Creates recent projects container.
    explicit KexiRecentProjects(KexiDB::MessageHandler* handler = 0);

    void addProjectDataInternal(KexiProjectData *data);

    KexiProjectData* takeProjectDataInternal(KexiProjectData *data);

private:
    class Private;
    Private * const d;
    friend class KexiInternal;
    friend class Private;
};

#endif // KEXIRECENTPROJECTS_H
