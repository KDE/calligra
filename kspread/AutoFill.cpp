/* This file is part of the KDE project

   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2002-2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 1999-2001 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>
   Copyright 1998-1999 Stephan Kulow <coolo@kde.org>
   Copyright 1998 Reginald Stadlbauer <reggie@kde.org>

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

#include <math.h>

#include <QList>
#include <QRegExp>

#include <kconfig.h>
#include <kdebug.h>
#include <kinstance.h>

#include "Doc.h"
#include "Localization.h"
#include "Sheet.h"
#include "Undo.h"
#include "Value.h"
#include "ValueConverter.h"
#include "AutoFill.h"

using namespace KSpread;

QStringList *AutoFillSequenceItem::month = 0;
QStringList *AutoFillSequenceItem::shortMonth = 0;
QStringList *AutoFillSequenceItem::day = 0;
QStringList *AutoFillSequenceItem::shortDay = 0;
QStringList *AutoFillSequenceItem::other = 0;
/**********************************************************************************
 *
 * AutoFillDeltaSequence
 *
 **********************************************************************************/

AutoFillDeltaSequence::AutoFillDeltaSequence( AutoFillSequence *_first, AutoFillSequence *_next )
  : m_ok(true),
    m_sequence(0)
{
  if ( _first->count() != _next->count() )
  {
    m_ok = false;
    return;
  }

  m_sequence = new QVector<double> ( _first->count() );

  int index = 0;
  AutoFillSequenceItem *item = _first->value( index );
  AutoFillSequenceItem *item2 = _next->value( index );
  int i = 0;
  // for( item = _first->value( index ); item != 0 && item2 != 0L; item = _first->value( ++index ) );
  for ( i = 0; i < _first->count(); i++ )
  {
    double d;
    if ( !item->getDelta( item2, d ) )
      {
        m_ok = false;
        return;
      }
    m_sequence->insert( i++, d );
    item2 = _next->value( ++index );
    item = _first->value( ++index );
  }
}

AutoFillDeltaSequence::~AutoFillDeltaSequence()
{
    delete m_sequence;
}

bool AutoFillDeltaSequence::equals( AutoFillDeltaSequence *_delta )
{
  if ( m_sequence == 0 )
    return false;
  if ( _delta->getSequence() == 0 )
    return false;
  if ( m_sequence->size() != _delta->getSequence()->size() )
    return false;

  for ( int i = 0; i < m_sequence->size(); i++ )
  {
    if ( m_sequence->at( i ) != _delta->getSequence()->at( i ) )
      return false;
  }

  return true;
}

double AutoFillDeltaSequence::getItemDelta( int _pos )
{
  if ( m_sequence == 0 )
    return 0.0;

  return m_sequence->at( _pos );
}

/**********************************************************************************
 *
 * AutoFillSequenceItem
 *
 **********************************************************************************/

AutoFillSequenceItem::AutoFillSequenceItem( int _i )
{
    m_IValue = _i;
    m_Type = INTEGER;
}

AutoFillSequenceItem::AutoFillSequenceItem( double _d )
{
    m_DValue = _d;
    m_Type = FLOAT;
}

