/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "koscript_eval.h"
#include "koscript_value.h"
#include "koscript_context.h"
#include "koscript_func.h"
#include "koscript_struct.h"
#include "koscript.h"
#include "koscript_parsenode.h"
#include "koscript_util.h"
#include "koscript_property.h"
#include "koscript_method.h"
#include <kdebug.h>
#include <stdio.h>
#include <math.h>

#include <kregexp.h>

#include <qfileinfo.h>
#include <klocale.h>

// Get a left and right operand for arithmetic
// operations like add, mul, div etc. If leftexpr
// is true, then the left value must be assignable.
#define EVAL_OPS( ctx, l, r, leftexpr ) \
  KSParseNode *left = node->branch1();  \
  KSParseNode *right = node->branch2(); \
  if ( !left || !right )                \
    return false;                       \
                                        \
  KSContext l( ctx, leftexpr );         \
  KSContext r( ctx );                   \
  if ( !left->eval( l ) )               \
  {                                     \
    ctx.setException( l );              \
    return false;                       \
  }                                     \
  if ( !right->eval( r ) )              \
  {                                     \
    ctx.setException( r );              \
    return false;                       \
  }

#define EVAL_LEFT_OP( ctx, l )          \
  KSParseNode *left = node->branch1();  \
  if ( !left )                          \
    return false;                       \
                                        \
  KSContext l( ctx );                   \
  if ( !left->eval( l ) )               \
  {                                     \
    ctx.setException( l );              \
    return false;                       \
  }                                     \

#define EVAL_RIGHT_OP( ctx, r )         \
  KSParseNode *right = node->branch2(); \
  if ( !right )                         \
    return false;                       \
                                        \
  KSContext r( ctx );                   \
  if ( !right->eval( r ) )              \
  {                                     \
    ctx.setException( r );              \
    return false;                       \
  }                                     \

// Try to reuse one of the KSValue objects l or r
// and assign it to ctx. This is faster than the default
// behaviour of creating a new KSValue object all the time.
#define FILL_VALUE( ctx, l, r )                  \
  if ( l.value()->mode() == KSValue::Temp )      \
    ctx.setValue( l.shareValue() );              \
  else if ( r.value()->mode() == KSValue::Temp ) \
    ctx.setValue( r.shareValue() );              \
  else                                           \
    ctx.setValue( new KSValue );

bool KSEval_definitions( KSParseNode* node, KSContext& context )
{
  if ( node->branch1() )
  {
    if ( node->branch1()->getType() == func_dcl )
    {
      Q_ASSERT( context.scope() );
      context.scope()->addObject( node->branch1()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch1() ) ) );
    }
    else if ( !node->branch1()->eval( context ) )
      return false;
  }
  if ( node->branch2() )
  {
    if ( node->branch2()->getType() == func_dcl )
    {
      Q_ASSERT( context.scope() );
      context.scope()->addObject( node->branch2()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch2() ) ) );
    }
    else if ( !node->branch2()->eval( context ) )
      return false;
  }

  return true;
}

bool KSEval_exports( KSParseNode* node, KSContext& context )
{
  Q_ASSERT( context.value() );

  if ( context.value()->type() == KSValue::StructClassType )
  {
    if ( node->branch1() )
    {
      if ( node->branch1()->getType() == func_dcl )
        context.value()->structClassValue()->nameSpace()->insert( node->branch1()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch1() ) ) );
      else if ( !node->branch1()->eval( context ) )
        return false;
    }
    if ( node->branch2() )
    {
      if ( node->branch2()->getType() == func_dcl )
        context.value()->structClassValue()->nameSpace()->insert( node->branch2()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch2() ) ) );
      else if ( !node->branch2()->eval( context ) )
        return false;
    }
  }
  else
    Q_ASSERT( 0 );

  return true;
}

bool KSEval_t_vertical_line( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, false );

    if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) ||
         !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
    {
        context.exception()->addLine( node->getLineNo() );
        return false;
    }

    context.setValue( new KSValue( (KScript::Boolean)( l.value()->intValue() | r.value()->intValue() ) ) );

    return true;
}

bool KSEval_t_circumflex( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, false );

    if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) ||
         !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
    {
        context.exception()->addLine( node->getLineNo() );
        return false;
    }

    context.setValue( new KSValue( (KScript::Boolean)( l.value()->intValue() ^ r.value()->intValue() ) ) );

    return true;
}

bool KSEval_t_ampersand( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, false );

    if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) ||
         !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
    {
        context.exception()->addLine( node->getLineNo() );
        return false;
    }

    context.setValue( new KSValue( (KScript::Boolean)( l.value()->intValue() & r.value()->intValue() ) ) );

    return true;
}

bool KSEval_t_shiftright( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, false );

    if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) ||
         !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
    {
        context.exception()->addLine( node->getLineNo() );
        return false;
    }

    context.setValue( new KSValue( (KScript::Long)( l.value()->intValue() >> r.value()->intValue() ) ) );

    return true;
}

bool KSEval_t_shiftleft( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, false );

    if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) ||
         !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
    {
        context.exception()->addLine( node->getLineNo() );
        return false;
    }

    context.setValue( new KSValue( (KScript::Long)( l.value()->intValue() << r.value()->intValue() ) ) );

    return true;
}

