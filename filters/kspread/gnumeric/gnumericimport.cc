/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>
   Copyright (C) 2005 Laurent Montel <montel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/* GNUmeric import filter by Phillip Ezolt 6-2-2001 */
/*                        phillipezolt@hotmail.com  */
/* additions: Norbert Andres nandres@web.de         */

#include <q3dict.h>
#include <QFile>
#include <QStringList>
//Added by qt3to4:
#include <QByteArray>

#include <gnumericimport.h>
#include <kmessagebox.h>
#include <kfilterdev.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>

// KOffice
#include <KoDocumentInfo.h>

// KSpread
#include <kspread/Doc.h>
#include <kspread/Map.h>
#include <kspread/Format.h>
#include <kspread/Sheet.h>
#include <kspread/SheetPrint.h>
#include <kspread/Cell.h>
#include <kspread/Util.h>
#include <kspread/Validity.h>

#include <math.h>

#define SECS_PER_DAY 86400
#define HALF_SEC (0.5 / SECS_PER_DAY)

using namespace KSpread;

// copied from gnumeric: src/format.c:
static const int g_dateSerial_19000228 = 59;
/* One less that the Julian day number of 19000101.  */
static int g_dateOrigin = 0;

// copied from gnumeric: src/formats.c:
static char const * cell_format_date [] = {
	"m/d/yy",		/* 0 Cell::date_format5*/
	"m/d/yyyy",		/* 1 Cell::date_format6*/
	"d-mmm-yy",		/* 2 Cell::date_format1 18-Feb-99 */
	"d-mmm-yyyy",		/* 3 Cell::date_format2 18-Feb-1999 */
	"d-mmm",		/* 4 Cell::date_format3 18-Feb */
	"d-mm",			/* 5 Cell::date_format4 18-05 */
	"mmm/d",		/* 6 Cell::date_format11*/
	"mm/d",			/* 7 Cell::date_format12*/
	"mm/dd/yy",		/* 8 Cell::date_format19*/
	"mm/dd/yyyy",		/* 9 Cell::date_format18*/
	"mmm/dd/yy",		/* 10 Cell::date_format20*/
	"mmm/dd/yyyy",		/* 11 Cell::date_format21*/
	"mmm/ddd/yy",		/* 12 */
	"mmm/ddd/yyyy",		/* 13 */
	"mm/ddd/yy",		/* 14 */
	"mm/ddd/yyyy",		/* 15 */
	"mmm-yy",		/* 16 Cell::date_format7*/
	"mmm-yyyy",		/* 17 Cell::date_format22*/
	"mmmm-yy",		/* 18 Cell::date_format8*/
	"mmmm-yyyy",		/* 19 Cell::date_format9*/
	"m/d/yy h:mm",		/* 20 */
	"m/d/yyyy h:mm",	/* 21 */
	"yyyy/mm/d",		/* 22 Cell::date_format25*/
	"yyyy/mmm/d",		/* 23 Cell::date_format14*/
	"yyyy/mm/dd",		/* 24 Cell::date_format25*/
	"yyyy/mmm/dd",		/* 25 Cell::date_format26*/
	"yyyy-mm-d",		/* 26 Cell::date_format16*/
	"yyyy-mmm-d",		/* 27 Cell::date_format15*/
	"yyyy-mm-dd",		/* 28 Cell::date_format16*/
	"yyyy-mmm-dd",		/* 29 Cell::date_format15*/
	"yy",			/* 30 Cell::date_format24*/
	"yyyy",			/* 31 Cell::date_format23*/
	NULL
};

// copied from gnumeric: src/formats.c:
static char const * cell_format_time [] = {
  "h:mm AM/PM",    // Cell::Time_format1 9 : 01 AM
  "h:mm:ss AM/PM", // Cell::Time_format2 9:01:05 AM
  "h:mm",          // Cell::Time_format4 9:01
  "h:mm:ss",       // Cell::Time_format5 9:01:12
  "m/d/yy h:mm",
  "mm:ss",         // Cell::Time_format6 01:12
  "mm:ss.0",       // Cell::Time_format6 01:12
  "[h]:mm:ss",
  "[h]:mm",
  "[mm]:ss",
  "[ss]",
  NULL
};

namespace gnumeric_import_LNS
{
  QStringList list1;
  QStringList list2;
}

using namespace gnumeric_import_LNS;

void GNUMERICFilter::dateInit()
{
  // idea copied form gnumeric src/format.c:
  /* Day 1 means 1st of January of 1900 */
  g_dateOrigin = GnumericDate::greg2jul( 1900, 1, 1 ) - 1;
}

uint GNUMERICFilter::GnumericDate::greg2jul( int y, int m, int d )
{
  return QDate::gregorianToJulian( y, m, d );
}

void GNUMERICFilter::GnumericDate::jul2greg( double num, int & y, int & m, int & d )
{
  int i = (int) floor( num + HALF_SEC );
  if (i > g_dateSerial_19000228)
    --i;
  else if (i == g_dateSerial_19000228 + 1)
    kWarning(30521) << "Request for date 02/29/1900." << endl;

  kDebug(30521) << "***** Num: " << num << ", i: " << i  << endl;

  QDate::julianToGregorian( i + g_dateOrigin, y, m, d );
  kDebug(30521) << "y: " << y << ", m: " << m << ", d: " << d << endl;
}

QTime GNUMERICFilter::GnumericDate::getTime( double num )
{
  // idea copied from gnumeric: src/datetime.c
  num += HALF_SEC;
  int secs = qRound( (num - floor(num)) * SECS_PER_DAY );

  kDebug(30521) << "***** Num: " << num << ", secs " << secs << endl;

  const int h = secs / 3600;
  secs -= h * 3600;
  const int m = secs / 60;
  secs -= h * 60;

  kDebug(30521) << "****** h: " << h << ", m: " << m << ", secs: " << secs << endl;
  const QTime time( h, m, ( secs < 0 || secs > 59 ? 0 : secs ) );

  return time;
}

typedef KGenericFactory<GNUMERICFilter> GNUMERICFilterFactory;
K_EXPORT_COMPONENT_FACTORY( libgnumericimport, GNUMERICFilterFactory( "kofficefilters" ) )

GNUMERICFilter::GNUMERICFilter( QObject* parent, const QStringList & )
  : KoFilter(parent)
{
}

/* This converts GNUmeric's color string "0:0:0" to a QColor. */
void  convert_string_to_qcolor(QString color_string, QColor * color)
{
  int red, green, blue, first_col_pos, second_col_pos;

  bool number_ok;

  first_col_pos  = color_string.indexOf(":", 0);
  second_col_pos = color_string.indexOf(":", first_col_pos + 1);

  /* Fore="0:0:FF00" */
  /* If GNUmeric kicks out some invalid colors, we could crash. */
  /* GNUmeric gives us two bytes of color data per element. */
  /* We only care about the top byte. */

  red   = color_string.mid( 0, first_col_pos).toInt( &number_ok, 16 ) >> 8;
  green = color_string.mid(first_col_pos + 1,
                           (second_col_pos-first_col_pos) - 1).toInt( &number_ok, 16) >> 8;
  blue  = color_string.mid(second_col_pos + 1,
                           (color_string.length() - first_col_pos) - 1).toInt( &number_ok, 16) >> 8;
  color->setRgb(red, green, blue);
}

void areaNames( Doc * ksdoc, const QString &_name, QString _zone )
{
//Sheet2!$A$2:$D$8
    QString tableName;
    int pos = _zone.indexOf( '!' );
    if ( pos != -1 )
    {
        tableName = _zone.left( pos );
        _zone = _zone.right( _zone.length()-pos-1 );
         pos = _zone.indexOf( ':' );
        QRect rect;
        if ( pos != -1 )
        {
            QString left = _zone.mid( 1, pos-1 );
            QString right = _zone.mid( pos+2, _zone.length()-pos-2 );
            int pos = left.indexOf( '$' );

            rect.setLeft( util_decodeColumnLabelText(left.left(pos ) ) );
            rect.setTop( left.right( left.length()-pos-1 ).toInt() );

            pos = right.indexOf( '$' );
            rect.setRight( util_decodeColumnLabelText(right.left(pos ) ) );
            rect.setBottom( right.right( right.length()-pos-1 ).toInt() );
       }
        else
        {
            QString left = _zone;
            int pos = left.indexOf( '$' );
            int leftPos = util_decodeColumnLabelText(left.left(pos ) );
            rect.setLeft( leftPos );
            rect.setRight( leftPos );

            int top = left.right( left.length()-pos-1 ).toInt();
            rect.setTop( top );
            rect.setBottom( top );
        }
        ksdoc->addAreaName( rect, _name ,tableName);
    }
}


