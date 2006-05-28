/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <powerpointimport.h>
#include <powerpointimport.moc>

#include <QBuffer>
#include <q3cstring.h>
#include <QColor>
#include <QFile>
#include <QString>
#include <QTextStream>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>

#include "libppt.h"
#include <iostream>
#include <math.h>

using namespace Libppt;

typedef KGenericFactory<PowerPointImport> PowerPointImportFactory;
K_EXPORT_COMPONENT_FACTORY( libpowerpointimport, 
  PowerPointImportFactory( "kofficefilters" ) )

namespace Libppt {

inline QConstString string( const Libppt::UString& str ) 
{
   return QConstString( reinterpret_cast<const QChar*>( str.data() ), str.length() );
}

}

class PowerPointImport::Private
{
public:
  QString inputFile;
  QString outputFile;

  Presentation *presentation;  
};


PowerPointImport::PowerPointImport ( QObject*parent, const QStringList& )
    : KoFilter(parent)
{
  d = new Private;
}

PowerPointImport::~PowerPointImport()
{
  delete d;
}

KoFilter::ConversionStatus PowerPointImport::convert( const QByteArray& from, const QByteArray& to )
{
  if ( from != "application/mspowerpoint" )
    return KoFilter::NotImplemented; 

  if ( to != "application/vnd.oasis.opendocument.presentation" )     
    return KoFilter::NotImplemented;

  d->inputFile = m_chain->inputFile();
  d->outputFile = m_chain->outputFile();

  // open inputFile
  d->presentation = new Presentation;
  if( !d->presentation->load( d->inputFile.local8Bit() ) )
  {
    delete d->presentation;
    d->presentation = 0;
    return KoFilter::StupidError;
  }

  // create output store
  KoStore* storeout;
  storeout = KoStore::createStore( d->outputFile, KoStore::Write, 
    "application/vnd.oasis.opendocument.presentation",   KoStore::Zip );

  if ( !storeout )
  {
    kWarning() << "Couldn't open the requested file." << endl;
    return KoFilter::FileNotFound;
  }

  // store document content
  if ( !storeout->open( "content.xml" ) )
  {
    kWarning() << "Couldn't open the file 'content.xml'." << endl;
    return KoFilter::CreationError;
  }
  storeout->write( createContent() );
  storeout->close();

  // store document styles
  if ( !storeout->open( "styles.xml" ) )
  {
    kWarning() << "Couldn't open the file 'styles.xml'." << endl;
    return KoFilter::CreationError;
  }
  storeout->write( createStyles() );
  storeout->close();

  // store document manifest
  storeout->enterDirectory( "META-INF" );
  if ( !storeout->open( "manifest.xml" ) )
  {
     kWarning() << "Couldn't open the file 'META-INF/manifest.xml'." << endl;
     return KoFilter::CreationError;
  }
  storeout->write( createManifest() );
  storeout->close();


  // we are done!
  delete d->presentation;
  delete storeout;
  d->inputFile = QString::null;
  d->outputFile = QString::null;
  d->presentation = 0;

  return KoFilter::OK;
}

