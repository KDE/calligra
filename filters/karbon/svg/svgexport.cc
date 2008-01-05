/* This file is part of the KDE project
   Copyright (C) 2002 Lars Siebold <khandha5@gmx.net>
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2002,2005-2006 David Faure <faure@kde.org>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
   Copyright (C) 2005 Boudewijn Rempt <boud@valdyas.org>
   Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005,2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006 Gábor Lehel <illissius@gmail.com>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Christian Mueller <cmueller@gmx.de>
   Copyright (C) 2006 Ariya Hidayat <ariya@kde.org>

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

#include "svgexport.h"

#include <KarbonDocument.h>
#include <karbon_part.h>

#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoShapeLayer.h>
#include <KoPathShape.h>
#include <KoLineBorder.h>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtGui/QLinearGradient>
#include <QtGui/QRadialGradient>


static void printIndentation( QTextStream *stream, unsigned int indent )
{
    static const QString INDENT("  ");
    for( unsigned int i = 0; i < indent;++i)
        *stream << INDENT;
}

typedef KGenericFactory<SvgExport> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonsvgexport, SvgExportFactory( "kofficefilters" ) )

SvgExport::SvgExport( QObject*parent, const QStringList& )
    : KoFilter(parent), m_indent( 0 ), m_indent2( 0 )
{
}

KoFilter::ConversionStatus SvgExport::convert( const QByteArray& from, const QByteArray& to )
{
    if ( to != "image/svg+xml" || from != "application/vnd.oasis.opendocument.graphics" )
        return KoFilter::NotImplemented;

    KoDocument * document = m_chain->inputDocument();
    if( ! document )
        return KoFilter::ParsingError;

    KarbonPart * karbonPart = dynamic_cast<KarbonPart*>( document );
    if( ! karbonPart )
        return KoFilter::WrongFormat;

    QFile fileOut( m_chain->outputFile() );
    if( !fileOut.open( QIODevice::WriteOnly ) )
        return KoFilter::StupidError;

    m_stream = new QTextStream( &fileOut );
    QString body;
    m_body = new QTextStream( &body, QIODevice::ReadWrite );
    QString defs;
    m_defs = new QTextStream( &defs, QIODevice::ReadWrite );

    saveDocument( karbonPart->document() );

    *m_stream << defs;
    *m_stream << body;

    fileOut.close();

    delete m_stream;
    delete m_defs;
    delete m_body;

    return KoFilter::OK;
}

void SvgExport::saveDocument( KarbonDocument& document )
{
    // get the bounding box of the page
    QSizeF pageSize = document.pageSize();

    // standard header:
    *m_defs <<
        "<?xml version=\"1.0\" standalone=\"no\"?>\n" <<
        "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " <<
        "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">"
    << endl;

    // add some PR.  one line is more than enough.  
    *m_defs <<
        "<!-- Created using Karbon14, part of koffice: http://www.koffice.org/karbon -->" << endl;

    *m_defs <<
        "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"" <<
        pageSize.width() << "px\" height=\"" << pageSize.height() << "px\">" << endl;
    printIndentation( m_defs, ++m_indent2 );
    *m_defs << "<defs>" << endl;

    m_indent++;
    m_indent2++;

    // export layers:
    foreach( KoShapeLayer * layer, document.layers() )
        saveLayer( layer );

    // end tag:
    printIndentation( m_defs, --m_indent2 );
    *m_defs << "</defs>" << endl;
    *m_body << "</svg>" << endl;
}

void SvgExport::saveLayer( KoShapeLayer * layer )
{
    printIndentation( m_body, m_indent++ );
    *m_body << "<g" << getID( layer ) << ">" << endl;

    foreach( KoShape * shape, layer->iterator() )
    {
        KoShapeContainer * container = dynamic_cast<KoShapeContainer*>( shape );
        if( container )
            saveGroup( container );
        else
            saveShape( shape );
    }

    printIndentation( m_body, --m_indent );
    *m_body << "</g>" << endl;
}

void SvgExport::saveGroup( KoShapeContainer * group )
{
    printIndentation( m_body, m_indent++ );
    *m_body << "<g" << getID( group ) << ">" << endl;

    foreach( KoShape * shape, group->iterator() )
    {
        KoShapeContainer * container = dynamic_cast<KoShapeContainer*>( shape );
        if( container )
            saveGroup( container );
        else
            saveShape( shape );
    }

    printIndentation( m_body, --m_indent );
    *m_body << "</g>" << endl;
}

void SvgExport::saveShape( KoShape * shape )
{
    KoPathShape * path = dynamic_cast<KoPathShape*>( shape );
    if( path )
    {
        // TODO differentiate between rects, circles and paths
        savePath( path );
    }
    else
    {
        // TODO export text and images
    }
}

void SvgExport::savePath( KoPathShape * path )
{
    printIndentation( m_body, m_indent );
    *m_body << "<path" << getID( path );

    getFill( path->background(), m_body );
    getStroke( path->border(), m_body );

    *m_body << " d=\"" << path->toString( path->absoluteTransformation(0) ) << "\" ";

    if( path->fillRule() == Qt::OddEvenFill )
        *m_body << " fill-rule=\"evenodd\"";
    else
        *m_body << " fill-rule=\"nonzero\"";

    *m_body << " />" << endl;
}

QString SvgExport::getID( KoShape *obj )
{
    if( obj && !obj->name().isEmpty() )
        return QString( " id=\"%1\"" ).arg( obj->name() );
    return QString();
}

static QString createUID()
{
    static unsigned int nr = 0;

    return "defitem" + QString().setNum( nr++ );
}

void SvgExport::getColorStops( const QGradientStops & colorStops )
{
    m_indent2++;
    foreach( QGradientStop stop, colorStops )
    {
        printIndentation( m_defs, m_indent2 );
        *m_defs << "<stop stop-color=\"";
        getHexColor( m_defs, stop.second );
        *m_defs << "\" offset=\"" << QString().setNum( stop.first );
        *m_defs << "\" stop-opacity=\"" << stop.second.alphaF() << "\"" << " />" << endl;
    }
    m_indent2--;
}

void SvgExport::getGradient( const QGradient * grad )
{
    const QString spreadMethod[3] = {
        QString("spreadMethod=\"pad\" "),
        QString("spreadMethod=\"reflect\" "),
        QString("spreadMethod=\"repeat\" ")
    };

    QString uid = createUID();
    if( grad->type() == QGradient::LinearGradient )
    {
        const QLinearGradient * g = static_cast<const QLinearGradient*>( grad );
        // do linear grad
        printIndentation( m_defs, m_indent2 );
        *m_defs << "<linearGradient id=\"" << uid << "\" ";
        *m_defs << "gradientUnits=\"userSpaceOnUse\" ";
        *m_defs << "x1=\"" << g->start().x() << "\" ";
        *m_defs << "y1=\"" << g->start().y() << "\" ";
        *m_defs << "x2=\"" << g->finalStop().x() << "\" ";
        *m_defs << "y2=\"" << g->finalStop().y() << "\" ";
        *m_defs << spreadMethod[g->spread()];
        *m_defs << ">" << endl;

        // color stops
        getColorStops( grad->stops() );

        printIndentation( m_defs, m_indent2 );
        *m_defs << "</linearGradient>" << endl;
        *m_body << "url(#" << uid << ")";
    }
    else if( grad->type() == QGradient::RadialGradient )
    {
        const QRadialGradient * g = static_cast<const QRadialGradient*>( grad );
        // do radial grad
        printIndentation( m_defs, m_indent2 );
        *m_defs << "<radialGradient id=\"" << uid << "\" ";
        *m_defs << "gradientUnits=\"userSpaceOnUse\" ";
        *m_defs << "cx=\"" << g->center().x() << "\" ";
        *m_defs << "cy=\"" << g->center().y() << "\" ";
        *m_defs << "fx=\"" << g->focalPoint().x() << "\" ";
        *m_defs << "fy=\"" << g->focalPoint().y() << "\" ";
        *m_defs << "r=\"" << QString().setNum( g->radius() ) << "\" ";
        *m_defs << spreadMethod[g->spread()];
        *m_defs << ">" << endl;

        // color stops
        getColorStops( grad->stops() );

        printIndentation( m_defs, m_indent2 );
        *m_defs << "</radialGradient>" << endl;
        *m_body << "url(#" << uid << ")";
    }
    else if( grad->type() == QGradient::ConicalGradient )
    {
        //const QConicalGradient * g = static_cast<const QConicalGradient*>( grad );
        // fake conical grad as radial.
        // fugly but better than data loss.
        /*
        printIndentation( m_defs, m_indent2 );
        *m_defs << "<radialGradient id=\"" << uid << "\" ";
        *m_defs << "gradientUnits=\"userSpaceOnUse\" ";
        *m_defs << "cx=\"" << g->center().x() << "\" ";
        *m_defs << "cy=\"" << g->center().y() << "\" ";
        *m_defs << "fx=\"" << grad.focalPoint().x() << "\" ";
        *m_defs << "fy=\"" << grad.focalPoint().y() << "\" ";
        double r = sqrt( pow( grad.vector().x() - grad.origin().x(), 2 ) + pow( grad.vector().y() - grad.origin().y(), 2 ) );
        *m_defs << "r=\"" << QString().setNum( r ) << "\" ";
        *m_defs << spreadMethod[g->spread()];
        *m_defs << ">" << endl;

        // color stops
        getColorStops( grad->stops() );

        printIndentation( m_defs, m_indent2 );
        *m_defs << "</radialGradient>" << endl;
        *m_body << "url(#" << uid << ")";
        */
    }
}

