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
#include <koGlobal.h>
#include "kprkword.h"
#include "kprkword.moc"
#include <klocale.h>
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
    QDomElement frame = outdoc.createElement( "FRAME" );
    frameset.appendChild( frame );
    frame.setAttribute( "left", 28 );
    frame.setAttribute( "top", 42 );
    frame.setAttribute( "right", 566 );
    frame.setAttribute( "bottom", 798 );
    frame.setAttribute( "autoCreateNewFrame", 1 );
    frame.setAttribute( "newFrameBehaviour", 0 );

    titleStyleName = i18n("Slide Title");

    // Convert !

    convert();

    // Create a style for the slide titles

    QDomElement styles = outdoc.createElement( "STYLES" );
    kwdoc.appendChild( styles );

    QDomElement style = outdoc.createElement( "STYLE" );
    styles.appendChild( style );
    QDomElement elem = outdoc.createElement( "NAME" );
    style.appendChild( elem );
    elem.setAttribute( "value", titleStyleName );
    elem = outdoc.createElement( "FOLLOWING" );
    style.appendChild( elem );
    elem.setAttribute( "name", "Standard" ); // no i18n here!

    QDomElement counter = outdoc.createElement( "COUNTER" );
    style.appendChild( counter );
    counter.setAttribute( "type", 1 ); // numbered
    counter.setAttribute( "depth", 0 );
    counter.setAttribute( "start", 1 );
    counter.setAttribute( "numberingtype", 1 ); // chapter
    counter.setAttribute( "righttext", ". " );

    QDomElement format = outdoc.createElement( "FORMAT" );
    style.appendChild( format );
    QDomElement font = outdoc.createElement( "FONT" );
    format.appendChild( font );
    // by picking up a font from the kpr doc we ensure that it's installed, at least
    font.setAttribute( "name", aFont );
    QDomElement size = outdoc.createElement( "SIZE" );
    format.appendChild( size );
    size.setAttribute( "value", 24 );
    QDomElement bold = outdoc.createElement( "WEIGHT" );
    format.appendChild( bold );
    bold.setAttribute( "value", 75 );

    // Write output file

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

void KprKword::convert()
{
    QDomElement docElem = inpdoc.documentElement();
    QDomElement paper = docElem.namedItem( "PAPER" ).toElement();
    int ptPageHeight = paper.attribute( "ptHeight" ).toInt();

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

    int curPage = -1;

    for ( QListIterator<KprObject> it(objList); it.current(); ++it )
    {
        QDomElement elem = it.current()->elem;
        // Detect the first object of each page
        int page = it.current()->y / ptPageHeight;
        bool isTitle = ( page > curPage );
        //kdDebug() << "KprKword::convert y=" << it.current()->y << " ptPageHeight=" << ptPageHeight
        //          << " isTitle=" << isTitle << endl;
        curPage = page;

        QDomElement textObj = elem.namedItem( "TEXTOBJ" ).toElement();
        if (textObj.isNull())
            continue;
        // For each paragraph in this text object...
        QDomNodeList lst = textObj.elementsByTagName( "P" );
        uint lstcount = lst.count();
        for ( uint item = 0; item < lstcount ; ++item )
        {
            QDomElement p = lst.item( item ).toElement();

            // Create paragraph in KWord doc
            QDomElement parag = outdoc.createElement( "PARAGRAPH" );
            frameset.appendChild( parag );

            QDomElement outFormatsElem = outdoc.createElement( "FORMATS" );

            QString text;
            // For each text element in the paragraph...
            QDomElement textElem = p.firstChild().toElement();
            for ( ; !textElem.isNull() ; textElem = textElem.nextSibling().toElement() )
            {
                int oldLen = text.length();
                text += textElem.text();
                //kdDebug() << "KprKword::convert text now " << text << endl;
                QDomElement outFormatElem = outdoc.createElement( "FORMAT" );

                if ( textElem.attribute( "italic" ).toInt() )
                {
                    QDomElement e = outdoc.createElement("ITALIC");
                    e.setAttribute( "value", 1 );
                    outFormatElem.appendChild( e );
                }
                if ( textElem.attribute( "underline" ).toInt() )
                {
                    QDomElement e = outdoc.createElement("UNDERLINE");
                    e.setAttribute( "value", 1 );
                    outFormatElem.appendChild( e );
                }
                /*if ( textElem.attribute( "bold" ).toInt() )
                {
                    QDomElement e = outdoc.createElement("WEIGHT");
                    e.setAttribute( "value", 75 );
                    outFormatElem.appendChild( e );
                }*/ // doesn't look good
                if ( aFont.isEmpty() )
                    aFont = textElem.attribute( "family" );

                // Family, colour and point size are voluntarily NOT passed over.

                if ( !outFormatElem.firstChild().isNull() )
                {
                    outFormatElem.setAttribute( "id", 1 ); // normal exte
                    outFormatElem.setAttribute( "pos", oldLen );
                    outFormatElem.setAttribute( "len", text.length() - oldLen );
                    outFormatsElem.appendChild( outFormatElem );
                }

            }
            // KPresenter seems to save a trailing space (bug!)
            int len = text.length();
            if ( len > 0 && text[ len - 1 ] == ' ' )
                text.truncate( len - 1 );

            QDomElement outTextElem = outdoc.createElement( "TEXT" );
            parag.appendChild( outTextElem );
            outTextElem.appendChild( outdoc.createTextNode( text ) );

            if ( !outFormatsElem.firstChild().isNull() ) // Do we have formats to save ?
                parag.appendChild( outFormatsElem );

            QDomElement layoutElem = outdoc.createElement( "LAYOUT" );
            parag.appendChild( layoutElem );
            QDomElement nameElem = outdoc.createElement( "NAME" );
            layoutElem.appendChild( nameElem );
            nameElem.setAttribute( "value", isTitle ? titleStyleName : "Standard" );

            // Only the first parag of the top text object is set to the 'title' style
            isTitle = false;
        }
    }
}
