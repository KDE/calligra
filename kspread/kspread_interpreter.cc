#include "kspread_interpreter.h"
#include "kspread_util.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_cell.h"
#include "kspread_table.h"

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <klocale.h>
#include <kapp.h>
#include <qdatetime.h>

#include <kdebug.h>

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
  KSParseNodeExtraPoint( const QString& s, KSpreadMap* m, KSpreadTable* t ) : m_point( s, m, t ) { }

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
  KSParseNodeExtraRange( const QString& s, KSpreadMap* m, KSpreadTable* t ) : m_range( s, m, t ) { }

  KSpreadRange* range() { return &m_range; }

private:
  KSpreadRange m_range;
};

/****************************************************
 *
 * Helper functions
 *
 ****************************************************/

/**
 * Creates dependencies from the parse tree of a formula.
 */
void makeDepends( KSContext& context, KSParseNode* node, KSpreadMap* m, KSpreadTable* t, QList<KSpreadDepend>& depends )
{
  KSParseNodeExtra* extra = node->extra();
  if ( !extra )
  {
    if ( node->getType() == t_cell )
    {
      KSParseNodeExtraPoint* extra = new KSParseNodeExtraPoint( node->getStringLiteral(), m, t );
      kdDebug(36002) << "-------- Got dep " << util_cellName( extra->point()->pos.x(), extra->point()->pos.y() ) << endl;
      KSpreadDepend* d = new KSpreadDepend;
      d->m_iColumn = extra->point()->pos.x();
      d->m_iRow = extra->point()->pos.y();
      d->m_iColumn2 = -1;
      d->m_iRow2 = -1;
      d->m_pTable = extra->point()->table;
      if (!d->m_pTable)
      {
        QString tmp( i18n("The expression %1 is not valid") );
        tmp = tmp.arg( node->getStringLiteral() );
        context.setException( new KSException( "InvalidTableExpression", tmp ) );
        return;
      }
      depends.append( d );
      node->setExtra( extra );
    }
    else if ( node->getType() == t_range )
    {
      KSParseNodeExtraRange* extra = new KSParseNodeExtraRange( node->getStringLiteral(), m, t );
      KSpreadDepend* d = new KSpreadDepend;
      d->m_iColumn = extra->range()->range.left();
      d->m_iRow = extra->range()->range.top();
      d->m_iColumn2 = extra->range()->range.right();
      d->m_iRow2 = extra->range()->range.bottom();
      d->m_pTable = extra->range()->table;
      if (!d->m_pTable)
      {
        QString tmp( i18n("The expression %1 is not valid") );
        tmp = tmp.arg( node->getStringLiteral() );
        context.setException( new KSException( "InvalidTableExpression", tmp ) );
        return;
      }
      depends.append( d );
      node->setExtra( extra );
    }
  }

  if ( node->branch1() )
    makeDepends( context, node->branch1(), m, t, depends );
  if ( node->branch2() )
    makeDepends( context, node->branch2(), m, t, depends );
  if ( node->branch3() )
    makeDepends( context, node->branch3(), m, t, depends );
  if ( node->branch4() )
    makeDepends( context, node->branch4(), m, t, depends );
  if ( node->branch5() )
    makeDepends( context, node->branch5(), m, t, depends );
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

/*********************************************************************
 *
 * Module with global functions like "sin", "cos", "sum" etc.
 *
 *********************************************************************/

static bool kspreadfunc_sin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sin", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if( !KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
	return false;
    }
  val=args[0]->doubleValue();

  context.setValue( new KSValue( sin( val ) ) );

  return true;
}




static bool kspreadfunc_cos( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "cos", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( cos( val ) ) );

  return true;
}

static bool kspreadfunc_sqrt( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sqrt", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( sqrt( val ) ) );

  return true;
}

static bool kspreadfunc_sqrtn( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "SQRTn", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  context.setValue( new KSValue( exp( log(args[0]->doubleValue())/args[1]->intValue() ) ) );

  return true;
}

static bool kspreadfunc_cur( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "CUR", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  context.setValue( new KSValue( exp( log(args[0]->doubleValue())/3)) );

  return true;
}


static bool kspreadfunc_fabs( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "fabs", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( fabs(val ) ) );

  return true;
}

static bool kspreadfunc_tan( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "tan", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();
  context.setValue( new KSValue( tan(val) ) );

  return true;
}

static bool kspreadfunc_exp( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "exp",true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( exp( val ) ) );

  return true;
}

static bool kspreadfunc_ceil( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ceil", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if (approx_equal(floor(args[0]->doubleValue()),args[0]->doubleValue()))
    context.setValue( new KSValue(args[0]->doubleValue()));
  else
    context.setValue( new KSValue( ceil( args[0]->doubleValue() ) ) );

  return true;
}

static bool kspreadfunc_floor( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "floor", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( floor( val ) ) );

  return true;
}

static bool kspreadfunc_atan( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "atan", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( atan( val ) ) );

  return true;
}

static bool kspreadfunc_ln( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ln", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( log( val ) ) );

  return true;
}

static bool kspreadfunc_logn( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "LOGn", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  context.setValue( new KSValue( log( args[0]->doubleValue() ) /log((double)args[1]->intValue() )) );

  return true;
}


static bool kspreadfunc_asin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "asin", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( asin( val ) ) );

  return true;
}

static bool kspreadfunc_acos( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "acos", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();


  context.setValue( new KSValue( acos( val ) ) );

  return true;
}

static bool kspreadfunc_log( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "log", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( log10( val ) ) );

  return true;
}

static bool kspreadfunc_asinh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "asinh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( asinh( val ) ) );

  return true;
}

static bool kspreadfunc_acosh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "acosh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( acosh( val ) ) );

  return true;
}

static bool kspreadfunc_atanh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "atanh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();


  context.setValue( new KSValue( atanh( val ) ) );

  return true;
}

static bool kspreadfunc_tanh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "tanh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( tanh( val ) ) );

  return true;
}

static bool kspreadfunc_sinh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sinh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( sinh( val ) ) );

  return true;
}

static bool kspreadfunc_cosh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "cosh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( cosh( val ) ) );

  return true;
}

static bool kspreadfunc_degree( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "degree", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( (val*180)/M_PI  ));

  return true;
}

static bool kspreadfunc_radian( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "radian", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();


  context.setValue( new KSValue( (val*M_PI )/180  ));

  return true;
}


static bool kspreadfunc_sum_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sum_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      result += (*it)->doubleValue();
      }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_sum( KSContext& context )
{
  double result = 0.0;
  bool b = kspreadfunc_sum_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_sumsq_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sum_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      result += ((*it)->doubleValue()*(*it)->doubleValue());
      }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_sumsq( KSContext& context )
{
  double result = 0.0;
  bool b = kspreadfunc_sumsq_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}


static bool kspreadfunc_max_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result,int& inter)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {

      if ( !kspreadfunc_max_helper( context, (*it)->listValue(), result,inter ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      if(inter == 0)
      {
        result=(*it)->doubleValue();
        inter=1;
      }
      if(result <  (*it)->doubleValue())
        result =(*it)->doubleValue();
    }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_max( KSContext& context )
{
  double result = 0.0;

  //init first element
  int inter=0;

  bool b = kspreadfunc_max_helper( context, context.value()->listValue(), result ,inter );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_min_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result,int& inter)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {

      if ( !kspreadfunc_min_helper( context, (*it)->listValue(), result,inter ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      if(inter == 0)
      {
        result=(*it)->doubleValue();
        inter=1;
      }
      if(result >  (*it)->doubleValue())
        result =(*it)->doubleValue();
    }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_min( KSContext& context )
{
  double result = 0.0;

  //init first element
  int inter=0;

  bool b = kspreadfunc_min_helper( context, context.value()->listValue(), result ,inter );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_average_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result,int& number)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_average_helper( context, (*it)->listValue(), result ,number) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      result += (*it)->doubleValue();
      number++;
      }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_average( KSContext& context )
{
  double result = 0.0;

  int number=0;
  bool b = kspreadfunc_average_helper( context, context.value()->listValue(), result ,number);

  if ( number == 0 )
    {
    context.setValue( new KSValue( i18n("#DIV/0") ) );
    return true;
    }

  if ( b )
    context.setValue( new KSValue( result / (double)number ) );

  return b;
}

static bool kspreadfunc_variance_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result, double avera)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_variance_helper( context, (*it)->listValue(), result ,avera) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      result += ( (*it)->doubleValue() - avera ) * ( (*it)->doubleValue() - avera );
      }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_variance( KSContext& context )
{
  double result = 0.0;
  double avera = 0.0;
  int number = 0;
  bool b = kspreadfunc_average_helper( context, context.value()->listValue(), result ,number);

  if ( number == 0 )
      return false;

  if ( b )
  {
    avera = result / (double)number;
    result = 0.0;
    bool b = kspreadfunc_variance_helper( context, context.value()->listValue(), result, avera );
    if(b)
      context.setValue( new KSValue(result / (double)number ) );
  }

  return b;
}

static bool kspreadfunc_stddev_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result,double& avera)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_stddev_helper( context, (*it)->listValue(), result ,avera) )
        return false;

    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result += (((*it)->doubleValue()-avera)*((*it)->doubleValue()-avera));
    }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_stddev( KSContext& context )
{
  double result = 0.0;
  double avera = 0.0;
  int number=0;
  bool b = kspreadfunc_average_helper( context, context.value()->listValue(), result ,number);

  if ( number == 0 )
      return false;

  if ( b )
  {
    avera=result/number;
    result=0.0;
    bool b = kspreadfunc_stddev_helper( context, context.value()->listValue(), result,avera );
    if(b)
      context.setValue( new KSValue(sqrt(result/number )) );
  }

  return b;
}

static bool kspreadfunc_mult_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_mult_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      result *= (*it)->doubleValue();
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

static bool kspreadfunc_mult( KSContext& context )
{
  double result = 1.0;
  bool b = kspreadfunc_mult_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}


static bool kspreadfunc_join_helper( KSContext& context, QValueList<KSValue::Ptr>& args, QString& tmp )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();
  QString tmp2;
  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_join_helper( context, (*it)->listValue(), tmp ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
      tmp+= (*it)->stringValue();
    else if( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      tmp+= KGlobal::locale()->formatNumber((*it)->doubleValue());
    else
      return false;
  }
  return true;
}

static bool kspreadfunc_join( KSContext& context )
{
  QString tmp;
  bool b = kspreadfunc_join_helper( context, context.value()->listValue(), tmp );

  if ( b )
    context.setValue( new KSValue( tmp ) );

  return b;
}

