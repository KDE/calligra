/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <ctype.h>

#include <qregexp.h>

#include <kdebug.h>

#include "kspread_doc.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_sheet.h"

#include "kspread_util.h"

using namespace KSpread;

//helper functions for the formatting
bool formatIsDate (FormatType fmt)
{
  return ((fmt == ShortDate_format) || (fmt == TextDate_format) ||
      (((int) fmt >= 200) && ((int) fmt < 300)));
}

bool formatIsTime (FormatType fmt)
{
  return (((int) fmt >= 50) && ((int) fmt < 70));
}

bool formatIsFraction (FormatType fmt)
{
  return (((int) fmt >= 70) && ((int) fmt < 80));
}


//used in KSpreadPoint::init, Cell::encodeFormula and
//  dialogs/kspread_dlg_paperlayout.cc
int util_decodeColumnLabelText( const QString &_col )
{
    int col = 0;
    int offset='a'-'A';
    int counterColumn = 0;
    for ( uint i=0; i < _col.length(); i++ )
    {
        counterColumn = (int) pow(26.0 , static_cast<int>(_col.length() - i - 1));
        if( _col[i] >= 'A' && _col[i] <= 'Z' )
            col += counterColumn * ( _col[i].latin1() - 'A' + 1);  // okay here (Werner)
        else if( _col[i] >= 'a' && _col[i] <= 'z' )
            col += counterColumn * ( _col[i].latin1() - 'A' - offset + 1 );
        else
            kdDebug(36001) << "util_decodeColumnLabelText: Wrong characters in label text for col:'" << _col << "'" << endl;
    }
    return col;
}

//used in dialogs/kspread_dlg_paperlayout.cc
QString util_rangeColumnName( const QRect &_area)
{
    return QString("%1:%2")
        .arg( Cell::columnName( _area.left()))
        .arg( Cell::columnName(_area.right()));
}

//used in dialogs/kspread_dlg_paperlayout.cc
QString util_rangeRowName( const QRect &_area)
{
    return QString("%1:%2")
        .arg( _area.top())
        .arg(_area.bottom());
}

QString util_rangeName(const QRect &_area)
{
    return Cell::name( _area.left(), _area.top() ) + ":" +
	Cell::name( _area.right(), _area.bottom() );
}

QString util_rangeName(KSpreadSheet * _sheet, const QRect &_area)
{
    return _sheet->sheetName() + "!" + util_rangeName(_area);
}

QDomElement util_createElement( const QString & tagName, const QFont & font, QDomDocument & doc )
{
  QDomElement e( doc.createElement( tagName ) );

  e.setAttribute( "family", font.family() );
  e.setAttribute( "size", font.pointSize() );
  e.setAttribute( "weight", font.weight() );
  if ( font.bold() )
    e.setAttribute( "bold", "yes" );
  if ( font.italic() )
    e.setAttribute( "italic", "yes" );
  if ( font.underline() )
    e.setAttribute( "underline", "yes" );
  if ( font.strikeOut() )
    e.setAttribute( "strikeout", "yes" );
  //e.setAttribute( "charset", KGlobal::charsets()->name( font ) );

  return e;
}

QDomElement util_createElement( const QString & tagname, const QPen & pen, QDomDocument & doc )
{
  QDomElement e( doc.createElement( tagname ) );
  e.setAttribute( "color", pen.color().name() );
  e.setAttribute( "style", (int)pen.style() );
  e.setAttribute( "width", (int)pen.width() );
  return e;
}

QFont util_toFont( QDomElement & element )
{
  QFont f;
  f.setFamily( element.attribute( "family" ) );

  bool ok;
  f.setPointSize( element.attribute("size").toInt( &ok ) );
  if ( !ok )
    return QFont();

  f.setWeight( element.attribute("weight").toInt( &ok ) );
  if ( !ok )
    return QFont();

  if ( element.hasAttribute( "italic" ) && element.attribute("italic") == "yes" )
    f.setItalic( true );

  if ( element.hasAttribute( "bold" ) && element.attribute("bold") == "yes" )
    f.setBold( true );

  if ( element.hasAttribute( "underline" ) && element.attribute("underline") == "yes" )
    f.setUnderline( true );

  if ( element.hasAttribute( "strikeout" ) && element.attribute("strikeout") == "yes" )
    f.setStrikeOut( true );

  /* Uncomment when charset is added to kspread_dlg_layout
     + save a document-global charset
     if ( element.hasAttribute( "charset" ) )
       KGlobal::charsets()->setQFont( f, element.attribute("charset") );
      else
  */
  // ######## Not needed anymore in 3.0?
  //KGlobal::charsets()->setQFont( f, KGlobal::locale()->charset() );

  return f;
}

