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

#include "kexiidentifier.h"
#include <assert.h>

KexiIdentifier::KexiIdentifier(const QString& location, const KexiUUID& id) {
	m_objectLocation=location;
	assert (id.isValid());
	m_objectID=id;
	m_sharingAllowed=true;
}

KexiIdentifier::KexiIdentifier(const QString& location, const QString& id) {
	m_objectLocation=location;
	if (id.isEmpty()) {
		 m_objectID=KexiUUID();
		m_sharingAllowed=true;
	}
	else {
		m_objectID=KexiUUID(id);
		m_sharingAllowed=m_objectID.isValid();
		if (!m_sharingAllowed) m_objectID2=id;
	}
}

QString KexiIdentifier::objectLocation() const {
	return m_objectLocation;
}

	
QString KexiIdentifier::objectPath() const {
	
	return m_objectLocation+"/"+(m_sharingAllowed?m_objectID.asString():m_objectID2);
}

const KexiUUID &KexiIdentifier::objectID() const {
	return m_objectID;
}

bool KexiIdentifier::sharingAllowed() const {
	return m_sharingAllowed;
}