AutoFillSequenceItem::AutoFillSequenceItem( const QString &_str )
{
    m_String = _str;
    m_Type = STRING;

    if ( month == 0 )
    {
        month = new QStringList();
        month->append( i18n("January") );
        month->append( i18n("February") );
        month->append( i18n("March") );
        month->append( i18n("April") );
        month->append( i18n("May") );
        month->append( i18n("June") );
        month->append( i18n("July") );
        month->append( i18n("August") );
        month->append( i18n("September") );
        month->append( i18n("October") );
        month->append( i18n("November") );
        month->append( i18n("December") );
    }

    if ( shortMonth == 0 )
    {
        shortMonth = new QStringList();
        shortMonth->append( i18n("Jan") );
        shortMonth->append( i18n("Feb") );
        shortMonth->append( i18n("Mar") );
        shortMonth->append( i18n("Apr") );
        shortMonth->append( i18nc("May short", "May") );
        shortMonth->append( i18n("Jun") );
        shortMonth->append( i18n("Jul") );
        shortMonth->append( i18n("Aug") );
        shortMonth->append( i18n("Sep") );
        shortMonth->append( i18n("Oct") );
        shortMonth->append( i18n("Nov") );
        shortMonth->append( i18n("Dec") );
    }

    if ( day == 0 )
    {
        day = new QStringList();
        day->append( i18n("Monday") );
        day->append( i18n("Tuesday") );
        day->append( i18n("Wednesday") );
        day->append( i18n("Thursday") );
        day->append( i18n("Friday") );
        day->append( i18n("Saturday") );
        day->append( i18n("Sunday") );
    }

    if ( shortDay == 0 )
    {
        shortDay = new QStringList();
        shortDay->append( i18n("Mon") );
        shortDay->append( i18n("Tue") );
        shortDay->append( i18n("Wed") );
        shortDay->append( i18n("Thu") );
        shortDay->append( i18n("Fri") );
        shortDay->append( i18n("Sat") );
        shortDay->append( i18n("Sun") );
    }

    if( other==0)
      {
	//	other=new QStringList();
	KConfig *config = Factory::global()->config();
	config->setGroup( "Parameters" );
        other=new QStringList(config->readEntry("Other list", QStringList()));
      }

    if ( month->indexOf( _str ) != -1 )
    {
        m_Type = MONTH;
        return;
    }

    if ( shortMonth->indexOf( _str ) != -1 )
    {
        m_Type = SHORTMONTH;
        return;
    }

    if ( day->indexOf( _str ) != -1 )
    {
      m_Type = DAY;
      return;
    }

    if ( shortDay->indexOf( _str ) != -1 )
    {
      m_Type = SHORTDAY;
      return;
    }

    if( other->indexOf(_str) != -1 )
      {
	m_Type = OTHER;
	int index = other->indexOf(_str);
        int otherBegin = other->lastIndexOf( "\\", index ); // backward
        int otherEnd = other->indexOf( "\\", index ); // forward
        m_OtherBegin = (otherBegin != -1) ? otherBegin : 0;
        m_OtherEnd = (otherEnd != -1) ? otherEnd : other->count();
	return;
      }

    if ( m_String[0] == '=' )
        m_Type = FORMULA;
}

bool AutoFillSequenceItem::getDelta( AutoFillSequenceItem *seq, double &_delta )
{
    if ( seq->getType() != m_Type )
        return false;

    switch( m_Type )
    {
    case INTEGER:
        _delta = (double)( seq->getIValue() - m_IValue );
        return true;
    case FLOAT:
        _delta = seq->getDValue() - m_DValue;
        return true;
    case FORMULA:
    case STRING:
        if ( m_String == seq->getString() )
        {
            _delta = 0.0;
            return true;
        }
        return false;
    case MONTH:
        {
            int i = month->indexOf( m_String );
            int j = month->indexOf( seq->getString() );
            int k = j;

            if ( j + 1 == i )
                _delta = -1.0;
            else
                _delta = ( double )( k - i );
            return true;
        }

    case SHORTMONTH:
        {
            int i = shortMonth->indexOf( m_String );
            int j = shortMonth->indexOf( seq->getString() );
            int k = j;

            if ( j + 1 == i )
                _delta = -1.0;
            else
                _delta = ( double )( k - i );
            return true;
        }

    case DAY:
        {
            int i = day->indexOf( m_String );
            int j = day->indexOf( seq->getString() );
            int k = j;

            if ( j + 1 == i )
                _delta = -1.0;
            else
                _delta = ( double )( k - i );
            kDebug() << m_String << " i: " << i << " j: " << j << " k: " << k << " delta: " << _delta << endl;
            return true;
        }

    case SHORTDAY:
        {
            int i = shortDay->indexOf( m_String );
            int j = shortDay->indexOf( seq->getString() );
            int k = j;

            if ( j + 1 == i )
                _delta = -1.0;
            else
                _delta = ( double )( k - i );
            return true;
        }
    case OTHER:
      {
	if( m_OtherEnd!= seq->getIOtherEnd() || m_OtherBegin!= seq->getIOtherBegin())
	  return false;
	int i = other->indexOf( m_String );
	int j = other->indexOf( seq->getString() );
	int k = j;
	if ( j < i )
	  k += (m_OtherEnd - m_OtherBegin - 1);
	/*if ( j + 1 == i )
	  _delta = -1.0;
	  else*/
	  _delta = ( double )( k - i );
	return true;
      }
     default:
      return false;
    }
}

