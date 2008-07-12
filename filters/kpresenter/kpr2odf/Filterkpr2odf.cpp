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

#include <cmath>

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
#include <KoUnit.h>

#include "Filterkpr2odf.h"

typedef KGenericFactory<Filterkpr2odf> Filterkpr2odfFactory;
K_EXPORT_COMPONENT_FACTORY( libFilterkpr2odf, Filterkpr2odfFactory( "kofficefilters" ) )

Filterkpr2odf::Filterkpr2odf(QObject *parent,const QStringList&)
: KoFilter(parent)
, m_currentPage( 1 )
, m_objectIndex( 1 )
{
}

KoFilter::ConversionStatus Filterkpr2odf::convert( const QByteArray& from, const QByteArray& to )
{
    //Check that the type of files are right
    if( from != "application/x-kpresenter"
         || to != "application/vnd.oasis.opendocument.presentation" )
        return KoFilter::BadMimeType;

    //open the input file file
    KoStore* input = KoStore::createStore( m_chain->inputFile(), KoStore::Read );
    if( !input )
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

    if( !output )
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
    //write the sounds and fill the m_sounds hash
    createSoundList( output, input, manifest );
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
    //TODO: check whcih settings we still use in 2.0
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

//TODO: improve createImageList and createSoundList so that only save the _used_ sounds and images
void Filterkpr2odf::createImageList( KoStore* output, KoStore* input, KoXmlWriter* manifest )
{
    KoXmlElement key( m_mainDoc.namedItem( "DOC" ).namedItem( "PICTURES" ).firstChild().toElement() );
    if( key.isNull() )
        return;

    output->enterDirectory( "Pictures" );
//     manifest->addManifestEntry( "Pictures/", "" );//FIXME: is this needed or not? 1.6 doesn't add an entry, ODEssentials does

    //Iterate over all the keys to copy the image, get the file name and
    //its "representation" inside the KPR file
    for( ; !key.isNull(); key = key.nextSibling().toElement() )
    {
        QString name = key.attribute( "name" );
        QString fullFilename = getPictureNameFromKey( key );

        //Get the name how will be saved in the file
        QStringList filenameComponents = name.split( "/" );
        QString odfName = filenameComponents.at( filenameComponents.size()-1 );

        m_pictures[ fullFilename ] = odfName;

        //Copy the picture
        QByteArray* image = new QByteArray();
        input->extractFile( name, *image );
        output->open( odfName );
        output->write( *image );
        output->close();
        delete image;

        //generate manifest entry
        //FIXME: is there a better way to do it?
        QString mediaType;
        if( odfName.endsWith( "png" ) ) {
            mediaType = "image/png";
        }
        else if( odfName.endsWith( "jpg" ) ) {
            mediaType = "image/jpg";
        }
        else if( odfName.endsWith( "jpeg" ) ){
            mediaType = "image/jpeg";
        }
        manifest->addManifestEntry( name, mediaType );
    }
    output->leaveDirectory();
}

void Filterkpr2odf::createSoundList( KoStore* output, KoStore* input, KoXmlWriter* manifest )
{
    KoXmlElement file( m_mainDoc.namedItem( "DOC" ).namedItem( "SOUNDS" ).firstChild().toElement() );
    if( file.isNull() )
        return;

    output->enterDirectory( "Sounds" );
    manifest->addManifestEntry( "Sounds/", "" );

    //Iterate over all files to copy the sound, get the file name and
    //its "representation" inside the KPR file
    for( ; !file.isNull(); file = file.nextSibling().toElement() )
    {
        QString name( file.attribute( "name" ) );
        QString filename( file.attribute( "filename" ) );
        QStringList filenameComponents( name.split( "/" ) );
        QString odfName( filenameComponents.at( filenameComponents.size()-1 ) );

        m_sounds[ filename ] = odfName;

        //Copy the sound
        QByteArray* sound = new QByteArray();
        input->extractFile( name, *sound );
        output->open( odfName );
        output->write( *sound );
        output->close();
        delete sound;

        //generate manifest entry
        //FIXME: is there a better way to do it?
        QString mediaType;
        if( odfName.endsWith( "wav" ) ) {
            mediaType = "audio/wav";
        }
        else if( odfName.endsWith( "mp3" ) ) {
            mediaType = "audio/mp3";
        }
        manifest->addManifestEntry( name, mediaType );
    }
    output->leaveDirectory();
}

void Filterkpr2odf::convertContent( KoXmlWriter* content )
{
    content->startElement( "office:body" );
    content->startElement( KoOdf::bodyContentElement( KoOdf::Presentation, true ) );

    //We search all this here so that we can make the search just once
    KoXmlNode titles = m_mainDoc.namedItem( "DOC" ).namedItem( "PAGETITLES" );
    KoXmlNode notes = m_mainDoc.namedItem( "DOC" ).namedItem( "PAGENOTES" );
    KoXmlNode backgrounds = m_mainDoc.namedItem( "DOC" ).namedItem( "BACKGROUND" );
    KoXmlNode objects = m_mainDoc.namedItem( "DOC" ).namedItem( "OBJECTS" );
    KoXmlNode paper = m_mainDoc.namedItem( "DOC" ).namedItem( "PAPER" );
    m_pageHeight = paper.toElement().attribute( "ptHeight" ).toFloat();

    //Go to the first background, there might be missing backgrounds
    KoXmlElement pageBackground = backgrounds.firstChild().toElement();
    //Parse pages
    //create the master page style
    const QString masterPageStyleName = createMasterPageStyle();
    //The pages are all stored inside PAGETITLES
    //and all notes in PAGENOTES
    KoXmlNode title = titles.firstChild();
    KoXmlNode note = notes.firstChild();
    for( ; !title.isNull() && !note.isNull();
          title = title.nextSibling(), note = note.nextSibling() )
    {
        //Every page is a draw:page
        content->startElement( "draw:page" );
        content->addAttribute( "draw:name", title.toElement().attribute( "title" ) );
        content->addAttribute( "draw:style-name", createPageStyle( pageBackground ) );
        pageBackground = pageBackground.nextSibling().toElement();//next background
        content->addAttribute( "draw:id", QString( "page%1").arg( m_currentPage ) );
        content->addAttribute( "draw:master-page-name", masterPageStyleName );

        //convert the objects (text, images, charts...) in this page
        convertObjects( content, objects );

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
        ++m_currentPage;
    }//for

    content->startElement( "presentation:settings" );

    //Load wether the presentation ends or it's in an infinite loop
    KoXmlElement infinitLoop( m_mainDoc.namedItem( "DOC" ).namedItem( "INFINITLOOP" ).toElement() );
    if( !infinitLoop.isNull() )
    {
        bool value = infinitLoop.attribute( "value", "0" ) == "1";
        content->addAttribute( "presentation:endless", ( value )? "true" : "false" );
    }

    //Specify wether the effects can be started automatically or
    //ignore any previous order and start them manually
    KoXmlElement manualSwitch = m_mainDoc.namedItem( "DOC" ).namedItem( "MANUALSWITCH" ).toElement();
    if( !manualSwitch.isNull() )
    {
        bool value = manualSwitch.attribute( "value", "0" ) == "1";
        content->addAttribute( "presentation:force-manual", ( value )? "true" : "false" );
    }

    //Store the default show
    KoXmlElement customSlideShowDefault = m_mainDoc.namedItem( "DOC" ).namedItem( "DEFAULTCUSTOMSLIDESHOWNAME" ).toElement();
    if( !customSlideShowDefault.isNull() )
    {
        content->addAttribute( "presentation:show", customSlideShowDefault.attribute( "name" ) );
    }

    //Now store all the shows
    KoXmlElement customSlideShowConfig = m_mainDoc.namedItem( "DOC" ).namedItem( "CUSTOMSLIDESHOWCONFIG" ).toElement();
    for( KoXmlElement customSlideShow = customSlideShowConfig.firstChild().toElement(); !customSlideShow.isNull();
          customSlideShow = customSlideShow.nextSibling().toElement() )
    {
        //FIXME: is this needed? show.tagName()=="CUSTOMSLIDESHOW" )
        content->startElement( "presentation:show" );
        content->addAttribute( "presentation:name", customSlideShow.attribute( "name" ) );
        content->addAttribute( "presentation:pages", customSlideShow.attribute( "pages" ) );
        content->endElement();//presentation:show
    }

    content->endElement();//presentation:settings
    content->endElement();//office:presentation
    content->endElement();//office:body
    content->endDocument();
}

void Filterkpr2odf::convertObjects( KoXmlWriter* content, const KoXmlNode& objects )
{
    //We search through all the objects' nodes because
    //we are not sure if they are saved in order
    for( KoXmlNode object = objects.firstChild(); !object.isNull(); object = object.nextSibling() )
    {
        float y = object.namedItem( "ORIG" ).toElement().attribute( "y" ).toFloat();

        //We check if the y is on the current page
        if( y < m_pageHeight * ( m_currentPage - 1 )
             || y >= m_pageHeight * m_currentPage )
            continue; // object not on current page

        //Now define what kind of object is
        KoXmlElement objectElement = object.toElement();
        //Enum: ObjType
        switch( objectElement.attribute( "type" ).toInt() )
        {
        case 0: // picture
            appendPicture( content, objectElement );
            break;
        case 1: // line
            appendLine( content, objectElement );
            break;
        case 2: // rectangle
            appendRectangle( content, objectElement );
            break;
        case 3: // ellipse or circle
            appendEllipse( content, objectElement );
            break;
        case 4: // text
            appendTextBox( content, objectElement );
            break;
        case 5: //autoform
//             appendAutoform( content, objectElement );
            break;
        case 6: //clipart
            break;
        //NOTE: 7 is undefined, never happens in a file (according to kpresenter.dtd)
        case 8: // pie, chord, arc
            appendPie( content, objectElement );
            break;
        case 9: //part
            break;
        case 10: //group
            appendGroupObject( content, objectElement );
            break;
        case 11: //freehand
            appendFreehand( content, objectElement );
            break;
        case 12: // polyline
            //a bunch of points that are connected and not closed
            appendPoly( content, objectElement, false /*polyline*/ );
            break;
        case 13: //quadric bezier curve
            break;
        case 14: //cubic bezier curve
            break;
        case 15: //polygon
            //a regular polygon, easily drawn by the number of sides it has
            appendPolygon( content, objectElement );
            break;
        case 16: //closed polyline
            //that is a closed non-regular polygon
            appendPoly( content, objectElement, true /*closedPolygon*/ );
            break;
        default:
            kWarning()<<"Unexpected object found in page "<<m_currentPage;
            break;
        }//switch objectElement
        ++m_objectIndex;
    }//for
}

void Filterkpr2odf::appendPicture( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:frame" );
    set2DGeometry( content, objectElement );//sizes mostly
    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );

    content->startElement( "draw:image" );
    content->addAttribute( "xlink:type", "simple" );
    content->addAttribute( "xlink:show", "embed" );
    content->addAttribute( "xlink:actuate", "onLoad" );
    content->addAttribute( "xlink:href", "Pictures/" + m_pictures[ getPictureNameFromKey( objectElement.namedItem( "KEY" ).toElement() ) ] );

    content->endElement();//draw:image
    content->endElement();//draw:frame
    //TODO: port the effects
}

