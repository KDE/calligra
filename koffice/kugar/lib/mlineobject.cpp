/***************************************************************************
             mlineobject.cpp  -  Kugar report line object
             -------------------
   begin     : Wed Feb 23 2000
   copyright : (C) 2000 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mlineobject.h"

namespace Kugar
{

/** Constructor */
MLineObject::MLineObject() : QObject()
{
    // Set the object's default geometry
    xpos1 = 0;
    ypos1 = 0;
    xpos2 = 0;
    ypos2 = 0;

    // Set the object's default attributes
    penWidth = 1;
    penColor.setRgb( 0, 0, 0 );
    penStyle = MLineObject::SolidLine;
}

/** Copy constructor */
MLineObject::MLineObject( const MLineObject& mLineObject )  /*: QObject((QObject &) mLineObject)*/
{
    copy( &mLineObject );
}

/** Assignment operator */
MLineObject MLineObject::operator=( const MLineObject& mLineObject )
{
    if ( &mLineObject == this )
        return * this;

    // Copy the derived class's data
    copy( &mLineObject );

    // Copy the base class's data
    //((QObject &) *this) = mLineObject;

    return *this;
}

/** Destructor */
MLineObject::~MLineObject()
{}

/** Sets the start and end points for the line */
void MLineObject::setLine( int xStart, int yStart, int xEnd, int yEnd )
{
    xpos1 = xStart;
    ypos1 = yStart;
    xpos2 = xEnd;
    ypos2 = yEnd;
}

/** Sets the object's color */
void MLineObject::setColor( int r, int g, int b )
{
    penColor.setRgb( r, g, b );
}

/** Sets the object's style */
void MLineObject::setStyle( int style )
{
    penStyle = style;
}

/** Sets the object's width */
void MLineObject::setWidth( int width )
{
    penWidth = width;
}

/** Draws the object to the specified painter & x/y offsets */
void MLineObject::draw( QPainter* p, int xoffset, int yoffset )
{
    drawBase( p, xoffset, yoffset );
}

/** Draws the base object to the specified painter & x/y offsets */
void MLineObject::drawBase( QPainter* p, int xoffset, int yoffset )
{
    QPen linePen( penColor, penWidth, ( QPen::PenStyle ) penStyle );

    // Set the offsets
    int xcalc1 = xpos1 + xoffset;
    int ycalc1 = ypos1 + yoffset;
    int xcalc2 = xpos2 + xoffset;
    int ycalc2 = ypos2 + yoffset;

    // Draw the line
    p->setPen( linePen );
    p->drawLine( xcalc1, ycalc1, xcalc2, ycalc2 );
}

/** Copies member data from one object to another.
    Used by the copy constructor and assignment operator */
void MLineObject::copy( const MLineObject* mLineObject )
{
    // Copy the object's geometry
    xpos1 = mLineObject->xpos1;
    ypos1 = mLineObject->ypos1;
    xpos2 = mLineObject->xpos2;
    ypos2 = mLineObject->ypos2;

    // copy the object's attributes
    penWidth = mLineObject->penWidth;
    penColor = mLineObject->penColor;
    penStyle = mLineObject->penStyle;
}

}