bool KSEval_t_plus_sign( KSParseNode* node, KSContext& context )
{
  // Unary ?
  if ( node->branch1() && !node->branch2() )
  {
    if ( !node->branch1()->eval( context ) )
      return false;

    /* this operator is valid on numeric types so if it casts to a double it's ok.
       I'm not sure if there is possible data loss converting from int to double
       so avoid casting to double in that case just to be safe.
    */
    if ( context.value()->type() == KSValue::IntType ||
         context.value()->cast( KSValue::DoubleType) )
    {
      return true;
    }

    QString tmp( i18n("Unary Operator + not defined for type %1") );
    context.setException( new KSException( "UnknownOperation", tmp.arg( context.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  // Binary operator
  EVAL_OPS( context, l, r, false );


  //allows 5+date or 5+time
  //before you can just make time+5 and date +5
 if ( r.value()->type() == KSValue::TimeType )
  {
      if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) )
          return false;
      QTime t = r.value()->timeValue();
      t = t.addSecs( l.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( t );
      return TRUE;
  }
  else if ( r.value()->type() == KSValue::DateType )
  {
      if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) )
          return false;
      QDate d = r.value()->dateValue();
      d = d.addDays( l.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( d );
      return TRUE;
  }

  // allows addition of boolean value, such as True+False
  if ( r.value()->type() == KSValue::BoolType )
  {
      r.value()->cast( KSValue::IntType );
  }

  if ( l.value()->type() == KSValue::TimeType )
  {
      if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
          return false;
      QTime t = l.value()->timeValue();
      t = t.addSecs( r.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( t );
      return TRUE;
  }
  else if ( l.value()->type() == KSValue::DateType )
  {
      if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
          return false;
      QDate d = l.value()->dateValue();
      d = d.addDays( r.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( d );
      return TRUE;
  }

  // allows addition of boolean value, such as True+False
  if ( l.value()->type() == KSValue::BoolType )
  {
      l.value()->cast( KSValue::IntType );
  }

  // If we have double and int, then always convert to double
  else if ( l.value()->type() == KSValue::DoubleType )
  {
      if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
          return false;
  }
  else
  {
      if ( !KSUtil::checkType( context, l.value(), r.value()->type(), true ) )
          return false;
      l.value()->cast( r.value()->type());
  }

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        KScript::Long result = r.value()->intValue() + l.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
      break;
    case KSValue::DoubleType:
      {
        KScript::Double result = l.value()->doubleValue() + r.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::StringType:
      {
        QString result = l.value()->stringValue() + r.value()->stringValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
      break;
    case KSValue::ListType:
      {
        QValueList<KSValue::Ptr> result = l.value()->listValue() + r.value()->listValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
      break;
    case KSValue::MapType:
      {
          QMap<QString,KSValue::Ptr> result = l.value()->mapValue();
          QMap<QString,KSValue::Ptr>::ConstIterator it = r.value()->mapValue().begin();
          QMap<QString,KSValue::Ptr>::ConstIterator end = r.value()->mapValue().end();
          for( ; it != end; ++it )
              result.insert( it.key(), it.data() );
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
      break;
    case KSValue::DateType:
    case KSValue::TimeType:
        // Handled above
        return true;
    default:
      QString tmp( i18n("Operator + not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_minus_sign( KSParseNode* node, KSContext& context )
{
  // Unary ?
  if ( node->branch1() && !node->branch2() )
  {
    if ( !node->branch1()->eval( context ) )
      return false;
    if ( context.value()->type() == KSValue::IntType )
    {
      context.setValue( new KSValue( -( context.value()->intValue() ) ) );
      return true;
    }
    if ( context.value()->type() == KSValue::DoubleType )
    {
      context.setValue( new KSValue( -( context.value()->doubleValue() ) ) );
      return true;
    }

    QString tmp( i18n("Unary Operator - not defined for type %1") );
    context.setException( new KSException( "UnknownOperation", tmp.arg( context.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  EVAL_OPS( context, l, r, false );

  //allows 5-date and 5-time
  if ( r.value()->type() == KSValue::TimeType )
  {
      if ( KSUtil::checkType( context, l.value(), KSValue::TimeType, false ) )
      {
          QTime d = r.value()->timeValue();
          int diff = d.secsTo( l.value()->timeValue() );
          FILL_VALUE( context, l, r );
          QTime _time(0,0,0);
          _time=_time.addSecs(diff);
          context.value()->setValue( /*(KScript::Long) diff*/_time );
          return TRUE;
      }

      if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) )
          return false;
      QTime t = r.value()->timeValue();
      t = t.addSecs( -l.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( t );
      return TRUE;
  }
  else if ( r.value()->type() == KSValue::DateType )
  {
      if ( KSUtil::checkType( context, l.value(), KSValue::DateType, false ) )
      {
          QDate d = r.value()->dateValue();
          int diff = d.daysTo( l.value()->dateValue() );
          FILL_VALUE( context, l, r );
          context.value()->setValue( (KScript::Long)diff );
          return TRUE;
      }

      if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) )
          return false;
      QDate d = r.value()->dateValue();
      d = d.addDays( -l.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( d );
      return TRUE;
  }



  if ( l.value()->type() == KSValue::TimeType )
  {
      if ( KSUtil::checkType( context, r.value(), KSValue::TimeType, false ) )
      {
          QTime d = r.value()->timeValue();
          int diff = d.secsTo( l.value()->timeValue() );
          FILL_VALUE( context, l, r );
          context.value()->setValue( (KScript::Long) diff );
          return TRUE;
      }

      if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
          return false;
      QTime t = l.value()->timeValue();
      t = t.addSecs( -r.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( t );
      return TRUE;
  }
  else if ( l.value()->type() == KSValue::DateType )
  {
      if ( KSUtil::checkType( context, r.value(), KSValue::DateType, false ) )
      {
          QDate d = r.value()->dateValue();
          int diff = d.daysTo( l.value()->dateValue() );
          FILL_VALUE( context, l, r );
          context.value()->setValue( (KScript::Long)diff );
          return TRUE;
      }

      if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
          return false;
      QDate d = l.value()->dateValue();
   d = d.addDays( -r.value()->intValue() );
      FILL_VALUE( context, l, r );
      context.value()->setValue( d );
      return TRUE;
  }
  // If we have double and int, then always convert to double
  else if ( l.value()->type() == KSValue::DoubleType )
  {
    if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, l.value(), r.value()->type(), true ) )
      return false;
    l.value()->cast( r.value()->type());
  }

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        KScript::Long result = l.value()->intValue() - r.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DoubleType:
      {
        KScript::Double result = l.value()->doubleValue() - r.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    default:
      QString tmp( i18n("Operator - not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_asterik( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  // If we have double and int, then always convert to double
  if ( l.value()->type() == KSValue::DoubleType )
  {
    if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, l.value(), r.value()->type(), true ) )
      return false;
    l.value()->cast( r.value()->type());
  }

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        // promote to double multiplication( fix bug #42499 )
        // hold it in long integer only if it is small enough
        KScript::Double v = r.value()->doubleValue() * l.value()->doubleValue();
        FILL_VALUE( context, l, r );
        if( fabs( v ) < 1e9 )
          context.value()->setValue( (long) v );
        else
          context.value()->setValue( v );
        return true;
      }
    case KSValue::DoubleType:
      {
        KScript::Double result = r.value()->doubleValue() * l.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    default:
      QString tmp( i18n("Operator * not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_solidus( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  // If we have double and int, then always convert to double
  if ( l.value()->type() == KSValue::DoubleType )
  {
    if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, l.value(), r.value()->type(), true ) )
      return false;
    l.value()->cast( r.value()->type());
  }

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        // If the devision has a "rest" then we have to convert to doubles
        if ( r.value()->intValue()!=0 && ( l.value()->intValue() % r.value()->intValue() ) == 0 )
        {
          KScript::Long result = l.value()->intValue() / r.value()->intValue();
          FILL_VALUE( context, l, r );
          context.value()->setValue( result );
        }
        else
        {
          KScript::Double result = (double)l.value()->intValue() / (double)r.value()->intValue();
          FILL_VALUE( context, l, r );
          context.value()->setValue( result );
        }
        return true;
      }
    case KSValue::DoubleType:
      {
        KScript::Double result = l.value()->doubleValue() / r.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    default:
      QString tmp( i18n("Operator / not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_percent_sign( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !KSUtil::checkType( context, l.value(), KSValue::IntType, true ) )
    return false;
  if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
    return false;

  if(r.value()->intValue()!=0)
        {
        KScript::Long result = l.value()->intValue() % r.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        }
  else
        {
        KScript::Double result = (double)l.value()->intValue() / (double)r.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        }
  return true;
}

bool KSEval_t_tilde( KSParseNode* , KSContext& ) { return false; }

bool KSEval_t_integer_literal( KSParseNode* node, KSContext& context )
{
  context.setValue( new KSValue( node->getIntegerLiteral() ) );
  return true;
}

bool KSEval_t_string_literal( KSParseNode* node, KSContext& context )
{
  context.setValue( new KSValue( node->getStringLiteral() ) );
  return true;
}

bool KSEval_t_character_literal( KSParseNode* node, KSContext& context )
{
  context.setValue( new KSValue( node->getCharacterLiteral() ) );
  return true;
}

bool KSEval_t_floating_pt_literal( KSParseNode* node, KSContext& context )
{
  context.setValue( new KSValue( node->getFloatingPtLiteral() ) );
  return true;
}

bool KSEval_t_boolean_literal( KSParseNode* node, KSContext& context )
{
  context.setValue( new KSValue( node->getBooleanLiteral() ) );
  return true;
}

bool KSEval_scoped_name( KSParseNode* node, KSContext& context )
{
  KSValue* v = context.object( node->getIdent() );
  if ( !v )
  {
    context.setException( new KSException( "UnknownName", node->getIdent(), node->getLineNo() ) );
    return false;
  }

  v->ref();
  context.setValue( v );

  return true;
}


bool KSEval_const_dcl( KSParseNode* node, KSContext& context )
{
  Q_ASSERT( node->branch1() );

  KSContext( l );
  if ( !node->branch1()->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  if ( !context.value() )
    context.scope()->addObject( node->getIdent(), l.shareValue() );
  else if ( context.value()->type() == KSValue::StructClassType )
    context.value()->structClassValue()->nameSpace()->insert( node->getIdent(), l.shareValue() );
  else
    Q_ASSERT( 0 );

  return true;
}

bool KSEval_func_dcl( KSParseNode* node, KSContext& context )
{
  // We want an additional namespace in the scope
  KSNamespace nspace;
  KSSubScope scope( &nspace );
  context.scope()->pushLocalScope( &scope );

  // Fill parameters in our namespace
  if ( node->branch1() )
    if ( !node->branch1()->eval( context ) )
    {
      context.scope()->popLocalScope();
      return false;
    }

  // Are parameters left ?
  if ( !context.value()->listValue().isEmpty() )
  {
    const QString tmp( i18n("1 argument is not needed", "%n arguments are not needed", context.value()->listValue().count() ) );
    context.setException( new KSException( "TooManyArguments", tmp, node->getLineNo() ) );
    context.scope()->popLocalScope();
    return false;
  }

  bool res = true;
  // Call the function
  if ( node->branch2() )
    res = node->branch2()->eval( context );

  // Finish stack unwinding
  context.clearReturnFlag();

  // Remove the local scope
  context.scope()->popLocalScope();

  return res;
}

bool KSEval_func_lines( KSParseNode* node, KSContext& context )
{
  if ( node->branch1() )
  {
    context.interpreter()->context().setException( 0 ); // ### reset -- HACK (Simon)
    if ( !node->branch1()->eval( context ) )
      return false;
    if ( context.returnFlag() )
      return true;
    // if ( node->branch1()->getType() == t_return )
    // return true;
  }

  // We are not interested in the value of the evaluation
  // since it is not a return value.
  context.setValue( 0 );

  // Did some destructor cause a exception ?
  if ( context.interpreter()->context().exception() )
  {
    context.setException( context.interpreter()->context().exception() );
    return false;
  }

  // The second branch can only hold a "func_lines" parsenode.
  if ( node->branch2() )
    if ( !node->branch2()->eval( context ) )
      return false;

  return true;
}

bool KSEval_assign_expr( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, true );

  if ( l.value()->mode() != KSValue::LeftExpr )
  {
    context.setException( new KSException( "NoLeftExpr", i18n("Expected a left expression in assignment"), node->getLineNo() ) );
    return false;
  }

  // Special handling for strings
  if ( l.value()->type() == KSValue::CharRefType )
  {
    if ( !r.value()->cast( KSValue::CharType ) )
    {
      QString tmp( i18n("From %1 to Char") );
      context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ), node->getLineNo() ) );
      return false;
    }
    l.value()->charRefValue() = r.value()->charValue();

    // Dont return the CharRef, so create a new value
    context.setValue( new KSValue( r.value()->charValue() ) );
    return true;
  }

  // Special handling for properties
  if ( l.value()->type() == KSValue::PropertyType )
  {
    if ( ! l.value()->propertyValue()->set( context, r.shareValue() ) )
      return false;
    // Return the value we just assigned
    context.setValue( r.shareValue() );
  }
  else
  {
    l.value()->suck( r.value() );
    // Return the value we just assigned
    context.setValue( l.shareValue() );
  }
  // Now it is a rightexpr. Dont allow to change it -> constant
  // context.value()->setMode( KSValue::Constant );

  return true;
}

bool KSEval_t_equal( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  KScript::Boolean result;
  if ( !r.value()->cast( l.value()->type() ) )
  {
    /* If we can't cast the values to match, then they definitely aren't
       equal.  Don't return a parse error here -- our users aren't expected
       to have detailed understanding of the syntax.
    */
    result = false;
  }
  else
  {

    result = ( r.value()->cmp( *l.value() ) );
  }

  FILL_VALUE( context, l, r );
  context.value()->setValue( result );
  return true;
}

bool KSEval_t_notequal( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !r.value()->cast( l.value()->type() ) )
  {
    QString tmp( i18n("From %1 to %2") );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ).arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  KScript::Boolean result = !( r.value()->cmp( *l.value() ) );
  FILL_VALUE( context, l, r );
  context.value()->setValue( result );
  return true;
}

bool KSEval_t_less_or_equal( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
    return false;

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        KScript::Boolean result = l.value()->intValue() <= r.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DoubleType:
      {
        KScript::Boolean result = l.value()->doubleValue() <= r.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::CharType:
      {
        KScript::Boolean result = l.value()->charValue() <= r.value()->charValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::StringType:
      {
        KScript::Boolean result = l.value()->stringValue() <= r.value()->stringValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DateType:
      {
	KScript::Boolean result = l.value()->dateValue() <= r.value()->dateValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::TimeType:
      {
	KScript::Boolean result = l.value()->timeValue() <= r.value()->timeValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    default:
      QString tmp( i18n("Operator <= not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_greater_or_equal( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
    return false;

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        KScript::Boolean result = l.value()->intValue() >= r.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DoubleType:
      {
        KScript::Boolean result = l.value()->doubleValue() >= r.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::StringType:
      {
        KScript::Boolean result = l.value()->stringValue() >= r.value()->stringValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::CharType:
      {
        KScript::Boolean result = l.value()->charValue() >= r.value()->charValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DateType:
      {
        KScript::Boolean result = l.value()->dateValue() >= r.value()->dateValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::TimeType:
      {
        KScript::Boolean result = l.value()->timeValue() >= r.value()->timeValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    default:
      QString tmp( i18n("Operator >= not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_array( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !r.value()->cast( KSValue::IntType ) )
  {
    QString tmp( i18n("From %1 to Integer in array index") );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  int index = r.value()->intValue();

  if ( index < 0 )
  {
    QString tmp( i18n("Negative array index %1"));
    context.setException( new KSException( "IndexOutOfRange", tmp.arg( index ), node->getLineNo() ) );
    return false;
  }

  // is it a string ? -> special handling
  if ( l.value()->type() == KSValue::StringType )
  {
    int len = l.value()->stringValue().length();

    if ( index >= len && !context.leftExpr() )
    {
      QString tmp( i18n("Too large index %1"));
      context.setException( new KSException( "IndexOutOfRange", tmp.arg( index ), node->getLineNo() ) );
      return false;
    }

    // Get a QChar
    if ( !context.leftExpr() )
    {
      const QString& str = l.value()->stringValue();
      context.setValue( new KSValue( str[ index ] ) );
      return true;
    }

    // Get a CharRef since leftexpr is needed
    context.setValue( new KSValue( KScript::CharRef( &(l.value()->stringValue()), index ) ) );
    context.value()->setMode( KSValue::LeftExpr );
    return true;
  }

  if ( !l.value()->cast( KSValue::ListType ) )
  {
    QString tmp( i18n("From %1 to List") );
    context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  int len = l.value()->listValue().count();
  if ( index >= len )
  {
    if ( !context.leftExpr() )
    {
      QString tmp( i18n("Too large index %1"));
      context.setException( new KSException( "IndexOutOfRange", tmp.arg( index ), node->getLineNo() ) );
      return false;
    }
    else
    {
      // Fill the list with empty values
      for( int i = 0; i <= index - len; ++i )
        l.value()->listValue().append( new KSValue() );
    }
  }

  context.setValue( l.value()->listValue()[ index ] );
  context.value()->setMode( l.value()->mode() );

  return true;
}

bool KSEval_t_dict( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !r.value()->cast( KSValue::StringType ) )
  {
    QString tmp( i18n("From %1 to String in dict") );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  if ( !l.value()->cast( KSValue::MapType ) )
  {
    QString tmp( i18n("From %1 to Map") );
    context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  QMap<QString,KSValue::Ptr>::Iterator it = l.value()->mapValue().find( r.value()->stringValue() );
  // Unknown element ?
  if ( it == l.value()->mapValue().end() )
  {
    // No left expr needed-> return <none>
    if ( !context.leftExpr() )
    {
      context.setValue( new KSValue() );
      return true;
    }
    // Left expr needed
    //    we got Left expr -> insert empty element
    else if ( l.value()->mode() == KSValue::LeftExpr )
    {
      KSValue::Ptr v( new KSValue() );
      v->setMode( l.value()->mode() );
      l.value()->mapValue().insert( r.value()->stringValue(), v );
      context.setValue( v );
      return true;
    }
    //    we can not provide a left expression
    else
    {
      context.setException( new KSException( "NoLeftExpr", i18n("Expected a left expression."), node->getLineNo() ) );
      return false;
    }
  }

  context.setValue( it.data() );
  context.value()->setMode( l.value()->mode() );

  return true;
}

bool KSEval_func_params( KSParseNode* node, KSContext& context )
{
  // process a parameter
  if ( node->branch1() )
    if ( !node->branch1()->eval( context ) )
      return false;

  // process more parameters
  if ( node->branch2() )
    if ( !node->branch2()->eval( context ) )
      return false;

  return true;
}

bool KSEval_func_param_in( KSParseNode* node, KSContext& context )
{
  KSValue* v = 0;

  // No more arguments ?
  if ( context.value()->listValue().isEmpty() )
  {
    // Do we have a default Argument ?
    if ( node->branch1() )
    {
      KSContext d( context );
      if ( !node->branch1()->eval( d ) )
        return false;
      if ( d.value()->mode() == KSValue::Temp )
        v = d.shareValue();
      else
        v = new KSValue( *d.value() );
    }
    else
    {
      QString tmp( i18n("Argument for parameters %1 missing") );
      context.setException( new KSException( "ToFewArguments", tmp.arg( node->getIdent() ), node->getLineNo() ) );
      return false;
    }
  }
  else
  {
    // Put the arguments as parameter in our namespace
    KSValue* arg = *(context.value()->listValue().begin());
    if ( arg->mode() == KSValue::Temp )
    {
      arg->ref();
      v = arg;
    }
    else
      v = new KSValue( *arg );

    // Remove the argument from the list
    context.value()->listValue().remove( context.value()->listValue().begin() );
  }

  v->setMode( KSValue::LeftExpr );
  context.scope()->addObject( node->getIdent(), v );

  return true;
}

bool KSEval_func_param_out( KSParseNode* node, KSContext& context )
{
  // No more arguments ?
  if ( context.value()->listValue().isEmpty() )
  {
    QString tmp( i18n("Argument for parameters %1 missing") );
    context.setException( new KSException( "ToFewArguments", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  KSValue* arg = *(context.value()->listValue().begin());

  // Is the argument not a leftexpr ?
  if ( arg->mode() != KSValue::LeftExpr )
  {
    QString tmp( i18n("LeftExpr needed for parameter %1") );
    context.setException( new KSException( "NoLeftExpr", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  // The difference between out/inout. We empty the value here to make
  // shure that nobody write "out" where he means "inout".
  context.value()->clear();

  // Put the arguments as parameter in our namespace
  arg->ref();
  context.scope()->addObject( node->getIdent(), arg );

  // Remove the argument from the list
  context.value()->listValue().remove( context.value()->listValue().begin() );

  return true;
}

bool KSEval_func_param_inout( KSParseNode* node, KSContext& context )
{
  // No more arguments ?
  if ( context.value()->listValue().isEmpty() )
  {
    QString tmp( i18n("Argument for parameters %1 missing") );
    context.setException( new KSException( "ToFewArguments", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  KSValue* arg = *(context.value()->listValue().begin());

  // Is the argument not a leftexpr ?
  if ( arg->mode() != KSValue::LeftExpr )
  {
    QString tmp( i18n("LeftExpr needed for parameter %1") );
    context.setException( new KSException( "NoLeftExpr", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  // Put the arguments as parameter in our namespace
  arg->ref();
  context.scope()->addObject( node->getIdent(), arg );

  // Remove the argument from the list
  context.value()->listValue().remove( context.value()->listValue().begin() );

  return true;
}

bool KSEval_t_func_call( KSParseNode* node, KSContext& context )
{
  // Get the function object
  KSParseNode *left = node->branch1();
  if ( !left )
    return true;

  KSContext l( context );
  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  if ( !l.value()->cast( KSValue::FunctionType ) &&
       !l.value()->cast( KSValue::MethodType ) && !l.value()->cast( KSValue::StructClassType ) )
  {
    QString tmp( i18n("From %1 to Function") );
    context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  // Create a list of parameters
  context.setValue( new KSValue( KSValue::ListType ) );
  context.setExtraData(new KSValue(KSValue::ListType));

  KSParseNode *right = node->branch2();
  if ( right )
    if ( !right->eval( context ) )
      return false;

  // Remove our namespaces
  KSSubScope* scope = context.scope()->popLocalScope();
  KSModule* module = context.scope()->popModule();

  bool b = FALSE;
  if ( l.value()->cast( KSValue::FunctionType ) )
  {
    context.scope()->pushModule( l.value()->functionValue()->module() );
    // Call the function
    b = l.value()->functionValue()->call( context );
    context.scope()->popModule();
  }
  else if ( l.value()->cast( KSValue::StructClassType ) )
  {
    context.scope()->pushModule( l.value()->structClassValue()->module() );
    // Call struct constructor
    b = l.value()->structClassValue()->constructor( context );
    context.scope()->popModule();
  }
  else if ( l.value()->cast( KSValue::MethodType ) )
  {
    context.scope()->pushModule( l.value()->methodValue()->module() );
    // Call method
    b = l.value()->methodValue()->call( context );
    context.scope()->popModule();
  }
  else
    Q_ASSERT( 0 );

  // Resume namespaces
  context.scope()->pushLocalScope( scope );
  context.scope()->pushModule( module );

  if ( !b )
    return false;

  // Lets have at least a <none> as return value
  if ( !context.value() )
    context.setValue( KSValue::null() );

  return true;
}

bool KSEval_member_expr( KSParseNode* node, KSContext& context )
{
  KSParseNode *left = node->branch1();
  Q_ASSERT( left );

  // This resets leftExpr to FALSE
  KSContext l( context );
  // Try to find the object
  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  /**
   * This is a syntax shortcut. If a function or method returns a object and takes
   * no arguments, then you can skip the explizit function call.
   * Instead of map().Table1().B3().setText("Hallo") you can write
   * map.Table1.B3.setText("Hallo")
   */
  if ( l.value()->type() == KSValue::FunctionType || l.value()->type() == KSValue::MethodType )
  {
        // Copy l.value to func
        KSContext func( context );
        func.setValue( new KSValue( *l.value() ) );

        // Create a list of parameters
        l.setValue( new KSValue( KSValue::ListType ) );

        // Remove our namespaces
        KSSubScope* scope = l.scope()->popLocalScope();
        KSModule* module = l.scope()->popModule();

        bool b = FALSE;
        if ( func.value()->type() == KSValue::FunctionType )
        {
            l.scope()->pushModule( l.value()->functionValue()->module() );
            // Call the function
            b = func.value()->functionValue()->call( l );
            l.scope()->popModule();
        }
        else if ( func.value()->type() == KSValue::MethodType )
        {
            l.scope()->pushModule( l.value()->methodValue()->module() );
            // Call method
            b = func.value()->methodValue()->call( l );
            l.scope()->popModule();
        }
        else
            Q_ASSERT( 0 );

        // Resume namespaces
        l.scope()->pushLocalScope( scope );
        l.scope()->pushModule( module );

        if ( !b )
        {
            context.setException( l.exception() );
            return false;
        }

        // Lets have at least a <none> as return value
        if ( !l.value() )
            l.setValue( KSValue::null() );
  }
  /** End of Syntax trick ;-) **/

  // Special handling for modules
  if ( l.value()->cast( KSValue::ModuleType ) )
  {
    KSValue::Ptr v = l.value()->moduleValue()->member( context, node->getIdent() );
    if ( !v )
    {
      context.exception()->addLine( node->getLineNo() );
      return false;
    }

    context.setValue( v );

    return true;
  }
  // Special handling for struct classes
  else if ( l.value()->cast( KSValue::StructClassType ) )
  {
    KSValue::Ptr v = l.value()->structClassValue()->member( context, node->getIdent() );
    if ( !v )
    {
      context.exception()->addLine( node->getLineNo() );
      return false;
    }

    context.setValue( v );

    return true;
  }

  KSValue::Ptr v;
  KSModule* module;
  if ( l.value()->cast( KSValue::StructType ) )
  {
    v = l.value()->structValue()->member( context, node->getIdent() );
    module = l.value()->structValue()->module();
  }
  // Special handling for all kind of built in data types
  else
  {
    KSValue* v = context.object( node->getIdent() );
    if ( !v )
    {
      context.setException( new KSException( "UnknownName", node->getIdent(), node->getLineNo() ) );
      return false;
    }
    if ( v->type() != KSValue::FunctionType )
    {
      KSUtil::castingError( context, v, KSValue::FunctionType );
      return false;
    }
    v->ref();
    context.setValue( new KSValue( new KSMethod( context.scope()->module(), l.shareValue(), v ) ) );
    return true;
  }
  /*
  else
  {
    QString tmp( "From %1 to Object" );
    context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
    } */

  if ( !v )
  {
    context.exception()->addLine( node->getLineNo() );
    return false;
  }

  if ( v->type() == KSValue::FunctionType  )
      context.setValue( new KSValue( new KSMethod( module, l.shareValue(), v ) ) );
  else if ( v->type() == KSValue::StructBuiltinMethodType )
    context.setValue( new KSValue( new KSMethod( module, l.shareValue(), v, node->getIdent() ) ) );
  else
      context.setValue( v );

  return true;
}

bool KSEval_t_array_const( KSParseNode* node, KSContext& context )
{
  context.setValue( new KSValue( KSValue::ListType ) );

  KSParseNode *right = node->branch1();
  if ( !right )
    return true;

  if ( !right->eval( context ) )
    return false;

  return true;
}

bool KSEval_t_array_element( KSParseNode* node, KSContext& context )
{
  KSParseNode *left = node->branch1();
  if ( !left )
    return true;

  KSContext l( context );
  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  if ( l.value()->mode() == KSValue::Temp )
  {
    l.value()->ref();
    context.value()->listValue().append( KSValue::Ptr( l.value() ) );
  }
  else
  {
    KSValue::Ptr v( new KSValue );
    v->suck( l.value() );
    context.value()->listValue().append( v );
  }

  KSParseNode *right = node->branch2();
  if ( !right )
    return true;

  if ( !right->eval( context ) )
    return false;

  return true;
}

bool KSEval_t_dict_const( KSParseNode* node, KSContext& context )
{
  context.setValue( new KSValue( KSValue::MapType ) );

  KSParseNode *right = node->branch1();
  if ( !right )
    return true;

  if ( !right->eval( context ) )
    return false;

  return true;
}

bool KSEval_t_dict_element( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !l.value()->cast( KSValue::StringType ) )
  {
    QString tmp( i18n("From %1 to String") );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  if ( r.value()->mode() == KSValue::Temp )
  {
    r.value()->ref();
    context.value()->mapValue().insert( l.value()->stringValue(), KSValue::Ptr( r.value() ) );
  }
  else
  {
    KSValue::Ptr v( new KSValue );
    v->suck( r.value() );
    context.value()->mapValue().insert( l.value()->stringValue(), v );
  }

  KSParseNode *next = node->branch3();
  if ( !next )
    return true;

  if ( !next->eval( context ) )
    return false;

  return true;
}

bool KSEval_t_while( KSParseNode* node, KSContext& context )
{
  do
  {
    EVAL_LEFT_OP( context, l );

    if ( !l.value()->implicitCast( KSValue::BoolType ) )
    {
      QString tmp( i18n("From %1 to Boolean") );
      context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

    // Head of the while loop
    if ( !l.value()->boolValue() )
      return true;

    // Tail of the while loop
    EVAL_RIGHT_OP( context, r );
  } while( 1 );

  // Never reached
  return false;
}

bool KSEval_t_do( KSParseNode* node, KSContext& context )
{
  do
  {
    // Body of the loop
    if ( !node->branch1()->eval( context ) )
      return false;

    // Tail
    if ( !node->branch2()->eval( context ) )
      return false;

    if ( !context.value()->cast( KSValue::BoolType ) )
    {
      KSUtil::castingError( context, context.value(), KSValue::BoolType );
      return false;
    }

    // Head of the while loop
    if ( !context.value()->boolValue() )
      return true;

  } while( 1 );

  // Never reached
  return false;
}

bool KSEval_t_for( KSParseNode* node, KSContext& context )
{
  // Evaluate the start code
  if ( !node->branch1()->eval( context ) )
    return false;

  do
  {
    // Evaluate the condition
    if ( !node->branch2()->eval( context ) )
      return false;

    if ( !context.value()->cast( KSValue::BoolType ) )
    {
      KSUtil::castingError( context, context.value(), KSValue::BoolType );
      return false;
    }

    // Condition failed ?
    if ( !context.value()->boolValue() )
      return true;

    // Evaluate the body
    if ( !node->branch4()->eval( context ) )
      return false;

    // Evaluate the iterator
    if ( !node->branch3()->eval( context ) )
      return false;

  } while(1);

  // Bever reached
  return false;
}

bool KSEval_t_if( KSParseNode* node, KSContext& context )
{
  // Evaluate the condition
  if ( !node->branch1()->eval( context ) )
    return false;

  if ( !context.value()->cast( KSValue::BoolType ) )
  {
    KSUtil::castingError( context, context.value(), KSValue::BoolType );
    return false;
  }

  // Condition failed ?
  if ( !context.value()->boolValue() )
  {
    if ( node->branch3() )
      return node->branch3()->eval( context );
    return true;
  }

  return node->branch2()->eval( context );
}

bool KSEval_t_incr( KSParseNode* node, KSContext& context )
{
  // Evaluate the expression
  if ( !node->branch1()->eval( context ) )
    return false;

  if ( !KSUtil::checkType( context, context.value(), KSValue::IntType, true ) )
    return false;

  if ( context.value()->mode() != KSValue::LeftExpr )
  {
    context.setException( new KSException( "NoLeftExpr", i18n("Expected a left expression in assignment"), node->getLineNo() ) );
    return false;
  }

  // postfix ?
  if ( node->branch2() )
  {
    KSValue::Ptr p = context.shareValue();
    KScript::Long l = p->intValue();
    p->setValue( p->intValue() + 1 );
    context.setValue( new KSValue( l ) );
    context.value()->setMode( KSValue::Temp );
  }
  else
    context.value()->setValue( context.value()->intValue() + 1 );

  return true;
}

bool KSEval_t_decr( KSParseNode* node, KSContext& context )
{
  // Evaluate the expression
  if ( !node->branch1()->eval( context ) )
    return false;

  if ( !KSUtil::checkType( context, context.value(), KSValue::IntType, true ) )
    return false;

  if ( context.value()->mode() != KSValue::LeftExpr )
  {
    context.setException( new KSException( "NoLeftExpr", i18n("Expected a left expression in assignment."), node->getLineNo() ) );
    return false;
  }

  // postfix ?
  if ( node->branch2() )
  {
    KSValue::Ptr p = context.shareValue();
    KScript::Long l = p->intValue();
    p->setValue( p->intValue() - 1 );
    context.setValue( new KSValue( l ) );
    context.value()->setMode( KSValue::Temp );
  }
  else
    context.value()->setValue( context.value()->intValue() - 1 );

  return true;
}

bool KSEval_t_less( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
    return false;

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        KScript::Boolean result = l.value()->intValue() < r.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DoubleType:
      {
        KScript::Boolean result = l.value()->doubleValue() < r.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::StringType:
      {
        KScript::Boolean result = l.value()->stringValue() < r.value()->stringValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DateType:
      {
        KScript::Boolean result = l.value()->dateValue() < r.value()->dateValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::TimeType:
      {
        KScript::Boolean result = l.value()->timeValue() < r.value()->timeValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::CharType:
      {
        KScript::Boolean result = l.value()->charValue() < r.value()->charValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    default:
      QString tmp( i18n("Operator < not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_greater( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !KSUtil::checkType( context, r.value(), l.value()->type(), true ) )
    return false;

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
        KScript::Boolean result = l.value()->intValue() > r.value()->intValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::DoubleType:
      {
        KScript::Boolean result = l.value()->doubleValue() > r.value()->doubleValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::StringType:
      {
        KScript::Boolean result = l.value()->stringValue() > r.value()->stringValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::CharType:
      {
        KScript::Boolean result = l.value()->charValue() > r.value()->charValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
   case KSValue::DateType:
      {
        KScript::Boolean result = l.value()->dateValue() > r.value()->dateValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    case KSValue::TimeType:
      {
        KScript::Boolean result = l.value()->timeValue() > r.value()->timeValue();
        FILL_VALUE( context, l, r );
        context.value()->setValue( result );
        return true;
      }
    default:
      QString tmp( i18n("Operator > not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}


bool KSEval_t_foreach( KSParseNode* node, KSContext& context )
{
  // Evaluate the list/map
  if ( !node->branch1()->eval( context ) )
    return false;

  // Is the array a LeftExpr, Temp or Constant
  KSValue::Mode mode = context.value()->mode();

  // Little hack to test wether we are in list or map mode
  if ( node->branch3() )
  {
    if ( !context.value()->cast( KSValue::MapType ) )
    {
      KSUtil::castingError( context, context.value(), KSValue::MapType );
      return false;
    }

    KSNamespace nspace;
    context.scope()->localScope()->pushNamespace( &nspace );

    QMap<QString,KSValue::Ptr>::Iterator it = context.value()->mapValue().begin();
    QMap<QString,KSValue::Ptr>::Iterator end = context.value()->mapValue().end();
    for( ; it != end; ++it )
    {
      // Get the element of the map in the local scope
      it.data()->ref();
      KSValue* v = it.data();
      // Same mode as the array
      v->setMode( mode );
      context.scope()->addObject( node->getStringLiteral(), v );

      // Get the key of the map in the local scope
      v = new KSValue( it.key() );
      v->setMode( KSValue::Constant );
      context.scope()->addObject( node->getIdent(), v );

      // Evaluate the body
      KSContext ctx( context );
      if ( !node->branch2()->eval( ctx ) )
      {
        context.setException( ctx );
        context.scope()->localScope()->popNamespace();
        return false;
      }
    }

    context.scope()->localScope()->popNamespace();
  }
  else
  {
    if ( !context.value()->cast( KSValue::ListType ) )
    {
      KSUtil::castingError( context, context.value(), KSValue::ListType );
      return false;
    }

    KSNamespace nspace;
    context.scope()->localScope()->pushNamespace( &nspace );

    QValueList<KSValue::Ptr>::Iterator it = context.value()->listValue().begin();
    QValueList<KSValue::Ptr>::Iterator end = context.value()->listValue().end();
    for( ; it != end; ++it )
    {
      // Get the element of the array in our local variable
      (*it)->ref();
      KSValue* v = (*it);
      // Same mode as the array
      v->setMode( mode );
      context.scope()->addObject( node->getIdent(), v );

      // Evaluate the body
      KSContext ctx( context );
      if ( !node->branch2()->eval( ctx ) )
      {
        context.setException( ctx );
        context.scope()->localScope()->popNamespace();
        return false;
      }
    }

    context.scope()->localScope()->popNamespace();
  }

  return true;
}

bool KSEval_t_match( KSParseNode* node , KSContext& context )
{
    if ( !node->branch1()->eval( context ) )
        return false;

    if ( !KSUtil::checkType( context, context.value(), KSValue::StringType, TRUE ) )
        return FALSE;

    KRegExp* exp = context.interpreter()->regexp();
    exp->compile( node->getIdent().latin1() ); // identifiers are a-zA-Z etc, so latin1 is ok. (David)

    kdDebug() << "Matching " << context.value()->stringValue() << " against " << node->getIdent() << endl;

    context.setValue( new KSValue( exp->match( context.value()->stringValue().latin1() ) ) );

    return TRUE;
}

bool KSEval_t_subst( KSParseNode* node, KSContext& context )
{
    KSContext l( context, TRUE );
    if ( !node->branch1()->eval( l ) )
        return false;

    if ( l.value()->mode() != KSValue::LeftExpr )
    {
        context.setException( new KSException( "NoLeftExpr", i18n("Expected a left expression in substitute."), node->getLineNo() ) );
        return false;
    }

    if ( !KSUtil::checkType( l, l.value(), KSValue::StringType, TRUE ) )
        return FALSE;

    int pos = node->getIdent().find( '/' );
    Q_ASSERT( pos != -1 );
    QString match = node->getIdent().left( pos );
    QString subst = node->getIdent().mid( pos + 1 );
    KRegExp* exp = context.interpreter()->regexp();
    exp->compile( match.latin1() ); // identifiers are a-zA-Z etc, so latin1 is ok. (David)

    kdDebug() << "Matching " << l.value()->stringValue() << " against " << node->getIdent() << endl;

    if ( !exp->match( l.value()->stringValue().latin1() ) )
    {
        context.setValue( new KSValue( FALSE ) );
        return TRUE;
    }
    else
    {
        int len = subst.length();
        int i = 0;
        while( i < len )
        {
            if ( subst[i] == '\\' && i + 1 < len && subst[i+1].isDigit() )
            {
                const char* grp = exp->group( subst[i+1].latin1() - '0' );
                QString repl;
                if ( grp )
                    repl = grp;
                else
                    repl = "";
                subst.replace( i, 2, repl );
                len += repl.length() + 1;
                i += repl.length();
            }
            else
                ++i;
        }
        QString& str = l.value()->stringValue();
        str.replace( exp->groupStart( 0 ), exp->groupEnd( 0 ) - exp->groupStart( 0 ), subst );
    }

    context.setValue( new KSValue( TRUE ) );
    return TRUE;
}

bool KSEval_t_not( KSParseNode* node, KSContext& context )
{
  if ( !node->branch1()->eval( context ) )
    return false;

  if ( !context.value()->cast( KSValue::BoolType ) )
  {
    QString tmp( i18n("Unary Operator ! not defined for type %1") );
    context.setException( new KSException( "UnknownOperation", tmp.arg( context.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  context.setValue( new KSValue( !( context.value()->boolValue() ) ) );
  return true;
}

bool KSEval_func_call_params( KSParseNode* node, KSContext& context )
{
  // Get parameter
  KSParseNode *left = node->branch1();
  if ( !left )
    return true;

  KSContext l( context );
  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  context.value()->listValue().append( l.shareValue() );

  if (left->getType() == t_cell || left->getType() == t_range)
  {
    context.extraData()->listValue().append(new KSValue(left->getStringLiteral()));
  }
  else
  {
    context.extraData()->listValue().append(new KSValue());
  }

  // More parameters ?
  KSParseNode *right = node->branch2();
  if ( right )
    if ( !right->eval( context ) )
      return false;

  return true;
}

bool KSEval_t_return( KSParseNode* node, KSContext& context )
{
  // Get return value if available
  KSParseNode *left = node->branch1();
  if ( left )
  {
    if ( !left->eval( context ) )
    {
      context.setException( context );
      return false;
    }

    // We may not return a LeftExpr here => make a copy
    if ( context.value()->mode() == KSValue::LeftExpr )
    {
      KSValue* v = new KSValue( *context.value() );
      context.setValue( v );
    }
  }
  // No return value
  else
  {
    // TODO: return the none object here -> faster
    context.setValue( new KSValue() );
  }

  context.setReturnFlag();

  return true;
}

bool KSEval_destructor_dcl( KSParseNode* node, KSContext& context )
{
  // We want an additional namespace in the scope
  KSNamespace nspace;
  KSSubScope scope( &nspace );
  context.scope()->pushLocalScope( &scope );

  // Fill parameters in our namespace
  if ( node->branch1() )
    if ( !node->branch1()->eval( context ) )
    {
      context.scope()->popLocalScope();
      return false;
    }

  // Are parameters left ?
  if ( !context.value()->listValue().isEmpty() )
  {
    const QString tmp( i18n("1 argument is not needed", "%n arguments are not needed", context.value()->listValue().count() ) );
    context.setException( new KSException( "TooManyArguments", tmp, node->getLineNo() ) );
    context.scope()->popLocalScope();
    return false;
  }

  // Call the function
  if ( node->branch2() )
    if ( !node->branch2()->eval( context ) )
    {
      context.scope()->popLocalScope();
      return false;
    }

  context.scope()->popLocalScope();
  return true;
}

bool KSEval_import( KSParseNode* node, KSContext& context )
{
    // KSNamespace space;
  // TODO: Find module in searchpath

  KSContext d( context );
  // This function puts a KSModule in d.value()
  if ( !context.interpreter()->runModule( d, node->getIdent() ) )
  {
    context.setException( d );
    return false;
  }

  // Register the imported module in the scope
  context.scope()->addObject( node->getIdent(), d.shareValue() );

  return true;
}

bool KSEval_t_struct( KSParseNode* node, KSContext& context )
{
  KSStructClass* p;

  // All children should know about the new KSStructClass
  context.setValue( new KSValue( ( p = new KSStructClass( context.scope()->module(), node->getIdent() ) ) ) );
  context.scope()->addObject( node->getIdent(), context.shareValue() );

  KSParseNode *left = node->branch1();
  if ( left )
      if ( !left->eval( context ) )
          return false;

  context.setValue( 0 );

  return true;
}

bool KSEval_t_struct_members( KSParseNode* node, KSContext& context )
{
  Q_ASSERT( context.value() && context.value()->type() == KSValue::StructClassType );

  context.value()->structClassValue()->addVariable( node->getIdent() );

  // process more members if available
  if ( node->branch1() )
    if ( !node->branch1()->eval( context ) )
      return false;

  return true;
}

extern bool KSEval_t_qualified_names( KSParseNode* node, KSContext& context )
{
  Q_ASSERT( context.value() && context.value()->type() == KSValue::ListType );

  KSParseNode *left = node->branch1();
  if ( !left )
    return true;

  KSContext l( context );
  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  context.value()->listValue().append( l.shareValue() );

  KSParseNode *right = node->branch2();
  if ( !right )
    return true;

  if ( !right->eval( context ) )
    return false;

  return true;
}

extern bool KSEval_t_scope( KSParseNode* node, KSContext& context )
{
    KSParseNode *left = node->branch1();
    // a construction like "{ }" ?
    if ( !left )
        return TRUE;

    KSNamespace nspace;
    context.scope()->localScope()->pushNamespace( &nspace );

    bool res = left->eval( context );

    context.scope()->localScope()->popNamespace();

    return res;
}

extern bool KSEval_t_try( KSParseNode* node, KSContext& context )
{
  KSNamespace nspace;
  context.scope()->localScope()->pushNamespace( &nspace );

  // Execute the questionable code
  KSParseNode *left = node->branch1();
  Q_ASSERT( left );
  // No error -> Return
  if ( left->eval( context ) )
  {
    context.scope()->localScope()->popNamespace();
    return true;
  }

  // We got an error. First resume the namespace. This
  // will do automatically a stack unwinding
  context.scope()->localScope()->popNamespace();

  // Execute the catch clauses
  KSParseNode *right = node->branch2();
  Q_ASSERT( right );
  return right->eval( context );
}

extern bool KSEval_t_catch( KSParseNode* node, KSContext& context )
{
  KSContext d( context );

  // Find the type to which we want to compare
  KSParseNode *left = node->branch1();
  Q_ASSERT( left );
  if ( !left->eval( d ) )
  {
    context.setException( d );
    return false;
  }

  // Exception of the correct type ?
  Q_ASSERT( context.exception() );
  if ( context.exception()->type()->cmp( *d.value() ) )
  {
     // Get infos about the exception
    KSValue* value = context.exception()->value();
    value->ref();

    // Add variables to the namespace
    KSNamespace nspace;
    nspace.insert( node->getIdent(), new KSValue( *value ) );
    context.scope()->localScope()->pushNamespace( &nspace );

    // Clear the exception since we caught it
    context.setException( 0 );

    // Evaluate the catch code
    KSParseNode *right = node->branch2();
    Q_ASSERT( right );

    /* bool res = */ right->eval( context );

    // Resume namespace
    context.scope()->localScope()->popNamespace();

    return true;
  }

  // Could not catch. Try next if available
  KSParseNode* more = node->branch4();
  if ( more )
    return more->eval( context );

  // We could not catch :-(
  return false;
}

extern bool KSEval_t_catch_default( KSParseNode* node, KSContext& context )
{
  KSContext d( context );

  // Find out na,me of the variable that
  // holds the type
  KSParseNode *left = node->branch1();
  Q_ASSERT( left );
  QString name1 = left->getIdent();

  // Clear the exception
  KSValue* type = context.exception()->type();
  type->ref();
  KSValue* value = context.exception()->value();
  value->ref();
  context.setException( 0 );

  // Add variables to the namespace
  KSNamespace nspace;
  nspace.insert( name1, new KSValue( *type ) );
  nspace.insert( node->getIdent(), new KSValue( *value ) );
  context.scope()->localScope()->pushNamespace( &nspace );

  // Evaluate the catch code
  KSParseNode *right = node->branch2();
  Q_ASSERT( right );
  bool res = right->eval( context );

  context.scope()->localScope()->popNamespace();

  return res;
}

extern bool KSEval_t_raise( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  // Raise the exception
  context.setException( new KSException( l.shareValue(), r.shareValue(), node->getLineNo() ) );

  return false;
}

extern bool KSEval_t_cell( KSParseNode* node, KSContext& context )
{
  return context.interpreter()->processExtension( context, node );
}

extern bool KSEval_t_range( KSParseNode* node, KSContext& context )
{
  return context.interpreter()->processExtension( context, node );
}

extern bool KSEval_from( KSParseNode* node, KSContext& context )
{
    // Get the list of symbols which have to be imported.
    QStringList lst = QStringList::split( "/", node->getStringLiteral() );

    KSContext d( context );
    // This function puts a KSModule in d.value()
    if ( !context.interpreter()->runModule( d, node->getIdent(), node->getIdent() + ".ks", QStringList() ) )
    {
        context.setException( d );
        return false;
    }

    // Register the imported module in the scope
    context.scope()->addObject( node->getIdent(), d.shareValue() );

    // Import all symbols ?
    // Syntax: "from mymodule import *;"
    if ( lst.isEmpty() )
    {
        // Iterate over all symbols of the module
        KSNamespace::Iterator it = d.value()->moduleValue()->nameSpace()->begin();
        KSNamespace::Iterator end = d.value()->moduleValue()->nameSpace()->end();
        for(; it != end; ++it )
            context.scope()->module()->addObject( it.key(), it.data() );
    }
    // Syntax: "from mymodule import sym1, sym2;"
    else
    {
        // Import from this module
        KSModule* m = d.value()->moduleValue();

        // Iterate over all symbols that we should import
        QStringList::ConstIterator sit = lst.begin();
        for( ; sit != lst.end(); ++sit )
        {
            // Symbol known ?
            KSValue* v = m->object( *sit );
            if ( !v )
            {
                QString tmp( i18n("The module %1 does not contain a symbol named %2") );
                context.setException( new KSException( "SymbolUnknown",
                                                       tmp.arg( node->getIdent() ).arg( *sit ),
                                                       node->getLineNo() ) );
                return false;
            }

            // Add the symbol to the current namespace
            v->ref();
            context.scope()->module()->addObject( *sit, v );
        }
    }

    return TRUE;
}

bool KSEval_plus_assign( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, true );

    if ( l.value()->mode() != KSValue::LeftExpr )
    {
        context.setException( new KSException( "NoLeftExpr", i18n("Expected a left expression in assignment."), node->getLineNo() ) );
        return false;
    }

    if ( l.value()->type() == KSValue::TimeType )
    {
        if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
            return false;
        QTime t = l.value()->timeValue();
        t = t.addSecs( r.value()->intValue() );
        l.value()->setValue( t );
    }
    else if ( l.value()->type() == KSValue::DateType )
    {
        if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
            return false;
        QDate d = l.value()->dateValue();
        d = d.addDays( r.value()->intValue() );
        l.value()->setValue( d );
    }
    else if ( !KSUtil::checkType( context, l.value(), r.value()->type(), true ) )
        return false;

    switch( l.value()->type() )
    {
    case KSValue::IntType:
        l.value()->setValue( r.value()->intValue() + l.value()->intValue() );
        break;
    case KSValue::DoubleType:
        l.value()->setValue( r.value()->doubleValue() + l.value()->doubleValue() );
        break;
    case KSValue::StringType:
        l.value()->setValue( l.value()->stringValue() + r.value()->stringValue() );
        break;
    case KSValue::ListType:
        l.value()->setValue( l.value()->listValue() + r.value()->listValue() );
        break;
    case KSValue::MapType:
        {
            QMap<QString,KSValue::Ptr>& map = l.value()->mapValue();
            QMap<QString,KSValue::Ptr>::ConstIterator it = r.value()->mapValue().begin();
            QMap<QString,KSValue::Ptr>::ConstIterator end = r.value()->mapValue().end();
            for( ; it != end; ++it )
                map.insert( it.key(), it.data() );
        }
        break;
    case KSValue::TimeType:
    case KSValue::DateType:
        // Handled above
        break;
    default:
      QString tmp( i18n("Operator += not defined for type %1") );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

    l.value()->setMode( KSValue::LeftExpr );

    context.setValue( l.shareValue() );

    return TRUE;
}

bool KSEval_minus_assign( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, true );

    if ( l.value()->mode() != KSValue::LeftExpr )
    {
        context.setException( new KSException( "NoLeftExpr", i18n("Expected a left expression in assignment."), node->getLineNo() ) );
        return false;
    }

    if ( l.value()->type() == KSValue::TimeType )
    {
        if ( KSUtil::checkType( context, r.value(), KSValue::TimeType, false ) )
        {
            QTime d = r.value()->timeValue();
            int diff = d.secsTo( l.value()->timeValue() );
            l.value()->setValue( (KScript::Long)diff );
        }
        else
        {
            if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
                return false;
            QTime t = l.value()->timeValue();
            t = t.addSecs( -r.value()->intValue() );
            l.value()->setValue( t );
        }
    }
    else if ( l.value()->type() == KSValue::DateType )
    {
        if ( KSUtil::checkType( context, r.value(), KSValue::DateType, false ) )
        {
            QDate d = r.value()->dateValue();
            int diff = d.daysTo( l.value()->dateValue() );
            l.value()->setValue( (KScript::Long)diff );
        }
        else
        {
            if ( !KSUtil::checkType( context, r.value(), KSValue::IntType, true ) )
                return false;
            QDate d = l.value()->dateValue();
            d = d.addDays( -r.value()->intValue() );
            l.value()->setValue( d );
        }
    }
    else if ( !KSUtil::checkType( context, l.value(), r.value()->type(), true ) )
        return false;
    else
    {
        switch( l.value()->type() )
        {
        case KSValue::IntType:
            l.value()->setValue( r.value()->intValue() + l.value()->intValue() );
            break;
        case KSValue::DoubleType:
            l.value()->setValue( r.value()->doubleValue() + l.value()->doubleValue() );
            break;
        case KSValue::StringType:
            l.value()->setValue( l.value()->stringValue() + r.value()->stringValue() );
            break;
        case KSValue::ListType:
            l.value()->setValue( l.value()->listValue() + r.value()->listValue() );
            break;
        case KSValue::MapType:
        {
            QMap<QString,KSValue::Ptr>& map = l.value()->mapValue();
            QMap<QString,KSValue::Ptr>::ConstIterator it = r.value()->mapValue().begin();
            QMap<QString,KSValue::Ptr>::ConstIterator end = r.value()->mapValue().end();
            for( ; it != end; ++it )
                map.insert( it.key(), it.data() );
        }
        break;
        case KSValue::TimeType:
        case KSValue::DateType:
            // Handled above
            break;
        default:
            QString tmp( i18n("Operator += not defined for type %1") );
            context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
            return false;
        }
    }

    l.value()->setMode( KSValue::LeftExpr );

    context.setValue( l.shareValue() );

    return TRUE;
}

bool KSEval_bool_or( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, false );

    if ( !KSUtil::checkType( context, l.value(), KSValue::BoolType, true ) ||
         !KSUtil::checkType( context, r.value(), KSValue::BoolType, true ) )
    {
        context.exception()->addLine( node->getLineNo() );
        return false;
    }

    context.setValue( new KSValue( (KScript::Boolean)( l.value()->boolValue() || r.value()->boolValue() ) ) );

    return true;
}

bool KSEval_bool_and( KSParseNode* node, KSContext& context )
{
    EVAL_OPS( context, l, r, false );

    if ( !KSUtil::checkType( context, l.value(), KSValue::BoolType, true ) ||
         !KSUtil::checkType( context, r.value(), KSValue::BoolType, true ) )
    {
        context.exception()->addLine( node->getLineNo() );
        return false;
    }

    context.setValue( new KSValue( (KScript::Boolean)( l.value()->boolValue() && r.value()->boolValue() ) ) );

    return true;
}

bool KSEval_t_regexp_group( KSParseNode* node, KSContext& context )
{
    KRegExp* exp = context.interpreter()->regexp();
    const char* grp = exp->group( node->getIntegerLiteral() );
    if ( grp )
        context.setValue( new KSValue( QString( grp ) ) );
    else
        context.setValue( new KSValue( QString( "" ) ) );

    return TRUE;
}

bool KSEval_t_input( KSParseNode*, KSContext& context )
{
    context.setValue( new KSValue( context.interpreter()->readInput() ) );

    return TRUE;
}

bool KSEval_t_line( KSParseNode* /*node*/, KSContext& context )
{
    context.setValue( context.interpreter()->lastInputLine() );

    return TRUE;
}

bool KSEval_t_match_line( KSParseNode* node, KSContext& context )
{
    KSValue::Ptr line = context.interpreter()->lastInputLine();
    if ( !KSUtil::checkType( context, line, KSValue::StringType, TRUE ) )
        return FALSE;

    KRegExp* exp = context.interpreter()->regexp();
    exp->compile( node->getIdent().latin1() ); // regexps are a-zA-Z etc, so latin1 is ok. (David)

    context.setValue( new KSValue( exp->match( line->stringValue().latin1() ) ) );

    return TRUE;
}