void set_document_area_names( Doc * ksdoc, QDomElement * docElem )
{
    QDomNode areaNamesElement = docElem->namedItem( "gmr:Names" );
    if ( areaNamesElement.isNull() )
        return;
    QDomNode areaNameItem = areaNamesElement.namedItem( "gmr:Name" );
    while ( !areaNameItem.isNull() )
    {
        QDomNode gmr_name  = areaNameItem.namedItem("gmr:name");
        QDomNode gmr_value = areaNameItem.namedItem("gmr:value");
        QString name = gmr_name.toElement().text();
        areaNames( ksdoc, name, gmr_value.toElement().text() );
        areaNameItem = areaNameItem.nextSibling();
    }
}



void set_document_attributes( Doc * ksdoc, QDomElement * docElem)
{
    ksdoc->loadConfigFromFile();
    QDomNode attributes  = docElem->namedItem("gmr:Attributes");
    if ( attributes.isNull() )
        return;

    QDomNode attributeItem = attributes.namedItem("gmr:Attribute");
    while( !attributeItem.isNull() )
    {
        QDomNode gmr_name  = attributeItem.namedItem("gmr:name");
        QDomNode gmr_value = attributeItem.namedItem("gmr:value");
        if (gmr_name.toElement().text() == "WorkbookView::show_horizontal_scrollbar")
        {
            ksdoc->setShowHorizontalScrollBar( gmr_value.toElement().text().toLower()=="true"? true : false );
        }
        else if ( gmr_name.toElement().text() == "WorkbookView::show_vertical_scrollbar")
        {
            ksdoc->setShowVerticalScrollBar( gmr_value.toElement().text().toLower()=="true"? true : false );
        }
        else if ( gmr_name.toElement().text() == "WorkbookView::show_notebook_tabs")
        {
            ksdoc->setShowTabBar(gmr_value.toElement().text().toLower()=="true"? true : false );
        }
        else if ( gmr_name.toElement().text() == "WorkbookView::do_auto_completion")
        {
            ksdoc->setCompletionMode( KGlobalSettings::CompletionAuto);
        }
        else if ( gmr_name.toElement().text() == "WorkbookView::is_protected")
        {
            //TODO protect document ???
            //ksdoc->map()->isProtected()
        }

        attributeItem = attributeItem.nextSibling();
    }
}

/* This sets the documentation information from the information stored in
   the GNUmeric file. Particularly in the "gmr:Summary" subcategory.
*/
void set_document_info(KoDocument * document, QDomElement * docElem)
{
  /* Summary Handling START */
  QDomNode summary  = docElem->namedItem("gmr:Summary");
  QDomNode gmr_item = summary.namedItem("gmr:Item");

  while( !gmr_item.isNull() )
  {
    QDomNode gmr_name  = gmr_item.namedItem("gmr:name");
    QDomNode gmr_value = gmr_item.namedItem("gmr:val-string");

    KoDocumentInfo * DocumentInfo     = document->documentInfo();

    if (gmr_name.toElement().text() == "title")
    {
      DocumentInfo->setAboutInfo("title", gmr_value.toElement().text());
    }
    else if (gmr_name.toElement().text() == "keywords")
    {
      DocumentInfo->setAboutInfo("keyword", gmr_value.toElement().text());
    }
    else if (gmr_name.toElement().text() == "comments")
    {
      DocumentInfo->setAboutInfo("comments", gmr_value.toElement().text());
    }
    else if (gmr_name.toElement().text() == "category")
    {
      /* Not supported by KSpread */
    }
    else if (gmr_name.toElement().text() == "manager")
    {
      /* Not supported by KSpread */
    }
    else if (gmr_name.toElement().text() == "application")
    {
      /* Not supported by KSpread */
    }
    else if (gmr_name.toElement().text() == "author")
    {
      DocumentInfo->setAuthorInfo("creator", gmr_value.toElement().text());
    }
    else if (gmr_name.toElement().text() == "company")
    {
      DocumentInfo->setAuthorInfo("company", gmr_value.toElement().text());
    }

    gmr_item = gmr_item.nextSibling();
  }

  /* Summany Handling STOP */
}


void setColInfo(QDomNode * sheet, Sheet * table)
{
  QDomNode columns =  sheet->namedItem("gmr:Cols");
  QDomNode columninfo = columns.namedItem("gmr:ColInfo");

  QDomElement def = columns.toElement();
  if ( def.hasAttribute( "DefaultSizePts" ) )
  {
    bool ok = false;
    double d = def.attribute( "DefaultSizePts" ).toDouble( &ok );
    if ( ok )
    {
      Format::setGlobalColWidth( d );
      table->setDefaultWidth( d );
    }
  }

  while( !columninfo.isNull() )
  {
    QDomElement e = columninfo.toElement(); // try to convert the node to an element.
    int column_number;

    column_number = e.attribute("No").toInt()+1;
    ColumnFormat *cl = new ColumnFormat(table, column_number);
    if (e.hasAttribute("Hidden"))
    {
      if (e.attribute("Hidden")=="1")
      {
        cl->setHide(true);
      }
    }
    if (e.hasAttribute("Unit"))
    {
      //  xmm = (x_points) * (1 inch / 72 points) * (25.4 mm/ 1 inch)
      cl->setDblWidth(e.attribute("Unit").toDouble());
      //cl->setWidth(e.attribute("Unit").toInt());
    }
    table->insertColumnFormat(cl);
    columninfo = columninfo.nextSibling();
  }
}

void setRowInfo(QDomNode *sheet, Sheet *table)
{
  QDomNode rows =  sheet->namedItem("gmr:Rows");
  QDomNode rowinfo = rows.namedItem("gmr:RowInfo");

  double d;
  bool ok = false;

  QDomElement def = rows.toElement();
  if ( def.hasAttribute( "DefaultSizePts" ) )
  {
    d = def.attribute( "DefaultSizePts" ).toDouble( &ok );
    if ( ok )
    {
      Format::setGlobalRowHeight( d );
      table->setDefaultHeight( d );
    }
  }

  while( !rowinfo.isNull() )
  {
    QDomElement e = rowinfo.toElement(); // try to convert the node to an element.
    int row_number;
    row_number = e.attribute("No").toInt() + 1;
    RowFormat *rl = new RowFormat(table, row_number);

    if (e.hasAttribute("Hidden"))
    {
      if (e.attribute("Hidden") == "1")
      {
        rl->setHide(true);
      }
    }
    if (e.hasAttribute("Unit"))
    {
      double dbl = e.attribute( "Unit" ).toDouble( &ok );
      if ( ok )
        rl->setDblHeight( dbl );
    }
    table->insertRowFormat(rl);
    rowinfo = rowinfo.nextSibling();
  }
}

void setSelectionInfo( QDomNode * sheet, Sheet * /* table */ )
{
  QDomNode selections =  sheet->namedItem("gmr:Selections");
  QDomNode selection = selections.namedItem("gmr:Selection");

  /* Kspread does not support mutiple selections.. */
  /* This code will set the selection to the last one GNUmeric's multiple
     selections. */
  while( !selection.isNull() )
  {
    QDomElement e = selection.toElement(); // try to convert the node to an element.
    QRect kspread_selection;

    kspread_selection.setLeft((e.attribute("startCol").toInt() + 1));
    kspread_selection.setTop((e.attribute("startRow").toInt() + 1));
    kspread_selection.setRight((e.attribute("endCol").toInt() + 1));
    kspread_selection.setBottom((e.attribute("endRow").toInt() + 1));

    /* can't set it in the table -- must set it to a view */
    //	  table->setSelection(kspread_selection);

    selection = selection.nextSibling();
  }
}


void setObjectInfo(QDomNode * sheet, Sheet * table)
{
  QDomNode gmr_objects =  sheet->namedItem("gmr:Objects");
  QDomNode gmr_cellcomment = gmr_objects.namedItem("gmr:CellComment");
  while( !gmr_cellcomment.isNull() )
  {
    QDomElement e = gmr_cellcomment.toElement(); // try to convert the node to an element.
    if (e.hasAttribute("Text"))
    {
      if (e.hasAttribute("ObjectBound"))
      {
        Point point(e.attribute("ObjectBound"));
        Cell * cell = table->nonDefaultCell( point.pos().x(), point.pos().y() );
        cell->format()->setComment(e.attribute("Text"));
      }
    }

    gmr_cellcomment  = gmr_cellcomment.nextSibling();
  }
}