static bool kspreadfunc_not( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "NOT", true ) || !KSUtil::checkArgumentsCount( context, 1, "not", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
    return false;

  bool toto = !args[0]->boolValue();
  context.setValue( new KSValue(toto));
  return true;
}


static bool kspreadfunc_or_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_or_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = (first || (*it)->boolValue());
    else
      return false;
  }

  return true;
}

static bool kspreadfunc_or( KSContext& context )
{
  bool first = false;
  bool b = kspreadfunc_or_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

static bool kspreadfunc_nor( KSContext& context )
{
  bool first = false;
  bool b = kspreadfunc_or_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( !first ) );

  return b;
}


static bool kspreadfunc_and_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_and_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = first && (*it)->boolValue();
    else
      return false;
  }

  return true;
}

static bool kspreadfunc_and( KSContext& context )
{
  bool first = true;
  bool b = kspreadfunc_and_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

static bool kspreadfunc_nand( KSContext& context )
{
  bool first = true;
  bool b = kspreadfunc_and_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( !first ) );

  return b;
}


static bool kspreadfunc_if( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "if", true ) || !KSUtil::checkArgumentsCount( context, 3, "IF", true ))
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
      return false;

    if (  args[0]->boolValue() == true )
      context.setValue( new KSValue( *(args[1]) ) );
    else
      context.setValue( new KSValue( *(args[2]) ) );

    return true;
}

static bool kspreadfunc_left( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "left", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;
    int nb;
    if( KSUtil::checkType( context, args[1], KSValue::DoubleType, false ) )
      nb = (int) args[1]->doubleValue();
    else if( KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
      nb = args[1]->intValue();
    else
      return false;

    QString tmp = args[0]->stringValue().left(nb);
    context.setValue( new KSValue( tmp ) );
    return true;
}

static bool kspreadfunc_right( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "right", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;
    int nb;
    if( KSUtil::checkType( context, args[1], KSValue::DoubleType, false ) )
      nb = (int) args[1]->doubleValue();
    else if( KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
      nb = args[1]->intValue();
    else
      return false;

    QString tmp = args[0]->stringValue().right(nb);
    context.setValue( new KSValue(tmp));
    return true;
}

static bool kspreadfunc_upper( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "upper", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    QString tmp = args[0]->stringValue().upper();
    context.setValue( new KSValue( tmp ) );
    return true;
}

static bool kspreadfunc_lower( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "lower", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    QString tmp = args[0]->stringValue().lower();
    context.setValue( new KSValue( tmp ) );
    return true;
}

static bool kspreadfunc_find( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "find", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
      return false;

    QString string_find = args[0]->stringValue();
    QString string = args[1]->stringValue();
    bool exist=(string.find(string_find)!=-1)?true:false;
    context.setValue( new KSValue( exist ) );
    return true;
}



static bool kspreadfunc_mid( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    uint len = 0xffffffff;
    if ( KSUtil::checkArgumentsCount( context, 3, "mid", false ) )
    {
      if( KSUtil::checkType( context, args[2], KSValue::DoubleType, false ) )
        len = (uint) args[2]->doubleValue();
      else if( KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
        len = (uint) args[2]->intValue();
      else
        return false;
    }
    else if ( !KSUtil::checkArgumentsCount( context, 2, "mid", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;
    int pos;
    if( KSUtil::checkType( context, args[1], KSValue::DoubleType, false ) )
      pos = (int) args[1]->doubleValue();
    else if( KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
      pos = args[1]->intValue();
    else
      return false;

    QString tmp = args[0]->stringValue().mid( pos, len );
    context.setValue( new KSValue(tmp));
    return true;
}

static bool kspreadfunc_len( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "len", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  int nb=args[0]->stringValue().length();
  context.setValue( new KSValue(nb));
  return true;
}

static bool kspreadfunc_EXACT( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "EXACT", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
    return false;
  bool exact = args[1]->stringValue() == args[0]->stringValue();
  context.setValue( new KSValue(exact) );
  return true;
}



static bool kspreadfunc_INT( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "INT", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
    if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
      return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue((int)val));
  return true;
}

static bool kspreadfunc_PI( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 0, "PI", true ) )
      return false;

    context.setValue( new KSValue(M_PI));
    return true;
}

static bool kspreadfunc_rand( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 0, "rand", true ) )
      return false;

    context.setValue( new KSValue((double) rand()/(RAND_MAX + 1.0)));
    return true;
}

static bool kspreadfunc_randbetween( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "RANDBETWEEN", true ) )
      return false;
    if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        return false;
    if( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
        return false;
    if(args[0]->intValue()>args[1]->intValue())
        {
        context.setValue( new KSValue(i18n("Err")));
        return true;
        }
    context.setValue( new KSValue((double)(((double)args[1]->intValue())*rand()/(RAND_MAX+((double)args[0]->intValue())))));
    return true;
}


static bool kspreadfunc_REPT( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "REPT", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;
  if( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  int nb=(int) args[1]->doubleValue();
  QString tmp=args[0]->stringValue();
  QString tmp1;
  for (int i=0 ;i<nb;i++)
    tmp1+=tmp;
  context.setValue( new KSValue(tmp1));
  return true;
}

static bool kspreadfunc_islogic( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISLOGIC", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::BoolType, true );
  context.setValue( new KSValue(logic));
  return true;
}

static bool kspreadfunc_istext( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISTEXT", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::StringType, true );
  context.setValue( new KSValue(logic));
  return true;
}

static bool kspreadfunc_isnottext( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISNOTTEXT", true ) )
    return false;

  bool logic = !KSUtil::checkType( context, args[0], KSValue::StringType, true );
  context.setValue( new KSValue(logic));
  return true;
}

static bool kspreadfunc_isnum( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISNUM", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::DoubleType, true );
  context.setValue( new KSValue(logic));
  return true;
}

static bool kspreadfunc_istime( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISTIME", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::TimeType, true );
  context.setValue( new KSValue(logic));
  return true;
}

static bool kspreadfunc_isdate( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISDATE", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::DateType, true );
  context.setValue( new KSValue(logic));
  return true;
}


static bool kspreadfunc_pow( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "pow",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  context.setValue( new KSValue( pow( args[0]->doubleValue(),args[1]->doubleValue() ) ) );

  return true;
}

static bool kspreadfunc_mod( KSContext& context )
{
  double result=0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "MOD",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if( (int)args[1]->doubleValue()!=0)
  {
        result=(int)args[0]->doubleValue() % (int)args[1]->doubleValue();
        context.setValue( new KSValue(  result  ) );
  }
  else
  {
        context.setValue( new KSValue( i18n("#DIV/0") ) );

  }
  return true;
}

static bool kspreadfunc_hours( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "hours",true ) )
    return false;
  if (!KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
	return false;
      QTime tmp=KGlobal::locale()->readTime(args[0]->stringValue());
      if(tmp.isValid())
	context.setValue(new KSValue(tmp.hour()));
      else
	context.setValue(new KSValue(i18n("Err")));
      return true;
    }
  else
    {
      QTime tmp=args[0]->timeValue();
      context.setValue(new KSValue(tmp.hour()));
    }
   return true;
}

static bool kspreadfunc_minutes( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "minutes",true ) )
    return false;
  if (!KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
	return false;
      QTime tmp=KGlobal::locale()->readTime(args[0]->stringValue());
      if(tmp.isValid())
	context.setValue(new KSValue(tmp.minute()));
      else
	context.setValue(new KSValue(i18n("Err")));
      return true;
    }
  else
    {
      QTime tmp=args[0]->timeValue();
      context.setValue(new KSValue(tmp.minute()));
    }
   return true;
}

static bool kspreadfunc_seconds( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "seconds",true ) )
    return false;
  if (!KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
	return false;
      QTime tmp=KGlobal::locale()->readTime(args[0]->stringValue());
      if(tmp.isValid())
	context.setValue(new KSValue(tmp.second()));
      else
	context.setValue(new KSValue(i18n("Err")));
      return true;
    }
  else
    {
      QTime tmp=args[0]->timeValue();
      context.setValue(new KSValue(tmp.second()));
    }
   return true;
}

static bool kspreadfunc_date( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "date",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  QDate _date;
  if( _date.setYMD(args[0]->intValue(), args[1]->intValue(), args[2]->intValue()) )
    context.setValue( new KSValue(KGlobal::locale()->formatDate(_date)));
  else
    context.setValue( new KSValue(QString(i18n("Err"))) );

  return true;
}

static bool kspreadfunc_day( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString tmp;
  if ( !KSUtil::checkArgumentsCount( context,1, "day",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if(KGlobal::locale()->weekDayName(args[0]->intValue()).isNull())
        tmp=i18n("Err");
  else
        tmp= KGlobal::locale()->weekDayName(args[0]->intValue());

  //context.setValue( new KSValue(KGlobal::locale()->weekDayName(args[0]->intValue())));
  context.setValue( new KSValue(tmp));
  return true;
}

static bool kspreadfunc_month( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString tmp;
  if ( !KSUtil::checkArgumentsCount( context,1, "month",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if(KGlobal::locale()->monthName(args[0]->intValue()).isNull())
        tmp=i18n("Err");
  else
        tmp=KGlobal::locale()->monthName(args[0]->intValue());

  context.setValue( new KSValue(tmp));
  //context.setValue( new KSValue(KGlobal::locale()->monthName(args[0]->intValue())));

  return true;
}

static bool kspreadfunc_time( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "time",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  context.setValue( new KSValue(KGlobal::locale()->formatTime(
    QTime(args[0]->intValue(),
          args[1]->intValue(),args[2]->intValue()),true )));

  return true;
}

static bool kspreadfunc_currentDate( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context,0, "currentDate",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatDate(QDate::currentDate())));

    return true;
}

static bool kspreadfunc_shortcurrentDate( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context,0, "shortcurrentDate",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatDate(QDate::currentDate(),true)));

    return true;
}


static bool kspreadfunc_currentTime( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context,0, "currentTime",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatTime(QTime::currentTime())));

    return true;
}

static bool kspreadfunc_currentDateTime( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context,0, "currentDateTime",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatDateTime(QDateTime::currentDateTime())));

    return true;
}

static bool kspreadfunc_dayOfYear( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "dayOfYear",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  context.setValue( new KSValue(QDate(args[0]->intValue(),
                                      args[1]->intValue(),args[2]->intValue()).dayOfYear() ));

  return true;
}

