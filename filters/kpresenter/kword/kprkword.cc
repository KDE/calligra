/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <david@mandrakesoft.com>

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

#include <config.h>

#include <koStoreDevice.h>
#include "kprkword.h"
#include "kprkword.moc"
#include <kdebug.h>
#include <qsortedlist.h>

KprKword::KprKword(KoFilter *parent, const char *name) :
    KoFilter(parent, name),
    outdoc( "DOC" )
{
}

// This filter can act as an import filter for KWord and as an export
// filter for KPresenter (isn't our architecture really nice ? :)
// This is why we use the file-to-file method, not a QDomDoc one.
bool KprKword::filter(const QString &fileIn, const QString &fileOut,
                      const QString& from, const QString& to,
                      const QString &)
{
    if(to!="application/x-kword" || from!="application/x-kpresenter")
        return false;

    KoStore in( fileIn, KoStore::Read );
    if ( in.bad() )
    {
        kdError(30502) << "Unable to open input file " << fileIn << endl;
        in.close();
        return false;
    }

    if ( !in.open( "root" ) )
    {
        kdError(30502) << "No root document in " << fileIn << endl;
        in.close();
        return false;
    }

    KoStoreDevice inpdev( &in );
    QDomDocument inpdoc;
    inpdoc.setContent( &inpdev );


    outdoc.appendChild( outdoc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement kwdoc = outdoc.createElement( "DOC" );
    kwdoc.setAttribute( "editor", "KprKword converter" );
    kwdoc.setAttribute( "mime", "application/x-kword" );
    kwdoc.setAttribute( "syntaxVersion", 1 );
    outdoc.appendChild( kwdoc );

    QDomElement paper = outdoc.createElement( "PAPER" );
    kwdoc.appendChild( paper );
    paper.setAttribute( "format", 1 ); // A4. How on earth could I know what the user really wants ? :)
    paper.setAttribute( "width", 595 );
    paper.setAttribute( "height", 841 );
    QDomElement borders = outdoc.createElement( "PAPERBORDERS" );
    paper.appendChild( borders );
    borders.setAttribute( "left", 28 );
    borders.setAttribute( "top", 42 );
    borders.setAttribute( "right", 28 );
    borders.setAttribute( "bottom", 42 );

    QDomElement framesets = outdoc.createElement( "FRAMESETS" );
    kwdoc.appendChild( framesets );

    frameset = outdoc.createElement( "FRAMESET" );
    framesets.appendChild( frameset );
    frameset.setAttribute( "frameType", 1 ); // text
    frameset.setAttribute( "frameInfo", 0 ); // body

    convert( inpdoc );

    QDomElement frame = outdoc.createElement( "FRAME" );
    frameset.appendChild( frame );
    frame.setAttribute( "left", 28 );
    frame.setAttribute( "top", 42 );
    frame.setAttribute( "right", 566 );
    frame.setAttribute( "bottom", 798 );
    frame.setAttribute( "autoCreateNewFrame", 1 );
    frame.setAttribute( "newFrameBehaviour", 0 );

    QDomElement styles = outdoc.createElement( "STYLES" );
    kwdoc.appendChild( styles );
    // TODO copy from a template

    KoStore out = KoStore( fileOut, KoStore::Write);
    if(!out.open("root")) {
        kdError(30502) << "Unable to open output file!" << endl;
        in.close();
        out.close();
        return false;
    }
    QCString cstring = outdoc.toCString(); // utf-8 already
    out.write( cstring.data(), cstring.length() );
    out.close();
    in.close();
    return true;
}

// This class is used to sort the objects by y position
class KprObject {
 public:
    int y;
    QDomElement elem;
    bool operator < ( const KprObject & c ) const
    {
        return y < c.y;
    }
    bool operator == ( const KprObject & c ) const
    {
        return y == c.y;
    }
};

void KprKword::convert( const QDomDocument & inpdoc )
{
    QDomElement docElem = inpdoc.documentElement();
    QDomElement objects = docElem.namedItem( "OBJECTS" ).toElement();
    if ( objects.isNull() )
        return;

    QSortedList< KprObject > objList;
    objList.setAutoDelete( true );

    QDomNodeList lst = objects.elementsByTagName( "OBJECT" );
    uint lstcount = lst.count();
    for ( uint item = 0 ; item < lstcount ; ++item )
    {
        QDomElement object = lst.item( item ).toElement();
        if ( object.attribute( "type" ).toInt() == 4 ) // we only care about text objs
        {
            QDomElement orig = object.namedItem( "ORIG" ).toElement();
            KprObject * obj = new KprObject;
            obj->y = orig.attribute( "y" ).toInt();
            obj->elem = object;
            objList.inSort( obj );
        }
    }

    for ( QListIterator<KprObject> it(objList); it.current(); ++it )
    {
        QDomElement elem = it.current()->elem;
        QDomElement textObj = elem.namedItem( "TEXTOBJ" ).toElement();
        if (textObj.isNull())
            continue;
        // For each paragraph in this text object...
        QDomNodeList lst = textObj.elementsByTagName( "P" );
        uint lstcount = lst.count();
        for ( uint item = 0; item < lstcount ; ++item )
        {
            QDomElement p = lst.item( item ).toElement();

            QString text = p.text();
            writeParag( text );
        }
    }
}


void KprKword::writeParag( const QString & text )
{
    QDomElement parag = outdoc.createElement( "PARAGRAPH" );
    frameset.appendChild( parag );

    QDomElement textElem = outdoc.createElement( "TEXT" );
    parag.appendChild( textElem );
    textElem.appendChild( outdoc.createTextNode( text ) );
}
