/* This file is part of the KDE project
   Copyright (C) 2003 Percy Leonhardt

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

#include "ooimpressexport.h"

#include <qdom.h>
#include <QFile>
#include <qdatetime.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>

typedef KGenericFactory<OoImpressExport> OoImpressExportFactory;
K_EXPORT_COMPONENT_FACTORY( libooimpressexport, OoImpressExportFactory( "kofficefilters" ) )


OoImpressExport::OoImpressExport( KoFilter *, const char *, const QStringList & )
    : KoFilter(parent)
    , m_currentPage( 0 )
    , m_objectIndex( 0 )
    , m_pageHeight( 0 )
    , m_activePage( 0 )
    , m_gridX( -1.0 )
    , m_gridY( -1.0 )
    , m_snapToGrid( false )
    , m_pictureIndex( 0 )
    , m_storeinp( 0L )
    , m_storeout( 0L )
{
}

OoImpressExport::~OoImpressExport()
{
    delete m_storeout;
    delete m_storeinp;
}

KoFilter::ConversionStatus OoImpressExport::convert( const QByteArray & from,
                                                     const QByteArray & to )
{
    kDebug(30518) << "Entering Ooimpress Export filter: " << from << " - " << to << endl;

    if ( ( to != "application/vnd.sun.xml.impress") || (from != "application/x-kpresenter" ) )
    {
        kWarning(30518) << "Invalid mimetypes " << to << " " << from << endl;
        return KoFilter::NotImplemented;
    }

    // read in the KPresenter file
    KoFilter::ConversionStatus preStatus = openFile();

    if ( preStatus != KoFilter::OK )
        return preStatus;

    QDomImplementation impl;
    QDomDocument meta( impl.createDocumentType( "office:document-meta",
                                                "-//OpenOffice.org//DTD OfficeDocument 1.0//EN",
                                                "office.dtd" ) );

    createDocumentMeta( meta );

    // store document meta
    m_storeout = KoStore::createStore( m_chain->outputFile(), KoStore::Write, "", KoStore::Zip );

    if ( !m_storeout )
    {
        kWarning(30518) << "Couldn't open the requested file." << endl;
        return KoFilter::FileNotFound;
    }

    if ( !m_storeout->open( "meta.xml" ) )
    {
        kWarning(30518) << "Couldn't open the file 'meta.xml'." << endl;
        return KoFilter::CreationError;
    }

    Q3CString metaString = meta.toCString();
    //kDebug(30518) << "meta :" << metaString << endl;
    m_storeout->write( metaString , metaString.length() );
    m_storeout->close();

    QDomDocument content( impl.createDocumentType( "office:document-content",
                                                   "-//OpenOffice.org//DTD OfficeDocument 1.0//EN",
                                                   "office.dtd" ) );

    createDocumentContent( content );

    // add the automatic styles
    m_styleFactory.addAutomaticStyles( content, m_styles );

    // store document content
    if ( !m_storeout->open( "content.xml" ) )
    {
        kWarning(30518) << "Couldn't open the file 'content.xml'." << endl;
        return KoFilter::CreationError;
    }

    Q3CString contentString = content.toCString();
    //kDebug(30518) << "content :" << contentString << endl;
    m_storeout->write( contentString , contentString.length() );
    m_storeout->close();

    QDomDocument settings( impl.createDocumentType( "office:document-content",
                                                   "-//OpenOffice.org//DTD OfficeDocument 1.0//EN",
                                                   "office.dtd" ) );

    createDocumentSettings( settings );

    // store document content
    if ( !m_storeout->open( "settings.xml" ) )
    {
        kWarning(30518) << "Couldn't open the file 'settings.xml'." << endl;
        return KoFilter::CreationError;
    }

    Q3CString settingsString = settings.toCString();
    //kDebug(30518) << "content :" << settingsString << endl;
    m_storeout->write( settingsString , settingsString.length() );
    m_storeout->close();


    QDomDocument styles( impl.createDocumentType( "office:document-styles",
                                                  "-//OpenOffice.org//DTD OfficeDocument 1.0//EN",
                                                  "office.dtd" ) );

    createDocumentStyles( styles );

    // store document styles
    if ( !m_storeout->open( "styles.xml" ) )
    {
        kWarning(30518) << "Couldn't open the file 'styles.xml'." << endl;
        return KoFilter::CreationError;
    }

    Q3CString stylesString = styles.toCString();
    //kDebug(30518) << "styles :" << stylesString << endl;
    m_storeout->write( stylesString , stylesString.length() );
    m_storeout->close();

    QDomDocument manifest( impl.createDocumentType( "manifest:manifest",
                                                    "-//OpenOffice.org//DTD Manifest 1.0//EN",
                                                    "Manifest.dtd" ) );

    createDocumentManifest( manifest );

    // store document manifest
    m_storeout->enterDirectory( "META-INF" );
    if ( !m_storeout->open( "manifest.xml" ) )
    {
        kWarning(30518) << "Couldn't open the file 'META-INF/manifest.xml'." << endl;
        return KoFilter::CreationError;
    }

    Q3CString manifestString = manifest.toCString();
    //kDebug(30518) << "manifest :" << manifestString << endl;
    m_storeout->write( manifestString , manifestString.length() );
    m_storeout->close();

    return KoFilter::OK;
}

KoFilter::ConversionStatus OoImpressExport::openFile()
{
    m_storeinp = KoStore::createStore( m_chain->inputFile(), KoStore::Read );

    if ( !m_storeinp )
    {
        kWarning(30518) << "Couldn't open the requested file." << endl;
        return KoFilter::FileNotFound;
    }

    if ( !m_storeinp->open( "maindoc.xml" ) )
    {
        kWarning(30518) << "This file doesn't seem to be a valid KPresenter file" << endl;
        return KoFilter::WrongFormat;
    }

    m_maindoc.setContent( m_storeinp->device() );
    m_storeinp->close();

    if ( m_storeinp->open( "documentinfo.xml" ) )
    {
        m_documentinfo.setContent( m_storeinp->device() );
        m_storeinp->close();
    }
    else
        kWarning(30518) << "Documentinfo do not exist!" << endl;

    emit sigProgress( 10 );

    return KoFilter::OK;
}

void OoImpressExport::createDocumentMeta( QDomDocument & docmeta )
{
    docmeta.appendChild( docmeta.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = docmeta.createElement( "office:document-meta" );
    content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office" );
    content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    content.setAttribute( "xmlns:dc", "http://purl.org/dc/elements/1.1/" );
    content.setAttribute( "xmlns:meta", "http://openoffice.org/2000/meta" );
    content.setAttribute( "office:version", "1.0" );

    QDomNode meta = docmeta.createElement( "office:meta" );

    QDomElement generator = docmeta.createElement( "meta:generator" );
    generator.appendChild( docmeta.createTextNode( "KPresenter 1.5" ) );
    meta.appendChild( generator );

    QDomNode i = m_documentinfo.namedItem( "document-info" );
    if ( !i.isNull() )
    {
        QDomNode n = i.namedItem( "author" ).namedItem( "full-name" );
        if ( !n.isNull() )
        {
            QDomElement fullName = n.toElement();
            QDomElement creator = docmeta.createElement( "meta:initial-creator" );
            creator.appendChild( docmeta.createTextNode( fullName.text() ) );
            meta.appendChild( creator );

            creator = docmeta.createElement( "meta:creator" );
            creator.appendChild( docmeta.createTextNode( fullName.text() ) );
            meta.appendChild( creator );
        }
        n = i.namedItem( "about" ).namedItem( "abstract" );
        if ( !n.isNull() )
        {
            QDomElement user = docmeta.createElement( "dc:description" );
            user.appendChild( n.firstChild() );
            meta.appendChild( user );
        }
        n = i.namedItem( "about" ).namedItem( "keyword" );
        if ( !n.isNull() )
        {
            QDomElement text = n.toElement();
            QDomElement key = docmeta.createElement( "meta:keywords" );
            QDomElement keyword = docmeta.createElement( "meta:keyword" );
            key.appendChild( keyword );
            keyword.appendChild(  docmeta.createTextNode( text.text() ) );
            meta.appendChild( key );
        }
        n = i.namedItem( "about" ).namedItem( "subject" );
        if ( !n.isNull() )
        {
            QDomElement text = n.toElement();
            QDomElement subjet = docmeta.createElement( "dc:subject" );
            subjet.appendChild(  docmeta.createTextNode( text.text() ) );
            meta.appendChild( subjet );
        }
        n = i.namedItem( "about" ).namedItem( "title" );
        if ( !n.isNull() )
        {
            QDomElement text = n.toElement();
            QDomElement title = docmeta.createElement( "dc:title" );
            title.appendChild(  docmeta.createTextNode( text.text() ) );
            meta.appendChild( title );
        }
    }

//     QDomElement statistic = docmeta.createElement( "meta:document-statistic" );
//     statistic.setAttribute( "meta:object-count", 0 );
//     meta.appendChild( data );

    content.appendChild( meta );
    docmeta.appendChild( content );
}

void OoImpressExport::createDocumentStyles( QDomDocument & docstyles )
{
    docstyles.appendChild( docstyles.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = docstyles.createElement( "office:document-content" );
    content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office" );
    content.setAttribute( "xmlns:style", "http://openoffice.org/2000/style" );
    content.setAttribute( "xmlns:text", "http://openoffice.org/2000/text" );
    content.setAttribute( "xmlns:table", "http://openoffice.org/2000/table" );
    content.setAttribute( "xmlns:draw", "http://openoffice.org/2000/drawing" );
    content.setAttribute( "xmlns:fo", "http://www.w3.org/1999/XSL/Format" );
    content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    content.setAttribute( "xmlns:number", "http://openoffice.org/2000/datastyle" );
    content.setAttribute( "xmlns:svg", "http://www.w3.org/2000/svg" );
    content.setAttribute( "xmlns:chart", "http://openoffice.org/2000/chart" );
    content.setAttribute( "xmlns:dr3d", "http://openoffice.org/2000/dr3d" );
    content.setAttribute( "xmlns:math", "http://www.w3.org/1998/Math/MathML" );
    content.setAttribute( "xmlns:form", "http://openoffice.org/2000/form" );
    content.setAttribute( "xmlns:script", "http://openoffice.org/2000/script" );
    content.setAttribute( "office:version", "1.0" );

    // order important here!
    QDomElement styles = docstyles.createElement( "office:styles" );
    m_styleFactory.addOfficeStyles( docstyles, styles );
    content.appendChild( styles );

    QDomElement automatic = docstyles.createElement( "office:automatic-styles" );
    m_styleFactory.addOfficeAutomatic( docstyles, automatic );
    content.appendChild( automatic );

    QDomElement master = docstyles.createElement( "office:master-styles" );
    m_styleFactory.addOfficeMaster( docstyles, master );
    content.appendChild( master );

    docstyles.appendChild( content );
}

void OoImpressExport::createDocumentSettings( QDomDocument & docsetting )
{
    docsetting.appendChild( docsetting.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement setting = docsetting.createElement( "office:document-settings" );
    setting.setAttribute( "xmlns:office", "http://openoffice.org/2000/office");
    setting.setAttribute( "xmlns:config", "http://openoffice.org/2001/config" );
    setting.setAttribute( "office:class", "presentation" );
    setting.setAttribute( "office:version", "1.0" );

    QDomElement begin = docsetting.createElement( "office:settings" );

    QDomElement configItem = docsetting.createElement("config:config-item-set" );
    configItem.setAttribute( "config:name", "view-settings" );

    QDomElement mapIndexed = docsetting.createElement( "config:config-item-map-indexed" );
    mapIndexed.setAttribute("config:name", "Views" );
    configItem.appendChild( mapIndexed );

    //<config:config-item-map-indexed config:name="Views">

    QDomElement mapItem = docsetting.createElement("config:config-item-map-entry" );

    QDomElement attribute =  docsetting.createElement("config:config-item" );
    attribute.setAttribute( "config:name", "SnapLinesDrawing" );
    attribute.setAttribute( "config:type", "string" );
    attribute.appendChild( docsetting.createTextNode( m_helpLine ) );
    mapItem.appendChild( attribute );
    //<config:config-item config:name="SnapLinesDrawing" config:type="string">H5983V700V10777H4518V27601P50000,9000P8021,2890</config:config-item>

    attribute =  docsetting.createElement("config:config-item" );
    attribute.setAttribute( "config:name", "IsSnapToGrid" );
    attribute.setAttribute( "config:type", "boolean" );
    attribute.appendChild( docsetting.createTextNode( m_snapToGrid ? "true" : "false" ) );
    mapItem.appendChild( attribute );

    if (  m_gridX >=0 )
    {
        attribute =  docsetting.createElement("config:config-item" );
        attribute.setAttribute( "config:name", "GridFineWidth" );
        attribute.setAttribute( "config:type", "int" );
        attribute.appendChild( docsetting.createTextNode( QString::number( ( int ) ( KoUnit::toMM( ( m_gridX )  )*100 ) ) ) );
        mapItem.appendChild( attribute );
    }

    if ( m_gridY >=0 )
    {
        attribute =  docsetting.createElement("config:config-item" );
        attribute.setAttribute( "config:name", "GridFineHeight" );
        attribute.setAttribute( "config:type", "int" );
        attribute.appendChild( docsetting.createTextNode( QString::number( ( int ) ( KoUnit::toMM( ( m_gridY )  )*100 ) ) ) );
        mapItem.appendChild( attribute );
    }

    attribute =  docsetting.createElement("config:config-item" );
    attribute.setAttribute( "config:name", "SelectedPage" );
    attribute.setAttribute( "config:type", "short" );
    attribute.appendChild( docsetting.createTextNode( QString::number( m_activePage ) ) );
    mapItem.appendChild( attribute );


    mapIndexed.appendChild( mapItem );

    begin.appendChild( configItem );

    setting.appendChild( begin );


    docsetting.appendChild( setting );

}

void OoImpressExport::createDocumentContent( QDomDocument & doccontent )
{
    doccontent.appendChild( doccontent.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = doccontent.createElement( "office:document-content" );
    content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office");
    content.setAttribute( "xmlns:style", "http://openoffice.org/2000/style" );
    content.setAttribute( "xmlns:text", "http://openoffice.org/2000/text" );
    content.setAttribute( "xmlns:table", "http://openoffice.org/2000/table" );
    content.setAttribute( "xmlns:draw", "http://openoffice.org/2000/drawing" );
    content.setAttribute( "xmlns:fo", "http://www.w3.org/1999/XSL/Format" );
    content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    content.setAttribute( "xmlns:number", "http://openoffice.org/2000/datastyle" );
    content.setAttribute( "xmlns:svg", "http://www.w3.org/2000/svg" );
    content.setAttribute( "xmlns:chart", "http://openoffice.org/2000/chart" );
    content.setAttribute( "xmlns:dr3d", "http://openoffice.org/2000/dr3d" );
    content.setAttribute( "xmlns:math", "http://www.w3.org/1998/Math/MathML" );
    content.setAttribute( "xmlns:form", "http://openoffice.org/2000/form" );
    content.setAttribute( "xmlns:script", "http://openoffice.org/2000/script" );
    content.setAttribute( "xmlns:presentation", "http://openoffice.org/2000/presentation" );
    content.setAttribute( "office:class", "presentation" );
    content.setAttribute( "office:version", "1.0" );

    QDomElement script = doccontent.createElement( "office:script" );
    content.appendChild( script );

    m_styles = doccontent.createElement( "office:automatic-styles" );
    content.appendChild( m_styles );

    QDomElement body = doccontent.createElement( "office:body" );
    exportBody( doccontent, body );
    content.appendChild( body );

    doccontent.appendChild( content );
}

void OoImpressExport::createDocumentManifest( QDomDocument & docmanifest )
{
    docmanifest.appendChild( docmanifest.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement manifest = docmanifest.createElement( "manifest:manifest" );
    manifest.setAttribute( "xmlns:manifest", "http://openoffice.org/2001/manifest" );

    QDomElement entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "application/vnd.sun.xml.impress" );
    entry.setAttribute( "manifest:full-path", "/" );
    manifest.appendChild( entry );

    QMap<QString, QString>::Iterator it;
    for ( it = m_pictureLst.begin(); it != m_pictureLst.end(); ++it )
    {
        entry = docmanifest.createElement( "manifest:file-entry" );
        entry.setAttribute( "manifest:media-type", it.data() );
        entry.setAttribute( "manifest:full-path", it.key() );
        manifest.appendChild( entry );
    }

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "content.xml" );
    manifest.appendChild( entry );

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "styles.xml" );
    manifest.appendChild( entry );

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "meta.xml" );
    manifest.appendChild( entry );

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "settings.xml" );
    manifest.appendChild( entry );

    docmanifest.appendChild( manifest );
}

QString OoImpressExport::pictureKey( QDomElement &elem )
{
    // Default date/time is the *nix epoch: 1970-01-01 00:00:00,000
    int year=1970, month=1, day=1;
    int hour=0, minute=0, second=0, msec=0; // We must initialize to zero, as not all compilers are C99-compliant
    if ( elem.tagName() ==  "KEY" )
    {
        if( elem.hasAttribute( "year" ) )
            year=elem.attribute( "year" ).toInt();
        if( elem.hasAttribute( "month" ) )
            month=elem.attribute( "month" ).toInt();
        if( elem.hasAttribute( "day" ) )
            day=elem.attribute( "day" ).toInt();
        if( elem.hasAttribute( "hour" ) )
            hour=elem.attribute( "hour" ).toInt();
        if( elem.hasAttribute( "minute" ) )
            minute=elem.attribute( "minute" ).toInt();
        if( elem.hasAttribute( "second" ) )
            second=elem.attribute( "second" ).toInt();
        if( elem.hasAttribute( "msec" ) )
            msec=elem.attribute( "msec" ).toInt();
    }
    QDateTime key;
    key.setDate( QDate( year, month, day ) );
    key.setTime( QTime( hour, minute, second, msec ) );
    return key.toString();
}

void OoImpressExport::createPictureList( QDomNode &pictures )
{
    pictures = pictures.firstChild();
    for( ; !pictures.isNull(); pictures = pictures.nextSibling() )
    {
        if ( pictures.isElement() )
        {
            QDomElement element = pictures.toElement();
            if ( element.tagName() ==  "KEY" )
            {
                //kDebug(30518)<<"element.attribute( name ) :"<<element.attribute( "name" )<<endl;
                m_kpresenterPictureLst.insert( pictureKey( element ), element.attribute( "name" ) );
            }
            else
                kDebug(30518)<<" Tag not recognize :"<<element.tagName()<<endl;
        }
    }
}

void OoImpressExport::createAttribute( QDomNode &attributeValue )
{
    QDomElement elem = attributeValue.toElement();
    if(elem.hasAttribute("activePage"))
        m_activePage=elem.attribute("activePage").toInt();
    if(elem.hasAttribute("gridx"))
        m_gridX = elem.attribute("gridx").toDouble();
    if(elem.hasAttribute("gridy"))
        m_gridY = elem.attribute("gridy").toDouble();
    if(elem.hasAttribute("snaptogrid"))
        m_snapToGrid = (bool)elem.attribute("snaptogrid").toInt();
}

void OoImpressExport::createHelpLine( QDomNode &helpline )
{
    helpline = helpline.firstChild();
    QDomElement helplines;
    for( ; !helpline.isNull(); helpline = helpline.nextSibling() )
    {
        if ( helpline.isElement() )
        {
            helplines = helpline.toElement();
            if ( helplines.tagName()=="Vertical" )
            {
                int tmpX = ( int ) ( KoUnit::toMM( helplines.attribute("value").toDouble() )*100 );
                m_helpLine+="V"+QString::number( tmpX );
            }
            else if ( helplines.tagName()=="Horizontal" )
            {
                int tmpY = ( int ) ( KoUnit::toMM( helplines.attribute("value").toDouble() )*100 );
                m_helpLine+="H"+QString::number( tmpY );
            }
            else if ( helplines.tagName()=="HelpPoint" )
            {
                QString str( "P%1,%2" );
                int tmpX = ( int ) ( KoUnit::toMM( helplines.attribute("posX").toDouble()  )*100 );
                int tmpY = ( int ) ( KoUnit::toMM( helplines.attribute("posY").toDouble() )*100 );
                m_helpLine+=str.arg( QString::number( tmpX ) ).arg( QString::number( tmpY ) );
            }
        }
    }
    //kDebug(30518)<<"m_helpLine :"<<m_helpLine<<endl;
}


void OoImpressExport::exportBody( QDomDocument & doccontent, QDomElement & body )
{
    QDomNode doc = m_maindoc.namedItem( "DOC" );
    QDomNode paper = doc.namedItem( "PAPER" );
    QDomNode background = doc.namedItem( "BACKGROUND" );
    QDomNode header = doc.namedItem( "HEADER" );
    QDomNode footer = doc.namedItem( "FOOTER" );
    QDomNode titles = doc.namedItem( "PAGETITLES" );
    QDomNode notes = doc.namedItem( "PAGENOTES" );
    QDomNode objects = doc.namedItem( "OBJECTS" );
    QDomNode pictures = doc.namedItem( "PICTURES" );
    QDomNode sounds = doc.namedItem( "SOUNDS" );
    QDomNode helpline = doc.namedItem( "HELPLINES" );
    QDomNode attributeValue = doc.namedItem( "ATTRIBUTES" );
    QDomNode infiniLoop = doc.namedItem( "INFINITLOOP" );
    QDomNode manualSwitch = doc.namedItem( "MANUALSWITCH" );
    QDomNode customSlideShow = doc.namedItem( "CUSTOMSLIDESHOWCONFIG" );
    QDomNode customSlideShowDefault = doc.namedItem( "DEFAULTCUSTOMSLIDESHOWNAME" );

    QDomNode bgpage = background.firstChild();

    createPictureList( pictures );

    createHelpLine( helpline );

    createAttribute( attributeValue );

    // store the paper settings
    QDomElement p = paper.toElement();
    m_masterPageStyle = m_styleFactory.createPageMasterStyle( p );
    m_pageHeight = p.attribute( "ptHeight" ).toFloat();

    m_currentPage = 1;

    // parse all pages
    QDomNode note = notes.firstChild();
    for ( QDomNode title = titles.firstChild(); !title.isNull() && !note.isNull();
          title = title.nextSibling(), note = note.nextSibling() )
    {
        // create the page style and ignore the fact that there may
        // be less backgrounds than pages
        QDomElement bg = bgpage.toElement();
        QString ps = m_styleFactory.createPageStyle( bg );
        bgpage = bgpage.nextSibling();

        QDomElement t = title.toElement();
        QDomElement drawPage = doccontent.createElement( "draw:page" );
        drawPage.setAttribute( "draw:name", t.attribute( "title" ) );
        drawPage.setAttribute( "draw:style-name", ps );
        drawPage.setAttribute( "draw:id", m_currentPage );
        drawPage.setAttribute( "draw:master-page-name", m_masterPageStyle );

        appendObjects( doccontent, objects, drawPage );

        QDomElement noteElement = note.toElement();
        appendNote( doccontent, noteElement, drawPage );
        body.appendChild( drawPage );
        m_currentPage++;
    }
    int infiniLoopValue = -1;
    int manualSwitchValue = -1;
    if ( !infiniLoop.isNull() && infiniLoop.toElement().hasAttribute( "value" ))
    {
        bool ok;
        int val = infiniLoop.toElement().attribute( "value" ).toInt( &ok );
        if ( ok )
            infiniLoopValue = val;
    }
    if ( !manualSwitch.isNull() && manualSwitch.toElement().hasAttribute( "value" ))
    {
        bool ok;
        int val = manualSwitch.toElement().attribute( "value" ).toInt( &ok );
        if ( ok )
            manualSwitchValue = val;
    }
    if ( infiniLoopValue != -1 || manualSwitchValue != -1 || !customSlideShowDefault.isNull())
    {
        QDomElement settings = doccontent.createElement( "presentation:settings" );
        if ( infiniLoopValue !=-1 )
            settings.setAttribute( "presentation:force-manual", ( manualSwitchValue==1 ) ? "true" : "false" );
        if ( manualSwitchValue != -1 )
            settings.setAttribute( "presentation:endless", ( infiniLoopValue==1 ) ? "true": "false" );
        if ( !customSlideShowDefault.isNull() )
            settings.setAttribute( "presentation:show", customSlideShowDefault.toElement().attribute( "name" ) );

        if ( !customSlideShow.isNull() )
        {
            for ( QDomNode customPage = customSlideShow.firstChild(); !customPage.isNull();
                  customPage = customPage.nextSibling() )
            {
                QDomElement show = customPage.toElement();
                if ( !show.isNull() && show.tagName()=="CUSTOMSLIDESHOW" )
                {
                    QDomElement showElement = doccontent.createElement( "presentation:show" );
                    showElement.setAttribute( "presentation:name",show.attribute( "name" ) );
                    showElement.setAttribute( "presentation:pages",show.attribute( "pages" ) );
                    settings.appendChild( showElement );
                }
            }
        }
        body.appendChild( settings );
    }
}


void OoImpressExport::appendObjects(QDomDocument & doccontent, QDomNode &objects, QDomElement &drawPage)
{
    // I am not sure if objects are always stored sorted so I parse all
    // of them to find the ones belonging to a certain page.
    for ( QDomNode object = objects.firstChild(); !object.isNull();
          object = object.nextSibling() )
    {
        QDomElement o = object.toElement();

        QDomElement orig = o.namedItem( "ORIG" ).toElement();
        float y = orig.attribute( "y" ).toFloat();

        if ( y < m_pageHeight * ( m_currentPage - 1 ) ||
             y >= m_pageHeight * m_currentPage )
            continue; // object not on current page

        switch( o.attribute( "type" ).toInt() )
        {
        case 0: // image
            appendPicture( doccontent, o, drawPage );
            break;
        case 1: // line
            appendLine( doccontent, o, drawPage );
            break;
        case 2: // rectangle
            appendRectangle( doccontent, o, drawPage );
            break;
        case 3: // circle, ellipse
            appendEllipse( doccontent, o, drawPage );
            break;
        case 4: // textbox
            appendTextbox( doccontent, o, drawPage );
            break;
        case 5:
            kDebug(30518)<<" autoform not implemented\n";
            break;
        case 6:
            kDebug(30518)<<" clipart not implemented\n";
            break;
        case 8: // pie, chord, arc
            appendEllipse( doccontent, o, drawPage, true );
            break;
        case 9: //part
            kDebug(30518)<<" part object not implemented \n";
            break;
        case 10:
            appendGroupObject( doccontent, o, drawPage );
            break;
        case 11:
            kDebug(30518)<<" free hand not implemented\n";
            break;
        case 12: // polyline
            appendPolyline( doccontent, o, drawPage );
            break;
        case 13: //OT_QUADRICBEZIERCURVE = 13
        case 14: //OT_CUBICBEZIERCURVE = 14
            //todo
            // "draw:path"
            break;
        case 15: // polygon
        case 16: // close polygone
            appendPolyline( doccontent, o, drawPage, true /*polygon*/ );
            break;
        }
        ++m_objectIndex;
    }

}

