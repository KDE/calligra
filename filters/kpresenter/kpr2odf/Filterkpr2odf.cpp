/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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
   Boston, MA  02110-1301  USA.
*/

//Qt includes
#include <QByteArray>
#include <QBuffer>
#include <QStringList>
#include <QString>

//KDE includes
#include <kgenericfactory.h>
#include <kdebug.h>

//KOffice includes
#include <KoStore.h>
#include <KoOdfWriteStore.h>
#include <KoDocumentInfo.h>
#include <KoFilterChain.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdf.h>
#include <KoGenStyle.h>

#include "Filterkpr2odf.h"

typedef KGenericFactory<Filterkpr2odf> Filterkpr2odfFactory;
K_EXPORT_COMPONENT_FACTORY( libFilterkpr2odf, Filterkpr2odfFactory( "kofficefilters" ) )

Filterkpr2odf::Filterkpr2odf(QObject *parent,const QStringList&)
: KoFilter(parent)
{
}

KoFilter::ConversionStatus Filterkpr2odf::convert( const QByteArray& from, const QByteArray& to )
{
    //Check that the type of files are right
    if ( from != "application/x-kpresenter"
         || to != "application/vnd.oasis.opendocument.presentation" )
        return KoFilter::BadMimeType;

    //open the input file file
    KoStore* input = KoStore::createStore( m_chain->inputFile(), KoStore::Read );
    if ( !input )
        return KoFilter::FileNotFound;

    //Load the document
     //Load maindoc.xml
    if( !input->open( "maindoc.xml" ) )
        return KoFilter::WrongFormat;
    m_mainDoc.setContent( input->device(), false );
    input->close();

     //Load documentinfo.xml
    if( !input->open( "documentinfo.xml" ) )
        return KoFilter::WrongFormat;

    m_documentInfo.setContent( input->device(), false );
    input->close();

     //Load the preview picture
     QByteArray* preview = new QByteArray();
     if( !input->extractFile("preview.png", *preview) )
         return KoFilter::WrongFormat;

    //If we find everything let the saving begin

    //Create the output file
    KoStore* output = KoStore::createStore( m_chain->outputFile(), KoStore::Write
                                           ,KoOdf::mimeType( KoOdf::Presentation ), KoStore::Zip );

    if ( !output )
        return KoFilter::StorageCreationError;

    KoOdfWriteStore odfWriter( output );
    KoXmlWriter* manifest = odfWriter.manifestWriter(KoXmlNS::presentation);

    //Save the preview picture
    output->enterDirectory( "Thumbnails" );
    output->open( "thubnail.png" );
    output->write( *preview );
    output->close();
    output->leaveDirectory();
    manifest->addManifestEntry( "Thubnails/", "" );
    manifest->addManifestEntry( "Thubnails/thubnail.png", "" );
    delete preview;

    //Write the Pictures directory and its children, also fill the m_pictures hash
    createImageList( output, input, manifest );
    delete input;

    //Create the content.xml file
    KoXmlWriter *content = odfWriter.contentWriter();
    KoXmlWriter *body = odfWriter.bodyWriter();
    convertContent( body );
    m_styles.saveOdfAutomaticStyles( content, false );
    odfWriter.closeContentWriter();
    manifest->addManifestEntry( "content.xml", "text/xml" );

    //Create the styles.xml file
    m_styles.saveOdfStylesDotXml( output, manifest );

    //Create settings.xml
    output->open( "settings.xml" );
    KoStoreDevice device( output );
    KoXmlWriter *settings = KoOdfWriteStore::createOasisXmlWriter( &device, "office:document-settings" );
//     settings->startElement( "office:settings" );
//     settings->startElement( "config:config-item-set" );
//     settings->endElement();//config:config-item-set
//     settings->endElement();//office:settings
    settings->endElement();//office:document-settings
    settings->endDocument();
    output->close();
    manifest->addManifestEntry( "settings.xml", "text/xml" );

    //Create the meta.xml file
    output->open( "meta.xml" );
    KoDocumentInfo* meta = new KoDocumentInfo();
    meta->load( m_documentInfo );
    meta->saveOasis( output );
    delete meta;
    output->close();
    manifest->addManifestEntry( "meta.xml", "text/xml" );

    //Write the document manifest
    odfWriter.closeManifestWriter();

    delete output;

    return KoFilter::OK;
}

void Filterkpr2odf::createImageList( KoStore* output, KoStore* input, KoXmlWriter* manifest )
{
    output->enterDirectory( "Pictures" );
    manifest->addManifestEntry( "Pictures/", "" );
    KoXmlElement key( m_mainDoc.namedItem( "DOC" ).namedItem("PICTURES").firstChild().toElement() );

    //Iterate over all the keys to copy the image, get the file name and
    //its "representation" inside the KPR file
    for( ; !key.isNull(); key = key.nextSibling().toElement() )
    {
        QString name( key.attribute( "name" ) );
        QString fullFilename( getPictureNameFromKey( key ) );
        QStringList filenameComponents( name.split( "/" ) );
        QString odfName( filenameComponents.at( filenameComponents.size()-1 ) );

        m_pictures[ fullFilename ] = odfName;

        //Copy the picture
        QByteArray* image = new QByteArray();
        input->extractFile( name, *image );
        output->open( odfName );
        output->write( *image );
        output->close();
        delete image;

        //generate manifest entry
        QString mediaType;
        if( odfName.contains( "png" ) ) {
            mediaType = "image/png";
        }
        else if( odfName.contains( "jpg" ) ) {
            mediaType = "image/jpg";
        }
        else if( odfName.contains( "jpeg" ) ){
            mediaType = "image/jpeg";
        }
        manifest->addManifestEntry( odfName, mediaType );
    }
    output->leaveDirectory();
}

