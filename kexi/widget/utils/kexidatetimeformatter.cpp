/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidatetimeformatter.h"

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetable.h>
#include <klineedit.h>
#include <kmenu.h>
#include <kdatewidget.h>

KexiDateFormatter::KexiDateFormatter()
{
  // use "short date" format system settings
//! @todo allow to override the format using column property and/or global app settings
  QString df( KGlobal::locale()->dateFormatShort() );
  if (df.length()>2)
    m_separator = df.mid(2,1);
  else
    m_separator = "-";
  const int separatorLen = m_separator.length();
  QString yearMask("9999");
  QString yearDateFormat("yyyy"), 
    monthDateFormat("MM"), 
    dayDateFormat("dd"); //for setting up m_dateFormat
  bool ok = df.length()>=8;
  int yearpos, monthpos, daypos; //result of df.find()
  if (ok) {//look at % variables
//! @todo more variables are possible here, see void KLocale::setDateFormatShort() docs
//!       http://developer.kde.org/documentation/library/3.5-api/kdelibs-apidocs/kdecore/html/classKLocale.html#a59
    yearpos = df.find("%y", 0, false); //&y or %y
    m_longYear = !(yearpos>=0 && df.mid(yearpos+1, 1)=="y"); 
    if (!m_longYear) {
      yearMask = "99";
      yearDateFormat = "yy";
    }
    monthpos = df.find("%m", 0, true); //%m or %n
    m_monthWithLeadingZero = true;
    if (monthpos<0) {
      monthpos = df.find("%n", 0, false);
      m_monthWithLeadingZero = false;
      monthDateFormat = "M";
    }
    daypos = df.find("%d", 0, true);//%d or %e
    m_dayWithLeadingZero = true;
    if (daypos<0) {
      daypos = df.find("%e", 0, false);
      m_dayWithLeadingZero = false;
      dayDateFormat = "d";
    }
    ok = (yearpos>=0 && monthpos>=0 && daypos>=0);
  }
  m_order = QDateEdit::YMD; //default
  if (ok) {
    if (yearpos<monthpos && monthpos<daypos) {
      //will be set in "default: YMD"
    }
    else if (yearpos<daypos && daypos<monthpos) {
      m_order = QDateEdit::YDM;
//! @todo use QRegExp (to replace %Y by %1, etc.) instead of hardcoded "%1%299%399" 
//!       because df may contain also other characters
      m_inputMask = QString("%1%299%399").arg(yearMask).arg(m_separator).arg(m_separator);
      m_qtFormat = yearDateFormat+m_separator+dayDateFormat+m_separator+monthDateFormat;
      m_yearpos = 0;
      m_daypos = yearMask.length()+separatorLen;
      m_monthpos = m_daypos+2+separatorLen;
    }
    else if (daypos<monthpos && monthpos<yearpos) {
      m_order = QDateEdit::DMY;
      m_inputMask = QString("99%199%2%3").arg(m_separator).arg(m_separator).arg(yearMask);
      m_qtFormat = dayDateFormat+m_separator+monthDateFormat+m_separator+yearDateFormat;
      m_daypos = 0;
      m_monthpos = 2+separatorLen;
      m_yearpos = m_monthpos+2+separatorLen;
    }
    else if (monthpos<daypos && daypos<yearpos) {
      m_order = QDateEdit::MDY;
      m_inputMask = QString("99%199%2%3").arg(m_separator).arg(m_separator).arg(yearMask);
      m_qtFormat = monthDateFormat+m_separator+dayDateFormat+m_separator+yearDateFormat;
      m_monthpos = 0;
      m_daypos = 2+separatorLen;
      m_yearpos = m_daypos+2+separatorLen;
    }
    else
      ok = false;
  }
  if (!ok || m_order == QDateEdit::YMD) {//default: YMD
    m_inputMask = QString("%1%299%399").arg(yearMask).arg(m_separator).arg(m_separator);
    m_qtFormat = yearDateFormat+m_separator+monthDateFormat+m_separator+dayDateFormat;
    m_yearpos = 0;
    m_monthpos = yearMask.length()+separatorLen;
    m_daypos = m_monthpos+2+separatorLen;
  }
  m_inputMask += ";_";
}

KexiDateFormatter::~KexiDateFormatter()
{
}

