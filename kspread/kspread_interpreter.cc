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

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_interpreter.h"

#include "kspread_functions.h"
#include "kspread_util.h"
#include "kspread_sheet.h"
#include "kspread_doc.h"
#include "kspread_locale.h"


/***************************************************************
 *
 * Classes which store extra informations in some KSParseNode.
 *
 ***************************************************************/

/**
 * For a node of type t_cell.
 */
class KSParseNodeExtraPoint : public KSParseNodeExtra
{
public:
  KSParseNodeExtraPoint( const QString& s, KSpreadMap* m, KSpreadSheet* t ) : m_point( s, m, t )
  {
  }

  KSpreadPoint* point() { return &m_point; }

private:
  KSpreadPoint m_point;
};

/**
 * For a node of type t_range.
 */
class KSParseNodeExtraRange : public KSParseNodeExtra
{
public:
  KSParseNodeExtraRange( const QString& s, KSpreadMap* m, KSpreadSheet* t )
    : m_range( s, m, t )
  {
  }

  KSpreadRange* range() { return &m_range; }

private:
  KSpreadRange m_range;
};

/****************************************************
 *
 * Helper functions
 *
 ****************************************************/

static bool kspreadfunc_cell( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "cell", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::ListType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[2], KSValue::StringType, true ) )
      return false;

    const QValueList<KSValue::Ptr>& lines = args[0]->listValue();
    if ( lines.count() < 2 )
      return FALSE;

    QValueList<KSValue::Ptr>::ConstIterator it = lines.begin();
    if ( !KSUtil::checkType( context, (*it), KSValue::ListType, true ) )
      return false;
    const QValueList<KSValue::Ptr>& line = (*it)->listValue();
    QValueList<KSValue::Ptr>::ConstIterator it2 = line.begin();
    int x = 1;
    ++it;
    ++it2;
    for( ; it2 != line.end(); ++it2 )
    {
      if ( !KSUtil::checkType( context, (*it2), KSValue::StringType, true ) )
        return false;
      if ( (*it2)->stringValue() == args[1]->stringValue() )
        break;
      ++x;
    }
    if ( it2 == line.end() )
      return FALSE;

    kdDebug(36001) <<"x= "<<x<<endl;
    for( ; it != lines.end(); ++it )
    {
      const QValueList<KSValue::Ptr>& l = (*it)->listValue();
      if ( x >= (int)l.count() )
        return FALSE;
      if ( l[0]->stringValue() == args[2]->stringValue() )
      {
        context.setValue( new KSValue( *(l[x]) ) );
        return TRUE;
      }
    }

    context.setValue( new KSValue( 0.0 ) );
    return true;
}

static bool kspreadfunc_select_helper( KSContext& context, QValueList<KSValue::Ptr>& args, QString& result )
{
    QValueList<KSValue::Ptr>::Iterator it = args.begin();
    QValueList<KSValue::Ptr>::Iterator end = args.end();

    for( ; it != end; ++it )
    {
      if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
      {
        if ( !kspreadfunc_select_helper( context, (*it)->listValue(), result ) )
          return false;
      }
      else if ( !(*it)->toString( context ).isEmpty() )
      {
        if ( !result.isEmpty() )
          result += "\\";
        result += (*it)->toString( context );
      }
    }

    return true;
}

static bool kspreadfunc_select( KSContext& context )
{
  QString result( "" );
  bool b = kspreadfunc_select_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static KSModule::Ptr kspreadCreateModule_KSpread( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "kspread" );

  module->addObject( "CELL", new KSValue( new KSBuiltinFunction( module, "CELL", kspreadfunc_cell ) ) );
  module->addObject( "SELECT", new KSValue( new KSBuiltinFunction( module, "SELECT", kspreadfunc_select ) ) );

  // add all built-in functions
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();
  QStringList functionList = repo->regFunctionNames();
  for( unsigned i = 0; i < functionList.count(); i++ )
  {
    QString name = functionList[i];
    KSpreadFunction* function = repo->function( name );
    if ( function && function->functionPtr )
      module->addObject( name, new KSValue( new KSBuiltinFunction( module, name, function->functionPtr ) ) );
  }

  return module;
}

