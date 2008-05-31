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

//KDE includes
#include <kgenericfactory.h>
#include <kdebug.h>

//KOffice includes
#include <KoStore.h>
#include <KoFilterChain.h>
#include <KoXmlWriter.h>

#include "Filterkpr2odf.h"
#include "manifestCreator.h"

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
    //Create the manifest creator
    manifestCreator manifest;
    manifest.addFile("application/vnd.oasis.opendocument.presentation", "/");

    //Create the output file
    KoStore* output = KoStore::createStore( m_chain->outputFile(), KoStore::Write
                                           ,"application/vnd.oasis.opendocument.presentation", KoStore::Zip );

    if ( !output )
        return KoFilter::StorageCreationError;

    //Save the preview picture
    output->enterDirectory( "Thumbnails" );
    output->open( "thubnail.png" );
    output->write( *preview );
    output->close();
    output->leaveDirectory();
    manifest.addFile( "Thubnails/thubnail.png" );
    manifest.addFile( "Thubnails/" );
    delete preview;

    //We cannot handle the QIODevice by ourselves when it's in writing mode so we
    //workaround that issue by writing to a QByteArray in a QBuffer and then returning the array
     //Create the content.xml file
    output->open( "content.xml" );
    manifest.addFile( "content.xml", "text/xml" );
    output->close();

     //Create the styles.xml file
    output->open( "styles.xml" );
    manifest.addFile( "styles.xml", "text/xml" );
    output->close();

     //Create the meta.xml file
    output->open( "meta.xml" );
    output->write( createMetadata() );
    manifest.addFile( "meta.xml", "text/xml" );
    output->close();

     //Write the Pictures directory and its children
    output->enterDirectory( "Pictures" );
    //FIXME: how in earth do we get the files inside the pictures' directory in the KPR?!
    output->leaveDirectory();
    manifest.addFile( "Pictures/" );

     //Write the document manifest
    output->enterDirectory( "META-INF" );
    output->open( "manifest.xml" );
    output->write( manifest.endManifest() );
    output->close();
    output->leaveDirectory();

    delete output;

    return KoFilter::OK;
}

QByteArray Filterkpr2odf::createMetadata()
{
    KoXmlWriter* meta;//the meta document itself
    KoXmlNode node;//this is a working node, as the reciver of namedItem's calls

    //Workaround, see the convert function
    QByteArray metaData;
    QBuffer buffer( &metaData );
    buffer.open( QIODevice::WriteOnly );

    meta = new KoXmlWriter( &buffer );

    //create the document
    meta->startDocument( "office:document-meta" );

    //loads of XMLNameSpaces declarations
    meta->startElement( "office:document-meta" );
    meta->addAttribute( "xmlns:office", "http://openoffice.org/2000/office" );
    meta->addAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    meta->addAttribute( "xmlns:dc", "http://purl.org/dc/elements/1.1/" );
    meta->addAttribute( "xmlns:meta", "http://openoffice.org/2000/meta" );
    //FIXME: Opening a OOo Impress file I found those NS also declared
    //are those really needed in this file?
    meta->addAttribute( "xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0" );
    meta->addAttribute( "xmlns:ooo", "http://openoffice.org/2004/office" );
    meta->addAttribute( "xmlns:smil", "urn:oasis:names:tc:opendocument:xmlns:smil-compatible:1.0" );
    meta->addAttribute( "xmlns:anim", "urn:oasis:names:tc:opendocument:xmlns:animation:1.0" );
    meta->addAttribute( "office:version", "1.0" );

    meta->startElement( "office:meta" );
    //Now the real beef: reading the KPresenter's nodes and writing their conterparts
    meta->startElement( "meta:generator" );
    meta->addTextNode( "KPresenter 1.5" );//FIXME: Kpresenter 1.5 or 2.0?
    meta->endElement();//end of meta:generator

    KoXmlNode documentInfoNode = m_documentInfo.namedItem("document-info");
    if( !documentInfoNode.isNull() ) {
        //about node
        KoXmlNode aboutNode = documentInfoNode.namedItem("about");
        if( !aboutNode.isNull() ) {
            node = aboutNode.namedItem("abstract");
            if( !node.isNull() ) {
                meta->startElement("dc:description");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
            node = aboutNode.namedItem("title");
            if( !node.isNull() ) {
                meta->startElement("dc:title");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
            node = aboutNode.namedItem("keyword");
            if( !node.isNull() ) {
                meta->startElement("dc:subject");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
            node = aboutNode.namedItem("initial-creator");
            if( !node.isNull() ) {
                meta->startElement("meta:initial-creator");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
            node = aboutNode.namedItem("editing-cycles");
            if( !node.isNull() ) {
                meta->startElement("meta:editing-cycles");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
            node = aboutNode.namedItem("creation-date");
            if( !node.isNull() ) {
                meta->startElement("meta:creation-date");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
            node = aboutNode.namedItem("date");
            if( !node.isNull() ) {
                meta->startElement("dc:date");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
        }//end of aboutNode's if

        //author node
        KoXmlNode authorNode = m_documentInfo.namedItem("author");
        if( !authorNode.isNull() ) {
            node = aboutNode.namedItem("full-name");
            if( !node.isNull() ) {
                meta->startElement("meta:initial-creator");
                meta->addTextNode( node.firstChild().toText().data() );
                meta->endElement();
            }
        }//end of authorNode's if
        //FIXME: loads of the autor information is being lost, what should we do?
    }//end of documentInfoNode's if

    meta->endElement();//end of office:meta
    meta->endElement();//end of office:document-meta
    meta->endDocument();

    return metaData;
}

#include "Filterkpr2odf.moc"
