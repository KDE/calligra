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

#ifndef _KEXIUUID_H_
#define _KEXIUUID_H_

#include <qstring.h>

class KexiUUID {
public:
	/* generates new uuid */
	KexiUUID();
	/*copies uuid*/
	KexiUUID(const KexiUUID&);
	/*parse from uuid string*/
	KexiUUID(const QString&);

	QString asString() const;
	bool isValid() const;
	bool	operator==( const KexiUUID  & ) const;
    	bool	operator!=( const KexiUUID  & ) const;
	bool	operator<( const KexiUUID &) const;
private:
	unsigned char uuid[16];
	bool m_isValid;
};

#endif