QByteArray PowerPointImport::createStyles()
{
  KoXmlWriter* stylesWriter;
  QByteArray stylesData;
  QBuffer stylesBuffer( &stylesData );

  QString pageWidth = QString("%1pt").arg( d->presentation->masterSlide()->pageWidth() );
  QString pageHeight = QString("%1pt").arg( d->presentation->masterSlide()->pageHeight() );
  
  stylesBuffer.open( QIODevice::WriteOnly );
  stylesWriter = new KoXmlWriter( &stylesBuffer );

  stylesWriter->startDocument( "office:document-styles" );
  stylesWriter->startElement( "office:document-styles" );
  stylesWriter->addAttribute( "xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
  stylesWriter->addAttribute( "xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" );
  stylesWriter->addAttribute( "xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0" );
  stylesWriter->addAttribute( "xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
  stylesWriter->addAttribute( "xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0" );
  stylesWriter->addAttribute( "xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" );
  stylesWriter->addAttribute( "office:version","1.0" );

  // office:styles
  stylesWriter->startElement( "office:styles" );
  stylesWriter->endElement();

  // office:automatic-styles
  stylesWriter->startElement( "office:automatic-styles" );
  stylesWriter->startElement( "style:page-layout" );
  stylesWriter->addAttribute( "style:name","pm1" );
  stylesWriter->addAttribute( "style:page-usage","all" );
  stylesWriter->startElement( "style:page-layout-properties" );
  stylesWriter->addAttribute( "fo:margin-bottom","0pt" ); 
  stylesWriter->addAttribute( "fo:margin-left","0pt" );   
  stylesWriter->addAttribute( "fo:margin-right","0pt" );  
  stylesWriter->addAttribute( "fo:margin-top","0pt" );    
  stylesWriter->addAttribute( "fo:page-height", pageHeight );
  stylesWriter->addAttribute( "fo:page-width", pageWidth );
  stylesWriter->addAttribute( "style:print-orientation","landscape" );
  stylesWriter->endElement(); // style:page-layout-properties
  stylesWriter->endElement(); // style:page-layout

  stylesWriter->startElement( "style:style" );
  stylesWriter->addAttribute( "style:name","dp1" );
  stylesWriter->addAttribute( "style:family","drawing-page" );
  stylesWriter->startElement( "style:drawing-page-properties" );
  stylesWriter->addAttribute( "draw:background-size","border" );
  stylesWriter->addAttribute( "draw:fill","solid" );
  stylesWriter->addAttribute( "draw:fill-color","#ffffff" );
  stylesWriter->endElement(); // style:drawing-page-properties
  stylesWriter->endElement(); // style:style

  stylesWriter->startElement( "style:style" );
  stylesWriter->addAttribute( "style:name","P1" );
  stylesWriter->addAttribute( "style:family","paragraph" );
  stylesWriter->startElement( "style:paragraph-properties" );
  stylesWriter->addAttribute( "fo:margin-left","0cm" );
  stylesWriter->addAttribute( "fo:margin-right","0cm" );
  stylesWriter->addAttribute( "fo:text-indent","0cm" );
  stylesWriter->endElement(); // style:paragraph-properties
  stylesWriter->startElement( "style:text-properties" );
  stylesWriter->addAttribute( "fo:font-size","14pt" );
  stylesWriter->addAttribute( "style:font-size-asian","14pt" );
  stylesWriter->addAttribute( "style:font-size-complex","14pt" );
  stylesWriter->endElement(); // style:text-properties
  stylesWriter->endElement(); // style:style

  stylesWriter->startElement( "text:list-style" ); 
  stylesWriter->addAttribute( "style:name","L2" );
  stylesWriter->startElement( "text:list-level-style-bullet" ); 
  stylesWriter->addAttribute( "text:level","1" );
  stylesWriter->addAttribute( "text:bullet-char","●" );
  stylesWriter->startElement( "style:text-properties" ); 
  stylesWriter->addAttribute( "fo:font-family","StarSymbol" );
  stylesWriter->addAttribute( "font-pitch","variable" );
  stylesWriter->addAttribute( "fo:color","#000000" );
  stylesWriter->addAttribute( "fo:font-size","45%" );
  stylesWriter->endElement(); // style:text-properties
  stylesWriter->endElement(); // text:list-level-style-bullet
  stylesWriter->endElement(); // text:list-style

  stylesWriter->endElement(); // office:automatic-styles

  // office:master-stylesborder
  stylesWriter->startElement( "office:master-styles" );
  stylesWriter->startElement( "style:master-page" );
  stylesWriter->addAttribute( "style:name", "Standard" );
  stylesWriter->addAttribute( "style:page-layout-name", "pm1" );
  stylesWriter->addAttribute( "draw:style-name", "dp1" );
  stylesWriter->endElement();
  stylesWriter->endElement();

  stylesWriter->endElement();  // office:document-styles
  stylesWriter->endDocument();
  delete stylesWriter;
#warning "kde4 port it"
#if 0
  // for troubleshooting only !!
  QString dbg;
  for( unsigned i=0; i<stylesData.size(); i++ )
    dbg.append( stylesData[i] );
  qDebug("\nstyles.xml:\n%s\n", dbg.latin1() );
#endif
  return stylesData;
}


QByteArray PowerPointImport::createContent()
{
  KoXmlWriter* contentWriter;
  QByteArray contentData;
  QBuffer contentBuffer( &contentData );

  contentBuffer.open( QIODevice::WriteOnly );
  contentWriter = new KoXmlWriter( &contentBuffer );

  contentWriter->startDocument( "office:document-content" );
  contentWriter->startElement( "office:document-content" );
  contentWriter->addAttribute( "xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
  contentWriter->addAttribute( "xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0" );
  contentWriter->addAttribute( "xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
  contentWriter->addAttribute( "xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" );
  contentWriter->addAttribute( "xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0" );
  contentWriter->addAttribute( "xmlns:svg","urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" );
  contentWriter->addAttribute( "office:version","1.0" );

  // office:automatic-styles

  drawingObjectCounter = 0;
  contentWriter->startElement( "office:automatic-styles" );
  for( unsigned c=0; c < d->presentation->slideCount(); c++ )
  {
    Slide* slide = d->presentation->slide( c );
    processSlideForStyle( c, slide, contentWriter );
  }
  contentWriter->endElement();



  // office:body

  contentWriter->startElement( "office:body" );
  contentWriter->startElement( "office:presentation" );

  drawingObjectCounter = 0;
  for( unsigned c=0; c < d->presentation->slideCount(); c++ )
  {
    Slide* slide = d->presentation->slide( c );
    processSlideForBody( c, slide, contentWriter );
  }

  contentWriter->endElement();  // office:presentation
  contentWriter->endElement();  // office:body

  contentWriter->endElement();  // office:document-content
  contentWriter->endDocument();
  delete contentWriter;
#warning "kde4 port it"
#if 0
  // for troubleshooting only !!
  QString dbg;
  for( unsigned i=0; i<contentData.size(); i++ )
    dbg.append( contentData[i] );
  qDebug("\ncontent.xml:\n%s\n", dbg.latin1() );
#endif
  return contentData;
}

QByteArray PowerPointImport::createManifest()
{
  KoXmlWriter* manifestWriter;
  QByteArray manifestData;
  QBuffer manifestBuffer( &manifestData );

  manifestBuffer.open( QIODevice::WriteOnly );
  manifestWriter = new KoXmlWriter( &manifestBuffer );

  manifestWriter->startDocument( "manifest:manifest" );
  manifestWriter->startElement( "manifest:manifest" );
  manifestWriter->addAttribute( "xmlns:manifest", 
    "urn:oasis:names:tc:openoffice:xmlns:manifest:1.0" );

  manifestWriter->addManifestEntry( "/", "application/vnd.oasis.opendocument.presentation" );
  manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
  manifestWriter->addManifestEntry( "content.xml", "text/xml" );

  manifestWriter->endElement();
  manifestWriter->endDocument();
  delete manifestWriter;
#warning "kde4: port it"
#if 0
  // for troubleshooting only !!
  QString dbg;
  for( unsigned i=0; i<manifestData.size(); i++ )
    dbg.append( manifestData[i] );
  qDebug("\nmanifest.xml:\n%s\n", dbg.latin1() );
#endif
  return manifestData;
}

void PowerPointImport::processEllipse (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:ellipse" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
  xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->endElement(); // draw:ellipse
}

void PowerPointImport::processRectangle (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject ) return;
  if( !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:rect" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  if ( drawObject->hasProperty ( "libppt:rotation" ) )
  {     
    
     double rotAngle = drawObject->getDoubleProperty("libppt:rotation" );
     double xMid = ( drawObject->left() + 0.5*drawObject->width() ); 
     double yMid = ( drawObject->top() + 0.5*drawObject->height() ); 
     double xVec = drawObject->left()- xMid;   
     double yVec = yMid - drawObject->top(); 

     double xNew = xVec*cos(rotAngle) - yVec*sin(rotAngle);
     double yNew = xVec*sin(rotAngle) + yVec*cos(rotAngle); 
     QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew+xMid).arg(yMid-yNew); 
     xmlWriter->addAttribute( "draw:transform", rot );        
  }
  else 
  { 
    xmlWriter->addAttribute( "svg:x", xStr );
    xmlWriter->addAttribute( "svg:y", yStr );
  }
  xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->endElement(); // draw:rect
}

void PowerPointImport::processRoundRectangle (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:custom-shape" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  
     
  if ( drawObject->hasProperty ( "libppt:rotation" ) )
  {     
     double rotAngle = drawObject->getDoubleProperty("libppt:rotation" );
    
  
    
    if ( rotAngle > 0.785399 ) // > 45 deg
     {  xmlWriter->addAttribute( "svg:width", heightStr );
        xmlWriter->addAttribute( "svg:height", widthStr );
        double xMid = ( drawObject->left() - 0.5*drawObject->height() ); 
        double yMid = ( drawObject->top() + 0.5*drawObject->width() ); 
        double xVec = drawObject->left()- xMid;   
        double yVec =  drawObject->top()- yMid; 

        double xNew = xVec*cos(rotAngle) - yVec*sin(rotAngle);
        double yNew = xVec*sin(rotAngle) + yVec*cos(rotAngle); 
        QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew+xMid).arg(yMid+yNew); 
        xmlWriter->addAttribute( "draw:transform", rot );        
     }  
     else
     {  xmlWriter->addAttribute( "svg:width", widthStr );
        xmlWriter->addAttribute( "svg:height", heightStr );
        double xMid = ( drawObject->left() + 0.5*drawObject->width() ); 
        double yMid = ( drawObject->top() + 0.5*drawObject->height() ); 
        double xVec = drawObject->left()- xMid;   
        double yVec = yMid - drawObject->top(); 

        double xNew = xVec*cos(rotAngle) - yVec*sin(rotAngle);
        double yNew = xVec*sin(rotAngle) + yVec*cos(rotAngle); 
        QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew+xMid).arg(yMid-yNew); 
        xmlWriter->addAttribute( "draw:transform", rot );                
     } 
     
     
  }
  else 
  { xmlWriter->addAttribute( "svg:width", widthStr );
    xmlWriter->addAttribute( "svg:height", heightStr );
    xmlWriter->addAttribute( "svg:x", xStr );
    xmlWriter->addAttribute( "svg:y", yStr );
  } 
 // xmlWriter->addAttribute( "svg:x", xStr );
 // xmlWriter->addAttribute( "svg:y", yStr );
  
  xmlWriter->addAttribute( "draw:layer", "layout" );
    xmlWriter->startElement( "draw:enhanced-geometry" );
    xmlWriter->addAttribute( "draw:type", "round-rectangle"); 
      xmlWriter->startElement( "draw:equation" );
      xmlWriter->addAttribute( "draw:formula", "$0 /3" );
      xmlWriter->addAttribute( "draw:name", "f0" ); 
      xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" );
      xmlWriter->addAttribute( "draw:formula", "right-?f0 " );
      xmlWriter->addAttribute( "draw:name", "f1" ); 
      xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" );
      xmlWriter->addAttribute( "draw:formula", "bottom-?f0 " );
      xmlWriter->addAttribute( "draw:name", "f2" ); 
      xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" );
      xmlWriter->addAttribute( "draw:formula", "left+?f0 " );
      xmlWriter->addAttribute( "draw:name", "f3" ); 
      xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" );
      xmlWriter->addAttribute( "draw:formula", "top+?f0 " );
      xmlWriter->addAttribute( "draw:name", "f4" ); 
      xmlWriter->endElement(); // draw:equation 
    xmlWriter->endElement(); // draw:enhanced-geometry
  xmlWriter->endElement(); // draw:custom-shape
}

void PowerPointImport::processDiamond (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:custom-shape" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 5 );
    xmlWriter->addAttribute( "svg:y", 0 );
    xmlWriter->endElement(); 
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 0 );
    xmlWriter->addAttribute( "svg:y", 5 );
    xmlWriter->endElement(); 
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 5 );
    xmlWriter->addAttribute( "svg:y", 10 );
    xmlWriter->endElement(); 
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 10 );
    xmlWriter->addAttribute( "svg:y", 5 );
    xmlWriter->endElement();
    xmlWriter->startElement( "draw:enhanced-geometry" );
    xmlWriter->addAttribute( "draw:type", "diamond");
    xmlWriter->endElement();    
  xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->endElement(); 
}