void convertToPen( QPen & pen, int style )
{
  switch( style )
  {
   case 0:
    break;
   case 1:
    pen.setStyle( Qt::SolidLine );
    pen.setWidth( 1 );
    break;
   case 2:
    pen.setStyle( Qt::SolidLine );
    pen.setWidth( 2 );
    break;
   case 3:
    pen.setStyle( Qt::DashLine );
    pen.setWidth( 1 );
    break;
   case 4:
    // width should be 1 but otherwise it would be the same as 7
    pen.setStyle( Qt::DotLine );
    pen.setWidth( 2 );
    break;
   case 5:
    pen.setStyle( Qt::SolidLine );
    pen.setWidth( 3 );
    break;
   case 6:
    // TODO should be double
    pen.setStyle( Qt::SolidLine );
    pen.setWidth( 1 );
    break;
   case 7:
    // very thin dots => no match in KSpread
    pen.setStyle( Qt::DotLine );
    pen.setWidth( 1 );
    break;
   case 8:
    pen.setStyle( Qt::DashLine );
    pen.setWidth( 2 );
    break;
   case 9:
    pen.setStyle( Qt::DashDotLine );
    pen.setWidth( 1 );
    break;
   case 10:
    pen.setStyle( Qt::DashDotLine );
    pen.setWidth( 2 );
    break;
   case 11:
    pen.setStyle( Qt::DashDotDotLine );
    pen.setWidth( 1 );
    break;
   case 12:
    pen.setStyle( Qt::DashDotDotLine );
    pen.setWidth( 2 );
    break;
   case 13:
    // TODO: long dash, short dash, long dash,...
    pen.setStyle( Qt::DashDotLine );
    pen.setWidth( 3 );
    break;
   default:
    pen.setStyle( Qt::SolidLine );
    pen.setWidth( 1 );
  }
}

void GNUMERICFilter::ParseBorder( QDomElement & gmr_styleborder, Cell * kspread_cell )
{
  QDomNode gmr_diagonal = gmr_styleborder.namedItem("gmr:Diagonal");
  QDomNode gmr_rev_diagonal = gmr_styleborder.namedItem("gmr:Rev-Diagonal");
  QDomNode gmr_top = gmr_styleborder.namedItem("gmr:Top");
  QDomNode gmr_bottom = gmr_styleborder.namedItem("gmr:Bottom");
  QDomNode gmr_left = gmr_styleborder.namedItem("gmr:Left");
  QDomNode gmr_right = gmr_styleborder.namedItem("gmr:Right");

  // NoPen - no line at all. For example,
  // QPainter::drawRect() fills but does not
  // draw any explicit boundary
  // line. SolidLine - a simple line. DashLine
  // - dashes, separated by a few
  // pixels. DotLine - dots, separated by a
  // few pixels. DashDotLine - alternately
  // dots and dashes. DashDotDotLine - one dash, two dots, one dash, two dots...

  if ( !gmr_left.isNull() )
  {
    QDomElement e = gmr_left.toElement(); // try to convert the node to an element.
    importBorder( e, Left, kspread_cell);
   }

  if ( !gmr_right.isNull() )
  {
    QDomElement e = gmr_right.toElement(); // try to convert the node to an element.
    importBorder( e, Right, kspread_cell);
  }

  if ( !gmr_top.isNull() )
  {
    QDomElement e = gmr_top.toElement(); // try to convert the node to an element.
    importBorder( e, Top,  kspread_cell);
  }

  if ( !gmr_bottom.isNull() )
  {
    QDomElement e = gmr_bottom.toElement(); // try to convert the node to an element.
    importBorder( e, Bottom, kspread_cell);
  }

  if ( !gmr_diagonal.isNull() )
  {
    QDomElement e = gmr_diagonal.toElement(); // try to convert the node to an element.
    importBorder( e, Diagonal, kspread_cell);
  }

  if ( !gmr_rev_diagonal.isNull() )
  {
    QDomElement e = gmr_rev_diagonal.toElement(); // try to convert the node to an element.
    importBorder( e, Revdiagonal, kspread_cell);
  }

  //  QDomElement gmr_styleborder_element = gmr_styleborder.toElement();
}


void GNUMERICFilter::importBorder( QDomElement border, borderStyle _style,  Cell *cell)
{
    if ( !border.isNull() )
    {
        QDomElement e = border.toElement(); // try to convert the node to an element.
        if ( e.hasAttribute( "Style" ) )
        {
            int style = e.attribute( "Style" ).toInt();

            QPen pen;
            convertToPen( pen, style );

            if ( style > 0 )
            {
                switch( _style )
                {
                case Left:
                    cell->setLeftBorderPen( pen );
                    break;
                case Right:
                    cell->setRightBorderPen( pen );
                    break;
                case Top:
                    cell->setTopBorderPen( pen );
                    break;
                case Bottom:
                    cell->setBottomBorderPen( pen );
                    break;
                case Diagonal:
                    cell->format()->setFallDiagonalPen( pen ); // check if this is really Fall border
                    break;
                case Revdiagonal:
                    cell->format()->setGoUpDiagonalPen( pen ); // check if this is really GoUp
                    break;
                }
            }
            if ( e.hasAttribute( "Color" ) )
            {
                QColor color;
                QString colorString = e.attribute( "Color" );
                convert_string_to_qcolor( colorString, &color );
                {
                    switch( _style )
                    {
                    case Left:
                        cell->format()->setLeftBorderColor( color );
                        break;
                    case Right:
                        cell->format()->setRightBorderColor( color );
                        break;
                    case Top:
                        cell->format()->setTopBorderColor( color );
                        break;
                    case Bottom:
                        cell->format()->setBottomBorderColor( color );
                        break;
                    case Diagonal:
                        cell->format()->setFallDiagonalColor( color );
                        break;
                    case Revdiagonal:
                        cell->format()->setGoUpDiagonalPen( color );
                        break;
                    }
                }
            }
        }
    }

}

bool GNUMERICFilter::setType( Cell * kspread_cell,
                              QString const & formatString,
                              QString & cell_content )
{
  int i = 0;
  for ( i = 0; cell_format_date[i] ; ++i )
  {
    kDebug(30521) << "Cell_format: " << cell_format_date[i] << ", FormatString: " << formatString << endl;
    if ( ( formatString == "d/m/yy" ) || ( formatString == cell_format_date[i] ) )
    {
      kDebug(30521) << "   FormatString: Date: " << formatString << ", CellContent: " << cell_content << endl;
      QDate date;
      if ( !kspread_cell->isDate() )
      {
        // convert cell_content to date
        int y, m, d;
        bool ok = true;
        int val  = cell_content.toInt( &ok );

        kDebug(30521) << "!!!   FormatString: Date: " << formatString << ", CellContent: " << cell_content
                  << ", Double: " << val << endl;
        if ( !ok )
          return false;

        GnumericDate::jul2greg( val, y, m, d );
        kDebug(30521) << "     num: " << val << ", y: " << y << ", m: " << m << ", d: " << d << endl;

        date.setYMD( y, m, d );
      }
      else
        date = kspread_cell->value().asDate();

      FormatType type;
      switch( i )
      {
       case 0:  type = date_format5;  break;
       case 1:  type = date_format6;  break;
       case 2:  type = date_format1;  break;
       case 3:  type = date_format2;  break;
       case 4:  type = date_format3;  break;
       case 5:  type = date_format4;  break;
       case 6:  type = date_format11; break;
       case 7:  type = date_format12; break;
       case 8:  type = date_format19; break;
       case 9:  type = date_format18; break;
       case 10: type = date_format20; break;
       case 11: type = date_format21; break;
       case 16: type = date_format7;  break;
       case 17: type = date_format22; break;
       case 18: type = date_format8;  break;
       case 19: type = date_format9;  break;
       case 22: type = date_format25; break;
       case 23: type = date_format14; break;
       case 24: type = date_format25; break;
       case 25: type = date_format26; break;
       case 26: type = date_format16; break;
       case 27: type = date_format15; break;
       case 28: type = date_format16; break;
       case 29: type = date_format15; break;
       case 30: type = date_format24; break;
       case 31: type = date_format23; break;
       default:
        type = ShortDate_format;
        break;
        /* 12, 13, 14, 15, 20, 21 */
      }

      kDebug(30521) << "i: " << i << ", Type: " << type << ", Date: " << date.toString() << endl;

      kspread_cell->setValue( date );
      kspread_cell->format()->setFormatType( type );

      return true;
    }
  }

  for ( i = 0; cell_format_time[i] ; ++i )
  {
    if (formatString == cell_format_time[i])
    {
      QTime time;

      if ( !kspread_cell->isTime() )
      {
        bool ok = true;
        double content = cell_content.toDouble( &ok );

        kDebug(30521) << "   FormatString: Time: " << formatString << ", CellContent: " << cell_content
                  << ", Double: " << content << endl;

        if ( !ok )
          return false;

        time = GnumericDate::getTime( content );
      }
      else
        time = kspread_cell->value().asTime();

      FormatType type;
      switch( i )
      {
       case 0: type = Time_format1; break;
       case 1: type = Time_format2; break;
       case 2: type = Time_format4; break;
       case 3: type = Time_format5; break;
       case 5: type = Time_format6; break;
       case 6: type = Time_format6; break;
       default:
        type = Time_format1; break;
      }

      kDebug(30521) << "i: " << i << ", Type: " << type << endl;
      kspread_cell->setValue( time );
      kspread_cell->format()->setFormatType( type );

      return true;
    }
  }

  return false; // no date or time
}

