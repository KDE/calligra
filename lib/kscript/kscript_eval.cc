#include "kscript_eval.h"
#include "kscript_value.h"
#include "kscript_context.h"
#include "kscript_func.h"
#include "kscript_class.h"
#include "kscript_object.h"
#include "kscript_struct.h"
#include "kscript_interface.h"
#include "kscript_corbafunc.h"
#include "kscript_proxy.h"
#include "kscript.h"
#include "kscript_parsenode.h"
#include <stdio.h>

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

// Try to reuse one of the KSValue objects or l or r
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
      ASSERT( context.scope() );
      context.scope()->addObject( node->branch1()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch1() ) ) );
    }
    else if ( !node->branch1()->eval( context ) )
      return false;
  }
  if ( node->branch2() )
  {
    if ( node->branch2()->getType() == func_dcl )
    {
      ASSERT( context.scope() );
      context.scope()->addObject( node->branch2()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch2() ) ) );
    }
    else if ( !node->branch2()->eval( context ) )
      return false;
  }

  return true;
}

bool KSEval_exports( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() );

  if ( context.value()->type() == KSValue::ClassType )
  {
    if ( node->branch1() )
    {
      if ( node->branch1()->getType() == func_dcl )
	context.value()->classValue()->nameSpace()->insert( node->branch1()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch1() ) ) );
      else if ( node->branch1()->getType() == destructor_dcl )
	context.value()->classValue()->nameSpace()->insert( node->branch1()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch1() ) ) );
      else if ( node->branch1()->getType() == signal_dcl )
	context.value()->classValue()->nameSpace()->insert( node->branch1()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch1() ) ) );
      else if ( !node->branch1()->eval( context ) )
	return false;
    }
    if ( node->branch2() )
    {
      if ( node->branch2()->getType() == func_dcl )
	context.value()->classValue()->nameSpace()->insert( node->branch2()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch2() ) ) );
      else if ( node->branch2()->getType() == destructor_dcl )
	context.value()->classValue()->nameSpace()->insert( node->branch2()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch2() ) ) );
      else if ( node->branch2()->getType() == signal_dcl )
	context.value()->classValue()->nameSpace()->insert( node->branch2()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch2() ) ) );
      else if ( !node->branch2()->eval( context ) )
	return false;
    }
  }
  else if ( context.value()->type() == KSValue::StructClassType )
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
  else if ( context.value()->type() == KSValue::InterfaceType )
  {
    if ( node->branch1() )
    {
      if ( node->branch1()->getType() == func_dcl )
	context.value()->interfaceValue()->nameSpace()->insert( node->branch1()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch1() ) ) );
      else if ( !node->branch1()->eval( context ) )
	return false;
    }
    if ( node->branch2() )
    {
      if ( node->branch2()->getType() == func_dcl )
	context.value()->interfaceValue()->nameSpace()->insert( node->branch2()->getIdent(), new KSValue( new KSScriptFunction( context.scope()->module(), node->branch2() ) ) );
      else if ( !node->branch2()->eval( context ) )
	return false;
    }
  }
  else
    ASSERT( 0 );

  return true;
}

bool KSEval_interface_dcl( KSParseNode* node, KSContext& context )
{
  KSParseNode *left = node->branch1();
  ASSERT( left );

  // All child nodes should know about the KSInterface
  context.setValue( new KSValue( new KSInterface( context.scope()->module(), left->getIdent() ) ) );

  if ( !left->eval( context ) )
    return false;

  KSParseNode *right = node->branch2();
  if ( right )
    if ( !right->eval( context ) )
      return false;

  // Tell the interpreter that we are responsible for a certain repoid
  const KSNamespace* n = context.value()->interfaceValue()->nameSpace();
  QMap<QString,KSValue::Ptr>::ConstIterator it = n->find( "repoid" );
  if ( it != n->end() )
    context.interpreter()->addRepoidImplementation( it.data()->stringValue(), context.shareValue() );
  else
    printf("Interface %s has no repoid\n",node->getIdent().ascii());

  context.setValue( 0 );

  return true; 
}

