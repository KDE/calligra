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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kspread_util.h"
#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_locale.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <ctype.h>

#include <qregexp.h>

#include <kdebug.h>
#include <kcalendarsystem.h>

/**
 * helper function to read integers
 * @param str
 * @param pos the position to start at. It will be updated when we parse it.
 * @return the integer read in the string, or -1 if no string
 */
static int readInt(const QString &str, uint &pos)
{
  if (!str.at(pos).isDigit())
    return -1;
  int result = 0;
  for ( ; str.length() > pos && str.at(pos).isDigit(); pos++ )
  {
    result *= 10;
    result += str.at(pos).digitValue();
  }

  return result;
}

QDateTime util_readTime( const QString & intstr, KLocale * locale, bool withSeconds,
                         bool * ok, bool & duration )
{
  duration = false;
  QString str = intstr.simplifyWhiteSpace().lower();
  QString format = locale->timeFormat().simplifyWhiteSpace();
  if ( !withSeconds )
  {
    int n = format.find("%S");
    format = format.left( n - 1 );
  }

  int days = -1;
  int hour = -1, minute = -1;
  int second = withSeconds ? -1 : 0; // don't require seconds
  bool g_12h = false;
  bool pm = false;
  uint strpos = 0;
  uint formatpos = 0;

  QDate refDate( 1899, 12, 31 );

  uint l  = format.length();
  uint sl = str.length();

  while (l > formatpos || sl > strpos)
  {
    if ( !(l > formatpos && sl > strpos) )
      goto error;

    QChar c( format.at( formatpos++ ) );

    if (c != '%')
    {
      if (c.isSpace())
        ++strpos;
      else if (c != str.at(strpos++))
        goto error;
      continue;
    }

    // remove space at the begining
    if (sl > strpos && str.at( strpos).isSpace() )
      ++strpos;

    c = format.at( formatpos++ );
    switch (c)
    {
     case 'p':
      {
        QString s;
        s = locale->translate("pm").lower();
        int len = s.length();
        if (str.mid(strpos, len) == s)
        {
          pm = true;
          strpos += len;
        }
        else
        {
          s = locale->translate("am").lower();
          len = s.length();
          if (str.mid(strpos, len) == s)
          {
            pm = false;
            strpos += len;
          }
          else
            goto error;
        }
      }
      break;

     case 'k':
     case 'H':
      g_12h = false;
      hour = readInt(str, strpos);
      if (hour < 0)
        goto error;
      if (hour > 23)
      {
        days = (int)(hour / 24);
        hour %= 24;
      }

      break;

     case 'l':
     case 'I':
      g_12h = true;
      hour = readInt(str, strpos);
      if (hour < 1 || hour > 12)
        goto error;

      break;

     case 'M':
      minute = readInt(str, strpos);
      if (minute < 0 || minute > 59)
        goto error;

      break;

     case 'S':
      second = readInt(str, strpos);
      if (second < 0 || second > 59)
        goto error;

      break;
    }
  }

  if (g_12h)
  {
    hour %= 12;
    if (pm) hour += 12;
  }

  if (days > 0)
  {
    refDate.addDays( days );
    duration = true;
  }

  if (ok)
    *ok = true;
  return QDateTime( refDate, QTime( hour, minute, second ) );

 error:
  if (ok)
    *ok = false;
  return QDateTime( refDate, QTime( -1, -1, -1 ) ); // return invalid date if it didn't work
}

