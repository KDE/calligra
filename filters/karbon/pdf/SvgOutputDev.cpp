/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgOutputDev.h"
#include <poppler/Object.h>
#include <poppler/GfxState.h>
#include <poppler/Stream.h>
#include <poppler/GfxFont.h>

#include <KDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QSizeF>
#include <QtCore/QBuffer>
#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QImage>

class SvgOutputDev::Private
{
public:
    Private( const QString &fname )
        : svgFile( fname ), defs( 0 ), body( 0 ), state( gTrue )
        , brush(Qt::SolidPattern)
    {}

    ~Private()
    {
        delete defs;
        delete body;
    }

    QFile svgFile;
    QString bodyData;
    QString defsData;
    QTextStream * defs;
    QTextStream * body;
    GBool state;
    QSizeF pageSize;
    QPen pen;
    QBrush brush;
    QMatrix textMatrix;
};

SvgOutputDev::SvgOutputDev( const QString &fileName )
: d( new Private( fileName ) )
{
    if( ! d->svgFile.open( QIODevice::WriteOnly ) )
    {
        d->state = gFalse;
        return;
    }

    d->body = new QTextStream( &d->bodyData, QIODevice::ReadWrite );
    d->defs = new QTextStream( &d->defsData, QIODevice::ReadWrite );
}

SvgOutputDev::~SvgOutputDev()
{
    delete d;
}

GBool SvgOutputDev::isOk()
{
    return d->state;
}

GBool SvgOutputDev::upsideDown()
{
    return gTrue;
}

GBool SvgOutputDev::useDrawChar()
{
    return gFalse;
}

GBool SvgOutputDev::interpretType3Chars()
{
    return gFalse;
}

void SvgOutputDev::startPage(int pageNum, GfxState *state)
{
    kDebug(30516) << "starting page" << pageNum;
    d->pageSize = QSizeF( state->getPageWidth(), state->getPageHeight() );
    kDebug(30516) << "page size =" << d->pageSize;
    if( pageNum == 1 )
        *d->body << "<g>" << endl;
    else
        *d->body << "<g display=\"none\">" << endl;
}

void SvgOutputDev::endPage()
{
    kDebug(30516) << "ending page";
    *d->body << "</g>" << endl;
}

void SvgOutputDev::dumpContent()
{
    kDebug(30516) << "dumping pages";
    //kDebug(30516) << "defs =" << d->defsData;
    //kDebug(30516) << "body =" << d->bodyData;

    QTextStream stream( &d->svgFile );

    stream << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
    stream << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" ";
    stream << "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl;

    // add some PR.  one line is more than enough.  
    stream << "<!-- Created using Karbon14, part of koffice: http://www.koffice.org/karbon -->" << endl;

    stream << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" ";
    stream << "width=\"" << d->pageSize.width() << "px\" height=\"" << d->pageSize.height() << "px\">" << endl;

    stream << "<defs>" << endl;
    stream << d->defsData;
    stream << "</defs>" << endl;

    stream << d->bodyData;
    stream << "</svg>" << endl;

    d->svgFile.close();
}

void SvgOutputDev::stroke( GfxState * state )
{
    //kDebug(30516) << "stroke path";

    QString path = convertPath( state->getPath() );
    *d->body << "<path";
    *d->body << " transform=\"" << convertMatrix( state->getCTM() ) << "\"";
    *d->body << printStroke();
    *d->body << " fill=\"none\"";
    *d->body << " d=\"" << path << "\"";
    *d->body << "/>" << endl;
}

void SvgOutputDev::fill( GfxState * state )
{
    //kDebug(30516) << "fill path winding";

    QString path = convertPath( state->getPath() );
    *d->body << "<path";
    *d->body << " transform=\"" << convertMatrix( state->getCTM() ) << "\"";
    *d->body << printFill();
    *d->body << " fill-rule=\"nonzero\"";
    *d->body << " d=\"" << path << "\"";
    *d->body << "/>" << endl;
}

void SvgOutputDev::eoFill( GfxState *state )
{
    //kDebug(30516) << "fill path even-odd";

    QString path = convertPath( state->getPath() );
    *d->body << "<path";
    *d->body << " transform=\"" << convertMatrix( state->getCTM() ) << "\"";
    *d->body << printFill();
    *d->body << " fill-rule=\"evenodd\"";
    *d->body << " d=\"" << path << "\"";
    *d->body << "/>" << endl;
}

