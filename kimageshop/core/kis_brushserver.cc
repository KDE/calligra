/*
 *  kis_brushserver.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qdir.h>
#include <qfileinfo.h>
#include <qstringlist.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kinstance.h>

#include "kis_factory.h"
#include "kis_brushserver.h"

KisBrushServer::KisBrushServer()
{
  m_brushes.setAutoDelete(true);

  QStringList list;
  list = KisFactory::global()->dirs()->findAllResources("kis_brushes", "*.png", false, true);
  QString file;
  
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
      file = *it;
      (void) loadBrush( file );
    }
}

KisBrushServer::~KisBrushServer()
{
  m_brushes.clear();
}

const KisBrush * KisBrushServer::loadBrush( const QString& filename )
{
  KisBrush *brush = new KisBrush( filename );

  if ( brush->isValid() )
      m_brushes.append(brush);
  else {
    delete brush;
    brush = 0L;
  }

  return brush;
}