void OoImpressExport::appendGroupObject( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement groupElement = doc.createElement( "draw:g" );
    QDomNode objects = source.namedItem( "OBJECTS" );
    appendObjects( doc, objects, groupElement);
    target.appendChild( groupElement );
}

void OoImpressExport::appendNote( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QString noteText = source.attribute("note");
    //kDebug(30518)<<"noteText :"<<noteText<<endl;
    if ( noteText.isEmpty() )
        return;
    QDomElement noteElement = doc.createElement( "presentation:notes" );
    QDomElement noteTextBox = doc.createElement( "draw:text-box" );

    //TODO : add draw:text-box size :
    //<draw:text-box draw:style-name="gr2" draw:text-style-name="P2" draw:layer="layout" svg:width="13.336cm" svg:height="56.288cm" svg:x="-0.54cm" svg:y="-14.846cm">

    QStringList text = QStringList::split( "\n", noteText );
    for ( QStringList::Iterator it = text.begin(); it != text.end(); ++it ) {
        QDomElement tmp = doc.createElement( "text:p" );
        tmp.appendChild( doc.createTextNode( *it ) );
        noteTextBox.appendChild( tmp );
    }
    noteElement.appendChild( noteTextBox );
    target.appendChild( noteElement );
}

void OoImpressExport::appendTextbox( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement textbox = doc.createElement( "draw:text-box" );

    QDomNode textobject = source.namedItem( "TEXTOBJ" );

    // create the graphic style
    QString gs = m_styleFactory.createGraphicStyle( source );
    textbox.setAttribute( "draw:style-name", gs );

    // set the geometry
    set2DGeometry( source, textbox );

    // parse every paragraph
    for ( QDomNode paragraph = textobject.firstChild(); !paragraph.isNull();
          paragraph = paragraph.nextSibling() )
    {
        QDomElement p = paragraph.toElement();
        appendParagraph( doc, p, textbox );
    }

    target.appendChild( textbox );
}

