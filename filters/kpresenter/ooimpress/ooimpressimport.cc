// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>

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
#include <qfileinfo.h>
#include <qdir.h>
#include <kzip.h>
#include <karchive.h>
#include <kdebug.h>
#include <koUnit.h>
#include <koDocumentInfo.h>
#include <koDocument.h>

#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koGlobal.h>
#include <ooutils.h>

typedef KGenericFactory<OoImpressImport, KoFilter> OoImpressImportFactory;
K_EXPORT_COMPONENT_FACTORY( libooimpressimport, OoImpressImportFactory( "ooimpressimport" ) )


OoImpressImport::OoImpressImport( KoFilter *, const char *, const QStringList & )
    : KoFilter(),
      m_numPicture( 1 ),
      m_numSound(1),
      m_styles( 23, true )
{
    m_styles.setAutoDelete( true );
}

OoImpressImport::~OoImpressImport()
{
    QDictIterator<animationList> it( m_animations ); // See QDictIterator
    for( ; it.current(); ++it )
    {
        delete it.current()->element;
    }
    m_animations.clear();
}

KoFilter::ConversionStatus OoImpressImport::convert( QCString const & from, QCString const & to )
{
    kdDebug(30518) << "Entering Ooimpress Import filter: " << from << " - " << to << endl;

    if ( (from != "application/vnd.sun.xml.impress" && from != "application/vnd.sun.xml.impress.template" )
                            || to != "application/x-kpresenter" )
    {
        kdWarning(30518) << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    m_zip = new KZip( m_chain->inputFile() );

    if ( !m_zip->open( IO_ReadOnly ) )
    {
        kdError(30518) << "Couldn't open the requested file "<< m_chain->inputFile() << endl;
        delete m_zip;
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus preStatus = openFile();

    if ( preStatus != KoFilter::OK )
    {
        m_zip->close();
        delete m_zip;
        return preStatus;
    }

    QDomDocument docinfo;
    createDocumentInfo( docinfo );

    // store document info
    KoStoreDevice* out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if( out )
    {
        QCString info = docinfo.toCString();
        //kdDebug(30518) << " info :" << info << endl;
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
        //kdDebug(30518) << " content :" << content << endl;
        out->writeBlock( content , content.length() );
    }

    m_zip->close();
    delete m_zip;

    kdDebug(30518) << "######################## OoImpressImport::convert done ####################" << endl;
    return KoFilter::OK;
}

// Very related to OoWriterImport::openFile()
KoFilter::ConversionStatus OoImpressImport::openFile()
{
    KoFilter::ConversionStatus status = loadAndParse( "content.xml", m_content );
    if ( status != KoFilter::OK )
    {
        kdError(30518) << "Content.xml could not be parsed correctly! Aborting!" << endl;
        return status;
    }

    // We do not stop if the following calls fail.
    QDomDocument styles;
    loadAndParse( "styles.xml", styles );
    loadAndParse( "meta.xml", m_meta );
    loadAndParse( "settings.xml", m_settings );

    emit sigProgress( 10 );
    createStyleMap( styles );

    return KoFilter::OK;
}

KoFilter::ConversionStatus OoImpressImport::loadAndParse(const QString& filename, QDomDocument& doc)
{
    kdDebug(30518) << "Trying to open " << filename << endl;

    if (!m_zip)
    {
        kdError(30518) << "No ZIP file!" << endl;
        return KoFilter::CreationError; // Should not happen
    }

    const KArchiveEntry* entry = m_zip->directory()->entry( filename );
    if (!entry)
    {
        kdWarning(30518) << "Entry " << filename << " not found!" << endl;
        return KoFilter::FileNotFound;
    }
    if (entry->isDirectory())
    {
        kdWarning(30518) << "Entry " << filename << " is a directory!" << endl;
        return KoFilter::WrongFormat;
    }
    const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
    QIODevice* io=f->device();
    kdDebug(30518) << "Entry " << filename << " has size " << f->size() << endl;

    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !doc.setContent( io, &errorMsg, &errorLine, &errorColumn ) )
    {
        kdError(30518) << "Parsing error in " << filename << "! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
        delete io;
        return KoFilter::ParsingError;
    }
    delete io;

    kdDebug(30518) << "File " << filename << " loaded and parsed!" << endl;

    return KoFilter::OK;
}

// Very related to OoWriterImport::createDocumentInfo
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

    //kdDebug(30518) << " meta-info :" << m_meta.toCString() << endl;
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

    // presentation settings
    QDomElement settings = body.namedItem("presentation:settings").toElement();
    if (!settings.isNull())
    {
        if (settings.attribute("presentation:endless")=="true")
        {
            QDomElement infElem = doc.createElement("INFINITLOOP");
            infElem.setAttribute("value", 1);
            docElement.appendChild(infElem);
        }

        if (settings.attribute("presentation:force-manual")=="true")
        {
            QDomElement manualElem = doc.createElement("MANUALSWITCH");
            manualElem.setAttribute("value", 1);
            docElement.appendChild(manualElem);
        }
    }

    // it seems that ooimpress has different paper-settings for every slide.
    // we take the settings of the first slide for the whole document.
    QDomNode drawPage = body.namedItem( "draw:page" );
    if ( drawPage.isNull() ) // no slides? give up.
        return;

    QDomElement objectElement = doc.createElement( "OBJECTS" );
    QDomElement pictureElement = doc.createElement( "PICTURES" );
    QDomElement pageTitleElement = doc.createElement( "PAGETITLES" );
    QDomElement pageNoteElement = doc.createElement( "PAGENOTES" );
    QDomElement backgroundElement = doc.createElement( "BACKGROUND" );
    QDomElement soundElement = doc.createElement( "SOUNDS" );
    QDomElement selSlideElement = doc.createElement( "SELSLIDES" );
    QDomElement helpLineElement = doc.createElement( "HELPLINES" );

    QDomElement settingsDoc = m_settings.documentElement();

    QDomElement dp = drawPage.toElement();
    QDomElement *master = m_styles[dp.attribute( "draw:master-page-name" )];

    appendObject(*master, doc, soundElement,pictureElement,pageNoteElement,objectElement, 0, true);

    QDomElement *style = m_styles[master->attribute( "style:page-master-name" )];
    QDomElement properties = style->namedItem( "style:properties" ).toElement();
	QDomElement *backgroundStyle = m_stylesPresentation[ master->attribute("draw:style-name" ).isEmpty() ? "Standard-background" : master->attribute("draw:style-name" ) ];

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
        paperElement.setAttribute( "ptWidth", KoUnit::parseValue(properties.attribute( "fo:page-width" ) ) );
        paperElement.setAttribute( "ptHeight", KoUnit::parseValue(properties.attribute( "fo:page-height" ) ) );
//         paperElement.setAttribute( "unit", 0 );
//         paperElement.setAttribute( "format", 5 );
//         paperElement.setAttribute( "tabStopValue", 42.5198 );
//         paperElement.setAttribute( "orientation", 0 );
        // Keep pageHeight in cm to avoid rounding-errors that would
        // get multiplied with every new slide.

        if (properties.attribute("style:print-orientation")=="portrait")
            paperElement.setAttribute("orientation", 0);
        else if (properties.attribute("style:print-orientation")=="landscape")
            paperElement.setAttribute("orientation", 1);

        pageHeight = properties.attribute( "fo:page-height" ).remove( "cm" ).toDouble();

        QDomElement paperBorderElement = doc.createElement( "PAPERBORDERS" );
        paperBorderElement.setAttribute( "ptRight", KoUnit::parseValue( properties.attribute( "fo:margin-right" ) ) );
        paperBorderElement.setAttribute( "ptBottom", KoUnit::parseValue( properties.attribute( "fo:margin-bottom" ) ) );
        paperBorderElement.setAttribute( "ptLeft", KoUnit::parseValue( properties.attribute( "fo:margin-left" ) ) );
        paperBorderElement.setAttribute( "ptTop", KoUnit::parseValue( properties.attribute( "fo:margin-top" ) ) );
        paperElement.appendChild( paperBorderElement );
    }



    // parse all pages
    for ( drawPage = body.firstChild(); !drawPage.isNull(); drawPage = drawPage.nextSibling() )
    {
        dp = drawPage.toElement();
        if ( dp.tagName()=="draw:page" && dp.hasAttribute( "draw:id" ))
        {
        m_styleStack.clear(); // remove all styles
        fillStyleStack( dp );
        m_styleStack.save();
        int pagePos = dp.attribute( "draw:id" ).toInt() - 1;
        // take care of a possible page background or slide transition or sound
        if ( m_styleStack.hasAttribute( "draw:fill" )
             || m_styleStack.hasAttribute( "presentation:transition-style" ))
        {
            appendBackgroundPage( doc, backgroundElement,pictureElement, soundElement );
        }
        else if ( !m_styleStack.hasAttribute( "draw:fill" ) && backgroundStyle)
        {
            m_styleStack.save();
            m_styleStack.push( *backgroundStyle );
            appendBackgroundPage( doc, backgroundElement,pictureElement, soundElement );
            m_styleStack.restore();
            kdDebug()<<" load standard bacground \n";
        }
        if ( m_styleStack.hasAttribute( "presentation:visibility" ) )
        {
            QString str = m_styleStack.attribute( "presentation:visibility" );
            QDomElement slide = doc.createElement("SLIDE");
            slide.setAttribute( "nr", pagePos );
            slide.setAttribute( "show", ( ( str=="hidden" ) ? "0" : "1" ));
            selSlideElement.appendChild( slide );
            //todo add support
            kdDebug()<<"m_styleStack.hasAttribute( presentation:visibility ) :"<<str<<" position page "<<pagePos<<endl;
        }

        // set the pagetitle
        QDomElement titleElement = doc.createElement( "Title" );
        titleElement.setAttribute( "title", dp.attribute( "draw:name" ) );
        pageTitleElement.appendChild( titleElement );

        // The '+1' is necessary to avoid that objects that start on the first line
        // of a slide will show up on the last line of the previous slide.
        double offset = CM_TO_POINT( ( dp.attribute( "draw:id" ).toInt() - 1 ) * pageHeight ) + 1;

        // animations (object effects)
        createPresentationAnimation(drawPage.namedItem("presentation:animations").toElement() );

        // parse all objects
        appendObject(drawPage, doc, soundElement,pictureElement,pageNoteElement,objectElement, offset);


        //m_animations.clear();
        m_styleStack.restore();
    }
    }

    docElement.appendChild( paperElement );
    docElement.appendChild( backgroundElement );
    if ( appendHelpLine( doc, settingsDoc, helpLineElement ) )
        docElement.appendChild( helpLineElement );
    docElement.appendChild( pageTitleElement );
    docElement.appendChild( pageNoteElement );
    docElement.appendChild( objectElement );
    docElement.appendChild( selSlideElement );
    docElement.appendChild( soundElement );
    docElement.appendChild( pictureElement );

    doccontent.appendChild( doc );
}