bool KSEval_interface_header( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::InterfaceType );

  // TODO: Avoid circles in super classes

  // Is the symbol already used ?
  KSValue* v = context.scope()->object( node->getIdent() );
  if ( v )
  {
    QString tmp( "The symbol %1 is defined twice" );
    context.setException( new KSException( "SymbolDefinedTwice", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  // Add the interface to the scope
  context.scope()->addObject( node->getIdent(), context.shareValue() );

  // Search super classes
  KSContext l( context );
  KSParseNode *left = node->branch1();
  if ( !left )
    // No super classes
    return true;

  l.setValue( new KSValue( KSValue::ListType ) );

  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  context.value()->interfaceValue()->setSuperInterfaces( l.value()->listValue() );

  return true;
}

bool KSEval_corba_func_dcl( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::InterfaceType );

  KSCorbaFunc* func = new KSCorbaFunc( context.scope()->module(), node );
  context.value()->interfaceValue()->nameSpace()->insert( node->getIdent(), new KSValue( func ) );

  // Dont evaluate the other branches here. They may refere to ScopedNames
  // which are yet not known. Instead we process the branches if the function is called
  // first. In addition this allows for a better startup time.
  return true;
}

bool KSEval_t_in_param_dcl( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::FunctionType );

  KSContext d( context );
  if ( node->branch1() )
    if ( !node->branch1()->eval( d ) )
    {
      context.setException( d );
      return false;
    }

  KSTypeCode::Ptr tc = KSTypeCode::typeCode( context, d.value() );
  if ( !tc )
    return false;

  ((KSCorbaFunc*)context.value()->functionValue())->addParameter( KSCorbaFunc::T_IN, node->getIdent(), tc );

  return true;
}

bool KSEval_t_out_param_dcl( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::FunctionType );

  KSContext d( context );
  if ( node->branch1() )
    if ( !node->branch1()->eval( d ) )
    {
      context.setException( d );
      return false;
    }

  KSTypeCode::Ptr tc = KSTypeCode::typeCode( context, d.value() );
  if ( !tc )
    return false;

  ((KSCorbaFunc*)context.value()->functionValue())->addParameter( KSCorbaFunc::T_OUT, node->getIdent(), tc );

  return true;
}

bool KSEval_t_inout_param_dcl( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::FunctionType );

  KSContext d( context );
  if ( node->branch1() )
    if ( !node->branch1()->eval( d ) )
    {
      context.setException( d );
      return false;
    }

  KSTypeCode::Ptr tc = KSTypeCode::typeCode( context, d.value() );
  if ( !tc )
    return false;

  ((KSCorbaFunc*)context.value()->functionValue())->addParameter( KSCorbaFunc::T_INOUT, node->getIdent(), tc );

  return true;
}


bool KSEval_param_dcls( KSParseNode* node, KSContext& context )
{
  if ( node->branch1() )
    if ( !node->branch1()->eval( context ) )
      return false;

  if ( node->branch2() )
    if ( !node->branch2()->eval( context ) )
      return false;

  return true;
}

bool KSEval_t_vertical_line( KSParseNode*, KSContext& ) { return false; }
bool KSEval_t_circumflex( KSParseNode*, KSContext& ) { return false; }
bool KSEval_t_ampersand( KSParseNode*, KSContext& ) { return false; }
bool KSEval_t_shiftright( KSParseNode*, KSContext& ) { return false; }
bool KSEval_t_shiftleft( KSParseNode*, KSContext& ) { return false; }