QString SvgOutputDev::convertPath( GfxPath *path )
{
    if( ! path )
        return QString();

    QString output;

    for( int i = 0; i < path->getNumSubpaths(); ++i )
    {
        GfxSubpath * subpath = path->getSubpath( i );
        if( subpath->getNumPoints() > 0 )
        {
            output += QString( "M%1 %2" ).arg( subpath->getX(0) ).arg( subpath->getY(0) );
            int j = 1;
            while( j < subpath->getNumPoints() )
            {
                if( subpath->getCurve( j ) )
                {
                    output += QString( "C%1 %2 %3 %4 %5 %6" )
                    .arg( subpath->getX(j) ).arg( subpath->getY(j) )
                    .arg( subpath->getX(j+1) ).arg( subpath->getY(j+1) )
                    .arg( subpath->getX(j+2) ).arg( subpath->getY(j+2) );
                    j += 3;
                }
                else
                {
                    output += QString( "L%1 %2" ).arg( subpath->getX(j) ).arg( subpath->getY(j) );
                    ++j;
                }
            }
            if( subpath->isClosed() )
            {
                output += QString( "Z" );
            }
        }
    }
    return output;
}

QString SvgOutputDev::convertMatrix( const QMatrix &matrix )
{
    return QString( "matrix(%1 %2 %3 %4 %5 %6)" )
                .arg( matrix.m11() ).arg( matrix.m12() )
                .arg( matrix.m21() ).arg( matrix.m22() )
                .arg( matrix.dx() ) .arg( matrix.dy() );
}

QString SvgOutputDev::convertMatrix( double * matrix )
{
    return QString( "matrix(%1 %2 %3 %4 %5 %6)" )
                .arg( matrix[0] ).arg( matrix[1] )
                .arg( matrix[2] ).arg( matrix[3] )
                .arg( matrix[4] ) .arg( matrix[5]);
}

void SvgOutputDev::updateAll(GfxState *state)
{
    kDebug(30516) << "update complete";

    //updateLineDash(state);
    updateLineJoin(state);
    updateLineCap(state);
    updateLineWidth(state);
    //updateFlatness(state);
    updateMiterLimit(state);
    updateFillColor(state);
    updateStrokeColor(state);
    updateFillOpacity(state);
    updateStrokeOpacity(state);
}

void SvgOutputDev::updateFillColor( GfxState *state )
{
    kDebug(30516) << "update fill color";
    GfxRGB rgb;
    QColor brushColour = d->brush.color();
    state->getFillRGB(&rgb);
    brushColour.setRgbF(colToDbl(rgb.r), colToDbl(rgb.g), colToDbl(rgb.b), brushColour.alphaF());
    d->brush.setColor(brushColour);
}

void SvgOutputDev::updateStrokeColor( GfxState *state )
{
    kDebug(30516) << "update stroke color";
    GfxRGB rgb;
    QColor penColour = d->pen.color();
    state->getStrokeRGB(&rgb);
    penColour.setRgbF(colToDbl(rgb.r), colToDbl(rgb.g), colToDbl(rgb.b), penColour.alphaF());
    d->pen.setColor(penColour);
}

void SvgOutputDev::updateFillOpacity( GfxState *state )
{
    kDebug(30516) << "update fill opacity";
    QColor brushColour = d->brush.color();
    brushColour.setAlphaF(state->getFillOpacity());
    d->brush.setColor(brushColour);
}

void SvgOutputDev::updateStrokeOpacity( GfxState *state )
{
    kDebug(30516) << "update stroke opacity";
    QColor penColour = d->pen.color();
    penColour.setAlphaF(state->getStrokeOpacity());
    d->pen.setColor(penColour);
}

void SvgOutputDev::updateLineJoin( GfxState *state )
{
    switch( state->getLineJoin() )
    {
    case 0:
        d->pen.setJoinStyle(Qt::MiterJoin);
        break;
    case 1:
        d->pen.setJoinStyle(Qt::RoundJoin);
        break;
    case 2:
        d->pen.setJoinStyle(Qt::BevelJoin);
        break;
    }
}