QString AutoFillSequenceItem::getSuccessor( int _no, double _delta )
{
    QString erg;
    switch( m_Type )
    {
    case INTEGER:
        erg.sprintf("%i", m_IValue + _no * (int)_delta );
        break;
    case FLOAT:
        erg.sprintf("%f", m_DValue + (double)_no * _delta );
        break;
    case FORMULA:
    case STRING:
        erg = m_String;
        break;
    case MONTH:
        {
            int i = month->indexOf( m_String );
            int j = i + _no * (int) _delta;
            while (j < 0)
              j += month->count();
            int k = j % month->count();
            erg = (month->at( k ));
        }
        break;
    case SHORTMONTH:
        {
            int i = shortMonth->indexOf( m_String );
            int j = i + _no * (int) _delta;
            while (j < 0)
              j += shortMonth->count();
            int k = j % shortMonth->count();
            erg = (shortMonth->at( k ));
        }
        break;
    case DAY:
        {
            int i = day->indexOf( m_String );
            int j = i + _no * (int) _delta;
            while (j < 0)
              j += day->count();
            int k = j % day->count();
            erg = (day->at( k ));
        }
	break;
    case SHORTDAY:
        {
            int i = shortDay->indexOf( m_String );
            int j = i + _no * (int) _delta;
            while (j < 0)
              j += shortDay->count();
            int k = j % shortDay->count();
            erg = (shortDay->at( k ));
        }
        break;
    case OTHER:
      {
	 int i = other->indexOf( m_String )-(m_OtherBegin+1);
	 int j = i + _no * (int) _delta;
	 int k = j % (m_OtherEnd - m_OtherBegin-1);
	 erg = (other->at( (k+m_OtherBegin+1) ));
      }
     case TIME:
     case DATE:
      // gets never called but fixes a warning while compiling
      break;
    }

    return QString( erg );
}

QString AutoFillSequenceItem::getPredecessor( int _no, double _delta )
{
  QString erg;
  switch( m_Type )
  {
   case INTEGER:
    erg.sprintf("%i", m_IValue - _no * (int)_delta );
    break;
   case FLOAT:
    erg.sprintf("%f", m_DValue - (double)_no * _delta );
    break;
   case FORMULA:
   case STRING:
    erg = m_String;
    break;
   case MONTH:
    {
      int i = month->indexOf( m_String );
      int j = i - _no * (int) _delta;
      while ( j < 0 )
        j += month->count();
      int k = j % month->count();
      erg = (month->at( k ));
    }
    break;
   case SHORTMONTH:
    {
      int i = shortMonth->indexOf( m_String );
      int j = i - _no * (int) _delta;
      while ( j < 0 )
        j += shortMonth->count();
      int k = j % shortMonth->count();
      erg = (shortMonth->at( k ));
    }
    break;
   case DAY:
    {
      int i = day->indexOf( m_String );
      int j = i - _no * (int) _delta;
      while ( j < 0 )
        j += day->count();
      int k = j % day->count();
      erg = (day->at( k ));
    }
    break;
   case SHORTDAY:
    {
      int i = shortDay->indexOf( m_String );
      int j = i - _no * (int) _delta;
      while ( j < 0 )
        j += shortDay->count();
      int k = j % shortDay->count();
      erg = (shortDay->at( k ));
    }
    break;
   case OTHER:
    {
      int i = other->indexOf( m_String ) - (m_OtherBegin + 1);
      int j = i - _no * (int) _delta;
      while ( j < 0 )
        j += (m_OtherEnd - m_OtherBegin - 1);
      int k = j % (m_OtherEnd - m_OtherBegin - 1);
      erg = (other->at( (k + m_OtherBegin + 1) ));
    }
   case TIME:
   case DATE:
    // gets never called but fixes a warning while compiling
    break;
  }

  return QString( erg );
}

/**********************************************************************************
 *
 * AutoFillSequence
 *
 **********************************************************************************/

