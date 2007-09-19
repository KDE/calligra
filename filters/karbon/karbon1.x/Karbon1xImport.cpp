/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "Karbon1xImport.h"

#include <KoFilterChain.h>
#include <KoStoreDevice.h>
#include <KoOasisStore.h>
#include <KoGenStyles.h>
#include <KoPageLayout.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeLayer.h>

#include <kgenericfactory.h>

K_PLUGIN_FACTORY( KarbonImportFactory, registerPlugin<KarbonImport>(); )
K_EXPORT_PLUGIN( KarbonImportFactory( "kofficefilters" ) )

KarbonImport::KarbonImport(QObject*parent, const QVariantList&)
    : KoFilter(parent)
{
}

KarbonImport::~KarbonImport()
{
}

KoFilter::ConversionStatus KarbonImport::convert(const QByteArray& from, const QByteArray& to)
{
    // check for proper conversion
    if( to != "application/vnd.oasis.opendocument.graphics" || from != "application/x-karbon" )
        return KoFilter::NotImplemented;

    const QString fileName( m_chain->inputFile() );
    if ( fileName.isEmpty() )
    {
        kError() << "No input file name!" << endl;
        return KoFilter::StupidError;
    }

    KoStore* store = KoStore::createStore( fileName, KoStore::Read );
    if ( store && store->hasFile( "maindoc.xml" ) )
    {
        kDebug() <<"Maindoc.xml found in KoStore!";

        if ( ! store->open( "maindoc.xml" ) )
        {
            kError() << "Opening root has failed" << endl;
            delete store;
            return KoFilter::StupidError;
        }
        KoStoreDevice ioMain( store );
        ioMain.open( QIODevice::ReadOnly );
        kDebug () <<"Processing root...";
        if ( ! parseRoot ( &ioMain ) )
        {
            kWarning() << "Parsing maindoc.xml has failed! Aborting!";
            delete store;
            return KoFilter::StupidError;
        }
        ioMain.close();
        store->close();
    }
    else
    {
        kWarning() << "Opening store has failed. Trying raw XML file!";
        // Be sure to undefine store
        delete store;
        store = 0;

        QFile file( fileName );
        file.open( QIODevice::ReadOnly );
        if ( ! parseRoot( &file ) )
        {
            kError() << "Could not process document! Aborting!" << endl;
            file.close();
            return KoFilter::StupidError;
        }
        file.close();
    }

    // We have finished with the input store/file, so close the store (already done for a raw XML file)
    kDebug() <<"Deleting input store...";
    delete store;
    store = 0;
    kDebug() <<"Input store deleted!";

    // create output store
    KoStore* storeout = KoStore::createStore( m_chain->outputFile(), KoStore::Write, to, KoStore::Zip );

    if ( !storeout )
    {
        kWarning() << "Couldn't open the requested file.";
        return KoFilter::FileNotFound;
    }

    // Tell KoStore not to touch the file names
    storeout->disallowNameExpansion();
    KoOasisStore oasisStore( storeout );
    KoXmlWriter* manifestWriter = oasisStore.manifestWriter( to );

    KoGenStyles mainStyles;

    bool success = m_document.saveOasis( storeout, manifestWriter, mainStyles );

    // cleanup
    oasisStore.closeManifestWriter();
    delete storeout;

    if( ! success )
        return KoFilter::CreationError;
    else
        return KoFilter::OK;
}

bool KarbonImport::parseRoot( QIODevice* io )
{
    int line, col;
    QString errormessage;

    KoXmlDocument inputDoc;
    const bool parsed = inputDoc.setContent( io, &errormessage, &line, &col );

    if ( ! parsed )
    {
        kError() << "Error while parsing file: "
                << "at line " << line << " column: " << col
                << " message: " << errormessage << endl;
        // ### TODO: feedback to the user
        return false;
    }

    // Do the conversion!
    convert( inputDoc );

    return true;
}

bool KarbonImport::convert( const KoXmlDocument &document )
{
    KoXmlElement doc = document.documentElement();

    bool success = loadXML( doc );

    KoPageLayout pageLayout;

    // <PAPER>
    KoXmlElement paper = doc.namedItem( "PAPER" ).toElement();
    if ( !paper.isNull() )
    {
        pageLayout.format = static_cast<KoPageFormat::Format>( getAttribute( paper, "format", 0 ) );
        pageLayout.orientation = static_cast<KoPageFormat::Orientation>( getAttribute( paper, "orientation", 0 ) );

        if( pageLayout.format == KoPageFormat::CustomSize )
        {
            pageLayout.width = m_document.pageSize().width();
            pageLayout.height = m_document.pageSize().height();
        }
        else
        {
            pageLayout.width = getAttribute( paper, "width", 0.0 );
            pageLayout.height = getAttribute( paper, "height", 0.0 );
        }
    }
    else
    {
        pageLayout.width = getAttribute( doc, "width", 595.277);
        pageLayout.height = getAttribute( doc, "height", 841.891 );
    }

    kDebug() <<" width=" << pageLayout.width;
    kDebug() <<" height=" << pageLayout.height;
    KoXmlElement borders = paper.namedItem( "PAPERBORDERS" ).toElement();
    if( !borders.isNull() )
    {
        if( borders.hasAttribute( "left" ) )
            pageLayout.left = borders.attribute( "left" ).toDouble();
        if( borders.hasAttribute( "top" ) )
            pageLayout.top = borders.attribute( "top" ).toDouble();
        if( borders.hasAttribute( "right" ) )
            pageLayout.right = borders.attribute( "right" ).toDouble();
        if( borders.hasAttribute( "bottom" ) )
            pageLayout.bottom = borders.attribute( "bottom" ).toDouble();
    }

    // TODO set page layout to the ouput document

    return success;
}

