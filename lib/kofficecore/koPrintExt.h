#ifndef __ko_print_ext_h__
#define __ko_print_ext_h__

#include "koffice.h"

#include <qpicture.h>
#include <qpainter.h>

class KoPrintExt : virtual public KOffice::Print_skel
{
public:
  // C++
  KoPrintExt();
  
  // IDL
  char* encodedMetaFile( CORBA::Long _width, CORBA::Long _height );

  // C++
  QPicture* picture();

protected:
  /**
   * Overload this function to provide printing services for your application.
   * Your document class must derive from this class in addition.
   * Dont forget to call 
   * <pre>
   * ADD_INTERFACE( "IDL:KOffice/Print:1.0" );
   * </pre>
   * in the constructor of your document to register the new interface.
   * Lets have an example taken from KImage:
   * <pre>
   * class ImageDocument_impl : public QObject,
   *                            virtual public KoDocument,
   *                            virtual public KoPrintExt,
   *                            virtual public KImage::ImageDocument_skel
   * </pre>
   * Your constructor must then look like
   * <pre>
   * ImageDocument_impl::ImageDocument_impl()
   * {
   *   ADD_INTERFACE( "IDL:OPParts/Print:1.0" );
   *   ....
   * }
   * </pre>
   */
  virtual void draw( QPaintDevice*, CORBA::Long _width, CORBA::Long _height ) = 0;
  virtual void draw( CORBA::Long _width, CORBA::Long _height );

  QPicture* m_pPicture;
};

#endif