bool OoImpressImport::appendHelpLine( QDomDocument &doc,const QDomElement &settingElement, QDomElement &helpLineElement )
{
    bool foundElement = false;
    //<config:config-item config:name="SnapLinesDrawing" config:type="string">V7939H1139</config:config-item>
    //by default show line
    QDomNode tmp = settingElement.namedItem( "office:settings" );
    if (tmp.isNull() )
        return false;

    for ( QDomNode n = tmp.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        //kdDebug()<<"e.tagName() :"<<e.tagName()<<endl;
        if ( e.hasAttribute( "config:name" ) && ( e.attribute( "config:name" )=="view-settings" ) )
        {
            for ( QDomNode viewSetting = n.firstChild(); !viewSetting.isNull(); viewSetting = viewSetting.nextSibling() )
            {
                QDomElement configItem = viewSetting.toElement();
                if ( configItem.tagName()== "config:config-item-map-indexed"
                     && ( configItem.attribute( "config:name" )=="Views" ) )
                {
                    QDomNode item = configItem.firstChild(); //<config:config-item-map-entry>
                    for ( QDomNode item2 = item.firstChild(); !item2.isNull(); item2 = item2.nextSibling() )
                    {
                        QDomElement viewItem = item2.toElement();
                        //kdDebug()<<"viewItem.tagName() :"<<viewItem.tagName()<<endl;
                        if ( viewItem.tagName()=="config:config-item" && ( viewItem.attribute("config:name")=="SnapLinesDrawing" ) )
                        {
                            kdDebug()<<"SnapLinesDrawing****************:"<<viewItem.text()<<endl;
                            parseHelpLine( doc, helpLineElement, viewItem.text() );
                            //display it by default
                            helpLineElement.setAttribute( "show", true );
                            foundElement = true;
                            break;
                        }

                    }
                }
            }
        }
    }

    return foundElement;
}

void OoImpressImport::parseHelpLine( QDomDocument &doc,QDomElement &helpLineElement, const QString &text )
{
    QString str;
    int newPos = text.length()-1; //start to element = 1
    for ( int pos = text.length()-1; pos >=0;--pos )
    {
        if ( text[pos]=='P' )
        {

            //point
            str = text.mid( pos+1, ( newPos-pos ) );
            QDomElement point=doc.createElement("HelpPoint");

            kdDebug()<<" point element  :"<< str <<endl;
            QStringList listVal = QStringList::split( ",", str );
            int posX = ( listVal[0].toInt()/100 );
            int posY = ( listVal[1].toInt()/100 );
            QString pt_x;
            QString pt_y;
            pt_x.setNum(posX);
            pt_x+="mm";
            pt_y.setNum(posY);
            pt_y+="mm";
            point.setAttribute("posX", KoUnit::parseValue(pt_x));
            point.setAttribute("posY", KoUnit::parseValue(pt_y));

            helpLineElement.appendChild( point );
            newPos = pos-1;
        }
        else if ( text[pos]=='V' )
        {
            QDomElement lines=doc.createElement("Vertical");
            //vertical element
            str = text.mid( pos+1, ( newPos-pos ) );
            kdDebug()<<" vertical  :"<< str <<endl;
            int posX = ( str.toInt()/100 );
            QString pt_x;
            pt_x.setNum(posX);
            pt_x+="mm";
            lines.setAttribute( "value",  KoUnit::parseValue(pt_x) );
            helpLineElement.appendChild( lines );

            newPos = ( pos-1 );

        }
        else if ( text[pos]=='H' )
        {
            //horizontal element
            QDomElement lines=doc.createElement("Horizontal");
            str = text.mid( pos+1, ( newPos-pos ) );
            kdDebug()<<" horizontal  :"<< str <<endl;

            int posY = ( str.toInt()/100 );
            QString pt_y;
            pt_y.setNum(posY);
            pt_y+="mm";

            lines.setAttribute( "value", KoUnit::parseValue(pt_y)  );
            helpLineElement.appendChild( lines );
            newPos = pos-1;
        }
    }
}


