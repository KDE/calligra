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
#include "kword_doc.h"
#include "defs.h"
#include "kword_utils.h"

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
void KWImage::save( QTextStream&out )
{
    out << indent << "<FILENAME value=\"" << correctQString( filename ).latin1() << "\"/>" << endl;
}

/*================================================================*/
void KWImage::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst, KWordDocument *_doc )
{
    doc = _doc;
    ref = 0;

    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) )
    {
        parser.parseTag( tag, name, lst );

        // filename
        if ( name == "FILENAME" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                {
                    filename = correctQString( ( *it ).m_strValue );
                    QImage::load( filename );
                }
            }
        }

        else
            kdError(32001) << "Unknown tag '" << tag << "' in IMAGE" << endl;

        if ( !parser.close( tag ) )
        {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
}
