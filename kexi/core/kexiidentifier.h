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