void Filterkpr2odf::appendLine( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:line" );
    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );

    KoXmlElement angle = objectElement.namedItem( "ANGLE" ).toElement();
    if ( !angle.isNull() )
    {
        content->addAttribute( "draw:transform", rotateValue( angle.attribute( "value" ).toDouble() ) );
    }

    KoXmlElement orig = objectElement.namedItem( "ORIG" ).toElement();
    KoXmlElement size = objectElement.namedItem( "SIZE" ).toElement();
    double x1 = orig.attribute( "x" ).toDouble();
    double y1 = orig.attribute( "y" ).toDouble() - m_pageHeight * ( m_currentPage - 1 );
    double x2 = size.attribute( "width" ).toDouble() + x1;
    double y2 = size.attribute( "height" ).toDouble() + y1;

    KoXmlElement lineType = objectElement.namedItem( "LINETYPE" ).toElement();
    int type = 0;
    if ( !lineType.isNull() )
    {
        type = lineType.attribute( "value" ).toInt();
    }

    content->addAttribute( "draw:id",  QString( "object%1" ).arg( m_objectIndex ) );
    QString xpos1 = QString( "%1cm" ).arg( KoUnit::toCentimeter( x1 ) );
    QString xpos2 = QString( "%1cm" ).arg( KoUnit::toCentimeter( x2 ) );

    //Enum: LineType
    switch( type )
    {
    case 0: //Horizontal
        content->addAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCentimeter( y2/2.0 ) ) );
        content->addAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCentimeter( y2/2.0 ) ) );
        break;
    case 1: //Vertical
        content->addAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCentimeter( y1 ) ) );
        content->addAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCentimeter( y2 ) ) );
        xpos1 = QString( "%1cm" ).arg( KoUnit::toCentimeter( x1/2.0 ) );
        xpos2 = xpos1;
        break;
    case 2: //Left Top to Right Bottom
        content->addAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCentimeter( y1 ) ) );
        content->addAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCentimeter( y2 ) ) );
        break;
    case 3: //Left Bottom to Right Top
        content->addAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCentimeter( y2 ) ) );
        content->addAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCentimeter( y1 ) ) );
        break;
    }

    content->addAttribute( "svg:x1", xpos1 );
    content->addAttribute( "svg:x2", xpos2 );

    KoXmlElement name = objectElement.namedItem( "OBJECTNAME").toElement();
    QString nameString = name.attribute( "objectName" );
    if( !nameString.isNull() )
    {
        content->addAttribute( "draw:name", nameString );
    }
    content->endElement();//draw:line
}