void Filterkpr2odf::convertContent( KoXmlWriter* content )
{
    content->startElement( "office:body" );
    content->startElement( KoOdf::bodyContentElement( KoOdf::Presentation, true ) );

    //We search all this here so that we can make the search just once
    KoXmlNode titles( m_mainDoc.namedItem( "DOC" ).namedItem( "PAGETITLES" ) );
    KoXmlNode notes( m_mainDoc.namedItem( "DOC" ).namedItem( "PAGENOTES" ) );
    KoXmlNode backgrounds( m_mainDoc.namedItem( "DOC" ).namedItem( "BACKGROUND" ) );
    KoXmlNode objects( m_mainDoc.namedItem( "DOC" ).namedItem( "OBJECTS" ) );
    KoXmlNode paper( m_mainDoc.namedItem( "DOC" ).namedItem( "PAPER" ) );
    m_pageHeight = paper.toElement().attribute( "ptHeight" ).toFloat();

    //Go to the first background, there might be missing backgrounds
    KoXmlElement pageBackground = backgrounds.firstChild().toElement();
    //Parse pages
    //create the master page style
    const QString masterPageStyleName( createMasterPageStyle() );
    int currentPage = 1;
    //The pages are all stored inside PAGETITLES
    //and all notes in PAGENOTES
    KoXmlNode title = titles.firstChild();
    KoXmlNode note = notes.firstChild();
    for ( ; !title.isNull() && !note.isNull();
          title = title.nextSibling(), note = note.nextSibling() )
    {
        //Every page is a draw:page
        content->startElement( "draw:page" );
        content->addAttribute( "draw:name", title.toElement().attribute("title") );
        content->addAttribute( "draw:style-name", createPageStyle( pageBackground ) );
        pageBackground = pageBackground.nextSibling().toElement();//next background
        content->addAttribute( "draw:id", QString( "page%1").arg( currentPage ) );
        content->addAttribute( "draw:master-page-name", masterPageStyleName );

        //convert the objects (text, images, charts...) in this page
        convertObjects( content, objects, currentPage );

        //Append the notes
        content->startElement( "presentation:notes" );
        content->startElement( "draw:page-thumbnail" );
        content->endElement();//draw:page-thumbnail
        content->startElement( "draw:frame" );//FIXME: add drawing attributes
        content->startElement( "draw:text-box" );
        QStringList noteTextList = note.toElement().attribute( "note" ).split("\n");

        foreach( QString string, noteTextList ) {
            content->startElement( "text:p" );
            content->addTextNode( string );
            content->endElement();
        }

        content->endElement();//draw:text-box
        content->endElement();//draw:frame
        content->endElement();//presentation:notes
        content->endElement();//draw:page
        ++currentPage;
    }//for

    content->endElement();//office:presentation
    content->endElement();//office:body
    content->endDocument();
}

void Filterkpr2odf::convertObjects( KoXmlWriter* content, const KoXmlNode& objects, const int currentPage )
{
    //We search through all the objects' nodes because
    //we are not sure if they are saved in order
    for( KoXmlNode object( objects.firstChild() ); !object.isNull(); object = object.nextSibling() ) {
        float y = object.namedItem( "ORIG" ).toElement().attribute( "y" ).toFloat();

        //We check if the y is on the current page
        if ( y < m_pageHeight * ( currentPage - 1 )
             || y >= m_pageHeight * currentPage )
            continue; // object not on current page

        //Now define what kind of object is
        KoXmlElement objectElement = object.toElement();
        switch( objectElement.attribute( "type" ).toInt() )
        {
        case 0: // picture
//             appendPicture( content, objectElement );
            break;
        case 1: // line
            break;
        case 2: // rectangle
            break;
        case 3: // ellipse
            break;
        case 4: // text
            break;
        case 5: //autoform
            break;
        case 6: //clipart
            break;
        //NOTE: 7 is undefined, never happens in a file (according to kpresenter.dtd)
        case 8: // pie
            break;
        case 9: //part
            break;
        case 10: //group
            break;
        case 11: //freehand
            break;
        case 12: // polyline
            break;
        case 13: //quadratic bezier curve
            break;
        case 14: //cubic bezier curve
            break;
        case 15: //polygon
            break;
        case 16: //close line
            break;
        default:
            kWarning()<<"Unexpected object found in page "<<currentPage;
            break;
        }//switch objectElement
    }//for
}

void Filterkpr2odf::appendPicture( KoXmlWriter* content, KoXmlElement objectElement ) {
    content->startElement( "draw:image" );
    content->addAttribute( "xlink:type", "simple" );
    content->addAttribute( "xlink:show", "embed" );
    content->addAttribute( "xlink:actuate", "onLoad" );
    content->endElement();//draw:image
}

const QString Filterkpr2odf::getPictureNameFromKey( const KoXmlElement& key )
{
    return key.attribute( "msec" ) + key.attribute( "second" ) + key.attribute( "minute" )
           + key.attribute( "hour" ) + key.attribute( "day" ) + key.attribute( "month")
           + key.attribute( "year" ) + key.attribute( "filename" );
}

#include "StylesFilterkpr2odf.cpp"

#include "Filterkpr2odf.moc"