QPen util_toPen( QDomElement & element )
{
  bool ok;
  QPen p;

  p.setStyle( (Qt::PenStyle)element.attribute("style").toInt( &ok ) );
  if ( !ok )
    return QPen();

  p.setWidth( element.attribute("width").toInt( &ok ) );
  if ( !ok )
    return QPen();

  p.setColor( QColor( element.attribute("color") ) );

  return p;
}

KSpreadPoint::KSpreadPoint(const QString & _str)
{
    sheet = 0;
    init(_str);
}

void KSpreadPoint::init(const QString & _str)
{

//    kdDebug(36001) <<"KSpreadPoint::init ("<<_str<<")"<<endl;
    pos.setX(-1);

    uint len = _str.length();
    if ( !len )
    {
	kdDebug(36001) << "KSpreadPoint::init: len = 0" << endl;
	return;
    }

    QString str( _str );
    int n = _str.find( '!' );
    if ( n != -1 )
    {
      sheetName = _str.left( n );
      str = _str.right( len - n - 1 ); // remove the '!'
      len = str.length();
    }

    uint p = 0;

    // Fixed ?
    if ( str[0] == '$' )
    {
	columnFixed = true;
	p++;
    }
    else
	columnFixed = false;

    // Malformed ?
    if ( p == len )
    {
	kdDebug(36001) << "KSpreadPoint::init: no point after '$' (str: '" << str.mid( p ) << "'" << endl;
	return;
    }
    if ( str[p] < 'A' || str[p] > 'Z' )
    {
	if ( str[p] < 'a' || str[p] > 'z' )
	{
	    kdDebug(36001) << "KSpreadPoint::init: wrong first character in point (str: '" << str.mid( p ) << "'" << endl;
	    return;
	}
    }
    //default is error
    int x = -1;
    //search for the first character != text
    int result = str.find( QRegExp("[^A-Za-z]+"), p );

    //get the colomn number for the character between actual position and the first non text charakter
    if ( result != -1 )
	x = util_decodeColumnLabelText( str.mid( p, result - p ) ); // x is defined now
    else  // If there isn't any, then this is not a point -> return
    {
	kdDebug(36001) << "KSpreadPoint::init: no number in string (str: '" << str.mid( p, result ) << "'" << endl;
	return;
    }
    p = result;

    //limit is KS_colMax
    if ( x > KS_colMax )
    {
	kdDebug(36001) << "KSpreadPoint::init: column value too high (col: " << x << ")" << endl;
	return;
    }

    // Malformed ?
    if (p == len)
    {
	kdDebug(36001) << "KSpreadPoint::init: p==len after cols" << endl;
	return;
    }

    if (str[p] == '$')
    {
	rowFixed = true;
	p++;
	// Malformed ?
	if ( p == len )
	{
	    kdDebug(36001) << "KSpreadPoint::init: p==len after $ of row" << endl;
	    return;
	}
    }
    else
	rowFixed = false;

    uint p2 = p;
    while ( p < len )
    {
	if ( !isdigit( QChar(str[p++]) ) )
	{
	    kdDebug(36001) << "KSpreadPoint::init: no number" << endl;
	    return;
	}
    }

    bool ok;
    int y = str.mid( p2, p-p2 ).toInt( &ok );
    if ( !ok )
    {
	kdDebug(36001) << "KSpreadPoint::init: Invalid number (str: '" << str.mid( p2, p-p2 ) << "'" << endl;
	return;
    }
    if ( y > KS_rowMax )
    {
	kdDebug(36001) << "KSpreadPoint::init: row value too high (row: " << y << ")" << endl;
	return;
    }
    if ( y <= 0 )
    {
	kdDebug(36001) << "KSpreadPoint::init: y <= 0" << endl;
	return;
    }
    pos = QPoint( x, y );
}