// better than nothing
void SvgExport::getPattern( const QPixmap & )
{
    QString uid = createUID();
    printIndentation( m_defs, m_indent2 );
    *m_defs << "<pattern id=\"" << uid << "\" ";
    *m_defs << "width=\"" << "\" ";
    *m_defs << "height=\"" << "\" ";
    *m_defs << "patternUnits=\"userSpaceOnUse\" ";
    *m_defs << "patternContentUnits=\"userSpaceOnUse\" "; 
    *m_defs << " />" << endl;
    // TODO: insert hard work here ;)
    printIndentation( m_defs, m_indent2 );
    *m_defs << "</pattern>" << endl;
    *m_body << "url(#" << uid << ")";
}

void SvgExport::getFill( const QBrush& fill, QTextStream *stream )
{
    *stream << " fill=\"";

    switch( fill.style() )
    {
        case Qt::NoBrush:
            *stream << "none";
            break;
        case Qt::LinearGradientPattern:
        case Qt::RadialGradientPattern:
        case Qt::ConicalGradientPattern:
            getGradient( fill.gradient() );
            break;
        case Qt::TexturePattern:
            getPattern( fill.texture() );
            break;
        case Qt::SolidPattern:
            getHexColor( stream, fill.color() );
            break;
        default:
            break;
    }

    *stream << "\"";

    *stream << " fill-opacity=\"" << fill.color().alphaF() << "\"";
}

