#include <qstring.h>
#include <rtfexport.h>
#include <kiExport.h>
#include <qvaluelist.h>
#include <kiRTFExport.h>

// define the conversions of killustrator fill patterns to RTF fill patterns
static const int fillPatternTable[16] = {0, 1, 13, 12, 9, 8, 7, 3, 2, 20, 21,
                                  24, 23, 22, 25, 8 };

/**************************************************************************/
Size    kiDraw::sizeObject( QValueList<Point> &ptList )
   {
   unsigned int minx =0xFFFF;
   unsigned int miny =0xFFFF;
   unsigned int maxx =0;
   unsigned int maxy =0;
   QValueList<Point>::Iterator ptListIt;
   Size size;
   int  firstx;  // first point is used to generate markup for arcs
   int  firsty;

   // get the first point for arc curve orientation
   ptListIt = ptList.begin();
   firstx   = (*ptListIt).x;
   firsty   = (*ptListIt).y;
   size.arcFlipx = false;
   size.arcFlipx = false;
   // find the minimum and maximum extremes of the point set
   for( ptListIt  = ptList.begin();
        ptListIt != ptList.end();
        ptListIt++ )
      {
      minx = minx < (*ptListIt).x ? minx : (*ptListIt).x;
      miny = miny < (*ptListIt).y ? minx : (*ptListIt).y;
      maxx = maxx < (*ptListIt).x ? maxx : (*ptListIt).x;
      maxy = maxy < (*ptListIt).y ? maxy : (*ptListIt).y;
      }  // end for
   size.w = (int)( maxx - minx );
   size.h = (int)( maxy - miny );
   size.x = minx;  // bounding rectangle of the bezier curve needed for RTF
   size.y = miny;
   ptListIt--;  // point to last entry
   if( firstx < (*ptListIt).x ) size.arcFlipx = true;
   if( firsty > (*ptListIt).y ) size.arcFlipy = true;
   return size;
   }  // end sizeObject()

/**************************************************************************/
QString kiDraw::doStart(void)
   {
   return QString( "{\\*\\do\\dobxpage\\dobypage" );
   }  // end doStart()

/**************************************************************************/
QString kiDraw::doEnd(void)
   {
   return QString( "}\n" );
   } // end doEnd()

/**************************************************************************/
QString kiDraw::doArrowDraw(int arrow1, int arrow2 )
   {
   QString markup = "";

   if( arrow1 == 1 )
      {
      markup += "\\dpastartsol\\dpastartl2\\dpastartw2";  // med arrow at line stare
      }
   if( arrow2 == 1 )
      {
      markup += "\\dpaendsol\\dpaendl2\\dpaendw2"; // medium arrow at end of line
      }

   return markup;
   } // end doArrowDraw()

/**************************************************************************/
// The following function does RTF markup for drawing object point locations
QString kiDraw::doPointList( QValueList<Point> &points )
   {
   QString markup;
   int length;
   QValueList<Point>::Iterator pointIt;

   if( (length = (int)points.count()) == 0 )
      return "";
   markup = "\\dppolycount" + QString::number( length ); // set length first

   // output point locations
   for(  pointIt =  points.begin();
         pointIt != points.end();
         pointIt++ )
      {
      markup += "\\dpptx" + QString::number( (*pointIt).x ) +
                "\\dppty" + QString::number( (*pointIt).y );
      }  // end for
   return markup;
   } // end doPointList()

/**************************************************************************/
QString kiDraw::doSizeLocation(int x, int y, int height, int width)
   {
   QString markup;

   markup = "\\dpx" + QString::number( x ) + "\\dpy" + QString::number( y );
   markup += "\\dpxsize" + QString::number( width ) +
             "\\dpysize" + QString::number( height );
   return markup;
   } // end doSizeLocation()