void SvgOutputDev::updateLineCap( GfxState *state )
{
    switch( state->getLineCap() )
    {
    case 0:
        d->pen.setCapStyle(Qt::FlatCap);
        break;
    case 1:
        d->pen.setCapStyle(Qt::RoundCap);
        break;
    case 2:
        d->pen.setCapStyle(Qt::SquareCap);
        break;
    }
}

void SvgOutputDev::updateMiterLimit( GfxState *state )
{
    d->pen.setMiterLimit( state->getMiterLimit() );
}

void SvgOutputDev::updateLineWidth( GfxState *state )
{
    //d->pen.setWidthF( state->getTransformedLineWidth() );
    d->pen.setWidthF( state->getLineWidth() );
}

QString SvgOutputDev::printFill()
{
    QString fill;
    fill += " fill=\"";

    switch( d->brush.style() )
    {
        case Qt::NoBrush:
            fill += "none";
            break;
        case Qt::SolidPattern:
            fill += printHexColor( d->brush.color() );
            break;
        default:
            return QString();
            break;
    }

    fill += "\"";
    fill += QString(" fill-opacity=\"%1\"").arg( d->brush.color().alphaF() );

    return fill;
}

QString SvgOutputDev::printStroke()
{
    QString stroke;
    stroke += " stroke=\"";
    if( d->pen.style() == Qt::NoPen )
        stroke += "none";
    else
        stroke += printHexColor( d->pen.color() );
    stroke += "\"";

    stroke += QString( " stroke-opacity=\"%1\"").arg( d->pen.color().alphaF() );
    stroke += QString( " stroke-width=\"%1\"" ).arg( d->pen.widthF() );

    if( d->pen.capStyle() == Qt::FlatCap )
        stroke += " stroke-linecap=\"butt\"";
    else if( d->pen.capStyle() == Qt::RoundCap )
        stroke += " stroke-linecap=\"round\"";
    else if( d->pen.capStyle() == Qt::SquareCap )
        stroke += " stroke-linecap=\"square\"";

    if( d->pen.joinStyle() == Qt::MiterJoin )
    {
        stroke += " stroke-linejoin=\"miter\"";
        stroke += QString( " stroke-miterlimit=\"%1\"" ).arg( d->pen.miterLimit() );
    }
    else if( d->pen.joinStyle() == Qt::RoundJoin )
        stroke += " stroke-linejoin=\"round\"";
    else if( d->pen.joinStyle() == Qt::BevelJoin )
        stroke += " stroke-linejoin=\"bevel\"";

    // dash
    if(  d->pen.style() > Qt::SolidLine )
    {
        //*stream << " stroke-dashoffset=\"" << line->dashPattern().offset() << "\"";
        stroke += " stroke-dasharray=\" ";

        foreach( qreal dash, d->pen.dashPattern() )
        {
            stroke += dash + ' ';
        }
        stroke += "\"";
    }

    return stroke;
}

QString SvgOutputDev::printHexColor( const QColor & color )
{
    return QString( "#%1%2%3" )
            .arg( color.red(), 2, 16, QChar('0') )
            .arg( color.green(), 2, 16, QChar('0') )
            .arg( color.blue(), 2, 16, QChar('0') );
}

void SvgOutputDev::updateTextMat( GfxState * state )
{
    double * tm = state->getTextMat();
    d->textMatrix.setMatrix( tm[0], tm[1], tm[2], tm[3], tm[4], tm[5] );
    kDebug(30516) << d->textMatrix;
}

