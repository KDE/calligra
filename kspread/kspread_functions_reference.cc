/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

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

// built-in reference functions

/*
All these functions are temporarily disabled.
Reasons are the same as with the database functions.
Not all of these functions require range info, but almost all do.
*/

#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_util.h"
#include "kspread_value.h"

#include "functions.h"
#include "valuecalc.h"

using namespace KSpread;

// prototypes (sorted alphabetically)
KSpreadValue func_address (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_areas (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_choose (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_column (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_columns (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_indirect (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_lookup (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_row (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_rows (valVector args, ValueCalc *calc, FuncExtra *);

// registers all reference functions
void KSpreadRegisterReferenceFunctions()
{
#if 0
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;
  
  f = new Function ("ADDRESS",  func_address);
  repo->add (f);
  f = new Function ("AREAS",    func_areas);
  repo->add (f);
  f = new Function ("CHOOSE",   func_choose);
  repo->add (f);
  f = new Function ("COLUMN",   func_column);
  repo->add (f);
  f = new Function ("COLUMNS",  func_columns);
  repo->add (f);
  f = new Function ("INDIRECT", func_indirect);
  repo->add (f);
  f = new Function ("LOOKUP",   func_lookup);
  repo->add (f);
  f = new Function ("ROW",      func_row);
  repo->add (f);
  f = new Function ("ROWS",     func_rows);
  repo->add (f);
#endif
}

#if 0
// Function: ADDRESS
KSpreadValue func_address (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();
  bool r1c1 = false;
  QString sheetName;
  int absNum = 1;

  if ( !KSUtil::checkArgumentsCount( context, 5, "ADDRESS", false ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, "ADDRESS", false ) )
    {
      r1c1 = false;

      if ( !KSUtil::checkArgumentsCount( context, 3, "ADDRESS", false ) )
      {
        absNum = 1;

        if ( !KSUtil::checkArgumentsCount( context, 2, "ADDRESS", true ) )
          return false;
      }
      else
      {
        if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
          return false;

        absNum = args[2]->intValue();
      }
    }
    else
    {
      if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
        return false;

      if ( !KSUtil::checkType( context, args[3], KSValue::BoolType, true ) )
        return false;

      absNum = args[2]->intValue();
      r1c1   = !args[3]->boolValue();
    }
  }
  else
  {
    if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
      return false;

    if ( !KSUtil::checkType( context, args[3], KSValue::BoolType, true ) )
      return false;

    if ( !KSUtil::checkType( context, args[4], KSValue::StringType, true ) )
      return false;

    absNum    = args[2]->intValue();
    r1c1      = !args[3]->boolValue();
    sheetName = args[4]->stringValue();
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  QString result;
  int row = args[0]->intValue();
  int col = args[1]->intValue();

  if ( !sheetName.isEmpty() )
  {
    result += sheetName;
    result += "!";
  }

  if ( r1c1 )
  {
    // row first
    bool abs = false;
    if ( absNum == 1 || absNum == 2 )
      abs = true;

    result += 'R';
    if ( !abs )
      result += '[';
    result += QString::number( row );

    if ( !abs )
      result += ']';

    // column
    abs = false;
    if ( absNum == 1 || absNum == 3 )
      abs = true;

    result += 'C';
    if ( !abs )
      result += '[';
    result += QString::number( col );

    if ( !abs )
      result += ']';
  }
  else
  {
    bool abs = false;
    if ( absNum == 1 || absNum == 3 )
      abs = true;

    if ( abs )
      result += '$';

    result += KSpreadCell::columnName( col );

    abs = false;
    if ( absNum == 1 || absNum == 2 )
      abs = true;

    if ( abs )
      result += '$';

    result += QString::number( row );
  }

  context.setValue( new KSValue( result ) );

  return true;
}

bool checkRef( QString const & ref )
{
  KSpreadRange r( ref );
  if ( !r.isValid() )
  {
    KSpreadPoint p( ref );
    if ( !p.isValid() )
      return false;
  }
  return true;
}

// Function: AREAS
KSpreadValue func_areas (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( extra.count() > 0 )
  {
    context.setValue( new KSValue( (int) extra.count() ) );
    return true;
  }

  if ( !KSUtil::checkArgumentsCount( context, 1, "AREAS", true ) )
    return false;

  QString s = args[0]->stringValue();
  if ( s[0] != '(' || s[s.length() - 1] != ')' )
    return false;

  int l = s.length();

  int num = 0;
  QString ref;
  for ( int i = 1; i < l; ++i )
  {
    if ( s[i] == ',' || s[i] == ')' )
    {
      if ( !checkRef( ref ) )
        return false;
      else
      {
        ++num;
        ref = "";
      }
    }
    else
      ref += s[i];
  }

  context.setValue( new KSValue( num ) );
  return true;
}

// Function: CHOOSE
KSpreadValue func_choose (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  int index = -1;
  int count = 0;

  for( ; it != end; ++it )
  {
    if ( index == -1 )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        return false;

      index = args[0]->intValue();

      if ( index < 1 )
        return false;
    }
    else
    {
      if ( KSUtil::checkType( context, *it, KSValue::ListType, true ) )
      {
        QValueList<KSValue::Ptr> list( (*it)->listValue() );

        QValueList<KSValue::Ptr>::Iterator listIter = list.begin();
        QValueList<KSValue::Ptr>::Iterator listEnd  = list.end();

        if ( KSUtil::checkType( context, *listIter, KSValue::ListType, true ) )
        {
          list = (*listIter)->listValue();

          listIter = list.begin();
          listEnd  = list.end();
        }

        while ( listIter != listEnd )
        {
          if ( count == index )
          {
            if ( KSUtil::checkType( context, *listIter, KSValue::StringType, true ) )
              context.setValue( new KSValue( (*listIter)->stringValue()  ) );
            else if ( KSUtil::checkType( context, *listIter, KSValue::DoubleType, true ) )
              context.setValue( new KSValue( (*listIter)->doubleValue()  ) );
            else if ( KSUtil::checkType( context, *listIter, KSValue::BoolType, true ) )
              context.setValue( new KSValue( (*listIter)->boolValue()  ) );
            else if ( KSUtil::checkType( context, *listIter, KSValue::IntType, true ) )
              context.setValue( new KSValue( (*listIter)->intValue()  ) );
            else if ( KSUtil::checkType( context, *listIter, KSValue::DateType, true ) )
              context.setValue( new KSValue( (*listIter)->dateValue()  ) );
            else if ( KSUtil::checkType( context, *listIter, KSValue::TimeType, true ) )
              context.setValue( new KSValue( (*listIter)->timeValue()  ) );
            else
            {
              kdDebug() << "Unkown type" << endl;
              return false;
            }

            return true;
          }
          ++count;
          ++listIter;
        }
      }
      else if ( count == index )
      {
        if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
          context.setValue( new KSValue( (*it)->stringValue()  ) );
        else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
          context.setValue( new KSValue( (*it)->doubleValue()  ) );
        else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
          context.setValue( new KSValue( (*it)->boolValue()  ) );
        else if ( KSUtil::checkType( context, *it, KSValue::IntType, true ) )
          context.setValue( new KSValue( (*it)->intValue()  ) );
        else if ( KSUtil::checkType( context, *it, KSValue::DateType, true ) )
          context.setValue( new KSValue( (*it)->dateValue()  ) );
        else if ( KSUtil::checkType( context, *it, KSValue::TimeType, true ) )
          context.setValue( new KSValue( (*it)->timeValue()  ) );
        else
          return false;
        return true;
      }
    }
    ++count;
  }

  return false;
}

// Function: COLUMN
KSpreadValue func_column (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  KSpreadCell *  cell  = ((KSpreadInterpreter *) context.interpreter() )->cell();

  if ( !KSUtil::checkArgumentsCount( context, 1, "COLUMN", false ) )
  {
    if ( cell )
    {
      context.setValue( new KSValue( (int) cell->column() ) );
      return true;
    }
    else
      return false;
  }
  else
  {
    if ( extra.count() > 0 )
    {
      QString s( extra[0]->stringValue() );

      KSpreadRange ra( s );
      if ( ra.range.left() <= 0 || ra.range.right() <= 0 )
      {
        KSpreadPoint p( s );
        if ( p.pos.x() <= 0 || p.pos.y() <= 0 )
          return false;

        context.setValue( new KSValue( p.pos.x() ) );
        return true;
      }
      else //if ( ra.range.width() <= 1 )
      {
        context.setValue( new KSValue( ra.range.left() ) );
        return true;
      }
      /*
      else
      {
        QString result( "{" );
        int r = ra.range.right();
        for ( int i = ra.range.left(); i <= r; ++i )
        {
          if ( result.length() > 1 )
            result += ", ";
          result += QString::number( i );
        }
        result += "}";

        context.setValue( new KSValue( result ) );
        return true;
      }
      */
    }
  }

  return false;
}

// Function: COLUMNS
KSpreadValue func_columns (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "COLUMNS", true ) )
    return false;

  if ( extra.count() > 0 )
  {
    QString s( extra[0]->stringValue() );

    KSpreadRange r( s );
    if ( r.range.left() <= 0 || r.range.right() <= 0 )
    {
      KSpreadPoint p( s );
      if ( p.pos.x() <= 0 || p.pos.y() <= 0 )
        return false;

      context.setValue( new KSValue( (int) 1 ) );
      return true;
    }

    context.setValue( new KSValue( r.range.width() ) );
    return true;
  }
  return false;
  if ( args.count() > 0 )
  {
    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    QString s( args[0]->stringValue() );

    int l = s.length();
    int n = s.find( '[' );
    if ( n == -1 )
      return false;
    int n2 = s.find( '[', n + 1 );
    if ( n2 != -1 )
      n = n2;
    int count = 0;
    for ( int i = n; i < l; ++i )
    {
      if ( s[i] == ',' )
        ++count;
      else if ( s[i] == ']' )
      {
        ++count;
        break;
      }
    }

    context.setValue( new KSValue( count ) );
    return true;
  }

  return false;
}


// Function: INDIRECT
KSpreadValue func_indirect (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "INDIRECT", true ) )
    return false;

  bool r1c1 = false;
  if ( !KSUtil::checkArgumentsCount( context, 2, "INDIRECT", false ) )
    r1c1 = false;
  else
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::BoolType, true ) )
      return false;

    r1c1 = !args[1]->boolValue();
  }

  QString ref;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
  {
    if ( !KSUtil::checkType( context, extra[0], KSValue::StringType, true ) )
      return false;
    ref = extra[0]->stringValue();
  }
  else
    ref = args[0]->stringValue();

  if ( ref.isEmpty() )
    return false;

  if ( r1c1 )
  {
    // TODO: translate the r1c1 style to a1 style
    ref = ref;
  }

  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * sheet = ((KSpreadInterpreter *) context.interpreter() )->sheet();

  KSpreadPoint p( ref, map, sheet );

  if ( !p.isValid() )
    return false;

  KSpreadCell * cell = p.cell();
  if ( cell )
  {
    if ( cell->value().isString() )
      context.setValue( new KSValue( cell->value().asString() ) );
    else if ( cell->value().isNumber() )
      context.setValue( new KSValue( cell->value().asFloat() ) );
    else if ( cell->value().isBoolean() )
      context.setValue( new KSValue( cell->value().asBoolean() ) );
    else if ( cell->isDate() )
      context.setValue( new KSValue( cell->value().asDate() ) );
    else if ( cell->isTime() )
      context.setValue( new KSValue( cell->value().asTime() ) );
    else if ( cell->value().isEmpty() || cell->isEmpty() || cell->isDefault() )
      context.setValue( new KSValue( (int) 0 ) );
    else
      context.setValue( new KSValue( cell->strOutText() ) );

    return true;
  }

  return false;
}