AutoFillSequence::AutoFillSequence( Cell *_cell )
{
    if ( _cell->isFormula() )
    {
        QString d = _cell->encodeFormula();
        sequence.append( new AutoFillSequenceItem( d ) );
    }
    else if ( _cell->value().isNumber() )
    {
        if ( floor( _cell->value().asFloat() ) == _cell->value().asFloat() )
        {
            sequence.append( new AutoFillSequenceItem( (int)_cell->value().asFloat()) );
        }
        else
            sequence.append( new AutoFillSequenceItem(_cell->value().asFloat() ) );
    }
    else if ( !_cell->text().isEmpty() )
        sequence.append( new AutoFillSequenceItem( _cell->text() ) );
}

AutoFillSequence::~AutoFillSequence()
{
  qDeleteAll( sequence );
}

bool AutoFillSequence::matches( AutoFillSequence* _seq, AutoFillDeltaSequence *_delta )
{
    AutoFillDeltaSequence delta( this, _seq );
    if ( !delta.isOk() )
        return false;

    if ( delta.equals( _delta ) )
         return true;

    return false;
}

void AutoFillSequence::fillCell( Cell *src, Cell *dest, AutoFillDeltaSequence *delta, int _block, bool down )
{
    QString erg = "";

    // Special handling for formulas
    if ( sequence.value(0) != 0 && sequence.first()->getType() == AutoFillSequenceItem::FORMULA )
    {
        QString f = dest->decodeFormula( sequence.first()->getString() );
        dest->setCellText( f );
        dest->copyFormat( src );
        return;
    }

    int i = 0;
    if (down)
    {
      foreach ( AutoFillSequenceItem* item, sequence )
        erg += item->getSuccessor( _block, delta->getItemDelta( i++ ) );
    }
    else
    {
      foreach ( AutoFillSequenceItem* item, sequence )
        erg += item->getPredecessor( _block, delta->getItemDelta( i++ ) );
    }

    dest->setCellText( erg );
    dest->copyFormat( src );
}

/**********************************************************************************
 *
 * Sheet
 *
 **********************************************************************************/

void Sheet::autofill( QRect &src, QRect &dest )
{
    if (src == dest)
    {
        return;
    }

    setRegionPaintDirty( dest );

    doc()->emitBeginOperation();

    if ( !doc()->undoLocked() )
    {
      UndoAutofill *undo = new UndoAutofill( doc(), this, dest );
      doc()->addCommand( undo );
    }

    // disable the update of the max sroll range on each cell insertion
    // Bug 124806: creating series takes extremely long time
    enableScrollBarUpdates(false);

    // Fill from left to right
    if ( src.left() == dest.left() && src.right() < dest.right() )
    {
        for ( int y = src.top(); y <= src.bottom(); y++ )
        {
            int x;
            QList<Cell*> destList;
            for ( x = src.right() + 1; x <= dest.right(); x++ )
                destList.append( nonDefaultCell( x, y ) );
            QList<Cell*> srcList;
            for ( x = src.left(); x <= src.right(); x++ )
                srcList.append( cellAt( x, y ) );
            QList<AutoFillSequence*> seqList;
            for ( x = src.left(); x <= src.right(); x++ )
                seqList.append( new AutoFillSequence( cellAt( x, y ) ) );
            fillSequence( srcList, destList, seqList );
            qDeleteAll( seqList );
        }
    }

    // Fill from top to bottom
    if ( src.top() == dest.top() && src.bottom() < dest.bottom() )
    {
        for ( int x = src.left(); x <= dest.right(); x++ )
        {
            int y;
            QList<Cell*> destList;
            for ( y = src.bottom() + 1; y <= dest.bottom(); y++ )
                destList.append( nonDefaultCell( x, y ) );
            QList<Cell*> srcList;
            for ( y = src.top(); y <= src.bottom(); y++ )
            {
                srcList.append( cellAt( x, y ) );
            }
            QList<AutoFillSequence*> seqList;
            for ( y = src.top(); y <= src.bottom(); y++ )
                seqList.append( new AutoFillSequence( cellAt( x, y ) ) );
            fillSequence( srcList, destList, seqList );
            qDeleteAll( seqList );
        }
    }

    // Fill from right to left
    if ( ( src.left() == dest.right() || src.left() == dest.right() - 1) && src.right() >= dest.right() )
    {
        if ( src.left() != dest.right() )
            dest.setRight( dest.right() - 1 );

        for ( int y = dest.top(); y <= dest.bottom(); y++ )
        {
            int x;
            QList<Cell*> destList;

            for ( x = dest.left(); x < src.left(); x++ )
            {
                destList.append( nonDefaultCell( x, y ) );
            }
            QList<Cell*> srcList;
            for ( x = src.left(); x <= src.right(); x++ )
            {
                srcList.append( cellAt( x, y ) );
            }
            QList<AutoFillSequence*> seqList;
            for ( x = src.left(); x <= src.right(); x++ )
                seqList.append( new AutoFillSequence( cellAt( x, y ) ) );
            fillSequence( srcList, destList, seqList, false );
            qDeleteAll( seqList );
        }
    }

    // Fill from bottom to top
    if ( (src.top() == dest.bottom() || src.top() == (dest.bottom() - 1) ) && src.bottom() >= dest.bottom() )
    {
        if (src.top() != dest.bottom() )
            dest.setBottom(dest.bottom() - 1);
        int startVal = qMin( dest.left(), src.left());
        int endVal = qMax(src.right(), dest.right());
        for ( int x = startVal; x <= endVal; x++ )
        {
            int y;
            QList<Cell*> destList;
            for ( y = dest.top(); y < src.top(); y++ )
                destList.append( nonDefaultCell( x, y ) );
            QList<Cell*> srcList;
            for ( y = src.top(); y <= src.bottom(); ++y )
            {
                srcList.append( cellAt( x, y ) );
            }
            QList<AutoFillSequence*> seqList;
            for ( y = src.top(); y <= src.bottom(); y++ )
                seqList.append( new AutoFillSequence( cellAt( x, y ) ) );
            fillSequence( srcList, destList, seqList, false );
            qDeleteAll( seqList );
        }
    }

    // update the max sroll range ONCE here
    enableScrollBarUpdates(true);
    checkRangeHBorder(dest.right());
    checkRangeVBorder(dest.bottom());

    emit sig_updateView( this );
    // doc()->emitEndOperation();
}