void SvgOutputDev::drawString( GfxState * state, GooString * s )
{
    int render = state->getRender();
    // check for invisible text -- this is used by Acrobat Capture
    if( render == 3)
        return;

    // ignore empty strings
    if( s->getLength() == 0 )
        return;

    //kDebug(30516) << "text" << s->getCString() << " length =" << s->getLength() << "(" << strlen(s->getCString()) << ")";
    //if( s->getLength() <= 1 )
    //    return;

    kDebug(30516) << "unicode ?" << s->hasUnicodeMarker();

    GfxFont * font = state->getFont();

    QString str;

    int wMode = font->getWMode();
    char * p = s->getCString();
    int len = s->getLength();
    CharCode code;
    Unicode *u = NULL;
    int uLen;
    double dx, dy, originX, originY;
    while( len > 0 )
    {
        int n = font->getNextChar(p, len, &code, &u, &uLen, &dx, &dy, &originX, &originY);
        p += n;
        len -= n;
        str += QChar( *u );
    }

    kDebug(30516) << "text :" << str;

    str = str.simplified();
    if( str.isEmpty() )
        return;
    if( str.length() == 1 && str.at( 0 ) == QChar('&') )
        str = "&amp;";

    double x = state->getLineX();
    double y = state->getLineY();

    state->textTransform( x, y, &x, &y );
    state->transform( x, y, &x, &y );
    
    *d->body << "<text";
    *d->body << " x=\"" << x << "px\"";
    *d->body << " y=\"" << y << "px\"";

    if( font && font->getFamily() )
        *d->body << " font-family=\"" << QString::fromAscii( font->getFamily()->getCString() ) << "\"";
    *d->body << " font-size=\"" << state->getFontSize() << "px\"";

    // fill
    if( ! (render & 1) )
        *d->body << printFill();
    // stroke
    if( (render & 3) == 1 || (render & 3) == 2 )
        *d->body << printStroke();

    *d->body << ">";
    *d->body << str;
    *d->body << "</text>" << endl;
}

void SvgOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
                int width, int height, GfxImageColorMap *colorMap,
                int *maskColors, GBool inlineImg)
{
    /* TODO: Do we want to cache these? */
    ImageStream * imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(), colorMap->getBits());
    imgStr->reset();

    /* ICCBased color space doesn't do any color correction
    * so check its underlying color space as well */
    int is_identity_transform = colorMap->getColorSpace()->getMode() == csDeviceRGB ||
            colorMap->getColorSpace()->getMode() == csICCBased && 
            ((GfxICCBasedColorSpace*)colorMap->getColorSpace())->getAlt()->getMode() == csDeviceRGB;

    unsigned int *dest = 0;
    unsigned char * buffer = new unsigned char[width * height * 4];

    QImage * image = 0;
    if( maskColors )
    {
        for (int y = 0; y < height; y++)
        {
            dest = (unsigned int *) (buffer + y * 4 * width);
            Guchar * pix = imgStr->getLine();
            colorMap->getRGBLine (pix, dest, width);

            for (int x = 0; x < width; x++)
            {
                for (int i = 0; i < colorMap->getNumPixelComps(); ++i)
                {
                    if (pix[i] < maskColors[2*i] * 255|| pix[i] > maskColors[2*i+1] * 255)
                    {
                            *dest = *dest | 0xff000000;
                            break;
                    }
                }
                pix += colorMap->getNumPixelComps();
                dest++;
            }
        }

        image = new QImage(buffer, width, height, QImage::Format_ARGB32);
    }
    else
    {
        for (int y = 0; y < height; y++)
        {
            dest = (unsigned int *) (buffer + y * 4 * width);
            Guchar * pix = imgStr->getLine();
            colorMap->getRGBLine (pix, dest, width);
        }

        image = new QImage(buffer, width, height, QImage::Format_RGB32);
    }

    if( image == NULL || image->isNull() )
    {
        kDebug(30516) << "Null image";
        delete imgStr;
        delete image;
        return;
    }

    double * ctm = state->getCTM();
    QMatrix m;
    m.setMatrix( ctm[0]/width, ctm[1]/width, -ctm[2]/height, -ctm[3]/height, ctm[2]+ctm[4], ctm[3]+ctm[5] );

    QByteArray ba;
    QBuffer device(&ba);
    device.open(QIODevice::WriteOnly);
    if( image->save( &device,"PNG") )
    {
        *d->body << "<image";
        *d->body << " transform=\"" << convertMatrix( m ) << "\"";
        *d->body << " width=\"" << width << "px\"";
        *d->body << " height=\"" << height <<"px\"";
        *d->body << " xlink:href=\"data:image/png;base64," << ba.toBase64() <<  "\"";
        *d->body << " />" << endl;
    }

    delete image;
    delete buffer;
    delete imgStr;
}