void OoImpressImport::appendObject(QDomNode & drawPage,  QDomDocument & doc,  QDomElement & soundElement, QDomElement & pictureElement, QDomElement & pageNoteElement, QDomElement &objectElement, double offset, bool sticky)
{
    for ( QDomNode object = drawPage.firstChild(); !object.isNull(); object = object.nextSibling() )
    {
        QDomElement o = object.toElement();
        QString name = o.tagName();
        QString drawID = o.attribute("draw:id");
        m_styleStack.save();

        QDomElement e;
        if ( name == "draw:text-box" ) // textbox
        {
            fillStyleStack( o, sticky );
            e = doc.createElement( "OBJECT" );
            e.setAttribute( "type", 4 );
            if ( sticky )
                e.setAttribute( "sticky", "1" );
            append2DGeometry( doc, e, o, (int)offset );
            appendName(doc, e, o);
            appendPen( doc, e );
            appendBrush( doc, e );
            appendRounding( doc, e, o );
            appendShadow( doc, e );
            appendObjectEffect(doc, e, o, soundElement);
            e.appendChild( parseTextBox( doc, o ) );
        }
        else if ( name == "draw:rect" ) // rectangle
        {
            fillStyleStack( o, sticky );
            e = doc.createElement( "OBJECT" );
            e.setAttribute( "type", 2 );
            if ( sticky )
                e.setAttribute( "sticky", "1" );
            append2DGeometry( doc, e, o, (int)offset );
            appendName(doc, e, o);
            appendPen( doc, e );
            appendBrush( doc, e );
            appendRounding( doc, e, o );
            appendShadow( doc, e );

            appendObjectEffect(doc, e, o, soundElement);
        }
        else if ( name == "draw:circle" || name == "draw:ellipse" )
        {
            fillStyleStack( o, sticky );
            e = doc.createElement( "OBJECT" );
            if ( sticky )
                e.setAttribute( "sticky", "1" );
            append2DGeometry( doc, e, o, (int)offset );
            appendName(doc, e, o);
            appendPen( doc, e );
            appendShadow( doc, e );
            appendLineEnds( doc, e );
            appendObjectEffect(doc, e, o, soundElement);

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
            fillStyleStack( o, sticky );
            e = doc.createElement( "OBJECT" );
            e.setAttribute( "type", 1 );
            if ( sticky )
                e.setAttribute( "sticky", "1" );
            bool orderEndStartLine = appendLineGeometry( doc, e, o, (int)offset );
            appendName(doc, e, o);
            appendPen( doc, e );
            appendBrush( doc, e );
            appendShadow( doc, e );
            appendLineEnds( doc, e, orderEndStartLine );
            appendObjectEffect(doc, e, o, soundElement);
        }
        else if (name=="draw:polyline") { // polyline
            fillStyleStack(o, sticky);
            e = doc.createElement("OBJECT");
            e.setAttribute("type", 12);
            if ( sticky )
                e.setAttribute( "sticky", "1" );
            append2DGeometry(doc, e, o, (int)offset);
            appendName(doc, e, o);
            appendPoints(doc, e, o);
            appendPen(doc, e);
            appendBrush(doc, e);
            appendLineEnds(doc, e);
            //appendShadow(doc, e);
            appendObjectEffect(doc, e, o, soundElement);
        }
        else if (name=="draw:polygon") { // polygon
            fillStyleStack(o, sticky);
            e = doc.createElement("OBJECT");
            e.setAttribute("type", 16);
            if ( sticky )
                e.setAttribute( "sticky", "1" );
            append2DGeometry(doc, e, o, (int)offset);
            appendName(doc, e, o);
            appendPoints(doc, e, o);
            appendPen(doc, e);
            appendBrush(doc, e);
            //appendLineEnds(doc, e);
            //appendShadow(doc, e);
            appendObjectEffect(doc, e, o, soundElement);
        }
        else if ( name == "draw:image" ) // image
        {
            fillStyleStack( o, sticky );
            e = doc.createElement( "OBJECT" );
            e.setAttribute( "type", 0 );
            if ( sticky )
                e.setAttribute( "sticky", "1" );
            append2DGeometry( doc, e, o, (int)offset );
            appendName(doc, e, o);
            appendImage( doc, e, pictureElement, o );
            appendObjectEffect(doc, e, o, soundElement);
        }
        else if ( name == "draw:object" )
        {
            //todo add part object
        }
        else if ( name == "draw:g" )
        {
            //todo add group object
        }
        else if ( name == "draw:path" )
        {
            //todo add path object (freehand/cubic/quadricbeziercurve
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
            kdDebug(30518) << "Unsupported object '" << name << "'" << endl;
            m_styleStack.restore();
            continue;
        }

        objectElement.appendChild( e );
        m_styleStack.restore();
    }
}


