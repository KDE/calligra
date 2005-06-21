/***************************************************************************
             mreportobject.h  -  Kugar report object baseclass
             -------------------
   begin     : Wed Aug 11 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2004 Alexander Dymo
   email     : cloudtemple@mksat.net
***************************************************************************/

#ifndef MREPORTOBJECT_H
#define MREPORTOBJECT_H

#include <qobject.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qcolor.h>

/**Kugar report object baseclass
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MReportObject : public QObject
{

public:
    /** Border style constants */
    enum BorderStyle { NoPen = 0, SolidLine, DashLine,
                       DotLine, DashDotLine, DashDotDotLine };

    /** Constructor */
    MReportObject();
    /** Copy constructor */
    MReportObject( const MReportObject& mReportObject );
    /** Assignment operator */
    MReportObject operator=( const MReportObject& mReportObject );
    /** Destructor */
    virtual ~MReportObject();

protected:
    /** Object's x position */
    int xpos;
    /** Object's y postition */
    int ypos;
    /** Object's width */
    int width;
    /** Object's height */
    int height;

    /** Object's background color */
    QColor backgroundColor;
    /** Object's foreground color */
    QColor foregroundColor;

    /** Object's border color */
    QColor borderColor;
    /** Object's border width */
    int borderWidth;
    /** Object's border style */
    int borderStyle;
    /** Draw left border */
    bool drawLeft;
    /** Draw right border */
    bool drawRight;
    /** Draw top border */
    bool drawTop;
    /** Draw bottom border */
    bool drawBottom;

public:
    /** Draws the object to the specified painter & x/y offsets */
    virtual void draw( QPainter* p, int xoffset, int yoffset );
    /** Sets the object's position and size */
    void setGeometry( int x, int y, int w, int h );
    /** Sets the object's position */
    void move( int x, int y );
    /** Gets the object's x position */
    int getX();
    /** Gets the object's y position */
    int getY();
    /** Sets the object's background color - default is white*/
    void setBackgroundColor( int r, int g, int b );
    /** Sets the object's foreground color - default is black*/
    void setForegroundColor( int r, int g, int b );
    /** Sets the object's border color - default is black*/
    void setBorderColor( int r, int g, int b );
    /** Sets the object's border width - default is 1*/
    void setBorderWidth( int width );
    /** Sets the object's border style - default is SolidLine*/
    void setBorderStyle( int style );

    /** Gets the draw left border flag */
    bool getDrawLeft();
    /** Gets the draw right border flag */
    bool getDrawRight();
    /** Gets the draw top border flag */
    bool getDrawTop();
    /** Gets the draw bottom border flag */
    bool getDrawBottom();

    /** Sets the draw left border flag */
    void setDrawLeft( bool d );
    /** Sets the draw right border flag */
    void setDrawRight( bool d );
    /** Sets the draw top border flag */
    void setDrawTop( bool d );
    /** Sets the draw bottom border flag */
    void setDrawBottom( bool d );

protected:
    /** Draws the base object to the specified painter & x/y offsets */
    void drawBase( QPainter* p, int xoffset, int yoffset );

private:
    /** Copies member data from one object to another.
      * Used by the copy constructor and assignment operator
      */
    void copy( const MReportObject* mReportObject );

};

}

#endif