/**************************************************************************/
QString kiDraw::doLineParameters( Gobject &gobject)
   {
   QString markup;
   ColorLayout   lineColors;

   lineColors = decodeColorString( gobject.strokecolor );  //get individual colors
   markup = "\\dplinew" + QString::number( (gobject.linewidth * 20) ); //line width
   // markup line colors
   markup += "\\dplinecor" + QString::number( lineColors.red ) +
             "\\dplinecog" + QString::number( lineColors.green ) +
             "\\dplinecob" + QString::number( lineColors.blue );

   // line style solid, dotted, etc.
   switch( gobject.strokestyle )
      {
      case 0:  // no line
         {
         markup += "\\dplinehollow";
         break;
         }
      case 1:  // solid line
         {
         markup += "\\dplinesolid";
         break;
         }
      case 2:  // dashed line
         {
         markup += "\\dplinedash";
         break;
         }
      case 3:  // dotted line
         {
         markup += "\\dplinedot";
         break;
         }
      case 4:  // dashdot line
         {
         markup += "\\dplinedado";
         break;
         }
      case 5:  // dashdotdot line
         {
         markup += "\\dplinedadodo";
         break;
         }
      default:
         {
         markup += "\\dplinesolid";
         }
      }  // end switch
   return markup;
   } // end doLineParameters()

/**************************************************************************/
QString kiDraw::doBackgroundFill( Gobject &gobject )
   {
   QString markup = "";
   ColorLayout   fillColors;
   int     fillpat;  // RTF fill pattern number

   // foreground fill colors
   fillColors = decodeColorString( gobject.fillcolor );
   markup += "\\dpfillfgcr" + QString:: number( fillColors.red ) +
             "\\dpfillfgcg" + QString:: number( fillColors.green ) +
             "\\dpfillfgcb" + QString:: number( fillColors.blue );

   // Background fill colors = gradient1 colors
   fillColors = decodeColorString( gobject.gradcolor1 );
   markup += "\\dpfillbgcr" + QString:: number( fillColors.red ) +
             "\\dpfillbgcg" + QString:: number( fillColors.green ) +
             "\\dpfillbgcb" + QString:: number( fillColors.blue );

   // Note - RTF does not have command equivalent to fillstyle
   // It's included in fillpattern
   // calculate fillpat
   // convert to RTF pattern number
   if( gobject.fillpattern >= 0 && gobject.fillpattern <= 15 )
      {
      fillpat = fillPatternTable[ gobject.fillpattern ];
      markup += "\\dpfillpat" + QString::number( fillpat );
      }
   else markup += "\\dpfillpat0";  // no fill is default
   return markup;
   } // end doBackgroundFill()

/**************************************************************************/
ColorLayout   kiDraw::decodeColorString( QString &colorString )
   {
   ColorLayout   colorDec;
   bool    ok;  // indicates conversion was correct
   QString color;
kdError (3505) << "color string " << colorString << endl;
   color = colorString.mid( 1, 2 );
   colorDec.red = color.toInt(&ok, 16 );
   if( !ok ) colorDec.red = 255;
   color = colorString.mid( 3, 2 );
   colorDec.green = color.toInt(&ok, 16 );
   if( !ok ) colorDec.green = 255;
   color = colorString.mid( 5, 2 );
   colorDec.blue = color.toInt(&ok, 16 );
   if( !ok ) colorDec.blue = 255;
kdError (3505) << "red " << colorDec.red << " green " << colorDec.green<< " blue " << colorDec.blue << endl;

   return colorDec;
   } // end dodecodeColorString()


/**************************************************************************/
QString kiDraw::doRectangle( Rectangle &rectangle ) // draws a rectangle in RTF markup
   {
   QString markup;

   isPolyLine = false;
   markup = doStart();
   markup += "\\dprect";
   if( rectangle.rounding > 0 ) markup += "\\dproundr"; // round corners
   markup += doSizeLocation( rectangle.x, rectangle.y, rectangle.height,
                             rectangle.width );
   markup += doBackgroundFill( rectangle.polyline.gobject );
   markup += doLineParameters( rectangle.polyline.gobject );
   markup += doEnd();

   return markup;
   } // end doRecdtangle()

/**************************************************************************/
QString kiDraw::doEllipse( Ellipse &ellipse )
   {
   QString markup;

   markup = doStart();
   markup += "\\dpellipse";
   markup += doSizeLocation( ellipse.x, ellipse.y, ( 2 * ellipse.ry ),
                             ( 2 *ellipse.rx ) );
   markup += doBackgroundFill( ellipse.gobject );
   markup += doLineParameters( ellipse.gobject );
   markup += doEnd();

   return markup;

   } // end doEllipse()