void PowerPointImport::processTriangle (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  /* draw IsocelesTriangle or RightTriangle */
  xmlWriter->startElement( "draw:custom-shape" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
  xmlWriter->addAttribute( "draw:layer", "layout" );
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 5 );
    xmlWriter->addAttribute( "svg:y", 0 );
    xmlWriter->endElement(); 
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 2.5 );
    xmlWriter->addAttribute( "svg:y", 5 );
    xmlWriter->endElement(); 
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 0 );
    xmlWriter->addAttribute( "svg:y", 10 );
    xmlWriter->endElement(); 
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 5 );
    xmlWriter->addAttribute( "svg:y", 10 );
    xmlWriter->endElement();
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 10 );
    xmlWriter->addAttribute( "svg:y", 10 );
    xmlWriter->endElement();
    xmlWriter->startElement( "draw:glue-point" );
    xmlWriter->addAttribute( "svg:x", 7.5 );
    xmlWriter->addAttribute( "svg:y", 5 );
    xmlWriter->endElement();

    xmlWriter->startElement( "draw:enhanced-geometry" );
       
    if  (drawObject->hasProperty("draw:mirror-vertical"))
    { 
      xmlWriter->addAttribute("draw:mirror-vertical","true");         
    } 
    if ( drawObject->hasProperty("draw:mirror-horizontal"))
    { 
      xmlWriter->addAttribute("draw:mirror-horizontal","true");         
    }
    if ( drawObject->hasProperty ( "libppt:rotation" ) )
    { // draw:transform="rotate (1.5707963267946) translate (6.985cm 14.181cm)" 
       
     double rotAngle = drawObject->getDoubleProperty("libppt:rotation" );
     double xMid = ( drawObject->left() + 0.5*drawObject->width() ); 
     double yMid = ( drawObject->top() + 0.5*drawObject->height() ); 
     QString rot = QString("rotate (%1) translate (%2cm %3cm)").arg(rotAngle).arg(xMid).arg(yMid); 
     xmlWriter->addAttribute( "draw:transform", rot );   
    }
    if (drawObject->shape() == DrawObject::RightTriangle)
    {
    xmlWriter->addAttribute( "draw:type", "right-triangle");
    }
    else if (drawObject->shape() == DrawObject::IsoscelesTriangle)
    {    
    xmlWriter->addAttribute( "draw:type", "isosceles-triangle");
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 " );
          xmlWriter->addAttribute( "draw:name", "f0" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 /2" );
          xmlWriter->addAttribute( "draw:name", "f1" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f1 +10800" );
          xmlWriter->addAttribute( "draw:name", "f2" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 *2/3" );
          xmlWriter->addAttribute( "draw:name", "f3" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f3 +7200" );
          xmlWriter->addAttribute( "draw:name", "f4" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f0 " );
          xmlWriter->addAttribute( "draw:name", "f5" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f5 /2" );
          xmlWriter->addAttribute( "draw:name", "f6" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f6 " );
          xmlWriter->addAttribute( "draw:name", "f7" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:handle" );
    xmlWriter->addAttribute( "draw:handle-range-x-maximum", 21600);  
    xmlWriter->addAttribute( "draw:handle-range-x-minimum", 0);
      xmlWriter->addAttribute( "draw:handle-position","$0 top");         
          xmlWriter->endElement();    
    }
   
    xmlWriter->endElement();    // enhanced-geometry  
  xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processTrapezoid (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:custom-shape" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
  xmlWriter->addAttribute( "draw:layer", "layout" );
  
        xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 0 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 2.5 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 0 );
  xmlWriter->addAttribute( "svg:y", 10 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 10 );
  xmlWriter->endElement();
        xmlWriter->startElement( "draw:enhanced-geometry" );
        if ( drawObject->hasProperty("draw:mirror-vertical") ) 
        { 
          xmlWriter->addAttribute("draw:mirror-vertical","true");         
        } 
        if ( drawObject->hasProperty("draw:mirror-horizontal")) 
        { 
          xmlWriter->addAttribute("draw:mirror-horizontal","true");         
        }
  xmlWriter->addAttribute( "draw:type", "trapezoid" );
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-$0 " );
          xmlWriter->addAttribute( "draw:name", "f0" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0" );
          xmlWriter->addAttribute( "draw:name", "f1" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 *10/18" );
          xmlWriter->addAttribute( "draw:name", "f2" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f2 +1750");
          xmlWriter->addAttribute( "draw:name", "f3" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f3" );
          xmlWriter->addAttribute( "draw:name", "f4" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 /2" );
          xmlWriter->addAttribute( "draw:name", "f5" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f5" );
          xmlWriter->addAttribute( "draw:name", "f6" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:handle" );
    xmlWriter->addAttribute( "draw:handle-range-x-maximum", 10800);  
    xmlWriter->addAttribute( "draw:handle-range-x-minimum", 0);
      xmlWriter->addAttribute("draw:handle-position","$0 bottom");         
          xmlWriter->endElement();    
  xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape  
}

void PowerPointImport::processParallelogram (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:custom-shape" );
  xmlWriter->addAttribute( "draw:style-name", styleName );  
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
  xmlWriter->addAttribute( "draw:layer", "layout" );
        xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 6.25 );
  xmlWriter->addAttribute( "svg:y", 0 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 4.5 );
  xmlWriter->addAttribute( "svg:y", 0 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 8.75 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 3.75 );
  xmlWriter->addAttribute( "svg:y", 10 );
  xmlWriter->endElement();
        xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 10 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 1.25 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement();
        xmlWriter->startElement( "draw:enhanced-geometry" );
        if  (drawObject->hasProperty("draw:mirror-vertical"))
        { 
          xmlWriter->addAttribute("draw:mirror-vertical","true");         
        } 
        if ( drawObject->hasProperty("draw:mirror-horizontal"))
        { 
          xmlWriter->addAttribute("draw:mirror-horizontal","true");         
        }
  xmlWriter->addAttribute( "draw:type", "parallelogram" );
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 " );
          xmlWriter->addAttribute( "draw:name", "f0" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-$0" );
          xmlWriter->addAttribute( "draw:name", "f1" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 *10/24" );
          xmlWriter->addAttribute( "draw:name", "f2" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f2 +1750");
          xmlWriter->addAttribute( "draw:name", "f3" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f3" );
          xmlWriter->addAttribute( "draw:name", "f4" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f0 /2" );
          xmlWriter->addAttribute( "draw:name", "f5" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "10800+?f5" );
          xmlWriter->addAttribute( "draw:name", "f6" ); 
          xmlWriter->endElement();    
    xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f0-10800 " );
          xmlWriter->addAttribute( "draw:name", "f7" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "if(?f7,?f12,0" );
          xmlWriter->addAttribute( "draw:name", "f8" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "10800-?f5" );
          xmlWriter->addAttribute( "draw:name", "f9" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "if(?f7, ?f12, 21600");
          xmlWriter->addAttribute( "draw:name", "f10" );  
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f5" );
          xmlWriter->addAttribute( "draw:name", "f11" );  
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600*10800/?f0" );
          xmlWriter->addAttribute( "draw:name", "f12" );  
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f12" );
          xmlWriter->addAttribute( "draw:name", "f13" );  
          xmlWriter->endElement();    
    xmlWriter->startElement( "draw:handle" );
    xmlWriter->addAttribute( "draw:handle-range-x-maximum", 21600);  
    xmlWriter->addAttribute( "draw:handle-range-x-minimum", 0);
      xmlWriter->addAttribute("draw:handle-position","$0 top");         
          xmlWriter->endElement();    
  xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape  
}

void PowerPointImport::processHexagon (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

    xmlWriter->startElement( "draw:custom-shape" );
    xmlWriter->addAttribute( "draw:style-name", styleName );
    xmlWriter->addAttribute( "svg:width", widthStr );
    xmlWriter->addAttribute( "svg:height", heightStr );
    xmlWriter->addAttribute( "svg:x", xStr );
    xmlWriter->addAttribute( "svg:y", yStr );
    xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 0 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 0 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 10 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 10 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement();
        xmlWriter->startElement( "draw:enhanced-geometry" );
  xmlWriter->addAttribute( "draw:type", "hexagon" );
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 " );
          xmlWriter->addAttribute( "draw:name", "f0" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-$0" );
          xmlWriter->addAttribute( "draw:name", "f1" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 *100/234" );
          xmlWriter->addAttribute( "draw:name", "f2" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "?f2 +1700");
          xmlWriter->addAttribute( "draw:name", "f3" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "21600-?f3" );
          xmlWriter->addAttribute( "draw:name", "f4" ); 
          xmlWriter->endElement();     
      xmlWriter->startElement( "draw:handle" );
    xmlWriter->addAttribute( "draw:handle-range-x-maximum", 10800);  
    xmlWriter->addAttribute( "draw:handle-range-x-minimum", 0);
      xmlWriter->addAttribute("draw:handle-position","$0 top");         
          xmlWriter->endElement();    
  xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processOctagon (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:custom-shape" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
  xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 0 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 0 );
  xmlWriter->addAttribute( "svg:y", 4.782 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 10 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 10 );
  xmlWriter->addAttribute( "svg:y", 4.782 );
  xmlWriter->endElement();
        xmlWriter->startElement( "draw:enhanced-geometry" );
  xmlWriter->addAttribute( "draw:type", "octagon" );
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "left+$0 " );
          xmlWriter->addAttribute( "draw:name", "f0" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "top+$0 " );
          xmlWriter->addAttribute( "draw:name", "f1" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "right-$0 " );
          xmlWriter->addAttribute( "draw:name", "f2" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "bottom-$0 ");
          xmlWriter->addAttribute( "draw:name", "f3" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0 /2" );
          xmlWriter->addAttribute( "draw:name", "f4" ); 
          xmlWriter->endElement();     
    xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "left+?f4 " );
          xmlWriter->addAttribute( "draw:name", "f5" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "top+?f4 " );
          xmlWriter->addAttribute( "draw:name", "f6" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "right-?f4 " );
          xmlWriter->addAttribute( "draw:name", "f7" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "bottom-?f4 ");
          xmlWriter->addAttribute( "draw:name", "f8" ); 
          xmlWriter->endElement();    
          xmlWriter->startElement( "draw:handle" );
    xmlWriter->addAttribute( "draw:handle-range-x-maximum", 10800);  
    xmlWriter->addAttribute( "draw:handle-range-x-minimum", 0);
      xmlWriter->addAttribute("draw:handle-position","$0 top");         
          xmlWriter->endElement();    
  xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape  
}

void PowerPointImport::processArrow (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "draw:custom-shape" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
  xmlWriter->addAttribute( "draw:layer", "layout" );
    xmlWriter->startElement( "draw:enhanced-geometry" );
    
                if (drawObject->shape() == DrawObject::RightArrow)
                xmlWriter->addAttribute( "draw:type", "right-arrow" );
      else if (drawObject->shape() == DrawObject::LeftArrow)
      xmlWriter->addAttribute( "draw:type", "left-arrow" );
    else if (drawObject->shape() == DrawObject::UpArrow)
      xmlWriter->addAttribute( "draw:type", "up-arrow" );
    else if (drawObject->shape() == DrawObject::DownArrow)
      xmlWriter->addAttribute( "draw:type", "down-arrow" );   
    xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","$1");  
    xmlWriter->addAttribute( "draw:name","f0");  
                xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","$0");  
                xmlWriter->addAttribute( "draw:name","f1");  
    xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","21600-$1");  
                xmlWriter->addAttribute( "draw:name","f2");  
    xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","21600-?f1");  
                xmlWriter->addAttribute( "draw:name","f3");  
    xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","?f3 *?f0 /10800");  
    xmlWriter->addAttribute( "draw:name","f4");  
                xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","?f1 +?f4 " );
    xmlWriter->addAttribute( "draw:name","f5");    
                xmlWriter->endElement(); // draw:equation
      xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","?f1 *?f0 /10800");  
    xmlWriter->addAttribute( "draw:name","f6");  
                xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement( "draw:equation" ); 
                xmlWriter->addAttribute( "draw:formula","?f1 -?f6 ");  
    xmlWriter->addAttribute( "draw:name","f7");  
                xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement( "draw:handle" );
                if ( drawObject->shape() == DrawObject::RightArrow | drawObject->shape() == DrawObject::LeftArrow ) 
                {
      xmlWriter->addAttribute( "draw:handle-range-x-maximum", 21600);  
      xmlWriter->addAttribute( "draw:handle-range-x-minimum", 0);
        xmlWriter->addAttribute("draw:handle-position","$0 $1"); 
      xmlWriter->addAttribute("draw:handle-range-y-maximum",10800);
      xmlWriter->addAttribute("draw:handle-range-y-minimum",0);
    }
    else if ( drawObject->shape() == DrawObject::UpArrow | drawObject->shape() == DrawObject::DownArrow )
                {
                  xmlWriter->addAttribute( "draw:handle-range-x-maximum", 10800);  
      xmlWriter->addAttribute( "draw:handle-range-x-minimum", 0);
        xmlWriter->addAttribute("draw:handle-position","$1 $0"); 
      xmlWriter->addAttribute("draw:handle-range-y-maximum",21600);
      xmlWriter->addAttribute("draw:handle-range-y-minimum",0);            
    }
                xmlWriter->endElement(); // draw:handle
      xmlWriter->endElement(); // draw:enhanced-geometry  
    xmlWriter->endElement(); // draw:custom-shape
}

void PowerPointImport::processLine (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter) return;

  QString x1Str = QString("%1mm").arg( drawObject->left() );
  QString y1Str = QString("%1mm").arg( drawObject->top() );
  QString x2Str = QString("%1mm").arg( drawObject->left() + drawObject->width() );
  QString y2Str = QString("%1mm").arg( drawObject->top() + drawObject->height() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  if ( drawObject->hasProperty("draw:mirror-vertical") )
    { QString temp = y1Str; 
      y1Str = y2Str;
      y2Str = temp;
    }
  if ( drawObject->hasProperty("draw:mirror-horizontal") )
    { QString temp = x1Str;  
      x1Str = x2Str; 
      x2Str = temp;
    }
  
  xmlWriter->startElement( "draw:line" );
  xmlWriter->addAttribute( "draw:style-name", styleName );
  xmlWriter->addAttribute( "svg:y1", y1Str );
  xmlWriter->addAttribute( "svg:y2", y2Str );
  xmlWriter->addAttribute( "svg:x1", x1Str );
  xmlWriter->addAttribute( "svg:x2", x2Str );
  xmlWriter->addAttribute( "draw:layer", "layout" );
 
 xmlWriter->endElement(); 
}

void PowerPointImport::processSmiley (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject ||!xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

    xmlWriter->startElement( "draw:custom-shape" );
    xmlWriter->addAttribute( "draw:style-name", styleName );
    xmlWriter->addAttribute( "svg:width", widthStr );
    xmlWriter->addAttribute( "svg:height", heightStr );
    xmlWriter->addAttribute( "svg:x", xStr );
    xmlWriter->addAttribute( "svg:y", yStr );
    xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 0 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 1.461 );
  xmlWriter->addAttribute( "svg:y", 1.461 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 0 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 1.461 );
  xmlWriter->addAttribute( "svg:y", 8.536 );
  xmlWriter->endElement();
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 10 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement();
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 8.536 );
  xmlWriter->addAttribute( "svg:y", 1.461 );
  xmlWriter->endElement();
        xmlWriter->startElement( "draw:enhanced-geometry" );
  xmlWriter->addAttribute( "draw:type", "smiley" );
          xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "$0-15510 " );
          xmlWriter->addAttribute( "draw:name", "f0" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "17520-?f0" );
          xmlWriter->addAttribute( "draw:name", "f1" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:equation" );
          xmlWriter->addAttribute( "draw:formula", "15510+?f0" );
          xmlWriter->addAttribute( "draw:name", "f2" ); 
          xmlWriter->endElement();    
      xmlWriter->startElement( "draw:handle" );
          xmlWriter->addAttribute( "draw:position", 10800);  
    xmlWriter->addAttribute( "draw:handle-range-y-maximum", 17520);  
    xmlWriter->addAttribute( "draw:handle-range-y-minimum", 15510);
      xmlWriter->addAttribute("draw:handle-position","$0 top");         
          xmlWriter->endElement();    
  xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processHeart (DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

    xmlWriter->startElement( "draw:custom-shape" );
    xmlWriter->addAttribute( "draw:style-name", styleName );
    xmlWriter->addAttribute( "svg:width", widthStr );
    xmlWriter->addAttribute( "svg:height", heightStr );
    xmlWriter->addAttribute( "svg:x", xStr );
    xmlWriter->addAttribute( "svg:y", yStr );
    xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 1 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 1.43 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement(); 
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 5 );
  xmlWriter->addAttribute( "svg:y", 10 );
  xmlWriter->endElement();
  xmlWriter->startElement( "draw:glue-point" );
  xmlWriter->addAttribute( "svg:x", 8.553 );
  xmlWriter->addAttribute( "svg:y", 5 );
  xmlWriter->endElement();
  xmlWriter->startElement( "draw:enhanced-geometry" );
  xmlWriter->addAttribute( "draw:type", "heart" );
        
        xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processFreeLine (DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
  if( !drawObject ||!xmlWriter ) return;

  QString widthStr = QString("%1mm").arg( drawObject->width() );
  QString heightStr = QString("%1mm").arg( drawObject->height() );
  QString xStr = QString("%1mm").arg( drawObject->left() );
  QString yStr = QString("%1mm").arg( drawObject->top() );
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

    xmlWriter->startElement( "draw:path" );
    xmlWriter->addAttribute( "draw:style-name", styleName );
    xmlWriter->addAttribute( "svg:width", widthStr );
    xmlWriter->addAttribute( "svg:height", heightStr );
    xmlWriter->addAttribute( "svg:x", xStr );
    xmlWriter->addAttribute( "svg:y", yStr );
    xmlWriter->addAttribute( "draw:layer", "layout" );  
    xmlWriter->endElement(); // path
}

void PowerPointImport::processDrawingObjectForBody( DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
 
  if( !drawObject || !xmlWriter ) return;

  drawingObjectCounter++;
  
   
  if (drawObject->shape() == DrawObject::Ellipse)
  { 
    processEllipse (drawObject, xmlWriter ); 
  }
  else if (drawObject->shape() == DrawObject::Rectangle)
  { 
    processRectangle (drawObject, xmlWriter ); 
  }
  else if (drawObject->shape() == DrawObject::RoundRectangle)
  { 
    processRoundRectangle (drawObject, xmlWriter );
  }
  else  if (drawObject->shape() == DrawObject::Diamond)
  {  
    processDiamond (drawObject, xmlWriter ); 
  } 
  else  if (drawObject->shape() == DrawObject::IsoscelesTriangle |                
            drawObject->shape() == DrawObject::RightTriangle)
  {  
    processTriangle (drawObject, xmlWriter ); 
  }
  else if (drawObject->shape() == DrawObject::Trapezoid)
  {  
    processTrapezoid (drawObject, xmlWriter ); 
  }
  else if (drawObject->shape() == DrawObject::Parallelogram)
  {  
    processParallelogram( drawObject, xmlWriter); 
  }
  else if (drawObject->shape() == DrawObject::Hexagon)
  {  
    processHexagon ( drawObject, xmlWriter );
  }
  else if (drawObject->shape() == DrawObject::Octagon)
  {  
    processOctagon ( drawObject, xmlWriter );
  }
  else if (drawObject->shape() == DrawObject::RightArrow | 
     drawObject->shape() == DrawObject::LeftArrow | 
     drawObject->shape() == DrawObject::UpArrow | 
     drawObject->shape() == DrawObject::DownArrow )
  { 
    processArrow ( drawObject, xmlWriter );
  }
  else if (drawObject->shape() == DrawObject::Line)
  {  
    processLine ( drawObject, xmlWriter );
  }
  else if (drawObject->shape() == DrawObject::Smiley)
  { 
    processSmiley (drawObject, xmlWriter ); 
  }
  else if (drawObject->shape() == DrawObject::Heart)
  { 
    processHeart (drawObject, xmlWriter ); 
  }
  else if (drawObject->shape() == DrawObject::FreeLine)
  { 
    processFreeLine (drawObject, xmlWriter ); 
  }
}

void PowerPointImport::processGroupObjectForBody( GroupObject* groupObject, KoXmlWriter* xmlWriter )
{
  if( !groupObject || !xmlWriter ) return;
  if( !groupObject->objectCount() ) return;

  xmlWriter->startElement( "draw:g" );

  for( unsigned i = 0; i < groupObject->objectCount(); i++ )
  {
    Object* object = groupObject->object( i );
    if( object ) 
      processObjectForBody( object, xmlWriter );
  }

  xmlWriter->endElement(); // draw:g
}

void PowerPointImport::processTextObjectForBody( TextObject* textObject, KoXmlWriter* xmlWriter )
{
  if( !textObject || !xmlWriter ) return;

  QString classStr = "subtitle";
  if( textObject->type() == TextObject::Title )
    classStr = "title";
//  QString pStr = string( textObject->text() ).string();
  QString pStr;
 
  QString widthStr = QString("%1mm").arg( textObject->width() );
  QString heightStr = QString("%1mm").arg( textObject->height() );
  QString xStr = QString("%1mm").arg( textObject->left() );
  QString yStr = QString("%1mm").arg( textObject->top() );

  xmlWriter->startElement( "draw:frame" );
  xmlWriter->addAttribute( "presentation:style-name", "pr1" );
  xmlWriter->addAttribute( "draw:layer", "layout" );
  xmlWriter->addAttribute( "svg:width", widthStr );
  xmlWriter->addAttribute( "svg:height", heightStr );
  xmlWriter->addAttribute( "svg:x", xStr );
  xmlWriter->addAttribute( "svg:y", yStr );
  xmlWriter->addAttribute( "presentation:class", classStr );
    xmlWriter->startElement( "draw:text-box" );
  
  // count error if i begins with 0
    for (unsigned i=0; i<textObject->listSize(); i++)
    {
      pStr = Libppt::string(textObject->text(i)).string();
      if (textObject->bulletFlag(i) == 1 )
      {
        xmlWriter->startElement( "text:list" ); 
        xmlWriter->addAttribute( "text:style-name","L2"); 
        xmlWriter->startElement( "text:list-item" );          
        xmlWriter->startElement( "text:p" ); 
        xmlWriter->addAttribute( "text:style-name", "P1" );
        xmlWriter->addTextSpan( pStr ); 
        xmlWriter->endElement(); // text:p
        xmlWriter->endElement();  // text:list-item
        xmlWriter->endElement();  // text:list
      }
      else 
      {
        xmlWriter->startElement( "text:p" );
        xmlWriter->addAttribute( "text:style-name", "P1" );
        xmlWriter->addTextSpan( pStr ); 
        xmlWriter->endElement(); // text:p
      } 
    }

    xmlWriter->endElement(); // draw:text-box
  xmlWriter->endElement(); // draw:frame


}

void PowerPointImport::processObjectForBody( Object* object, KoXmlWriter* xmlWriter )
{
  if( !object ||  !xmlWriter) return;

  if( object->isText() )
    processTextObjectForBody( static_cast<TextObject*>(object), xmlWriter );
  else if( object->isGroup() )
    processGroupObjectForBody( static_cast<GroupObject*>(object), xmlWriter );
  else if( object->isDrawing() )
    processDrawingObjectForBody( static_cast<DrawObject*>(object), xmlWriter );
}

void PowerPointImport::processSlideForBody( unsigned slideNo, Slide* slide, KoXmlWriter* xmlWriter )
{
  if( !slide || !xmlWriter ) return;

  QString nameStr = Libppt::string( slide->title() ).string();
  if( nameStr.isEmpty() )
    nameStr = QString("page%1").arg(slideNo+1);

  QString styleNameStr = QString("dp%1").arg(slideNo+1);

  xmlWriter->startElement( "draw:page" );
  xmlWriter->addAttribute( "draw:master-page-name", "Default" );
  xmlWriter->addAttribute( "draw:name", nameStr );
  xmlWriter->addAttribute( "draw:style-name", styleNameStr );
  xmlWriter->addAttribute( "presentation:presentation-page-layout-name", "AL1T0");

  GroupObject* root = slide->rootObject();
  if( root )
  for( unsigned i = 0; i < root->objectCount(); i++ )
  {
    Object* object = root->object( i );
    if( object ) 
      processObjectForBody( object, xmlWriter );
  }

  xmlWriter->endElement(); // draw:page
}

void PowerPointImport::processSlideForStyle( unsigned , Slide* slide, KoXmlWriter* xmlWriter )
{
  if( !slide || !xmlWriter ) return;

  GroupObject* root = slide->rootObject();
  if( root )
  for( unsigned i = 0; i < root->objectCount(); i++ )
  {
    Object* object = root->object( i );
    if( object ) 
      processObjectForStyle( object, xmlWriter );
  }
}

void PowerPointImport::processObjectForStyle( Object* object, KoXmlWriter* xmlWriter )
{
  if( !object || !xmlWriter  ) return;

  if( object->isText() )
    processTextObjectForStyle( static_cast<TextObject*>(object), xmlWriter );
  else if( object->isGroup() )
    processGroupObjectForStyle( static_cast<GroupObject*>(object), xmlWriter );
  else if( object->isDrawing() )
    processDrawingObjectForStyle( static_cast<DrawObject*>(object), xmlWriter );
}

void PowerPointImport::processTextObjectForStyle( TextObject* textObject, KoXmlWriter* xmlWriter )
{
  if( !textObject || !xmlWriter ) return;
}

void PowerPointImport::processGroupObjectForStyle( GroupObject* groupObject, KoXmlWriter* xmlWriter )
{
  if( !groupObject ||!xmlWriter ) return;
}

QString hexname( const Color &c )
{
  QColor qc( c.red, c.green, c.blue );
  return qc.name();
}

void PowerPointImport::processDrawingObjectForStyle( DrawObject* drawObject, KoXmlWriter* xmlWriter )
{
  if( !drawObject || !xmlWriter) return;

  drawingObjectCounter++;
  QString styleName = QString("gr%1").arg( drawingObjectCounter );

  xmlWriter->startElement( "style:style" );
  xmlWriter->addAttribute( "style:name", styleName );
  xmlWriter->addAttribute( "style:family", "graphic" );
  xmlWriter->addAttribute( "style:parent-style-name", "standard" );

    xmlWriter->startElement( "style:graphic-properties" );
/*
    if (drawObject->hasProperty("draw:stroke") )        
     { 
       xmlWriter->addAttribute( "draw:stroke", "dash" );
       std::string s = drawObject->getStrProperty("draw:stroke-dash");
       QString ss( s.c_str() );  
       xmlWriter->addAttribute( "draw:stroke-dash", ss );  
     } else xmlWriter->addAttribute( "draw:stroke", "dash" );
*/
    if (drawObject->hasProperty( "libppt:invisibleLine" ))
    { 
      if (drawObject->getBoolProperty("libppt:invisibleLine") == true)
          xmlWriter->addAttribute( "draw:stroke", "none" );  
    }
    else if( drawObject->hasProperty( "draw:stroke" )  )
    {  if (drawObject->getStrProperty( "draw:stroke" ) == "dash")
              {
                xmlWriter->addAttribute( "draw:stroke", "dash" );
                std::string s = drawObject->getStrProperty("draw:stroke-dash");
                QString ss( s.c_str() );  
                xmlWriter->addAttribute( "draw:stroke-dash", ss );  
                qDebug("=============stroke dash===================");
// qDebug("test %g\n",angka);
              }
         else if (drawObject->getStrProperty( "draw:stroke" ) == "solid")
              xmlWriter->addAttribute( "draw:stroke", "solid" );
    }
    

    if( drawObject->hasProperty( "svg:stroke-width" ) )
    {
      double strokeWidth = drawObject->getDoubleProperty("svg:stroke-width" );
      xmlWriter->addAttribute( "svg:stroke-width",QString("%1mm").arg( strokeWidth ) );
    }

    if( drawObject->hasProperty( "svg:stroke-color" ) )
    {
      Color strokeColor = drawObject->getColorProperty("svg:stroke-color" );
      xmlWriter->addAttribute( "svg:stroke-color", hexname( strokeColor ) );
    }

    if( drawObject->hasProperty( "draw:marker-start" ) )
    {
      std::string s = drawObject->getStrProperty("draw:marker-start");
      QString ss( s.c_str() );
      xmlWriter->addAttribute( "draw:marker-start", ss ); 
    }
    if( drawObject->hasProperty( "draw:marker-end" ) )
    {
      std::string s = drawObject->getStrProperty("draw:marker-end");
      QString ss( s.c_str() );
      xmlWriter->addAttribute( "draw:marker-end", ss);
    }
/*   if( drawObject->hasProperty( "draw:marker-start-length" ) )
    {
      std::string s = drawObject->getStrProperty("draw:marker-start-length");
      QString ss( s.c_str() );
      xmlWriter->addAttribute("draw:marker-start-length", ss );
    }
    if( drawObject->hasProperty( "draw:marker-end-length" ) )
    {
      std::string s = drawObject->getStrProperty("draw:marker-end-length");
      QString ss( s.c_str() );
      xmlWriter->addAttribute( "draw:marker-end-length", ss);
    }   */

    if( drawObject->hasProperty( "draw:marker-start-width" ) )
    {      
      double strokeWidth = drawObject->getDoubleProperty("svg:stroke-width" );
      double arrowWidth = (drawObject->getDoubleProperty("draw:marker-start-width") * strokeWidth);
      xmlWriter->addAttribute( "draw:marker-start-width",QString("%1cm").arg( arrowWidth ) );
    }

    if( drawObject->hasProperty( "draw:marker-end-width" ) )
    {
      double strokeWidth = drawObject->getDoubleProperty("svg:stroke-width" );
      double arrowWidth = (drawObject->getDoubleProperty("draw:marker-end-width") * strokeWidth);
      xmlWriter->addAttribute( "draw:marker-end-width",QString("%1cm").arg( arrowWidth ) );
    }

    
  
    if( drawObject->hasProperty( "draw:fill" ) )
    { 
      std::string s = drawObject->getStrProperty("draw:fill");
      QString ss( s.c_str() );
      xmlWriter->addAttribute( "draw:fill", ss);
    }

    

    if( drawObject->hasProperty( "draw:fill-color" ) )
    {
      Color fillColor = drawObject->getColorProperty("draw:fill-color" );
      xmlWriter->addAttribute( "draw:fill-color", hexname( fillColor ) );
     
    }  else xmlWriter->addAttribute( "draw:fill-color", "#99ccff" );
            
#if 0
    if( drawObject->hasProperty( "draw:shadow-color" ) )
    { 
      xmlWriter->addAttribute( "draw:shadow", "visible" );
      Color shadowColor = drawObject->getColorProperty("draw:shadow-color" );
      xmlWriter->addAttribute( "draw:shadow-color", hexname( shadowColor ) );
    } 
    else  xmlWriter->addAttribute( "draw:shadow", "hidden" );
#endif    
    
    if( drawObject->hasProperty( "draw:shadow-opacity" ) )
    {      
      double opacity = drawObject->getDoubleProperty("draw:shadow-opacity") ;
      xmlWriter->addAttribute( "draw:shadow-opacity",QString("%1%").arg( opacity ) );
    }

    if( drawObject->hasProperty( "draw:shadow-offset-x" ) )
    {      
      double offset = drawObject->getDoubleProperty("draw:shadow-offset-x") ;
      xmlWriter->addAttribute( "draw:shadow-offset-x",QString("%1cm").arg( offset ) );
    }

    if( drawObject->hasProperty( "draw:shadow-offset-y" ) )
    {      
      double offset = drawObject->getDoubleProperty("draw:shadow-offset-y"); 
      xmlWriter->addAttribute( "draw:shadow-offset-y",QString("%1cm").arg( offset ) );
    }
    
   
    xmlWriter->endElement();

  xmlWriter->endElement();
}

