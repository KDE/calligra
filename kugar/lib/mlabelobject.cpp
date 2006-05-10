/***************************************************************************
             mlabelobject.cpp  -  Kugar report label object
             -------------------
   begin     : Wed Aug 11 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2002 Alexander Dymo
   email     : cloudtemple@mksat.net
***************************************************************************/

//#include <kglobalsettings.h>
#include <QFont>

#include "mlabelobject.h"

namespace Kugar
{

/** Constructor */
MLabelObject::MLabelObject() : MReportObject(), xMargin( 0 ), yMargin( 0 )
{
    // Set the default label text
    text = "";

    // Set the default font
    QFont defaultFont;
    fontFamily = defaultFont.family();
    fontSize = 10;
    fontWeight = MLabelObject::Normal;
    fontItalic = false;

    // Set the default alignment
    hAlignment = MLabelObject::Left;
    vAlignment = MLabelObject::Top;
    wordWrap = false;
}

/** Copy constructor */
MLabelObject::MLabelObject( const MLabelObject& mLabelObject ) :
        MReportObject( ( MReportObject & ) mLabelObject ), xMargin( 0 ), yMargin( 0 )
{
    copy( &mLabelObject );
}

/** Assignment operator */
MLabelObject MLabelObject::operator=( const MLabelObject& mLabelObject )
{
    if ( &mLabelObject == this )
        return * this;

    // Copy the derived class's data
    copy( &mLabelObject );

    // Copy the base class's data
    ( ( MReportObject & ) * this ) = mLabelObject;

    return *this;
}

/** Destructor */
MLabelObject::~MLabelObject()
{}

/** Sets the label's text string */
void MLabelObject::setText( const QString txt )
{
    text = txt;
}

/** Sets the label's text font */
void MLabelObject::setFont( const QString family, int size, int weight, bool italic )
{
    fontFamily = family;
    fontSize = size;
    fontWeight = weight;
    fontItalic = italic;
}

/** Sets the label's horizontal alignment */
void MLabelObject::setHorizontalAlignment( int a )
{
    hAlignment = a;
}

/** Sets the label's vertical alignment */
void MLabelObject::setVerticalAlignment( int a )
{
    vAlignment = a;
}

/** Sets the label's word wrap flag */
void MLabelObject::setWordWrap( bool state )
{
    wordWrap = state;
}

/** Draws the label using the specificed painter & x/y-offsets */
void MLabelObject::draw( QPainter* p, int xoffset, int yoffset )
{
    QFont font( fontFamily, fontSize, fontWeight, fontItalic );
    QPen textPen( foregroundColor, 0, Qt::NoPen );

    int tf = 0;

    // Set the offsets
    int xcalc = xpos + xoffset;
    int ycalc = ypos + yoffset;

    // Draw the base
    drawBase( p, xoffset, yoffset );

    // Set the font
    p->setFont( font );
    QFontMetrics fm = p->fontMetrics();

    // Set the text alignment flags

    // Horizontal
    switch ( hAlignment )
    {
    case MLabelObject::Left:
        tf = Qt::AlignLeft;
        break;
    case MLabelObject::Center:
        tf = Qt::AlignHCenter;
        break;
    case MLabelObject::Right:
        tf = Qt::AlignRight;
    }

    // Vertical
    switch ( vAlignment )
    {
    case MLabelObject::Top:
        tf = tf | Qt::AlignTop;
        break;
    case MLabelObject::Bottom:
        tf = tf | Qt::AlignBottom;
        break;
    case MLabelObject::Middle:
        tf = tf | Qt::AlignVCenter;
    }

    // Word wrap
    if ( wordWrap )
        tf = tf | Qt::TextWordWrap;

    // Draw the text
    p->setPen( textPen );
    p->drawText( xcalc + xMargin, ycalc + yMargin,
                 width - xMargin, height - yMargin,
                 tf, text );
}

/** Copies member data from one object to another.
      Used by the copy constructor and assignment operator */
void MLabelObject::copy( const MLabelObject* mLabelObject )
{
    // Copy the label's text
    text = mLabelObject->text;

    // Copy the label's font data
    fontFamily = mLabelObject->fontFamily;
    fontSize = mLabelObject->fontSize;
    fontWeight = mLabelObject->fontWeight;
    fontItalic = mLabelObject->fontItalic;

    // Copy the label's alignment data
    vAlignment = mLabelObject->vAlignment;
    hAlignment = mLabelObject->hAlignment;
    wordWrap = mLabelObject->wordWrap;
}

}
