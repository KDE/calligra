#include "koPrintExt.h"

#include <strstream>
#include <komlMime.h>

#include <iostream>

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

char* KoPrintExt::encodedMetaFile( CORBA::Long _width, CORBA::Long _height )
{
  if ( m_pPicture == 0L )
    draw( _width, _height );
  
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

void KoPrintExt::draw( CORBA::Long _width, CORBA::Long _height )
{
  // Create picture
  QPicture *p = picture();
  // Paint to it
  draw( p, _width, _height );
  cout << "QPicture has " << p->size() << " bytes" << endl;
}