QString
util_fractionFormat(double value, KSpreadCell::FormatType fmtType)
{
    double result = value - floor(value);
    int index;
    int limit = 0;

    /* return w/o fraction part if not necessary */
    if (result == 0)
	return QString::number(value);

    switch (fmtType) {
    case KSpreadCell::fraction_half:
	index = 2;
	break;
    case KSpreadCell::fraction_quarter:
	index = 4;
	break;
    case KSpreadCell::fraction_eighth:
	index = 8;
	break;
    case KSpreadCell::fraction_sixteenth:
	index = 16;
	break;
    case KSpreadCell::fraction_tenth:
	index = 10;
	break;
    case KSpreadCell::fraction_hundredth:
	index = 100;
	break;
    case KSpreadCell::fraction_one_digit:
	index = 3;
	limit = 9;
	break;
    case KSpreadCell::fraction_two_digits:
	index = 4;
	limit = 99;
	break;
    case KSpreadCell::fraction_three_digits:
	index = 5;
	limit = 999;
	break;
    default:
	kdDebug(36001) << "Error in Fraction format\n";
	return QString::number(value);
	break;
    } /* switch */


    /* handle halves, quarters, tenths, ... */

    if (fmtType != KSpreadCell::fraction_three_digits
	&& fmtType != KSpreadCell::fraction_two_digits
	&& fmtType != KSpreadCell::fraction_one_digit) {
	double calc = 0;
	int index1 = 0;
	double diff = result;
	for (int i = 1; i <= index; i++) {
	    calc = i * 1.0 / index;
	    if (fabs(result - calc) < diff) {
		index1 = i;
		diff = fabs(result - calc);
	    }
	}
	if( index1 == 0 ) return QString("%1").arg( floor(value) );
	if( index1 == index ) return QString("%1").arg( floor(value)+1 );
        if( floor(value) == 0)
            return QString("%1/%2").arg( index1 ).arg( index );

	return QString("%1 %2/%3")
		.arg( floor(value) )
	      	.arg( index1 )
		.arg( index );
    }


    /* handle fraction_one_digit, fraction_two_digit
     * and fraction_three_digit style */

    double precision, denominator, numerator;

    do {
	double val1 = result;
	double val2 = rint(result);
	double inter2 = 1;
	double inter4, p,  q;
	inter4 = p = q = 0;

	precision = pow(10.0, -index);
	numerator = val2;
	denominator = 1;

	while (fabs(numerator/denominator - result) > precision) {
		val1 = (1 / (val1 - val2));
		val2 = rint(val1);
		p = val2 * numerator + inter2;
		q = val2 * denominator + inter4;
		inter2 = numerator;
		inter4 = denominator;
		numerator = p;
		denominator = q;
	}
	index--;
    } while (fabs(denominator) > limit);

    denominator = fabs(denominator);
    numerator = fabs(numerator);

    if (denominator == numerator)
	return QString().setNum(floor(value + 1));
    else
    {
        if ( floor(value) == 0 )
            return QString("%1/%2").arg(numerator).arg(denominator);
        else
            return QString("%1 %2/%3")
		.arg(floor(value))
		.arg(numerator)
		.arg(denominator);
    }
}

QString
util_timeFormat(KLocale * locale, const QDateTime & dt,
		KSpreadCell::FormatType fmtType)
{
    if (fmtType == KSpreadCell::Time)
	return locale->formatTime(dt.time(), false);

    if (fmtType == KSpreadCell::SecondeTime)
	return locale->formatTime(dt.time(), true);

    int h = dt.time().hour();
    int m = dt.time().minute();
    int s = dt.time().second();

    QString hour = ( h < 10 ? "0" + QString::number(h) : QString::number(h) );
    QString minute = ( m < 10 ? "0" + QString::number(m) : QString::number(m) );
    QString second = ( s < 10 ? "0" + QString::number(s) : QString::number(s) );
    bool pm = (h > 12);
    QString AMPM( pm ? i18n("PM"):i18n("AM") );

    if (fmtType == KSpreadCell::Time_format1) {	// 9 : 01 AM
	return QString("%1:%2 %3")
		.arg((pm ? h - 12 : h),2)
		.arg(minute,2)
		.arg(AMPM);
    }

    if (fmtType == KSpreadCell::Time_format2) {	//9:01:05 AM
	return QString("%1:%2:%3 %4")
		.arg((pm ? h-12 : h),2)
		.arg(minute,2)
		.arg(second,2)
		.arg(AMPM);
    }

    if (fmtType == KSpreadCell::Time_format3) {
	return QString("%1 %2 %3 %4 %5 %6")			// 9 h 01 min 28 s
		.arg(hour,2)
		.arg(i18n("h"))
		.arg(minute,2)
		.arg(i18n("min"))
		.arg(second,2)
		.arg(i18n("s"));
    }

    if (fmtType == KSpreadCell::Time_format4) {	// 9:01
	return QString("%1:%2").arg(hour, 2).arg(minute, 2);
    }

    if (fmtType == KSpreadCell::Time_format5) {	// 9:01:12
	return QString("%1:%2:%3").arg(hour, 2).arg(minute, 2).arg(second, 2);
    }

    QDate d1(dt.date());
    QDate d2( 1899, 12, 31 );
    int d = d2.daysTo( d1 ) + 1;

    h += d * 24;

    if (fmtType == KSpreadCell::Time_format6)
    {	// [mm]:ss
      m += (h * 60);
      return QString("%1:%2").arg(m, 1).arg(second, 2);
    }
    if (fmtType == KSpreadCell::Time_format7) {	// [h]:mm:ss
	return QString("%1:%2:%3").arg(h, 1).arg(minute, 2).arg(second, 2);
    }
    if (fmtType == KSpreadCell::Time_format8)
    {	// [h]:mm
      m += (h * 60);
      return QString("%1:%2").arg(h, 1).arg(minute, 2);
    }

    return locale->formatTime( dt.time(), false );
}