void Filterkpr2odf::appendRectangle( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:rect" );

    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );
    set2DGeometry( content, objectElement );

    content->endElement();//draw:rect
}

void Filterkpr2odf::appendEllipse( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    KoXmlElement size = objectElement.namedItem( "SIZE" ).toElement();
    double width = size.attribute( "width" ).toDouble();
    double height = size.attribute( "height" ).toDouble();

    content->startElement( ( width == height ) ? "draw:circle" : "draw:ellipse" );
    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );
    set2DGeometry( content, objectElement );

    content->endElement();//draw:circle or draw:ellipse
}

void Filterkpr2odf::appendTextBox( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:frame" );
    set2DGeometry( content, objectElement );
    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );

    content->startElement( "draw:text-box" );
    KoXmlElement textObject = objectElement.namedItem( "TEXTOBJ" ).toElement();

    //export every paragraph
    for( KoXmlElement paragraph = textObject.firstChild().toElement(); !paragraph.isNull(); paragraph = paragraph.nextSibling().toElement() )
    {
        appendParagraph( content, paragraph );
    }

    content->endElement();//draw:text-box
    content->endElement();//draw:frame
}

void Filterkpr2odf::appendParagraph( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "text:p" );
    content->addAttribute( "text:style-name", createParagraphStyle( objectElement ) );

    //convert every text element
    for( KoXmlElement text = objectElement.firstChild().toElement(); !text.isNull();  text = text.nextSibling().toElement() )
    {
        if( text.nodeName() == "TEXT" ) //only TEXT childs are relevant
        {
            appendText( content, text );
        }
    }

    content->endElement();//text:p
}