void OoImpressExport::appendParagraph( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement paragraph = doc.createElement( "text:p" );

    // create the paragraph style
    QString ps = m_styleFactory.createParagraphStyle( source );
    paragraph.setAttribute( "text:style-name", ps );

    // parse every text element
    for ( QDomNode text = source.firstChild(); !text.isNull();
          text = text.nextSibling() )
    {
        if ( text.nodeName() == "TEXT" )
        {
            QDomElement t = text.toElement();
            appendText( doc, t, paragraph );
        }
    }

    // take care of lists
    QDomNode counter = source.namedItem( "COUNTER" );
    if ( !counter.isNull() )
    {
        QDomElement c = counter.toElement();
        int type = c.attribute( "type" ).toInt();

        int level = 1;
        if ( c.hasAttribute( "depth" ) )
            level = c.attribute( "depth" ).toInt() + 1;

        QDomElement endOfList = target;
        for ( int l = 0; l < level;  l++ )
        {
            QDomElement list;
            if ( type == 1 )
            {
                list = doc.createElement( "text:ordered-list" );
                list.setAttribute( "text:continue-numbering", "true" );
            }
            else
                list = doc.createElement( "text:unordered-list" );

            if ( l == 0 )
            {
                // create the list style
                QString ls = m_styleFactory.createListStyle( c );
                list.setAttribute( "text:style-name", ls );
            }

            QDomElement item = doc.createElement( "text:list-item" );
            list.appendChild( item );
            endOfList.appendChild( list );
            endOfList = item;
        }

        endOfList.appendChild( paragraph );
    }
    else
        target.appendChild( paragraph );
}