static bool isEqualLess( KSContext & context, KSpreadValue::Type type, KSValue::Ptr const & value,
                         double dValue, QString const & sValue, bool bValue )
{
  if ( ( type == KSpreadValue::Float )
       && ( KSUtil::checkType( context, value, KSValue::DoubleType, true ) ) )
  {
    if ( dValue <= value->doubleValue() )
      return true;

    return false;
  }

  else if ( type == KSpreadValue::Integer || type == KSpreadValue::Float )
    return true;

  else if ( ( type == KSpreadValue::String )
            && ( KSUtil::checkType( context, value, KSValue::StringType, true ) ) )
  {
    if ( sValue.lower() <= value->stringValue().lower() )
      return true;

    return false;
  }

  else if ( type == KSpreadValue::String )
    return true;

  else if ( ( type == KSpreadValue::Boolean )
            && ( KSUtil::checkType( context, value, KSValue::BoolType, true ) ) )
  {
    if ( (int) bValue <= (int) value->boolValue() )
      return true;
  }

  return false;
}

KSpreadValue func_lookup (valVector args, ValueCalc *calc, FuncExtra *)
{
  /* As soon as it works correctly: here is the doc
    <Function>
      <Name>LOOKUP</Name>
      <Parameter>
        <Comment>Lookup Value</Comment>
        <Type>String/Numeric</Type>
      </Parameter>
      <Parameter>
        <Comment>Lookup Vector</Comment>
        <Type>String/Numeric</Type>
      </Parameter>
      <Parameter>
        <Comment>Result Vector</Comment>
        <Type>String/Numeric</Type>
      </Parameter>
      <Help>
	<Text>The LOOKUP function looks up the first parameter in the lookup vector. It returns a value in the result Vector with the same index as the matching value in the lookup vector. If value is not in the lookup vector it takes the next lower one. If no value in the lookup vector matches an error is returned. The lookup vector must be in ascending order and lookup and result vector must have the same size. Numeric values, string and boolean values are recognized. Comparison between strings is case-insensitive.</Text>
	<Syntax>LOOKUP(value, lookup vector, result vector)</Syntax>
	<Example>LOOKUP(1.232; A1:A6; B1:B6) for A1 = 1, A2 = 2 returns the value of B1.</Example>
      </Help>
    </Function>
*/
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "LOOKUP", true ) )
    return false;

  KSpreadValue::Type type;

  if ( KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
    type = KSpreadValue::Boolean;
  else if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    type = KSpreadValue::Float;
  else
    return false;

  int index = -1;

  double dValue = 0.0;
  QString sValue;
  bool bValue = false;

  if ( type == KSpreadValue::Float || type == KSpreadValue::Integer )
    dValue = args[0]->doubleValue();
  if ( type == KSpreadValue::String )
    sValue = args[0]->stringValue();
  if ( type == KSpreadValue::Boolean )
    bValue = args[0]->boolValue();

  // single value / no list
  if ( !KSUtil::checkType( context, args[1], KSValue::ListType, true ) )
  {
    if ( KSUtil::checkType( context, args[2], KSValue::ListType, true ) )
      return false;

    if ( ( type == KSpreadValue::Float || type == KSpreadValue::Integer )
         && ( KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) ) )
    {
      if ( dValue <= args[1]->doubleValue() )
      {
        context.setValue( new KSValue( args[2] ) );
        return true;
      }
      return false;
    }
    else
    if ( type == KSpreadValue::Float || type == KSpreadValue::Integer )
    {
      context.setValue( new KSValue( args[2] ) );
      return true;
    }
    else if ( ( type == KSpreadValue::String )
              && ( KSUtil::checkType( context, args[1], KSValue::StringType, true ) ) )
    {
      if ( sValue.lower() <= args[1]->stringValue().lower() )
      {
        context.setValue( new KSValue( args[2] ) );
        return true;
      }
      return false;
    }
    else if ( type == KSpreadValue::String )
    {
      context.setValue( new KSValue( args[2] ) );
      return true;
    }
    else if ( ( type == KSpreadValue::Boolean )
              && ( KSUtil::checkType( context, args[1], KSValue::BoolType, true ) ) )
    {
      if ( bValue <= args[1]->boolValue() )
      {
        context.setValue( new KSValue( args[2] ) );
        return true;
      }
    }

    return false;
  }

  QValueList<KSValue::Ptr> lookup( args[1]->listValue() );
  QValueList<KSValue::Ptr> & result = args[2]->listValue();

  if ( lookup.count() != result.count() )
    return false;

  index = -1;
  int l = lookup.count();
  for ( int i = 0; i < l; ++i ) // ( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, lookup[i], KSValue::DoubleType, true ) )
      kdDebug() << "Double" << endl;
    if ( KSUtil::checkType( context, lookup[i], KSValue::ListType, true ) )
      kdDebug() << "List" << endl;
    if ( KSUtil::checkType( context, lookup[i], KSValue::IntType, true ) )
      kdDebug() << "Int" << endl;
    if ( KSUtil::checkType( context, lookup[i], KSValue::StringType, true ) )
      kdDebug() << "String" << endl;
    kdDebug() << "index: " << index << ", " << dValue << endl;
    if ( !isEqualLess( context, type, lookup[i], dValue, sValue, bValue ) )
    {
      if ( index < 0 )
        return false;
      context.setValue( new KSValue( result[index] ) );
      return true;
    }

    ++index;
  }

  return false;
}