QString
util_dateFormat(KLocale * locale, const QDate &date,
		KSpreadCell::FormatType fmtType)
{
    QString tmp;
    if (fmtType == KSpreadCell::ShortDate) {
	tmp = locale->formatDate(date, true);
    }
    else if (fmtType == KSpreadCell::TextDate) {
	tmp = locale->formatDate(date, false);
    }
    else if (fmtType == KSpreadCell::date_format1) {	/*18-Feb-99 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + locale->calendar()->monthString(date, true) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format2) {	/*18-Feb-1999 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + locale->calendar()->monthString(date, true) + "-";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format3) {	/*18-Feb */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + locale->calendar()->monthString(date, true);
    }
    else if (fmtType == KSpreadCell::date_format4) {	/*18-05 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + QString().sprintf("%02d", date.month() );
    }
    else if (fmtType == KSpreadCell::date_format5) {	/*18/05/00 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString().sprintf("%02d", date.month()) + "/";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format6) {	/*18/05/1999 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString().sprintf("%02d", date.month()) + "/";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format7) {	/*Feb-99 */
        tmp = locale->calendar()->monthString(date, true) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format8) {	/*February-99 */
      tmp = locale->calendar()->monthString(date, false) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format9) {	/*February-1999 */
        tmp = locale->calendar()->monthString(date, false) + "-";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format10) {	/*F-99 */
        tmp = locale->calendar()->monthString(date, false).at(0) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format11) {	/*18/Feb */
	tmp = QString().sprintf("%02d", date.day()) + "/";
	tmp += locale->calendar()->monthString(date, true);
    }
    else if (fmtType == KSpreadCell::date_format12) {	/*18/02 */
	tmp = QString().sprintf("%02d", date.day()) + "/";
	tmp += QString().sprintf("%02d", date.month());
    }
    else if (fmtType == KSpreadCell::date_format13) {	/*18/Feb/1999 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + locale->calendar()->monthString(date, true) + "/";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format14) {	/*2000/Feb/18 */
	tmp = QString::number(date.year());
	tmp = tmp + "/" + locale->calendar()->monthString(date, true) + "/";
	tmp = tmp + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format15) {	/*2000-Feb-18 */
	tmp = QString::number(date.year());
	tmp = tmp + "-" + locale->calendar()->monthString(date, true) + "-";
	tmp = tmp + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format16) {	/*2000-02-18 */
	tmp = QString::number(date.year());
	tmp = tmp + "-" + QString().sprintf("%02d", date.month()) + "-";
	tmp = tmp + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format17) {	/*2 february 2000 */
	tmp = QString().sprintf("%d", date.day());
	tmp = tmp + " " + locale->calendar()->monthString(date, false) + " ";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format18) {	/*02/18/1999 */
	tmp = QString().sprintf("%02d", date.month());
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format19) {	/*02/18/99 */
	tmp = QString().sprintf("%02d", date.month());
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format20) {	/*Feb/18/99 */
        tmp = locale->calendar()->monthString(date, true);
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format21) {	/*Feb/18/1999 */
        tmp = locale->calendar()->monthString(date, true);
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format22) {	/*Feb-1999 */
        tmp = locale->calendar()->monthString(date, true) + "-";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format23) {	/*1999 */
	tmp = QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format24) {	/*99 */
	tmp = QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format25) {	/*2000/02/18 */
	tmp = QString::number(date.year());
	tmp = tmp + "/" + QString().sprintf("%02d", date.month());
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format26) {	/*2000/Feb/18 */
	tmp = QString::number(date.year());
	tmp = tmp + "/" + locale->calendar()->monthString(date, true);
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
    }
    else
	tmp = locale->formatDate(date, true);

    // Missing compared with gnumeric:
    //	"m/d/yy h:mm",		/* 20 */
    //	"m/d/yyyy h:mm",	/* 21 */
    //	"mmm/ddd/yy",		/* 12 */
    //	"mmm/ddd/yyyy",		/* 13 */
    //	"mm/ddd/yy",		/* 14 */
    //	"mm/ddd/yyyy",		/* 15 */

    return tmp;
}