void OoImpressExport::appendText( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement textspan = doc.createElement( "text:span" );

    // create the text style
    QString ts = m_styleFactory.createTextStyle( source );
    textspan.setAttribute( "text:style-name", ts );

    textspan.appendChild( doc.createTextNode( source.text() ) );
    target.appendChild( textspan );
}

void OoImpressExport::appendPicture( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement image = doc.createElement( "draw:image" );

    // create the graphic style
    QString gs = m_styleFactory.createGraphicStyle( source );
    image.setAttribute( "draw:style-name", gs );
    QDomElement key = source.namedItem( "KEY" ).toElement();

    QString pictureName = QString( "Picture/Picture%1" ).arg( m_pictureIndex );

    image.setAttribute( "xlink:type", "simple" );
    image.setAttribute( "xlink:show", "embed" );
    image.setAttribute( "xlink:actuate", "onLoad");

    if ( !key.isNull() )
    {
        QString str = pictureKey( key );
        QString returnstr = m_kpresenterPictureLst[str];
        const int pos=returnstr.findRev('.');
        if (pos!=-1)
        {
            const QString extension( returnstr.mid(pos+1) );
            pictureName +="."+extension;
        }

        if ( m_storeinp->open( returnstr ) )
        {
            if ( m_storeout->open( pictureName ) )
            {
                QByteArray data(8*1024);
                uint total = 0;
                for ( int block = 0; ( block = m_storeinp->read(data.data(), data.size()) ) > 0;
                      total += block )
                    m_storeout->write(data.data(), data.size());
                m_storeout->close();
                m_storeinp->close();
            }
        }
    }
    image.setAttribute( "xlink:href", "#" + pictureName );

// set the geometry
    set2DGeometry( source, image );
    target.appendChild( image );

    m_pictureLst.insert( pictureName , "image/png" );

    ++m_pictureIndex;
}