void Sheet::fillSequence( const QList<Cell*>& _srcList,
                          const QList<Cell*>& _destList,
                          const QList<AutoFillSequence*>& _seqList,
                          bool down )
{
    if (_srcList.isEmpty() || _destList.isEmpty())
        return;

    doc()->emitBeginOperation(true);

    /* try finding an interval to use to fill the sequence */
    if (!fillSequenceWithInterval(_srcList, _destList, _seqList, down))
    {
      /* if no interval was found, just copy down through */
      fillSequenceWithCopy(_srcList, _destList, down);
    }

    doc()->emitEndOperation();

}

QVariant getDiff( const Value& value1, const Value& value2  , AutoFillSequenceItem::Type type  )
{
  if ( type == AutoFillSequenceItem::FLOAT )
	  return QVariant( value2.asFloat() - value1.asFloat() );
  if ( type == AutoFillSequenceItem::TIME || type == AutoFillSequenceItem::DATE )
	  return QVariant( (int)( value2.asInteger() - value1.asInteger() ) );

  return QVariant( (int)0 );
  // note: date and time difference can be calculated as
  // the difference of the serial number
 /* if( (type == AutoFillSequenceItem::FLOAT) ||
      (type == AutoFillSequenceItem::DATE) ||
      (type == AutoFillSequenceItem::TIME) )
    return ( value2.asFloat() - value1.asFloat() );
  else
    return 0.0;*/
}