double KarbonImport::getAttribute(KoXmlElement &element, const char *attributeName, double defaultValue)
{
    QString value = element.attribute( attributeName );
    if( ! value.isEmpty() )
        return value.toDouble();
    else
        return defaultValue;
}

int KarbonImport::getAttribute(KoXmlElement &element, const char *attributeName, int defaultValue)
{
    QString value = element.attribute( attributeName );
    if( ! value.isEmpty() )
        return value.toInt();
    else
        return defaultValue;
}

bool KarbonImport::loadXML( const KoXmlElement& doc )
{
    if( doc.attribute( "mime" ) != "application/x-karbon" || doc.attribute( "syntaxVersion" ) != "0.1" )
        return false;

    double width = doc.attribute( "width", "800.0" ).toDouble();
    double height = doc.attribute( "height", "550.0" ).toDouble();

    m_document.setPageSize( QSizeF( width, height ) );
    m_document.setUnit( KoUnit::unit( doc.attribute( "unit", KoUnit::unitName( m_document.unit() ) ) ) );

    KoShapeLayer * defaulLayer = m_document.layers().first();

    KoXmlElement e;
    forEachElement( e, doc )
    {
        if( e.tagName() == "LAYER" )
        {
            kDebug() << "loading layer";
            KoShapeLayer * layer = new KoShapeLayer();
            layer->setVisible( e.attribute( "visible" ) == 0 ? false : true );
            loadGroup( layer, e );

            m_document.insertLayer( layer );
        }
    }

    if( defaulLayer && m_document.layers().count() > 1 )
        m_document.removeLayer( defaulLayer );

    return true;
}

void KarbonImport::loadGroup( KoShapeContainer * group, const KoXmlElement &element )
{
    loadStyle( group, element );

    KoXmlElement e;
    forEachElement( e, element )
    {
        /*
        if( e.tagName() == "COMPOSITE" || e.tagName() == "PATH" ) // TODO : remove COMPOSITE later
        {
            VPath* composite = new VPath( this );
            composite->load( e );
            append( composite );
        }
        else if( e.tagName() == "ELLIPSE" )
        {
            VEllipse* ellipse = new VEllipse( this );
            ellipse->load( e );
            append( ellipse );
        }
        else if( e.tagName() == "RECT" )
        {
            VRectangle* rectangle = new VRectangle( this );
            rectangle->load( e );
            append( rectangle );
        }
        else if( e.tagName() == "POLYLINE" )
        {
            VPolyline* polyline = new VPolyline( this );
            polyline->load( e );
            append( polyline );
        }
        else if( e.tagName() == "POLYGON" )
        {
            VPolygon* polygon = new VPolygon( this );
            polygon->load( e );
            append( polygon );
        }
        else if( e.tagName() == "SINUS" )
        {
            VSinus* sinus = new VSinus( this );
            sinus->load( e );
            append( sinus );
        }
        else if( e.tagName() == "SPIRAL" )
        {
            VSpiral* spiral = new VSpiral( this );
            spiral->load( e );
            append( spiral );
        }
        else if( e.tagName() == "STAR" )
        {
            VStar* star = new VStar( this );
            star->load( e );
            append( star );
        }
        else if( e.tagName() == "GROUP" )
        {
            VGroup* group = new VGroup( this );
            group->load( e );
            append( group );
        }
        else if( e.tagName() == "CLIP" )
        {
            VClipGroup* grp = new VClipGroup( this );
            grp->load( e );
            append( grp );
        }
        else if( e.tagName() == "IMAGE" )
        {
            VImage* img = new VImage( this );
            img->load( e );
            append( img );
        }
        else if( e.tagName() == "TEXT" )
        {
            VText *text = new VText( this );
            text->load( e );
            append( text );
        }
        */
    }
}

void KarbonImport::loadStyle( KoShape * shape, const KoXmlElement &element )
{
    // TODO load fill
    // TODO load stroke

    if( ! element.attribute( "ID" ).isEmpty() )
        shape->setName( element.attribute( "ID" ) );
}

