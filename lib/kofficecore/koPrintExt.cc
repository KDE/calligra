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

#include "koPrintExt.h"

#include <strstream>

#include <kdebug.h>

#include <komlMime.h>

KoPrintExt::KoPrintExt()
{
  m_pPicture = 0L;
}

QPicture* KoPrintExt::picture()
{
  if ( m_pPicture == 0L )
    m_pPicture = new QPicture;
  
  return m_pPicture;
}

char* KoPrintExt::encodedMetaFile( CORBA::Long _width, CORBA::Long _height,
				   CORBA::Float _scale )
{
  if ( m_pPicture == 0L )
    draw( _width, _height, _scale );
  
  int size = m_pPicture->size() * 4 / 3 + 10;
  char *p = CORBA::string_alloc( size );

  {
    // Create a 7bit ASCII string of the image
    ostrstream str( p, size );
    {      
      Base64OStream out( str );
      out.write( m_pPicture->data(), m_pPicture->size() );
    }
    str.put( 0 );
  }

  // Save some memory
  delete m_pPicture;
  m_pPicture = 0L;

  return p;
}

void KoPrintExt::draw( CORBA::Long _width, CORBA::Long _height, CORBA::Float _scale )
{
  // Create picture
  QPicture *p = picture();
  // Paint to it
  draw( p, _width, _height, _scale );
  kdebug( KDEBUG_INFO, 30003, "QPicture has %i bytes", p->size() );
}










