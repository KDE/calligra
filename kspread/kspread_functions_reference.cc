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

// built-in logical functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <kspread_doc.h>
#include <kspread_functions.h>
#include <kspread_util.h>
#include <kspread_table.h>

// prototypes (sorted alphabetically)
bool kspreadfunc_address( KSContext & context );
bool kspreadfunc_areas( KSContext & context );
bool kspreadfunc_choose( KSContext & context );
bool kspreadfunc_column( KSContext & context );
bool kspreadfunc_columns( KSContext & context );
bool kspreadfunc_indirect( KSContext & context );
bool kspreadfunc_lookup( KSContext & context );
bool kspreadfunc_row( KSContext & context );
bool kspreadfunc_rows( KSContext & context );

// registers all reference functions
void KSpreadRegisterReferenceFunctions()
{
  KSpreadFunctionRepository * repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "ADDRESS",  kspreadfunc_address );
  repo->registerFunction( "AREAS",    kspreadfunc_areas );
  repo->registerFunction( "CHOOSE",   kspreadfunc_choose );
  repo->registerFunction( "COLUMN",   kspreadfunc_column );
  repo->registerFunction( "COLUMNS",  kspreadfunc_columns );
  repo->registerFunction( "INDIRECT", kspreadfunc_indirect );
  repo->registerFunction( "LOOKUP",   kspreadfunc_lookup );
  repo->registerFunction( "ROW",      kspreadfunc_row );
  repo->registerFunction( "ROWS",     kspreadfunc_rows );
}

