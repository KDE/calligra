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

#ifndef __kofactory_h__
#define __kofactory_h__

#include "koffice.h"
#include <komShutdownManager.h>

class KoDocumentFactory : public KOffice::DocumentFactory_skel
{
};

#define KOFFICE_DOCUMENT_FACTORY( DOC, NAME, SKEL ) \
class NAME : virtual public KoDocumentFactory, \
             virtual public SKEL \
{ \
public: \
  NAME( const CORBA::BOA::ReferenceData &refdata ) : SKEL( refdata ) { } \
  NAME( CORBA::Object_ptr _obj ) : SKEL( _obj ) { } \
  virtual ~NAME() { } \
  virtual KOffice::Document_ptr create()  \
  { \
    DOC *doc = new DOC; \
    KOMShutdownManager::self()->watchObject( doc ); \
    return KOffice::Document::_duplicate( doc ); \
  } \
};

#endif
