/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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

#include "koIMR.h"
#include <opIMR.h>
#include <opMediator.h>
#include <opPOAMediator.h>
#include "koQueryTrader.h"

#include <klocale.h>
#include <kapp.h>

#include <opApplication.h>
#include <qmsgbox.h>

/**
 * This code is inspired by micos "imr" implementation.
 * The original code is copyrighted by 
 * (c) 1997 Kay Roemer & Arno Puder
 *
 * Modifications and extensions done by Torben Weis
 * (c) 1998 Torben Weis <weis@kde.org>
 */

KOffice::Document_ptr imr_createDoc( const char *_server_name, const char *_repoid )
{
  CORBA::Object_var obj = imr_activate( _server_name, _repoid );
  if ( CORBA::is_nil( obj ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not start server %s" ), _server_name );
    QMessageBox::critical( (QWidget*)0L, i18n("KSpread Error"), tmp, i18n( "OK" ) );
    return 0L;
  }
  
  cerr << "1" << endl;
  
  KOffice::DocumentFactory_var factory = KOffice::DocumentFactory::_narrow( obj );
  if( CORBA::is_nil( factory ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Server %s does not implement a KOffice factory" ), _server_name );
    QMessageBox::critical( (QWidget*)0L, i18n("KSpread Error"), tmp, i18n( "Ok" ) );
    return 0L;
  }

  cerr << "2" << endl;

  KOffice::Document_ptr doc = factory->create();
  if( CORBA::is_nil( doc ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Server %s did not create a document" ), _server_name );
    QMessageBox::critical( (QWidget*)0L, i18n("KSpread Error"), tmp, i18n( "Ok" ) );
    return 0L;
  }

  cerr << "3" << endl;

  return doc;
}

KOffice::Document_ptr imr_createDoc( KoDocumentEntry& _e )
{
  return imr_createDoc( _e.name, _e.repoID.first() );
}

KOffice::Document_ptr imr_createDocByMimeType( const char *_mime_type )
{
  vector<KoDocumentEntry> lst = koQueryDocuments();
  vector<KoDocumentEntry>::iterator it = lst.begin();
  for( ; it != lst.end(); ++it )
  {
    if ( it->supportsMimeType( _mime_type ) )
      return imr_createDoc( it->name, it->repoID.first() );
  }

  return 0L;
}