QString GNUMERICFilter::convertVars( QString const & str, Sheet * table ) const
{
  QString result( str );
  uint count = list1.count();
  if ( count == 0 )
  {
    list1 << "&[TAB]" << "&[DATE]" << "&[PAGE]"
          << "&[PAGES]"<<"&[TIME]" << "&[FILE]";
    list2 << "<sheet>" << "<date>" << "<page>"
          << "<pages>" << "<time>" << "<file>";
    count = list1.count();
  }

  for ( uint i = 0; i < count; ++i )
  {
    int n = result.indexOf( list1[i] );

    if ( n != -1 )
    {
        kDebug(30521) << "Found var: " << list1[i] << endl;
      if ( i == 0 )
        result = result.replace( list1[i], table->sheetName() );
      else
        result = result.replace( list1[i], list2[i] );
    }
  }

  return result;
}

double GNUMERICFilter::parseAttribute( const QDomElement &_element )
{
    QString unit = _element.attribute( "PrefUnit" );
    bool ok;
    double value = _element.attribute("Points").toFloat( &ok );
    if ( !ok )
        value = 2.0;
    if ( unit == "mm" )
        return value;
    else if ( unit == "cm" )
        return ( value/10.0 );
    else if ( unit == "in" )
        return MM_TO_INCH( value );
    else if ( unit == "Pt" || unit == "Px" || unit == "points" )
        return MM_TO_POINT( value );
    else
        return value;
}

void GNUMERICFilter::ParsePrintInfo( QDomNode const & printInfo, Sheet * table )
{
  kDebug(30521) << "Parsing print info " << endl;

  float fleft = 2.0;
  float fright = 2.0;
  float ftop = 2.0;
  float fbottom = 2.0;

  QString paperSize("A4");
  QString orientation("Portrait");
  QString footLeft, footMiddle, footRight;
  QString headLeft, headMiddle, headRight; // no we are zombies :-)

  QDomNode margins( printInfo.namedItem("gmr:Margins") );
  if ( !margins.isNull() )
  {
    QDomElement top( margins.namedItem( "gmr:top" ).toElement() );
    if ( !top.isNull() )
        ftop = parseAttribute( top );

    QDomElement bottom( margins.namedItem( "gmr:bottom" ).toElement() );
    if ( !bottom.isNull() )
        fbottom= parseAttribute( bottom );

    QDomElement left( margins.namedItem( "gmr:left" ).toElement() );
    if ( !left.isNull() )
        fleft = parseAttribute( left );

    QDomElement right( margins.namedItem( "gmr:right" ).toElement() );
    if ( !right.isNull() )
        fright = parseAttribute( right );
  }

  QDomElement foot( printInfo.namedItem("gmr:Footer").toElement() );
  if ( !foot.isNull() )
  {
    kDebug(30521) << "Parsing footer: " << foot.attribute("Left") << ", " << foot.attribute("Middle") << ", "
              << foot.attribute("Right") << ", " <<endl;
    if ( foot.hasAttribute("Left") )
      footLeft = convertVars( foot.attribute("Left"), table );
    if ( foot.hasAttribute("Middle") )
      footMiddle = convertVars( foot.attribute("Middle"), table );
    if ( foot.hasAttribute("Right") )
      footRight = convertVars( foot.attribute("Right"), table );
  }

  QDomElement head( printInfo.namedItem("gmr:Header").toElement() );
  if ( !head.isNull() )
  {
    kDebug(30521) << "Parsing header: " << head.attribute("Left") << ", " << head.attribute("Middle") << ", " << head.attribute("Right") << ", "<< endl;
    if ( head.hasAttribute("Left") )
      headLeft = convertVars( head.attribute("Left"), table );
    if ( head.hasAttribute("Middle") )
      headMiddle = convertVars( head.attribute("Middle"), table );
    if ( head.hasAttribute("Right") )
      headRight = convertVars( head.attribute("Right"), table );
  }

  QDomElement repeateColumn( printInfo.namedItem("gmr:repeat_top").toElement() );
  if ( !repeateColumn.isNull() )
  {
      QString repeate = repeateColumn.attribute( "value" );
      if ( !repeate.isEmpty() )
      {
          Range range(repeate);
          //kDebug()<<" repeate :"<<repeate<<"range. ::start row : "<<range.startRow ()<<" start col :"<<range.startCol ()<<" end row :"<<range.endRow ()<<" end col :"<<range.endCol ()<<endl;
          table->print()->setPrintRepeatRows( qMakePair( range.startRow (),range.endRow ()) );
      }
  }

  QDomElement repeateRow( printInfo.namedItem("gmr:repeat_left").toElement() );
  if ( !repeateRow.isNull() )
  {
      QString repeate = repeateRow.attribute( "value" );
      if ( !repeate.isEmpty() )
      {
          //fix row too high
          repeate = repeate.replace( "65536", "32500" );
          Range range(repeate);
          //kDebug()<<" repeate :"<<repeate<<"range. ::start row : "<<range.startRow ()<<" start col :"<<range.startCol ()<<" end row :"<<range.endRow ()<<" end col :"<<range.endCol ()<<endl;
          table->print()->setPrintRepeatColumns( qMakePair( range.startCol (),range.endCol ()) );
      }
  }

  QDomElement orient( printInfo.namedItem("gmr:orientation").toElement() );
  if ( !orient.isNull() )
    orientation = orient.text();

  QDomElement size( printInfo.namedItem("gmr:paper").toElement() );
  if ( !size.isNull() )
    paperSize = size.text();

  table->print()->setPaperLayout( fleft, ftop, fright, fbottom,
                         paperSize, orientation );

  table->print()->setHeadFootLine( headLeft, headMiddle, headRight,
                                   footLeft, footMiddle, footRight );
}

void GNUMERICFilter::ParseFormat(QString const & formatString, Cell * kspread_cell)
{
  int l = formatString.length();
  int lastPos = 0;

  if (formatString[l - 1] == '%')
    kspread_cell->format()->setFormatType(Percentage_format);
  else if (formatString[0] == '$')
  {
    kspread_cell->format()->setFormatType(Money_format);
    kspread_cell->format()->setCurrency( 1, "$" );
    lastPos = 1;
  }
  else if (formatString[0] == '£')
  {
    kspread_cell->format()->setFormatType(Money_format);
    kspread_cell->format()->setCurrency( 1, "£" );
    lastPos = 1;
  }
  else if (formatString[0] == '¥')
  {
    kspread_cell->format()->setFormatType(Money_format);
    kspread_cell->format()->setCurrency( 1, "¥" );
    lastPos = 1;
  }
  else if (formatString[0] == '¤')
  {
    kspread_cell->format()->setFormatType(Money_format);
    kspread_cell->format()->setCurrency( 1, "¤" );
    lastPos = 1;
  }
  else if (l > 1)
  {
    if ((formatString[0] == '[') && (formatString[1] == '$'))
    {
      int n = formatString.indexOf(']');
      if (n != -1)
      {
        QString currency = formatString.mid(2, n - 2);
        kspread_cell->format()->setFormatType(Money_format);
        kspread_cell->format()->setCurrency( 1, currency );
      }
      lastPos = ++n;
    }
    else if (formatString.indexOf("E+0") != -1)
    {
      kspread_cell->format()->setFormatType(Scientific_format);
    }
    else
    {
      // do pattern matching with gnumeric formats
      QString content(kspread_cell->value().asString());

      if ( setType(kspread_cell, formatString, content) )
        return;

      if (formatString.indexOf("?/?") != -1)
      {
        // TODO: fixme!
        kspread_cell->format()->setFormatType( fraction_three_digits );
        return;
      }
      // so it's nothing we want to understand:-)
      return;
    }
  }

  while (formatString[lastPos] == ' ')
    ++lastPos;

  // GetPrecision and decimal point, format of negative items...

  // thousands separator
  if (formatString[lastPos] == '#')
  {
    bool sep = true;
    if (formatString[lastPos + 1] == ',')
      lastPos += 2;
    else
      sep = false;
    // since KSpread 1.3
    // kspread_cell->setThousandsSeparator( sep );
  }

  while (formatString[lastPos] == ' ')
    ++lastPos;

  int n = formatString.indexOf( '.', lastPos );
  if ( n != -1)
  {
    lastPos = n + 1;
    int precision = lastPos;
    while (formatString[precision] == '0')
      ++precision;

    int tmp = lastPos;
    lastPos = precision;
    precision -= tmp;

    kspread_cell->format()->setPrecision( precision );
  }

  bool red = false;
  if (formatString.indexOf("[RED]", lastPos) != -1)
  {
    red = true;
    kspread_cell->format()->setFloatColor( Style::NegRed );
  }
  if ( formatString.indexOf('(', lastPos) != -1 )
  {
    if ( red )
      kspread_cell->format()->setFloatColor( Style::NegRedBrackets );
    else
      kspread_cell->format()->setFloatColor( Style::NegBrackets );
  }
}