bool KSEval_t_plus_sign( KSParseNode* node, KSContext& context )
{
  // Unary ?
  if ( node->branch1() && !node->branch2() )
  {
    if ( !node->branch1()->eval( context ) )
      return false;
    if ( context.value()->cast( KSValue::IntType ) )
      return true;
    if ( context.value()->cast( KSValue::DoubleType ) )
      return true;

    QString tmp( "Unary Operator * not defined for type %1" );
    context.setException( new KSException( "UnknownOperation", tmp.arg( context.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  EVAL_OPS( context, l, r, false );

  if ( !r.value()->cast( l.value()->type() ) )
  {
    QString tmp( "From %1 to %2" );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ).arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
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
	QString result = r.value()->stringValue() + l.value()->stringValue();
	FILL_VALUE( context, l, r );
	context.value()->setValue( result );
	return true;
      }
      break;
    default:
      QString tmp( "Operator * not defined for type %1" );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_minus_sign( KSParseNode*, KSContext& ) { return false; }

bool KSEval_t_asterik( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !r.value()->cast( l.value()->type() ) )
  {
    QString tmp( "From %1 to %2" );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ).arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  switch( l.value()->type() )
    {
    case KSValue::IntType:
      {
	KScript::Long result = r.value()->intValue() * l.value()->intValue();
	FILL_VALUE( context, l, r );
	context.value()->setValue( result );
	return true;
      }
      break;
    case KSValue::DoubleType:
      {
	KScript::Double result = r.value()->doubleValue() * l.value()->doubleValue();
	FILL_VALUE( context, l, r );
	context.value()->setValue( result );
	return true;
      }
      break;
    default:
      QString tmp( "Operator * not defined for type %1" );
      context.setException( new KSException( "UnknownOperation", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
      return false;
    }

  // Never reached
  return false;
}

bool KSEval_t_solidus( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_percent_sign( KSParseNode* , KSContext& ) { return false; }
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

bool KSEval_t_attribute( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::InterfaceType );

  ASSERT( node->branch1() );
  KSContext d( context );
  if ( !node->branch1()->eval( d ) )
  {
    context.setException( d );
    return false;
  }

  KSTypeCode::Ptr tc = KSTypeCode::typeCode( context, d.value() );
  if ( !tc )
    return false;

  KSAttribute::Access attr = KSAttribute::Normal;
  if ( node->getType() == t_readonly_attribute )
    attr = KSAttribute::ReadOnly;

  KSValue::Ptr v = new KSValue( new KSAttribute( context.scope()->module(), node->getIdent(), attr, tc ) );
  context.value()->interfaceValue()->nameSpace()->insert( node->getIdent(), v );

  return true;
}

bool KSEval_t_readonly_attribute( KSParseNode* node, KSContext& context )
{
  return KSEval_t_attribute( node, context );
}

bool KSEval_raises_expr( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::FunctionType );

  // Get the list of exceptions
  KSContext d( context );
  d.setValue( new KSValue( KSValue::ListType ) );
  if ( node->branch1() )
  {
    if ( !node->branch1()->eval( d ) )
    {
      context.setException( d );
      return false;
    }
  }

  // Add all exceptions
  QValueList<KSValue::Ptr>::Iterator it = d.value()->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = d.value()->listValue().end();
  for( ; it != end; ++it )
  {
    if ( (*it)->type() != KSValue::StructClassType )
    {
      context.setException( new KSException( "InvalidException", "", node->getLineNo() ) );
      return false;
    }
    ((KSCorbaFunc*)context.value()->functionValue())->addException( *it );
  }

  return true;
}

bool KSEval_const_dcl( KSParseNode* node, KSContext& context )
{
  ASSERT( node->branch1() );

  KSContext( l );
  if ( !node->branch1()->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  if ( !context.value() )
    context.scope()->addObject( node->getIdent(), l.shareValue() );
  else if ( context.value()->type() == KSValue::ClassType )  
    context.value()->classValue()->nameSpace()->insert( node->getIdent(), l.shareValue() );
  else if ( context.value()->type() == KSValue::StructClassType )  
    context.value()->structClassValue()->nameSpace()->insert( node->getIdent(), l.shareValue() );
  else if ( context.value()->type() == KSValue::InterfaceType )  
    context.value()->interfaceValue()->nameSpace()->insert( node->getIdent(), l.shareValue() );
  else
    ASSERT( 0 );

  return true;
}

bool KSEval_t_pragma( KSParseNode*, KSContext& ) { return false; }

bool KSEval_class_dcl( KSParseNode* node, KSContext& context )
{
  KSParseNode *left = node->branch1();
  ASSERT( left );

  // All child nodes should know about the new KSScriptClass
  context.setValue( new KSValue( new KSScriptClass( context.scope()->module(), left->getIdent(), node ) ) );

  if ( !left->eval( context ) )
    return false;

  KSParseNode *right = node->branch2();
  if ( right )
    if ( !right->eval( context ) )
      return false;
  
  context.setValue( 0 );

  return true; 
}

bool KSEval_class_header( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::ClassType );

  // TODO: Avoid circles in super classes

  // Is the symbol already used ?
  KSValue* v = context.scope()->object( node->getIdent() );
  if ( v )
  {
    QString tmp( "The symbol %1 is defined twice" );
    context.setException( new KSException( "SymbolDefinedTwice", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  context.scope()->addObject( node->getIdent(), context.shareValue() );

  // Search super classes
  KSContext l( context );
  KSParseNode *left = node->branch1();
  if ( !left )
    return true;

  l.setValue( new KSValue( KSValue::ListType ) );

  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

  context.value()->classValue()->setSuperClasses( l.value()->listValue() );

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
    QString tmp( "%1 arguments are not needed" );
    context.setException( new KSException( "TooManyArguments", tmp.arg( context.value()->listValue().count() ), node->getLineNo() ) );
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
    context.setException( new KSException( "NoLeftExpr", "Expected a left expression in assignment", node->getLineNo() ) );
    return false;
  }

  // Special handling for strings
  if ( l.value()->type() == KSValue::CharRefType )
  {
    if ( !r.value()->cast( KSValue::CharType ) )
    {
      QString tmp( "From %1 to Char" );
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

  if ( !r.value()->cast( l.value()->type() ) )
  {
    QString tmp( "From %1 to %2" );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ).arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  KScript::Boolean result = ( r.value()->cmp( *l.value() ) );
  FILL_VALUE( context, l, r );
  context.value()->setValue( result );
  return true;
}

bool KSEval_t_notequal( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !r.value()->cast( l.value()->type() ) )
  {
    QString tmp( "From %1 to %2" );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ).arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  KScript::Boolean result = !( r.value()->cmp( *l.value() ) );
  FILL_VALUE( context, l, r );
  context.value()->setValue( result );
  return true;
}

bool KSEval_t_less_or_equal( KSParseNode*, KSContext& ) { return false; }
bool KSEval_t_greater_or_equal( KSParseNode*, KSContext& ) { return false; }

bool KSEval_t_array( KSParseNode* node, KSContext& context )
{
  EVAL_OPS( context, l, r, false );

  if ( !r.value()->cast( KSValue::IntType ) )
  {
    QString tmp( "From %1 to Integer in array index" );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  int index = r.value()->intValue();

  if ( index < 0 )
  {
    QString tmp( "Negative array index %1");
    context.setException( new KSException( "IndexOutOfRange", tmp.arg( index ), node->getLineNo() ) );
    return false;
  }

  // is it a string ? -> special handling
  if ( l.value()->type() == KSValue::StringType )
  {
    int len = l.value()->stringValue().length();
    
    if ( index >= len && !context.leftExpr() )
    {
      QString tmp( "Too large index %1");
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
    QString tmp( "From %1 to List" );
    context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  int len = l.value()->listValue().count();
  if ( index >= len )
  {
    if ( !context.leftExpr() )
    {
      QString tmp( "Too large index %1");
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
    QString tmp( "From %1 to String in dict" );
    context.setException( new KSException( "CastingError", tmp.arg( r.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  if ( !l.value()->cast( KSValue::MapType ) )
  {
    QString tmp( "From %1 to Map" );
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
      context.setException( new KSException( "NoLeftExpr", "Expected a left expression", node->getLineNo() ) );
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
      QString tmp( "Argument for parameters %1 missing" );
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
    QString tmp( "Argument for parameters %1 missing" );
    context.setException( new KSException( "ToFewArguments", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  KSValue* arg = *(context.value()->listValue().begin());

  // Is the argument not a leftexpr ?
  if ( arg->mode() != KSValue::LeftExpr )
  {
    QString tmp( "LeftExpr needed for parameter %1" );
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
    QString tmp( "Argument for parameters %1 missing" );
    context.setException( new KSException( "ToFewArguments", tmp.arg( node->getIdent() ), node->getLineNo() ) );
    return false;
  }

  KSValue* arg = *(context.value()->listValue().begin());

  // Is the argument not a leftexpr ?
  if ( arg->mode() != KSValue::LeftExpr )
  {
    QString tmp( "LeftExpr needed for parameter %1" );
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

  if ( !l.value()->cast( KSValue::FunctionType ) && !l.value()->cast( KSValue::ClassType ) &&
       !l.value()->cast( KSValue::MethodType ) && !l.value()->cast( KSValue::StructClassType ) &&
       !l.value()->cast( KSValue::InterfaceType ) )
  {
    QString tmp( "From %1 to Function" );
    context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  // Create a list of parameters
  context.setValue( new KSValue( KSValue::ListType ) );

  KSParseNode *right = node->branch2();
  if ( right )
    if ( !right->eval( context ) )
      return false;

  // Remove our namespaces
  KSSubScope* scope = context.scope()->popLocalScope();
  KSModule* module = context.scope()->popModule();

  bool b;
  if ( l.value()->cast( KSValue::FunctionType ) )
  {
    context.scope()->pushModule( l.value()->functionValue()->module() );
    // Call the function
    b = l.value()->functionValue()->call( context );
    context.scope()->popModule();
  }
  else if ( l.value()->cast( KSValue::ClassType ) )
  {
    context.scope()->pushModule( l.value()->classValue()->module() );
    // Call constructor
    b = l.value()->classValue()->constructor( node, context );
    context.scope()->popModule();
  }
  else if ( l.value()->cast( KSValue::StructClassType ) )
  {
    context.scope()->pushModule( l.value()->structClassValue()->module() );
    // Call struct constructor
    b = l.value()->structClassValue()->constructor( context );
    context.scope()->popModule();
  }
  else if ( l.value()->cast( KSValue::InterfaceType ) )
  {
    context.scope()->pushModule( l.value()->interfaceValue()->module() );
    // Call struct constructor
    b = l.value()->interfaceValue()->constructor( context );
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
    ASSERT( 0 );
    
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
  ASSERT( left );

  // This resets leftExpr to FALSE
  KSContext l( context );
  // Try to find the object
  if ( !left->eval( l ) )
  {
    context.setException( l );
    return false;
  }

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
  // Special handling for classess
  else if ( l.value()->cast( KSValue::ClassType ) )
  {
    KSValue::Ptr v = l.value()->classValue()->member( context, node->getIdent() );
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
  // Special handling for interfaces
  else if ( l.value()->cast( KSValue::InterfaceType ) )
  {
    KSValue::Ptr v = l.value()->interfaceValue()->member( context, node->getIdent() );
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
  if ( l.value()->cast( KSValue::ObjectType ) )
  {
    v = l.value()->objectValue()->member( context, node->getIdent() );
    module = l.value()->objectValue()->module();
  }
  else if ( l.value()->cast( KSValue::StructType ) )
  {
    v = l.value()->structValue()->member( context, node->getIdent() );
    module = l.value()->structValue()->module();
  }
  else if ( l.value()->cast( KSValue::ProxyType ) )
  {
    v = l.value()->proxyValue()->member( context, node->getIdent() );
    module = l.value()->proxyValue()->module();
  }
  else
  {
    QString tmp( "From %1 to Object" );
    context.setException( new KSException( "CastingError", tmp.arg( l.value()->typeName() ), node->getLineNo() ) );
    return false;
  }

  if ( !v )
  {
    context.exception()->addLine( node->getLineNo() );
    return false;
  }

  if ( v->type() == KSValue::FunctionType || v->type() == KSValue::BuiltinMethodType ||
       v->type() == KSValue::StructBuiltinMethodType || v->type() == KSValue::ProxyBuiltinMethodType )
    context.setValue( new KSValue( new KSMethod( module, l.shareValue(), v ) ) );
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
    QString tmp( "From %1 to String" );
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

bool KSEval_t_new( KSParseNode*, KSContext& ) { return false; }
bool KSEval_t_delete( KSParseNode*, KSContext& ) { return false; }

bool KSEval_t_while( KSParseNode* node, KSContext& context )
{
  do
  {
    EVAL_LEFT_OP( context, l );

    if ( !l.value()->cast( KSValue::BoolType ) )
    {
      QString tmp( "From %1 to Boolean" );
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

bool KSEval_t_do( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_for( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_if( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_else( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_incr( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_decr( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_for_head( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_less( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_greater( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_foreach( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_match( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_subst( KSParseNode* , KSContext& ) { return false; }
bool KSEval_t_not( KSParseNode* , KSContext& ) { return false; }

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
    QString tmp( "%1 arguments are not needed" );
    context.setException( new KSException( "TooManyArguments", tmp.arg( context.value()->listValue().count() ), node->getLineNo() ) );
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

bool KSEval_signal_dcl( KSParseNode* node, KSContext& context )
{
  // Get the object. It is always the first parameter
  KSValue::Ptr v = context.value()->listValue().first();
  // Remove it from the arguments list
  context.value()->listValue().remove( context.value()->listValue().begin() );

  // Fill in default arguments if needed
  context.tmpInt = 0;
  if ( node->branch1() )
    if ( !node->branch1()->eval( context ) )
      return false;

  // Are parameters left ?
  if ( context.value()->listValue().count() != (uint)context.tmpInt )
  {
    QString tmp( "%1 arguments are not needed" );
    context.setException( new KSException( "TooManyArguments", tmp.arg( context.value()->listValue().count() - context.tmpInt ), node->getLineNo() ) );
    return false;
  }

  // Call the signal
  if ( !v->objectValue()->emitSignal( node->getIdent(), context ) )
    return false;
  
  return true;
}

bool KSEval_signal_params( KSParseNode* node, KSContext& context )
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

bool KSEval_signal_param( KSParseNode* node, KSContext& context )
{
  KSValue* v = 0;

  QValueList<KSValue::Ptr>::Iterator it = context.value()->listValue().at( context.tmpInt );

  // No more arguments ?
  if ( it == context.value()->listValue().end() )
  {
    // Do we have a default Argument ?
    if ( node->branch1() )
    {
      KSContext d( context );
      if ( !node->branch1()->eval( d ) )
	return false;
      if ( d.value()->mode() == KSValue::Temp )
      {
	v = d.value();
	v->ref();
      }
      else
	v = new KSValue( *d.value() );

      context.value()->listValue().append( v );
    }
    else
    {
      QString tmp( "Argument for parameters %1 missing" );
      context.setException( new KSException( "ToFewArguments", tmp.arg( node->getIdent() ), node->getLineNo() ) );
      return false;
    }
  }

  context.tmpInt++;

  return true;
}

bool KSEval_t_emit( KSParseNode* node, KSContext& context )
{
  KSParseNode *left = node->branch1();
  ASSERT( left );

  return left->eval( context );
}

bool KSEval_import( KSParseNode* node, KSContext& context )
{
  KSNamespace space;
  // TODO: Find module in searchpath

  KSContext d( context );
  // This function puts a KSModule in d.value()
  if ( !context.interpreter()->runModule( d, node->getIdent(), node->getIdent() + ".ks" ) )
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

  // A struct in an interface or class ?
  if ( context.value() && ( context.value()->type() == KSValue::InterfaceType ||
			    context.value()->type() == KSValue::ClassType ) )
  {
    p = new KSStructClass( context.scope()->module(), node->getIdent(), node );
    KSValue::Ptr v = new KSValue( p );

    if ( context.value()->type() == KSValue::ClassType )
      context.value()->classValue()->nameSpace()->insert( node->getIdent(), v );
    else
      context.value()->interfaceValue()->nameSpace()->insert( node->getIdent(), v );

    KSParseNode *left = node->branch1();
    if ( !left )
      return true;

    KSContext d( context );
    // All children should know about the new KSStructClass
    d.setValue( v );
    if ( !left->eval( d ) )
    {
      context.setException( d );
      return false;
    }
  }
  else
  {
    // All children should know about the new KSStructClass
    context.setValue( new KSValue( ( p = new KSStructClass( context.scope()->module(), node->getIdent(), node ) ) ) );
    context.scope()->addObject( node->getIdent(), context.shareValue() );
  
    KSParseNode *left = node->branch1();
    if ( left )
      if ( !left->eval( context ) )
	return false;

    context.setValue( 0 );
  }

  // Tell the interpreter that we are responsible for a certain repoid
  QMap<QString,KSValue::Ptr>::Iterator it = p->nameSpace()->find( "repoid" );
  if ( it != p->nameSpace()->end() )
  {
    p->ref();
    context.interpreter()->addRepoidImplementation( it.data()->stringValue(), new KSValue( p ) );
  }

  return true; 
}

bool KSEval_t_struct_members( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::StructClassType );

  context.value()->structClassValue()->addVariable( node->getIdent() );

  // process more members if available
  if ( node->branch1() )
    if ( !node->branch1()->eval( context ) )
      return false;

  return true;
}

extern bool KSEval_t_qualified_names( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::ListType );

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
  KSNamespace nspace;
  context.scope()->localScope()->pushNamespace( &nspace );

  KSParseNode *left = node->branch1();
  ASSERT( left );
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
  ASSERT( left );
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
  ASSERT( right );
  return right->eval( context );
}

extern bool KSEval_t_catch( KSParseNode* node, KSContext& context )
{
  KSContext d( context );

  // Find the type to which we want to compare
  KSParseNode *left = node->branch1();
  ASSERT( left );
  if ( !left->eval( d ) )
  {
    context.setException( d );
    return false;
  }

  // Exception of the correct type ?
  ASSERT( context.exception() );
  if ( context.exception()->type()->cmp( *d.value() ) )
  {
     // Get infos about the exception
    KSValue* value = context.exception()->value();
    value->ref();

    // Add variables to the namespace
    KSNamespace nspace;
    nspace.insert( node->getIdent(), new KSValue( *value ) );
    context.scope()->localScope()->pushNamespace( &nspace );

    // Clear the exception since we catched it
    context.setException( 0 );

    // Evaluate the catch code
    KSParseNode *right = node->branch2();
    ASSERT( right );

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
  ASSERT( left );
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
  ASSERT( right );
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
