/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#ifndef __ko_query_types_h__
#define __ko_query_types_h__

#include <qstring.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <vector>

#include <CORBA.h>

void koInitTrader();

class KoComponentEntry
{
public:
  QString comment;
  QString name;
  QString exec;
  QString activationMode;
  QStringList repoID;
  QPixmap miniIcon;
  QPixmap icon;
  CORBA::Object_var reference;
};

class KoDocumentEntry : public KoComponentEntry
{
public:
  KoDocumentEntry() { }
  KoDocumentEntry( const KoComponentEntry& _e );
  
  QStringList mimeTypes;

  bool supportsMimeType( const char *_m ) { return ( mimeTypes.find( _m ) != mimeTypes.end() ); }
};

class KoFilterEntry : public KoComponentEntry
{
public:
  KoFilterEntry() { }
  KoFilterEntry( const KoComponentEntry& _e );

  QString import;
  QString importDescription;
  QString export;
  QString exportDescription;

  bool imports( const char *_m ) { return ( import.find( _m ) != -1 ); }
  bool exports( const char *_m ) { return ( export.find( _m ) != -1 ); }
};

vector<KoDocumentEntry> koQueryDocuments( const char *_constr = "", int _count = 100 );
vector<KoFilterEntry> koQueryFilters( const char *_constr = "", int _count = 100 );

#endif
