/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef contents_h
#define contents_h

#include <qstringlist.h>

class KWDocument;
class KWParag;

class KWContents
{
public:
    KWContents( KWDocument *doc_ );

    void createContents();
    void addParagName( const QString &n ) {
	parags.append( n );
    }
    void setEnd( KWParag *e ) {
	end = e;
    }
    bool hasContents() {
	return end && !parags.isEmpty();
    }
    bool numParags() {
	return parags.count();
    }
    QStringList::Iterator begin() {
	return parags.begin();
    }
    QStringList::Iterator ending() {
	return parags.end();
    }

protected:
    KWDocument *doc;
    KWParag *end;
    QStringList parags;

};

#endif
