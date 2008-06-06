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

    delete input;

    //If we find everything let the saving begin

    //Create the output file
    KoStore* output = KoStore::createStore( m_chain->outputFile(), KoStore::Write
                                           ,KoXmlNS::presentation, KoStore::Zip );

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

     //Create the content.xml file
    KoXmlWriter *content = odfWriter.contentWriter();
    KoXmlWriter *body = odfWriter.bodyWriter();
    convertContent( body );
    odfWriter.closeContentWriter();
    manifest->addManifestEntry( "content.xml", "text/xml" );

     //Create the styles.xml file
//     output->open( "styles.xml" );
//     manifest->addManifestEntry( "styles.xml", "text/xml" );
//     output->close();

     //Create the meta.xml file
//     KoDocumentInfo* meta = new KoDocumentInfo();
//     meta->load( m_documentInfo );
//     meta->saveOasis( output );
//     delete meta;

     //Write the Pictures directory and its children
//     output->enterDirectory( "Pictures" );
//     output->leaveDirectory();
//     manifest->addManifestEntry( "Pictures/", "" );

     //Write the document manifest
    odfWriter.closeManifestWriter();

    delete output;

    return KoFilter::OK;
}

void Filterkpr2odf::convertContent( KoXmlWriter* content )
{
    KoXmlNode titles = m_mainDoc.namedItem("DOC").namedItem( "PAGETITLES" );
    KoXmlNode notes = m_mainDoc.namedItem("DOC").namedItem( "PAGENOTES" );
    KoXmlNode backgrounds = m_mainDoc.namedItem( "BACKGROUND" );

    //Parse pages
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
        //FIXME:missing draw:style-name
        content->addAttribute( "draw:id", currentPage );
        //FIXME:missing draw:master-page-name

        //TODO:Append objects

        //Append the notes
        content->startElement( "presentation:notes" );
        content->startElement( "draw:text-box" );
        QStringList noteTextList = note.toElement().attribute("note").split("\n");

        foreach( QString string, noteTextList ) {
            content->startElement( "text:p" );
            content->addTextNode( string );
            content->endElement();
        }

        content->endElement();//end of draw:text-box
        content->endElement();//end of presentation:notes
        content->endElement();//end of draw:page
        ++currentPage;
    }
    content->endDocument();
}

#include "Filterkpr2odf.moc"
