/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#include "ooimpressimport.h"

#include <math.h>

#include <qregexp.h>
#include <qdatetime.h>

#include <kzip.h>
#include <karchive.h>
#include <kdebug.h>
#include <koUnit.h>
#include <koDocumentInfo.h>
#include <koDocument.h>

#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koGlobal.h>

typedef KGenericFactory<OoImpressImport, KoFilter> OoImpressImportFactory;
K_EXPORT_COMPONENT_FACTORY( libooimpressimport, OoImpressImportFactory( "ooimpressimport" ) );


OoImpressImport::OoImpressImport( KoFilter *, const char *, const QStringList & )
    : KoFilter(),
      m_numPicture( 1 ),
      m_styles( 23, true )
{
    m_styles.setAutoDelete( true );
}

OoImpressImport::~OoImpressImport()
{
}

KoFilter::ConversionStatus OoImpressImport::convert( QCString const & from, QCString const & to )
{
    kdDebug() << "Entering Ooimpress Import filter: " << from << " - " << to << endl;

    if ( from != "application/vnd.sun.xml.impress" || to != "application/x-kpresenter" )
    {
        kdWarning() << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    KoFilter::ConversionStatus preStatus = openFile();

    if ( preStatus != KoFilter::OK )
        return preStatus;

    QDomDocument docinfo;
    createDocumentInfo( docinfo );

    // store document info
    KoStoreDevice* out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if( out )
    {
        QCString info = docinfo.toCString();
        //kdDebug() << " info :" << info << endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->writeBlock( info , info.length() );
    }

    QDomDocument doccontent;
    createDocumentContent( doccontent );

    // store document content
    out = m_chain->storageFile( "maindoc.xml", KoStore::Write );
    if( out )
    {
        QCString content = doccontent.toCString();
        //kdDebug() << " content :" << content << endl;
        out->writeBlock( content , content.length() );
    }

    kdDebug() << "######################## OoImpressImport::convert done ####################" << endl;
    return KoFilter::OK;
}

KoFilter::ConversionStatus OoImpressImport::openFile()
{
  KoStore * store = KoStore::createStore( m_chain->inputFile(), KoStore::Read);

  if ( !store )
  {
    kdWarning() << "Couldn't open the requested file." << endl;
    return KoFilter::FileNotFound;
  }

  if ( !store->open( "content.xml" ) )
  {
    kdWarning() << "This file doesn't seem to be a valid OoImpress file" << endl;
    delete store;
    return KoFilter::WrongFormat;
  }

  QDomDocument styles;

  m_content.setContent( store->device() );
  store->close();

  //kdDebug() << "m_content.toCString() :" << m_content.toCString() << endl;
  kdDebug() << "File containing content loaded " << endl;

  if ( store->open( "styles.xml" ) )
  {
    styles.setContent( store->device() );
    store->close();

    //kdDebug() << "styles.toCString() :" << styles.toCString() << endl;
    kdDebug() << "File containing styles loaded" << endl;
  }
  else
    kdWarning() << "Style definitions do not exist!" << endl;

  if ( store->open( "meta.xml" ) )
  {
    m_meta.setContent( store->device() );
    store->close();

    kdDebug() << "File containing meta definitions loaded" << endl;
  }
  else
    kdWarning() << "Meta definitions do not exist!" << endl;

  if ( store->open( "settings.xml" ) )
  {
    m_settings.setContent( store->device() );
    store->close();

    kdDebug() << "File containing settings loaded" << endl;
  }
  else
    kdWarning() << "Settings do not exist!" << endl;

  delete store;

  emit sigProgress( 10 );
  createStyleMap( styles );

  return KoFilter::OK;
}

void OoImpressImport::createDocumentInfo( QDomDocument &docinfo )
{
    docinfo.appendChild( docinfo.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomDocument doc = KoDocument::createDomDocument( "document-info" /*DTD name*/, "document-info" /*tag name*/, "1.1" );

    QDomNode meta   = m_meta.namedItem( "office:document-meta" );
    QDomNode office = meta.namedItem( "office:meta" );

    if ( office.isNull() )
        return;
    QDomElement elementDocInfo  = doc.documentElement();

    QDomElement e = office.namedItem( "dc:creator" ).toElement();
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        QDomElement author = doc.createElement( "author" );
        QDomElement t = doc.createElement( "full-name" );
        author.appendChild( t );
        t.appendChild( doc.createTextNode( e.text() ) );
        elementDocInfo.appendChild( author);
    }

    e = office.namedItem( "dc:title" ).toElement();
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        QDomElement about = doc.createElement( "about" );
        QDomElement title = doc.createElement( "title" );
        about.appendChild( title );
        title.appendChild( doc.createTextNode( e.text() ) );
        elementDocInfo.appendChild( about );
    }
#if 0
    e = office.namedItem( "dc:description" ).toElement();
    if ( !e.isNull() && !e.text().isEmpty() )
    {

    }
#endif
    docinfo.appendChild(doc);

    //kdDebug() << " meta-info :" << m_meta.toCString() << endl;
}