static double fact( double val, double end )
{
    /* fact =i*(i-1)*(i-2)*...*1 */
    if(val <0||end<0)
        return (-1);
    if(val==0)
        return(1);
    else if(val==end)
        return(1);
    /*val==end => you don't multiplie it */
    else
        return(val*fact((double)(val-1),end));

}

static bool kspreadfunc_fact( KSContext& context )
{
  double result;
  QString tmp;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "fact",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  result=fact((double)args[0]->intValue(),0);
  //In fact function val must be positive
  tmp=i18n("Err");
  if(result==-1)
        context.setValue( new KSValue(tmp));
  else
        context.setValue( new KSValue(result ));

  return true;
}

static bool kspreadfunc_arrang( KSContext& context )
{ /* arrang : fact(n)/(fact(n-m) */
  double result;
  QString tmp;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "PERMUT",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  tmp=i18n("Err");
  if((double)args[0]->intValue()<(double)args[1]->intValue())
          context.setValue( new KSValue(tmp ));

  else if((double)args[1]->intValue()<0)
          context.setValue( new KSValue(tmp ));

  else
        {
        result=fact((double)args[0]->intValue(),
        ((double)args[0]->intValue()-(double)args[1]->intValue()));
        //In fact function val must be positive

        if(result==-1)
                context.setValue( new KSValue(tmp));
        else
                context.setValue( new KSValue(result ));
        }
  return true;
}

static bool kspreadfunc_combin( KSContext& context )
{ /*combin : fact(n)/(fact(n-m)*fact(m)) */
  double result;
  QString tmp;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "COMBIN",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  tmp=i18n("Err");
  if((double)args[0]->intValue()<(double)args[1]->intValue())
          context.setValue( new KSValue(tmp ));

  else if((double)args[1]->intValue()<0)
          context.setValue( new KSValue(tmp ));

  else
        {
        result=(fact((double)args[0]->intValue(),
        ((double)args[0]->intValue()-(double)args[1]->intValue()))
        /fact((double)args[1]->intValue(),0));
        //In fact function val must be positive

        if(result==-1)
                context.setValue( new KSValue(tmp));
        else
                context.setValue( new KSValue(result ));
        }
  return true;
}

static bool kspreadfunc_bino( KSContext& context )
{
  double result=0;
  QString tmp;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "BINO",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  tmp=i18n("Err");
  if(args[0]->doubleValue()<args[1]->doubleValue())
    context.setValue( new KSValue(tmp ));

  else if(args[1]->doubleValue()<0)
    context.setValue( new KSValue(tmp ));

  // 0<proba<1
  else if((args[2]->doubleValue()<0)||(args[2]->doubleValue()>1))
    context.setValue( new KSValue(tmp ));
  else
  {
    result=(fact(args[0]->doubleValue(),
                 (args[0]->doubleValue()-args[1]->doubleValue()))
            /fact(args[1]->doubleValue(),0));
    //In fact function val must be positive

    if(result==-1)
      context.setValue( new KSValue(tmp));
    else
    {
      result=result*pow(args[2]->doubleValue(),(int)args[1]->doubleValue())*
        pow((1-args[2]->doubleValue()),((int)args[0]->doubleValue()-
                                        ((int)args[1]->doubleValue())));
      context.setValue( new KSValue(result ));
    }
  }
  return true;


}


static bool kspreadfunc_bino_inv( KSContext& context )
{
  double result=0;
  QString tmp;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "INVBINO",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  tmp=i18n("Err");
  if(args[0]->doubleValue()<args[1]->doubleValue())
          context.setValue( new KSValue(tmp ));

  else if(args[1]->doubleValue()<0)
          context.setValue( new KSValue(tmp ));

  // 0<proba<1
  else if((args[2]->doubleValue()<0)||(args[2]->doubleValue()>1))
    context.setValue( new KSValue(tmp ));
  else
  {
        result=(fact(args[0]->doubleValue(),
        (args[0]->doubleValue()-args[1]->doubleValue()))
        /fact(args[1]->doubleValue(),0));
        //In fact function val must be positive

        if(result==-1)
          context.setValue( new KSValue(tmp));
        else
        {
          result=result*pow((1-args[2]->doubleValue()),((int)args[0]->doubleValue()-
                                                        (int)args[1]->doubleValue()))*pow(args[2]->doubleValue(),(
                                                          (int)args[1]->doubleValue()));
          context.setValue( new KSValue(result ));
        }
  }
  return true;


}

static bool kspreadfunc_fv( KSContext& context )
{
/* Returns future value, given current value, interest rate and time */
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "FV", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  double present = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double periods = args[2]->doubleValue();


  context.setValue( new KSValue( present * pow(1+interest, periods)));
  return true;
}

static bool kspreadfunc_compound( KSContext& context )
{
/* Returns value after compounded interest, given principal, rate, periods
per year and year */
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "compound", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;
  double principal = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double periods = args[2]->doubleValue();
  double years = args[3]->doubleValue();

  context.setValue( new KSValue( principal * pow(1+(interest/periods),
periods*years)));

  return true;
}

static bool kspreadfunc_continuous( KSContext& context )
{
    // If you still don't understand this, let me know!  ;-)  jsinger@leeta.net

/* Returns value after continuous compounding of interest, given prinicpal,
rate and years */
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "continuous", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  double principal = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double years = args[2]->doubleValue();


  context.setValue( new KSValue( principal * exp(interest * years)));
  return true;
}

static bool kspreadfunc_pv( KSContext& context )
{
/* Returns presnt value, given future value, interest rate and years */
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "PV", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  double future = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double periods = args[2]->doubleValue();


  context.setValue( new KSValue( future / pow(1+interest, periods)));
  return true;
}

static bool kspreadfunc_pv_annuity( KSContext& context )
{
    /* Returns present value of an annuity or cash flow, given payment,
       interest rate,
       periods, initial amount and whether payments are made at the start (TRUE)
       or end of a period */

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "PV_annuity", true ) )
	return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
	return false;
    double amount = args[0]->doubleValue();
    double interest = args[1]->doubleValue();
    double periods = args[2]->doubleValue();

    double result;
    result = amount * (1 - 1/(pow( (1+interest), periods ))) / interest ;

  context.setValue( new KSValue( result ) );

  return true;
}


static bool kspreadfunc_fv_annuity( KSContext& context )
{
    /* Returns future value of an annuity or cash flow, given payment, interest
       rate and periods */

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "FV_annuity", true ) )
	return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
	return false;
    double amount= args[0]->doubleValue();
    double interest = args[1]->doubleValue();
    double periods = args[2]->doubleValue();

    double result;

    result = amount * ((pow( (1+interest),periods))/interest - 1/interest)   ;

    context.setValue( new KSValue( result ) );

    return true;
}