void OoImpressImport::appendBackgroundPage( QDomDocument &doc, QDomElement &backgroundElement, QDomElement & pictureElement,  QDomElement &soundElement)
{
    QDomElement bgPage = doc.createElement( "PAGE" );

    // background
    if ( m_styleStack.hasAttribute( "draw:fill" ) )
    {
        const QString fill = m_styleStack.attribute( "draw:fill" );
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
        }
        else if ( fill == "gradient" )
        {
            QString style = m_styleStack.attribute( "draw:fill-gradient-name" );
            QDomElement* draw = m_draws[style];
            appendBackgroundGradient( doc, bgPage, *draw );
        }
        else if ( fill == "bitmap" )
        {
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
        }
    }

    if ( m_styleStack.hasAttribute( "presentation:duration" ) )
    {
        QString str = m_styleStack.attribute("presentation:duration");
        kdDebug()<<"styleStack.hasAttribute(presentation:duration ) :"<<str<<endl;
        //convert date duration
	    int hour( str.mid( 2, 2 ).toInt() );
	    int minute( str.mid( 5, 2 ).toInt() );
	    int second( str.mid( 8, 2 ).toInt() );
        int pageTimer = second + minute*60 + hour*60*60;
        QDomElement pgEffect = doc.createElement("PGTIMER");
        pgEffect.setAttribute( "timer", pageTimer );
        bgPage.appendChild(pgEffect);
    }
    // slide transition
    if (m_styleStack.hasAttribute("presentation:transition-style"))
    {
        QDomElement pgEffect = doc.createElement("PGEFFECT");

        const QString effect = m_styleStack.attribute("presentation:transition-style");
        //kdDebug(30518) << "Transition name: " << effect << endl;
        int pef;

        if (effect=="vertical-stripes" || effect=="vertical-lines") // PEF_BLINDS_VER
            pef=14;
        else if (effect=="horizontal-stripes" || effect=="horizontal-lines") // PEF_BLINDS_HOR
            pef=13;
        else if (effect=="spiralin-left" || effect=="spiralin-right"
                 || effect== "spiralout-left" || effect=="spiralout-right") // PEF_SURROUND1
            pef=11;
        else if (effect=="fade-from-upperleft") // PEF_STRIPS_RIGHT_DOWN
            pef=39;
        else if (effect=="fade-from-upperright") // PEF_STRIPS_LEFT_DOWN
            pef=37;
        else if (effect=="fade-from-lowerleft") // PEF_STRIPS_RIGHT_UP
            pef=38;
        else if (effect=="fade-from-lowerright") // PEF_STRIPS_LEFT_UP
            pef=36;
        else if (effect=="fade-from-top") // PEF_COVER_DOWN
            pef=19;
        else if (effect=="fade-from-bottom") // PEF_COVER_UP
            pef=21;
        else if (effect=="fade-from-left") // PEF_COVER_RIGHT
            pef=25;
        else if (effect=="fade-from-right") // PEF_COVER_LEFT
            pef=23;
        else if (effect=="fade-to-center") // PEF_CLOSE_ALL
            pef=3;
        else if (effect=="fade-from-center") // PEF_OPEN_ALL
            pef=6;
        else if (effect=="open-vertical") // PEF_OPEN_HORZ; really, no kidding ;)
            pef=4;
        else if (effect=="open-horizontal") // PEF_OPEN_VERT
            pef=5;
        else if (effect=="close-vertical") // PEF_CLOSE_HORZ
            pef=1;
        else if (effect=="close-horizontal") // PEF_CLOSE_VERT
            pef=2;
        else if (effect=="dissolve") // PEF_DISSOLVE; perfect hit ;)
            pef=35;
        else if (effect=="horizontal-checkerboard") // PEF_CHECKBOARD_ACROSS
            pef=17;
        else if (effect=="vertical-checkerboard") // PEF_CHECKBOARD_DOWN
            pef=18;
        else if (effect=="roll-from-left") // PEF_UNCOVER_RIGHT
            pef=26;
        else if (effect=="roll-from-right") // PEF_UNCOVER_LEFT
            pef=24;
        else if (effect=="roll-from-bottom") // PEF_UNCOVER_UP
            pef=22;
        else if (effect=="roll-from-top") // PEF_UNCOVER_DOWN
            pef=20;
        else if (effect=="random") // PEF_RANDOM
            pef=-1;
        else         // we choose a random transition instead of the unsupported ones ;)
            pef=-1;

        pgEffect.setAttribute("value", pef);
        bgPage.appendChild(pgEffect);
    }

    // slide transition sound
    if (m_styleStack.hasChildNode("presentation:sound"))
    {
        QString soundUrl = storeSound(m_styleStack.childNode("presentation:sound").toElement(),
                                      soundElement, doc);

        if (!soundUrl.isNull())
        {
            QDomElement pseElem = doc.createElement("PGSOUNDEFFECT");
            pseElem.setAttribute("soundEffect", 1);
            pseElem.setAttribute("soundFileName", soundUrl);

            bgPage.appendChild(pseElem);
        }
    }

    backgroundElement.appendChild(bgPage);
}

void OoImpressImport::appendName(QDomDocument& doc, QDomElement& e, const QDomElement& object)
{
    if( object.hasAttribute( "draw:name" ))
        {
            QDomElement name = doc.createElement( "OBJECTNAME" );
            name.setAttribute( "objectName", object.attribute( "draw:name" ));
            e.appendChild( name );
        }
}


void OoImpressImport::append2DGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset )
{
    QDomElement orig = doc.createElement( "ORIG" );
    orig.setAttribute( "x", KoUnit::parseValue( object.attribute( "svg:x" ) ) );
    orig.setAttribute( "y", KoUnit::parseValue( object.attribute( "svg:y" ) ) + offset );
    e.appendChild( orig );

    QDomElement size = doc.createElement( "SIZE" );
    size.setAttribute( "width", KoUnit::parseValue( object.attribute( "svg:width" ) ) );
    size.setAttribute( "height", KoUnit::parseValue( object.attribute( "svg:height" ) ) );
    e.appendChild( size );
    if( object.hasAttribute( "draw:transform" ))
        {
            QString transform = object.attribute( "draw:transform" );
            if( transform.contains("rotate ("))
                {
                    transform = transform.remove("rotate (" );
                    transform = transform.left(transform.find(")"));
                    bool ok;
                    double radian = transform.toDouble(&ok);
                    if( ok )
                        {
                            QDomElement angle = doc.createElement( "ANGLE" );
                            //angle is defined as a radian in oo but degree into kpresenter.
                            angle.setAttribute("value", (-1 * ((radian*180)/M_PI)));

                            e.appendChild( angle );
                        }
                }
        }

}

//return true if (x1 < x2) necessary to load correctly start-line and end-line
bool OoImpressImport::appendLineGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset )
{
    double x1 = KoUnit::parseValue( object.attribute( "svg:x1" ) );
    double y1 = KoUnit::parseValue( object.attribute( "svg:y1" ) );
    double x2 = KoUnit::parseValue( object.attribute( "svg:x2" ) );
    double y2 = KoUnit::parseValue( object.attribute( "svg:y2" ) );

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
    return (x1 < x2);
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
            pen.setAttribute( "width", (int) KoUnit::parseValue( m_styleStack.attribute( "svg:stroke-width" ) ) );
        if ( m_styleStack.hasAttribute( "svg:stroke-color" ) )
            pen.setAttribute( "color", m_styleStack.attribute( "svg:stroke-color" ) );
        e.appendChild( pen );
    }
}