void OoImpressImport::createDocumentContent( QDomDocument &doccontent )
{
    QDomDocument doc = KoDocument::createDomDocument( "kpresenter", "DOC", "1.2" );
    QDomElement docElement = doc.documentElement();
    docElement.setAttribute( "editor", "KPresenter" );
    docElement.setAttribute( "mime", "application/x-kpresenter" );
    docElement.setAttribute( "syntaxVersion", "2" );

    QDomElement content = m_content.documentElement();

    // content.xml contains some automatic-styles that we need to store
    QDomNode automaticStyles = content.namedItem( "office:automatic-styles" );
    if ( !automaticStyles.isNull() )
        insertStyles( automaticStyles.toElement() );

    QDomNode body = content.namedItem( "office:body" );
    if ( body.isNull() )
        return;

    // it seems that ooimpress has different paper-settings for every slide.
    // we take the settings of the first slide for the whole document.
    QDomNode drawPage = body.namedItem( "draw:page" );
    if ( drawPage.isNull() ) // no slides? give up.
        return;

    QDomElement dp = drawPage.toElement();
    QDomElement *master = m_styles[dp.attribute( "draw:master-page-name" )];
    QDomElement *style = m_styles[master->attribute( "style:page-master-name" )];
    QDomElement properties = style->namedItem( "style:properties" ).toElement();

    double pageHeight;
    QDomElement paperElement = doc.createElement( "PAPER" );
    if ( properties.isNull() )
    {
        paperElement.setAttribute( "ptWidth", CM_TO_POINT(28) );
        paperElement.setAttribute( "ptHeight", CM_TO_POINT(21) );
        paperElement.setAttribute( "unit", 0 );
        paperElement.setAttribute( "format", 5 );
        paperElement.setAttribute( "tabStopValue", 42.5198 );
        paperElement.setAttribute( "orientation", 0 );
        pageHeight = 21;

        QDomElement paperBorderElement = doc.createElement( "PAPERBORDERS" );
        paperBorderElement.setAttribute( "ptRight", 0 );
        paperBorderElement.setAttribute( "ptBottom", 0 );
        paperBorderElement.setAttribute( "ptLeft", 0 );
        paperBorderElement.setAttribute( "ptTop", 0 );
        paperElement.appendChild( paperBorderElement );
    }
    else
    {
        paperElement.setAttribute( "ptWidth", toPoint(properties.attribute( "fo:page-width" ) ) );
        paperElement.setAttribute( "ptHeight", toPoint(properties.attribute( "fo:page-height" ) ) );
//         paperElement.setAttribute( "unit", 0 );
//         paperElement.setAttribute( "format", 5 );
//         paperElement.setAttribute( "tabStopValue", 42.5198 );
//         paperElement.setAttribute( "orientation", 0 );
        // Keep pageHeight in cm to avoid rounding-errors that would
        // get multiplied with every new slide.
        pageHeight = properties.attribute( "fo:page-height" ).remove( "cm" ).toDouble();

        QDomElement paperBorderElement = doc.createElement( "PAPERBORDERS" );
        paperBorderElement.setAttribute( "ptRight", toPoint( properties.attribute( "fo:margin-right" ) ) );
        paperBorderElement.setAttribute( "ptBottom", toPoint( properties.attribute( "fo:margin-bottom" ) ) );
        paperBorderElement.setAttribute( "ptLeft", toPoint( properties.attribute( "fo:page-left" ) ) );
        paperBorderElement.setAttribute( "ptTop", toPoint( properties.attribute( "fo:page-top" ) ) );
        paperElement.appendChild( paperBorderElement );
    }

    QDomElement objectElement = doc.createElement( "OBJECTS" );
    QDomElement pictureElement = doc.createElement( "PICTURES" );
    QDomElement pageTitleElement = doc.createElement( "PAGETITLES" );
    QDomElement pageNoteElement = doc.createElement( "PAGENOTES" );
    QDomElement backgroundElement = doc.createElement( "BACKGROUND" );

    // parse all pages
    for ( drawPage = body.firstChild(); !drawPage.isNull(); drawPage = drawPage.nextSibling() )
    {
        dp = drawPage.toElement();
        m_styleStack.clear(); // remove all styles
        fillStyleStack( dp );
        m_styleStack.setPageMark();

        // take care of a possible page background
        if ( m_styleStack.hasAttribute( "draw:fill" ) )
        {
            QDomElement bgPage = doc.createElement( "PAGE" );
            QString fill = m_styleStack.attribute( "draw:fill" );
            if ( fill == "solid" )
            {
                QDomElement backColor1 = doc.createElement( "BACKCOLOR1" );
                backColor1.setAttribute( "color", m_styleStack.attribute( "draw:fill-color" ) );
                bgPage.appendChild( backColor1 );

                QDomElement bcType = doc.createElement( "BCTYPE" );
                bcType.setAttribute( "value", 0 ); // plain
                bgPage.appendChild( bcType );

                QDomElement backType = doc.createElement( "BACKTYPE" );
                backType.setAttribute( "value", 0 ); // color/gradient
                bgPage.appendChild( backType );

                backgroundElement.appendChild( bgPage );
            }
            else if ( fill == "gradient" )
            {
                QDomElement bgPage = doc.createElement( "PAGE" );
                QString style = m_styleStack.attribute( "draw:fill-gradient-name" );
                QDomElement* draw = m_draws[style];
                appendBackgroundGradient( doc, bgPage, *draw );

                backgroundElement.appendChild( bgPage );
            }
            else if ( fill == "bitmap" )
            {
                QDomElement bgPage = doc.createElement( "PAGE" );
                QString style = m_styleStack.attribute( "draw:fill-image-name" );
                QDomElement* draw = m_draws[style];
                appendBackgroundImage( doc, bgPage, pictureElement, *draw );

                QDomElement backView = doc.createElement( "BACKVIEW" );
                if ( m_styleStack.hasAttribute( "style:repeat" ) )
                {
                    QString repeat = m_styleStack.attribute( "style:repeat" );
                    if ( repeat == "stretch" )
                        backView.setAttribute( "value", 0 ); // zoomed
                    else if ( repeat == "no-repeat" )
                        backView.setAttribute( "value", 1 ); // centered
                    else
                        backView.setAttribute( "value", 2 ); // use tiled as default
                }
                else
                    backView.setAttribute( "value", 2 ); // use tiled as default
                bgPage.appendChild( backView );

                QDomElement backType = doc.createElement( "BACKTYPE" );
                backType.setAttribute( "value", 1 ); // image
                bgPage.appendChild( backType );

                backgroundElement.appendChild( bgPage );
            }
        }

        // set the pagetitle
        QDomElement titleElement = doc.createElement( "Title" );
        titleElement.setAttribute( "title", dp.attribute( "draw:name" ) );
        pageTitleElement.appendChild( titleElement );

        // The '+1' is necessary to avoid that objects that start on the first line
        // of a slide will show up on the last line of the previous slide.
        double offset = CM_TO_POINT( ( dp.attribute( "draw:id" ).toInt() - 1 ) * pageHeight ) + 1;

        // parse all objects
        for ( QDomNode object = drawPage.firstChild(); !object.isNull(); object = object.nextSibling() )
        {
            QDomElement o = object.toElement();
            QString name = o.tagName();

            QDomElement e;
            if ( name == "draw:text-box" ) // textbox
            {
                storeObjectStyles( o );
                e = doc.createElement( "OBJECT" );
                e.setAttribute( "type", 4 );
                append2DGeometry( doc, e, o, (int)offset );
                appendPen( doc, e );
                appendBrush( doc, e );
                appendRounding( doc, e, o );
                appendShadow( doc, e );
                e.appendChild( parseTextBox( doc, o ) );
            }
            else if ( name == "draw:rect" ) // rectangle
            {
                storeObjectStyles( o );
                e = doc.createElement( "OBJECT" );
                e.setAttribute( "type", 2 );
                append2DGeometry( doc, e, o, (int)offset );
                appendPen( doc, e );
                appendBrush( doc, e );
                appendRounding( doc, e, o );
                appendShadow( doc, e );
            }
            else if ( name == "draw:circle" || name == "draw:ellipse" )
            {
                storeObjectStyles( o );
                e = doc.createElement( "OBJECT" );
                append2DGeometry( doc, e, o, (int)offset );
                appendPen( doc, e );
                appendShadow( doc, e );

                if ( o.hasAttribute( "draw:kind" ) ) // pie, chord or arc
                {
                    e.setAttribute( "type", 8 );
                    appendPie( doc, e, o );
                    QDomElement type = doc.createElement( "PIETYPE" );

                    QString kind = o.attribute( "draw:kind" );
                    if ( kind == "section" )
                    {
                        appendBrush( doc, e );
                        type.setAttribute( "value", 0 );
                    }
                    else if ( kind == "cut" )
                    {
                        appendBrush( doc, e );
                        type.setAttribute( "value", 2 );
                    }
                    else if ( kind == "arc" )
                    {
                        // arc has no brush
                        type.setAttribute( "value", 1 );
                    }
                    e.appendChild( type );
                }
                else  // circle or ellipse
                {
                    e.setAttribute( "type", 3 );
                    appendBrush( doc, e );
                }
            }
            else if ( name == "draw:line" ) // line
            {
                storeObjectStyles( o );
                e = doc.createElement( "OBJECT" );
                e.setAttribute( "type", 1 );
                appendLineGeometry( doc, e, o, (int)offset );
                appendPen( doc, e );
                appendBrush( doc, e );
                appendShadow( doc, e );
                appendLineEnds( doc, e );
            }
            else if ( name == "draw:image" ) // image
            {
                storeObjectStyles( o );
                e = doc.createElement( "OBJECT" );
                e.setAttribute( "type", 0 );
                append2DGeometry( doc, e, o, (int)offset );
                appendImage( doc, e, pictureElement, o );
            }
            else if ( name == "presentation:notes" ) // notes
            {
                QDomNode textBox = o.namedItem( "draw:text-box" );
                if ( !textBox.isNull() )
                {
                    QString note;
                    for ( QDomNode text = textBox.firstChild(); !text.isNull(); text = text.nextSibling() )
                    {
                        // We don't care about styles as they are not supported in kpresenter.
                        // Only add a linebreak for every child.
                        QDomElement t = text.toElement();
                        note += t.text() + "\n";
                    }
                    QDomElement notesElement = doc.createElement( "Note" );
                    notesElement.setAttribute( "note", note );
                    pageNoteElement.appendChild( notesElement );
                }
            }
            else
            {
                kdDebug() << "Unsupported object '" << name << "'" << endl;
                continue;
            }

            objectElement.appendChild( e );
        }
    }

    docElement.appendChild( paperElement );
    docElement.appendChild( backgroundElement );
    docElement.appendChild( pageTitleElement );
    docElement.appendChild( pageNoteElement );
    docElement.appendChild( objectElement );
    docElement.appendChild( pictureElement );
    doccontent.appendChild( doc );
}