// Function: ADDRESS
bool kspreadfunc_address( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();
  bool r1c1 = false;
  QString tableName;
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
    tableName = args[4]->stringValue();
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  QString result;
  int col = args[0]->intValue();
  int row = args[1]->intValue();

  if ( !tableName.isEmpty() )
  {
    result += tableName;
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

    result += util_encodeColumnLabelText( col );

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
bool kspreadfunc_areas( KSContext & context )
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
bool kspreadfunc_choose( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();  

  int index = -1;
  int count = 0;

  for( ; it != end; ++it )
  {
    if ( index == - 1 )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        return false;
      
      index = args[0]->intValue();

      if ( index < 1 )
        return false;
    }
    else
    {
      // TODO: handle list values!
      if ( !KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
        return false;

      if ( count == index )
      {
        /*
          if ( extra.count() > count )
          {
          context.setValue( new KSValue( extra[count] ) );
          return true;
          }
        */
        context.setValue( new KSValue( (*it)->doubleValue() ) );
        return true;
      }
    }
    ++count;
  }

  return false;
}

// Function: COLUMN
bool kspreadfunc_column( KSContext & context )
{
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();
  KSpreadCell *  cell  = 0;//((KSpreadInterpreter *) context.interpreter() )->cell();

  if ( !KSUtil::checkArgumentsCount( context, 1, "COLUMN", false ) )
  {
    if ( cell )
      return cell->column();
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
bool kspreadfunc_columns( KSContext & context )
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
bool kspreadfunc_indirect( KSContext & context )
{
  QValueList<KSValue::Ptr> & args  = context.value()->listValue();
  QValueList<KSValue::Ptr> & extra = context.value()->listValue();

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

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString ref( args[0]->stringValue() );

  if ( ref.isEmpty() )
    return false;

  if ( r1c1 )
  {    
    // TODO: translate the r1c1 style to a1 style
    ref = ref;
  }
  
  KSpreadMap *   map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * sheet = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadPoint p( ref, map, sheet );
  if ( !p.isValid() || !p.isTableKnown() )
    return false;

  KSpreadCell * cell = sheet->cellAt( p.pos.x(), p.pos.y() );
  if ( cell )
  {
    if ( cell->isString() )
      context.setValue( new KSValue( cell->valueString() ) );
    else if ( cell->isNumeric() )
      context.setValue( new KSValue( cell->valueDouble() ) );
    else if ( cell->isBool() )
      context.setValue( new KSValue( cell->valueBool() ) );
    else if ( cell->isDate() )
      context.setValue( new KSValue( cell->valueDate() ) );
    else if ( cell->isTime() )
      context.setValue( new KSValue( cell->valueTime() ) );
    else
      context.setValue( new KSValue( cell->strOutText() ) );
      
    return true;    
  }

  return false;
}

static bool isEqualLess( KSContext & context, KSpreadCell::DataType type, KSValue::Ptr const & value, 
                         double dValue, QString const & sValue, bool bValue )
{
  if ( ( type == KSpreadCell::NumericData )
       && ( KSUtil::checkType( context, value, KSValue::DoubleType, true ) ) )
  {
    kdDebug() << "Values: " << value->doubleValue() << endl;
    if ( dValue <= value->doubleValue() )
      return true;

    return false;
  }

  else if ( type == KSpreadCell::NumericData )
    return true;

  else if ( ( type == KSpreadCell::StringData )
            && ( KSUtil::checkType( context, value, KSValue::StringType, true ) ) )
  {
    if ( sValue.lower() <= value->stringValue().lower() )
      return true;

    return false;
  }

  else if ( type == KSpreadCell::StringData )
    return true;

  else if ( ( type == KSpreadCell::BoolData )
            && ( KSUtil::checkType( context, value, KSValue::BoolType, true ) ) )
  {
    if ( (int) bValue <= (int) value->boolValue() )
      return true;
  }

  return false;
}

bool kspreadfunc_lookup( KSContext & context )
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
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "LOOKUP", true ) )
    return false;

  kdDebug() << "H1 " << endl;
  KSpreadCell::DataType type;

  if ( KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
    type = KSpreadCell::BoolData;
  else if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    type = KSpreadCell::NumericData;
  else
    return false;
  kdDebug() << "H2 " << endl;
  
  int index = -1;

  double dValue = 0.0;
  QString sValue;
  bool bValue = false;

  if ( type == KSpreadCell::NumericData )
    dValue = args[0]->doubleValue();
  if ( type == KSpreadCell::StringData )
    sValue = args[0]->stringValue();
  if ( type == KSpreadCell::BoolData )
    bValue = args[0]->boolValue();   

  // single value / no list
  if ( !KSUtil::checkType( context, args[1], KSValue::ListType, true ) )
  {
    if ( KSUtil::checkType( context, args[2], KSValue::ListType, true ) )
      return false;

    kdDebug() << "Here1 " << endl;
    if ( ( type == KSpreadCell::NumericData )
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
    if ( type == KSpreadCell::NumericData )
    {
      context.setValue( new KSValue( args[2] ) );
      return true;
    }
    else if ( ( type == KSpreadCell::StringData )
              && ( KSUtil::checkType( context, args[1], KSValue::StringType, true ) ) )
    {
      if ( sValue.lower() <= args[1]->stringValue().lower() )
      {
        context.setValue( new KSValue( args[2] ) );
        return true;
      }
      return false;
    }
    else if ( type == KSpreadCell::StringData )
    {
      context.setValue( new KSValue( args[2] ) );
      return true;
    }    
    else if ( ( type == KSpreadCell::BoolData )
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

  kdDebug() << "Here2 " << endl;
  QValueList<KSValue::Ptr> lookup( args[1]->listValue() );
  QValueList<KSValue::Ptr> & result = args[2]->listValue();

  if ( lookup.count() != result.count() )
    return false;

  QValueList<KSValue::Ptr>::Iterator it  = lookup.begin();
  QValueList<KSValue::Ptr>::Iterator end = lookup.end();  

  index = -1;
  kdDebug() << "Start" << endl;
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
bool kspreadfunc_row( KSContext & context )
{
  QValueList<KSValue::Ptr> & extra = context.extraData()->listValue();

  KSpreadSheet * table = ((KSpreadInterpreter *) context.interpreter() )->table();
  KSpreadCell *  cell  = 0;//((KSpreadInterpreter *) context.interpreter() )->cell();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ROW", false ) )
  {
    if ( cell )
      return cell->row();
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
bool kspreadfunc_rows( KSContext & context )
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