void OoImpressImport::appendBrush( QDomDocument& doc, QDomElement& e )
{
    if ( m_styleStack.hasAttribute( "draw:fill" ) )
    {
        const QString fill = m_styleStack.attribute( "draw:fill" );

        if ( fill == "solid" )
        {
            QDomElement brush = doc.createElement( "BRUSH" );
            brush.setAttribute( "style", 1 );
            if ( m_styleStack.hasAttribute( "draw:fill-color" ) )
                brush.setAttribute( "color", m_styleStack.attribute( "draw:fill-color" ) );
            e.appendChild( brush );
        }
        else if ( fill == "hatch" )
        {
            QDomElement brush = doc.createElement( "BRUSH" );
            QString style = m_styleStack.attribute( "draw:fill-hatch-name" );
            QDomElement* draw = m_draws[style];
            if ( draw )
                {
                    if( draw->hasAttribute( "draw:color" ) )
                        brush.setAttribute( "color", draw->attribute( "draw:color" ) );
                    int angle = 0;
                    if( draw->hasAttribute( "draw:rotation" ))
                        {
                            angle = (draw->attribute( "draw:rotation" ).toInt())/10;
                            kdDebug()<<"angle :"<<angle<<endl;
                        }
                    if( draw->hasAttribute( "draw:style" ))
                        {
                            QString styleHash = draw->attribute( "draw:style" );
                            if( styleHash == "single")
                                {
                                    switch( angle )
                                        {
                                        case 0:
                                        case 180:
                                            brush.setAttribute( "style", 9 );
                                            break;
                                        case 45:
                                        case 225:
                                            brush.setAttribute( "style", 12 );
                                            break;
                                        case 90:
                                        case 270:
                                            brush.setAttribute( "style", 10 );
                                            break;
                                        case 135:
                                        case 315:
                                            brush.setAttribute( "style", 13 );
                                            break;
                                        default:
                                            //todo fixme when we will have a kopaint
                                            kdDebug()<<" draw:rotation 'angle' : "<<angle<<endl;
                                            break;
                                        }
                                }
                            else if( styleHash == "double")
                                {
                                    switch( angle )
                                        {
                                        case 0:
                                        case 180:
                                        case 90:
                                        case 270:
                                            brush.setAttribute("style", 11 );
                                            break;
                                        case 45:
                                        case 135:
                                        case 225:
                                        case 315:
                                            brush.setAttribute("style",14 );
                                            break;
                                        default:
                                            //todo fixme when we will have a kopaint
                                            kdDebug()<<" draw:rotation 'angle' : "<<angle<<endl;
                                            break;
                                        }

                                }
                            else if( styleHash == "triple")
                                {
                                    kdDebug()<<" it is not implemented :( \n";
                                }

                        }
                }
            e.appendChild( brush );

        }
        else if ( fill == "gradient" )
        {
            // We have to set a brush with brushstyle != no background fill
            // otherwise the properties dialog for the object won't
            // display the preview for the gradient.
            QDomElement brush = doc.createElement( "BRUSH" );
            brush.setAttribute( "style", 1 );
            e.appendChild( brush );

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

void OoImpressImport::appendImage( QDomDocument& doc, QDomElement& e, QDomElement& p,
                                   const QDomElement& object )
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
    if ( m_styleStack.hasAttribute( "draw:luminance" ) )
    {
        QString str( m_styleStack.attribute( "draw:luminance" ) );
        str = str.remove( '%' );
        settings.setAttribute( "bright", str );
    }
    else
        settings.setAttribute( "bright", 0 );

    settings.setAttribute( "mirrorType", 0 );
    settings.setAttribute( "swapRGB", 0 );
    settings.setAttribute( "depth", 0 );
    e.appendChild( settings );

    QDomElement effects = doc.createElement( "EFFECTS" );
    bool hasEffect = false;
    if ( m_styleStack.hasAttribute( "draw:contrast" ) )
    {
        QString str( m_styleStack.attribute( "draw:contrast" ) );
        str = str.remove( '%' );
        int val = str.toInt();
        val = ( int )( 255.0 *val/100.0 );
        effects.setAttribute( "type", "5" );
        effects.setAttribute( "param1", QString::number( val ) );
        hasEffect = true;
    }
    if ( hasEffect )
        e.appendChild( effects );

    QDomElement key = image.cloneNode().toElement();
    key.setAttribute( "name", "pictures/" + fileName );
    p.appendChild( key );
}

void OoImpressImport::appendBackgroundImage( QDomDocument& doc, QDomElement& e,
                                             QDomElement& p, const QDomElement& object )
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

void OoImpressImport::appendBackgroundGradient( QDomDocument& doc, QDomElement& e,
                                                const QDomElement& object )
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
        int corner = static_cast<int>(KoUnit::parseValue(object.attribute("draw:corner-radius")));
        rounding.setAttribute( "x", corner );
        rounding.setAttribute( "y", corner );
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
            shadow.setAttribute( "distance", KoUnit::parseValue( distance ) );
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
        double x = KoUnit::parseValue( m_styleStack.attribute( "draw:shadow-offset-x" ) );
        double y = KoUnit::parseValue( m_styleStack.attribute( "draw:shadow-offset-y" ) );

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
    if ( m_styleStack.hasAttribute( "draw:size-protect" ) || m_styleStack.hasAttribute("draw:move-protect" ) )
    {
        bool b = ( m_styleStack.attribute("draw:size-protect" ) == "true" ) || ( m_styleStack.attribute("draw:move-protect" ) == "true" );
        if ( b )
        {
            QDomElement protect  = doc.createElement( "PROTECT" );
            protect.setAttribute("state" , b);
            e.appendChild(protect);
        }
    }
}

void OoImpressImport::appendLineEnds( QDomDocument& doc, QDomElement& e, bool orderEndStartLine )
{
    QString attr = orderEndStartLine ? "draw:marker-start" : "draw:marker-end";
    if ( m_styleStack.hasAttribute( attr ) )
    {
        QDomElement lineBegin = doc.createElement( "LINEBEGIN" );
        QString type = m_styleStack.attribute( attr );
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
    attr = orderEndStartLine ? "draw:marker-end" : "draw:marker-start";
    if ( m_styleStack.hasAttribute( attr ) )
    {
        QDomElement lineEnd = doc.createElement( "LINEEND" );
        QString type = m_styleStack.attribute( attr );
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

void OoImpressImport::appendTextObjectMargin( QDomDocument& /*doc*/, QDomElement& e )
{
    if( m_styleStack.hasAttribute( "fo:padding-top" ) )
        e.setAttribute( "btoppt", KoUnit::parseValue( m_styleStack.attribute( "fo:padding-top" ) ) );
    if( m_styleStack.hasAttribute( "fo:padding-bottom" ) )
        e.setAttribute( "bbottompt", KoUnit::parseValue( m_styleStack.attribute( "fo:padding-bottom" ) ) );
    if( m_styleStack.hasAttribute( "fo:padding-left" ) )
        e.setAttribute( "bleftpt", KoUnit::parseValue( m_styleStack.attribute( "fo:padding-left" ) ) );
    if( m_styleStack.hasAttribute( "fo:padding-right" ) )
        e.setAttribute( "brightpt", KoUnit::parseValue( m_styleStack.attribute( "fo:padding-right" ) ) );
}


QDomElement OoImpressImport::parseTextBox( QDomDocument& doc, const QDomElement& textBox )
{
    QDomElement textObjectElement = doc.createElement( "TEXTOBJ" );
    appendTextObjectMargin( doc, textObjectElement );

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

        textObjectElement.setAttribute("verticalValue", 0.0);
    }

    for ( QDomNode text = textBox.firstChild(); !text.isNull(); text = text.nextSibling() )
    {
        QDomElement t = text.toElement();
        QString name = t.tagName();

        QDomElement e;
        if ( name == "text:p" ) // text paragraph
            e = parseParagraph( doc, t );
        else if ( name == "text:h" ) // heading - can this happen in ooimpress?
        {
            e = parseParagraph( doc, t );
        }
        else if ( name == "text:unordered-list" || name == "text:ordered-list" ) // listitem
            e = parseList( doc, t );
        // TODO text:sequence-decls
        else
        {
            kdDebug(30518) << "Unsupported texttype '" << name << "'" << endl;
            continue;
        }

        textObjectElement.appendChild( e );
    }

    return textObjectElement;
}

QDomElement OoImpressImport::parseList( QDomDocument& doc, const QDomElement& list )
{
    //kdDebug(30518) << k_funcinfo << "parsing list"<< endl;

    bool isOrdered;
    if ( list.tagName() == "text:ordered-list" )
        isOrdered = true;
    else
        isOrdered = false;

    // take care of nested lists
    // ### DF: I think this doesn't take care of them the right way. We need to save/parse-whole-list/restore.
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
    // ### Where are the sibling paragraphs of 'e' parsed?

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

    // Style name
    QString styleName = m_styleStack.userStyleName();
    if ( !styleName.isEmpty() )
    {
        QDomElement nameElem = doc.createElement("NAME");
        nameElem.setAttribute("value", styleName);
        p.appendChild(nameElem);
    }

    // Paragraph alignment
    if ( m_styleStack.hasAttribute( "fo:text-align" ) )
    {
        QString align = m_styleStack.attribute( "fo:text-align" );
        if ( align == "center" )
            p.setAttribute( "align", 4 );
        else if ( align == "justify" )
            p.setAttribute( "align", 8 );
        else if ( align == "start" )
            p.setAttribute( "align", 0 );
        else if ( align == "end" )
            p.setAttribute( "align", 2 );
    }
    else
        p.setAttribute( "align", 0 ); // use left aligned as default


    // Offset before and after paragraph
    OoUtils::importTopBottomMargin( p, m_styleStack );

    // Indentation (margins)
    OoUtils::importIndents( p, m_styleStack );

    // Line spacing
    OoUtils::importLineSpacing( p, m_styleStack );

    // Tabulators
    OoUtils::importTabulators( p, m_styleStack );

    // Borders
    OoUtils::importBorders( p, m_styleStack );

    uint pos = 0;

    m_styleStack.save();
    // parse every childnode of the paragraph
    parseSpanOrSimilar( doc, paragraph, p, pos);
    m_styleStack.restore(); // remove possible garbage (should not be needed)

    return p;
}

void OoImpressImport::parseSpanOrSimilar( QDomDocument& doc, const QDomElement& parent,
    QDomElement& outputParagraph, uint& pos)
{
    // parse every child node of the parent
    for( QDomNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement ts = node.toElement();
        QString textData;
        QString tagName = ts.tagName();
        QDomText t = node.toText();

        // Try to keep the order of the tag names by probability of happening
        if ( tagName == "text:span" )
        {
            m_styleStack.save();
            fillStyleStack( ts );
            parseSpanOrSimilar( doc, ts, outputParagraph, pos);
            m_styleStack.restore();
        }
        else if (tagName == "text:s")
        {
            textData = OoUtils::expandWhitespace(ts);
        }
        else if ( tagName == "text:tab-stop" )
        {
            // KPresenter currently uses \t.
            // Known bug: a line with only \t\t\t\t isn't loaded - XML (QDom) strips out whitespace.
            // One more good reason to switch to <text:tab-stop> instead...
            textData = '\t';
        }
        else if ( tagName == "text:line-break" )
        {
            textData = '\n';
        }
        else if ( tagName == "draw:image" )
        {
            textData = '#'; // anchor placeholder
            // TODO
        }
        else if ( tagName == "text:a" )
        {
            m_styleStack.save();
            QString href( ts.attribute("xlink:href") );
            if ( href.startsWith("#") )
            {
                // We have a reference to a bookmark (### TODO)
                // As we do not support it now, treat it as a <text:span> without formatting
                parseSpanOrSimilar( doc, ts, outputParagraph, pos);
            }
            else
            {
#if 0 // TODO
                // The problem is that KPresenter's hyperlink text is not inside the normal text, but for OOWriter it is nearly a <text:span>
                // So we have to fake.
                QDomElement fakeParagraph, fakeFormats;
                uint fakePos=0;
                QString text;
                parseSpanOrSimilar( doc, ts, fakeParagraph, fakeFormats, text, fakePos);
                textData = '#'; // hyperlink placeholder
                QDomElement linkElement (doc.createElement("LINK"));
                linkElement.setAttribute("hrefName",ts.attribute("xlink:href"));
                linkElement.setAttribute("linkName",text);
                appendVariable(doc, ts, pos, "STRING", 9, text, linkElement);
#endif
            }
            m_styleStack.restore();
        }
        else if (tagName == "text:date" // fields
                 || tagName == "text:time"
                 || tagName == "text:page-number"
                 || tagName == "text:file-name"
                 || tagName == "text:author-name"
                 || tagName == "text:author-initials")
        {
            textData = "#";     // field placeholder
            appendField(doc, outputParagraph, ts, pos);
        }
        else if ( t.isNull() ) // no textnode, we must ignore
        {
            kdWarning(30518) << "Ignoring tag " << ts.tagName() << endl;
            continue;
        }
        else
            textData = t.data();

        pos += textData.length();

        QDomElement text = saveHelper(textData, doc);

        //kdDebug(30518) << k_funcinfo << "Para text is: " << paragraph.text() << endl;

        if (m_styleStack.hasAttribute("fo:language")) {
            QString lang = m_styleStack.attribute("fo:language");
            if (lang=="en")
                text.setAttribute("language", "en_US");
            else
                text.setAttribute("language", lang);
        }

        // parse the text-properties
        if ( m_styleStack.hasAttribute( "fo:color" ) )
            text.setAttribute( "color", m_styleStack.attribute( "fo:color" ) );
        if ( m_styleStack.hasAttribute( "fo:font-family" ) )
        {
            // 'Thorndale/Albany' are not known outside OpenOffice so we substitute them
            // with 'Times New Roman/Arial' that look nearly the same.
            if ( m_styleStack.attribute( "fo:font-family" ) == "Thorndale" )
                text.setAttribute( "family", "Times New Roman" );
            else if ( m_styleStack.attribute( "fo:font-family" ) == "Albany" )
                text.setAttribute( "family", "Arial" );
            else
                text.setAttribute( "family", m_styleStack.attribute( "fo:font-family" ).remove( "'" ) );
        }
        if ( m_styleStack.hasAttribute( "fo:font-size" ) )
        {
            double pointSize = m_styleStack.fontSize();
            text.setAttribute( "pointSize", qRound(pointSize) ); // KPresenter uses toInt()!
        }
        if ( m_styleStack.hasAttribute( "fo:font-weight" ) ) // 3.10.24
            if ( m_styleStack.attribute( "fo:font-weight" ) == "bold" )
                text.setAttribute( "bold", 1 );
        if ( m_styleStack.hasAttribute( "fo:font-style" ) )
            if ( m_styleStack.attribute( "fo:font-style" ) == "italic" )
                text.setAttribute( "italic", 1 );

        if ( m_styleStack.hasAttribute( "style:text-position" ) ) // 3.10.17
        {
            QString text_position = m_styleStack.attribute("style:text-position");
            QString value;
            QString relativetextsize;
            OoUtils::importTextPosition( text_position, value, relativetextsize );
            text.setAttribute( "VERTALIGN", value );
            if ( !relativetextsize.isEmpty() )
                text.setAttribute( "relativetextsize", relativetextsize );
        }

        bool wordByWord = (m_styleStack.hasAttribute("fo:score-spaces"))
                          && (m_styleStack.attribute("fo:score-spaces") == "false");

        // strikeout
        if ( m_styleStack.hasAttribute("style:text-crossing-out")
             && m_styleStack.attribute("style:text-crossing-out") != "none")
        {
            QString strikeOutType = m_styleStack.attribute( "style:text-crossing-out" );
            if ( strikeOutType =="double-line" )
            {
                text.setAttribute( "strikeOut", "double" );
                text.setAttribute( "strikeoutstyleline", "solid" );
            }
            else if ( strikeOutType =="thick-line" )
            {
                text.setAttribute( "strikeOut", "single-bold" );
                text.setAttribute( "strikeoutstyleline", "solid" );
            }
            else //if ( strikeOutType == "single-line" ) //fall back to the default strikeout
            {
                text.setAttribute( "strikeOut", "single" );
                text.setAttribute( "strikeoutstyleline", "solid" );
            }

            if (wordByWord)
                text.setAttribute("wordbyword", 1);
        }

        // underlining
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

            if (wordByWord)
                text.setAttribute("wordbyword", 1);
        }

        // background color (property of the paragraph in OOo, of the text in kword/kpresenter)
        if (m_styleStack.hasAttribute( "fo:background-color" ))
        {
            QString bgColor = m_styleStack.attribute("fo:background-color");
            if (bgColor != "transparent")
                text.setAttribute("textbackcolor", bgColor);
        }

        appendShadow( doc, outputParagraph ); // this is necessary to take care of shadowed paragraphs
        outputParagraph.appendChild( text );
    } // for each text span
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
        insertStylesPresentation( fixedStyles.toElement() );
    }

    QDomNode automaticStyles = styles.namedItem( "office:automatic-styles" );
    if ( !automaticStyles.isNull() )
    {
        insertStyles( automaticStyles.toElement() );
        insertStylesPresentation( automaticStyles.toElement() );
    }
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
        //kdDebug(30518) << "Style: '" << name << "' loaded " << endl;
    }
}

