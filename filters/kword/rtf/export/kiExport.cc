/* This file is part of the KDE project
   Copurogjt (C) 2001 Michael Johnson <mikej@xnet.com>


   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kiExport.h>
#include <docinfoExport.h>
#include <kiRTFExport.h>

#define INSERT_TABLE_IN_PARA 1
#define TABLES_WITH_TITLES   0

/***************************************************************************/
// Globals
kiDraw kilDraw;  // create an instance of kiDraw to process tag data
double units = 20.0;
bool isPolyLine = true;


// Every tag has its own processing function. All of those functions
// have the same parameters since the functions are passed to
// ProcessSubtags throuch the TagProcessing class.  The top level
// function is ProcessSpreadsheetTag and can be called with the node returned
// by QDomDocument::documentElement (). The tagData argument can be
// used to either pass variables down to the subtags or to allow
// subtags to return values. As a bare minimum the tag processing
// functions must handle the tag's attributes and the tag's subtags
// (which it can choose to ignore). Currently implemented is
// processing for the following tags and attributes. The element tags are
// listed in capital letters but they are actually lower case.
//
// maindoc.xml (root):
// DOC editor, mime
//   HEAD
//     GRID dx dy align
//     HELPLINES  align
//        HL pos
//        VL pos
//   PAGE
//     LAYOUT format width height orientation tmargin bmargin rmargin lmargin unit
//     INFO  author creation_date last_modified
//   LAYER
//     POLYLINE  arrow1 arrow2
//       POINT x y
//       GOBJECT strokecolor strokestyle linewidth fillcolor fillstyle
//               fillpattern gradstyle gradcolor1 gradcolor2 id ref
//          MATRIX dx dy m21 m22 m11 m12
//     ELLIPSE x y rx ry angle1 angle2 kind
//        GOBJECT strokecolor strokestyle linewidth fillcolor fillstyle
//                fillpattern gradstyle gradcolor1 gradcolor2 id ref
//           MATRIX dx dy m21 m22 m11 m12
//     RECTANGLE base-attributes x y width height rounding
//        POLYLINE  arrow1 arrow2
//          POINT x y
//          GOBJECT strokecolor strokestyle linewidth fillcolor fillstyle
//                  fillpattern gradstyle gradcolor1 gradcolor2 id ref
//             MATRIX dx dy m21 m22 m11 m12
//     TEXT  x y align
//       FONT  face point-size weight
//       CDATA text
//       GOBJECT strokecolor strokestyle linewidth fillcolor fillstyle
//               fillpattern gradstyle gradcolor1 gradcolor2 id ref
//          MATRIX dx dy m21 m22 m11 m12
//     POLYGON
//        POLYLINE  arrow1 arrow2
//          POINT x y
//          GOBJECT strokecolor strokestyle linewidth fillcolor fillstyle
//                  fillpattern gradstyle gradcolor1 gradcolor2 id ref
//             MATRIX dx dy m21 m22 m11 m12
//     BEZIER arrow1 arrow2 closed
//        POLYLINE  arrow1 arrow2
//          POINT x y
//          GOBJECT strokecolor strokestyle linewidth fillcolor fillstyle
//                  fillpattern gradstyle gradcolor1 gradcolor2 id ref
//             MATRIX dx dy m21 m22 m11 m12
//     GROUP This tag surrounds a group of ellipse, rectangle, text, polygon,
//            and polylinetags that are grouped together.

//

/***************************************************************************/

void   ProcessKillustratorTag ( QDomNode    myNode,
                                void       *tagData,
                                QString    &outputText )
// Gets the attributes for the DOC tag and calls it's subtag processors
{
    Docinfo *docinfo = (Docinfo *) tagData;
    QString head;


    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "editor", "QString", (void *) &docinfo->editor  )
                       << AttrProcessing ( "version","QString", (void *) &docinfo->version )
                       << AttrProcessing ( "mime",   "QString", (void *) &docinfo->mime    );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "head", ProcessHeadTag,   (void *) &head )
                      << TagProcessing ( "page", kiProcessPageTag,  NULL          );
    ProcessSubtags (myNode, tagProcessingList, outputText);

}   // end ProcessKillustratorTag


/***************************************************************************/

void   kiProcessPageTag ( QDomNode    myNode,
                          void       *,
                          QString    &outputText )
