/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// built-in database functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <kspread_cell.h>
#include <kspread_doc.h>
#include <kspread_functions.h>
#include <kspread_map.h>
#include <kspread_table.h>
#include <kspread_util.h>

#include <qmemarray.h>
#include <qptrlist.h>
#include <qrect.h>
#include <qvaluelist.h>

enum Comp { isEqual, isLess, isGreater, lessEqual, greaterEqual, notEqual };
enum Type { numeric, string };

struct Condition
{
  Comp     comp;
  int      index;
  double   value;
  QString  stringValue;
  Type     type;
};

typedef QValueList<Condition> ConditionList;

// prototypes
bool kspreadfunc_dsum( KSContext& context );

// registers all math functions
void KSpreadRegisterDatabaseFunctions()
{
  KSpreadFunctionRepository * repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "DSUM",       kspreadfunc_dsum );
}

/*********************************************************************
 *
 * Helper function to avoid problems with rounding floating point
 * values. Idea for this kind of solution taken from Openoffice.
 *
 *********************************************************************/

static bool approx_equal (double a, double b)
{
  if ( a == b )
    return TRUE;
  double x = a - b;
  return (x < 0.0 ? -x : x)  <  ((a < 0.0 ? -a : a) * DBL_EPSILON);
}

void getCond( Condition & cond, QString text )
{
  text = text.stripWhiteSpace();

  if ( text.startsWith( "<=" ) )
  {
    cond.comp = lessEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( ">=" ) )
  {
    cond.comp = greaterEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "!=" ) || text.startsWith( "<>" ) )
  {
    cond.comp = notEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "==" ) )
  {
    cond.comp = isEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "<" ) )
  {
    cond.comp = isLess;
    text = text.remove( 0, 1 );
  }          
  else if ( text.startsWith( ">" ) )
  {
    cond.comp = isGreater;
    text = text.remove( 0, 1 );
  }          
  else if ( text.startsWith( "=" ) )
  {
    cond.comp = isEqual;
    text = text.remove( 0, 1 );
  }          

  text = text.stripWhiteSpace();

  bool ok = false;
  double d = text.toDouble( &ok );
  if ( ok )
  {
    cond.type = numeric;
    cond.value = d;
    kdDebug() << "Numeric: " << d << ", Op: " << cond.comp << endl;
  }
  else
  {
    cond.type = string;
    cond.stringValue = text;
    kdDebug() << "String: " << text << ", Op: " << cond.comp << endl;
  }
}

int getFieldIndex( QString const & fieldname, QRect const & database, KSpreadTable * table )
{
  int r   = database.right();
  int row = database.top();
  KSpreadCell * cell = 0;

  kdDebug() << "Database: " << database.left() << ", " << row << ", right: " << r << endl;

  for ( int i = database.left(); i <= r; ++i )
  {
    cell = table->cellAt( i, row );
    if ( cell->isDefault() )
      continue;

    if ( fieldname == cell->strOutText() )
      return i;
  }

  return -1;
}

void parseConditions( QPtrList<ConditionList> * result, QRect const & database, QRect const & conditions, KSpreadTable * table )
{
  int cCols  = conditions.width();
  int right  = conditions.right();
  int left   = conditions.left();
  int top    = conditions.top();
  int bottom = conditions.bottom();

  QMemArray<int> list( cCols );
  KSpreadCell * cell = 0;

  kdDebug() << "Top: " << top << ", Left: " << left << ", right: " << right << ", " << bottom << endl;

  // Save the databases indices of condition header
  for ( int i = left; i <= right; ++i )
  {
    cell = table->cellAt( i, top );
    if ( cell->isDefault() || cell->isEmpty() )
      list[i - 1] = -1;
    else
    {
      int p = getFieldIndex( cell->strOutText(), database, table );
      list[i - 1] = p;

      kdDebug() << "header: " << cell->strOutText() << ", " << list[i] << ", P: " << p << endl;
    }
  }

  for ( int r = top + 1; r <= bottom; ++r ) // first row are headers
  {
    ConditionList * criteria = new ConditionList();
    
    for ( int c = 0; c < cCols; ++c )
    {
      if ( list[c] == -1 )
        continue;

      Condition cond;
      cond.index = list[c];

      kdDebug() << "Cell: " << c+left << ", " << r << ", Str: " 
                << table->cellAt( c + left, r )->strOutText() << ", index: " << list[c] << endl;

      getCond( cond, table->cellAt( c + left, r )->strOutText() );

      criteria->append( cond );
    }

    result->append( criteria );
  }
  kdDebug() << "Criterias: " << result->count() << endl;
}