void Filterkpr2odf::appendText( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "text:span" );

    content->addAttribute( "text:style-name", createTextStyle( objectElement ) );
    content->addTextNode( objectElement.text() );

    content->endElement();//text:span
}

void Filterkpr2odf::appendPie( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    KoXmlElement size = objectElement.namedItem( "SIZE" ).toElement();
    double width = size.attribute( "width" ).toDouble();
    double height = size.attribute( "height" ).toDouble();

    content->startElement( ( width == height ) ? "draw:circle" : "draw:ellipse" );
    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );
    set2DGeometry( content, objectElement );

    //Type of the enclosure of the circle/ellipse
    KoXmlElement pie = objectElement.namedItem( "PIETYPE" ).toElement();
    QString kind = "section";//We didn't find the type, we set "section" by default
    if( !pie.isNull() )
    {
        int typePie = pie.attribute( "value" ).toInt();
        switch( typePie )
        {
        case 0:
            kind = "section";
            break;
        case 1:
            kind = "arc";
            break;
        case 2:
            kind = "cut";
            break;
        }
    }
    content->addAttribute( "draw:kind", kind );

    KoXmlElement pieAngle = objectElement.namedItem( "PIEANGLE" ).toElement();
    int startAngle = 45; //default value take it into kppieobject
    if( !pieAngle.isNull() )
    {
        startAngle = ( pieAngle.attribute( "value" ).toInt() ) / 16;
    }
    content->addAttribute( "draw:start-angle", startAngle );

    KoXmlElement pieLength = objectElement.namedItem( "PIELENGTH" ).toElement();
    int endAngle = 90 + startAngle; //default
    if( !pieLength.isNull() )
    {
        endAngle = pieLength.attribute( "value" ).toInt() / 16 + startAngle;
    }
    content->addAttribute( "draw:end-angle", endAngle );

    content->endElement();//draw:circle or draw:ellipse
}

