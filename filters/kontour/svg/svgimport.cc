/*
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>.
    This file is part of the KDE project

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

DESCRIPTION
    Filter to convert a svg file to a killustrator document using koDocument
*/

#include "svgimport.h"
#include <kdebug.h>
#include "GPage.h"
#include "GGroup.h"
#include <qfile.h>
#include <KIllustrator_doc.h>
#include <koGlobal.h>
#include "path.h"
#include "basicshapes.h"
#include "text.h"

QList<GObject>          objList;  // contains the killu GObjects
QList<SVGComposite>  composites;  // keep track of composite objects
QDict<SVGElement> referencedSVG;  // keep track of SVGElements with ID
KoPageLayout            *pageLayout;

SVGImport::SVGImport( KoFilter *parent, const char *name ) : KoFilter(parent, name)
{   
    //referencedSVG.setAutoDelete( true );
}


SVGImport::~SVGImport()
{
    objList.clear();
    composites.clear();
    referencedSVG.clear();
}


bool SVGImport::filterImport( const QString &file, KoDocument *doc,
                              const QString &from, const QString &to,
                              const QString & ) {

    if( to != "application/x-killustrator" || from != "image/x-svg" )
        return false;

    pageLayout = NULL;

    SVGHandler handler;
    QFile fin( file );
    QXmlInputSource source( fin );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    if( !reader.parse( source ) )
    {
        return false;
    }
    
    KIllustratorDocument *kidoc = (KIllustratorDocument *) doc;
    GDocument *gdoc             = kidoc->gdoc();
    GPage *activePage           = gdoc->activePage();

    //gdoc->setAutoUpdate(false);

    QListIterator<GObject> it( objList );
    double index = 0;
    for( ;it.current() ; ++it ) {
        kdDebug() << "progress:" << ( (index * 100.0) / (double)objList.count() ) << endl;
        activePage->insertObject( it.current() );
	emit sigProgress( (index++ * 100.0) / (double)objList.count() );
    }
    
    //gdoc->setAutoUpdate(true);
    if( pageLayout ) activePage->setPageLayout( *pageLayout );
    doc->setModified( false ); // not modified yet
    return true;
}


SVGHandler::SVGHandler()
{
    state = Normal;
}
 
 
SVGHandler::~SVGHandler()
{
}


QString SVGHandler::errorProtocol()
{
    return "";
}
 
 
bool SVGHandler::startDocument()
{
    return TRUE;
}


bool SVGHandler::startElement( const QString &, const QString &, const QString &qName, const QXmlAttributes &atts )
{
    kdDebug() << "startElement:qName: " << qName.local8Bit() << endl;
    SVGElement *obj = NULL;
    SVGComposite *parent = composites.current();
    if(      qName == "rect")    obj = new SVGRectElement( parent );
    else if( qName == "path")    obj = new SVGPathElement( parent );
    else if( qName == "line")    obj = new SVGLineElement( parent );
    else if( qName == "polyline")obj = new SVGPolylineElement( parent );
    else if( qName == "polygon") obj = new SVGPolygonElement( parent );
    else if( qName == "ellipse") obj = new SVGEllipseElement( parent );
    else if( qName == "circle")  obj = new SVGCircleElement( parent );
    else if( qName == "use")     obj = new SVGUseElement( parent );
    else if( qName == "image")   obj = new SVGImageElement( parent );

    if( obj )
    {
        obj->setAttributes( atts );
        if( obj->hasIdentifier() )
        {
	    kdDebug() << "Yes, adding : " << obj->id().local8Bit() << endl;
            referencedSVG.insert( obj->id(), obj );
        }
        if( parent )
	{
            if( state != Defs || parent->type() != "defs" )
	        parent->addObject( obj );
        }
    }
    else if( qName == "g" || qName == "svg" || qName == "text" || qName == "defs" || qName == "symbol" )
    {
	if( qName == "svg" )
	    composites.append( new SVGSVGElement( parent ) );
        else if( qName == "text")
            composites.append( new SVGTextElement( parent ) );
        else if( qName == "defs" )
        {
            state = Defs;
            composites.append( new SVGDefsElement( parent ) );
        }
	else if( qName == "g" )
	    composites.append( new SVGGElement( parent ) );
	else if( qName == "symbol" )
	    composites.append( new SVGSymbolElement( parent ) );

        composites.current()->setAttributes( atts );
        // set width and height of page if specified
        if( qName == "svg" && !parent &&
	    ((SVGSVGElement *)composites.current())->width().value() >= 0 &&
	    ((SVGSVGElement *)composites.current())->height().value() >= 0 )
	{
	    if (!pageLayout) pageLayout = new KoPageLayout;
	    pageLayout->format      = PG_CUSTOM;
            pageLayout->unit        = PG_PT;
	    pageLayout->orientation = PG_PORTRAIT;
            pageLayout->ptLeft      = 0;
            pageLayout->ptRight     = 0;
            pageLayout->ptTop       = 0;
            pageLayout->ptBottom    = 0;
            pageLayout->ptWidth     = ((SVGSVGElement *)composites.current())->width().value();
            pageLayout->ptHeight    = ((SVGSVGElement *)composites.current())->height().value();
        }
	//if( parent )
	//    parent->addObject( obj );
    }

    return TRUE;
}