// Gets the PAGE tag and calls it's subtag processors
{
    QString pageId;
    Layout layout;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "id", "QString", (void *) &pageId );
    ProcessAttributes (myNode, attrProcessingList);
    if( pageId != "Page 1" ) outputText += "\\page\n";


    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "layout", kiProcessLayoutTag, (void *) &layout )
                      << TagProcessing ( "layer",  ProcessLayerTag,    NULL           );

    ProcessSubtags (myNode, tagProcessingList, outputText);

}   // end ProcessKillustratorTag

/***************************************************************************/

void kiProcessLayoutTag ( QDomNode    myNode,
                          void       *tagData,
                          QString    &/*outputText*/ )
// Gets the attributes in the layout tag - information on page layout
// called by ProcessPageTag()

{
    Layout *layout = (Layout *) tagData;
// Declare number strings (double or int?
    QString width;
    QString height;
    QString lmargin;
    QString tmargin;
    QString rmargin;
    QString bmargin;


    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "format",     "QString", (void *) &layout->format      )
                       << AttrProcessing ( "width",      "QString", (void *) &width               )
                       << AttrProcessing ( "height",     "QString", (void *) &height              )
                       << AttrProcessing ( "lmargin",    "QString", (void *) &lmargin             )
                       << AttrProcessing ( "tmargin",    "QString", (void *) &tmargin             )
                       << AttrProcessing ( "rmargin",    "QString", (void *) &rmargin             )
                       << AttrProcessing ( "bmargin",    "QString", (void *) &bmargin             )
                       << AttrProcessing ( "unit",       "QString", (void *) &layout->unit        )
                       << AttrProcessing ( "orientation","QString", (void *) &layout->orientation );
    ProcessAttributes (myNode, attrProcessingList);

    if( layout->unit == "pt"     ) units = 20.0;
    if( layout->unit == "mm"     ) units = 56.69;
    if( layout->unit == "inch"   ) units = 1440.0;
    if( layout->unit == "cm"     ) units = 566.9;
    if( layout->unit == "pica"   ) units = 240.0;
    if( layout->unit == "didot"  ) units = 18.69;
    if( layout->unit == "cicero" ) units = 224.3;

    layout->width   = toTwips( width   );  // convert to twips
    layout->height  = toTwips( height  );
    layout->lmargin = toTwips( lmargin );
    layout->tmargin = toTwips( tmargin );
    layout->rmargin = toTwips( rmargin );
    layout->bmargin = toTwips( bmargin );

    AllowNoSubtags( myNode );

}   // end kiProcessLayoutTag


/***************************************************************************/

void ProcessHeadTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &outputText )
// Gets the subtags of the head tag
// called by ProcesszKillustratorTag()

{
    QString *currentpagenum = (QString *) tagData;
    Grid      grid;
    //Helplines helplines;
    Info      info;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "currentpagenum", "QString", (void *) currentpagenum );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "grid",      ProcessGridTag,     (void *) &grid      )
                      << TagProcessing ( "info",      ProcessInfoTag,     (void *) &info      );
    ProcessSubtags (myNode, tagProcessingList, outputText);


}   // end ProcessHeadTag


/***************************************************************************/

void ProcessHelplinesTag ( QDomNode    myNode,
                           void       *tagData,
                           QString    &outputText )
// Gets the attributes and subtags of the helplines tag
// called by ProcessGridTag()

{
    Helplines *helplines = (Helplines *) tagData;  // may need list for mult objects

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "align", "int", (void *) &helplines->align );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "hl", ProcessHlvlTag, (void *) &helplines->hl )
                      << TagProcessing ( "vl", ProcessHlvlTag, (void *) &helplines->vl );
    ProcessSubtags (myNode, tagProcessingList, outputText);


}   // end ProcessHelplinesTag


/***************************************************************************/

void ProcessInfoTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         )
// Gets the attributes in the info tag - information on author, creation, modification
// called by ProcessHeadTag()

{
    Info *info = (Info *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "author",        "QString", (void *) &info->author        )
                       << AttrProcessing ( "creation_date", "QString", (void *) &info->creation_date )
                       << AttrProcessing ( "last_modified", "QString", (void *) &info->last_modified );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags( myNode );

}   // end ProcessInfoTag


/***************************************************************************/

void ProcessHlvlTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         )
// Gets the attributes in the hl and vl tags - pos attribute
// called by ProcessHeadTag()

