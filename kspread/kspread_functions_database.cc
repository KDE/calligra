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

#include "kspread_cell.h"
#include "kspread_sheet.h"
#include "kspread_interpreter.h"
#include "kspread_doc.h"
#include "kspread_functions.h"
#include "kspread_functions_helper.h"
#include "kspread_util.h"

#include <qmemarray.h>
#include <qptrlist.h>
#include <qrect.h>
#include <qvaluelist.h>

// prototypes
bool kspreadfunc_daverage( KSContext & context );
bool kspreadfunc_dcount( KSContext & context );
bool kspreadfunc_dcounta( KSContext & context );
bool kspreadfunc_dget( KSContext & context );
bool kspreadfunc_dmax( KSContext & context );
bool kspreadfunc_dmin( KSContext & context );
bool kspreadfunc_dproduct( KSContext & context );
bool kspreadfunc_dstdev( KSContext & context );
bool kspreadfunc_dstdevp( KSContext & context );
bool kspreadfunc_dsum( KSContext & context );
bool kspreadfunc_dvar( KSContext & context );
bool kspreadfunc_dvarp( KSContext & context );
bool kspreadfunc_getpivotdata( KSContext & context );

// registers all database functions
void KSpreadRegisterDatabaseFunctions()
{
  KSpreadFunctionRepository * repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "DAVERAGE",     kspreadfunc_daverage );
  repo->registerFunction( "DCOUNT",       kspreadfunc_dcount );
  repo->registerFunction( "DCOUNTA",      kspreadfunc_dcounta );
  repo->registerFunction( "DGET",         kspreadfunc_dget );
  repo->registerFunction( "DMAX",         kspreadfunc_dmax );
  repo->registerFunction( "DMIN",         kspreadfunc_dmin );
  repo->registerFunction( "DPRODUCT",     kspreadfunc_dproduct );
  repo->registerFunction( "DSTDEV",       kspreadfunc_dstdev );
  repo->registerFunction( "DSTDEVP",      kspreadfunc_dstdevp );
  repo->registerFunction( "DSUM",         kspreadfunc_dsum );
  repo->registerFunction( "DVAR",         kspreadfunc_dvar );
  repo->registerFunction( "DVARP",        kspreadfunc_dvarp );
  repo->registerFunction( "GETPIVOTDATA", kspreadfunc_getpivotdata ); // partially Excel-compatible
}

/*********************************************************************
 *
 * Helper function to avoid problems with rounding floating point
 * values. Idea for this kind of solution taken from Openoffice.
 *
 *********************************************************************/

/**
 * Use the implementation from kspread_functions_helper.cc instead.
 *
static bool approx_equal (double a, double b)
{
  if ( a == b )
    return TRUE;
  double x = a - b;
  return (x < 0.0 ? -x : x)  <  ((a < 0.0 ? -a : a) * DBL_EPSILON);
}
 */

bool conditionMatches( KSpreadDB::Condition &cond, KSpreadCell * cell )
{
  if ( !cell || cell->isEmpty() || cell->isDefault() )
  {
    kdDebug() << "Match: Cell is empty " << endl;
    return false;
  }

  if ( cond.type == KSpreadDB::numeric && cell->value().isNumber() ) {
    double d = cell->value().asFloat();
    return conditionMatches( cond, d );
  }
  if ( cond.type == KSpreadDB::string && cell->value().isString() ) {
    QString d = cell->strOutText();
    return conditionMatches( cond, d );
  }

  return false;
}

int getFieldIndex( QString const & fieldname, QRect const & database, KSpreadSheet * table )
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

    if ( fieldname.lower() == cell->strOutText().lower() )
      return i;
  }

  return -1;
}

void parseConditions( QPtrList<KSpreadDB::ConditionList> * result, QRect const & database, QRect const & conditions, KSpreadSheet * table )
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
    KSpreadDB::ConditionList * criteria = new KSpreadDB::ConditionList();

    for ( int c = 0; c < cCols; ++c )
    {
      if ( list[c] == -1 )
        continue;

      KSpreadDB::Condition cond;
      cond.index = list[c];

      kdDebug() << "Cell: " << c+left << ", " << r << ", Str: "
                << table->cellAt( c + left, r )->strOutText() << ", index: " << list[c] << endl;

      if( !table->cellAt( c + left,r )->isEmpty() )
      {
        getCond( cond, table->cellAt( c + left, r )->strOutText() );
        criteria->append( cond );
      }
    }

    result->append( criteria );
  }
  kdDebug() << "Criterias: " << result->count() << endl;
}

