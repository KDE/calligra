/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
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

#include <kdebug.h>
#include <shapes/vellipse.h>
#include <shapes/vrectangle.h>
#include <shapes/vpolygon.h>
#include <core/vstroke.h>
#include <core/vfill.h>

#include "wmfimportparser.h"
//Added by qt3to4:
#include <Q3PointArray>
#include <Q3ValueList>
#include <Q3PtrList>

/*
bug : see motar.wmf
*/

WMFImportParser::WMFImportParser() : KoWmfRead() {
}


bool WMFImportParser::play( VDocument& doc )
{
    mDoc = &doc;
    mScaleX = mScaleY = 1;
    
    // Play the wmf file
    return KoWmfRead::play( );
}


//-----------------------------------------------------------------------------
// Virtual Painter

bool WMFImportParser::begin() { 
    QRect bounding = boundingRect();
    
    mBackgroundMode = Qt::TransparentMode;
    mCurrentOrg.setX( bounding.left() );
    mCurrentOrg.setY( bounding.top() );
    
    if ( isStandard() ) {
        mDoc->setUnit( KoUnit::U_PT );
        mDoc->setWidth( bounding.width() );  
        mDoc->setHeight( bounding.height() ); 
    }
    else {
        // Placeable Wmf store the boundingRect() in pixel and the default DPI
        // The placeable format doesn't have information on which Unit to use
        // so we choose millimeters by default
        mDoc->setUnit( KoUnit::U_MM );
        mDoc->setWidth( INCH_TO_POINT( (double)bounding.width() / defaultDpi() ) );  
        mDoc->setHeight( INCH_TO_POINT( (double)bounding.height() / defaultDpi() ) ); 
    }
    if ( (bounding.width() != 0) && (bounding.height() != 0) ) {
        mScaleX = mDoc->width() / (double)bounding.width();
        mScaleY = mDoc->height() / (double)bounding.height();
    }    
    return true;
}


bool WMFImportParser::end() {
    return true;
}


void WMFImportParser::save() {
}


void WMFImportParser::restore() {
}


void WMFImportParser::setFont( const QFont & ) {
}


void WMFImportParser::setPen( const QPen &pen ) {
    mPen = pen;
}


const QPen &WMFImportParser::pen() const {
    return mPen;
}


void WMFImportParser::setBrush( const QBrush &brush ) {
    mBrush = brush;
}


void WMFImportParser::setBackgroundColor( const QColor &c ) {
    mBackgroundColor = c;
}


void WMFImportParser::setBackgroundMode( Qt::BGMode mode ) {
    mBackgroundMode = mode;
}


void WMFImportParser::setRasterOp( Qt::RasterOp  ) {
}


void WMFImportParser::setWindowOrg( int left, int top ) {
    mCurrentOrg.setX( left );
    mCurrentOrg.setY( top );
}


void WMFImportParser::setWindowExt( int width, int height ) {    
    // the wmf file can change width/height during the drawing
    if ( (width != 0) && (height != 0) ) {  
        mScaleX = mDoc->width() / (double)width;
        mScaleY = mDoc->height() / (double)height;
    }
}


void WMFImportParser::setMatrix( const QMatrix &, bool  ) {
}


void WMFImportParser::setClipRegion( const QRegion & ) {
}


QRegion WMFImportParser::clipRegion() {
    return mClippingRegion;
}


void WMFImportParser::moveTo( int left, int top ) {
    mCurrentPoint.setX( left );
    mCurrentPoint.setY( top );
}


void WMFImportParser::lineTo( int left, int top ) {    
    VPath *line = new VPath( mDoc );
    line->moveTo( KoPoint( coordX(mCurrentPoint.x()), coordY(mCurrentPoint.y()) ) );
    line->lineTo( KoPoint( coordX(left), coordY(top) ) );
    appendPen( *line );
    
    mDoc->append( line );
    mCurrentPoint.setX( left );
    mCurrentPoint.setY( top );
}


void WMFImportParser::drawRect( int left, int top, int width, int height ) {
    VRectangle *rectangle;
    
    rectangle = new VRectangle( mDoc, KoPoint( coordX(left), coordY(top) ), scaleW(width), scaleH(height), 0 );
    appendPen( *rectangle );
    appendBrush( *rectangle );

    mDoc->append( rectangle );
}


void WMFImportParser::drawRoundRect( int left, int top, int width, int height, int roudw, int  ) {
    VRectangle *rectangle;
    
    // TODO : round rectangle
    rectangle = new VRectangle( mDoc, KoPoint( coordX(left), coordY(top) ), scaleW(width), scaleH(height), roudw );
    appendPen( *rectangle );
    appendBrush( *rectangle );

    mDoc->append( rectangle );
}


void WMFImportParser::drawEllipse( int left, int top, int width, int height ) {
    VEllipse *ellipse;
     
    ellipse = new VEllipse( mDoc, KoPoint( coordX(left), coordY(top+height) ), scaleW(width), scaleH(height) );
    appendPen( *ellipse );
    appendBrush( *ellipse );

    mDoc->append( ellipse );
}


void WMFImportParser::drawArc( int x, int y, int w, int h, int aStart, int aLen ) {
    double start = (aStart * 180) / 2880.0;
    double end = (aLen * 180) / 2880.0;
    end += start;
    VEllipse::VEllipseType type = VEllipse::arc;
    
    VEllipse *arc = new VEllipse( mDoc, KoPoint( coordX(x), coordY(y+h) ), scaleW(w), scaleH(h), type, start, end );    
    appendPen( *arc );
    
    mDoc->append( arc );
}


