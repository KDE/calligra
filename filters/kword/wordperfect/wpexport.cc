/* This file is part of the KDE project
   Copyright (C) 2001 Ariya Hidayat <ariyahidayat@yahoo.de>

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


#include <kdebug.h>
#include <qdom.h>
#include <qfile.h>

#include "wpexport.h"
#include "wpexport.moc"


WPExport::WPExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) 
{
}

bool WPExport::filter(const QString  &filenameIn,
                         const QString  &filenameOut,
                         const QString  &from,
                         const QString  &to,
                         const QString  &         )
{
  // check for proper conversion
  if ( to != "application/wordperfect" || from != "application/x-kword" )
      return false;

  kdDebug() << "KWord WordPerfect filter: sorry, export is not supported now " << endl;

  return false;
}

