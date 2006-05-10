/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <config.h>

#include <kgenericfactory.h>
#include <KoStoreDevice.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <kprkword.h>
#include <klocale.h>
#include <kdebug.h>
#include <q3sortedlist.h>
#include <QColor>
//Added by qt3to4:
#include <Q3CString>

typedef KGenericFactory<KprKword> KprKwordFactory;
K_EXPORT_COMPONENT_FACTORY( libkprkword, KprKwordFactory( "kofficefilters" ) )

KprKword::KprKword(KoFilter *, const char *, const QStringList&) :
    KoFilter(parent),
    outdoc( "DOC" )
{
}

// This filter can act as an import filter for KWord and as an export
// filter for KPresenter (isn't our architecture really nice ? :)
// This is why we use the file-to-file method, not a QDomDoc one.
KoFilter::ConversionStatus KprKword::convert( const QByteArray& from, const QByteArray& to )
{
    if(to!="application/x-kword" || from!="application/x-kpresenter")
        return KoFilter::NotImplemented;

    KoStoreDevice* inpdev = m_chain->storageFile( "root", KoStore::Read );
    if ( !inpdev )
    {
        kError(30502) << "Unable to open input stream" << endl;
        return KoFilter::StorageCreationError;
    }

    inpdoc.setContent( inpdev );


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
    counter.setAttribute( "righttext", "." );

    QDomElement format = outdoc.createElement( "FORMAT" );
    style.appendChild( format );
    QDomElement font = outdoc.createElement( "FONT" );
    format.appendChild( font );
    font.setAttribute( "name", titleFont ); // found when reading the first title
    QDomElement size = outdoc.createElement( "SIZE" );
    format.appendChild( size );
    size.setAttribute( "value", 24 );
    QDomElement bold = outdoc.createElement( "WEIGHT" );
    format.appendChild( bold );
    bold.setAttribute( "value", 75 );

    // Create the standard style
    style = outdoc.createElement( "STYLE" );
    styles.appendChild( style );
    elem = outdoc.createElement( "NAME" );
    style.appendChild( elem );
    elem.setAttribute( "value", "Standard" ); // no i18n here!
    format = outdoc.createElement( "FORMAT" );
    style.appendChild( format ); // empty format == use defaults

    // Write output file

    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if(!out) {
        kError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    Q3CString cstring = outdoc.toCString(); // utf-8 already
    out->write( cstring.data(), cstring.length() );
    return KoFilter::OK;
}

// This class is used to sort the objects by y position
class KprObject {
 public:
    double y;
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

    Q3SortedList< KprObject > objList;
    objList.setAutoDelete( true );

    QDomNodeList lst = objects.elementsByTagName( "OBJECT" );
    uint lstcount = lst.count();
    for ( uint item = 0 ; item < lstcount ; ++item )
    {
        QDomElement object = lst.item( item ).toElement();
        if ( object.attribute( "type" ).toInt() == 4 ) // we only care about text objs
        {
            QDomElement orig = object.namedItem( "ORIG" ).toElement();
            if ( !orig.isNull() )
            {
                KprObject * obj = new KprObject;
                obj->y = orig.attribute( "y" ).toDouble();
                obj->elem = object;
                objList.inSort( obj );
            }
        }
    }

    int curPage = -1;
    //kDebug() << "found " << objList.count() << " objects" << endl;

    for ( Q3PtrListIterator<KprObject> it(objList); it.current(); ++it )
    {
        QDomElement elem = it.current()->elem;
        // Detect the first object of each page
        int page = int( it.current()->y / ptPageHeight );
        bool isTitle = ( page > curPage );
        //kDebug() << "KprKword::convert y=" << it.current()->y << " ptPageHeight=" << ptPageHeight
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

            QDomElement counter = p.namedItem( "COUNTER" ).toElement();
            QDomElement indent=p.namedItem("INDENTS").toElement();
            QDomElement lineSpacing=p.namedItem( "LINESPACING" ).toElement();
            QDomElement offset=p.namedItem("OFFSETS").toElement();
            QDomElement leftBorder = p.namedItem( "LEFTBORDER" ).toElement();
            QDomElement rightBorder = p.namedItem( "RIGHTBORDER" ).toElement();
            QDomElement topBorder = p.namedItem( "TOPBORDER" ).toElement();
            QDomElement bottomBorder = p.namedItem( "BOTTOMBORDER" ).toElement();

            QDomElement shadow=p.namedItem("SHADOW").toElement();

            for ( ; !textElem.isNull() ; textElem = textElem.nextSibling().toElement() )
            {
                int oldLen = text.length();
                text += textElem.text();
                //kDebug() << "KprKword::convert text now " << text << endl;
                QDomElement outFormatElem = outdoc.createElement( "FORMAT" );

                if ( textElem.attribute( "italic" ).toInt() )
                {
                    QDomElement e = outdoc.createElement("ITALIC");
                    e.setAttribute( "value", 1 );
                    outFormatElem.appendChild( e );
                }
                QColor underlineColor;
                if ( textElem.hasAttribute("underlinecolor" ))
                {
                    underlineColor =QColor(textElem.attribute("underlinecolor" ));
                }
                QString underlineStyleLine;
                if ( textElem.hasAttribute("underlinestyleline"))
                {
                    underlineStyleLine = textElem.attribute("underlinestyleline");
                }
                if ( textElem.hasAttribute("underline" ))
                {
                    QDomElement e = outdoc.createElement("UNDERLINE");
                    QString value = textElem.attribute( "underline" );
                     if ( value == "double" )
                     {
                         e.setAttribute( "value", "double" );
                     }
                     else if ( value == "single" )
                     {
                         e.setAttribute( "value", "double" );
                     }
                     else
                     {
                         e.setAttribute( "value", (bool)value.toInt() ? "1" :"0" );
                     }
                     if ( underlineColor.isValid())
                     {
                         e.setAttribute("underlinecolor", underlineColor.name());
                     }
                     if ( !underlineStyleLine.isEmpty() )
                         e.setAttribute("styleline", underlineStyleLine);
                     outFormatElem.appendChild( e );

                }

                QString strikeOutStyleLine;
                if ( textElem.hasAttribute("strikeoutstyleline"))
                {
                    strikeOutStyleLine = textElem.attribute("strikeoutstyleline");
                }
                QString strikeOutValue;
                if ( textElem.hasAttribute("strikeOut"))
                {
                    strikeOutValue = textElem.attribute("strikeOut");
                }

                if( !strikeOutValue.isEmpty())
                {
                    QDomElement e = outdoc.createElement("STRIKEOUT");
                    e.setAttribute( "value", strikeOutValue );
                    if ( !strikeOutStyleLine.isEmpty())
                        e.setAttribute("styleline", strikeOutStyleLine);
                    outFormatElem.appendChild( e );
                }
                /*if ( textElem.attribute( "bold" ).toInt() )
                {
                    QDomElement e = outdoc.createElement("WEIGHT");
                    e.setAttribute( "value", 75 );
                    outFormatElem.appendChild( e );
                }*/ // doesn't look good
                if ( titleFont.isEmpty() && isTitle )
                    titleFont = textElem.attribute( "family" );

                // Family and point size are voluntarily NOT passed over.
                if ( !textElem.attribute( "color" ).isEmpty())
                {
                    QColor col;
                    col.setNamedColor(textElem.attribute( "color" ));
                    QDomElement e = outdoc.createElement("COLOR");
                    e.setAttribute( "red", col.Qt::red() );
                    e.setAttribute( "green", col.Qt::green() );
                    e.setAttribute( "blue", col.Qt::blue() );
                    outFormatElem.appendChild( e );
                }
                if ( !textElem.attribute("textbackcolor").isEmpty())
                {
                    QColor col;
                    col.setNamedColor(textElem.attribute( "textbackcolor" ));
                    QDomElement e = outdoc.createElement("TEXTBACKGROUNDCOLOR");
                    e.setAttribute( "red", col.Qt::red() );
                    e.setAttribute( "green", col.Qt::green() );
                    e.setAttribute( "blue", col.Qt::blue() );
                    outFormatElem.appendChild( e );
                }

                //before VERTICAL align
                double relative = 0;
                if( textElem.attribute("relativetextsize").toDouble())
                {
                    relative = textElem.attribute("relativetextsize").toDouble();
                }


                if( textElem.attribute("VERTALIGN").toInt())
                {
                    QDomElement e = outdoc.createElement("VERTALIGN");
                    e.setAttribute( "value", textElem.attribute("VERTALIGN").toInt() );
                    if ( relative != 0)
                        e.setAttribute( "relativetextsize", relative );
                    outFormatElem.appendChild( e );
                }

                if( textElem.hasAttribute("shadowtext"))
                {
                    QDomElement e = outdoc.createElement("SHADOWTEXT");
                    e.setAttribute( "value", textElem.attribute("shadowtext").toInt() );
                    outFormatElem.appendChild( e );
                }

                if( textElem.hasAttribute("offsetfrombaseline"))
                {
                    QDomElement e = outdoc.createElement("OFFSETFROMBASELINE");
                    e.setAttribute( "value", textElem.attribute("offsetfrombaseline").toInt() );
                    outFormatElem.appendChild( e );
                }

                if( textElem.hasAttribute("wordbyword"))
                {
                    QDomElement e = outdoc.createElement("WORDBYWORD");
                    e.setAttribute( "value", textElem.attribute("wordbyword").toInt() );
                    outFormatElem.appendChild( e );
                }

                if( textElem.hasAttribute("fontattribute"))
                {
                    QDomElement e = outdoc.createElement("FONTATTRIBUTE");
                    e.setAttribute( "value", textElem.attribute("fontattribute") );
                    outFormatElem.appendChild( e );
                }
                if( textElem.hasAttribute("language"))
                {
                    QDomElement e = outdoc.createElement("LANGUAGE");
                    e.setAttribute( "value", textElem.attribute("language") );
                    outFormatElem.appendChild( e );
                }
                if ( !outFormatElem.firstChild().isNull() )
                {
                    outFormatElem.setAttribute( "id", 1 ); // normal exte
                    outFormatElem.setAttribute( "pos", oldLen );
                    outFormatElem.setAttribute( "len", text.length() - oldLen );
                    outFormatsElem.appendChild( outFormatElem );
                }

            } // end "for each text element"

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
            nameElem.setAttribute( "value", isTitle ? titleStyleName : QString("Standard") );
            QDomElement align=outdoc.createElement("FLOW");
            if(p.hasAttribute("align"))
            {
                switch(p.attribute( "align" ).toInt())
                {
                case 1:
                    align.setAttribute( "align","left");
                    break;
                case 2:
                    align.setAttribute( "align","right");
                    break;
                case 4:
                    align.setAttribute( "align","center");
                    break;
                case 8:
                    align.setAttribute( "align","justify");
                    break;
                }
            }
            if(!counter.isNull() )
                layoutElem.appendChild( counter );
            if(!indent.isNull())
                layoutElem.appendChild( indent );
            if(!lineSpacing.isNull())
                layoutElem.appendChild( lineSpacing );
            if(!offset.isNull())
                layoutElem.appendChild( offset);
            if(!leftBorder.isNull())
                layoutElem.appendChild(leftBorder);
            if(!rightBorder.isNull())
                layoutElem.appendChild(rightBorder);
            if(!topBorder.isNull())
                layoutElem.appendChild(topBorder);
            if(!bottomBorder.isNull())
                layoutElem.appendChild(bottomBorder);
            if(!align.isNull())
                layoutElem.appendChild(align);
            if(!shadow.isNull())
                layoutElem.appendChild(shadow);
            // Only the first parag of the top text object is set to the 'title' style
            isTitle = false;
        }
    }
}

#include <kprkword.moc>
