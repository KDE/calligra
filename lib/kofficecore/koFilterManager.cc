/* $Id$
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

KoFilterManager* KoFilterManager::s_pSelf = 0;

KoFilterManager* KoFilterManager::self()
{
  if( s_pSelf == 0 )
  {
    s_pSelf = new KoFilterManager;
  }

  return s_pSelf;
}

QString KoFilterManager::fileSelectorList( Direction direction, const char *_format, bool allfiles ) const
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

KoFilterManager::KoFilterManager()
{
}
