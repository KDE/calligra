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

#include "koFilterManager.h"
#include "koQueryTypes.h"
#include "koffice.h"

#include <kapp.h>
#include <klocale.h>
#include <kregistry.h>
#include <kregfactories.h>
#include <kmimetypes.h>
#include <kmimemagic.h>

#include <qmsgbox.h>
#include <qstringlist.h>

#include <assert.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

KoFilterManager* KoFilterManager::s_pSelf = 0;

KoFilterManager* KoFilterManager::self()
{
  if( s_pSelf == 0 )
  {
    s_pSelf = new KoFilterManager;
  }

  return s_pSelf;
}

vector<KoFilterEntry> koQueryFilters( const char *_constr , int _count );
KRegistry * registry = 0L;

QString KoFilterManager::fileSelectorList( Direction direction, const char *_format,
					   const char *_native_pattern,
					   const char *_native_name,
					   bool allfiles ) const
{
  QString constr;
  if ( direction == Import )
    constr = "Export == '";
  else
    constr = "Import == '";
  constr += _format;
  constr += "'";
  vector<KoFilterEntry> vec = koQueryFilters( constr );

  if (!registry)
  {
      registry = new KRegistry;
      registry->addFactory( new KServiceTypeFactory );
      registry->load();
  }

  QString ret;

  if ( _native_pattern && _native_name )
  {
    ret += _native_pattern;
    ret += "|";
    ret += _native_name;
    ret += " (";
    ret += _native_pattern;
    ret += ")";
  }

  for( unsigned int i = 0; i < vec.size(); ++i )
  {
    KMimeType *t;
    QString mime;
    if ( direction == Import )
      mime = vec[i].import;
    else
      mime = vec[i].export;

    t = KMimeType::find( mime );
    // Did we get exact this mime type ?
    if ( t && mime == t->mimeType() )
    {
      QStringList patterns = t->patterns();
      const char* s;
      for(unsigned int j = 0;j < patterns.count();j++)
      {
	s = patterns[j];
	if ( !ret.isEmpty() )
	  ret += "\n";
	ret += s;
	ret += "|";
	if ( direction == Import )
	  ret += vec[i].importDescription;
	else
	  ret += vec[i].exportDescription;
	ret += " (";
	ret += s;
	ret += ")";
      }
    }
    else
    {
      if ( !ret.isEmpty() )
	ret += "\n";
      ret += "*.*|";
      if ( direction == Import )
	ret += vec[i].importDescription;
      else
	ret += vec[i].exportDescription;
      ret += " (*.*)";
    }
  }

  if( allfiles )
  {
    if ( !ret.isEmpty() )
      ret += "\n";
    ret += "*.*|";
    ret += i18n( "All files (*.*)" );
  }

  return ret;
}

QString KoFilterManager::import( const char* _url, const char *_native_format )
{
  KURL url( _url );

  KMimeType *t = KMimeType::findByURL( url, 0, url.isLocalFile() );
  QString mimeType;
  if (t) {
    cerr << "######### FOUND MimeType " << t->mimeType() << endl;
    mimeType = t->mimeType();
  }
  else {
    cerr << "####### No MimeType found. findByURL returned 0. Setting text/plain" << endl;
    mimeType = "text/plain";
  }

  if ( (strcmp( mimeType, _native_format ) == 0) )
  {
    cerr << "strcmp( mimeType, _native_format ) == 0 !! Returning without conversion. " << endl;
    // TODO: fetch remote file!
    assert( url.isLocalFile() );

    return QString( url.url() );
  }

  QString constr = "Export == '";
  constr += _native_format;
  constr += "' and Import == '";
  constr += mimeType;
  constr += "'";

  vector<KoFilterEntry> vec = koQueryFilters( constr );
  if ( vec.size() == 0 )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not import file of type\n%s"), t->mimeType().ascii() );
    QMessageBox::critical( 0L, i18n("Missing import filter"), tmp, i18n("OK") );
    return QString();
  }

  KOffice::Filter::Data data;

  struct stat buff;
  stat( url.path(), &buff );
  unsigned int size = buff.st_size;

  FILE *f = fopen( url.path(), "rb" );
  if ( !f )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not open file\n%s"), url.path().ascii() );
    QMessageBox::critical( 0L, i18n("Error"), tmp, i18n("OK") );
    return QString();
  }

  char *p = new char[ size ];
  fread( p, 1, size, f );
  fclose( f );

  data.length( size );
  for( CORBA::ULong l = 0; l < size; l++ )
    data[l] = (CORBA::Octet)p[l];

  delete []p;

  KOffice::FilterFactory_var factory;
  factory = KOffice::FilterFactory::_narrow( vec[0].reference );
  assert( !CORBA::is_nil( factory ) );
  KOffice::Filter_var filter = factory->create();
  assert( !CORBA::is_nil( filter ) );

  filter->filter( data, mimeType, _native_format );

  p = new char[ data.length() ];
  for( CORBA::ULong l = 0; l < data.length(); l++ )
    p[l] = (char)data[l];

  // TODO: Better tmp name
  f = fopen("/tmp/kofficefilter", "w" );
  if ( !f )
  {
    delete []p;

    QString tmp;
    tmp.sprintf( i18n("Could not write file\n%s"), "/tmp/kofficefilter" );
    QMessageBox::critical( 0L, i18n("Error"), tmp, i18n("OK") );
    return QString();
  }

  fwrite( p, 1, data.length(), f );
  fclose( f );

  delete []p;

  return QString("/tmp/kofficefilter");
}

KoFilterManager::KoFilterManager()
{
}