// Function: ROW
KSpreadValue func_row (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  KSpreadCell *  cell  = ((KSpreadInterpreter *) context.interpreter() )->cell();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ROW", false ) )
  {
    if ( cell )
    {
      context.setValue( new KSValue( (int) cell->row() ) );
      return true;
    }
    else
      return false;
  }
  else
  {
    if ( extra.count() > 0 )
    {
      QString s( extra[0]->stringValue() );

      KSpreadRange r( s );
      if ( r.range.left() <= 0 || r.range.right() <= 0 )
      {
        KSpreadPoint p( s );
        if ( p.pos.y() <= 0 || p.pos.x() <= 0 )
          return false;

        context.setValue( new KSValue( p.pos.y() ) );
        return true;
      }
      else
      {
        /*
        QString result( "{" );
        int b = r.range.bottom();
        for ( int i = r.range.top(); i < b; ++i )
        {
          if ( result.length() > 1 )
            result += ", ";
          result += QString::number( i );
        }
        result += "}";
        */
        context.setValue( new KSValue( r.range.top() ) );
        return true;
      }

    }
  }

  return false;
}

// Function: ROWS
KSpreadValue func_rows (valVector args, ValueCalc *calc, FuncExtra *)
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ROWS", true ) )
    return false;

  if ( extra.count() > 0 )
  {
    QString s( extra[0]->stringValue() );
    KSpreadRange r( s );
    if ( r.range.left() <= 0 || r.range.right() <= 0 )
    {
      KSpreadPoint p( s );
      if ( p.pos.y() <= 0 || p.pos.x() <= 0 )
        return false;

      context.setValue( new KSValue( (int) 1 ) );
      return true;
    }

    context.setValue( new KSValue( r.range.height() ) );
    return true;
  }
  else if ( args.count() > 0 )
  {
    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    QString s( args[0]->stringValue() );
    int l = s.length();
    int n = s.find( '[' );
    if ( n == -1 )
      return false;
    int n2 = s.find( '[', n + 1 );
    if ( n2 != -1 )
      n = n2;
    int count = 0;
    for ( int i = n; i < l; ++i )
    {
      if ( s[i] == '[' )
        ++count;
    }

    context.setValue( new KSValue( count ) );
    return true;
  }

  return false;
}

#endif