void WMFImportParser::drawPie( int x, int y, int w, int h, int aStart, int aLen ) {
    double start = (aStart * 180) / 2880.0;
    double end = (aLen * 180) / 2880.0;
    end += start;
    VEllipse::VEllipseType type = VEllipse::cut;
    
    VEllipse *arc = new VEllipse( mDoc, KoPoint( coordX(x), coordY(y+h) ), scaleW(w), scaleH(h), type, start, end );    
    appendPen( *arc );
    appendBrush( *arc );
    
    mDoc->append( arc );
}


void WMFImportParser::drawChord( int x, int y, int w, int h, int aStart, int aLen ) {
    double start = (aStart * 180) / 2880.0;
    double end = (aLen * 180) / 2880.0;
    end += start;
    VEllipse::VEllipseType type = VEllipse::section;
    
    VEllipse *arc = new VEllipse( mDoc, KoPoint( coordX(x), coordY(y+h) ), scaleW(w), scaleH(h), type, start, end );    
    appendPen( *arc );
    appendBrush( *arc );
    
    mDoc->append( arc );
}


void WMFImportParser::drawPolyline( const Q3PointArray &pa ) {
    VPath *polyline = new VPath( mDoc );
    appendPen( *polyline );
    appendPoints( *polyline, pa );
    
    mDoc->append( polyline );
}


void WMFImportParser::drawPolygon( const Q3PointArray &pa, bool ) {
    VPath *polygon = new VPath( mDoc );
    appendPen( *polygon );
    appendBrush( *polygon );
    appendPoints( *polygon, pa );
    
    polygon->close();
    mDoc->append( polygon );
}


void WMFImportParser::drawPolyPolygon( Q3PtrList<Q3PointArray>& listPa, bool ) {
    VPath *path = new VPath( mDoc );
    
    if ( listPa.count() > 0 ) {
        appendPen( *path );
        appendBrush( *path );
        appendPoints( *path, *listPa.first() );
        path->close();

        while ( listPa.next() ) {
            VPath *newPath = new VPath( mDoc );
            appendPoints( *newPath, *listPa.current() );
            newPath->close();
            path->combine( *newPath ); 
        }

        mDoc->append( path );
    }
}


void WMFImportParser::drawImage( int , int , const QImage &, int , int , int , int  ) {}


void WMFImportParser::drawText( int , int , int , int , int , const QString& , double ) {}


//-----------------------------------------------------------------------------
// Utilities

void WMFImportParser::appendPen( VObject& obj )
{
    VStroke stroke( mDoc );
    stroke.setLineCap( VStroke::capRound );
    
    if ( mPen.style() == Qt::NoPen ) {
        stroke.setType( VStroke::none );
    }
    else {
        Q3ValueList<float> dashes;
        stroke.setType( VStroke::solid );
        switch ( mPen.style() ) {
            case Qt::DashLine :
            stroke.dashPattern().setArray( dashes << MM_TO_POINT(3) << MM_TO_POINT(2) );
            break;
            case Qt::DotLine :
            stroke.dashPattern().setArray( dashes << MM_TO_POINT(1) << MM_TO_POINT(1) );
            break;
            case Qt::DashDotLine :
            stroke.dashPattern().setArray( dashes << MM_TO_POINT(3) << MM_TO_POINT(1) << MM_TO_POINT(1) << MM_TO_POINT(1) );
            break;
            case Qt::DashDotDotLine :
            stroke.dashPattern().setArray( dashes << MM_TO_POINT(3) << MM_TO_POINT(1) << MM_TO_POINT(1) << MM_TO_POINT(1) << MM_TO_POINT(1) << MM_TO_POINT(1) );
            break;
            default:
            break;
        }
    }
    stroke.setColor( mPen.color() );
    double width = mPen.width() * mScaleX;
    stroke.setLineWidth( ((width < 0.99) ? 1 : width) );
    obj.setStroke( stroke );    
}


void WMFImportParser::appendBrush( VObject& obj )
{
    VFill fill( mBackgroundColor );
    fill.setColor( mBrush.color() );
        
    switch ( mBrush.style() ) {
        case Qt::NoBrush :
        fill.setType( VFill::none );
        break;
        case Qt::SolidPattern :
        fill.setType( VFill::solid );
        break;
        case Qt::TexturePattern :
        // TODO: bitmap pattern brush
        fill.setType( VFill::solid );
        //fill.pattern().
        break;
        default :
        // TODO: pattern brush
        if ( mBackgroundMode == Qt::OpaqueMode ) {
            fill.setColor( mBackgroundColor );
            fill.setType( VFill::solid );
        }
        else {
            fill.setType( VFill::none );
        }
        break;
    }
    obj.setFill( fill );
}


void WMFImportParser::appendPoints(VPath &path, const Q3PointArray& pa)
{
    // list of point array
    if ( pa.size() > 0 ) {
        path.moveTo( KoPoint( coordX(pa.point(0).x()), coordY(pa.point(0).y()) ) );
    }
    for ( uint i=1 ; i < pa.size() ; i++ ) {
        path.lineTo( KoPoint( coordX(pa.point(i).x()), coordY(pa.point(i).y()) ) );
    }
}