KSpreadPoint::KSpreadPoint( const QString & _str, KSpreadMap * _map,
                            KSpreadSheet * _sheet )
{
    uint p = 0;
    int p2 = _str.find( '!' );
    if ( p2 != -1 )
    {
        sheetName = _str.left( p2++ );
        while ( true )
        {
            sheet = _map->findSheet( sheetName );
            if ( !sheet && sheetName[0] == ' ' )
            {
                sheetName = sheetName.right( sheetName.length() - 1 );
                continue;
            }
            break;
        }
        p = p2;

        //If the loop didn't return a sheet, better keep a string for isValid
        if ( sheetName.isEmpty() )
        {
            kdDebug(36001) << "KSpreadPoint: tableName is unknown" << endl;
            sheetName = "unknown";
        }
    }
    else
    {
        if ( _sheet != 0 )
        {
            sheet = _sheet;
            sheetName = _sheet->sheetName();
        }
        else
            sheet = 0;
    }

    init( _str.mid( p ) );
}

Cell *KSpreadPoint::cell() const
{
    return sheet->cellAt(pos);
}

bool KSpreadPoint::operator== (const KSpreadPoint &cell) const
{
  //sheet info ignored
  return (pos == cell.pos);
}

bool KSpreadPoint::operator< (const KSpreadPoint &cell) const
{
  //sheet info ignored
  return (pos.y() < cell.pos.y()) ? true :
      ((pos.y() == cell.pos.y()) && (pos.x() < cell.pos.x()));
}

KSpreadRange::KSpreadRange()
{
  	sheet = 0;
	range.setLeft( -1 );

	leftFixed=false;
	rightFixed=false;
	topFixed=false;
	bottomFixed=false;
}
KSpreadRange::KSpreadRange(const QString & _str)
{
    range.setLeft(-1);
    sheet = 0;

    int p = _str.find(':');
    if (p == -1)
	return;

    KSpreadPoint ul(_str.left(p));
    KSpreadPoint lr(_str.mid(p + 1));
    range = QRect(ul.pos, lr.pos);
    sheetName = ul.sheetName;

    leftFixed = ul.columnFixed;
    rightFixed = lr.columnFixed;
    topFixed = ul.rowFixed;
    bottomFixed = lr.rowFixed;
}

KSpreadRange::KSpreadRange(const QString & _str, KSpreadMap * _map,
			   KSpreadSheet * _sheet)
{
    range.setLeft(-1);
    sheet = 0;
    //used area Name as range
    if (_str.at(0) == "'" && _str.at(_str.length() - 1) == "'") {
	QString tmp = _str.right(_str.length() - 1);
	tmp = tmp.left(tmp.length() - 1);
	QValueList < Reference >::Iterator it;
	QValueList < Reference > area = _map->doc()->listArea();
	for (it = area.begin(); it != area.end(); ++it) {
	    if ((*it).ref_name == tmp) {
		range = (*it).rect;
		sheet = _map->findSheet((*it).sheet_name);
		break;
	    }
	}
	leftFixed = false;
	rightFixed = false;
	topFixed = false;
	bottomFixed = false;
	return;
    }
    range.setLeft(-1);
    sheet = 0;

    int p = 0;
    int p2 = _str.find('!');
    if (p2 != -1)
    {
      sheetName = _str.left(p2++);
      while ( true )
      {
	sheet = _map->findSheet(sheetName);
        if ( !sheet && sheetName[0] == ' ' )
        {
          sheetName = sheetName.right( sheetName.length() - 1 );
          continue;
        }
        break;
      }
      p = p2;
    } else
      sheet = _sheet;


    int p3 = _str.find(':', p);
    if (p3 == -1)
	return;

    KSpreadPoint ul(_str.mid(p, p3 - p));
    KSpreadPoint lr(_str.mid(p3 + 1));
    range = QRect(ul.pos, lr.pos);

    leftFixed = ul.columnFixed;
    rightFixed = lr.columnFixed;
    topFixed = ul.rowFixed;
    bottomFixed = lr.rowFixed;
}

QString KSpreadRange::toString()
{
	QString result;

	if (sheet)
	{
		result=util_rangeName(sheet,range);
	}
	else
	{
		result=util_rangeName(range);
	}

	//Insert $ characters to show fixed parts of range

	int pos=result.find("!")+1;
	Q_ASSERT(pos != -1);

	if (leftFixed)
	{
		result.insert(pos,'$');
		pos++; //Takes account of extra character added in
	}
	if (topFixed)
	{
		result.insert(pos+Cell::columnName(range.left()).length(),'$');
	}

	pos=result.find(":")+1;
	Q_ASSERT(pos != -1);

	if (rightFixed)
	{
		result.insert(pos,'$');
		pos++; //Takes account of extra character added in
	}
	if (bottomFixed)
	{
		result.insert(pos+Cell::columnName(range.right()).length(),'$');
	}


	return result;
}