void GNUMERICFilter::convertFormula( QString & formula ) const
{
  int n = formula.indexOf( '=', 1 );

  // TODO: check if we do not screw something up here...
  if ( n != -1 )
    formula = formula.replace( n, 1, "==" );

  bool inQuote1 = false;
  bool inQuote2 = false;
  int l = formula.length();
  for ( int i = 0; i < l; ++i )
  {
    if ( formula[i] == '\'' )
      inQuote1 = !inQuote1;
    else if ( formula[i] == '"' )
      inQuote2 = !inQuote2;
    else if ( formula[i] == ',' && !inQuote1 && !inQuote2 )
      formula = formula.replace( i, 1, ";" );
  }
}

void GNUMERICFilter::setStyleInfo(QDomNode * sheet, Sheet * table)
{
    kDebug(30521) << "SetStyleInfo entered " << endl;

    int row, column;
    QDomNode styles =  sheet->namedItem( "gmr:Styles" );
    if ( !styles.isNull() )
    {
        // Get a style region within that sheet.
        QDomNode style_region =  styles.namedItem( "gmr:StyleRegion" );

        while ( !style_region.isNull() )
        {
            QDomElement e = style_region.toElement(); // try to convert the node to an element.

            QDomNode style = style_region.namedItem( "gmr:Style" );
            QDomNode font = style.namedItem( "gmr:Font" );
            QDomNode validation = style.namedItem( "gmr:Validation" );
            QDomNode gmr_styleborder = style.namedItem( "gmr:StyleBorder" );
            QDomNode hyperlink = style.namedItem( "gmr:HyperLink" );
            int startCol = e.attribute( "startCol" ).toInt() + 1;
            int endCol   = e.attribute( "endCol" ).toInt() + 1;
            int startRow = e.attribute( "startRow" ).toInt() + 1;
            int endRow   = e.attribute( "endRow" ).toInt() + 1;

            kDebug(30521) << "------Style: " << startCol << ", "
                           << startRow << " - " << endCol << ", " << endRow << endl;

            if ( endCol - startCol > 200 || endRow - startRow > 200 )
            {
                style_region = style_region.nextSibling();
                continue;
            }

            for ( column = startCol; column <= endCol; ++column )
            {
                for ( row = startRow; row <= endRow; ++row )
                {
                    kDebug(30521) << "Cell: " << column << ", " << row << endl;
                    Cell * kspread_cell = table->cellAt( column, row, false );

                    // don't create new cells -> don't apply format on empty cells, if bigger region
                    if ( ( kspread_cell->isDefault() || kspread_cell->isEmpty() )
                         && ( ( endCol - startCol > 2 ) || ( endRow - startRow > 2 ) ) )
                    {
                        kDebug(30521) << "CELL EMPTY OR RANGE TOO BIG " << endl;
                        continue;
                    }

                    QDomElement style_element = style.toElement(); // try to convert the node to an element.

                    kDebug(30521) << "Style valid for kspread" << endl;
                    kspread_cell = table->nonDefaultCell( column, row );

                    if (style_element.hasAttribute("Fore"))
                    {
                        QString color_string = style_element.attribute("Fore");
                        QColor color;
                        convert_string_to_qcolor(color_string, &color);
                        kspread_cell->format()->setTextColor(color);
                    }

                    if (style_element.hasAttribute("Back"))
                    {
                        QString color_string = style_element.attribute("Back");
                        QColor color;
                        convert_string_to_qcolor(color_string, &color);
                        kspread_cell->format()->setBgColor(color);
                    }

                    if (style_element.hasAttribute("PatternColor"))
                    {
                        QString color_string = style_element.attribute("PatternColor");
                        QColor color;
                        convert_string_to_qcolor(color_string, &color);
                        kspread_cell->format()->setBackGroundBrushColor( color );
                    }

                    if (style_element.hasAttribute("Shade"))
                    {
                        /* Pattern's taken from: gnumeric's pattern.c */
                        /* if "TODO" added: doesn't match exactly the gnumeric one */

                        QString shade = style_element.attribute("Shade");
                        if (shade == "0")
                        {
                            // nothing to do
                        }
                        else if (shade == "1")
                        {
                            /* 1 Solid */
                            //kspread_cell->format()->setBackGroundBrushStyle(Qt::SolidPattern);
                            //This is as empty
                            /* What should this be? */

                        }
                        else if (shade == "2")
                        {
                            /* 2 75% */
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::Dense2Pattern);
                        }
                        else if (shade == "3")
                        {
                            /* 3 50% */
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::Dense4Pattern);
                        }
                        else if (shade == "4")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::Dense5Pattern);
                            /* This should be 25%... All qt has is 37% */

                            /* 4 25% */
                        }
                        else if (shade == "5")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::Dense6Pattern);
                            /* 5 12.5% */
                        }
                        else if (shade == "6")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::Dense7Pattern);
                            /* 6 6.25% */

                        }
                        else if (shade == "7")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::HorPattern);
                            /* 7 Horizontal Stripe */
                        }
                        else if (shade == "8")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::VerPattern);
                            /* 8 Vertical Stripe */
                        }
                        else if (shade == "9")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::BDiagPattern);
                            /* 9 Reverse Diagonal Stripe */
                        }
                        else if (shade == "10")
                        {
                            /* 10 Diagonal Stripe */
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::FDiagPattern);
                        }
                        else if (shade == "11")
                        {
                            /* 11 Diagonal Crosshatch */
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::DiagCrossPattern);
                        }
                        else if (shade == "12")
                        {
                            /* 12 Thick Diagonal Crosshatch TODO!*/
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::DiagCrossPattern);
                        }
                        else if (shade == "13")
                        {
                            /* 13 Thin Horizontal Stripe TODO: wrong: this is thick!*/
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::HorPattern);
                        }
                        else if (shade == "14")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::VerPattern);
                        }
                        else if (shade == "15")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::FDiagPattern);
                        }
                        else if (shade == "16")
                        {
                            /* 16 Thick Reverse Stripe TODO:*/
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::BDiagPattern);
                        }
                        else if (shade == "17")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::DiagCrossPattern);
                        }
                        else if (shade == "18")
                        {
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::DiagCrossPattern);
                        }
                        else if (shade == "19")
                        {
                            /* 19 Applix small circle */
                        }
                        else if (shade == "20")
                        {
                            /* 20 Applix semicircle */
                        }
                        else if (shade == "21")
                        {
                            /* 21 Applix small thatch */
                        }
                        else if (shade == "22")
                        {
                            /* 22 Applix round thatch */
                        }
                        else if (shade == "23")
                        {
                            /* 23 Applix Brick */
                        }
                        else if (shade == "24")
                        {
                            /* 24 100% */
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::SolidPattern);
                        }
                        else if (shade == "25")
                        {
                            /* 25 87.5% */
                            kspread_cell->format()->setBackGroundBrushStyle(Qt::Dense2Pattern);
                        }
                    }

                    if ( style_element.hasAttribute( "Rotation" ) )
                    {
                        int rot = style_element.attribute( "Rotation" ).toInt();
                        kspread_cell->format()->setAngle( -1* rot );
                    }
                    if (style_element.hasAttribute("Indent"))
                    {
                        double indent = style_element.attribute("Indent").toDouble();
                        // gnumeric saves indent in characters, we in points:
                        kspread_cell->format()->setIndent( indent * 10.0 );
                    }

                    if (style_element.hasAttribute("HAlign"))
                    {
                        QString halign_string=style_element.attribute("HAlign");

                        if (halign_string == "1")
                        {
                            /* General: No equivalent in Kspread. */
                        }
                        else if (halign_string == "2")
                        {
                            kspread_cell->format()->setAlign(Style::Left);
                        }
                        else if (halign_string == "4")
                        {
                            kspread_cell->format()->setAlign(Style::Right);
                        }
                        else if (halign_string == "8")
                        {
                            kspread_cell->format()->setAlign(Style::Center);
                        }
                        else if (halign_string == "16")
                        {
                            /* Fill: No equivalent in Kspread. */
                        }
                        else if (halign_string == "32")
                        {
                            /* Justify: No equivalent in Kspread */
                        }
                        else if (halign_string == "64")
                        {
                            /* Centered across selection*/
                        }

                    }

                    if (style_element.hasAttribute("VAlign"))
                    {
                        QString valign_string=style_element.attribute("VAlign");

                        if (valign_string == "1")
                        {
                            /* General: No equivalent in Kspread. */
                            kspread_cell->format()->setAlignY(Style::Top);
                        }
                        else if (valign_string == "2")
                        {
                            kspread_cell->format()->setAlignY(Style::Bottom);
                        }
                        else if (valign_string == "4")
                        {
                            kspread_cell->format()->setAlignY(Style::Middle);
                        }
                        else if (valign_string == "8")
                        {
                            /* Justify: No equivalent in Kspread */
                        }
                    }

                    if (style_element.hasAttribute("WrapText"))
                    {
                        QString multiRow = style_element.attribute("WrapText");

                        if ( multiRow == "1" )
                            kspread_cell->format()->setMultiRow( true );
                    }

                    if (style_element.hasAttribute("Format"))
                    {
                        QString formatString = style_element.attribute("Format");

                        kDebug(30521) << "Format: " << formatString << endl;
                        ParseFormat(formatString, kspread_cell);

                    } // End "Format"

                    if (!gmr_styleborder.isNull())
                    {
                        QDomElement style_element = gmr_styleborder.toElement(); // try to convert the node to an element.
                        ParseBorder( style_element, kspread_cell );
                    }
                    if ( !validation.isNull() )
                    {
                        QDomElement validation_element = validation.toElement();
                        if ( !validation_element.isNull() )
                        {
                            kDebug(30521)<<" Cell validation \n";
                            Validity* kspread_validity = kspread_cell->validity(true);
                            if ( validation_element.hasAttribute( "AllowBlank" ) && validation_element.attribute( "AllowBlank" )=="true" )
                            {
                                kspread_validity->allowEmptyCell=true;
                            }
                            if ( validation_element.hasAttribute( "Title" ))
                            {
                                kspread_validity->title=validation_element.attribute( "Title" );
                            }
                            if ( validation_element.hasAttribute( "Message" ))
                            {
                                kspread_validity->message=validation_element.attribute( "Message" );
                            }
                            if ( validation_element.hasAttribute( "Style" ) )
                            {
                                int value = validation_element.attribute( "Style" ).toInt();
                                switch( value )
                                {
                                case 0:
                                    kspread_validity->displayMessage=false;
                                    break;
                                case 1:
                                  kspread_validity->m_action=Action::Stop;
                                    break;
                                case 2:
                                  kspread_validity->m_action=Action::Warning;
                                    break;
                                case 3:
                                  kspread_validity->m_action=Action::Information;
                                    break;
                                default:
                                    kDebug()<<" Error in validation style :"<<value<<endl;
                                    break;
                                }
                            }
                            QDomNode expression0 = validation_element.namedItem( "gmr:Expression0" );
                            QDomNode expression1 = validation_element.namedItem( "gmr:Expression1" );
                            //kDebug()<<" expression0.isNull() "<<expression0.isNull()<<endl;
                            //kDebug()<<" expression1.isNull() "<<expression1.isNull()<<endl;
                            if ( validation_element.hasAttribute( "Type" ) )
                            {
                                int valueOp = validation_element.attribute( "Type" ).toInt();
                                switch( valueOp )
                                {
                                case 0:
                                    kspread_validity->m_restriction=Restriction::None;
                                    break;
                                case 1:
                                {
                                    kspread_validity->m_restriction=Restriction::Integer;
                                    if ( validation_element.hasAttribute( "Operator" ) )
                                    {
                                        int value = validation_element.attribute( "Operator" ).toInt();

                                        switch( value )
                                        {
                                        case 0:
                                          kspread_validity->m_cond=Conditional::Between;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            if ( !expression1.isNull() )
                                                kspread_validity->valMax=expression1.toElement().text().toInt();
                                            break;
                                        case 1:
                                          kspread_validity->m_cond=Conditional::DifferentTo;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            if ( !expression1.isNull() )
                                                kspread_validity->valMax=expression1.toElement().text().toInt();
                                            break;
                                        case 2:
                                          kspread_validity->m_cond=Conditional::Equal;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 3:
                                          kspread_validity->m_cond=Conditional::Different;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 4:
                                          kspread_validity->m_cond=Conditional::Superior;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 5:
                                          kspread_validity->m_cond=Conditional::Inferior;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 6:
                                          kspread_validity->m_cond=Conditional::SuperiorEqual;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 7:
                                          kspread_validity->m_cond=Conditional::InferiorEqual;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        default:
                                            kDebug()<<" Error in validation Operator :"<<value<<endl;
                                            break;
                                        }
                                    }
                                }
                                break;
                                case 2:
                                    kspread_validity->m_restriction=Restriction::Number;
                                    if ( validation_element.hasAttribute( "Operator" ) )
                                    {
                                        int value = validation_element.attribute( "Operator" ).toInt();
                                        switch( value )
                                        {
                                        case 0:
                                          kspread_validity->m_cond=Conditional::Between;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            if ( !expression1.isNull() )
                                                kspread_validity->valMax=expression1.toElement().text().toInt();
                                            break;
                                        case 1:
                                          kspread_validity->m_cond=Conditional::DifferentTo;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            if ( !expression1.isNull() )
                                                kspread_validity->valMax=expression1.toElement().text().toInt();
                                            break;
                                        case 2:
                                          kspread_validity->m_cond=Conditional::Equal;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 3:
                                          kspread_validity->m_cond=Conditional::Different;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 4:
                                          kspread_validity->m_cond=Conditional::Superior;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 5:
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            kspread_validity->m_cond=Conditional::Inferior;
                                            break;
                                        case 6:
                                          kspread_validity->m_cond=Conditional::SuperiorEqual;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 7:
                                          kspread_validity->m_cond=Conditional::InferiorEqual;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        default:
                                            kDebug()<<" Error in validation Operator :"<<value<<endl;
                                            break;
                                        }
                                    }
                                    break;
                                case 3:
                                    kspread_validity->m_restriction=Restriction::List;
                                    break;
                                case 4:
                                    kspread_validity->m_restriction=Restriction::Date;
                                    if ( validation_element.hasAttribute( "Operator" ) )
                                    {
                                        int value = validation_element.attribute( "Operator" ).toInt();
                                        switch( value )
                                        {
                                        case 0:
                                          kspread_validity->m_cond=Conditional::Between;
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            if ( !expression1.isNull() )
                                                kspread_validity->dateMax=QDate::fromString( expression1.toElement().text() );

                                            break;
                                        case 1:
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            if ( !expression1.isNull() )
                                                kspread_validity->dateMax=QDate::fromString( expression1.toElement().text() );
                                            kspread_validity->m_cond=Conditional::DifferentTo;
                                            break;
                                        case 2:
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::Equal;
                                            break;
                                        case 3:
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::Different;
                                            break;
                                        case 4:
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::Superior;
                                            break;
                                        case 5:
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::Inferior;
                                            break;
                                        case 6:
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::SuperiorEqual;
                                            break;
                                        case 7:
                                            if ( !expression0.isNull() )
                                                kspread_validity->dateMin=QDate::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::InferiorEqual;
                                            break;
                                        default:
                                            kDebug()<<" Error in validation Operator :"<<value<<endl;
                                            break;
                                        }
                                    }
                                    break;
                                case 5:
                                    kspread_validity->m_restriction=Restriction::Time;
                                    if ( validation_element.hasAttribute( "Operator" ) )
                                    {
                                        int value = validation_element.attribute( "Operator" ).toInt();
                                        switch( value )
                                        {
                                        case 0:
                                          kspread_validity->m_cond=Conditional::Between;
                                            if ( !expression0.isNull() )
                                                kspread_validity->timeMin=QTime::fromString( expression0.toElement().text() );
                                            if ( !expression1.isNull() )
                                                kspread_validity->timeMax=QTime::fromString( expression1.toElement().text() );
                                            break;
                                        case 1:
                                            if ( !expression0.isNull() )
                                                kspread_validity->timeMin=QTime::fromString( expression0.toElement().text() );
                                            if ( !expression1.isNull() )
                                                kspread_validity->timeMax=QTime::fromString( expression1.toElement().text() );
                                            kspread_validity->m_cond=Conditional::DifferentTo;
                                            break;
                                        case 2:
                                            if ( !expression0.isNull() )
                                                kspread_validity->timeMin=QTime::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::Equal;
                                            break;
                                        case 3:
                                          kspread_validity->m_cond=Conditional::Different;
                                            break;
                                        case 4:
                                            if ( !expression0.isNull() )
                                                kspread_validity->timeMin=QTime::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::Superior;
                                            break;
                                        case 5:
                                          kspread_validity->m_cond=Conditional::Inferior;
                                            if ( !expression0.isNull() )
                                                kspread_validity->timeMin=QTime::fromString( expression0.toElement().text() );
                                            break;
                                        case 6:
                                          kspread_validity->m_cond=Conditional::SuperiorEqual;
                                            if ( !expression0.isNull() )
                                                kspread_validity->timeMin=QTime::fromString( expression0.toElement().text() );
                                            break;
                                        case 7:
                                            if ( !expression0.isNull() )
                                                kspread_validity->timeMin=QTime::fromString( expression0.toElement().text() );
                                            kspread_validity->m_cond=Conditional::InferiorEqual;
                                            break;
                                        default:
                                            kDebug()<<" Error in validation Operator :"<<value<<endl;
                                            break;
                                        }
                                    }
                                    break;
                                case 6:
                                    kspread_validity->m_restriction=Restriction::TextLength;
                                    if ( validation_element.hasAttribute( "Operator" ) )
                                    {
                                        int value = validation_element.attribute( "Operator" ).toInt();
                                        switch( value )
                                        {
                                        case 0:
                                          kspread_validity->m_cond=Conditional::Between;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            if ( !expression1.isNull() )
                                                kspread_validity->valMax=expression1.toElement().text().toInt();
                                            break;
                                        case 1:
                                          kspread_validity->m_cond=Conditional::DifferentTo;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            if ( !expression1.isNull() )
                                                kspread_validity->valMax=expression1.toElement().text().toInt();
                                            break;
                                        case 2:
                                          kspread_validity->m_cond=Conditional::Equal;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 3:
                                          kspread_validity->m_cond=Conditional::Different;
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            break;
                                        case 4:
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            kspread_validity->m_cond=Conditional::Superior;
                                            break;
                                        case 5:
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            kspread_validity->m_cond=Conditional::Inferior;
                                            break;
                                        case 6:
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            kspread_validity->m_cond=Conditional::SuperiorEqual;
                                            break;
                                        case 7:
                                            if ( !expression0.isNull() )
                                                kspread_validity->valMin=expression0.toElement().text().toInt();
                                            kspread_validity->m_cond=Conditional::InferiorEqual;
                                            break;
                                        default:
                                            kDebug()<<" Error in validation Operator :"<<value<<endl;
                                            break;
                                        }
                                    }
                                    break;
                                default:
                                    kDebug()<<" Error in Type element : "<<valueOp<<endl;
                                }

                            }
                            //<gmr:Validation Style="0" Type="1" Operator="0" AllowBlank="true" UseDropdown="false">
                            //<gmr:Expression0>745</gmr:Expression0>
                            //<gmr:Expression1>4546</gmr:Expression1>
                        }
                    }
                    if (!font.isNull())
                    {
                        QDomElement font_element = font.toElement();

                        kspread_cell->format()->setTextFontFamily( font_element.text() );

                        if (!font_element.isNull())
                        {
                            if (font_element.attribute("Italic") == "1")
                            { kspread_cell->format()->setTextFontItalic(true); }

                            if (font_element.attribute("Bold") == "1")
                            { kspread_cell->format()->setTextFontBold(true); }

                            if (font_element.hasAttribute("Underline") && ( font_element.attribute("Underline") != "0") )
                            { kspread_cell->format()->setTextFontUnderline(true); }

                            if (font_element.hasAttribute("StrikeThrough" ) && ( font_element.attribute("StrikeThrough") != "0") )
                            { kspread_cell->format()->setTextFontStrike(true); }

                            if (font_element.hasAttribute("Unit"))
                            { kspread_cell->format()->setTextFontSize(font_element.attribute("Unit").toInt()); }

                        }
                        if ( !hyperlink.isNull() )
                        {
                            //<gmr:HyperLink type="GnmHLinkURL" target="www.kde.org"/>
                            if ( hyperlink.toElement().hasAttribute( "type" ) )
                            {
                                QString linkType= hyperlink.toElement().attribute( "type" );
                                QString target = hyperlink.toElement().attribute( "target" );
                                QString tip = hyperlink.toElement().attribute( "tip" );
                                if ( !tip.isEmpty() )
                                    kspread_cell->setCellText( tip );
                                if ( linkType=="GnmHLinkURL" )
                                {
                                    if ( !target.startsWith( "http://" ) )
                                        target="http://"+target;
                                    kspread_cell->setLink( target );
                                }
                                else if ( linkType=="GnmHLinkEMail" )
                                {
                                    if ( !target.startsWith( "mailto:/" ) )
                                        target="mailto:/"+target;
                                    kspread_cell->setLink( target );
                                }
                                else if ( linkType=="GnmHLinkExternal" )
                                {
                                    if ( !target.startsWith( "file://" ) )
                                        target="file://"+target;

                                    kspread_cell->setLink( target );
                                }
                                else if ( linkType=="GnmHLinkCurWB" )
                                {
                                    kspread_cell->setLink( target );
                                }
                                else
                                    kDebug()<<" linkType not defined : "<<linkType<<endl;
                            }
                        }
                    }
                }
            }
            style_region = style_region.nextSibling();
        }

    }
}