void OoImpressExport::appendLine( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement line = doc.createElement( "draw:line" );

    // create the graphic style
    QString gs = m_styleFactory.createGraphicStyle( source );
    line.setAttribute( "draw:style-name", gs );

    // set the geometry
    setLineGeometry( source, line );

    target.appendChild( line );
}

void OoImpressExport::appendRectangle( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement rectangle = doc.createElement( "draw:rect" );

    // create the graphic style
    QString gs = m_styleFactory.createGraphicStyle( source );
    rectangle.setAttribute( "draw:style-name", gs );

    // set the geometry
    set2DGeometry( source, rectangle );

    target.appendChild( rectangle );
}

void OoImpressExport::appendPolyline( QDomDocument & doc, QDomElement & source, QDomElement & target,  bool _poly)
{
    QDomElement polyline = doc.createElement( _poly ? "draw:polygon" : "draw:polyline" );

    // create the graphic style
    QString gs = m_styleFactory.createGraphicStyle( source );
    polyline.setAttribute( "draw:style-name", gs );

    // set the geometry
    set2DGeometry( source, polyline, false, true /*multipoint*/ );

    target.appendChild( polyline );
}

void OoImpressExport::appendEllipse( QDomDocument & doc, QDomElement & source, QDomElement & target, bool pieObject )
{
    QDomElement size = source.namedItem( "SIZE" ).toElement();

    double width = size.attribute( "width" ).toDouble();
    double height = size.attribute( "height" ).toDouble();

    QDomElement ellipse = doc.createElement( (width == height) ? "draw:circle" : "draw:ellipse" );

    // create the graphic style
    QString gs = m_styleFactory.createGraphicStyle( source );
    ellipse.setAttribute( "draw:style-name", gs );

    // set the geometry
    set2DGeometry( source, ellipse, pieObject );

    target.appendChild( ellipse );
}