void OoImpressImport::append2DGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset )
{
    QDomElement orig = doc.createElement( "ORIG" );
    orig.setAttribute( "x", toPoint( object.attribute( "svg:x" ) ) );
    orig.setAttribute( "y", toPoint( object.attribute( "svg:y" ) ) + offset );
    e.appendChild( orig );

    QDomElement size = doc.createElement( "SIZE" );
    size.setAttribute( "width", toPoint( object.attribute( "svg:width" ) ) );
    size.setAttribute( "height", toPoint( object.attribute( "svg:height" ) ) );
    e.appendChild( size );
}

void OoImpressImport::appendLineGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset )
{
    double x1 = toPoint( object.attribute( "svg:x1" ) );
    double y1 = toPoint( object.attribute( "svg:y1" ) );
    double x2 = toPoint( object.attribute( "svg:x2" ) );
    double y2 = toPoint( object.attribute( "svg:y2" ) );

    double x = QMIN( x1, x2 );
    double y = QMIN( y1, y2 );

    QDomElement orig = doc.createElement( "ORIG" );
    orig.setAttribute( "x", x );
    orig.setAttribute( "y", y + offset );
    e.appendChild( orig );

    QDomElement size = doc.createElement( "SIZE" );
    size.setAttribute( "width", fabs( x1 - x2 ) );
    size.setAttribute( "height", fabs( y1 - y2 ) );
    e.appendChild( size );

    QDomElement linetype = doc.createElement( "LINETYPE" );
    if ( ( x1 < x2 && y1 < y2 ) || ( x1 > x2 && y1 > y2 ) )
        linetype.setAttribute( "value", 2 );
    else
        linetype.setAttribute( "value", 3 );

    e.appendChild( linetype );
}

void OoImpressImport::appendPen( QDomDocument& doc, QDomElement& e )
{
    if ( m_styleStack.hasAttribute( "draw:stroke" ))
    {
        QDomElement pen = doc.createElement( "PEN" );
        if ( m_styleStack.attribute( "draw:stroke" ) == "none" )
            pen.setAttribute( "style", 0 );
        else if ( m_styleStack.attribute( "draw:stroke" ) == "solid" )
            pen.setAttribute( "style", 1 );
        else if ( m_styleStack.attribute( "draw:stroke" ) == "dash" )
        {
            QString style = m_styleStack.attribute( "draw:stroke-dash" );
            if ( style == "Ultrafine Dashed" || style == "Fine Dashed" ||
                 style == "Fine Dashed (var)" || style == "Dashed (var)" )
                pen.setAttribute( "style", 2 );
            else if ( style == "Fine Dotted" || style == "Ultrafine Dotted (var)" ||
                      style == "Line with Fine Dots" )
                pen.setAttribute( "style", 3 );
            else if ( style == "3 Dashes 3 Dots (var)" || style == "Ultrafine 2 Dots 3 Dashes" )
                pen.setAttribute( "style", 4 );
            else if ( style == "2 Dots 1 Dash" )
                pen.setAttribute( "style", 5 );
        }

        if ( m_styleStack.hasAttribute( "svg:stroke-width" ) )
            pen.setAttribute( "width", (int) toPoint( m_styleStack.attribute( "svg:stroke-width" ) ) );
        if ( m_styleStack.hasAttribute( "svg:stroke-color" ) )
            pen.setAttribute( "color", m_styleStack.attribute( "svg:stroke-color" ) );
        e.appendChild( pen );
    }
}