bool Sheet::fillSequenceWithInterval( const QList<Cell*>& _srcList,
                                      const QList<Cell*>& _destList,
                                      const QList<AutoFillSequence*>& _seqList,
                                      bool down )
{
  if (_srcList.first()->isFormula())
    return false;

  QList<AutoFillDeltaSequence*> deltaList;
  bool ok = false;

  if ( _srcList.first()->value().isNumber() || _srcList.first()->isDate() || _srcList.first()->isTime() )
  {
    AutoFillSequenceItem::Type type;

    QVector< QVariant > tmp( _seqList.count() );  /*= new QValueList< QVariant > ( _seqList.count() )*/;
    QVector< QVariant > diff( _seqList.count() ); /*= new QValueList< QVariant > ( _seqList.count() )*/;
    int p = -1;
    int count = 0;
    int tmpcount = 0;
    int srcIndex = 0;

    Cell * cell = _srcList.value( srcIndex );
    Cell * cell2 = _srcList.value( ++srcIndex );

    bool singleCellOnly = (cell2 == 0);

    if ( cell->isDate() )
      type = AutoFillSequenceItem::DATE;
    else if ( cell->isTime() )
      type = AutoFillSequenceItem::TIME;
    else if ( cell->value().isNumber() )
      type = AutoFillSequenceItem::FLOAT;
    else
      return false; // Cannot happen due to if condition

    while ( cell && (cell2 || singleCellOnly) )
    {

      Value cellValue = cell->value();
      Value cell2Value;

      //If we only have a single cell, the interval will depend upon the data type.
      //- For numeric values, set the interval to 0 as we don't know what might be useful as a sequence
      //- For time values, set the interval to one hour, as this will probably be the most useful setting
      //- For date values, set the interval to one day, as this will probably be the most useful setting
      //
      //Note that the above options were chosen for consistency with Excel.  Gnumeric (1.59) sets
      //the interval to 0 for all types, OpenOffice.org (2.00) uses increments of 1.00, 1 hour and 1 day
      //respectively
      if (singleCellOnly)
      {
      	if (type == AutoFillSequenceItem::FLOAT)
		cell2Value = cellValue;
	else if ( type == AutoFillSequenceItem::TIME)
		cell2Value = Value( cellValue.asTime( doc() ).addSecs( 60*60 ), doc() );
	else if ( type == AutoFillSequenceItem::DATE)
		cell2Value = Value ( cellValue.asDate( doc() ).addDays( 1 ), doc() );
      }
      else
      {
	cell2Value = cell2->value();

      	// check if both cells contain the same type
      	if ( ( !cellValue.isNumber() )
           || ( cell2->isDate() && type != AutoFillSequenceItem::DATE )
           || ( cell2->isTime() && type != AutoFillSequenceItem::TIME ) )
      	{
        	count = 0;
        	ok = false;
       		break;
      	}
      }

      QVariant delta = getDiff(cellValue , cell2Value , type );

      if (count < 1)
      {
        p = count;
        diff[ count++ ] = delta;
      }
      else
      {
        // same value again?
        if (diff[ p ] == delta)
        {
          // store it somewhere else for the case we need it later
          ++p;
          tmp[ tmpcount++ ] = delta;
        }
        else
        {
          // if we have saved values in another buffer we have to insert them first
          if ( tmpcount > 0 )
          {
            for ( int i = 0; i < tmpcount; ++i )
            {
              diff[ count++ ] = tmp.at( i );
            }

            tmpcount = 0;
          }

          // insert the value
          p = 0;
          diff[ count++ ] = delta;
        }
      }

      // check next cell pair
      cell  = cell2;
      cell2 = _srcList.value( ++srcIndex );
    }

    // we have found something:
    if (count > 0 && (tmpcount > 0 || count == 1))
    {
      QVariant cellValue( (int) 0 );

      Cell * dest;
      Cell * src;
      int destIndex = 0;
      int srcIndex = 0;

      int i = tmpcount;
      if (down)
      {
        dest = _destList.first();
        src  = _srcList.last();
        srcIndex = _srcList.count() - 1;
      }
      else
      {
        dest = _destList.last();
        destIndex = _destList.count() - 1;
        src  = _srcList.first();

        i   *= -1;
      }

      if ( type == AutoFillSequenceItem::FLOAT )
	      cellValue = src->value().asFloat();
      else
	      cellValue = (int)src->value().asInteger();

      QString res;
      // copy all the data
      while (dest)
      {
        if (down)
        {
          while ( i >= count )
            i -= count;
        }
        else
        {
          while ( i < 0)
            i += count;
        }

	QVariant currentDiff = diff.at( i );

	if (cellValue.type() == QVariant::Double)
        	if (down)
            		cellValue = cellValue.toDouble() + currentDiff.toDouble();
          	else
            		cellValue = cellValue.toDouble() -  currentDiff.toDouble();
	else
		if (down)
			cellValue = cellValue.toInt() + currentDiff.toInt();
		else
			cellValue = cellValue.toInt() - currentDiff.toInt();

	if ( type == AutoFillSequenceItem::TIME)
	{
		Value timeValue = doc()->converter()->asTime( Value(cellValue.toInt()) );
		Value stringValue = doc()->converter()->asString( timeValue );
		dest->setCellText( stringValue.asString() );
	}
	else if ( type == AutoFillSequenceItem::DATE)
	{
		Value dateValue = doc()->converter()->asDate( Value(cellValue.toInt()) );
		Value stringValue = doc()->converter()->asString( dateValue );
		dest->setCellText( stringValue.asString() );
	}
	else
        	dest->setCellText( cellValue.toString() );

	dest->copyFormat( src );

        if (down)
        {
          ++i;
          dest = _destList.value( ++destIndex );
          src = _srcList.value( ++srcIndex );
        }
        else
        {
          --i;
          dest = _destList.value( --destIndex );
          src = _srcList.value( --srcIndex );
        }

        if (!src)
        {
          src = _srcList.last();
          srcIndex = _srcList.count() - 1;
        }
      }

      ok = true;
    }
    else
    {
      ok = false;
    }

   // delete tmp;
   // delete diff;

    return ok;
  }

  // What is the interval (block)? If your sheet looks like this:
  // 1 3 5 7 9
  // then the interval has the length 1 and the delta list is [2].
  // 2 200 3 300 4 400
  // Here the interval has length 2 and the delta list is [1,100]


  // How big is the interval. It is in the range from [2...n/2].
  // The case of an interval of length n is handled below.
  //
  // We try to find the shortest interval.
  for ( int step = 1; step <= _seqList.count() / 2; step++ )
  {
    kDebug() << "Looking for interval: " << step << " seqList count: " << _seqList.count() << endl;
    // If the interval is of length 'step' then the _seqList size must
    // be a multiple of 'step'
    if ( _seqList.count() % step == 0 )
    {
      // Be optimistic
      ok = true;

      qDeleteAll( deltaList );
      deltaList.clear();

      // Guess the delta by looking at cells 0...2*step-1
      //
      // Since the interval may be of length 'step' we calculate the delta
      // between cells 0 and step, 1 and step+1, ...., step-1 and 2*step-1
      for ( int t = 0; t < step; t++ )
      {
	deltaList.append( new AutoFillDeltaSequence( _seqList.at(t),
						     _seqList.at(t+step) ) );
	ok = deltaList.last()->isOk();
      }

      /* Verify the delta by looking at cells step..._seqList.count()
	 We only looked at the cells 0 ... '2*step-1'.
	 Now test wether the cells from "(tst-1) * step + s" share the same delta
	 with the cell "tst * step + s" for all test=1..._seqList.count()/step
	 and for all s=0...step-1.
      */
      for ( int tst = 1; ok && ( tst * step < _seqList.count() ); tst++ )
      {
	for ( int s = 0; ok && ( s < step ); s++ )
	{
	  if ( !_seqList.at( (tst-1) * step + s )->
	       matches( _seqList.at( tst * step + s ), deltaList.at( s ) ) )
	    ok = false;
	}
      }
      // Did we find a valid interval ?
      if ( ok )
      {
	int s = 0;
	// Amount of intervals (blocks)
	int block = _seqList.count() / step;

	// Start iterating with the first cell
	Cell * cell;
        int destIndex = 0;
        if (down)
          cell = _destList.first();
        else
        {
          cell = _destList.last();
          destIndex = _destList.count() - 1;
          block -= (_seqList.count() - 1);
        }


	// Loop over all destination cells
	while ( cell )
	{
          kDebug() << "Valid interval, cell: " << cell->row() << " block: " << block << endl;

	  // End of block? -> start again from beginning
          if (down)
          {
            if ( s == step )
            {
              ++block;
              s = 0;
            }
          }
          else
          {
            if ( s >= step )
            {
              s = step - 1;
              ++block;
            }
          }

          kDebug() << "Step: " << step << " S: " << s << " Block " << block
                    << " SeqList: " << _seqList.count()
                    << " SrcList: " << _srcList.count() << " DeltaList: " << deltaList.count()
                    << endl;

	  // Set the value of 'cell' by adding 'block' times the delta tp the
	  // value of cell 's'.
	  _seqList.at( s )->fillCell( _srcList.at( s ), cell,
				      deltaList.at( s ), block, down );

          if (down)
          {
            // Next cell
            cell = _destList.value( ++destIndex );
            ++s;
          }
          else
          {
            // Previous cell
            cell = _destList.value( --destIndex );
            --s;
          }
	}
      }
    }
  }
  qDeleteAll( deltaList );
  return ok;
}

