/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#ifndef KEXIPROJECTSET_H
#define KEXIPROJECTSET_H

#include <kexidb/connectiondata.h>
#include <kexidb/object.h>

#include "core/kexiprojectdata.h"

class KexiProjectSetPrivate;

/*! Stores information about multiple kexi project-data items
*/

class KEXICORE_EXPORT KexiProjectSet : public KexiDB::Object
{
public:

	/*! Creates empty project set */
    KexiProjectSet();
    
	/*! Creates project set filled with all projects found using \a conndata. 
	 There may be error during project list retrieving - use appropriate 
	 KexiDB::Object::error(), and similar methods to get error message. */
    KexiProjectSet(KexiDB::ConnectionData &conndata);
	
	~KexiProjectSet();

	/*! Adds \a data as connection data. 
	 \a will be owned by a KexiDBConnectionSet object. */
	void addProjectData(KexiProjectData *data);
	
	//! return list object
	KexiProjectData::List list() const;

	//! finds (case insensitive) 
	//! and returns project data for databased \a dbName or NULL if not found
	KexiProjectData* findProject(const QString &dbName) const;

private:
	KexiProjectSetPrivate *d;
};

#endif // KEXINEWDBCONNDIALOG_H

