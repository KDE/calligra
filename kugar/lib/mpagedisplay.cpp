/***************************************************************************
             mpagedisplay.cpp  -  Kugar page display widget
             -------------------
   begin     : Sun Aug 15 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mpagedisplay.h"

namespace Kugar
{

/** Constructor */
MPageDisplay::MPageDisplay( QWidget *parent, const char *name ) : QWidget( parent, name )
{
    buffer.resize( 1, 1 );
}

/** Destructor */
MPageDisplay::~MPageDisplay()
{}

/** Sets the report page image */
void MPageDisplay::setPage( QPicture* image )
{
    buffer.fill( white );
    QPainter p( &buffer );
    image->play( &p );
}

/** Display object's paint event */
void MPageDisplay::paintEvent( QPaintEvent* event )
{
    bitBlt( this, 0, 0, &buffer );
}

/** Sets the page display dimensions */
void MPageDisplay::setPageDimensions( QSize size )
{
    buffer.resize( size );
    resize( size );
}


// Return the preferred size of the display.

QSize MPageDisplay::sizeHint() const
{
    return buffer.size();
}


// Return the size policy.

QSizePolicy MPageDisplay::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

}

#include "mpagedisplay.moc"