void OoImpressImport::appendBrush( QDomDocument& doc, QDomElement& e )
{
    if ( m_styleStack.hasAttribute( "draw:fill" ) )
    {
        if ( m_styleStack.attribute( "draw:fill" ) == "solid" )
        {
            QDomElement brush = doc.createElement( "BRUSH" );
            brush.setAttribute( "style", 1 );
            if ( m_styleStack.hasAttribute( "draw:fill-color" ) )
                brush.setAttribute( "color", m_styleStack.attribute( "draw:fill-color" ) );
            e.appendChild( brush );
        }
        else if ( m_styleStack.attribute( "draw:fill" ) == "hatch" )
        {
            QDomElement brush = doc.createElement( "BRUSH" );
            QString style = m_styleStack.attribute( "draw:fill-hatch-name" );
            if ( style == "Black 0 Degrees" )
                brush.setAttribute( "style", 9 );
            else if ( style == "Black 90 Degrees" )
                brush.setAttribute( "style", 10 );
            else if ( style == "Red Crossed 0 Degrees" || style == "Blue Crossed 0 Degrees" )
                brush.setAttribute( "style", 11 );
            else if ( style == "Black 45 Degrees" || style == "Black 45 Degrees Wide" )
                brush.setAttribute( "style", 12 );
            else if ( style == "Black -45 Degrees" )
                brush.setAttribute( "style", 13 );
            else if ( style == "Red Crossed 45 Degrees" || style == "Blue Crossed 45 Degrees" )
                brush.setAttribute( "style", 14 );

            QDomElement* draw = m_draws[style];
            if ( draw && draw->hasAttribute( "draw:color" ) )
                brush.setAttribute( "color", draw->attribute( "draw:color" ) );
            e.appendChild( brush );
        }
        else if ( m_styleStack.attribute( "draw:fill" ) == "gradient" )
        {
            QDomElement gradient = doc.createElement( "GRADIENT" );
            QString style = m_styleStack.attribute( "draw:fill-gradient-name" );

            QDomElement* draw = m_draws[style];
            if ( draw )
            {
                gradient.setAttribute( "color1", draw->attribute( "draw:start-color" ) );
                gradient.setAttribute( "color2", draw->attribute( "draw:end-color" ) );

                QString type = draw->attribute( "draw:style" );
                if ( type == "linear" )
                {
                    int angle = draw->attribute( "draw:angle" ).toInt() / 10;

                    // make sure the angle is between 0 and 359
                    angle = abs( angle );
                    angle -= ( (int) ( angle / 360 ) ) * 360;

                    // What we are trying to do here is to find out if the given
                    // angle belongs to a horizontal, vertical or diagonal gradient.
                    int lower, upper, nearAngle = 0;
                    for ( lower = 0, upper = 45; upper < 360; lower += 45, upper += 45 )
                    {
                        if ( upper >= angle )
                        {
                            int distanceToUpper = abs( angle - upper );
                            int distanceToLower = abs( angle - lower );
                            nearAngle = distanceToUpper > distanceToLower ? lower : upper;
                            break;
                        }
                    }

                    // nearAngle should now be one of: 0, 45, 90, 135, 180...
                    if ( nearAngle == 0 || nearAngle == 180 )
                        gradient.setAttribute( "type", 1 ); // horizontal
                    else if ( nearAngle == 90 || nearAngle == 270 )
                        gradient.setAttribute( "type", 2 ); // vertical
                    else if ( nearAngle == 45 || nearAngle == 225 )
                        gradient.setAttribute( "type", 3 ); // diagonal 1
                    else if ( nearAngle == 135 || nearAngle == 315 )
                        gradient.setAttribute( "type", 4 ); // diagonal 2
                }
                else if ( type == "radial" || type == "ellipsoid" )
                    gradient.setAttribute( "type", 5 ); // circle
                else if ( type == "square" || type == "rectangular" )
                    gradient.setAttribute( "type", 6 ); // rectangle
                else if ( type == "axial" )
                    gradient.setAttribute( "type", 7 ); // pipecross

                // Hard to map between x- and y-center settings of ooimpress
                // and (un-)balanced settings of kpresenter. Let's try it.
                int x, y;
                if ( draw->hasAttribute( "draw:cx" ) )
                    x = draw->attribute( "draw:cx" ).remove( '%' ).toInt();
                else
                    x = 50;

                if ( draw->hasAttribute( "draw:cy" ) )
                    y = draw->attribute( "draw:cy" ).remove( '%' ).toInt();
                else
                    y = 50;

                if ( x == 50 && y == 50 )
                {
                    gradient.setAttribute( "unbalanced", 0 );
                    gradient.setAttribute( "xfactor", 100 );
                    gradient.setAttribute( "yfactor", 100 );
                }
                else
                {
                    gradient.setAttribute( "unbalanced", 1 );
                    // map 0 - 100% to -200 - 200
                    gradient.setAttribute( "xfactor", 4 * x - 200 );
                    gradient.setAttribute( "yfactor", 4 * y - 200 );
                }
            }
            e.appendChild( gradient );

            QDomElement fillType = doc.createElement( "FILLTYPE" );
            fillType.setAttribute( "value", 1 );
            e.appendChild( fillType );
        }
    }
}

void OoImpressImport::appendPie( QDomDocument& doc, QDomElement& e, const QDomElement& object )
{
    QDomElement angle = doc.createElement( "PIEANGLE" );
    int start = (int) ( object.attribute( "draw:start-angle" ).toDouble() );
    angle.setAttribute( "value",  start * 16 );
    e.appendChild( angle );

    QDomElement length = doc.createElement( "PIELENGTH" );
    int end = (int) ( object.attribute( "draw:end-angle" ).toDouble() );
    if ( end < start )
        length.setAttribute( "value",  ( 360 - start + end ) * 16 );
    else
        length.setAttribute( "value",  ( end - start ) * 16 );
    e.appendChild( length );
}

void OoImpressImport::appendImage( QDomDocument& doc, QDomElement& e, QDomElement& p, const QDomElement& object )
{
    QString fileName = storeImage( object );

    // create a key for the picture
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();

    QDomElement image = doc.createElement( "KEY" );
    image.setAttribute( "msec", time.msec() );
    image.setAttribute( "second", time.second() );
    image.setAttribute( "minute", time.minute() );
    image.setAttribute( "hour", time.hour() );
    image.setAttribute( "day", date.day() );
    image.setAttribute( "month", date.month() );
    image.setAttribute( "year", date.year() );
    image.setAttribute( "filename", fileName );
    e.appendChild( image );

    QDomElement settings = doc.createElement( "PICTURESETTINGS" );
    settings.setAttribute( "grayscal", 0 );
    settings.setAttribute( "bright", 0 );
    settings.setAttribute( "mirrorType", 0 );
    settings.setAttribute( "swapRGB", 0 );
    settings.setAttribute( "depth", 0 );
    e.appendChild( settings );

    QDomElement key = image.cloneNode().toElement();
    key.setAttribute( "name", "pictures/" + fileName );
    p.appendChild( key );
}

void OoImpressImport::appendBackgroundImage( QDomDocument& doc, QDomElement& e, QDomElement& p, const QDomElement& object )
{
    QString fileName = storeImage( object );

    // create a key for the picture
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();

    QDomElement image = doc.createElement( "BACKPICTUREKEY" );
    image.setAttribute( "msec", time.msec() );
    image.setAttribute( "second", time.second() );
    image.setAttribute( "minute", time.minute() );
    image.setAttribute( "hour", time.hour() );
    image.setAttribute( "day", date.day() );
    image.setAttribute( "month", date.month() );
    image.setAttribute( "year", date.year() );
    image.setAttribute( "filename", fileName );
    e.appendChild( image );

    QDomElement key = image.cloneNode().toElement();
    key.setTagName( "KEY" );
    key.setAttribute( "name", "pictures/" + fileName );
    p.appendChild( key );
}