{
    int *pos = (int *) tagData;
    QString posString;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "pos", "QString", (void *) &posString );
    ProcessAttributes (myNode, attrProcessingList);

    *pos = toTwips( posString );
    AllowNoSubtags( myNode );

}   // end ProcessHlvlTag


/***************************************************************************/

void ProcessGridTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &outputText )
// Gets the attributes in the grid tag - information on help grid
// called by ProcessHeadTag()

{
    Grid *grid = (Grid *) tagData;
    Helplines helplines;
    QString dx;
    QString dy;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "dx",    "QString", (void *) &dx    )
                       << AttrProcessing ( "dy",    "QString", (void *) &dy    )
                       << AttrProcessing ( "align", "int",     (void *) &grid->align );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "helplines",   ProcessHelplinesTag,   (void *) &helplines );
    ProcessSubtags (myNode, tagProcessingList, outputText);


    // convert number strings to twips
    grid->dx = toTwips( dx );
    grid->dy = toTwips( dy );

}   // end ProcessGridTag


/***************************************************************************/

void ProcessPolylineTag ( QDomNode    myNode,
                          void       *tagData,
                          QString    &outputText )
// Gets the attributes and subtags of the polyline tag
// called by ProcessdocTag() or ProcessGroupTag()

{
    Polyline *polyline = (Polyline *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "arrow1", "int", (void *) &polyline->arrow1 )
                       << AttrProcessing ( "arrow2", "int", (void *) &polyline->arrow2 );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "point",   ProcessPointTag,   (void *) &polyline->point   )
                      << TagProcessing ( "gobject", ProcessGobjectTag, (void *) &polyline->gobject );
    ProcessSubtags (myNode, tagProcessingList, outputText);

    if( isPolyLine == true )
       outputText += ::kilDraw.doPolyline( *polyline );
    else isPolyLine = true;

}   // end ProcessPolylineTag

/***************************************************************************/

void ProcessPolygonTag ( QDomNode    myNode,
                         void       *,
                         QString    &outputText )
// Gets the attributes and subtags of the polygon tag
// called by ProcessdocTag() or ProcessGroupTag()

{
    Polyline polygon;;

    AllowNoAttributes(myNode);

    isPolyLine = false;
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "polyline",   ProcessPolylineTag,   (void *) &polygon );
    ProcessSubtags (myNode, tagProcessingList, outputText);
    outputText += kilDraw.doPolygon( polygon );


}   // end ProcessPolygonTag()

/***************************************************************************/

void ProcessRectangleTag ( QDomNode    myNode,
                           void       *,
                           QString    &outputText )
// Gets the attributes and subtags of the polygon tag
// called by ProcessdocTag() or ProcessGroupTag()

{
    Rectangle rectangle;
    QString x;
    QString y;
    QString width;
    QString height;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "x",        "QString", (void *) &x                   )
                       << AttrProcessing ( "y",        "QString", (void *) &y                   )
                       << AttrProcessing ( "width",    "QString", (void *) &width               )
                       << AttrProcessing ( "height",   "QString", (void *) &height              )
                       << AttrProcessing ( "rounding", "int",     (void *) &rectangle.rounding );
    ProcessAttributes (myNode, attrProcessingList);

    isPolyLine = false;
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "polyline",   ProcessPolylineTag,   (void *) &rectangle.polyline );
    ProcessSubtags (myNode, tagProcessingList, outputText);

    // Convers number string to twips
    rectangle.x      = toTwips( x );
    rectangle.y      = toTwips( y );
    rectangle.width  = toTwips( width );
    rectangle.height = toTwips( height );

    outputText += kilDraw.doRectangle( rectangle );

}   // end ProcessRectangleTag()

/***************************************************************************/

void ProcessBezierTag ( QDomNode    myNode,
                        void       *,
                        QString    &outputText )
// Gets the attributes and subtags of the bezier tag
// called by ProcessdocTag() or ProcessGroupTag()

{
    Bezier bezier;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "closed", "int", (void *) &bezier.closed  );
    ProcessAttributes (myNode, attrProcessingList);

    isPolyLine = false;
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "polyline",   ProcessPolylineTag,   (void *) &bezier.polyline );
    ProcessSubtags (myNode, tagProcessingList, outputText);
    outputText += kilDraw.doBezier( bezier );


}   // end ProcessBezierTag()

/***************************************************************************/

void ProcessEllipseTag ( QDomNode    myNode,
                         void       *,
                         QString    &outputText )