void KSpreadRange::getStartPoint(KSpreadPoint* pt)
{
	if (!isValid()) return;

	pt->setRow(startRow());
	pt->setColumn(startCol());
	pt->columnFixed=leftFixed;
	pt->rowFixed=topFixed;
	pt->sheet=sheet;
	pt->sheetName=sheetName;
}

void KSpreadRange::getEndPoint(KSpreadPoint* pt)
{
	if (!isValid()) return;

	pt->setRow(endRow());
	pt->setColumn(endCol());
	pt->columnFixed=rightFixed;
	pt->rowFixed=bottomFixed;
	pt->sheet=sheet;
	pt->sheetName=sheetName;
}

bool KSpreadRange::contains (const KSpreadPoint &cell) const
{
  return range.contains (cell.pos);
}

bool KSpreadRange::intersects (const KSpreadRange &r) const
{
  return range.intersects (r.range);
}

bool KSpreadRange::isValid() const
{
	return  ( range.left() >= 0 ) &&
		( range.right() >= 0 ) &&
		( sheet != 0 || sheetName.isEmpty() ) &&
		( range.isValid() ) ;
}

bool util_isAllSelected(const QRect &selection)
{
  return ( selection.top() == 1 && selection.bottom() == KS_rowMax
	   && selection.left() == 1 && selection.right() == KS_colMax);
}

bool util_isColumnSelected(const QRect &selection)
{
  return ( (selection.top() == 1) && (selection.bottom() == KS_rowMax) );
}

bool util_isRowSelected(const QRect &selection)
{
  return ( (selection.left() == 1) && (selection.right() == KS_colMax) );
}

bool util_isRowOrColumnSelected(const QRect &selection)
{
    return ( (selection.left() == 1) && (selection.right() == KS_colMax)
             || (selection.top() == 1) && (selection.bottom() == KS_rowMax) );
}

//used in KSpreadView::slotRename
bool util_validateSheetName(const QString &name)
{
  if (name[0] == ' ')
  {
    return false;
  }
  for (unsigned int i = 0; i < name.length(); i++)
  {
    if ( !(name[i].isLetterOrNumber() ||
           name[i] == ' ' || name[i] == '.' ||
           name[i] == '_'))
    {
      return false;
    }
  }
  return true;
}


KSpreadRangeIterator::KSpreadRangeIterator(QRect _range, KSpreadSheet* _sheet)
{
  range = _range;
  sheet = _sheet;
  current = QPoint(0,0);
}

KSpreadRangeIterator::~KSpreadRangeIterator()
{
}

Cell* KSpreadRangeIterator::first()
{
  current.setY(range.top());

  /* OK, because even if this equals zero, the 'getNextCellRight' won't
     try to access it*/
  current.setX(range.left() - 1);
  return next();
}

Cell* KSpreadRangeIterator::next()
{
  if (current.x() == 0 && current.y() == 0)
  {
    return first();
  }

  Cell* cell = NULL;
  bool done = false;

  while (cell == NULL && !done)
  {
    cell = sheet->getNextCellRight(current.x(), current.y());
    if (cell != NULL && cell->column() > range.right())
    {
      cell = NULL;
    }

    if (cell == NULL)
    {
      current.setX(range.left() - 1);
      current.setY(current.y() + 1);
      done = (current.y() > range.bottom());
    }
  }
  return cell;
}

//not used anywhere
int util_penCompare( QPen const & pen1, QPen const & pen2 )
{
  if ( pen1.style() == Qt::NoPen && pen2.style() == Qt::NoPen )
    return 0;

  if ( pen1.style() == Qt::NoPen )
    return -1;

  if ( pen2.style() == Qt::NoPen )
    return 1;

  if ( pen1.width() < pen2.width() )
    return -1;

  if ( pen1.width() > pen2.width() )
    return 1;

  if ( pen1.style() < pen2.style() )
    return -1;

  if ( pen1.style() > pen2.style() )
    return 1;

  if ( pen1.color().name() < pen2.color().name() )
    return -1;

  if ( pen1.color().name() > pen2.color().name() )
    return 1;

  return 0;
}


