/* $Id:$
   This file is part of KOffice
    Copyright (C) 1998 Kalle Dalheimer <kalle@kde.org>

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

#include "koFilterManager.h"

#include <kapp.h>
#include <klocale.h>

KoFilterManager* KoFilterManager::_instance = 0;

KoFilterManager* KoFilterManager::instance()
{
	if( _instance == 0 ) {
		_instance = new KoFilterManager;
	}
	return _instance;
}


void KoFilterManager::registerFilter( const char* name, 
									  const char* extensions, 
									  Direction direction, KoFilter* filter )
{
	KoFilterData* fd = new KoFilterData;
	fd->name = name;
	fd->extensions = extensions;
	fd->direction = direction;
	fd->filter = filter;
	_filterlist.append( fd );
}

QStrList KoFilterManager::fileSelectorList( Direction direction, 
											bool allfiles ) const
{
	QStrList ret;
	QListIterator<KoFilterData> it( _filterlist );
	KoFilterData* current;
	while( ( current = it.current() ) ) {
		++it;
		if( current->direction == direction ) {
			QString filterstr;
			filterstr = current->name + " (" + current->extensions + ")";
			ret.append( filterstr );
		}
	}
	if( allfiles ) {
		QString filterstr = i18n( "All files (*.*)" );
		ret.append( filterstr );
	}
	
	return ret;
}

void* KoFilterManager::invokeFilterParser( Direction direction, 
										   const char* filename,
										   KoFilter*& filter )
{
	return 0;
}

KoFilterManager::KoFilterManager()
{
	_filterlist.setAutoDelete( true );
}