void OoImpressImport::insertStylesPresentation( const QDomElement& styles )
{
    for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();

        if ( !e.hasAttribute( "style:name" ) )
            continue;

        QString name = e.attribute( "style:name" );
        m_stylesPresentation.insert( name, new QDomElement( e ) );
        //kdDebug(30518) << "Style: '" << name << "' loaded " << endl;
    }
}


void OoImpressImport::fillStyleStack( const QDomElement& object, bool sticky )
{
    // find all styles associated with an object and push them on the stack
    if ( object.hasAttribute( "presentation:style-name" ) )
    {
        if ( sticky )
            addStyles( m_stylesPresentation[object.attribute( "presentation:style-name" )] );
        else
            addStyles( m_styles[object.attribute( "presentation:style-name" )] );
    }

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

    m_styleStack.push( *style );
}

QString OoImpressImport::storeImage( const QDomElement& object )
{
    // store the picture
    QString url = object.attribute( "xlink:href" ).remove( '#' );
    KArchiveFile* file = (KArchiveFile*) m_zip->directory()->entry( url );

    QString extension = url.mid( url.find( '.' ) );
    QString fileName = QString( "picture%1" ).arg( m_numPicture++ ) + extension;
    KoStoreDevice* out = m_chain->storageFile( "pictures/" + fileName, KoStore::Write );

    if ( file && out )
    {
        QByteArray buffer = file->data();
        out->writeBlock( buffer.data(), buffer.size() );
    }

    return fileName;
}