void Filterkpr2odf::appendGroupObject( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:g" );
    KoXmlElement objects = objectElement.namedItem( "OBJECTS" ).toElement();
    convertObjects( content, objects );
    content->endElement();//draw:g
}

void Filterkpr2odf::appendPoly( KoXmlWriter* content, const KoXmlElement& objectElement, bool polygon )
{
    //The function was written so to add polygon and polyline because it's basically the same,
    //only the name is changed and I didn't want to copy&paste
    //TODO: find out wether i shuld use draw:regular-polygon instead of draw:polygon
    content->startElement( ( polygon )? "draw:polygon" : "draw:polyline" );

    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );

    set2DGeometry( content, objectElement );
    KoXmlElement points = objectElement.namedItem( "POINTS" ).toElement();
    if( !points.isNull() ) {
        KoXmlElement elemPoint = points.firstChild().toElement();
        QString listOfPoints;

        //No white spaces allowed before the first element
        int tmpX = ( int ) ( elemPoint.attribute( "point_x", "0" ).toDouble() * 10000 );
        int tmpY = ( int ) ( elemPoint.attribute( "point_y", "0" ).toDouble() * 10000 );
        listOfPoints = QString( "%1,%2" ).arg( tmpX ).arg( tmpY );

        int maxX = tmpX;
        int maxY = tmpY;
        int previousX = tmpX;
        int previousY = tmpY;

        while( !elemPoint.isNull() ) {
            tmpX = ( int ) ( elemPoint.attribute( "point_x", "0" ).toDouble() * 10000 );
            tmpY = ( int ) ( elemPoint.attribute( "point_y", "0" ).toDouble() * 10000 );
            //For some reason the last point is saved twice for some polygons, so we need to ignore the last one of them if they are equal
            //this fix assumes that the last child of the POINTS tag is a Point, seems to work but it's not garanteed
            if( tmpX == previousX && tmpY == previousY && elemPoint.nextSibling().isNull() )
            {
                break;
            }

            listOfPoints += QString( " %1,%2" ).arg( tmpX ).arg( tmpY );

            maxX = qMax( maxX, tmpX );
            maxY = qMax( maxY, tmpY );
            previousX = tmpX;
            previousY = tmpY;

            elemPoint = elemPoint.nextSibling().toElement();
        }//while !element.isNull()
        content->addAttribute( "draw:points", listOfPoints );
        content->addAttribute( "svg:viewBox", QString( "0 0 %1 %2" ).arg( maxX ).arg( maxY ) );
    }//if !points.isNull()

    content->endElement();//draw:polygon or draw:polyline
}

void Filterkpr2odf::appendPolygon( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:regular-polygon" );

    set2DGeometry( content, objectElement );
    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );

    KoXmlElement settings = objectElement.namedItem( "SETTINGS" ).toElement();
    int corners = settings.attribute( "cornersValue" ).toInt();
    content->addAttribute( "draw:corners", corners );
    bool concavePolygon = settings.attribute( "checkConcavePolygon", "0" ) == "1";
    if( concavePolygon )
    {
        content->addAttribute( "draw:concave", "true" );
        content->addAttribute( "draw:sharpness", QString( "%1%" ).arg( settings.attribute( "sharpnessValue" ) ) );
    }
    else
    {
        content->addAttribute( "draw:concave", "false" );
    }

    content->endElement();//draw:regular-polygon
}

void Filterkpr2odf::appendAutoform( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:path" );

    set2DGeometry( content, objectElement );
    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );

    QString g;
    //TODO:export
    content->addAttribute( "draw:g", g );

    content->endElement();//draw:path
}

