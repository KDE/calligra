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