void SvgExport::getStroke( const KoShapeBorderModel * stroke, QTextStream *stream )
{
    const KoLineBorder * line = dynamic_cast<const KoLineBorder*>( stroke );
    if( ! line )
        return;

    *stream << " stroke=\"";
    if( line->lineStyle() == Qt::NoPen )
        *stream << "none";
    /*
    else if( line->type() == VStroke::grad )
        getGradient( line->gradient() );
    */
    else
        getHexColor( stream, line->color() );
    *stream << "\"";

    *stream << " stroke-opacity=\"" << line->color().alphaF() << "\"";
    *stream << " stroke-width=\"" << line->lineWidth() << "\"";

    if( line->capStyle() == Qt::FlatCap )
        *stream << " stroke-linecap=\"butt\"";
    else if( line->capStyle() == Qt::RoundCap )
        *stream << " stroke-linecap=\"round\"";
    else if( line->capStyle() == Qt::SquareCap )
        *stream << " stroke-linecap=\"square\"";

    if( line->joinStyle() == Qt::MiterJoin )
    {
        *stream << " stroke-linejoin=\"miter\"";
        *stream << " stroke-miterlimit=\"" << line->miterLimit() << "\"";
    }
    else if( line->joinStyle() == Qt::RoundJoin )
        *stream << " stroke-linejoin=\"round\"";
    else if( line->joinStyle() == Qt::BevelJoin )
            *stream << " stroke-linejoin=\"bevel\"";

    // dash
    if(  line->lineStyle() > Qt::SolidLine )
    {
        //*stream << " stroke-dashoffset=\"" << line->dashPattern().offset() << "\"";
        *stream << " stroke-dasharray=\" ";

        foreach( qreal dash, line->lineDashes() )
        {
            *stream << dash << " ";
        }
        *stream << "\"";
    }
}

void SvgExport::getHexColor( QTextStream *stream, const QColor & color )
{
    // Convert the various color-spaces to hex
    QString Output;

    Output.sprintf( "#%02x%02x%02x", color.red(), color.green(), color.blue() );

    *stream << Output;
}

/*
void SvgExport::visitVText( VText& text )
{
	VPath path( 0L );
	path.combinePath( text.basePath() );

	QString id = createUID();
	writePathToStream( path, " id=\""+ id + "\"", m_defs, m_indent2 );

	printIndentation( m_body, m_indent++ );
	*m_body << "<text" << getID( &text );
	// *m_body << " transform=\"scale(1, -1) translate(0, -" << text.document()->height() << ")\"";
	getFill( *( text.fill() ), m_body );
	getStroke( *( text.stroke() ), m_body );

	*m_body << " font-family=\"" << text.font().family() << "\"";
	*m_body << " font-size=\"" << text.font().pointSize() << "\"";
	if( text.font().bold() )
		*m_body << " font-weight=\"bold\"";
	if( text.font().italic() )
		*m_body << " font-style=\"italic\"";
	if( text.alignment() == VText::Center )
		*m_body << " text-anchor=\"middle\"";
	else if( text.alignment() == VText::Right )
		*m_body << " text-anchor=\"end\"";

	*m_body << ">" << endl;
	
	printIndentation( m_body, m_indent );
	*m_body << "<textPath xlink:href=\"#" << id << "\"";
	if( text.offset() > 0.0 )
		*m_body << " startOffset=\"" << text.offset() * 100.0 << "%\"";	
	*m_body << ">";
	*m_body << text.text();
	*m_body << "</textPath>" << endl;
	printIndentation( m_body, --m_indent );
	*m_body << "</text>" << endl;
}
*/

// horrible but at least something gets exported now
// will need this for patterns
void SvgExport::saveImage( QImage& )
{
    printIndentation( m_body, m_indent );
    *m_body << "<image ";

    *m_body << "x=\"" << "\" ";
    *m_body << "y=\"" << "\" ";
    *m_body << "width=\"" << "\" ";
    *m_body << "height=\"" << "\" ";
    *m_body << "xlink:href=\"" << "\"";
    *m_body << " />" << endl;
}

#include "svgexport.moc"