void Sheet::fillSequenceWithCopy( const QList<Cell*>& _srcList,
                                  const QList<Cell*>& _destList,
                                  bool down )
{
  // We did not find any valid interval. So just copy over the marked
  // area.
  Cell * cell;
  int destIndex = 0;

  if (down)
    cell = _destList.first();
  else
  {
    cell = _destList.last();
    destIndex = _destList.count() - 1;
  }
  int incr = 1;
  int s = 0;
  double factor = 1;

  if (!down)
    s = _srcList.count() - 1;

  if ( _srcList.at( s )->value().isNumber() &&
       !(_srcList.at( s )->isDate() || _srcList.at( s )->isTime() ) )
    factor = _srcList.at( s )->value().asFloat();

  while ( cell )
  {
    if (down)
    {
      if ( s == _srcList.count() )
        s = 0;
    }
    else
    {
      if ( s >= _srcList.count() )
        s = _srcList.count() - 1;
    }

    if ( !_srcList.at( s )->text().isEmpty() )
    {
      if ( _srcList.at( s )->isFormula() )
      {
        QString d = _srcList.at( s )->encodeFormula();
        cell->setCellText( cell->decodeFormula( d ) );
      }
      else if(_srcList.at( s )->value().isNumber() && _srcList.count()==1)
      {
        double val;
        int format_type = _srcList.at( s )->formatType();
        if ( format_type == Percentage_format )
        {
          factor = 0.01;  // one percent
        }
        else if ( _srcList.at( s )->isTime() )
        {
          // FIXME this is a workaround to avoid those nasty one minute off
          //       "dragging down" time is inaccurate overa large lists!
          //       This is the best approximation I could find (raphael)
          if (down)
          {
//             factor = 1.000002/24.  + 0.000000001;
            factor = 0.041666751;
          }
          else
          {  //when dragging "up" the error must of course be the other way round
            factor = 0.0416665;
          }
        }

        if (!down)
          val = (_srcList.at( s )->value().asFloat() - (incr * factor));
        else
          val = (_srcList.at( s )->value().asFloat() + (incr * factor));

        QString tmp;
        tmp = tmp.setNum(val);
        cell->setCellText( tmp );
        ++incr;
      }
      else if ( !AutoFillSequenceItem::month->isEmpty()
	        && AutoFillSequenceItem::month->contains( _srcList.at( s )->text())
	        && _srcList.count() == 1 )
      {
	QString strMonth=_srcList.at( s )->text();
	int i = AutoFillSequenceItem::month->indexOf( strMonth )+incr;
	int k = (i) % AutoFillSequenceItem::month->count();
	cell->setCellText((AutoFillSequenceItem::month->at( k )));
        incr++;
      }
      else if ( AutoFillSequenceItem::day->isEmpty()
	        && AutoFillSequenceItem::day->contains( _srcList.at( s )->text())
	        && _srcList.count()==1 )
      {
	QString strDay=_srcList.at( s )->text();
	int i = AutoFillSequenceItem::day->indexOf( strDay )+incr;
	int k = (i) % AutoFillSequenceItem::day->count();
	cell->setCellText((AutoFillSequenceItem::day->at( k )));
        incr++;
      }
      else
      {
	QRegExp number("(\\d+)");
	int pos =number.indexIn(_srcList.at( s )->text());
	if( pos!=-1 )
	{
	  QString tmp=number.cap(1);
	  int num=tmp.toInt()+incr;
	  cell->setCellText(_srcList.at( s )->text().replace(number,QString::number(num)));
          ++incr;
	}
	else if ( !_srcList.at( s )->link().isEmpty() )
        {
	  cell->setCellText( _srcList.at( s )->text() );
	  cell->setLink( _srcList.at( s )->link() );
        }
        else
        {
	  cell->setCellText( _srcList.at( s )->text() );
        }
      }
    }
    else
      cell->setCellText( "" );

    cell->copyFormat( _srcList.at( s ) );

    if (down)
    {
      cell = _destList.value( ++destIndex );
      ++s;
    }
    else
    {
      cell = _destList.value( --destIndex );
      --s;
    }
  }
  return;
}