void OoImpressImport::appendBackgroundGradient( QDomDocument& doc, QDomElement& e, const QDomElement& object )
{
    QDomElement backColor1 = doc.createElement( "BACKCOLOR1" );
    backColor1.setAttribute( "color", object.attribute( "draw:start-color" ) );
    e.appendChild( backColor1 );

    QDomElement backColor2 = doc.createElement( "BACKCOLOR2" );
    backColor2.setAttribute( "color", object.attribute( "draw:end-color" ) );
    e.appendChild( backColor2 );

    QDomElement backType = doc.createElement( "BACKTYPE" );
    backType.setAttribute( "value", 0 ); // color/gradient
    e.appendChild( backType );

    QDomElement bcType = doc.createElement( "BCTYPE" );
    QString type = object.attribute( "draw:style" );
    if ( type == "linear" )
    {
        int angle = object.attribute( "draw:angle" ).toInt() / 10;

        // make sure the angle is between 0 and 359
        angle = abs( angle );
        angle -= ( (int) ( angle / 360 ) ) * 360;

        // What we are trying to do here is to find out if the given
        // angle belongs to a horizontal, vertical or diagonal gradient.
        int lower, upper, nearAngle = 0;
        for ( lower = 0, upper = 45; upper < 360; lower += 45, upper += 45 )
        {
            if ( upper >= angle )
            {
                int distanceToUpper = abs( angle - upper );
                int distanceToLower = abs( angle - lower );
                nearAngle = distanceToUpper > distanceToLower ? lower : upper;
                break;
            }
        }

        // nearAngle should now be one of: 0, 45, 90, 135, 180...
        if ( nearAngle == 0 || nearAngle == 180 )
            bcType.setAttribute( "value", 1 ); // horizontal
        else if ( nearAngle == 90 || nearAngle == 270 )
            bcType.setAttribute( "value", 2 ); // vertical
        else if ( nearAngle == 45 || nearAngle == 225 )
            bcType.setAttribute( "value", 3 ); // diagonal 1
        else if ( nearAngle == 135 || nearAngle == 315 )
            bcType.setAttribute( "value", 4 ); // diagonal 2
    }
    else if ( type == "radial" || type == "ellipsoid" )
        bcType.setAttribute( "value", 5 ); // circle
    else if ( type == "square" || type == "rectangular" )
        bcType.setAttribute( "value", 6 ); // rectangle
    else if ( type == "axial" )
        bcType.setAttribute( "value", 7 ); // pipecross

    e.appendChild( bcType );

    QDomElement bGradient = doc.createElement( "BGRADIENT" );

    // Hard to map between x- and y-center settings of ooimpress
    // and (un-)balanced settings of kpresenter. Let's try it.
    int x, y;
    if ( object.hasAttribute( "draw:cx" ) )
        x = object.attribute( "draw:cx" ).remove( '%' ).toInt();
    else
        x = 50;

    if ( object.hasAttribute( "draw:cy" ) )
        y = object.attribute( "draw:cy" ).remove( '%' ).toInt();
    else
        y = 50;

    if ( x == 50 && y == 50 )
    {
        bGradient.setAttribute( "unbalanced", 0 );
        bGradient.setAttribute( "xfactor", 100 );
        bGradient.setAttribute( "yfactor", 100 );
    }
    else
    {
        bGradient.setAttribute( "unbalanced", 1 );
        // map 0 - 100% to -200 - 200
        bGradient.setAttribute( "xfactor", 4 * x - 200 );
        bGradient.setAttribute( "yfactor", 4 * y - 200 );
    }

    e.appendChild( bGradient );
}

void OoImpressImport::appendRounding( QDomDocument& doc, QDomElement& e, const QDomElement& object )
{
    if ( object.hasAttribute( "draw:corner-radius" ) )
    {
        // kpresenter uses percent, ooimpress uses cm ... hmm?
        QDomElement rounding = doc.createElement( "RNDS" );
        rounding.setAttribute( "x", (int) toPoint( object.attribute( "draw:corner-radius" ) ) );
        rounding.setAttribute( "y", (int) toPoint( object.attribute( "draw:corner-radius" ) ) );
        e.appendChild( rounding );
    }
}

void OoImpressImport::appendShadow( QDomDocument& doc, QDomElement& e )
{
    // Note that ooimpress makes a difference between shadowed text and
    // a shadowed object while kpresenter only knows the attribute 'shadow'.
    // This means that a shadowed textobject in kpresenter will always show
    // a shadowed text but no shadow for the object itself.

    // make sure this is a textobject or textspan
    if ( !e.hasAttribute( "type" ) ||
         ( e.hasAttribute( "type" ) && e.attribute( "type" ) == "4" ) )
    {
        if ( m_styleStack.hasAttribute( "fo:text-shadow" ) &&
             m_styleStack.attribute( "fo:text-shadow" ) != "none" )
        {
            // use the shadow attribute to indicate a text-shadow
            QDomElement shadow = doc.createElement( "SHADOW" );
            QString distance = m_styleStack.attribute( "fo:text-shadow" );
            distance.truncate( distance.find( ' ' ) );
            shadow.setAttribute( "distance", toPoint( distance ) );
            shadow.setAttribute( "direction", 5 );
            shadow.setAttribute( "color", "#a0a0a0" );
            e.appendChild( shadow );
        }
    }
    else if ( m_styleStack.hasAttribute( "draw:shadow" ) &&
              m_styleStack.attribute( "draw:shadow" ) == "visible" )
    {
        // use the shadow attribute to indicate an object-shadow
        QDomElement shadow = doc.createElement( "SHADOW" );
        double x = toPoint( m_styleStack.attribute( "draw:shadow-offset-x" ) );
        double y = toPoint( m_styleStack.attribute( "draw:shadow-offset-y" ) );

        if ( x < 0 && y < 0 )
        {
            shadow.setAttribute( "direction", 1 );
            shadow.setAttribute( "distance", (int) fabs ( x ) );
        }
        else if ( x == 0 && y < 0 )
        {
            shadow.setAttribute( "direction", 2 );
            shadow.setAttribute( "distance", (int) fabs ( y ) );
        }
        else if ( x > 0 && y < 0 )
        {
            shadow.setAttribute( "direction", 3 );
            shadow.setAttribute( "distance", (int) fabs ( x ) );
        }
        else if ( x > 0 && y == 0 )
        {
            shadow.setAttribute( "direction", 4 );
            shadow.setAttribute( "distance", (int) fabs ( x ) );
        }
        else if ( x > 0 && y > 0 )
        {
            shadow.setAttribute( "direction", 5 );
            shadow.setAttribute( "distance", (int) fabs ( x ) );
        }
        else if ( x == 0 && y > 0 )
        {
            shadow.setAttribute( "direction", 6 );
            shadow.setAttribute( "distance", (int) fabs ( y ) );
        }
        else if ( x < 0 && y > 0 )
        {
            shadow.setAttribute( "direction", 7 );
            shadow.setAttribute( "distance", (int) fabs ( x ) );
        }
        else if ( x < 0 && y == 0 )
        {
            shadow.setAttribute( "direction", 8 );
            shadow.setAttribute( "distance", (int) fabs ( x ) );
        }

        if ( m_styleStack.hasAttribute ( "draw:shadow-color" ) )
            shadow.setAttribute( "color", m_styleStack.attribute( "draw:shadow-color" ) );

        e.appendChild( shadow );
    }
}