// Gets the attributes and subtags of the ellipse tag
// called by ProcessdocTag() or ProcessGroupTag()

{
    Ellipse ellipse;
    QString x;
    QString y;
    QString rx;
    QString ry;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "x",      "QString", (void *) &x              )
                       << AttrProcessing ( "y",      "QString", (void *) &y              )
                       << AttrProcessing ( "rx",     "QString", (void *) &rx             )
                       << AttrProcessing ( "ry",     "QString", (void *) &ry             )
                       << AttrProcessing ( "angle1", "int",     (void *) &ellipse.angle1 )
                       << AttrProcessing ( "angle2", "int",     (void *) &ellipse.angle2 )
                       << AttrProcessing ( "kind",   "int",     (void *) &ellipse.kind   );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "gobject",   ProcessGobjectTag,   (void *) &ellipse.gobject );
    ProcessSubtags (myNode, tagProcessingList, outputText);
    // Convers number string to twips
    ellipse.x  = toTwips( x );
    ellipse.y  = toTwips( y );
    ellipse.rx = toTwips( rx );
    ellipse.ry = toTwips( ry );

    outputText += kilDraw.doEllipse( ellipse );

}   // end ProcessEllipseTag()

/***************************************************************************/

void ProcessGobjectTag ( QDomNode    myNode,
                         void       *tagData,
                         QString    &outputText )
// Gets the attributes and subtags of the gobject tag
// called by ProcessPolylineTag() or ProcessEllipseTag()

{
    Gobject *gobject = (Gobject *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "strokecolor", "QString", (void *) &gobject->strokecolor )
                       << AttrProcessing ( "strokestyle", "int",     (void *) &gobject->strokestyle )
                       << AttrProcessing ( "linewidth",   "int",     (void *) &gobject->linewidth   )
                       << AttrProcessing ( "fillcolor",   "QString", (void *) &gobject->fillcolor   )
                       << AttrProcessing ( "fillstyle",   "int",     (void *) &gobject->fillstyle   )
                       << AttrProcessing ( "fillpattern", "int",     (void *) &gobject->fillpattern )
                       << AttrProcessing ( "gradstyle",   "int",     (void *) &gobject->gradstyle   )
                       << AttrProcessing ( "gradcolor1",  "QString", (void *) &gobject->gradcolor1  )
                       << AttrProcessing ( "gradcolor2",  "QString", (void *) &gobject->gradcolor2  )
                       << AttrProcessing ( "id",          "int",     (void *) &gobject->id          )
                       << AttrProcessing ( "ref",         "int",     (void *) &gobject->ref         );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "matrix",   ProcessMatrixTag,   (void *) &gobject->matrix );
    ProcessSubtags (myNode, tagProcessingList, outputText);
kdError(3505) << "strokecolor " << gobject->strokecolor << " fillcolor " << gobject->fillcolor
              << " gradcolor1 " << gobject->gradcolor1 << endl;

}   // end ProcessGobjectTag()

/***************************************************************************/

void ProcessMatrixTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         )
// Gets the attributes in the matrix tag
// called by ProcessGobjectTag()

{
    Matrix *matrix = (Matrix *) tagData;
    QString dx;
    QString dy;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "dx",  "QString", (void *) &dx          )
                       << AttrProcessing ( "dy",  "QString", (void *) &dy          )
                       << AttrProcessing ( "m21", "int",     (void *) &matrix->m21 )
                       << AttrProcessing ( "m22", "int",     (void *) &matrix->m22 )
                       << AttrProcessing ( "m11", "int",     (void *) &matrix->m11 )
                       << AttrProcessing ( "m12", "int",     (void *) &matrix->m12 );
    ProcessAttributes (myNode, attrProcessingList);

    matrix->dx = toTwips( dx );
    matrix->dy = toTwips( dy );
    AllowNoSubtags( myNode );

}   // end ProcessMatrixTag()

 /***************************************************************************/

void kiProcessFontTag   ( QDomNode    myNode,
                          void       *tagData,
                          QString    &         )
// Gets the attributes in the font tag
// called by kiProcessTextTag()

{
    Font *font = (Font *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "weight",     "int",     (void *) &font->weight    )
                       << AttrProcessing ( "face",       "QString", (void *) &font->face      )
                       << AttrProcessing ( "italic",      "int",    (void *) &font->italic    )
                       << AttrProcessing ( "point-size", "int",     (void *) &font->pointSize );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags( myNode );

}   // end kiProcessFontTag()

