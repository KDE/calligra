/***************************************************************************
             mreportobject.cpp  -  Kugar report object baseclass
             -------------------
   begin     : Wed Aug 11 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2004 Alexander Dymo
   email     : cloudtemple@mksat.net
***************************************************************************/

#include "mreportobject.h"

namespace Kugar
{

/** Constructor */
MReportObject::MReportObject() : QObject()
{
    // Set the object's default geometry
    xpos = 0;
    ypos = 0;
    width = 40;
    height = 23;

    // Set the object's default colors
    backgroundColor.setRgb( 255, 255, 255 );
    foregroundColor.setRgb( 0, 0, 0 );

    // Set the object's default border attributes
    borderColor.setRgb( 0, 0, 0 );
    borderWidth = 1;
    borderStyle = MReportObject::SolidLine;

    drawLeft = true;
    drawRight = true;
    drawTop = true;
    drawBottom = true;
}

/** Copy constructor */
MReportObject::MReportObject( const MReportObject& mReportObject )  /*: QObject((QObject &) mReportObject)*/
{
    copy( &mReportObject );
}

/** Assignment operator */
MReportObject MReportObject::operator=( const MReportObject& mReportObject )
{
    if ( &mReportObject == this )
        return * this;

    // Copy the derived class's data
    copy( &mReportObject );

    // Copy the base class's data
    //((QObject &) *this) = mReportObject;

    return *this;
}

/** Destructor */
MReportObject::~MReportObject()
{}

/** Draws the object to the specified painter & x/y offsets */
void MReportObject::draw( QPainter* p, int xoffset, int yoffset )
{
    drawBase( p, xoffset, yoffset );
}

/** Draws the base object to the specified painter & x/y offsets */
void MReportObject::drawBase( QPainter* p, int xoffset, int yoffset )
{
    QBrush bgBrush( backgroundColor );
    QPen borderPen( borderColor, borderWidth, ( QPen::PenStyle ) borderStyle );

    // Set the offsets
    int xcalc = xpos + xoffset;
    int ycalc = ypos + yoffset;

    // Set background in specified color
    p->setBrush( bgBrush );
    p->setPen( Qt::NoPen );
    p->drawRect( xcalc, ycalc, width, height );

    // Set border
    if ( borderStyle != 0 )
    {
        p->setPen( borderPen );
        if ( drawLeft )
            p->drawLine( xcalc, ycalc, xcalc, ycalc + height );
        if ( drawRight )
            p->drawLine( xcalc + width, ycalc, xcalc + width, ycalc + height );
        if ( drawTop )
            p->drawLine( xcalc, ycalc, xcalc + width, ycalc );
        if ( drawBottom )
            p->drawLine( xcalc, ycalc + height, xcalc + width, ycalc + height );
        //    p->drawRect(xcalc, ycalc, width, height);
    }
    else
    {
        p->setPen( QPen( QColor( 255, 255, 255 ), 1, QPen::SolidLine ) );
        p->drawRect( xcalc, ycalc, width, height );
    }
}

/** Set the object's position and size */
void MReportObject::setGeometry( int x, int y, int w, int h )
{
    xpos = x;
    ypos = y;
    width = w;
    height = h;
}

/** Set the object's position */
void MReportObject::move( int x, int y )
{
    xpos = x;
    ypos = y;
}

/** Gets the object's x position */
int MReportObject::getX()
{
    return xpos;
}

/** Gets the object's y position */
int MReportObject::getY()
{
    return ypos;
}

/** Sets the object's background color */
void MReportObject::setBackgroundColor( int r, int g, int b )
{
    backgroundColor.setRgb( r, g, b );
}

/** Sets the object's foreground color */
void MReportObject::setForegroundColor( int r, int g, int b )
{
    foregroundColor.setRgb( r, g, b );
}

/** Sets the object's border color */
void MReportObject::setBorderColor( int r, int g, int b )
{
    borderColor.setRgb( r, g, b );
}

/** Sets the object's border width */
void MReportObject::setBorderWidth( int width )
{
    borderWidth = width;
}

/** Sets the object's border style */
void MReportObject::setBorderStyle( int style )
{
    borderStyle = style;
}

/** Copies member data from one object to another.
    Used by the copy constructor and assignment operator */
void MReportObject::copy( const MReportObject* mReportObject )
{
    // Copy the object's geometry
    xpos = mReportObject->xpos;
    ypos = mReportObject->ypos;
    width = mReportObject->width;
    height = mReportObject->height;

    // Copy the colors
    backgroundColor = mReportObject->backgroundColor;
    foregroundColor = mReportObject->foregroundColor;

    // Copy the border attributes
    borderColor = mReportObject->borderColor;
    borderWidth = mReportObject->borderWidth;
    borderStyle = mReportObject->borderStyle;
}

bool MReportObject::getDrawLeft( )
{
    return drawLeft;
}

bool MReportObject::getDrawRight( )
{
    return drawRight;
}

bool MReportObject::getDrawTop( )
{
    return drawTop;
}

bool MReportObject::getDrawBottom( )
{
    return drawBottom;
}

void MReportObject::setDrawLeft( bool d )
{
    drawLeft = d;
}

void MReportObject::setDrawRight( bool d )
{
    drawRight = d;
}

void MReportObject::setDrawTop( bool d )
{
    drawTop = d;
}

void MReportObject::setDrawBottom( bool d )
{
    drawBottom = d;
}

}