void OoImpressExport::set2DGeometry( QDomElement & source, QDomElement & target, bool pieObject, bool multiPoint )
{
    QDomElement orig = source.namedItem( "ORIG" ).toElement();
    QDomElement size = source.namedItem( "SIZE" ).toElement();
    QDomElement name = source.namedItem( "OBJECTNAME").toElement();
    float y = orig.attribute( "y" ).toFloat();
    y -= m_pageHeight * ( m_currentPage - 1 );

    QDomElement angle = source.namedItem( "ANGLE").toElement();
    if ( !angle.isNull() )
    {
        QString returnAngle = rotateValue( angle.attribute( "value" ).toDouble() );
        if ( !returnAngle.isEmpty() )
            target.setAttribute("draw:transform",returnAngle );
    }

    target.setAttribute( "draw:id",  QString::number( m_objectIndex ) );
    target.setAttribute( "svg:x", StyleFactory::toCM( orig.attribute( "x" ) ) );
    target.setAttribute( "svg:y", QString( "%1cm" ).arg( KoUnit::toCM( y ) ) );
    target.setAttribute( "svg:width", StyleFactory::toCM( size.attribute( "width" ) ) );
    target.setAttribute( "svg:height", StyleFactory::toCM( size.attribute( "height" ) ) );
    QString nameStr = name.attribute("objectName");
    if( !nameStr.isEmpty() )
        target.setAttribute( "draw:name", nameStr );
    if ( pieObject )
    {
        QDomElement pie = source.namedItem( "PIETYPE").toElement();
        if( !pie.isNull() )
        {
            int typePie = pie.attribute("value").toInt();
            switch( typePie )
            {
            case 0:
                target.setAttribute( "draw:kind", "section");
                break;
            case 1:
                target.setAttribute( "draw:kind", "arc");
                break;
            case 2:
                target.setAttribute( "draw:kind", "cut");
                break;
            default:
                kDebug(30518)<<" type unknown : "<<typePie<<endl;
                break;
            }
        }
        else
            target.setAttribute( "draw:kind", "section");//by default
	QDomElement pieAngle = source.namedItem( "PIEANGLE").toElement();
	int startangle = 45;
	if( !pieAngle.isNull() )
        {
	    startangle = (pieAngle.attribute("value").toInt())/16;
	    target.setAttribute( "draw:start-angle", startangle);
        }
	else
        {
	    //default value take it into kppieobject
	    target.setAttribute( "draw:start-angle", 45 );
        }
	QDomElement pieLength = source.namedItem( "PIELENGTH").toElement();
	if( !pieLength.isNull() )
        {
	    int value = pieLength.attribute("value").toInt();
	    value = value /16;
	    value = value + startangle;
	    target.setAttribute( "draw:end-angle", value );
        }
	else
        {
	    //default value take it into kppieobject
	    //default is 90° into kpresenter
	    target.setAttribute( "draw:end-angle", (90+startangle) );
        }
    }
    if ( multiPoint )
    {
        //loadPoint
        QDomElement point = source.namedItem( "POINTS" ).toElement();
        if ( !point.isNull() ) {
            QDomElement elemPoint = point.firstChild().toElement();
            QString listOfPoint;
            int maxX=0;
            int maxY=0;
            while ( !elemPoint.isNull() ) {
                if ( elemPoint.tagName() == "Point" ) {
                    int tmpX = 0;
                    int tmpY = 0;
                    if( elemPoint.hasAttribute( "point_x" ) )
                        tmpX = ( int ) ( KoUnit::toMM( elemPoint.attribute( "point_x" ).toDouble() )*100 );
                    if( elemPoint.hasAttribute( "point_y" ) )
                        tmpY = ( int ) ( KoUnit::toMM(elemPoint.attribute( "point_y" ).toDouble() )*100 );
                    if ( !listOfPoint.isEmpty() )
                        listOfPoint += QString( " %1,%2" ).arg( tmpX ).arg( tmpY );
                    else
                        listOfPoint = QString( "%1,%2" ).arg( tmpX ).arg( tmpY );
                    maxX = qMax( maxX, tmpX );
                    maxY = qMax( maxY, tmpY );
                }
                elemPoint = elemPoint.nextSibling().toElement();
            }
            target.setAttribute( "draw:points", listOfPoint );
            target.setAttribute( "svg:viewBox", QString( "0 0 %1 %2" ).arg( maxX ).arg( maxY ) );
        }
    }
}

