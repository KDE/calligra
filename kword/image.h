/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef image_h
#define image_h

#include <qimage.h>
#include <qstring.h>
#include <qdom.h>
#include <qtextstream.h>

class KWDocument;

/******************************************************************/
/* Class: KWImage                                                 */
/******************************************************************/

class KWImage : public QImage
{
public:
    KWImage( KWDocument *_doc, QString _filename ) : QImage( _filename )
    { ref = 0; doc = _doc; filename = _filename; }
    KWImage( KWDocument *_doc, KWImage _image ) : QImage( ( QImage )_image )
    { ref = 0; filename = _image.getFilename(); doc = _doc; }
    KWImage( KWDocument *_doc, QImage _image, QString _filename ) : QImage( _image )
    { ref = 0; filename = _filename; doc = _doc; }
    KWImage() : QImage()
    { ref = 0; doc = 0L; }

    void setDocument( KWDocument *_doc )
    { doc = _doc; }

    void incRef();
    void decRef();
    int refCount()
    { return ref; }

    QString getFilename()
    { return filename; }

    void save( QDomElement &parentElem );
    void load( QDomElement &attributes, KWDocument* );

protected:
    int ref;
    KWDocument *doc;
    QString filename;

};

#endif