void OoImpressImport::appendLineEnds( QDomDocument& doc, QDomElement& e )
{
    if ( m_styleStack.hasAttribute( "draw:marker-start" ) )
    {
        QDomElement lineBegin = doc.createElement( "LINEBEGIN" );
        QString type = m_styleStack.attribute( "draw:marker-start" );
        if ( type == "Arrow" || type == "Small Arrow" || type == "Rounded short Arrow" ||
             type == "Symmetric Arrow" || type == "Rounded large Arrow" || type == "Arrow concave" )
            lineBegin.setAttribute( "value", 1 );
        else if ( type == "Square" )
            lineBegin.setAttribute( "value", 2 );
        else if ( type == "Circle" || type == "Square 45" )
            lineBegin.setAttribute( "value", 3 );
        else if ( type == "Line Arrow" )
            lineBegin.setAttribute( "value", 4 );
        else if ( type == "Dimension Lines" )
            lineBegin.setAttribute( "value", 5 );
        else if ( type == "Double Arrow" )
            lineBegin.setAttribute( "value", 6 );
        e.appendChild( lineBegin );
    }
    if ( m_styleStack.hasAttribute( "draw:marker-end" ) )
    {
        QDomElement lineEnd = doc.createElement( "LINEEND" );
        QString type = m_styleStack.attribute( "draw:marker-end" );
        if ( type == "Arrow" || type == "Small Arrow" || type == "Rounded short Arrow" ||
             type == "Symmetric Arrow" || type == "Rounded large Arrow" || type == "Arrow concave" )
            lineEnd.setAttribute( "value", 1 );
        else if ( type == "Square" )
            lineEnd.setAttribute( "value", 2 );
        else if ( type == "Circle" || type == "Square 45" )
            lineEnd.setAttribute( "value", 3 );
        else if ( type == "Line Arrow" )
            lineEnd.setAttribute( "value", 4 );
        else if ( type == "Dimension Lines" )
            lineEnd.setAttribute( "value", 5 );
        else if ( type == "Double Arrow" )
            lineEnd.setAttribute( "value", 6 );
        e.appendChild( lineEnd );
    }
}

double OoImpressImport::toPoint( QString value )
{
    value.simplifyWhiteSpace();
    value.remove( ' ' );

    int index = value.find( QRegExp( "[a-z]{1,2}$" ), -1 );
    if ( index == -1 )
        return 0;

    QString unit = value.mid( index - 1 );
    value.truncate ( index - 1 );

    if ( unit == "cm" )
        return CM_TO_POINT( value.toDouble() );
    else if ( unit == "mm" )
        return MM_TO_POINT( value.toDouble() );
    else if ( unit == "pt" )
        return value.toDouble();
    else
        return 0;
}

void OoImpressImport::appendTextObjectMargin( QDomDocument& /*doc*/, QDomElement& e )
{
    if( m_styleStack.hasAttribute( "fo:padding-top" ) )
        e.setAttribute( "btoppt", toPoint( m_styleStack.attribute( "fo:padding-top" ) ) );
    if( m_styleStack.hasAttribute( "fo:padding-bottom" ) )
        e.setAttribute( "bbottompt", toPoint( m_styleStack.attribute( "fo:padding-bottom" ) ) );
    if( m_styleStack.hasAttribute( "fo:padding-left" ) )
        e.setAttribute( "bleftpt", toPoint( m_styleStack.attribute( "fo:padding-left" ) ) );
    if( m_styleStack.hasAttribute( "fo:padding-right" ) )
        e.setAttribute( "brightpt", toPoint( m_styleStack.attribute( "fo:padding-right" ) ) );
}


QDomElement OoImpressImport::parseTextBox( QDomDocument& doc, const QDomElement& textBox )
{
    QDomElement textObjectElement = doc.createElement( "TEXTOBJ" );
    appendTextObjectMargin( doc, textObjectElement );

    // KPresenter needs an attribute 'verticalValue' for vertical alignment to work
    // correctly. It is somehow calculated like value = 'height of box' - 'height of text'.
    // But we have no chance to calculate this value at this place... so needs to be fixed
    // in kpresenter before we activate this...
    #if 0
    // vertical alignment
    if ( m_styleStack.hasAttribute( "draw:textarea-vertical-align" ) )
    {
        QString alignment = m_styleStack.attribute( "draw:textarea-vertical-align" );
        if ( alignment == "top" )
            textObjectElement.setAttribute( "verticalAlign", "top" );
        else if ( alignment == "middle" )
            textObjectElement.setAttribute( "verticalAlign", "center" );
        else if ( alignment == "bottom" )
            textObjectElement.setAttribute( "verticalAlign", "bottom" );
    }
    #endif

    for ( QDomNode text = textBox.firstChild(); !text.isNull(); text = text.nextSibling() )
    {
        QDomElement t = text.toElement();
        QString name = t.tagName();

        QDomElement e;
        if ( name == "text:p" ) // text paragraph
            e = parseParagraph( doc, t );
        else if ( name == "text:unordered-list" || name == "text:ordered-list" ) // listitem
        {
            e = parseList( doc, t );
        }
        else
        {
            kdDebug() << "Unsupported texttype '" << name << "'" << endl;
            continue;
        }

        textObjectElement.appendChild( e );
        m_styleStack.clearObjectMark(); // remove the styles added by the child-objects
    }

    return textObjectElement;
}

QDomElement OoImpressImport::parseList( QDomDocument& doc, const QDomElement& list )
{
    //kdDebug() << k_funcinfo << "parsing list"<< endl;

    bool isOrdered;
    if ( list.tagName() == "text:ordered-list" )
        isOrdered = true;
    else
        isOrdered = false;

    // take care of nested lists
    QDomElement e;
    for ( QDomNode n = list.firstChild(); !n.isNull(); n = n.firstChild() )
    {
        e = n.toElement();
        QString name = e.tagName();
        if ( name == "text:unordered-list" )
        {
            isOrdered = false;
            // parse the list-properties
            fillStyleStack( e );
        }
        else if ( name == "text:ordered-list" )
        {
            isOrdered = true;
            // parse the list-properties
            fillStyleStack( e );
        }
        if ( name == "text:p" )
            break;
    }

    QDomElement p = parseParagraph( doc, e );

    QDomElement counter = doc.createElement( "COUNTER" );
    counter.setAttribute( "numberingtype", 0 );
    counter.setAttribute( "depth", 0 );

    if ( isOrdered )
        counter.setAttribute( "type", 1 );
    else
        counter.setAttribute( "type", 10 );

    // Don't 'appendChild()'! Text elements have to be the last children of the
    // paragraph element otherwise kpresenter will cut off the last character of
    // every item!
    p.insertBefore( counter, QDomNode() );

    return p;
}

