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
  QByteArray encodedMetaFile( long int _width, long int _height, float _scale );

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
   *   ADD_INTERFACE( "IDL:KOffice/Print:1.0" );
   *   ....
   * }
   * </pre>
   */
  virtual void draw( QPaintDevice*, long int _width, long int _height,
		     float _scale ) = 0;
  virtual void draw( long int _width, long int _height, float _scale );

  QPicture* m_pPicture;
};

#endif
