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

#include "image.h"
#include "kwdoc.h"
#include "defs.h"
#include "kwutils.h"

#include <kdebug.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

/******************************************************************/
/* Class: KWImage                                                 */
/******************************************************************/

/*================================================================*/
void KWImage::decRef()
{
    --ref;
    QString key = doc->getImageCollection()->generateKey( this );

    if ( ref <= 0 && doc )
        doc->getImageCollection()->removeImage( this );
    if ( !doc && ref == 0 ) kdWarning() << "RefCount of the image == 0, but I couldn't delete it, "
                                " because I have not a pointer to the document!" << endl;
}

/*================================================================*/
void KWImage::incRef()
{
    ++ref;
    QString key = doc->getImageCollection()->generateKey( this );
}

/*================================================================*/
void KWImage::save( QDomElement& parentElem )
{
    QDomElement elem = parentElem.ownerDocument().createElement( "FILENAME" );
    parentElem.appendChild( elem );
    elem.setAttribute( "value", correctQString( filename ) );
}

/*================================================================*/
void KWImage::load( QDomElement &attributes, KWDocument *_doc )
{
    doc = _doc;
    ref = 0;

    // <FILENAME>
    QDomElement filenameElement = attributes.namedItem( "FILENAME" ).toElement();
    if ( !filenameElement.isNull() )
    {
        filename = correctQString( filenameElement.attribute( "value" ) );
        QImage::load( filename );
    }
    else
    {
        kdError(32001) << "Missing FILENAME tag in IMAGE" << endl;
    }
}