/* NOTE: As of now everything is in a single huge function.  This is
	 very ugly.  It should all be broken up into smaller
	 functions, probably one for each GNUMeric section.  It kind
	 of grew out of control.  It could probably be cleaned up in
	 an hour or so. --PGE
  */


KoFilter::ConversionStatus GNUMERICFilter::convert( const QByteArray & from, const QByteArray & to )
{
    dateInit();
    bool bSuccess=true;

    kDebug(30521) << "Entering GNUmeric Import filter." << endl;

    KoDocument * document = m_chain->outputDocument();
    if ( !document )
        return KoFilter::StupidError;

    kDebug(30521) << "here we go... " << document->metaObject()->className() << endl;

    if ( !qobject_cast<const KSpread::Doc *>( document ) )  // it's safer that way :)
    {
      kWarning(30521) << "document isn't a KSpread::Doc but a " << document->metaObject()->className() << endl;
        return KoFilter::NotImplemented;
    }
    if ( from != "application/x-gnumeric" || to != "application/x-kspread" )
    {
        kWarning(30521) << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    kDebug(30521) << "...still here..." << endl;

    // No need for a dynamic cast here, since we use Qt's moc magic
    Doc * ksdoc = ( Doc * ) document;

    if ( ksdoc->mimeType() != "application/x-kspread" )
    {
        kWarning(30521) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return KoFilter::NotImplemented;
    }


    QIODevice* in = KFilterDev::deviceForFile(m_chain->inputFile(),"application/x-gzip");

    if ( !in )
    {
        kError(30521) << "Cannot create device for uncompressing! Aborting!" << endl;
        return KoFilter::FileNotFound;
    }

    if (!in->open(QIODevice::ReadOnly))
    {
        kError(30521) << "Cannot open file for uncompressing! Aborting!" << endl;
        delete in;
        return KoFilter::FileNotFound;
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !doc.setContent(in, &errorMsg, &errorLine, &errorColumn) )
    {
        kError(30521) << "Parsing error in " << from << "! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
        in->close();
        return KoFilter::ParsingError;
    }

    in->close();
    delete in;

    int row, column;
    int value = 0;
    int currentTab = -1;
    int selectedTab = 0;
    Sheet * selTable = 0;

    QDomElement docElem = doc.documentElement();
    QDomElement uiData  = docElem.namedItem("gmr:UIData").toElement();
    if ( !uiData.isNull() )
    {
      if ( uiData.hasAttribute( "SelectedTab" ) )
      {
        bool ok = false;
        int n = uiData.attribute( "SelectedTab" ).toInt( &ok );
        if ( ok )
        {
          selectedTab = n;
        }
      }
    }
    QDomNode sheets = docElem.namedItem("gmr:Sheets");
    if ( sheets.isNull() )
    {
        //avoid crash with new file format.
        //TODO allow to load new file format
        return KoFilter::ParsingError;
    }
    QDomNode sheet =  sheets.namedItem("gmr:Sheet");

    /* This sets the Document information. */
    set_document_info( document, &docElem );

    /* This sets the Document attributes */
    set_document_attributes( ksdoc, &docElem );

    /* This sets the Area Names */
    set_document_area_names( ksdoc, &docElem );

    Sheet * table;

    // This is a mapping of exprID to expressions.

    Q3Dict<char> exprID_dict( 17, FALSE );
    int num = 1;

    while (!sheet.isNull())
    {
        ++currentTab;
        table = ksdoc->map()->addNewSheet();

        if ( currentTab == selectedTab )
          selTable = table;

        QDomElement name = sheet.namedItem( "gmr:Name" ).toElement();
        QDomElement sheetElement = sheet.toElement();

        if ( !name.isNull() )
          table->setSheetName( name.text(), false, false );
        else
          table->setSheetName( "Sheet" + QString::number( num ), false, false );
        table->enableScrollBarUpdates( false );

        //kDebug()<<" sheetElement.hasAttribute( DisplayFormulas ) :"<<sheetElement.hasAttribute( "DisplayFormulas" )<<endl;
        QString tmp;
        if ( sheetElement.hasAttribute( "DisplayFormulas" ) )
        {
            tmp=sheetElement.attribute( "DisplayFormulas");
            table->setShowFormula( ( tmp=="true" )||( tmp=="1" ) );
        }
        if ( sheetElement.hasAttribute( "HideZero" ) )
        {
            tmp = sheetElement.attribute( "HideZero" );
            table->setHideZero( ( tmp=="true" )||( tmp=="1" ) );
        }
        if ( sheetElement.hasAttribute( "HideGrid" ) )
        {
            tmp = sheetElement.attribute( "HideGrid" );
            table->setShowGrid( ( tmp=="false" )||( tmp=="0" ) );
        }
        if ( sheetElement.hasAttribute( "HideColHeader" ) )
        {
            tmp = sheetElement.attribute( "HideColHeader" );
            ksdoc->setShowColumnHeader( ( tmp=="false" )||( tmp=="0" ) );
        }
        if ( sheetElement.hasAttribute( "HideRowHeader" ) )
        {
            tmp =sheetElement.attribute( "HideRowHeader" );
            ksdoc->setShowRowHeader( ( tmp=="false" )||( tmp=="0" ) );
        }


	setObjectInfo(&sheet, table);
	setColInfo(&sheet, table);
	setRowInfo(&sheet, table);
	setSelectionInfo(&sheet, table);

        /* handling print information */
	QDomNode printInfo = sheet.namedItem("gmr:PrintInformation");
        if ( !printInfo.isNull() )
          ParsePrintInfo( printInfo, table );

        kDebug(30521) << "Reading in cells" << endl;

	/* CELL handling START */
	QDomNode cells = sheet.namedItem( "gmr:Cells" );
	QDomNode cell  = cells.namedItem( "gmr:Cell" );
        QDomNode mergedCells = sheet.namedItem( "gmr:MergedRegions" );
        QDomNode mergedRegion = mergedCells.namedItem( "gmr:Merge" );
        if ( cell.isNull() )
        {
          kWarning(30521) << "No cells" << endl;
        }

	while ( !cell.isNull() )
        {
	  value += 2;
	  emit sigProgress(value);

	  QDomElement e = cell.toElement(); // try to convert the node to an element.
	  if ( !e.isNull() )
          { // the node was really an element.
            kDebug(30521) << "New Cell " << endl;
	    QDomNode content_node = cell.namedItem("gmr:Content");

	    if (!content_node.isNull())
            {
	      QDomElement content = content_node.toElement();

	      if( !content.isNull() )
              { // the node was really an element.
		column = e.attribute( "Col" ).toInt() + 1;
		row    = e.attribute( "Row" ).toInt() + 1;

		QString cell_content( content.text() );
                //kDebug()<<"cell_content :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :"<<cell_content<<endl;
                if ( cell_content[0] == '=' )
                  convertFormula( cell_content );

                Cell * kspread_cell = table->nonDefaultCell( column, row );

                if (e.hasAttribute("ValueType"))
                {
                  // TODO: what is this for?
            	  // <xsd:enumeration value="10"/> <!-- empty     -->
            	  // <xsd:enumeration value="20"/> <!-- boolean   -->
                  // <xsd:enumeration value="30"/> <!-- integer   -->
                  // <xsd:enumeration value="40"/> <!-- float     -->
                  // <xsd:enumeration value="50"/> <!-- error     -->
                  // <xsd:enumeration value="60"/> <!-- string    -->
                  // <xsd:enumeration value="70"/> <!-- cellrange -->
                  // <xsd:enumeration value="80"/> <!-- array     -->
                    QString valuetype = e.attribute( "ValueType" );
                    if ( valuetype == "40" )//percentage
                    {
                        kspread_cell->format()->setFormatType( Percentage_format );
                        kspread_cell->setValue( cell_content );
                    }
                    else if ( valuetype =="60" )//string
                    {
                        kspread_cell->format()->setFormatType( Text_format );
                        kspread_cell->setValue( cell_content );
                    }
                }

                if (e.hasAttribute( "ValueFormat" ))
                {
                  QString formatString = e.attribute( "ValueFormat" );
                  if ( !setType( kspread_cell, formatString, cell_content ) )
                    table->setText(row, column, cell_content, false);
                }
                else
                  table->setText(row, column, cell_content, false);

		if (e.hasAttribute("ExprID"))
                {
		    // QString encoded_string(table->cellAt( column, row, false)->encodeFormula( row, column ).utf8());
		    QString encoded_string(table->cellAt( column, row, false )->encodeFormula().toLatin1());


		    char * tmp_string = ( char * ) malloc( strlen( encoded_string.toLatin1() ) );
		    strcpy( tmp_string, encoded_string.toLatin1() );

		    kDebug(30521) << encoded_string.toLatin1() << endl;

		    exprID_dict.insert(e.attribute("ExprID"), tmp_string);

		    kDebug(30521) << exprID_dict[e.attribute("ExprID")] << endl;
		    kDebug(30521) << exprID_dict[QString("1")] << endl;
		    kDebug(30521) << e.attribute("ExprID") << endl;

		  }
	      }
	    }
	    else
            {

                column = e.attribute( "Col" ).toInt() + 1;
		row    = e.attribute( "Row" ).toInt() + 1;

		QString cell_content( e.text() );
                //kDebug()<<"cell_content :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :"<<cell_content<<endl;
                if ( cell_content[0] == '=' )
                  convertFormula( cell_content );

                Cell * kspread_cell = table->nonDefaultCell( column, row );

                if (e.hasAttribute("ValueType"))
                {
                  // TODO: Defined type of cell
                    //<xsd:enumeration value="10"/> <!-- empty     -->
                    //<xsd:enumeration value="20"/> <!-- boolean   -->
                    //<xsd:enumeration value="30"/> <!-- integer   -->
                    //<xsd:enumeration value="40"/> <!-- float     -->
                    //<xsd:enumeration value="50"/> <!-- error     -->
                    //<xsd:enumeration value="60"/> <!-- string    -->
                    //<xsd:enumeration value="70"/> <!-- cellrange -->
                    //<xsd:enumeration value="80"/> <!-- array     -->
                    //kspread_cell->setValue( date );
                    //kspread_cell->format()->setFormatType( type );
                    QString valuetype = e.attribute( "ValueType" );
                    if ( valuetype == "40" )//percentage
                    {
                        kspread_cell->format()->setFormatType( Percentage_format );
                        kspread_cell->setValue( cell_content );
                    }
                    else if ( valuetype =="60" )//string
                    {
                        kspread_cell->format()->setFormatType( Text_format );
                        kspread_cell->setValue( cell_content );
                    }

                }

                if (e.hasAttribute( "ValueFormat" ))
                {
                  QString formatString = e.attribute( "ValueFormat" );
                  if ( !setType( kspread_cell, formatString, cell_content ) )
                    table->setText(row, column, cell_content, false);
                }
                else
                  table->setText(row, column, cell_content, false);


                if (e.hasAttribute("ExprID"))
                {
		    column = e.attribute("Col").toInt() + 1;
		    row    = e.attribute("Row").toInt() + 1;
		    char * expr;
		    expr = exprID_dict[e.attribute("ExprID")];
		    // expr = exprID_dict[QString("1")];

		    kDebug(30521) << "FOO:" << column << row << endl;
		    kDebug(30521) <<
                           table->cellAt( column, row, false )->decodeFormula( expr, column, row ).toLatin1() << endl;
		    kDebug(30521) << expr << endl;

		    table->setText(row, column,
                                   table->cellAt( column, row, false )->decodeFormula( expr, column, row ),
                                   false);
		  }
	      }
	  }
	  cell = cell.nextSibling();
	}

        kDebug(30521) << "Reading in cells done" << endl;

        if ( mergedRegion.isNull() )
        {
          kWarning(30521) << "No cells merged !" << endl;
        }
	while ( !mergedRegion.isNull() )
        {
            QDomElement e = mergedRegion.toElement(); // try to convert the node to an element.
            QString cell_merge_area( e.text() );
            Range range(cell_merge_area);
            //kDebug()<<"text !!! :"<<cell_merge_area<< "range :start row : "<<range.startRow ()<<" start col :"<<range.startCol ()<<" end row :"<<range.endRow ()<<" end col :"<<range.endCol ()<<endl;
            Cell * cell = table->nonDefaultCell( range.startCol (), range.startRow () );
            cell->mergeCells( range.startCol (), range.startRow (), range.endCol ()-range.startCol (),  range.endRow ()-range.startRow ());
            mergedRegion = mergedRegion.nextSibling();
        }
	/* There is a memory leak here...
	 * The strings in the exprID_dict have been allocated, but they have not been freed.
	 */

	/* exprID_dict.statistics(); */

	/* CELL handling STOP */

	/* STYLE handling START */
        //Laurent - 2001-12-07  desactivate this code : otherwise we
        //create 65535*255 cells (Styleregion is define for a area and
        //not for cell, so gnumeric define a style as : col start=0 col end=255
        //rowstart=0 rowend=255 => we create 255*255 cells
        //and gnumeric stocke all area and not just modify area
        //=> not good for kspread.
        // Norbert: activated again, only cells with texts get modified, nothing else created
	setStyleInfo(&sheet, table);

	/* STYLE handling STOP */
        table->enableScrollBarUpdates( true );

	sheet = sheet.nextSibling();
        ++num;
      }

    if ( selTable )
      ksdoc->setDisplaySheet( selTable );

    emit sigProgress(100);
    if ( bSuccess )
        return KoFilter::OK;
    else
        return KoFilter::StupidError;
}

#include <gnumericimport.moc>