QPtrList<KSpreadCell> * getCellList( QRect const & db, KSpreadSheet * table, int column, QPtrList<KSpreadDB::ConditionList> * conditions )
{
  kdDebug() << "***** getCellList *****" << endl;

  int top    = db.top();
  int bottom = db.bottom();

  QPtrList<KSpreadCell> * result = new QPtrList<KSpreadCell>();
  result->setAutoDelete( false ); // better not delete the cells...

  QValueList<KSpreadDB::Condition>::const_iterator it;
  QValueList<KSpreadDB::Condition>::const_iterator end;
  KSpreadCell * cell    = 0;
  KSpreadCell * conCell = 0;

  for ( int row = top + 1; row <= bottom; ++row ) // first row contains header
  {
    cell = table->cellAt( column, row );
    kdDebug() << "Cell: " << column << ", " << row << " - " << cell->strOutText() << endl;
    if ( cell->isDefault() )
      continue;

    // go through conditions
    //   go through criterias => all have to match
    //   if first condition matches => add cell, next row
    KSpreadDB::ConditionList * criterias = conditions->first();

    bool add = true;
    while ( criterias )
    {
      add = true;

      it  = criterias->begin();
      end = criterias->end();

      for ( ; it != end; ++it )
      {
        KSpreadDB::Condition cond = *it;
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
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double sum = 0;

  KSpreadCell * cell = cells->first();
  while ( cell )
  {
    if ( cell->value().isNumber() )
      sum += cell->value().asFloat();

    cell = cells->next();
  }

  context.setValue( new KSValue( sum ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DAVERAGE
bool kspreadfunc_daverage( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DAVERAGE", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  int    count = 0;
  double sum   = 0;

  KSpreadCell * cell = cells->first();
  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      ++count;
      sum += cell->value().asFloat();
    }

    cell = cells->next();
  }

  context.setValue( new KSValue( (double) ( sum / count) ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DCOUNT
bool kspreadfunc_dcount( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DCOUNT", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  int count = 0;

  KSpreadCell * cell = cells->first();
  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      ++count;
    }

    cell = cells->next();
  }

  context.setValue( new KSValue( count ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DCOUNTA
bool kspreadfunc_dcounta( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DCOUNTA", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  int count = 0;

  KSpreadCell * cell = cells->first();
  while ( cell )
  {
    if ( !cell->isEmpty() )
      ++count;

    cell = cells->next();
  }

  context.setValue( new KSValue( count ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DGET
bool kspreadfunc_dget( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DGET", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  KSValue value;
  int count = 0;

  KSpreadCell * cell = cells->first();

  while ( cell )
  {
    if ( !cell->isEmpty() )
    {
      ++count;
      if ( count > 1 )
        return false;

      if ( cell->value().isNumber() )
        value.setValue( cell->value().asFloat() );
      else if ( cell->value().isString() )
        value.setValue( cell->value().asString() );
      else if ( cell->value().isBoolean() )
        value.setValue( cell->value().asBoolean() );
      else
        return false;
    }

    cell = cells->next();
  }

  if ( count == 0 )
    return false;

  context.setValue( new KSValue( value ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DMAX
bool kspreadfunc_dmax( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DMAX", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double max = 0.0;

  KSpreadCell * cell = cells->first();
  if ( cell && cell->value().isNumber() )
    max = cell->value().asFloat();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      if ( cell->value().asFloat() > max )
        max = cell->value().asFloat();
    }

    cell = cells->next();
  }

  context.setValue( new KSValue( max ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DMIN
bool kspreadfunc_dmin( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DMIN", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double min = 0.0;

  KSpreadCell * cell = cells->first();
  if ( cell && cell->value().isNumber() )
    min = cell->value().asFloat();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      if ( cell->value().asFloat() < min )
        min = cell->value().asFloat();
    }

    cell = cells->next();
  }

  context.setValue( new KSValue( min ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DPRODUCT
bool kspreadfunc_dproduct( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DPRODUCT", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double product = 1.0;
  int count = 0;

  KSpreadCell * cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      ++count;
      product *= cell->value().asFloat();
    }

    cell = cells->next();
  }

  if ( count == 0 )
    return false;

  context.setValue( new KSValue( product ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DSTDEV
bool kspreadfunc_dstdev( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DSTDEV", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double sum = 0.0;
  int count = 0;

  KSpreadCell * cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      sum += cell->value().asFloat();
      ++count;
    }

    cell = cells->next();
  }

  if ( count == 0 )
    return false;

  double average = sum / count;
  double result = 0.0;

  cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      result += ( ( cell->value().asFloat() - average ) * ( cell->value().asFloat() - average ) );
    }

    cell = cells->next();
  }


  context.setValue( new KSValue( sqrt( result / ( ( double )( count - 1 ) ) ) ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DSTDEVP
bool kspreadfunc_dstdevp( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DSTDEVP", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double sum = 0.0;
  int count = 0;

  KSpreadCell * cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      sum += cell->value().asFloat();
      ++count;
    }

    cell = cells->next();
  }

  if ( count == 0 )
    return false;

  double average = sum / count;
  double result = 0.0;

  cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      result += ( ( cell->value().asFloat() - average ) * ( cell->value().asFloat() - average ) );
    }

    cell = cells->next();
  }

  context.setValue( new KSValue( sqrt( result / count ) ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DVAR
bool kspreadfunc_dvar( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DVAR", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double sum = 0.0;
  int count = 0;

  KSpreadCell * cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      sum += cell->value().asFloat();
      ++count;
    }

    cell = cells->next();
  }

  if ( count == 0 )
    return false;

  double average = sum / count;
  double result = 0.0;

  cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      result += ( ( cell->value().asFloat() - average ) * ( cell->value().asFloat() - average ) );
    }

    cell = cells->next();
  }

  context.setValue( new KSValue( result / (double) (count - 1) ) );

  delete cond;
  delete cells;

  return true;
}

// Function: DVARP
bool kspreadfunc_dvarp( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DVARP", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  KSpreadRange conditions( extra[2]->stringValue(), map, table );

  if ( !db.isValid() || !conditions.isValid() )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  QPtrList<KSpreadDB::ConditionList> * cond = new QPtrList<KSpreadDB::ConditionList>();
  cond->setAutoDelete( true );

  parseConditions( cond, db.range, conditions.range, table );

  QPtrList<KSpreadCell> * cells = getCellList( db.range, table, fieldIndex, cond );

  double sum = 0.0;
  int count = 0;

  KSpreadCell * cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      sum += cell->value().asFloat();
      ++count;
    }

    cell = cells->next();
  }

  if ( count == 0 )
    return false;

  double average = sum / count;
  double result = 0.0;

  cell = cells->first();

  while ( cell )
  {
    if ( cell->value().isNumber() )
    {
      result += ( ( cell->value().asFloat() - average ) * ( cell->value().asFloat() - average ) );
    }

    cell = cells->next();
  }

  context.setValue( new KSValue( result / count ) );

  delete cond;
  delete cells;

  return true;
}

// Function: GETPIVOTDATA
// FIXME implement more things with this, see Excel !
bool kspreadfunc_getpivotdata( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "GETPIVOTDATA", true ) )
    return false;

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadRange db( extra[0]->stringValue(), map, table );
  if ( !db.isValid()  )
    return false;

  int fieldIndex = getFieldIndex( args[1]->stringValue(), db.range, table );
  if ( fieldIndex == -1 )
    return false;

  kdDebug() << "Fieldindex: " << fieldIndex << endl;

  KSpreadCell * cell = table->cellAt( fieldIndex, db.range.bottom() );
  if( cell->isEmpty() )
    return false;

  KSValue value;
  if ( cell->value().isNumber() )
    value.setValue( cell->value().asFloat() );
  else if ( cell->value().isString() )
    value.setValue( cell->value().asString() );
  else if ( cell->value().isBoolean() )
    value.setValue( cell->value().asBoolean() );
  else
    return false;

  context.setValue( new KSValue( value ) );

  return true;
}
