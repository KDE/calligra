/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>
   code based on svgexport.cc from Inge Wallin <inge@lysator.liu.se>

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
#include <QImage>

//KOffice includes
#include <kgenericfactory.h>
#include <KoStore.h>
#include <KoFilterChain.h>
#include <kdebug.h>

#include "Filterkpr2odf.h"

typedef KGenericFactory<Filterkpr2odf> Filterkpr2odfFactory;
K_EXPORT_COMPONENT_FACTORY( libFilterkpr2odf, Filterkpr2odfFactory( "kofficefilters" ) )

Filterkpr2odf::Filterkpr2odf(QObject *parent,const QStringList&)
: KoFilter(parent)
{
}

KoFilter::ConversionStatus Filterkpr2odf::convert( const QByteArray& from, const QByteArray& to )
{
    kDebug() << "Hello from Filterkpr2odf";
    //Check that the type of files are right
    if ( from != "application/x-kpresenter"
         || to != "application/vnd.oasis.opendocument.presentation" )
        return KoFilter::BadMimeType;

    //open the input file file
    KoStore* input = KoStore::createStore( m_chain->inputFile(), KoStore::Read );
    if ( !input )
        return KoFilter::StorageCreationError;

    //Load the document
     //Load maindoc.xml
    if( !input->open( "maindoc.xml" ) )
        return KoFilter::WrongFormat;
    m_mainDoc.setContent( input->device() );
    input->close();

     //Load documentinfo.xml
    if( !input->open( "documentinfo.xml" ) )
        return KoFilter::WrongFormat;

    m_documentInfo.setContent( input->device() );
    input->close();

     //Load the preview picture
     //FIXME: how do we create folders in the inputDevice?
//     if( !input->open("preview.png") )
//         return KoFilter::WrongFormat;
//     QImage

    delete input;

    //If we find everything let the saving begin
    //Create the output file
    KoStore* output = KoStore::createStore( m_chain->outputFile(), KoStore::Write, "application/vnd.oasis.opendocument.presentation", KoStore::Zip );

    if ( !output )
        return KoFilter::StorageCreationError;

     //Create the content.xml file
    output->open( "content.xml" );
    output->close();

     //Create the styles.xml file
    output->open( "styles.xml" );
    output->close();

     //Create the content.xml file
    output->open( "meta.xml" );
    output->close();

    output->open( "content.xml" );
    return KoFilter::OK;
}

// void Filterkpr2odf::createMetadata()
// {
// }