static bool kspreadfunc_effective( KSContext& context )
{
/* Returns effective interest rate given nominal rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "effective", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double nominal = args[0]->doubleValue();
  double periods = args[1]->doubleValue();

  context.setValue( new KSValue(  pow( 1 + (nominal/periods), periods )- 1 ) );

  return true;
}


static bool kspreadfunc_zero_coupon( KSContext& context )
{
/* Returns effective interest rate given nominal rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "zero_coupon", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  double face = args[0]->doubleValue();
  double rate = args[1]->doubleValue();
  double years = args[2]->doubleValue();

  context.setValue( new KSValue(  face / pow( (1 + rate), years )  ) );

  return true;
}


static bool kspreadfunc_level_coupon( KSContext& context )
{
/* Returns effective interest rate given nominal rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 5, "level_coupon", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
    return false;
  double face = args[0]->doubleValue();
  double coupon_rate = args[1]->doubleValue();
  double coupon_year = args[2]->doubleValue();
  double years = args[3]->doubleValue();
  double market_rate = args[4]->doubleValue();

  double coupon = coupon_rate * face / coupon_year;
  double interest =  market_rate/coupon_year;
  double pv_annuity = (1 - 1/(pow( (1+interest), (years*coupon_year) ))) / interest ;
  context.setValue( new KSValue( coupon * pv_annuity + (face/ pow( (1+interest), (years*coupon_year) ) ) ) );

  return true;
}


static bool kspreadfunc_nominal( KSContext& context )
{
/* Returns nominal interest rate given effective rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "nominal", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double effective = args[0]->doubleValue();
  double periods = args[1]->doubleValue();

  if ( periods == 0.0 ) // Check null
      return false;

  context.setValue( new KSValue( periods * (pow( (effective + 1), (1 / periods) ) -1) ) );

  return true;
}


static bool kspreadfunc_sign( KSContext& context )
{
  int value=0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sign", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
	return false;
    }
  val=args[0]->doubleValue();

  if(val>0)
    value=1;
  else if(val<0)
    value=-1;
  else if(val==0)
    value=0;

  context.setValue( new KSValue( value ) );

  return true;
}

static bool kspreadfunc_atan2( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "atan2", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  context.setValue( new KSValue( atan2( args[1]->doubleValue(),args[0]->doubleValue() ) ) );

  return true;
}

static bool kspreadfunc_inv( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "INV",true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
	return false;
    }
  val=args[0]->doubleValue();


  context.setValue( new KSValue( val*(-1) ) );

  return true;
}

static bool kspreadfunc_dec2hex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "DECHEX", true ) ||!KSUtil::checkArgumentsCount( context, 1, "DEC2HEX", true ))
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  QString tmp;
  tmp=tmp.setNum( args[0]->intValue(),16);
  context.setValue( new KSValue( tmp ));

  return true;
}

static bool kspreadfunc_dec2oct( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "DEC2OCT", true )  || !KSUtil::checkArgumentsCount( context, 1, "DECOCT", true ))
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  QString tmp;
  tmp=tmp.setNum( args[0]->intValue(),8);
  context.setValue( new KSValue( tmp ));

  return true;
}

static bool kspreadfunc_dec2bin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "DEC2BIN", true )  || !KSUtil::checkArgumentsCount( context, 1, "DECBIN", true ))
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  QString tmp;
  tmp=tmp.setNum( args[0]->intValue(),2);
  context.setValue( new KSValue( tmp ));

  return true;
}

static bool kspreadfunc_bin2dec( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "BIN2DEC", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,2);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        context.setValue( new KSValue(val));

  return true;
}

static bool kspreadfunc_bin2oct( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "BIN2OCT", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,2);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        {
        tmp=tmp.setNum(val,8);
        context.setValue( new KSValue(tmp));
        }

  return true;
}

static bool kspreadfunc_bin2hex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "BIN2HEX", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,2);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        {
        tmp=tmp.setNum(val,16);
        context.setValue( new KSValue(tmp));
        }

  return true;
}

static bool kspreadfunc_oct2dec( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "OCT2DEC", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,8);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        context.setValue( new KSValue(val));

  return true;
}

static bool kspreadfunc_oct2bin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "OCT2BIN", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,8);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        {
        tmp=tmp.setNum(val,2);
        context.setValue( new KSValue(tmp));
        }

  return true;
}

static bool kspreadfunc_oct2hex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "OCT2HEX", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,8);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        {
        tmp=tmp.setNum(val,16);
        context.setValue( new KSValue(tmp));
        }

  return true;
}

static bool kspreadfunc_hex2dec( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "HEX2DEC", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,16);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        context.setValue( new KSValue(val));

  return true;
}

static bool kspreadfunc_hex2bin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "HEX2BIN", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,16);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        {
        tmp=tmp.setNum(val,2);
        context.setValue( new KSValue(tmp));
        }

  return true;
}

static bool kspreadfunc_hex2oct( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "HEX2OCT", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString tmp=args[0]->stringValue();
  bool ok;
  long val=tmp.toLong(&ok,16);
  if(!ok)
        context.setValue( new KSValue( QString(i18n("Err") )));
  else
        {
        tmp=tmp.setNum(val,8);
        context.setValue( new KSValue(tmp));
        }

  return true;
}


static bool kspreadfunc_rounddown( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  double result=0;
  int digits=0;
  if ( !KSUtil::checkArgumentsCount( context, 2, "ROUNDDOWN", true ) )
        {
        //just 1 argument => number of decimal =0 by default
        if ( !KSUtil::checkArgumentsCount( context, 1, "ROUNDDOWN", true ) )
                return false;
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
            return false;
        digits=0;
        }
  else
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
                return false;
        digits=args[1]->intValue();
        }
  result=floor(args[0]->doubleValue()*pow(10.0,digits))/pow(10.0,digits);
  context.setValue( new KSValue( result) );

  return true;
}

static bool kspreadfunc_roundup( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  double result=0;
  int digits=0;
  if ( !KSUtil::checkArgumentsCount( context, 2, "ROUNDUP", true ) )
        {
        //just 1 argument => number of decimal =0 by default
        if ( !KSUtil::checkArgumentsCount( context, 1, "ROUNDUP", true ) )
                return false;
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
            return false;
        digits=0;
        }
  else
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
                return false;
        digits=args[1]->intValue();
        }
  // This is not correct solution for problem with floating point numbers and probably
  // will fail in platforms where float and double lenghts are same.
  if (approx_equal(floor(args[0]->doubleValue()*pow(10,digits)), args[0]->doubleValue()*pow(10,digits)))
      result = args[0]->doubleValue();
  else
      result=floor(args[0]->doubleValue()*pow(10,digits)+1)/pow(10,digits);
  context.setValue( new KSValue( result) );

  return true;
}

static bool kspreadfunc_round( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  double result=0;
  int digits=0;
  if ( !KSUtil::checkArgumentsCount( context, 2, "ROUND", true ) )
        {
        //just 1 argument => number of decimal =0 by default
        if ( !KSUtil::checkArgumentsCount( context, 1, "ROUND", true ) )
                return false;
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
            return false;
        digits=0;
        }
  else
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
                return false;
        digits=args[1]->intValue();
        }
  result=floor(args[0]->doubleValue()*pow(10.0,digits)+0.5)/pow(10.0,digits);
  context.setValue( new KSValue( result) );

  return true;
}

static QString kspreadfunc_create_complex( double real,double imag )
{
  QString tmp,tmp2;
  if(imag ==0)
        {
        return KGlobal::locale()->formatNumber( real);
        }
  if(real!=0)
        tmp=KGlobal::locale()->formatNumber(real);
  else
	return KGlobal::locale()->formatNumber(imag)+"i";
  if (imag >0)
        tmp=tmp+"+"+KGlobal::locale()->formatNumber(imag)+"i";
  else
        tmp=tmp+KGlobal::locale()->formatNumber(imag)+"i";
  return tmp;

}

static bool kspreadfunc_complex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "COMPLEX",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if(args[1]->doubleValue() ==0)
        {
        context.setValue( new KSValue(args[0]->doubleValue()));
        return true;
        }
  QString tmp=kspreadfunc_create_complex(args[0]->doubleValue(),args[1]->doubleValue());
  bool ok;
  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));

  return true;
}


static double imag_complexe(QString str, bool &ok)
{
QString tmp=str;
if(tmp.find('i')==-1)
        {  //not a complex
        ok=true;
        return 0;
        }
else if( tmp.length()==1)
        {
        // i
        ok=true;
        return 1;
        }
else  if( tmp.length()==2 )
        {
        //-i,+i,
        int pos1;
        if((pos1=tmp.find('+'))!=-1&& pos1==0)
                {
                ok=true;
                return 1;
                }
        else if( (pos1=tmp.find('-'))!=-1 && pos1==0 )
                {
                ok=true;
                return -1;
                }
        else if(tmp[0].isDigit())
                { //5i
                ok=true;
                return KGlobal::locale()->readNumber(tmp.left(1));
                }
        else
                {
                ok=false;
                return 0;
                }
        }
else
        {//12+12i
        int pos1,pos2;
        if((pos1=tmp.find('i'))!=-1)
                {
                double val;
                QString tmpStr;

                if((pos2=tmp.findRev('+'))!=-1 && pos2!=0)
                        {
                        if((pos1-pos2)==1)
                                {
                                 ok=true;
                                 return 1;
                                }
                        else
                                {
                                tmpStr=tmp.mid(pos2,(pos1-pos2));
                                val=KGlobal::locale()->readNumber(tmpStr, &ok);
                                if(!ok)
                                        val=0;
                                return val;
                                }
                        }
                else if( (pos2=tmp.findRev('-'))!=-1&& pos2!=0)
                        {
                        if((pos1-pos2)==1)
                                {
                                 ok=true;
                                 return -1;
                                }
                        else
                                {
                                tmpStr=tmp.mid(pos2,(pos1-pos2));
                                val=KGlobal::locale()->readNumber(tmpStr, &ok);
                                if(!ok)
                                        val=0;
                                return val;
                                }
                        }
                else
                        {//15.55i
                        tmpStr=tmp.left(pos1);
                        val=KGlobal::locale()->readNumber(tmpStr, &ok);
                        if(!ok)
                                val=0;
                        return val;
                        }
                }
        }
ok=false;
return 0;
}

static bool kspreadfunc_complex_imag( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMAGINARY",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool good;
  double result=imag_complexe(tmp, good);
  if(good)
        context.setValue( new KSValue(result));
  else
        context.setValue( new KSValue(i18n("Err")));

  return true;
}


static double real_complexe(QString str, bool &ok)
{
double val;
int pos1,pos2;
QString tmp=str;
QString tmpStr;
if((pos1=tmp.find('i'))==-1)
        { //12.5
        val=KGlobal::locale()->readNumber(tmp, &ok);
        if(!ok)
                val=0;
        return val;
        }
else
        { //15-xi
        if((pos2=tmp.findRev('-'))!=-1 && pos2!=0)
                {
                tmpStr=tmp.left(pos2);
                val=KGlobal::locale()->readNumber(tmpStr, &ok);
                if(!ok)
                        val=0;
                return val;
                } //15+xi
        else if((pos2=tmp.findRev('+'))!=-1)
                {
                tmpStr=tmp.left(pos2);
                val=KGlobal::locale()->readNumber(tmpStr, &ok);
                if(!ok)
                        val=0;
                return val;
                }
        else
                {
                ok=true;
                return 0;
                }
        }

ok=false;
return 0;
}

static bool kspreadfunc_complex_real( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMREAL",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        tmp=args[0]->stringValue();
  bool good;
  double result=real_complexe(tmp, good);
  if(good)
        context.setValue( new KSValue(result));
  else
        context.setValue( new KSValue(i18n("Err")));

  return true;
}


static bool kspreadfunc_imsum_helper( KSContext& context, QValueList<KSValue::Ptr>& args, QString& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_imsum_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      imag=imag_complexe(result, ok);
      real=real_complexe(result,  ok);
      imag1=imag_complexe((*it)->stringValue(), ok);
      real1=real_complexe((*it)->stringValue(), ok);
      result=kspreadfunc_create_complex(real+real1,imag+imag1);
      }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      imag=imag_complexe(result, ok);
      real=real_complexe(result,  ok);
      imag1=0;
      real1=(*it)->doubleValue();
      result=kspreadfunc_create_complex(real+real1,imag+imag1);
      }
    else
      return false;
  }

  return true;
}

static bool kspreadfunc_imsum( KSContext& context )
{
  QString result ;
  bool b = kspreadfunc_imsum_helper( context, context.value()->listValue(), result );
  bool ok;
  QString tmp;
  double val=KGlobal::locale()->readNumber(result, &ok);
  if(ok&&b)
        context.setValue( new KSValue( val ) );
  else if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_imsub_helper( KSContext& context, QValueList<KSValue::Ptr>& args, QString& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_imsub_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      if(!result.isEmpty())
        {
        imag=imag_complexe(result, ok);
        real=real_complexe(result,  ok);
        imag1=imag_complexe((*it)->stringValue(), ok);
        real1=real_complexe((*it)->stringValue(), ok);
        result=kspreadfunc_create_complex(real-real1,imag-imag1);
        }
      else
        {
        imag1=imag_complexe((*it)->stringValue(), ok);
        real1=real_complexe((*it)->stringValue(), ok);
        result=kspreadfunc_create_complex(real1,imag1);
        }
      }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      imag=imag_complexe(result, ok);
      real=real_complexe(result,  ok);
      imag1=0;
      real1=(*it)->doubleValue();
      if(!result.isEmpty())
        result=kspreadfunc_create_complex(real-real1,imag-imag1);
      else
        result=kspreadfunc_create_complex(real1,imag1);
      }
    else
      return false;
  }

  return true;
}

static bool kspreadfunc_imsub( KSContext& context )
{
  QString result ;
  bool b = kspreadfunc_imsub_helper( context, context.value()->listValue(), result );
  bool ok;
  QString tmp;
  double val=KGlobal::locale()->readNumber(result, &ok);
  if(ok&&b)
        context.setValue( new KSValue( val ) );
  else if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}


static bool kspreadfunc_improduct_helper( KSContext& context, QValueList<KSValue::Ptr>& args, QString& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_improduct_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      if(!result.isEmpty())
        {
        imag=imag_complexe(result, ok);
        real=real_complexe(result,  ok);
        imag1=imag_complexe((*it)->stringValue(), ok);
        real1=real_complexe((*it)->stringValue(), ok);
        result=kspreadfunc_create_complex(real*real1+(imag*imag1)*-1,real*imag1+real1*imag);
        }
      else
        {
        imag1=imag_complexe((*it)->stringValue(), ok);
        real1=real_complexe((*it)->stringValue(), ok);
        result=kspreadfunc_create_complex(real1,imag1);
        }
      }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      imag=imag_complexe(result, ok);
      real=real_complexe(result,  ok);
      imag1=0;
      real1=(*it)->doubleValue();
      if(!result.isEmpty())
        result=kspreadfunc_create_complex(real*real1+(imag*imag1)*-1,real*imag1+real1*imag);
      else
        result=kspreadfunc_create_complex(real1,imag1);
      }
    else
      return false;
  }

  return true;
}

static bool kspreadfunc_improduct( KSContext& context )
{
  QString result ;
  bool b = kspreadfunc_improduct_helper( context, context.value()->listValue(), result );
  bool ok;
  QString tmp;
  double val=KGlobal::locale()->readNumber(result, &ok);
  if(ok&&b)
        context.setValue( new KSValue( val ) );
  else if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}


static bool kspreadfunc_imconjugate( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMCONJUGATE",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  tmp=kspreadfunc_create_complex(real,-imag);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));

  return true;
}

static bool kspreadfunc_imargument( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMARGUMENT",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  if(imag==0)
        {
        context.setValue( new KSValue(i18n("#Div/0")));
        return true;
        }
  double arg=atan2(imag,real);

  context.setValue( new KSValue(arg));

  return true;
}

static bool kspreadfunc_imabs( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMABS",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double arg=sqrt(pow(imag,2)+pow(real,2));

  context.setValue( new KSValue(arg));

  return true;
}

static bool kspreadfunc_imcos( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMCOS",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag_res=sin(real)*sinh(imag);
  double real_res=cos(real)*cosh(imag);


  tmp=kspreadfunc_create_complex(real_res,-imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));

  return true;
}

static bool kspreadfunc_imsin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMSIN",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag_res=cos(real)*sinh(imag);
  double real_res=sin(real)*cosh(imag);


  tmp=kspreadfunc_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));

  return true;
}

static bool kspreadfunc_imln( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMLN",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }


  double arg=sqrt(pow(imag,2)+pow(real,2));
  double real_res=log(arg);
  double imag_res=atan(imag/real);
  tmp=kspreadfunc_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));
  return true;
}

static bool kspreadfunc_imexp( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMEXP",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag_res=exp(real)*sin(imag);
  double real_res=exp(real)*cos(imag);


  tmp=kspreadfunc_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));

  return true;
}

static bool kspreadfunc_imsqrt( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "IMSQRT",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double arg=sqrt(sqrt(pow(imag,2)+pow(real,2)));
  double angle=atan(imag/real);

  double real_res=arg*cos((angle/2));
  double imag_res=arg*sin((angle/2));

  tmp=kspreadfunc_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));

  return true;
}

static bool kspreadfunc_impower( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "IMPOWER",true ) )
    return false;
  QString tmp;
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        tmp=KGlobal::locale()->formatNumber(args[0]->doubleValue());
        }
  else
        {
        tmp=args[0]->stringValue();
        }
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
        return false;

  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }
  double imag=imag_complexe(tmp,ok);
  if(!ok)
        {
        context.setValue( new KSValue(i18n("Err")));
        return false;
        }

  double arg=pow(sqrt(pow(imag,2)+pow(real,2)),args[1]->intValue());
  double angle=atan(imag/real);

  double real_res=arg*cos(angle*args[1]->intValue());
  double imag_res=arg*sin(angle*args[1]->intValue());

  tmp=kspreadfunc_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
        {
        context.setValue( new KSValue(result));
        return true;
        }
  context.setValue( new KSValue(tmp));

  return true;
}


static bool kspreadfunc_imdiv_helper( KSContext& context, QValueList<KSValue::Ptr>& args, QString& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_imdiv_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      if(!result.isEmpty())
        {
        imag=imag_complexe(result, ok);
        real=real_complexe(result,  ok);
        imag1=imag_complexe((*it)->stringValue(), ok);
        real1=real_complexe((*it)->stringValue(), ok);
        result=kspreadfunc_create_complex((real*real1+imag*imag1)/(real1*real1+imag1*imag1),(real1*imag-real*imag1)/(real1*real1+imag1*imag1));
        }
      else
        {
        imag1=imag_complexe((*it)->stringValue(), ok);
        real1=real_complexe((*it)->stringValue(), ok);
        result=kspreadfunc_create_complex(real1,imag1);
        }
      }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      double imag,real,imag1,real1;
      bool ok;
      imag=imag_complexe(result, ok);
      real=real_complexe(result,  ok);
      imag1=0;
      real1=(*it)->doubleValue();
      if(!result.isEmpty())
        result=kspreadfunc_create_complex((real*real1+imag*imag1)/(real1*real1+imag1*imag1),(real1*imag-real*imag1)/(real1*real1+imag1*imag1));
      else
        result=kspreadfunc_create_complex(real1,imag1);
      }
    else
      return false;
  }

  return true;
}

static bool kspreadfunc_imdiv( KSContext& context )
{
  QString result ;
  bool b = kspreadfunc_imdiv_helper( context, context.value()->listValue(), result );
  bool ok;
  QString tmp;
  double val=KGlobal::locale()->readNumber(result, &ok);
  if(ok&&b)
        context.setValue( new KSValue( val ) );
  else if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}



static bool kspreadfunc_polr( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "POLR",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double result=sqrt(pow(args[0]->doubleValue(),2)+pow(args[1]->doubleValue(),2));
  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_pola( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "POLA",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double result=acos(args[0]->doubleValue()/(sqrt(pow(args[0]->doubleValue(),2)+pow(args[1]->doubleValue(),2))));
  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_carx( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "CARX",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double result=args[0]->doubleValue()*cos(args[1]->doubleValue());
  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_cary( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "CARY",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double result=args[0]->doubleValue()*sin(args[1]->doubleValue());
  context.setValue( new KSValue(result));

  return true;
}


static bool kspreadfunc_sumproduct_helper( KSContext& context, QValueList<KSValue::Ptr>& list,QValueList<KSValue::Ptr>& list2, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = list.begin();
  QValueList<KSValue::Ptr>::Iterator end = list.end();
  QValueList<KSValue::Ptr>::Iterator it2 = list2.begin();
  QValueList<KSValue::Ptr>::Iterator end2 = list2.end();

  for( ; it != end,it2!=end2; ++it,++it2 )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sumproduct_helper( context, (*it)->listValue(),(*it2)->listValue(), result ))
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) && KSUtil::checkType( context, *it2, KSValue::DoubleType, true ))
      {
      result +=( (*it)->doubleValue()*(*it2)->doubleValue());
      }
    else if (!( KSUtil::checkType( context, *it, KSValue::Empty, true ) || KSUtil::checkType( context, *it2, KSValue::Empty, true )))
      return false;
  }

  return true;
}

static bool kspreadfunc_sumproduct( KSContext& context )
{
  double result = 0.0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context, 2, "SUMPRODUCT", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::ListType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::ListType, true ) )
      return false;
    if(args[0]->listValue().count() !=args[1]->listValue() .count())
        {
        context.setValue( new KSValue( i18n("Err") ) );
        return true;
        }
  bool b = kspreadfunc_sumproduct_helper( context,args[0]->listValue(),args[1]->listValue() , result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_sumx2py2_helper( KSContext& context, QValueList<KSValue::Ptr>& list,QValueList<KSValue::Ptr>& list2, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = list.begin();
  QValueList<KSValue::Ptr>::Iterator end = list.end();
  QValueList<KSValue::Ptr>::Iterator it2 = list2.begin();
  QValueList<KSValue::Ptr>::Iterator end2 = list2.end();

  for( ; it != end,it2!=end2; ++it,++it2 )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sumx2py2_helper( context, (*it)->listValue(),(*it2)->listValue(), result ))
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) && KSUtil::checkType( context, *it2, KSValue::DoubleType, true ))
      {
      result +=( pow((*it)->doubleValue(),2)+pow((*it2)->doubleValue(),2));
      }
    else if(!(KSUtil::checkType( context, *it, KSValue::Empty, true ) || KSUtil::checkType( context, *it2, KSValue::Empty, true )))
      return false;
  }

  return true;
}

static bool kspreadfunc_sumx2py2( KSContext& context )
{
  double result = 0.0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context, 2, "SUMX2PY2", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::ListType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::ListType, true ) )
      return false;
    if(args[0]->listValue().count() !=args[1]->listValue() .count())
        {
        context.setValue( new KSValue( i18n("Err") ) );
        return true;
        }
  bool b = kspreadfunc_sumx2py2_helper( context,args[0]->listValue(),args[1]->listValue() , result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}


static bool kspreadfunc_sumx2my2_helper( KSContext& context, QValueList<KSValue::Ptr>& list,QValueList<KSValue::Ptr>& list2, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = list.begin();
  QValueList<KSValue::Ptr>::Iterator end = list.end();
  QValueList<KSValue::Ptr>::Iterator it2 = list2.begin();
  QValueList<KSValue::Ptr>::Iterator end2 = list2.end();

  for( ; it != end,it2!=end2; ++it,++it2 )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sumx2my2_helper( context, (*it)->listValue(),(*it2)->listValue(), result ))
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) && KSUtil::checkType( context, *it2, KSValue::DoubleType, true ))
      {
      result +=( pow((*it)->doubleValue(),2)-pow((*it2)->doubleValue(),2));
      }
    else if(!(KSUtil::checkType( context, *it, KSValue::Empty, true ) || KSUtil::checkType( context, *it2, KSValue::Empty, true )))
      return false;
  }

  return true;
}

static bool kspreadfunc_sumx2my2( KSContext& context )
{
  double result = 0.0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context, 2, "SUMX2MY2", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::ListType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::ListType, true ) )
      return false;
    if(args[0]->listValue().count() !=args[1]->listValue() .count())
        {
        context.setValue( new KSValue( i18n("Err") ) );
        return true;
        }
  bool b = kspreadfunc_sumx2my2_helper( context,args[0]->listValue(),args[1]->listValue() , result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_sumxmy2_helper( KSContext& context, QValueList<KSValue::Ptr>& list,QValueList<KSValue::Ptr>& list2, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = list.begin();
  QValueList<KSValue::Ptr>::Iterator end = list.end();
  QValueList<KSValue::Ptr>::Iterator it2 = list2.begin();
  QValueList<KSValue::Ptr>::Iterator end2 = list2.end();

  for( ; it != end,it2!=end2; ++it,++it2 )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sumxmy2_helper( context, (*it)->listValue(),(*it2)->listValue(), result ))
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) && KSUtil::checkType( context, *it2, KSValue::DoubleType, true ))
      {
      result +=pow(( (*it)->doubleValue()-(*it2)->doubleValue()),2);
      }
    else if(!(KSUtil::checkType( context, *it, KSValue::Empty, true ) || KSUtil::checkType( context, *it2, KSValue::Empty, true )))
      return false;
  }

  return true;
}

static bool kspreadfunc_sumxmy2( KSContext& context )
{
  double result = 0.0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context, 2, "SUM2XMY", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::ListType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::ListType, true ) )
      return false;
    if(args[0]->listValue().count() !=args[1]->listValue() .count())
        {
        context.setValue( new KSValue( i18n("Err") ) );
        return true;
        }
  bool b = kspreadfunc_sumxmy2_helper( context,args[0]->listValue(),args[1]->listValue() , result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}


static bool kspreadfunc_delta( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  short result;
  if ( !KSUtil::checkArgumentsCount( context,2, "DELTA",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if(approx_equal(args[0]->doubleValue(), args[1]->doubleValue()))
        result=1;
  else
        result=0;
  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_even( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "EVEN",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  double result;
  double val=args[0]->doubleValue();
  int sign=1;
  if(val<0)
        {
        sign=-1;
        val=-val;
        }
  if (approx_equal(val, floor(val)))
    val = floor(val);
 double valsup=ceil( val );
 if(fmod(valsup,2.0)==0)
        {
        if(val>valsup)
                result=(int)(sign*(valsup+2));
        else
                result=(int)(sign*valsup);
        }
 else
        {
        result=(int)(sign*(valsup+1));
        }
  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_odd( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "ODD",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  double result;
  double valsup;
  int sign=1;
  double val= args[0]->doubleValue();
  if (val < 0)
        {
        sign = -1;
        val = -val;
        }
  if (approx_equal(val, floor(val)))
    val = floor(val);
  valsup = ceil(val);
  if (fmod(valsup, 2.0) == 1)
        {
        if (val > valsup)
                result=(int) (sign * (valsup + 2));
        else
                result=(int) (sign * valsup);
        }
  else
        result=(int) (sign * (valsup + 1));

 context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_isodd( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "ISODD",true ) )
    return false;
  bool result=true;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        result= false;
  if(result)
        {
        //it's a integer => test if it's an odd integer
        if(args[0]->intValue() & 1)
                result=true;
        else
                result=false;
        }

 context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_iseven( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "ISEVEN",true ) )
    return false;
  bool result=true;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        result= false;
  if(result)
        {
        //it's a integer => test if it's an even integer
        if(args[0]->intValue() & 1)
                result=false;
        else
                result=true;
        }

 context.setValue( new KSValue(result));

  return true;
}

static bool isLeapYear_helper(int _year)
{
    return (((_year % 4) == 0) && ((_year % 100) != 0) || ((_year % 400) == 0));
}

static bool kspreadfunc_isLeapYear ( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1,"isLeapYear",true ) )
    return false;
  bool result=true;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if(result)
  {
    int nYear = args[0]->intValue();
    result = isLeapYear_helper(nYear);
  }

  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_daysInYear ( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1,"daysInYear",true ) )
    return false;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  int nYear = args[0]->intValue();
  bool leap = isLeapYear_helper(nYear);
  int result;

  if (leap)
    result = 366;
  else
    result = 365;

  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_weeksInYear( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1,"weeksInYear",true ) )
    return false;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  int nYear = args[0]->intValue();
  int result;
  QDate _date(nYear, 1, 1);
  int nJan1DayOfWeek = _date.dayOfWeek();   //first day of the year

  if ( nJan1DayOfWeek == 4 ) { // Thursday
        result = 53;
  } else if ( nJan1DayOfWeek == 3 ) { // Wednesday
        result = isLeapYear_helper(nYear) ? 53 : 52 ;
  } else {
        result = 52;
  }

  context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_daysInMonth( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,2,"daysInMonth",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
  {
    context.setValue( new KSValue( i18n("Err") ) );
    return true;
  }
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
  {
    context.setValue( new KSValue( i18n("Err") ) );
    return true;
  }

  static uint aDaysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  int nYear = args[0]->intValue();
  int nMonth = args[1]->intValue();
  int result;

  if ( nMonth != 2)
    result = aDaysInMonth[nMonth-1];
  else
  {
    if (isLeapYear_helper(nYear))
        result = aDaysInMonth[nMonth-1] + 1;
    else
        result = aDaysInMonth[nMonth-1];
  }

  context.setValue( new KSValue(result));

  return true;
}


static bool kspreadfunc_count_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
      {

	if ( !kspreadfunc_count_helper( context, (*it)->listValue(), result ) )
	  return false;
      }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
	result++;
      }
  }

  return true;
}

static bool kspreadfunc_count( KSContext& context )
{
  double result = 0.0;

  bool b = kspreadfunc_count_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}


static bool kspreadfunc_decsex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "DECSEX",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  int inter;
  double val=args[0]->doubleValue();
  int hours,minutes,second;
  if(val>0)
    inter=1;
  else
    inter=-1;
  hours=inter*(int)(fabs(val));
  minutes=(int)(60*val-60*(int)(val));
  second=(int)(3600*val-3600*(int)(val)-60*(int)(60*val-60*(int)(val)));
  QTime _time(hours,minutes,second);
  context.setValue( new KSValue(_time));

  return true;
}

static bool kspreadfunc_sexdec( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  double result;
  if ( !KSUtil::checkArgumentsCount( context,3, "SEXDEC",true ) )
    {
      if ( !KSUtil::checkArgumentsCount( context,1, "SEXDEC",true ) )
	return false;
      if ( !KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
	return false;

      result=args[0]->timeValue().hour()+(double)args[0]->timeValue().minute()/60.0+(double)args[0]->timeValue().second()/3600.0;

      context.setValue( new KSValue(result));
    }
  else
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
	return false;
      if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
	return false;
      if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
	return false;
      result=args[0]->intValue()+(double)args[1]->intValue()/60.0+(double)args[2]->intValue()/3600.0;

      context.setValue( new KSValue(result));
    }

  return true;
}

static bool kspreadfunc_roman( KSContext& context )
{
    const QCString RNUnits[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
    const QCString RNTens[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
    const QCString RNHundreds[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
    const QCString RNThousands[] = {"", "M", "MM", "MMM"};


    QValueList<KSValue::Ptr>& args = context.value()->listValue();
    if ( !KSUtil::checkArgumentsCount( context,1, "ROMAN",true ) )
        return false;
    int value;
    if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
            return false;
        else
            value=(int)args[0]->doubleValue();
    }
    else
    	value=(int)args[0]->intValue();
    if(value<0)
    {
        context.setValue( new KSValue(i18n("Err")));
        return true;
    }
    if(value>3999)
    {
        context.setValue( new KSValue(i18n("Value too big")));
        return true;
    }
    QString result;

    result= QString::fromLatin1( RNThousands[ ( value / 1000 ) ] +
                                 RNHundreds[ ( value / 100 ) % 10 ] +
                                 RNTens[ ( value / 10 ) % 10 ] +
                                 RNUnits[ ( value ) % 10 ] );
    context.setValue( new KSValue(result));
    return true;
}

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

    kdDebug(36002) <<"x= "<<x<<endl;
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

  module->addObject( "cos", new KSValue( new KSBuiltinFunction( module, "cos", kspreadfunc_cos ) ) );
  module->addObject( "sin", new KSValue( new KSBuiltinFunction( module, "sin", kspreadfunc_sin ) ) );
  module->addObject( "sum", new KSValue( new KSBuiltinFunction( module, "sum", kspreadfunc_sum ) ) );
  module->addObject( "sumsq", new KSValue( new KSBuiltinFunction( module, "sumsq", kspreadfunc_sumsq ) ) );
  module->addObject( "sqrt", new KSValue( new KSBuiltinFunction( module, "sqrt", kspreadfunc_sqrt ) ) );
  module->addObject( "fabs", new KSValue( new KSBuiltinFunction( module, "fabs", kspreadfunc_fabs ) ) );
  module->addObject( "floor", new KSValue( new KSBuiltinFunction( module, "floor", kspreadfunc_floor ) ) );
  module->addObject( "ceil", new KSValue( new KSBuiltinFunction( module, "ceil", kspreadfunc_ceil ) ) );
  module->addObject( "tan", new KSValue( new KSBuiltinFunction( module, "tan", kspreadfunc_tan ) ) );
  module->addObject( "exp", new KSValue( new KSBuiltinFunction( module, "exp", kspreadfunc_exp ) ) );
  module->addObject( "ln", new KSValue( new KSBuiltinFunction( module, "ln", kspreadfunc_ln ) ) );
  module->addObject( "atan", new KSValue( new KSBuiltinFunction( module, "atan", kspreadfunc_atan ) ) );
  module->addObject( "asin", new KSValue( new KSBuiltinFunction( module, "asin", kspreadfunc_asin ) ) );
  module->addObject( "acos", new KSValue( new KSBuiltinFunction( module, "acos", kspreadfunc_acos ) ) );
  module->addObject( "log", new KSValue( new KSBuiltinFunction( module, "log", kspreadfunc_log ) ) );
  module->addObject( "max", new KSValue( new KSBuiltinFunction( module, "max", kspreadfunc_max ) ) );
  module->addObject( "min", new KSValue( new KSBuiltinFunction( module, "min", kspreadfunc_min ) ) );
  module->addObject( "cosh", new KSValue( new KSBuiltinFunction( module, "cosh", kspreadfunc_cosh ) ) );
  module->addObject( "sinh", new KSValue( new KSBuiltinFunction( module, "sinh", kspreadfunc_sinh ) ) );
  module->addObject( "tanh", new KSValue( new KSBuiltinFunction( module, "tanh", kspreadfunc_tanh ) ) );
  module->addObject( "acosh", new KSValue( new KSBuiltinFunction( module, "acosh", kspreadfunc_acosh ) ) );
  module->addObject( "asinh", new KSValue( new KSBuiltinFunction( module, "asinh", kspreadfunc_asinh ) ) );
  module->addObject( "atanh", new KSValue( new KSBuiltinFunction( module, "atanh", kspreadfunc_atanh ) ) );
  module->addObject( "degree", new KSValue( new KSBuiltinFunction( module, "degree", kspreadfunc_degree ) ) );
  module->addObject( "radian", new KSValue( new KSBuiltinFunction( module, "radian", kspreadfunc_radian ) ) );
  module->addObject( "average", new KSValue( new KSBuiltinFunction( module, "average", kspreadfunc_average ) ) );
  module->addObject( "variance", new KSValue( new KSBuiltinFunction( module, "variance", kspreadfunc_variance) ) );
  module->addObject( "multiply", new KSValue( new KSBuiltinFunction( module, "multiply", kspreadfunc_mult) ) );
  module->addObject( "OR", new KSValue( new KSBuiltinFunction( module, "OR", kspreadfunc_or) ) );
  module->addObject( "AND", new KSValue( new KSBuiltinFunction( module, "AND", kspreadfunc_and) ) );
  module->addObject( "NOR", new KSValue( new KSBuiltinFunction( module, "NOR", kspreadfunc_nor) ) );
  module->addObject( "NAND", new KSValue( new KSBuiltinFunction( module, "NAND", kspreadfunc_nand) ) );

  module->addObject( "stddev", new KSValue( new KSBuiltinFunction( module, "stderr", kspreadfunc_stddev) ) );
  module->addObject( "join", new KSValue( new KSBuiltinFunction( module, "join", kspreadfunc_join) ) );
  //compatibility with kspread1.0
  module->addObject( "not", new KSValue( new KSBuiltinFunction( module, "not", kspreadfunc_not) ) );
  module->addObject( "NOT", new KSValue( new KSBuiltinFunction( module, "NOT", kspreadfunc_not) ) );
  //compatibility with kspread1.0
  module->addObject( "if", new KSValue( new KSBuiltinFunction( module, "if", kspreadfunc_if) ) );
  module->addObject( "IF", new KSValue( new KSBuiltinFunction( module, "IF", kspreadfunc_if) ) );
  module->addObject( "left", new KSValue( new KSBuiltinFunction( module, "left", kspreadfunc_left) ) );
  module->addObject( "right", new KSValue( new KSBuiltinFunction( module, "right", kspreadfunc_right) ) );
  module->addObject( "mid", new KSValue( new KSBuiltinFunction( module, "mid", kspreadfunc_mid) ) );
  module->addObject( "len", new KSValue( new KSBuiltinFunction( module, "len", kspreadfunc_len) ) );
  module->addObject( "EXACT", new KSValue( new KSBuiltinFunction( module, "EXACT", kspreadfunc_EXACT) ) );
  module->addObject( "INT", new KSValue( new KSBuiltinFunction( module, "INT",kspreadfunc_INT) ) );
  //compatibility with kspread1.0
  module->addObject( "ENT", new KSValue( new KSBuiltinFunction( module, "ENT",kspreadfunc_INT) ) );
  module->addObject( "PI", new KSValue( new KSBuiltinFunction( module, "PI",kspreadfunc_PI) ) );
  module->addObject( "rand", new KSValue( new KSBuiltinFunction( module, "rand",kspreadfunc_rand) ) );
  module->addObject( "REPT", new KSValue( new KSBuiltinFunction( module, "REPT",kspreadfunc_REPT) ) );

  module->addObject( "ISLOGIC", new KSValue( new KSBuiltinFunction( module,"ISLOGIC",kspreadfunc_islogic) ) );
  module->addObject( "ISTEXT", new KSValue( new KSBuiltinFunction( module,"ISTEXT",kspreadfunc_istext) ) );
  module->addObject( "ISNUM", new KSValue( new KSBuiltinFunction( module,"ISNUM",kspreadfunc_isnum) ) );
  module->addObject( "ISNOTTEXT", new KSValue( new KSBuiltinFunction( module, "ISNOTTEXT", kspreadfunc_isnottext ) ) );
  module->addObject( "ISODD", new KSValue( new KSBuiltinFunction( module, "ISODD", kspreadfunc_isodd ) ) );
  module->addObject( "ISEVEN", new KSValue( new KSBuiltinFunction( module, "ISEVEN", kspreadfunc_iseven ) ) );

  module->addObject( "cell", new KSValue( new KSBuiltinFunction( module,"cell",kspreadfunc_cell) ) );
  module->addObject( "select", new KSValue( new KSBuiltinFunction( module,"select",kspreadfunc_select) ) );
  module->addObject( "pow", new KSValue( new KSBuiltinFunction( module,"pow",kspreadfunc_pow) ) );
  module->addObject( "MOD", new KSValue( new KSBuiltinFunction( module,"MOD",kspreadfunc_mod) ) );
  module->addObject( "date", new KSValue( new KSBuiltinFunction( module,"date",kspreadfunc_date) ) );
  module->addObject( "day", new KSValue( new KSBuiltinFunction( module,"day",kspreadfunc_day) ) );
  module->addObject( "month", new KSValue( new KSBuiltinFunction( module,"month",kspreadfunc_month) ) );
  module->addObject( "time", new KSValue( new KSBuiltinFunction( module,"time",kspreadfunc_time) ) );
  module->addObject( "currentTime", new KSValue( new KSBuiltinFunction( module,"currentTime",kspreadfunc_currentTime) ) );
  module->addObject( "currentDate", new KSValue( new KSBuiltinFunction( module,"currentDate",kspreadfunc_currentDate) ) );
  module->addObject( "currentDateTime", new KSValue( new KSBuiltinFunction( module,"currentDateTime",kspreadfunc_currentDateTime) ) );
  module->addObject( "dayOfYear", new KSValue( new KSBuiltinFunction( module,"dayOfYear",kspreadfunc_dayOfYear) ) );
  module->addObject( "fact", new KSValue( new KSBuiltinFunction( module,"fact",kspreadfunc_fact) ) );
  module->addObject( "COMBIN", new KSValue( new KSBuiltinFunction( module,"COMBIN",kspreadfunc_combin) ) );
  module->addObject( "PERMUT", new KSValue( new KSBuiltinFunction( module,"PERMUT",kspreadfunc_arrang) ) );
  module->addObject( "BINO", new KSValue( new KSBuiltinFunction( module,"BINO",kspreadfunc_bino) ) );
  module->addObject( "INVBINO", new KSValue( new KSBuiltinFunction( module,"INVBINO",kspreadfunc_bino_inv) ) );
  module->addObject( "lower", new KSValue( new KSBuiltinFunction( module,"lower",kspreadfunc_lower) ) );
  module->addObject( "upper", new KSValue( new KSBuiltinFunction( module,"upper",kspreadfunc_upper) ) );
  module->addObject( "find", new KSValue( new KSBuiltinFunction( module,"find",kspreadfunc_find) ) );
  module->addObject( "compound", new KSValue( new KSBuiltinFunction( module,"compound",kspreadfunc_compound) ) );
  module->addObject( "continuous", new KSValue( new KSBuiltinFunction( module,"continuous",kspreadfunc_continuous) ) );
  module->addObject( "effective", new KSValue( new KSBuiltinFunction( module,"effective",kspreadfunc_effective) ) );
  module->addObject( "nominal", new KSValue( new KSBuiltinFunction( module,"nominal",kspreadfunc_nominal) ) );
  module->addObject( "FV", new KSValue( new KSBuiltinFunction( module,"FV",kspreadfunc_fv) ) );
  module->addObject( "PV_annuity", new KSValue( new KSBuiltinFunction( module,"PV_annuity",kspreadfunc_pv_annuity) ) );
  module->addObject( "PV", new KSValue( new KSBuiltinFunction( module,"PV",kspreadfunc_pv) ) );
  module->addObject( "FV_annuity", new KSValue( new KSBuiltinFunction( module,"FV_annuity",kspreadfunc_fv_annuity) ) );
  module->addObject( "zero_coupon", new KSValue( new KSBuiltinFunction( module,"zero_coupon",kspreadfunc_zero_coupon) ) );
  module->addObject( "level_coupon", new KSValue( new KSBuiltinFunction( module,"level_coupon",kspreadfunc_level_coupon) ) );
  module->addObject( "sign", new KSValue( new KSBuiltinFunction( module,"sign",kspreadfunc_sign) ) );
  module->addObject( "atan2", new KSValue( new KSBuiltinFunction( module,"atan2",kspreadfunc_atan2) ) );
  module->addObject( "INV", new KSValue( new KSBuiltinFunction( module,"INV",kspreadfunc_inv) ) );

  module->addObject( "DEC2BIN", new KSValue( new KSBuiltinFunction( module,"DEC2BIN",kspreadfunc_dec2bin) ) );
  module->addObject( "DEC2OCT", new KSValue( new KSBuiltinFunction( module,"DEC2OCT",kspreadfunc_dec2oct) ) );
  module->addObject( "DEC2HEX", new KSValue( new KSBuiltinFunction( module,"DEC2HEX",kspreadfunc_dec2hex) ) );
  //compatibility with old formula
  module->addObject( "DECHEX", new KSValue( new KSBuiltinFunction( module,"DECHEX",kspreadfunc_dec2hex) ) );
  module->addObject( "DECBIN", new KSValue( new KSBuiltinFunction( module,"DECBIN",kspreadfunc_dec2bin) ) );
  module->addObject( "DECOCT", new KSValue( new KSBuiltinFunction( module,"DECOCT",kspreadfunc_dec2oct) ) );

  module->addObject( "ROUNDDOWN", new KSValue( new KSBuiltinFunction( module,"ROUNDDOWN",kspreadfunc_rounddown) ) );
  module->addObject( "ROUNDUP", new KSValue( new KSBuiltinFunction( module,"ROUNDUP",kspreadfunc_roundup) ) );
  module->addObject( "ROUND", new KSValue( new KSBuiltinFunction( module,"ROUND",kspreadfunc_round) ) );
  module->addObject( "BIN2DEC", new KSValue( new KSBuiltinFunction( module,"BIN2DEC",kspreadfunc_bin2dec) ) );
  module->addObject( "BIN2OCT", new KSValue( new KSBuiltinFunction( module,"BIN2OCT",kspreadfunc_bin2oct) ) );
  module->addObject( "BIN2HEX", new KSValue( new KSBuiltinFunction( module,"BIN2HEX",kspreadfunc_bin2hex) ) );
  module->addObject( "OCT2BIN", new KSValue( new KSBuiltinFunction( module,"OCT2BIN",kspreadfunc_oct2bin) ) );
  module->addObject( "OCT2DEC", new KSValue( new KSBuiltinFunction( module,"OCT2DEC",kspreadfunc_oct2dec) ) );
  module->addObject( "OCT2HEX", new KSValue( new KSBuiltinFunction( module,"OCT2HEX",kspreadfunc_oct2hex) ) );
  module->addObject( "HEX2BIN", new KSValue( new KSBuiltinFunction( module,"HEX2BIN",kspreadfunc_hex2bin) ) );
  module->addObject( "HEX2DEC", new KSValue( new KSBuiltinFunction( module,"HEX2DEC",kspreadfunc_hex2dec) ) );
  module->addObject( "HEX2OCT", new KSValue( new KSBuiltinFunction( module,"HEX2OCT",kspreadfunc_hex2oct) ) );
  module->addObject( "POLR", new KSValue( new KSBuiltinFunction( module,"POLR",kspreadfunc_polr) ) );
  module->addObject( "POLA", new KSValue( new KSBuiltinFunction( module,"POLA",kspreadfunc_pola) ) );
  module->addObject( "CARX", new KSValue( new KSBuiltinFunction( module,"CARX",kspreadfunc_carx) ) );
  module->addObject( "CARY", new KSValue( new KSBuiltinFunction( module,"CARY",kspreadfunc_cary) ) );
  //complex
  module->addObject( "COMPLEX", new KSValue( new KSBuiltinFunction( module,"COMPLEX",kspreadfunc_complex) ) );
  module->addObject( "IMAGINARY", new KSValue( new KSBuiltinFunction( module,"IMAGINARY",kspreadfunc_complex_imag) ) );
  module->addObject( "IMREAL", new KSValue( new KSBuiltinFunction( module,"IMREAL",kspreadfunc_complex_real) ) );
  module->addObject( "IMSUM", new KSValue( new KSBuiltinFunction( module, "IMSUM", kspreadfunc_imsum ) ) );
  module->addObject( "IMSUB", new KSValue( new KSBuiltinFunction( module, "IMSUB", kspreadfunc_imsub ) ) );
  module->addObject( "IMPRODUCT", new KSValue( new KSBuiltinFunction( module, "IMPRODUCT", kspreadfunc_improduct ) ) );
  module->addObject( "IMCONJUGATE", new KSValue( new KSBuiltinFunction( module, "IMCONJUGATE", kspreadfunc_imconjugate ) ) );
  module->addObject( "IMARGUMENT", new KSValue( new KSBuiltinFunction( module, "IMARGUMENT", kspreadfunc_imargument ) ) );
  module->addObject( "IMABS", new KSValue( new KSBuiltinFunction( module, "IMABS", kspreadfunc_imabs ) ) );
  module->addObject( "IMDIV", new KSValue( new KSBuiltinFunction( module, "IMDIV", kspreadfunc_imdiv ) ) );
  module->addObject( "IMCOS", new KSValue( new KSBuiltinFunction( module, "IMCOS", kspreadfunc_imcos ) ) );
  module->addObject( "IMSIN", new KSValue( new KSBuiltinFunction( module, "IMSIN", kspreadfunc_imsin ) ) );
  module->addObject( "IMEXP", new KSValue( new KSBuiltinFunction( module, "IMEXP", kspreadfunc_imexp ) ) );
  module->addObject( "IMLN", new KSValue( new KSBuiltinFunction( module, "IMLN", kspreadfunc_imln ) ) );
  module->addObject( "IMSQRT", new KSValue( new KSBuiltinFunction( module, "IMSQRT", kspreadfunc_imsqrt ) ) );
  module->addObject( "IMPOWER", new KSValue( new KSBuiltinFunction( module, "IMPOWER", kspreadfunc_impower ) ) );

  module->addObject( "SUMPRODUCT", new KSValue( new KSBuiltinFunction( module, "SUMPRODUCT", kspreadfunc_sumproduct ) ) );
  module->addObject( "SUMX2PY2", new KSValue( new KSBuiltinFunction( module, "SUMX2PY2", kspreadfunc_sumx2py2 ) ) );
  module->addObject( "SUMX2MY2", new KSValue( new KSBuiltinFunction( module, "SUMX2MY2", kspreadfunc_sumx2my2 ) ) );
  module->addObject( "SUM2XMY", new KSValue( new KSBuiltinFunction( module, "SUM2XMY", kspreadfunc_sumxmy2 ) ) );
  module->addObject( "DELTA", new KSValue( new KSBuiltinFunction( module, "DELTA", kspreadfunc_delta ) ) );
  module->addObject( "EVEN", new KSValue( new KSBuiltinFunction( module, "EVEN", kspreadfunc_even ) ) );
  module->addObject( "ODD", new KSValue( new KSBuiltinFunction( module, "ODD", kspreadfunc_odd ) ) );
  module->addObject( "RANDBETWEEN", new KSValue( new KSBuiltinFunction( module, "RANDBETWEEN", kspreadfunc_randbetween ) ) );
  module->addObject( "LOGn", new KSValue( new KSBuiltinFunction( module, "LOGn", kspreadfunc_logn ) ) );
  module->addObject( "SQRTn", new KSValue( new KSBuiltinFunction( module, "SQRTn", kspreadfunc_sqrtn ) ) );
  module->addObject( "count", new KSValue( new KSBuiltinFunction( module, "count", kspreadfunc_count ) ) );
  module->addObject( "CUR", new KSValue( new KSBuiltinFunction( module, "CUR", kspreadfunc_cur ) ) );
  module->addObject( "DECSEX", new KSValue( new KSBuiltinFunction( module, "DECSEX", kspreadfunc_decsex) ) );
  module->addObject( "SEXDEC", new KSValue( new KSBuiltinFunction( module, "SEXDEC", kspreadfunc_sexdec) ) );
  module->addObject( "ISTIME", new KSValue( new KSBuiltinFunction( module, "ISTIME", kspreadfunc_istime) ) );
  module->addObject( "ISDATE", new KSValue( new KSBuiltinFunction( module, "ISDATE", kspreadfunc_isdate) ) );
  module->addObject( "hours", new KSValue( new KSBuiltinFunction( module, "hours", kspreadfunc_hours) ) );
  module->addObject( "minutes", new KSValue( new KSBuiltinFunction( module, "minutes", kspreadfunc_minutes) ) );
  module->addObject( "isLeapYear", new KSValue( new KSBuiltinFunction( module, "isLeapYear", kspreadfunc_isLeapYear) ) );
  module->addObject( "daysInMonth", new KSValue( new KSBuiltinFunction( module, "daysInMonth", kspreadfunc_daysInMonth) ) );
  module->addObject( "daysInYear", new KSValue( new KSBuiltinFunction( module, "daysInYear", kspreadfunc_daysInYear) ) );
  module->addObject( "weeksInYear", new KSValue( new KSBuiltinFunction( module, "weeksInYear", kspreadfunc_weeksInYear) ) );
  module->addObject( "seconds", new KSValue( new KSBuiltinFunction( module, "seconds", kspreadfunc_seconds) ) );
  module->addObject( "ROMAN", new KSValue( new KSBuiltinFunction( module, "ROMAN", kspreadfunc_roman) ) );
  module->addObject( "shortcurrentDate", new KSValue( new KSBuiltinFunction( module, "shortcurrentDate", kspreadfunc_shortcurrentDate) ) );
  return module;
}

/*********************************************************************
 *
 * KSpreadInterpreter
 *
 *********************************************************************/

KSpreadInterpreter::KSpreadInterpreter( KSpreadDoc* doc ) : KSInterpreter()
{
  m_doc = doc;

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
      tmp = tmp.arg( util_cellName( cell->table(), cell->column(), cell->row() ) );
      tmp = tmp.arg( node->getStringLiteral() );
      context.setException( new KSException( "ErrorInCell", tmp ) );
      return false;
    }

    if ( cell->isDefault())
      context.setValue( new KSValue(  /*KSValue::Empty*/ 0.0 ) );
    else if(cell->isObscured() && cell->isObscuringForced())
      context.setValue( new KSValue( 0.0 ) );
    else if ( cell->isValue() )
      context.setValue( new KSValue( cell->valueDouble() ) );
    else if ( cell->isBool() )
      context.setValue( new KSValue( cell->valueBool() ) );
    else if ( cell->isTime() )
      context.setValue( new KSValue( cell->valueTime() ) );
    else if ( cell->isDate() )
      context.setValue( new KSValue( cell->valueDate() ) );
    else if ( cell->valueString().isEmpty() )
      context.setValue( new KSValue( 0.0  /*KSValue::Empty*/ ) );
    else
      context.setValue( new KSValue( cell->valueString() ) );
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
          tmp = tmp.arg( util_cellName( cell->table(), cell->column(), cell->row() ) );
          tmp = tmp.arg( node->getStringLiteral() );
          context.setException( new KSException( "ErrorInCell", tmp ) );
          return false;
        }

        if ( cell->isDefault() )
          c = new KSValue( 0.0 /*KSValue::Empty*/);
        else if ( cell->isValue() )
          c = new KSValue( cell->valueDouble() );
        else if ( cell->isBool() )
          c = new KSValue( cell->valueBool() );
        else if ( cell->isDate() )
          c = new KSValue( cell->valueDate() );
        else if ( cell->isTime() )
          c = new KSValue( cell->valueTime() );
        else if ( cell->valueString().isEmpty() )
          c = new KSValue( 0.0 /*KSValue::Empty*/ );
        else
          c = new KSValue( cell->valueString() );
        if ( !(cell->isObscured() && cell->isObscuringForced()) )
                l->listValue().append( c );
      }
      v->listValue().append( l );
    }
    context.setValue( v );

    return true;
  }
  else
    ASSERT( 0 );

  // Never reached
  return false;
}

KSParseNode* KSpreadInterpreter::parse( KSContext& context, KSpreadTable* table, const QString& formula, QList<KSpreadDepend>& depends )
{
    // Create the parse tree.
    KSParser parser;
    // Tell the parser the locale so that it can parse localized numbers.
    if ( !parser.parse( formula.local8Bit(), KSCRIPT_EXTENSION_KSPREAD, table->doc()->locale() ) )
    {
	context.setException( new KSException( "SyntaxError", parser.errorMessage() ) );
	return 0;
    }

    KSParseNode* n = parser.donateParseTree();
    makeDepends( context, n, table->map(), table, depends );

    return n;
}

bool KSpreadInterpreter::evaluate( KSContext& context, KSParseNode* node, KSpreadTable* table )
{
    // Save the current table to make this function reentrant.
    KSpreadTable* t = m_table;
    m_table = table;

    bool b = node->eval( context );

    m_table = t;

    return b;
}