bool SVGHandler::characters( const QString &ch )
{
    if( composites.current() )
        composites.current()->addContent( ch );
    return TRUE;
}

 
bool SVGHandler::endElement( const QString &, const QString &, const QString &qName )
{
    kdDebug() << "endElement:qName: " << qName.local8Bit() << endl;
    if( qName == "g" || qName == "svg" || qName == "text" || qName == "symbol" )
    {
	// perform group transformations and add final groups, i.e. without parents.
	if( composites.current()->hasIdentifier() )
	{
            kdDebug() << "Yes, adding : " << composites.current()->id().local8Bit() << endl;
            referencedSVG.insert( composites.current()->id(), composites.current() );
        }
	if( state != Defs )
	{
            GObject *obj = composites.current()->build();
            //composites.current()->performTransformations( obj );
            if( composites.current()->type() == "svg" && !composites.current()->hasParent() )
            {
	        objList.append( obj );
	        delete composites.current();
            }
        }
        composites.removeLast();
    }
    else if( qName == "defs" )
    {
        state = Normal;
        composites.removeLast();
    }
    return TRUE;
}


bool SVGHandler::fatalError( const QXmlParseException & /*exception*/ )
{
    return false; 
}

/********************* Use mechanism *******************************/

SVGElementInstance::SVGElementInstance( SVGComposite *parent, QString uri ) : correspondingElement(NULL) {
    uri.stripWhiteSpace();
    uri.replace( QRegExp( "#" ), QString::null );
    kdDebug() << "URI: " << uri.local8Bit() << endl;
    correspondingElement = referencedSVG[ uri ];
    if( correspondingElement ) correspondingElement->setParent( parent );
}


GObject *SVGElementInstance::build() {
    kdDebug() << "Going to build instance : " << correspondingElement << endl;
    return correspondingElement ? correspondingElement->build() : NULL;
}


GObject *SVGDefsElement::build() {
    return NULL;
}


void SVGUseElement::setAttributes( const QXmlAttributes& atts ) {
    setId( atts.value( "id" ) );
    transform = atts.value( "transform" );
    style = atts.value("style");
    group = new GGroup( 0 );

/*    if( !atts.value("x").isEmpty() ) x = convertNumToPt( atts.value("x") );
    if( !atts.value("y").isEmpty() ) y = convertNumToPt( atts.value("y") );
    if( !atts.value("width").isEmpty() )  width  = convertNumToPt( atts.value("width") );
    if( !atts.value("height").isEmpty() ) height = convertNumToPt( atts.value("height") );*/
    x.setValueAsString( atts.value("x") );
    x.convertToSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_PT );
    y.setValueAsString( atts.value("y") );
    y.convertToSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_PT );
    width.setValueAsString( atts.value("width") );
    width.convertToSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_PT );
    height.setValueAsString( atts.value("height") );
    height.convertToSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_PT );

    transform += "; translate(" + QString::number( x.value() ) + ", " + QString::number( y.value() ) + ")";
    instanceRoot = new SVGElementInstance( this, atts.value( "xlink:href" ) );
}


GObject *SVGUseElement::build() {

    performTransformations( group );
    setStyleProperties( group );

    GObject *obj = instanceRoot->build();
    if( obj ) group->addObject( obj );
    return group;
}
#include <svgimport.moc>