QDomElement OoImpressImport::parseParagraph( QDomDocument& doc, const QDomElement& paragraph )
{
    QDomElement p = doc.createElement( "P" );

    // parse the paragraph-properties
    fillStyleStack( paragraph );

    if ( m_styleStack.hasAttribute( "fo:text-align" ) )
    {
        if ( m_styleStack.attribute( "fo:text-align" ) == "center" )
            p.setAttribute( "align", 4 );
        else if ( m_styleStack.attribute( "fo:text-align" ) == "justify" )
            p.setAttribute( "align", 8 );
        else if ( m_styleStack.attribute( "fo:text-align" ) == "start" )
            p.setAttribute( "align", 0 );
        else if ( m_styleStack.attribute( "fo:text-align" ) == "end" )
            p.setAttribute( "align", 2 );
    }
    else
        p.setAttribute( "align", 0 ); // use left aligned as default

    // parse every childnode of the paragraph
    for( QDomNode n = paragraph.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QString textData;
        QDomText t = n.toText();
        if ( t.isNull() ) // no textnode, so maybe it's a text:span
        {
            QDomElement ts = n.toElement();
            if ( ts.tagName() != "text:span" ) // TODO: are there any other possible
                continue;                      // elements or even nested test:spans?

            fillStyleStack( ts.toElement() );

            // We do a look ahead to eventually find a text:span that contains
            // only a line-break. If found, we'll add it to the current string
            // and move on to the next sibling.
            QDomNode next = n.nextSibling();
            if ( !next.isNull() )
            {
                QDomNode lineBreak = next.namedItem( "text:line-break" );
                if ( !lineBreak.isNull() ) // found a line-break
                {
                    textData = ts.text() + "\n";
                    n = n.nextSibling(); // move on to the next sibling
                }
                else
                    textData = ts.text();
            }
            else
                textData = ts.text();
        }
        else
            textData = t.data();

        // offset before and after paragraph
        if( m_styleStack.hasAttribute("fo:margin-top") ||
            m_styleStack.hasAttribute("fo:margin-bottom"))
        {
            double mtop = toPoint( m_styleStack.attribute( "fo:margin-top" ) );
            double mbottom = toPoint( m_styleStack.attribute("fo:margin-bottom" ) );
            if( mtop != 0 || mbottom != 0 )
            {
                QDomElement offset = doc.createElement( "OFFSETS" );
                if( mtop != 0 )
                    offset.setAttribute( "before", mtop );
                if( mbottom != 0 )
                    offset.setAttribute( "after", mbottom );
                p.appendChild( offset );
            }
        }

        // indentation of paragraph
        if ( m_styleStack.hasAttribute( "fo:margin-left" ) ||
             m_styleStack.hasAttribute( "fo:margin-right" ) ||
             m_styleStack.hasAttribute( "fo:text-indent"))
        {
            double marginLeft =toPoint( m_styleStack.attribute( "fo:margin-left" ) );
            double marginRight = toPoint( m_styleStack.attribute( "fo:margin-right" ) );
            double first = toPoint( m_styleStack.attribute( "fo:text-indent" ) );
            if ( marginLeft != 0 || marginRight != 0 || first != 0 )
            {
                QDomElement indent = doc.createElement( "INDENTS" );
                if( marginLeft != 0 )
                    indent.setAttribute( "left", marginLeft );
                if( marginRight != 0 )
                    indent.setAttribute( "right", marginLeft );
                if( first != 0 )
                    indent.setAttribute( "first", first );
                p.appendChild( indent );
            }
        }

        // line spacing
        if( m_styleStack.hasAttribute( "fo:line-height" ) )
        {
            QString value = m_styleStack.attribute( "fo:line-height" );
            QDomElement lineSpacing = doc.createElement( "LINESPACING" );
            if( value == "150%" )
            {
                lineSpacing.setAttribute( "type", "oneandhalf" );
            }
            else if( value == "200%" )
            {
                lineSpacing.setAttribute( "type", "double" );
            }
            p.appendChild( lineSpacing );
        }

        QDomElement text = doc.createElement( "TEXT" );
        text.appendChild( doc.createTextNode( textData ) );

        //kdDebug() << k_funcinfo << "Para text is: " << paragraph.text() << endl;

        // parse the text-properties
        if ( m_styleStack.hasAttribute( "fo:color" ) )
            text.setAttribute( "color", m_styleStack.attribute( "fo:color" ) );
        if ( m_styleStack.hasAttribute( "fo:font-family" ) )
        {
            // 'Thorndale' is not known outside OpenOffice so we substitute it
            // with 'Times New Roman' that looks nearly the same.
            if ( m_styleStack.attribute( "fo:font-family" ) == "Thorndale" )
                text.setAttribute( "family", "Times New Roman" );
            else
                text.setAttribute( "family", m_styleStack.attribute( "fo:font-family" ).remove( "'" ) );
        }
        if ( m_styleStack.hasAttribute( "fo:font-size" ) )
            text.setAttribute( "pointSize", toPoint( m_styleStack.attribute( "fo:font-size" ) ) );
        if ( m_styleStack.hasAttribute( "fo:font-weight" ) )
            if ( m_styleStack.attribute( "fo:font-weight" ) == "bold" )
                text.setAttribute( "bold", 1 );
        if ( m_styleStack.hasAttribute( "fo:font-style" ) )
            if ( m_styleStack.attribute( "fo:font-style" ) == "italic" )
                text.setAttribute( "italic", 1 );
        if ( m_styleStack.hasAttribute( "style:text-crossing-out" ) )
        {
            QString strikeOutType = m_styleStack.attribute( "style:text-crossing-out" );
            if ( strikeOutType =="double-line" )
            {
                text.setAttribute( "strikeOut", "double" );
                text.setAttribute( "strikeoutstyleline", "solid" );
            }
            else if ( strikeOutType == "single-line" )
            {
                text.setAttribute( "strikeOut", "single" );
                text.setAttribute( "strikeoutstyleline", "solid" );
            }
            else if ( strikeOutType =="thick-line" )
            {
                text.setAttribute( "strikeOut", "single-bold" );
                text.setAttribute( "strikeoutstyleline","solid" );
            }

        }
        if ( m_styleStack.hasAttribute( "style:text-position" ) )
        {
            QString textPos =m_styleStack.attribute( "style:text-position" );
            //relativetextsize="0.58"
            //"super 58%"
            if( textPos.contains( "super" ) )
            {
                textPos = textPos.remove( "super" );
                textPos = textPos.remove( "%" );
                double value = textPos.stripWhiteSpace().toDouble();
                text.setAttribute( "VERTALIGN", 2 );
                text.setAttribute( "relativetextsize", value / 100 );
            }
            else if (textPos.contains("sub"))
            {
                textPos = textPos.remove( "sub" );
                textPos = textPos.remove( "%" );
                double value = textPos.stripWhiteSpace().toDouble();
                text.setAttribute( "VERTALIGN", 1 );
                text.setAttribute( "relativetextsize", value / 100 );
            }
        }
        if ( m_styleStack.hasAttribute( "style:text-underline" ) )
        {
            QString underType = m_styleStack.attribute( "style:text-underline" );
            QString underLineColor = m_styleStack.attribute( "style:text-underline-color" );
            if ( underType == "single" )
            {
                text.setAttribute( "underline", 1 );
                text.setAttribute( "underlinestyleline", "solid" );
                text.setAttribute( "underlinecolor", underLineColor );
            }
            else if ( underType == "double" )
            {
                text.setAttribute( "underline", "double" );
                text.setAttribute( "underlinestyleline", "solid" );
                text.setAttribute( "underlinecolor", underLineColor );
            }
            else if ( underType == "bold" )
            {
                text.setAttribute( "underline", "single-bold" );
                text.setAttribute( "underlinestyleline", "solid" );
                text.setAttribute( "underlinecolor", underLineColor );
            }
            else if ( underType == "wave" )
            {
                //not implemented into kpresenter
                text.setAttribute( "underline", "wave" );
                text.setAttribute( "underlinestyleline", "solid" );
                text.setAttribute( "underlinecolor", underLineColor );
            }
            else if ( underType == "dotted" )
            {
                text.setAttribute( "underline", 1 );
                text.setAttribute( "underlinestyleline", "dot" );
                text.setAttribute( "underlinecolor", underLineColor );

            }
            else if ( underType == "dash" )
            {
                text.setAttribute( "underline", 1 );
                text.setAttribute( "underlinestyleline", "dash" );
                text.setAttribute( "underlinecolor", underLineColor );
            }
            else if ( underType == "dot-dash" )
            {
                text.setAttribute( "underline", 1 );
                text.setAttribute( "underlinestyleline", "dash" );
                text.setAttribute( "underlinecolor", underLineColor );
            }
            else if ( underType == "bold-dotted" )
            {
                text.setAttribute( "underline", "single-bold" );
                text.setAttribute( "underlinestyleline", "dot" );
                text.setAttribute( "underlinecolor", underLineColor );
            }

        }

        appendShadow( doc, p ); // this is necessary to take care of shadowed paragraphs
        m_styleStack.clearObjectMark(); // remove possible test:span styles from the stack
        p.appendChild( text );
    }

    return p;
}