QString util_dateTimeFormat( KLocale * /*locale*/, double /*date*/, KSpreadCell::FormatType /*fmtType*/, QString const & /*format*/ )
{
    return QString::null;
}

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

QString util_rangeColumnName( const QRect &_area)
{
    return QString("%1:%2")
        .arg( KSpreadCell::columnName( _area.left()))
        .arg( KSpreadCell::columnName(_area.right()));
}

QString util_rangeRowName( const QRect &_area)
{
    return QString("%1:%2")
        .arg( _area.top())
        .arg(_area.bottom());
}


QString util_rangeName(const QRect &_area)
{
    return KSpreadCell::name( _area.left(), _area.top() ) + ":" +
	KSpreadCell::name( _area.right(), _area.bottom() );
}

QString util_rangeName(KSpreadSheet * _table, const QRect &_area)
{
    return _table->tableName() + "!" + util_rangeName(_area);
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
    f.setItalic( TRUE );

  if ( element.hasAttribute( "bold" ) && element.attribute("bold") == "yes" )
    f.setBold( TRUE );

  if ( element.hasAttribute( "underline" ) && element.attribute("underline") == "yes" )
    f.setUnderline( TRUE );

  if ( element.hasAttribute( "strikeout" ) && element.attribute("strikeout") == "yes" )
    f.setStrikeOut( TRUE );

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
    table = 0;
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
      tableName = _str.left( n );
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

    //int y = atoi( str.mid( p2, p-p2 ).latin1() );
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
                            KSpreadSheet * _table )
{
    uint p = 0;
    int p2 = _str.find( '!' );
    if ( p2 != -1 )
    {
        tableName = _str.left( p2++ );
        while ( true )
        {
            table = _map->findTable( tableName );
            if ( !table && tableName[0] == ' ' )
            {
                tableName = tableName.right( tableName.length() - 1 );
                continue;
            }
            break;
        }
        p = p2;

        //If the loop didn't return a table, better keep a string for isValid
        if ( tableName.isEmpty() )
        {
            kdDebug(36001) << "KSpreadPoint: tableName is unknown" << endl;
            tableName = "unknown";
        }
    }
    else
    {
        if ( _table != 0 )
        {
            table = _table;
            tableName = _table->tableName();
        }
        else
            table = 0;
    }

    init( _str.mid( p ) );
}

