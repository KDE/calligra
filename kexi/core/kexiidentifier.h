/* This file is part of the KDE project
Copyright (C) 2003 Joseph Wenninger<jowenn@kde.org>

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

#ifndef _KEXI_IDENTIFIER_H_
#define _KEXI_IDENTIFIER_H_

#include "kexiuuid.h"
#include <qstring.h>

/**class identifying a unique kexi object. the objectID itself is unique, but for easier, structured lookup there 
is an objectLocation, which must not be empty. the object location is something like kexi/table.
objectID is an UUID which can be converted into a string like: 8e716b12-cd72-4c85-955b-9cee77832478. */

class KexiIdentifier {
public:
	KexiIdentifier(const QString& location, const KexiUUID& id);
	KexiIdentifier(const QString& location, const QString& id=QString());
	QString objectLocation() const;
	QString objectPath() const; //objectPath+objectID
	const KexiUUID &objectID() const;
	bool sharingAllowed() const;
private:

	KexiUUID m_objectID;
	QString m_objectID2;
	QString m_objectLocation;
	bool m_sharingAllowed;
	
	class Private;
	Private *d;


};

#endif