/***************************************************************************/

void kiProcessTextTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &outputText )
// Gets the attributes and subtags of the text tag
// called by ProcessLayerTag()

{
    TextObject *text = (TextObject *) tagData;
    QDomNode thisNode;
    kiFormat   formatList;
    QString x;
    QString y;


    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "x",     "QString", (void *) &x           )
                       << AttrProcessing ( "y",     "QString", (void *) &y           )
                       << AttrProcessing ( "align", "int",     (void *) &text->align );
    ProcessAttributes (myNode, attrProcessingList);

   // Process CDATA text
   formatList.text = "";
   thisNode = myNode.firstChild();
   while( !thisNode.isNull() )
      {
      if( thisNode.isCDATASection() )
         {
         formatList.text = thisNode.nodeValue(); // extract text
         break;
         }
      }  // end while

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "gobject", ProcessGobjectTag, (void *) &text->gobject   )
                      << TagProcessing ( "font",    kiProcessFontTag,  (void *) &formatList.font );
    ProcessSubtags (myNode, tagProcessingList, outputText);

    // convert number strings to twips
    text->x = toTwips( x );
    text->y = toTwips( y );
    text->format << formatList;

    outputText += kilDraw.doTextBox( *text );

}   // end kiProcessTextTag()


/***************************************************************************/

void ProcessPointTag ( QDomNode    myNode,
                       void       *tagData,
                       QString    &         )
// Gets the attributes in the matrix tag
// called by ProcessGobjectTag()

{
    QValueList<Point> *pointList = (QValueList<Point> *) tagData;
    Point point;
    QString x;
    QString y;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "x", "QString", (void *) &x  )
                       << AttrProcessing ( "y", "QString", (void *) &y );
    ProcessAttributes (myNode, attrProcessingList);

    // convert number strings to twips
    point.x = toTwips( x );
    point.y = toTwips( y );
    *pointList << point;

    AllowNoSubtags( myNode );

}   // end ProcessPointTag()


/***************************************************************************/

void   ProcessLayerTag ( QDomNode    myNode,
                        void       *,
                        QString    &outputText )
// Processes the subtags of the layer tag
// Called by ProcessKillustratorTag()
{
    Group group;

    AllowNoAttributes (myNode );

    isPolyLine = true;
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "polyline",  ProcessPolylineTag,  NULL   )
                      << TagProcessing ( "rectangle", ProcessRectangleTag, NULL   )
                      << TagProcessing ( "ellipse",   ProcessEllipseTag,   NULL   )
                      << TagProcessing ( "text",      kiProcessTextTag,    NULL   )
                      << TagProcessing ( "polygon",   ProcessPolygonTag,   NULL   )
                      << TagProcessing ( "bezier",    ProcessBezierTag,    NULL   )
                      << TagProcessing ( "group",     ProcessGroupTag,     &group );
    ProcessSubtags (myNode, tagProcessingList, outputText);

}   // end ProcessLayerTag

/***************************************************************************/

void   ProcessGroupTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &outputText )
// Processes the subtags of the layer tag
// Called by ProcessKillustratorTag()
{
    Group *group = (Group *) tagData;

    outputText += kilDraw.doGroupStart( *group);
    AllowNoAttributes (myNode );

    isPolyLine = true;
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "gobject",   ProcessGobjectTag,  (void *) &group->gobject )
                      << TagProcessing ( "polyline",  ProcessPolylineTag,  NULL                    )
                      << TagProcessing ( "rectangle", ProcessRectangleTag, NULL                    )
                      << TagProcessing ( "ellipse",   ProcessEllipseTag,   NULL                    )
                      << TagProcessing ( "text",      kiProcessTextTag,    NULL                    )
                      << TagProcessing ( "polygon",   ProcessPolygonTag,   NULL                    )
                      << TagProcessing ( "bezier",    ProcessBezierTag,    NULL                    );
    ProcessSubtags (myNode, tagProcessingList, outputText);
    outputText += kilDraw.doGroupEnd( *group);

}   // end ProcessGroupTag

int toTwips( QString numString )
   {
   double dnum;
   int twips;

   if( numString.find( '.' ) < 0 )
      {
      numString += ".";
      }
   dnum = numString.toDouble();
   twips = (int) ( dnum * units );  // convert to twips
   return twips;

   }  // end toTwips()