QString convertRefToBase( const QString & sheet, const QRect & rect )
{
  QPoint bottomRight( rect.bottomRight() );

  QString s( "$" );
  s += sheet;
  s += ".$";
  s += Cell::columnName( bottomRight.x() );
  s += '$';
  s += QString::number( bottomRight.y() );

  return s;
}

QString convertRefToRange( const QString & sheet, const QRect & rect )
{
  QPoint topLeft( rect.topLeft() );
  QPoint bottomRight( rect.bottomRight() );

  if ( topLeft == bottomRight )
    return convertRefToBase( sheet, rect );

  QString s( "$" );
  s += sheet;
  s += ".$";
  s += /*util_encodeColumnLabelText*/Cell::columnName( topLeft.x() );
  s += '$';
  s += QString::number( topLeft.y() );
  s += ":.$";
  s += /*util_encodeColumnLabelText*/Cell::columnName( bottomRight.x() );
  s += '$';
  s += QString::number( bottomRight.y() );

  return s;
}

//used in Cell::convertFormulaToOasisFormat
void insertBracket( QString & s )
{
  QChar c;
  int i = (int) s.length() - 1;

  while ( i >= 0 )
  {
    c = s[i];
    if ( c == ' ' )
      s[i] = '_';
    if ( !(c.isLetterOrNumber() || c == ' ' || c == '.'
           || c == '_') )
    {
      s.insert( i + 1, '[' );
      return;
    }
    --i;
  }
}

 // e.g.: Sheet4.A1:Sheet4.E28
 //used in KSpreadSheet::saveOasis
QString convertRangeToRef( const QString & sheetName, const QRect & _area )
{
    return sheetName + "." + Cell::name( _area.left(), _area.top() ) + ":" + sheetName + "."+ KSpread::Cell::name( _area.right(), _area.bottom() );
}

QString convertOasisPenToString( const QPen & pen )
{
    kdDebug()<<"convertOasisPenToString( const QPen & pen ) :"<<pen<<endl;
    QString s = QString( "%1pt " ).arg( pen.width() );
    switch( pen.style() )
    {
    case Qt::NoPen:
        return "none";
    case Qt::SolidLine:
        s+="solid";
        break;
    case Qt::DashLine:
        s+="dashed";
        break;
    case Qt::DotLine:
        s+="dotted";
        break;
    case Qt::DashDotLine:
        s+="dot-dash";
        break;
    case Qt::DashDotDotLine:
        s+="dot-dot-dash";
        break;
    }
    kdDebug()<<" convertOasisPenToString :"<<s<<endl;
    if ( pen.color().isValid() )
    {
        s+=' ';
        s+=pen.color().name();
    }
    return s;
}

QPen convertOasisStringToPen( const QString &border )
{
    QPen pen;
    //string like "0.088cm solid #800000"
    if (border.isEmpty() || border=="none" || border=="hidden") // in fact no border
    {
        pen.setStyle( Qt::NoPen );
        return pen;
    }
    //code from koborder, for the moment kspread doesn't use koborder
    // ## isn't it faster to use QStringList::split than parse it 3 times?
    QString _width = border.section(' ', 0, 0);
    QCString _style = border.section(' ', 1, 1).latin1();
    QString _color = border.section(' ', 2, 2);

    pen.setWidth( ( int )( KoUnit::parseValue( _width, 1.0 ) ) );

    if ( _style =="none" )
        pen.setStyle( Qt::NoPen );
    else if ( _style =="solid" )
        pen.setStyle( Qt::SolidLine );
    else if ( _style =="dashed" )
        pen.setStyle( Qt::DashLine );
    else if ( _style =="dotted" )
        pen.setStyle( Qt::DotLine );
    else if ( _style =="dot-dash" )
        pen.setStyle( Qt::DashDotLine );
    else if ( _style =="dot-dot-dash" )
        pen.setStyle( Qt::DashDotDotLine );
    else
        kdDebug()<<" style undefined : "<<_style<<endl;

    if ( _color.isEmpty() )
        pen.setColor( QColor() );
    else
        pen.setColor(  QColor( _color ) );

    return pen;
}

//Return true when it's a reference to cell from sheet.
bool localReferenceAnchor( const QString &_ref )
{
    bool isLocalRef = (_ref.find("http://") != 0 &&
                       _ref.find("mailto:") != 0 &&
                       _ref.find("ftp://") != 0  &&
                       _ref.find("file:") != 0 );
    return isLocalRef;
}