QDate KexiDateFormatter::stringToDate( const QString& str ) const
{
  bool ok = true;
  int year = str.mid(m_yearpos, m_longYear ? 4 : 2).toInt(&ok);
  if (!ok)
    return QDate();
  if (year < 30) {//2000..2029
    year = 2000 + year;
  }
  else if (year < 100) {//1930..1999
    year = 1900 + year;
  }

  int month = str.mid(m_monthpos, 2).toInt(&ok);
  if (!ok)
    return QDate();

  int day = str.mid(m_daypos, 2).toInt(&ok);
  if (!ok)
    return QDate();

  QDate date(year, month, day);
  if (!date.isValid())
    return QDate();
  return date;
}

QVariant KexiDateFormatter::stringToVariant( const QString& str ) const
{
  if (isEmpty(str))
    return QVariant();
  const QDate date( stringToDate( str ) );
  if (date.isValid())
    return date;
  return QVariant();
}

bool KexiDateFormatter::isEmpty( const QString& str ) const
{
  QString s(str);
  return s.replace(m_separator,"").trimmed().isEmpty();
}

QString KexiDateFormatter::dateToString( const QDate& date ) const
{
  return date.toString(m_qtFormat);
}

//------------------------------------------------

KexiTimeFormatter::KexiTimeFormatter()
: m_hmsRegExp( new QRegExp("(\\d*):(\\d*):(\\d*).*( am| pm){,1}", false/*!CS*/) )
 , m_hmRegExp( new QRegExp("(\\d*):(\\d*).*( am| pm){,1}", false/*!CS*/) )
{
  QString tf( KGlobal::locale()->timeFormat() );
  //m_hourpos, m_minpos, m_secpos; are result of tf.find()
  QString hourVariable, minVariable, secVariable;

  //detect position of HOUR section: find %H or %k or %I or %l
  m_24h = true;
  m_hoursWithLeadingZero = true;
  m_hourpos = tf.find("%H", 0, true);
  if (m_hourpos>=0) {
    m_24h = true;
    m_hoursWithLeadingZero = true;
  }
  else {
    m_hourpos = tf.find("%k", 0, true);
    if (m_hourpos>=0) {
      m_24h = true;
      m_hoursWithLeadingZero = false;
    }
    else {
      m_hourpos = tf.find("%I", 0, true);
      if (m_hourpos>=0) {
        m_24h = false;
        m_hoursWithLeadingZero = true;
      }
      else {
        m_hourpos = tf.find("%l", 0, true);
        if (m_hourpos>=0) {
          m_24h = false;
          m_hoursWithLeadingZero = false;
        }
      }
    }
  }
  m_minpos = tf.find("%M", 0, true);
  m_secpos = tf.find("%S", 0, true); //can be -1
  m_ampmpos = tf.find("%p", 0, true); //can be -1

  if (m_hourpos<0 || m_minpos<0) {
    //set default: hr and min are needed, sec are optional
    tf = "%H:%M:%S";
    m_24h = true;
    m_hoursWithLeadingZero = false;
    m_hourpos = 0;
    m_minpos = 3;
    m_secpos = m_minpos + 3;
    m_ampmpos = -1;
  }
  hourVariable = tf.mid(m_hourpos, 2);

  m_inputMask = tf;
//	m_inputMask.replace( hourVariable, "00" );
//	m_inputMask.replace( "%M", "00" );
//	m_inputMask.replace( "%S", "00" ); //optional
  m_inputMask.replace( hourVariable, "99" );
  m_inputMask.replace( "%M", "99" );
  m_inputMask.replace( "%S", "00" ); //optional
  m_inputMask.replace( "%p", "AA" ); //am or pm
  m_inputMask += ";_";

  m_outputFormat = tf;
}

KexiTimeFormatter::~KexiTimeFormatter()
{
  delete m_hmsRegExp;
  delete m_hmRegExp;
}

QTime KexiTimeFormatter::stringToTime( const QString& str ) const
{
  int hour, min, sec;
  bool pm = false;

  bool tryWithoutSeconds = true;
  if (m_secpos>=0) {
    if (-1 != m_hmsRegExp->search(str)) {
      hour = m_hmsRegExp->cap(1).toInt();
      min = m_hmsRegExp->cap(2).toInt();
      sec = m_hmsRegExp->cap(3).toInt();
      if (m_ampmpos >= 0 && m_hmsRegExp->numCaptures()>3)
        pm = m_hmsRegExp->cap(4).trimmed().toLower()=="pm";
      tryWithoutSeconds = false;
    }
  }
  if (tryWithoutSeconds) {
    if (-1 == m_hmRegExp->search(str))
      return QTime(99,0,0);
    hour = m_hmRegExp->cap(1).toInt();
    min = m_hmRegExp->cap(2).toInt();
    sec = 0;
    if (m_ampmpos >= 0 && m_hmRegExp->numCaptures()>2)
      pm = m_hmsRegExp->cap(4).toLower()=="pm";
  }

  if (pm && hour < 12)
    hour += 12; //PM
  return QTime(hour, min, sec);
}

