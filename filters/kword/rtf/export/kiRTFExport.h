#ifndef KIRTFEXPORT_H
#define KIRTFEXPORT_H

#include <qstring.h>
#include <rtfexport.h>
#include <kiExport.h>
#include <qvaluelist.h>

extern bool isPolyLine;

// Structure used to store calculated data needed for RTF
//.markup. Data calculated from the point set in polyline
struct  Size
   {
   int  w;
   int  h;
   int  x;
   int  y;
   bool arcFlipx;
   bool arcFlipy;
   };

// Main class for the killustrator RTF export.
class kiDraw
   {
   private:

   QString     doStart(void);
   QString     doEnd(void);
   QString     doArrowDraw( int arrow1, int arrow2 );
   QString     doShadow( Gobject gobject );
   QString     doPointList( QValueList<Point> &points );
   QString     doSizeLocation( int x, int y, int height, int width );
   QString     doLineParameters(Gobject &gobject);
   QString     doBackgroundFill(Gobject &gobject);
   ColorLayout decodeColorString( QString &colorString );
   Size        sizeObject( QValueList<Point> &ptList );

   public :
   kwDraw()  {}
   ~kiDraw() {}


   QString doRectangle( Rectangle &rectangle ); // draws a rectangle in RTF markup
   QString doEllipse( Ellipse &ellipse );
   QString doPolyline( Polyline &polyline );
   QString doPolygon( Polyline &polygon );
   QString doBezier( Bezier &bezier );
   QString doTextBox( TextObject &text );
   QString doGroupStart( Group &group );
   QString doGroupEnd( Group &group );

   };  // end class kiDraw

#endif