bool conditionMatches( Condition cond, KSpreadCell * cell )
{
  if ( !cell || cell->isEmpty() || cell->isDefault() )
  {
    kdDebug() << "Match: Cell is empty " << endl;
    return false;
  }

  if ( cond.type == numeric )
  {
    if ( !cell->isNumeric() )
      return false;
    
    double d = cell->valueDouble();

    kdDebug() << "Comparing: " << d << " - " << cond.value << "; Comp: " << cond.comp << endl;

    switch ( cond.comp )
    {
     case isEqual: 
      if ( approx_equal( d, cond.value ) )
        return true;

      return false;

     case isLess: 
      if ( d < cond.value )
        return true;

      return false;

     case isGreater: 
      if ( d > cond.value )
        return true;

      return false;

     case lessEqual: 
      if ( d <= cond.value )
        return true;

      return false;

     case greaterEqual: 
      if ( d >= cond.value )
        return true;

      return false;

     case notEqual: 
      if ( d != cond.value )
        return true;

      return false;

     default:
      return false;
    }
  }
  else 
  {
    QString d = cell->strOutText();
    kdDebug() << "String: " << d << endl;

    switch ( cond.comp )
    {
     case isEqual: 
      if ( d == cond.stringValue )
        return true;

      return false;

     case isLess: 
      if ( d < cond.stringValue )
        return true;

      return false;

     case isGreater: 
      if ( d > cond.stringValue )
        return true;

      return false;

     case lessEqual: 
      if ( d <= cond.stringValue )
        return true;

      return false;

     case greaterEqual: 
      if ( d >= cond.stringValue )
        return true;

      return false;

     case notEqual: 
      if ( d != cond.stringValue )
        return true;

      return false;

     default:
      return false;
    }
  }

  return true;
}

QPtrList<KSpreadCell> * getCellList( QRect const & db, KSpreadTable * table, int column, QPtrList<ConditionList> * conditions )
{
  kdDebug() << "***** getCellList *****" << endl;

  int top    = db.top();
  int bottom = db.bottom();

  QPtrList<KSpreadCell> * result = new QPtrList<KSpreadCell>();
  result->setAutoDelete( false ); // better not delete the cells...

  QValueList<Condition>::const_iterator it;
  QValueList<Condition>::const_iterator end;
  KSpreadCell * cell    = 0;
  KSpreadCell * conCell = 0;

  for ( int row = top + 1; row <= bottom; ++row ) // first row contains header
  {
    cell = table->cellAt( column, row );
    kdDebug() << "Cell: " << column << ", " << row << " - " << cell->strOutText() << endl;
    if ( cell->isDefault() || !cell->isNumeric() )
      continue;

    // go through conditions
    //   go through criterias => all have to match
    //   if first condition matches => add cell, next row
    ConditionList * criterias = conditions->first();

    bool add = true; 
    while ( criterias )
    {
      add = true;

      it  = criterias->begin();
      end = criterias->end();

      for ( ; it != end; ++it )
      {        
        Condition cond = *it;
        conCell = table->cellAt( cond.index, row );
        kdDebug() << "Checking cell: " << cond.index << ", " << row << " - " << conCell->strOutText() << endl;
        if ( !conditionMatches( cond, conCell ) )
        {
          add = false;
          break; // all conditions per criteria must match, but just one criteria
        }
      }  
      if ( add )
        break; // just one criteria need to match

      criterias = conditions->next();
    }
    if ( add )
    {
      kdDebug() << "Appending cell: " << cell->strOutText() << endl;
      result->append( cell );
    }
  } // for row =...

  return result;
}


// Function: DSUM
bool kspreadfunc_dsum( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "dsum", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadTable * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;
  
  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<ConditionList> * cond = new QPtrList<ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double sum = 0;

  KSpreadCell * cell = cells->first();
  while ( cell )
  {
    if ( cell->isNumeric() )
      sum += cell->valueDouble();

    cell = cells->next();
  }

  context.setValue( new KSValue( sum ) );

  delete cond;
  delete cells;

  return true;
}