QString OoImpressImport::storeSound(const QDomElement & object, QDomElement & p, QDomDocument & doc)
{
    QFileInfo fi(m_chain->inputFile()); // handle relative URLs
    QDir::setCurrent(fi.dirPath(true));
    fi.setFile(object.attribute("xlink:href"));
    QString url = fi.absFilePath();

    //kdDebug(30518) << "Sound URL: " << url << endl;

    QFile file(url);
    if (!file.exists())
        return QString::null;

    QString extension = url.mid( url.find( '.' ) );
    QString fileName = QString( "sound%1" ).arg( m_numSound++ ) + extension;
    fileName = "sounds/" + fileName;
    KoStoreDevice* out = m_chain->storageFile( fileName, KoStore::Write );

    if (out)
    {
        if (!file.open(IO_ReadOnly))
            return QString::null;

        QByteArray data(8*1024);

        uint total = 0;
        for ( int block = 0; ( block = file.readBlock(data.data(), data.size()) ) > 0;
              total += block )
            out->writeBlock(data.data(), data.size());

        Q_ASSERT(total == fi.size());

        file.close();
    }
    else
        return QString::null;

    QDomElement key = doc.createElement("FILE");
    key.setAttribute("name", fileName);
    key.setAttribute("filename", url);
    p.appendChild(key);

    return url;
}

QDomElement OoImpressImport::saveHelper(const QString &tmpText, QDomDocument &doc)
{
    QDomElement element=doc.createElement("TEXT");

    if(tmpText.stripWhiteSpace().isEmpty()) // ### careful, this also strips \t and \n ....
        // working around a bug in QDom
        element.setAttribute("whitespace", tmpText.length());

    element.appendChild(doc.createTextNode(tmpText));
    return element;
}

void OoImpressImport::appendPoints(QDomDocument& doc, QDomElement& e, const QDomElement& object)
{
    QDomElement ptsElem = doc.createElement("POINTS");

    QStringList ptList = QStringList::split(' ', object.attribute("draw:points"));

    QString pt_x, pt_y;
    double tmp_x, tmp_y;
    for (QStringList::Iterator it = ptList.begin(); it != ptList.end(); ++it)
    {
        QDomElement point = doc.createElement("Point");

        tmp_x = (*it).section(',',0,0).toInt() / 100;
        tmp_y = (*it).section(',',1,1).toInt() / 100;

        pt_x.setNum(tmp_x);
        pt_x+="mm";

        pt_y.setNum(tmp_y);
        pt_y+="mm";

        point.setAttribute("point_x", KoUnit::parseValue(pt_x));
        point.setAttribute("point_y", KoUnit::parseValue(pt_y));
        ptsElem.appendChild(point);
    }

    e.appendChild(ptsElem);
}