void Filterkpr2odf::appendFreehand( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    content->startElement( "draw:path" );

    content->addAttribute( "draw:style-name", createGraphicStyle( objectElement ) );
    set2DGeometry( content, objectElement );

    KoXmlElement points = objectElement.namedItem( "POINTS" ).toElement();
    if( !points.isNull() ) {
        KoXmlElement elemPoint = points.firstChild().toElement();
        QString d;

        int tmpX = ( int ) ( elemPoint.attribute( "point_x", "0" ).toDouble() * 10000 );
        int tmpY = ( int ) ( elemPoint.attribute( "point_y", "0" ).toDouble() * 10000 );
        int maxX = tmpX;
        int maxY = tmpY;

        elemPoint = elemPoint.nextSibling().toElement();

        d += QString( "M%1 %2" ).arg( tmpX ).arg( tmpY );
        while( !elemPoint.isNull() ) {
            tmpX = ( int ) ( elemPoint.attribute( "point_x", "0" ).toDouble() * 10000 );
            tmpY = ( int ) ( elemPoint.attribute( "point_y", "0" ).toDouble() * 10000 );

            d += QString( "L%1 %2" ).arg( tmpX ).arg( tmpY );

            maxX = qMax( maxX, tmpX );
            maxY = qMax( maxY, tmpY );
            elemPoint = elemPoint.nextSibling().toElement();
        }
        content->addAttribute( "svg:d", d );
        content->addAttribute( "svg:viewBox", QString( "0 0 %1 %2" ).arg( maxX ).arg( maxY ) );
    }//if !points.isNull()

    content->endElement();//draw:path
}

const QString Filterkpr2odf::getPictureNameFromKey( const KoXmlElement& key )
{
    return key.attribute( "msec" ) + key.attribute( "second" ) + key.attribute( "minute" )
           + key.attribute( "hour" ) + key.attribute( "day" ) + key.attribute( "month" )
           + key.attribute( "year" ) + key.attribute( "filename" );
}

void Filterkpr2odf::set2DGeometry( KoXmlWriter* content, const KoXmlElement& objectElement )
{
    //This function sets the needed geometry-related attributes
    //for any object that is passed to it

    KoXmlElement name = objectElement.namedItem( "OBJECTNAME" ).toElement();

    QString nameStr = name.attribute( "objectName" );
    if( !nameStr.isEmpty() )
    {
        content->addAttribute( "draw:name", nameStr );
    }

    KoXmlElement angle = objectElement.namedItem( "ANGLE" ).toElement();
    if( !angle.isNull() )
    {
        QString returnAngle = rotateValue( angle.attribute( "value" ).toDouble() );
        if( !returnAngle.isEmpty() )
        {
            content->addAttribute( "draw:transform", returnAngle );
        }
    }

    KoXmlElement size = objectElement.namedItem( "SIZE" ).toElement();
    KoXmlElement orig = objectElement.namedItem( "ORIG" ).toElement();

    float y = orig.attribute( "y" ).toFloat();
    y -= m_pageHeight * ( m_currentPage - 1 );

    content->addAttribute( "draw:id", QString( "object%1" ).arg( m_objectIndex ) );
    content->addAttribute( "svg:x", QString( "%1cm" ).arg( KoUnit::toCentimeter( orig.attribute( "x" ).toDouble() ) ) );
    content->addAttribute( "svg:y", QString( "%1cm" ).arg( KoUnit::toCentimeter( y ) ) );
    content->addAttribute( "svg:width", QString( "%1cm" ).arg( KoUnit::toCentimeter( size.attribute( "width" ).toDouble() ) ) );
    content->addAttribute( "svg:height", QString( "%1cm" ).arg( KoUnit::toCentimeter( size.attribute( "height" ).toDouble() ) ) );
}

QString Filterkpr2odf::rotateValue( double val )
{
    QString str;
    if ( val != 0.0 )
    {
        //FIXME: OOo needs -1 *, KPresenter loads it just fine, which is the right behavior?
        double value = ( ( double )val * M_PI ) / 180.0;
        str = QString( "rotate(%1)" ).arg( value );
    }
    return str;
}

#include "StylesFilterkpr2odf.cpp"

#include "Filterkpr2odf.moc"