KSpreadCell *KSpreadPoint::cell() const
{
    return table->cellAt(pos);
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

KSpreadRange::KSpreadRange(const QString & _str)
{
    range.setLeft(-1);
    table = 0;

    int p = _str.find(':');
    if (p == -1)
	return;

    KSpreadPoint ul(_str.left(p));
    KSpreadPoint lr(_str.mid(p + 1));
    range = QRect(ul.pos, lr.pos);
    tableName = ul.tableName;

    leftFixed = ul.columnFixed;
    rightFixed = lr.columnFixed;
    topFixed = ul.rowFixed;
    bottomFixed = lr.rowFixed;
}

KSpreadRange::KSpreadRange(const QString & _str, KSpreadMap * _map,
			   KSpreadSheet * _table)
{
    range.setLeft(-1);
    table = 0;
    //used area Name as range
    if (_str.at(0) == "'" && _str.at(_str.length() - 1) == "'") {
	QString tmp = _str.right(_str.length() - 1);
	tmp = tmp.left(tmp.length() - 1);
	QValueList < Reference >::Iterator it;
	QValueList < Reference > area = _map->doc()->listArea();
	for (it = area.begin(); it != area.end(); ++it) {
	    if ((*it).ref_name == tmp) {
		range = (*it).rect;
		table = _map->findTable((*it).table_name);
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
    table = 0;

    int p = 0;
    int p2 = _str.find('!');
    if (p2 != -1)
    {
      tableName = _str.left(p2++);
      while ( true )
      {
	table = _map->findTable(tableName);
        if ( !table && tableName[0] == ' ' )
        {
          tableName = tableName.right( tableName.length() - 1 );
          continue;
        }
        break;
      }
      p = p2;
    } else
      table = _table;


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

bool KSpreadRange::contains (const KSpreadPoint &cell) const
{
  return range.contains (cell.pos);
}

bool KSpreadRange::intersects (const KSpreadRange &r) const
{
  return range.intersects (r.range);
}

double util_fact( double val, double end )
{
  /* fact =i*(i-1)*(i-2)*...*1 */
  if(val<0.0 || end<0.0)
    return (-1);
  if(val==0.0)
    return (1);
  else if (val==end)
    return(1);
  /*val==end => you don't multiplie it */
  else
    return (val*util_fact((double)(val-1),end));
}

bool util_isColumnSelected(const QRect &selection)
{
  return ( (selection.top() == 1) && (selection.bottom() == KS_rowMax) );
}

bool util_isRowSelected(const QRect &selection)
{
  return ( (selection.left() == 1) && (selection.right() == KS_colMax) );
}

bool util_validateTableName(QString name)
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


KSpreadRangeIterator::KSpreadRangeIterator(QRect _range, KSpreadSheet* _table)
{
  range = _range;
  table = _table;
  current = QPoint(0,0);
}

KSpreadRangeIterator::~KSpreadRangeIterator()
{
}

KSpreadCell* KSpreadRangeIterator::first()
{
  current.setY(range.top());

  /* OK, because even if this equals zero, the 'getNextCellRight' won't
     try to access it*/
  current.setX(range.left() - 1);
  return next();
}

KSpreadCell* KSpreadRangeIterator::next()
{
  if (current.x() == 0 && current.y() == 0)
  {
    return first();
  }

  KSpreadCell* cell = NULL;
  bool done = false;

  while (cell == NULL && !done)
  {
    cell = table->getNextCellRight(current.x(), current.y());
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


QString convertRefToBase( const QString & table, const QRect & rect )
{
  QPoint bottomRight( rect.bottomRight() );

  QString s( "$" );
  s += table;
  s += ".$";
  s += KSpreadCell::columnName( bottomRight.x() );
  s += '$';
  s += QString::number( bottomRight.y() );

  return s;
}

QString convertRefToRange( const QString & table, const QRect & rect )
{
  QPoint topLeft( rect.topLeft() );
  QPoint bottomRight( rect.bottomRight() );

  if ( topLeft == bottomRight )
    return convertRefToBase( table, rect );

  QString s( "$" );
  s += table;
  s += ".$";
  s += /*util_encodeColumnLabelText*/KSpreadCell::columnName( topLeft.x() );
  s += '$';
  s += QString::number( topLeft.y() );
  s += ":.$";
  s += /*util_encodeColumnLabelText*/KSpreadCell::columnName( bottomRight.x() );
  s += '$';
  s += QString::number( bottomRight.y() );

  return s;
}

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
QString convertRangeToRef( const QString & tableName, const QRect & _area )
{
    return tableName + "." + KSpreadCell::name( _area.left(), _area.top() ) + ":" + tableName + "."+ KSpreadCell::name( _area.right(), _area.bottom() );
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
        return pen;
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