void OoImpressImport::appendField(QDomDocument& doc, QDomElement& e, const QDomElement& object, uint pos)
{
    const QString tag = object.tagName();

    QDomElement custom = doc.createElement("CUSTOM");
    custom.setAttribute("pos", pos);
    QDomElement variable = doc.createElement("VARIABLE");

    if (tag == "text:date")
    {
        QDateTime dt(QDate::fromString(object.attribute("text:date-value"), Qt::ISODate));

        bool fixed = (object.hasAttribute("text:fixed") && object.attribute("text:fixed")=="true");

        if (!dt.isValid()) {
            dt = QDateTime::currentDateTime(); // OOo docs say so :)
            fixed = false;
        }

        QDomElement typeElem = doc.createElement("TYPE");
        typeElem.setAttribute("key", "DATE0locale"); // ### find out the correlation between KOffice and OOo date/time types
        typeElem.setAttribute("type", 0); // VT_DATE
        typeElem.setAttribute("text", object.text());

        variable.appendChild(typeElem);

        const QDate date(dt.date());
        const QTime time(dt.time());
        QDomElement dateElement = doc.createElement("DATE");
        dateElement.setAttribute("subtype", fixed ? 0 : 1); // VST_DATE_FIX, VST_DATE_CURRENT
        dateElement.setAttribute("fix", fixed ? 1 : 0);
        dateElement.setAttribute("day", date.day());
        dateElement.setAttribute("month", date.month());
        dateElement.setAttribute("year", date.year());
        dateElement.setAttribute("hour", time.hour());
        dateElement.setAttribute("minute", time.minute());
        dateElement.setAttribute("second", time.second());
        if (object.hasAttribute("text:date-adjust"))
            dateElement.setAttribute("correct", object.attribute("text:date-adjust"));

        variable.appendChild(dateElement);
    }
    else if (tag == "text:time")
    {
        // Use QDateTime to work around a possible problem of QTime::FromString in Qt 3.2.2
        QDateTime dt(QDateTime::fromString(object.attribute("text:time-value"), Qt::ISODate));

        bool fixed = (object.hasAttribute("text:fixed") && object.attribute("text:fixed")=="true");

        if (!dt.isValid()) {
            dt = QDateTime::currentDateTime(); // OOo docs say so :)
            fixed = false;
        }

        QDomElement typeElem = doc.createElement("TYPE");
        typeElem.setAttribute("key", "TIMElocale"); // ### find out the correlation between KOffice and OOo date/time types
        typeElem.setAttribute("type", 2); // VT_TIME
        typeElem.setAttribute("text", object.text());

        variable.appendChild(typeElem);

        const QTime time(dt.time());
        QDomElement timeElement = doc.createElement("TIME");
        timeElement.setAttribute("subtype", fixed ? 0 : 1); // VST_TIME_FIX, VST_TIME_CURRENT
        timeElement.setAttribute("fix", fixed ? 1 : 0);
        timeElement.setAttribute("hour", time.hour());
        timeElement.setAttribute("minute", time.minute());
        timeElement.setAttribute("second", time.second());
        /*if (object.hasAttribute("text:time-adjust"))
          timeElem.setAttribute("correct", object.attribute("text:time-adjust"));*/ // ### TODO

        variable.appendChild(timeElement);
    }
    else if (tag == "text:page-number")
    {
        QDomElement typeElem = doc.createElement("TYPE");
        typeElem.setAttribute("key", "NUMBER");
        typeElem.setAttribute("type", 4); // VT_PGNUM
        typeElem.setAttribute("text", object.text());

        variable.appendChild(typeElem);

        QDomElement pgNumElem = doc.createElement("PGNUM");

        int subtype = 0;        // VST_PGNUM_CURRENT

        if (object.hasAttribute("text:select-page"))
        {
            const QString select = object.attribute("text:select-page");

            if (select == "previous")
                subtype = 3;    // VST_PGNUM_PREVIOUS
            else if (select == "next")
                subtype = 4;    // VST_PGNUM_NEXT
            else
                subtype = 0;    // VST_PGNUM_CURRENT
        }

        pgNumElem.setAttribute("subtype", subtype);
        pgNumElem.setAttribute("value", object.text());

        variable.appendChild(pgNumElem);
    }
    else if (tag == "text:file-name")
    {
        QDomElement typeElem = doc.createElement("TYPE");
        typeElem.setAttribute("key", "STRING");
        typeElem.setAttribute("type", 8); // VT_FIELD
        typeElem.setAttribute("text", object.text());

        variable.appendChild(typeElem);

        int subtype = 5;

        if (object.hasAttribute("text:display"))
        {
            const QString display = object.attribute("text:display");

            if (display == "path")
                subtype = 1;    // VST_DIRECTORYNAME
            else if (display == "name")
                subtype = 6;    // VST_FILENAMEWITHOUTEXTENSION
            else if (display == "name-and-extension")
                subtype = 0;    // VST_FILENAME
            else
                subtype = 5;    // VST_PATHFILENAME
        }

        QDomElement fileNameElem = doc.createElement("FIELD");
        fileNameElem.setAttribute("subtype", subtype);
        fileNameElem.setAttribute("value", object.text());

        variable.appendChild(fileNameElem);
    }
    else if (tag == "text:author-name"
             || tag == "text:author-initials")
    {
        QDomElement typeElem = doc.createElement("TYPE");
        typeElem.setAttribute("key", "STRING");
        typeElem.setAttribute("type", 8); // VT_FIELD
        typeElem.setAttribute("text", object.text());

        variable.appendChild(typeElem);

        int subtype = 2;        // VST_AUTHORNAME

        if (tag == "text:author-initials")
            subtype = 16;       // VST_INITIAL

        QDomElement authorElem = doc.createElement("FIELD");
        authorElem.setAttribute("subtype", subtype);
        authorElem.setAttribute("value", object.text());

        variable.appendChild(authorElem);
    }

    custom.appendChild(variable);
    e.appendChild(custom);
}

QDomNode OoImpressImport::findAnimationByObjectID(const QString & id,  int & order)
{
    //kdDebug()<<"QDomNode OoImpressImport::findAnimationByObjectID(const QString & id) :"<<id<<endl;
    if (m_animations.isEmpty() )
        return QDomNode();

    animationList *animation = m_animations[id];
    //kdDebug()<<"QDomElement *animation = m_animations[id]; :"<<animation<<endl;
    if ( !animation )
        return QDomNode();
    for (QDomNode node = *( animation->element ); !node.isNull(); node = node.nextSibling())
    {
        QDomElement e = node.toElement();
        //kdDebug()<<"e.tagName() :"<<e.tagName()<<" e.attribute(draw:shape-id) :"<<e.attribute("draw:shape-id")<<endl;
        order = animation->order;
        if (e.tagName()=="presentation:show-shape" && e.attribute("draw:shape-id")==id)
            return node;
    }

    return QDomNode();
}

void OoImpressImport::createPresentationAnimation(const QDomElement& element)
{
    int order = 0;
  for ( QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        QCString tagName = e.tagName().latin1();
        if ( tagName == "presentation:show-shape")
        {
            Q_ASSERT( e.hasAttribute( "draw:shape-id" ) );
            QString name = e.attribute( "draw:shape-id" );
            animationList *lst = new animationList;
            //kdDebug()<<" insert animation style : name :"<<name<<endl;
            QDomElement* ep = new QDomElement( e );
            lst->element = ep;
            lst->order = order;
            m_animations.insert( name, lst );
            ++order;
        }
    }
}

void OoImpressImport::appendObjectEffect(QDomDocument& doc, QDomElement& e, const QDomElement& object,
                                         QDomElement& sound)
{
    int order = 0;
    QDomElement origEffect = findAnimationByObjectID(object.attribute("draw:id"), order).toElement();

    if (origEffect.isNull())
        return;

    QString effect = origEffect.attribute("presentation:effect");
    QString dir = origEffect.attribute("presentation:direction");
    int effVal=0;

    if (effect=="fade")
    {
        if (dir=="from-right")
            effVal=10;          // EF_WIPE_RIGHT
        else if (dir=="from-left")
            effVal=9;           // EF_WIPE_LEFT
        else if (dir=="from-top")
            effVal=11;          // EF_WIPE_TOP
        else if (dir=="from-bottom")
            effVal=12;          // EF_WIPE_BOTTOM
        else
            return;
    }
    else if (effect=="move")
    {
        if (dir=="from-right")
            effVal=1;           // EF_COME_RIGHT
        else if (dir=="from-left")
            effVal=2;           // EF_COME_LEFT
        else if (dir=="from-top")
            effVal=3;           // EF_COME_TOP
        else if (dir=="from-bottom")
            effVal=4;           // EF_COME_BOTTOM
        else if (dir=="from-upper-right")
            effVal=5;           // EF_COME_RIGHT_TOP
        else if (dir=="from-lower-right")
            effVal=6;           // EF_COME_RIGHT_BOTTOM
        else if (dir=="from-upper-left")
            effVal=7;           // EF_COME_LEFT_TOP
        else if (dir=="from-lower-left")
            effVal=8;           // EF_COME_LEFT_BOTTOM
        else
            return;
    }
    else
        return;                 // sorry, no more supported effects :(

    QDomElement effElem = doc.createElement("EFFECTS");
    effElem.setAttribute("effect", effVal);
    e.appendChild(effElem);

    QDomElement presNum = doc.createElement( "PRESNUM" );
    presNum.setAttribute("value", order);
    e.appendChild( presNum );

    // sound effect
    QDomElement origSoundEff = origEffect.namedItem("presentation:sound").toElement();
    if (!origSoundEff.isNull())
    {
        QString soundUrl = storeSound(origSoundEff, sound, doc);

        if (!soundUrl.isNull())
        {
            QDomElement pseElem = doc.createElement("APPEARSOUNDEFFECT");
            pseElem.setAttribute("appearSoundEffect", 1);
            pseElem.setAttribute("appearSoundFileName", soundUrl);

            e.appendChild(pseElem);
        }
    }
}

#include "ooimpressimport.moc"