QVariant KexiTimeFormatter::stringToVariant( const QString& str )
{
  if (isEmpty( str ))
    return QVariant();
  const QTime time( stringToTime( str ) );
  if (time.isValid())
    return time;
  return QVariant();
}

bool KexiTimeFormatter::isEmpty( const QString& str ) const
{
  QString s(str);
  return s.replace(':',"").trimmed().isEmpty();
}

QString KexiTimeFormatter::timeToString( const QTime& time ) const
{
  if (!time.isValid())
    return QString();

  QString s(m_outputFormat);
  if (m_24h) {
    if (m_hoursWithLeadingZero)
      s.replace( "%H", QString::fromLatin1(time.hour()<10 ? "0" : "") + QString::number(time.hour()) );
    else
      s.replace( "%k", QString::number(time.hour()) );
  }
  else {
    int time12 = (time.hour()>12) ? (time.hour()-12) : time.hour();
    if (m_hoursWithLeadingZero)
      s.replace( "%I", QString::fromLatin1(time12<10 ? "0" : "") + QString::number(time12) );
    else
      s.replace( "%l", QString::number(time12) );
  }
  s.replace( "%M", QString::fromLatin1(time.minute()<10 ? "0" : "") + QString::number(time.minute()) );
  if (m_secpos>=0)
    s.replace( "%S", QString::fromLatin1(time.second()<10 ? "0" : "") + QString::number(time.second()) );
  if (m_ampmpos>=0)
    s.replace( "%p", KGlobal::locale()->translate( time.hour()>=12 ? "pm" : "am") );
  return s;
}

//------------------------------------------------

QString dateTimeInputMask(const KexiDateFormatter& dateFormatter, const KexiTimeFormatter& timeFormatter)
{
  QString mask(dateFormatter.inputMask());
  mask.truncate(dateFormatter.inputMask().length()-2);
  return mask + " " + timeFormatter.inputMask();
}

QDateTime stringToDateTime(
  const KexiDateFormatter& dateFormatter, const KexiTimeFormatter& timeFormatter, const QString& str)
{
  QString s( str.trimmed() );
  const int timepos = s.find(" ");
  const bool emptyTime = timepos >= 0 && timeFormatter.isEmpty(s.mid(timepos+1)); //.replace(':',"").trimmed().isEmpty();
  if (emptyTime)
    s = s.left(timepos);
  if (timepos>0 && !emptyTime) {
    return QDateTime(
      dateFormatter.stringToDate( s.left(timepos) ),
      timeFormatter.stringToTime( s.mid(timepos+1) )
    );
  }
  else {
    return QDateTime(
      dateFormatter.stringToDate( s ),
      QTime(0,0,0)
    );
  }
}

bool dateTimeIsEmpty( const KexiDateFormatter& dateFormatter, const KexiTimeFormatter& timeFormatter, 
  const QString& str )
{
  int timepos = str.find(" ");
  const bool emptyTime = timepos >= 0 && timeFormatter.isEmpty(str.mid(timepos+1)); //s.mid(timepos+1).replace(':',"").trimmed().isEmpty();
  return (timepos >= 0 && dateFormatter.isEmpty(str.left(timepos)) //s.left(timepos).replace(m_dateFormatter.separator(), "").trimmed().isEmpty()
    && emptyTime);
}

bool dateTimeIsValid( const KexiDateFormatter& dateFormatter, 
  const KexiTimeFormatter& timeFormatter, const QString& str )
{
  int timepos = str.find(" ");
  const bool emptyTime = timepos >= 0 && timeFormatter.isEmpty(str.mid(timepos+1)); //s.mid(timepos+1).replace(':',"").trimmed().isEmpty();
  if (timepos >= 0 && dateFormatter.isEmpty(str.left(timepos)) // s.left(timepos).replace(m_dateFormatter.separator(), "").trimmed().isEmpty()
    && emptyTime)
    //empty date/time is valid
    return true;
  return timepos>=0 && dateFormatter.stringToDate( str.left(timepos) ).isValid()
    && (emptyTime /*date without time is also valid*/ || timeFormatter.stringToTime( str.mid(timepos+1) ).isValid());
}