QString OoImpressExport::rotateValue( double val )
{
    QString str;
    if ( val!=0.0 )
    {
        double value = -1 * ( ( double )val* M_PI )/180.0;
        str=QString( "rotate (%1)" ).arg( value );
    }
    return str;
}


void OoImpressExport::setLineGeometry( QDomElement & source, QDomElement & target )
{
    QDomElement orig = source.namedItem( "ORIG" ).toElement();
    QDomElement size = source.namedItem( "SIZE" ).toElement();
    QDomElement linetype = source.namedItem( "LINETYPE" ).toElement();
    QDomElement name = source.namedItem( "OBJECTNAME").toElement();
    QDomElement angle = source.namedItem( "ANGLE").toElement();
    if ( !angle.isNull() )
    {
        QString returnAngle = rotateValue( angle.attribute( "value" ).toDouble() );
        if ( !returnAngle.isEmpty() )
            target.setAttribute("draw:transform",returnAngle );
    }
    float x1 = orig.attribute( "x" ).toFloat();
    float y1 = orig.attribute( "y" ).toFloat();
    float x2 = size.attribute( "width" ).toFloat();
    float y2 = size.attribute( "height" ).toFloat();
    int type = 0;
    if ( !linetype.isNull() )
        type = linetype.attribute( "value" ).toInt();
    y1 -= m_pageHeight * ( m_currentPage - 1 );
    x2 += x1;
    y2 += y1;

    target.setAttribute( "draw:id",  QString::number( m_objectIndex ) );
    QString xpos1 = StyleFactory::toCM( orig.attribute( "x" ) );
    QString xpos2 = QString( "%1cm" ).arg( KoUnit::toCM( x2 ) );

    if ( type == 0 )
    {
        target.setAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCM( y2/2.0 ) ) );
        target.setAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCM( y2/2.0 ) ) );
    }
    else if ( type == 1 )
    {
        target.setAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCM( y1 ) ) );
        target.setAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCM( y2 ) ) );
        xpos1 = QString( "%1cm" ).arg( KoUnit::toCM( x1/2.0 ) );
        xpos2 = xpos1;
    }
    else if ( type == 3 ) // from left bottom to right top
    {
        target.setAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCM( y2 ) ) );
        target.setAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCM( y1 ) ) );
    }
    else // from left top to right bottom
    {
        target.setAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCM( y1 ) ) );
        target.setAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCM( y2 ) ) );
    }
    target.setAttribute( "svg:x1", xpos1 );
    target.setAttribute( "svg:x2", xpos2 );

    QString nameStr = name.attribute("objectName");
    if( !nameStr.isEmpty() )
      target.setAttribute( "draw:name", nameStr );
}

#include "ooimpressexport.moc"
