/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
  
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

#include <qdir.h>
#include <qfileinfo.h>
#include <qstringlist.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kinstance.h>

#include "karbon_factory.h"
#include "karbon_resourceserver.h"

KarbonResourceServer::KarbonResourceServer()
{
    m_patterns.setAutoDelete(true);

    // image formats
    QStringList formats;
    formats << "*.png" << "*.tif" << "*.xpm" << "*.bmp" << "*.jpg" << "*.gif";

    // init vars
    QStringList lst;
    QString format, file;

    // find pattern
    for( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
    {
		format = *it;
		QStringList l = KarbonFactory::instance()->dirs()->findAllResources("kis_pattern", format, false, true);
		lst += l;
	}
    // load pattern
    for( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
    {
		file = *it;
		loadPattern( file );
    }
}

KarbonResourceServer::~KarbonResourceServer()
{
    m_patterns.clear();
}

const VPattern *KarbonResourceServer::loadPattern( const QString& filename )
{
    VPattern *pattern = new VPattern( filename );

    if( pattern->isValid() )
        m_patterns.append( pattern );
    else
    {
        delete pattern;
        pattern = 0L;
    }

    return pattern;
}

