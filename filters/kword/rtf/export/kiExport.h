
#ifndef KIEXPORT_H
#define KIEXPORT_H

#include <qstring.h>
#include <qdom.h>
#include <kdebug.h>
#include <qdir.h>
#include <tagprocessing.h>
#include <qvaluelist.h>

#define KDEBUG_KIFILTER 30505




/***************************************************************************/
class Docinfo
   {
   public:

   QString editor;
   QString mime;
   QString version;
   };

/***************************************************************************/
class Layout
   {
   public:

   QString format;
   int     width;
   int     height;
   QString orientation;
   int     rmargin;
   int     lmargin;
   int     tmargin;
   int     bmargin;
   QString unit;
   };

/***************************************************************************/
class Point
   {
   public:
   Point() {}
   int x;
   int y;
   };

/***************************************************************************/
class Matrix
   {
   public:

   int dx;
   int dy;
   int m21;
   int m22;
   int m11;
   int m12;
   };

/***************************************************************************/
class Gobject
   {
   public:

   QString strokecolor;
   int     strokestyle;
   int     linewidth;    // in points
   QString fillcolor;
   int     fillstyle;
   int     fillpattern;
   int     gradstyle;
   QString gradcolor1;
   QString gradcolor2;
   int     id;
   int     ref;
   Matrix  matrix;
   };

/***************************************************************************/
class Info
   {
   public:

   QString author;
   QString creation_date;
   QString last_modified;
   };

/***************************************************************************/
class Grid
   {
   public:

   int dx;
   int dy;
   int align;
   };  // end Grid

/***************************************************************************/
class Helplines
   {
   public:

   int hl;
   int vl;
   int align;
   };  // end Helplines

/***************************************************************************/
class Polyline
   {
   public:

   int               arrow1;
   int               arrow2;
   QValueList<Point> point;
   Gobject           gobject;
   };  // end Polyline

/***************************************************************************/
class Rectangle
   {
   public:

   int      x;
   int      y;
   int      width;
   int      height;
   int      rounding;
   Polyline polyline;
   };  // end Rectangle

/***************************************************************************/
class Bezier
   {
   public:

   int      closed;
   Polyline polyline;
   };  // end Bezier

/***************************************************************************/
class Ellipse
   {
   public:

   int     x;
   int     y;
   int     rx;
   int     ry;
   int     angle1;
   int     angle2;
   int     kind;
   Gobject gobject;
   };  // end Ellipse

/***************************************************************************/
class Font
   {
   public:

   int     pointSize;
   QString face;
   int     weight;
   int     italic;
   };

/***************************************************************************/
class kiFormat
   {
   public:

   QString text;
   Font    font;
   };

/***************************************************************************/
class TextObject
   {
   public:

   int     x;
   int     y;
   int     align;
   QValueList <kiFormat> format;
   Gobject gobject;
   };

/***************************************************************************/
class Orig
   {
   public:

   int x;
   int y;
   };

/***************************************************************************/
class ObjectRect
   {
   public:

   int     x;
   int     y;
   int     w;
   int     h;
   };
/***************************************************************************/
class Object
   {
   public:

   QString    mime;
   QString    url;
   Gobject    gobject;
   ObjectRect rect;
   };
/***************************************************************************/
class Group
   {
   public:

   Gobject gobject;
   };


/***************************************************************************/

/***************************************************************************/
void   ProcessKillustratorTag ( QDomNode    myNode,
                                void       *tagData,
                                QString    &outputText );

void   kiProcessPageTag ( QDomNode    myNode,
                          void       *,
                          QString    &outputText );

void kiProcessLayoutTag ( QDomNode    myNode,
                          void       *tagData,
                          QString    &outputText );

void ProcessHeadTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &outputText );

void ProcessHelplinesTag ( QDomNode    myNode,
                           void       *tagData,
                           QString    &outputText );

void ProcessInfoTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         );

void ProcessHlvlTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         );

void ProcessGridTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &outputText );

void ProcessPolylineTag ( QDomNode    myNode,
                          void       *tagData,
                          QString    &outputText );

void ProcessPolygonTag ( QDomNode    myNode,
                         void       *tagData,
                         QString    &outputText );

void ProcessRectangleTag ( QDomNode    myNode,
                           void       *tagData,
                           QString    &outputText );

void ProcessBezierTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &outputText );

void ProcessEllipseTag ( QDomNode    myNode,
                         void       *tagData,
                         QString    &outputText );

void ProcessGobjectTag ( QDomNode    myNode,
                         void       *tagData,
                         QString    &outputText );

void ProcessMatrixTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void kiProcessFontTag   ( QDomNode    myNode,
                          void       *tagData,
                          QString    &         );

void kiProcessTextTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &outputText );

void ProcessPointTag ( QDomNode    myNode,
                       void       *tagData,
                       QString    &         );

void   ProcessLayerTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &outputText );

void   ProcessGroupTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &outputText );


int toTwips( QString numString );

#endif
