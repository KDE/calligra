/***************************************************************************
             mlineobject.h  -  Kugar report line object
             -------------------
   begin     : Wed Feb 23 2000
   copyright : (C) 2000 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MLINEOBJECT_H
#define MLINEOBJECT_H

#include <qobject.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <QColor>

/**Kugar report line object
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MLineObject : public QObject
{

public:
    /** Line style constants */
    enum Style { NoPen = 0, SolidLine, DashLine,
                 DotLine, DashDotLine, DashDotDotLine };

    /** Constructor */
    MLineObject();
    /** Copy constructor */
    MLineObject( const MLineObject& mLineObject );
    /** Assignment operator */
    MLineObject operator=( const MLineObject& mLineObject );
    /** Destructor */
    virtual ~MLineObject();

protected:
    /** Object's x start position */
    int xpos1;
    /** Object's y start postition */
    int ypos1;
    /** Object's x end position */
    int xpos2;
    /** Object's y end position */
    int ypos2;

    /** Object's width */
    int penWidth;
    /** Object's color */
    QColor penColor;
    /** Object's style */
    int penStyle;

public:
    /** Sets the start and end points for the line */
    void setLine( int xStart, int yStart, int xEnd, int yEnd );
    /** Sets the object's width */
    void setWidth( int width );
    /** Sets the object's style */
    void setStyle( int style );
    /** Sets the object's color */
    void setColor( int r, int g, int b );
    /** Draws the object to the specified painter & x/y offsets */
    virtual void draw( QPainter* p, int xoffset, int yoffset );

protected:
    /** Draws the object to the specified painter & x/y offsets */
    void drawBase( QPainter* p, int xoffset, int yoffset );

private:
    /** Copies member data from one object to another.
      * Used by the copy constructor and assignment operator
      */
    void copy( const MLineObject* mLineObject );
};

}

#endif