/**************************************************************************/
QString kiDraw::doPolyline( Polyline &polyline )  // used to draw lines and polygons also
   {
   QString markup;
   Size size;

   markup = doStart();
   if( polyline.point.count() > 2 )
         markup += "\\dppolyline";
   else if( polyline.point.count() == 2 )
         markup += "\\dpline";
   else return "";
   // get the size of the object defined by the point set
   size    = sizeObject( polyline.point );
   markup += doSizeLocation( size.x, size.y, size.h, size.w );
   markup += doBackgroundFill( polyline.gobject );
   markup += doLineParameters( polyline.gobject );
   markup += doArrowDraw( polyline.arrow1, polyline.arrow2 );
   markup += doEnd();

   return markup;
   } // end doPolyline()

/**************************************************************************/
QString kiDraw::doPolygon( Polyline &polygon )  // used to draw lines and polygons also
   {
   QString markup;
   Size size;

   markup = doStart();
   markup += "\\dppolygon";
   // get the size of the object defined by the point set
   size    = sizeObject( polygon.point );
   markup += doSizeLocation( size.x, size.y, size.h, size.w );
   markup += doBackgroundFill( polygon.gobject );
   markup += doLineParameters( polygon.gobject );
   markup += doArrowDraw( polygon.arrow1, polygon.arrow2 );
   markup += doEnd();

   return markup;
   } // end doPolygon()

/**************************************************************************/
QString kiDraw::doBezier( Bezier &bezier )
   {
   QString markup;
   Size size;

   markup = doStart();
   markup += "\\dparc";
   // get the size of the object defined by the point set
   size    = sizeObject( bezier.polyline.point );
   // check for orentation
   if( size.arcFlipx ) markup += "dparcflipx";
   if( size.arcFlipy ) markup += "dparcflipy";

   markup += doSizeLocation( size.x, size.y, size.h, size.w );
   markup += doBackgroundFill( bezier.polyline.gobject );
   markup += doLineParameters( bezier.polyline.gobject );
   markup += doEnd();

   return markup;
   } // end doBezier()

/**************************************************************************/
QString kiDraw::doTextBox( TextObject &text )
   {
   QString markup;
   // The following are used to call ProcessParagraph which process text
   // The following classes are defined in kwExport.h
   QValueList<FormatData>defaultFormat; // defaut formatting for paragraph
   QValueList<FormatData>formatList;    // format of text items
   FormatData formatData;
   ParaLayout layout;
   DocData    docData;
   QValueList<kiFormat>::Iterator It; // to iterate font list
   int        pos = 0;  // position of text
   QString    paraText;

   markup = doStart();
   markup += "\\dptxbx{{\\dptxbxtext";

   // enter data into data structures for text processing
   docData.grpMgr = false;
   switch( text.align )
      {
      case 0: {layout.flow = "left";   break; }
      case 1: {layout.flow = "right";  break; }
      case 2: {layout.flow = "center"; break; }
      }  // end switch
   layout.layout = "standard";


   // Iterate through all text entries and convert to kword RTF filter format
   for( It = text.format.begin();
        It != text.format.end();
        It++                   )
      {
      formatData.text.pos = pos;
      formatData.text.len = (*It).text.length();
      pos += formatData.text.len;
      formatData.text.fontSize = (*It).font.pointSize;
      formatData.text.fontWeight = (*It).font.weight;
      formatData.text.fontName = (*It).font.face;
      if( (*It).font.italic == 1 )
         formatData.text.italic = true;
      paraText += (*It).text;

      formatList << formatData;
      }
   // make paragraph default layout
   It = text.format.begin();
   formatData.text.pos = 0;
   formatData.text.len = pos;
   formatData.text.fontSize = (*It).font.pointSize;
   formatData.text.fontWeight = (*It).font.weight;
   formatData.text.fontName = (*It).font.face;
   if( (*It).font.italic == 1 )
      formatData.text.italic = true;
   defaultFormat << formatData;

   // insert paragraph text - use kword ProcessParagraph() to do this
   ProcessParagraph( paraText, defaultFormat, formatList, markup,
                     layout, &docData );

   markup += doSizeLocation( text.x, text.y, text.gobject.matrix.dy,
                             text.gobject.matrix.dx );
   markup += doBackgroundFill( text.gobject );
   markup += doLineParameters( text.gobject );
   markup += doEnd();

   return markup;

   } // end doTextBox()

/**************************************************************************/
QString kiDraw::doGroupStart( Group &group )
   {
   QString markup;
   markup = "\n\\dpgroup\\dpcount2";
   return markup;
   } // end doGroupStart()

/**************************************************************************/
QString kiDraw::doGroupEnd( Group &group )
   {
   QString markup;
   markup = "\\dpendgroup";
   return markup;
   } // end doGroupEnd()