void OoImpressImport::createStyleMap( QDomDocument &docstyles )
{
    QDomElement styles = docstyles.documentElement();
    if ( styles.isNull() )
        return;

    QDomNode fixedStyles = styles.namedItem( "office:styles" );
    if ( !fixedStyles.isNull() )
    {
        insertDraws( fixedStyles.toElement() );
        insertStyles( fixedStyles.toElement() );
    }

    QDomNode automaticStyles = styles.namedItem( "office:automatic-styles" );
    if ( !automaticStyles.isNull() )
        insertStyles( automaticStyles.toElement() );

    QDomNode masterStyles = styles.namedItem( "office:master-styles" );
    if ( !masterStyles.isNull() )
        insertStyles( masterStyles.toElement() );
}

void OoImpressImport::insertDraws( const QDomElement& styles )
{
    for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();

        if ( !e.hasAttribute( "draw:name" ) )
            continue;

        QString name = e.attribute( "draw:name" );
        m_draws.insert( name, new QDomElement( e ) );
    }
}

void OoImpressImport::insertStyles( const QDomElement& styles )
{
    for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();

        if ( !e.hasAttribute( "style:name" ) )
            continue;

        QString name = e.attribute( "style:name" );
        m_styles.insert( name, new QDomElement( e ) );
        //kdDebug() << "Style: '" << name << "' loaded " << endl;
    }
}

void OoImpressImport::fillStyleStack( const QDomElement& object )
{
    // find all styles associated with an object and push them on the stack
    if ( object.hasAttribute( "presentation:style-name" ) )
        addStyles( m_styles[object.attribute( "presentation:style-name" )] );

    if ( object.hasAttribute( "draw:style-name" ) )
        addStyles( m_styles[object.attribute( "draw:style-name" )] );

    if ( object.hasAttribute( "draw:text-style-name" ) )
        addStyles( m_styles[object.attribute( "draw:text-style-name" )] );

    if ( object.hasAttribute( "text:style-name" ) )
        addStyles( m_styles[object.attribute( "text:style-name" )] );
}

void OoImpressImport::addStyles( const QDomElement* style )
{
    // this function is necessary as parent styles can have parents themself
    if ( style->hasAttribute( "style:parent-style-name" ) )
        addStyles( m_styles[style->attribute( "style:parent-style-name" )] );

    m_styleStack.push( style );
}

QString OoImpressImport::storeImage( const QDomElement& object )
{
    // store the picture
    KZip inputFile = KZip( m_chain->inputFile() );
    inputFile.open( IO_ReadOnly );

    QString url = object.attribute( "xlink:href" ).remove( '#' );
    KArchiveFile* file = (KArchiveFile*) inputFile.directory()->entry( url );

    QString extension = url.mid( url.find( '.' ) );
    QString fileName = QString( "picture%1" ).arg( m_numPicture++ ) + extension;
    KoStoreDevice* out = m_chain->storageFile( "pictures/" + fileName, KoStore::Write );

    if ( out )
    {
        QByteArray buffer = file->data();
        out->writeBlock( buffer.data(), buffer.size() );
    }

    inputFile.close();
    return fileName;
}

void OoImpressImport::storeObjectStyles( const QDomElement& object )
{
    m_styleStack.clearPageMark(); // remove styles of previous object
    fillStyleStack( object );
    m_styleStack.setObjectMark();
}

StyleStack::StyleStack()
    : m_pageMark( 0 ),
      m_objectMark( 0 )
{
    m_stack.setAutoDelete( false );
}

StyleStack::~StyleStack()
{
}

void StyleStack::clear()
{
    m_pageMark = 0;
    m_objectMark = 0;
    m_stack.clear();
}

void StyleStack::clearPageMark()
{
    for ( uint index = m_stack.count() - 1; index >= m_pageMark; --index )
        m_stack.remove( index );
}

void StyleStack::setPageMark()
{
    m_pageMark= m_stack.count();
}

void StyleStack::clearObjectMark()
{
    for ( uint index = m_stack.count() - 1; index >= m_objectMark; --index )
        m_stack.remove( index );
}

void StyleStack::setObjectMark()
{
    m_objectMark= m_stack.count();
}

void StyleStack::pop()
{
    m_stack.removeLast();
}

void StyleStack::push( const QDomElement* style )
{
    m_stack.append( style );
}

bool StyleStack::hasAttribute( const QString& name )
{
    // TODO: has to be fixed for complex styles like list-styles
    for ( QDomElement *style = m_stack.last(); style; style = m_stack.prev() )
    {
        QDomElement properties = style->namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return true;
    }

    return false;
}

QString StyleStack::attribute( const QString& name )
{
    // TODO: has to be fixed for complex styles like list-styles
    for ( QDomElement *style = m_stack.last(); style; style = m_stack.prev() )
    {
        QDomElement properties = style->namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return properties.attribute( name );
    }

    return QString::null;
}

#include <ooimpressimport.moc>