/*********************************************************************
 *
 * KSpreadInterpreter
 *
 *********************************************************************/

KSpreadInterpreter::KSpreadInterpreter( KSpreadDoc * doc )
  : KSInterpreter(),
    m_cell( 0 ),
    m_doc( doc ),
    m_table( 0 )
{
  KSModule::Ptr m = kspreadCreateModule_KSpread( this );
  m_modules.insert( m->name(), m );

  // Integrate the KSpread module in the global namespace for convenience
  KSNamespace::Iterator it = m->nameSpace()->begin();
  KSNamespace::Iterator end = m->nameSpace()->end();
  for(; it != end; ++it )
    m_global->insert( it.key(), it.data() );
}

bool KSpreadInterpreter::processExtension( KSContext& context, KSParseNode* node )
{
  KSParseNodeExtra* extra = node->extra();
  if ( !extra )
  {
    if ( node->getType() == t_cell )
      extra = new KSParseNodeExtraPoint( node->getStringLiteral(), m_doc->map(), m_table );
    else if ( node->getType() == t_range )
      extra = new KSParseNodeExtraRange( node->getStringLiteral(), m_doc->map(), m_table );
    else
      return KSInterpreter::processExtension( context, node );
    node->setExtra( extra );
  }

  if ( node->getType() == t_cell )
  {
    KSParseNodeExtraPoint* p = (KSParseNodeExtraPoint*)extra;
    KSpreadPoint* point = p->point();

    if ( !point->isValid() )
    {
      QString tmp( i18n("The expression %1 is not valid") );
      tmp = tmp.arg( node->getStringLiteral() );
      context.setException( new KSException( "InvalidCellExpression", tmp ) );
      return false;
    }

    KSpreadCell* cell = point->cell();

    if ( cell->hasError() )
    {
      QString tmp( i18n("The cell %1 has an error:\n\n%2") );
      tmp = tmp.arg( cell->fullName() );
      tmp = tmp.arg( node->getStringLiteral() );
      context.setException( new KSException( "ErrorInCell", tmp ) );
      return false;
    }

    if ( cell->isDefault())
      context.setValue( new KSValue(  /*KSValue::Empty*/ 0.0 ) );
    else if(cell->isObscured() && cell->isObscuringForced())
      context.setValue( new KSValue( 0.0 ) );
    else if ( cell->value().isNumber() )
      context.setValue( new KSValue( cell->value().asFloat() ) );
    else if ( cell->value().isBoolean() )
      context.setValue( new KSValue( cell->value().asBoolean() ) );
    else if ( cell->isTime() )
      context.setValue( new KSValue( cell->value().asTime() ) );
    else if ( cell->isDate() )
      context.setValue( new KSValue( cell->value().asDate() ) );
    else if ( cell->value().asString().isEmpty() )
      context.setValue( new KSValue( 0.0  /*KSValue::Empty*/ ) );
    else
      context.setValue( new KSValue( cell->value().asString() ) );
    return true;
  }
  // Parse a range like "A1:B3"
  else if ( node->getType() == t_range )
  {
    KSParseNodeExtraRange* p = (KSParseNodeExtraRange*)extra;
    KSpreadRange* r = p->range();

    // Is it a valid range ?
    if ( !r->isValid() )
    {
      QString tmp( i18n("The expression %1 is not valid") );
      tmp = tmp.arg( node->getStringLiteral() );
      context.setException( new KSException( "InvalidRangeExpression", tmp ) );
      return false;
    }

    if ( r->range.left() == r->range.right()
         && r->range.top() == r->range.bottom() )
    {
      KSpreadCell * cell = r->table->cellAt( r->range.x(), r->range.y() );

      if ( cell->hasError() )
      {
        QString tmp( i18n("The cell %1 has an error:\n\n%2") );
        tmp = tmp.arg( cell->fullName() );
        tmp = tmp.arg( node->getStringLiteral() );
        context.setException( new KSException( "ErrorInCell", tmp ) );
        return false;
      }

      if ( cell->isDefault())
        context.setValue( new KSValue(  /*KSValue::Empty*/ 0.0 ) );
      else if(cell->isObscured() && cell->isObscuringForced())
        context.setValue( new KSValue( 0.0 ) );
      else if ( cell->value().isNumber() )
        context.setValue( new KSValue( cell->value().asFloat() ) );
      else if ( cell->value().isBoolean() )
      context.setValue( new KSValue( cell->value().asBoolean() ) );
      else if ( cell->isTime() )
        context.setValue( new KSValue( cell->value().asTime() ) );
      else if ( cell->isDate() )
        context.setValue( new KSValue( cell->value().asDate() ) );
      else if ( cell->value().asString().isEmpty() )
        context.setValue( new KSValue( 0.0  /*KSValue::Empty*/ ) );
      else
        context.setValue( new KSValue( cell->value().asString() ) );

      return true;
    }

    // The range is translated in a list or lists of integers
    KSValue* v = new KSValue( KSValue::ListType );
    for( int y = 0; y < r->range.height(); ++y )
    {
      KSValue* l = new KSValue( KSValue::ListType );

      for( int x = 0; x < r->range.width(); ++x )
      {
        KSValue* c;
        KSpreadCell* cell = r->table->cellAt( r->range.x() + x, r->range.y() + y );

        if ( cell->hasError() )
        {
          QString tmp( i18n("The cell %1 has an error:\n\n%2") );
          tmp = tmp.arg( cell->fullName() );
          tmp = tmp.arg( node->getStringLiteral() );
          context.setException( new KSException( "ErrorInCell", tmp ) );
          return false;
        }

        if ( cell->isDefault() )
          c = new KSValue( 0.0 /*KSValue::Empty*/);
        else if ( cell->value().isNumber() )
          c = new KSValue( cell->value().asFloat() );
        else if ( cell->value().isBoolean() )
          c = new KSValue( cell->value().asBoolean() );
        else if ( cell->isDate() )
          c = new KSValue( cell->value().asDate() );
        else if ( cell->isTime() )
          c = new KSValue( cell->value().asTime() );
        else if ( cell->value().asString().isEmpty() )
          c = new KSValue( 0.0 /*KSValue::Empty*/ );
        else
          c = new KSValue( cell->value().asString() );
        if ( !(cell->isObscured() && cell->isObscuringForced()) )
                l->listValue().append( c );
      }
      v->listValue().append( l );
    }
    context.setValue( v );

    return true;
  }
  else
    Q_ASSERT( 0 );

  // Never reached
  return false;
}

KSParseNode* KSpreadInterpreter::parse( KSContext& context, KSpreadSheet* table, const QString& formula )
{
    // Create the parse tree.
    KSParser parser;
    // Tell the parser the locale so that it can parse localized numbers.
    if ( !parser.parse( formula.utf8(), KSCRIPT_EXTENSION_KSPREAD, table->doc()->locale() ) )
    {
	context.setException( new KSException( "SyntaxError", parser.errorMessage() ) );
	return 0;
    }

    KSParseNode* n = parser.donateParseTree();

    return n;
}

bool KSpreadInterpreter::evaluate( KSContext& context, KSParseNode* node, KSpreadSheet* table, KSpreadCell* cell )
{
    // Save the current table to make this function reentrant.
    KSpreadSheet * t = m_table;
    KSpreadCell * c  = m_cell;
    m_table = table;
    m_cell  = cell;

    bool b = node->eval( context );

    m_table = t;
    m_cell  = c;

    return b;
}
