#include "kspread_util.h"
#include "kspread_doc.h"
#include "kspread_table.h"

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

// defined in kspread_functions_trig.cc
bool kspreadfunc_sin( KSContext& context );
bool kspreadfunc_cos( KSContext& context );
bool kspreadfunc_tan( KSContext& context );
bool kspreadfunc_atan( KSContext& context );
bool kspreadfunc_asin( KSContext& context );
bool kspreadfunc_acos( KSContext& context );
bool kspreadfunc_asinh( KSContext& context );
bool kspreadfunc_acosh( KSContext& context );
bool kspreadfunc_atanh( KSContext& context );
bool kspreadfunc_tanh( KSContext& context );
bool kspreadfunc_sinh( KSContext& context );
bool kspreadfunc_cosh( KSContext& context );
bool kspreadfunc_degree( KSContext& context );
bool kspreadfunc_radian( KSContext& context );
bool kspreadfunc_PI( KSContext& context );
bool kspreadfunc_atan2( KSContext& context );

// defined in kspread_datetime.cc
bool kspreadfunc_years( KSContext& context );
bool kspreadfunc_months( KSContext& context );
bool kspreadfunc_weeks( KSContext& context );
bool kspreadfunc_days( KSContext& context );
bool kspreadfunc_hours( KSContext& context );
bool kspreadfunc_minutes( KSContext& context );
bool kspreadfunc_seconds( KSContext& context );
bool kspreadfunc_date( KSContext& context );
bool kspreadfunc_day( KSContext& context );
bool kspreadfunc_month( KSContext& context );
bool kspreadfunc_time( KSContext& context );
bool kspreadfunc_currentDate( KSContext& context );
bool kspreadfunc_shortcurrentDate( KSContext& context );
bool kspreadfunc_currentTime( KSContext& context );
bool kspreadfunc_currentDateTime( KSContext& context );
bool kspreadfunc_dayOfYear( KSContext& context );
bool kspreadfunc_daysInMonth( KSContext& context );
bool kspreadfunc_isLeapYear ( KSContext& context );
bool kspreadfunc_daysInYear ( KSContext& context );
bool kspreadfunc_weeksInYear( KSContext& context );

// defined in kspread_functions_logic.cc
bool kspreadfunc_not( KSContext& context );
bool kspreadfunc_or( KSContext& context );
bool kspreadfunc_nor( KSContext& context );
bool kspreadfunc_and( KSContext& context );
bool kspreadfunc_nand( KSContext& context );
bool kspreadfunc_if( KSContext& context );
bool kspreadfunc_islogic( KSContext& context );
bool kspreadfunc_istext( KSContext& context );
bool kspreadfunc_isnottext( KSContext& context );
bool kspreadfunc_isnum( KSContext& context );
bool kspreadfunc_istime( KSContext& context );
bool kspreadfunc_isdate( KSContext& context );
bool kspreadfunc_isodd( KSContext& context );
bool kspreadfunc_iseven( KSContext& context );

// defined in kspread_functions_text.cc
bool kspreadfunc_join( KSContext& context );
bool kspreadfunc_left( KSContext& context );
bool kspreadfunc_right( KSContext& context );
bool kspreadfunc_upper( KSContext& context );
bool kspreadfunc_toggle( KSContext& context );
bool kspreadfunc_clean( KSContext& context );
bool kspreadfunc_sleek( KSContext& context );
bool kspreadfunc_proper(KSContext & context);
bool kspreadfunc_lower( KSContext& context );
bool kspreadfunc_find( KSContext& context );
bool kspreadfunc_mid( KSContext& context );
bool kspreadfunc_trim(KSContext& context );
bool kspreadfunc_len( KSContext& context );
bool kspreadfunc_EXACT( KSContext& context );
bool kspreadfunc_compare( KSContext& context );
bool kspreadfunc_replace( KSContext& context );
bool kspreadfunc_REPT( KSContext& context );

// defined in kspread_functions_financial.cc
bool kspreadfunc_fv( KSContext& context );
bool kspreadfunc_compound( KSContext& context );
bool kspreadfunc_continuous( KSContext& context );
bool kspreadfunc_pv( KSContext& context );
bool kspreadfunc_pv_annuity( KSContext& context );
bool kspreadfunc_fv_annuity( KSContext& context );
bool kspreadfunc_effective( KSContext& context );
bool kspreadfunc_zero_coupon( KSContext& context );
bool kspreadfunc_level_coupon( KSContext& context );
bool kspreadfunc_nominal( KSContext& context );

// defined in kspread_functions_statistical.cc
bool kspreadfunc_arrang( KSContext& context );
bool kspreadfunc_average( KSContext& context );
bool kspreadfunc_median( KSContext& context );
bool kspreadfunc_variance( KSContext& context );
bool kspreadfunc_stddev( KSContext& context );
bool kspreadfunc_combin( KSContext& context );
bool kspreadfunc_bino( KSContext& context );
bool kspreadfunc_bino_inv( KSContext& context );
bool kspreadfunc_phi(KSContext& context);
bool kspreadfunc_gauss(KSContext& context);
bool kspreadfunc_gammadist( KSContext& context );
bool kspreadfunc_betadist( KSContext& context );
bool kspreadfunc_fisher( KSContext& context );
bool kspreadfunc_fisherinv( KSContext& context );
bool kspreadfunc_normdist(KSContext& context );
bool kspreadfunc_lognormdist(KSContext& context );
bool kspreadfunc_stdnormdist(KSContext& context );
bool kspreadfunc_expondist(KSContext& context );
bool kspreadfunc_weibull( KSContext& context );
bool kspreadfunc_normsinv( KSContext& context );
bool kspreadfunc_norminv( KSContext& context );
bool kspreadfunc_gammaln( KSContext& context );
bool kspreadfunc_poisson( KSContext& context );
bool kspreadfunc_confidence( KSContext& context );
bool kspreadfunc_tdist( KSContext& context );
bool kspreadfunc_fdist( KSContext& context );
bool kspreadfunc_chidist( KSContext& context );
bool kspreadfunc_sumproduct( KSContext& context );
bool kspreadfunc_sumx2py2( KSContext& context );
bool kspreadfunc_sumx2my2( KSContext& context );
bool kspreadfunc_sumxmy2( KSContext& context );

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
void makeDepends( KSContext& context, KSParseNode* node, KSpreadMap* m, KSpreadTable* t, QPtrList<KSpreadDependency>& depends )
{
  KSParseNodeExtra* extra = node->extra();
  if ( !extra )
  {
    if ( node->getType() == t_cell )
    {
      KSParseNodeExtraPoint* extra = new KSParseNodeExtraPoint( node->getStringLiteral(), m, t );
      kdDebug(36002) << "-------- Got dep " << util_cellName( extra->point()->pos.x(), extra->point()->pos.y() ) << endl;
      KSpreadDependency* d = new KSpreadDependency(extra->point()->pos.x(), extra->point()->pos.y(),
					       extra->point()->table);
      if (!d->Table())
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
      KSpreadDependency* d = new KSpreadDependency(extra->range()->range.left(),
						   extra->range()->range.top(),
						   extra->range()->range.right(),
						   extra->range()->range.bottom(),
						   extra->range()->table);
      if (!d->Table())
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
 * Note: These modules must be registered in kspread_interpreter::kspreadCreateModule_KSpread
 *       They should also be documented in KSPREAD/extensions/builtin.xml
 *
 *********************************************************************/

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

  if ( !KSUtil::checkArgumentsCount( context, 1, "fabs", true ) || !KSUtil::checkArgumentsCount( context, 1, "abs", true ))
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
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  context.setValue( new KSValue( log( args[0]->doubleValue() ) /log( args[1]->doubleValue() )) );

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
    /*else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;*/
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

static bool kspreadfunc_product_helper( KSContext & context,
                                        QValueList<KSValue::Ptr> & args,
                                        double & result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_product_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result *= (*it)->doubleValue();
    }
  }

  return true;
}

static bool kspreadfunc_product( KSContext& context )
{
  double result = 1.0;
  bool b = kspreadfunc_product_helper( context,
                                       context.value()->listValue(),
                                       result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static int kspreadfunc_div_helper( KSContext & context,
                                   QValueList<KSValue::Ptr> & args,
                                   double & result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  result = (*it)->doubleValue();
  ++it;

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_product_helper( context, (*it)->listValue(), result ) )
        return 0;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      double val =(*it)->doubleValue();

      if (val == 0)
        return -1;

      result /= val;
    }
  }

  return 1;
}

static bool kspreadfunc_div( KSContext& context )
{
  double result = 0.0;
  int b = kspreadfunc_div_helper( context,
                                  context.value()->listValue(),
                                  result );

  if ( b == 1 )
    context.setValue( new KSValue( result ) );
  else if ( b == -1 )
    context.setValue( new KSValue( i18n("#DIV/0") ) );
  else
    return false;

  return true;
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

static int kspreadfunc_lcd_lcd(int value1, int value2)
{
  // start with the lower value.
  int n = (value1 <= value2 ? value1 : value2);

  // check if this is already the result
  if ((value1 % n == 0) && (value2 % n == 0))
  {
    return n;
  }
  // to save time: start with n = n / 2
  n = (int) (n / 2);

  while ((value1 % n != 0) || (value2 % n != 0))
  {
    --n;
  }

  return n;
}

static bool kspreadfunc_lcd_helper( KSContext & context,
                                    QValueList<KSValue::Ptr>& args,
                                    int & result)
{
  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  // at first get the smallest value to start with
  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_lcd_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::IntType, true ) )
    {
      int val = (*it)->intValue();

      if (val == 0)
      {
        result = 0;
        return true;
      }

      if ((result == 0) || (val < result))
        result = val;
    }
  }

  it = args.begin();

  // calculate the LCD:
  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_lcd_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::IntType, true ) )
    {
      int n = kspreadfunc_lcd_lcd(result, (*it)->intValue());

      if (n != result)
      {
        result = n;
        it = args.begin();

        continue;
      }
    }
  }

  if (result < 0)
    result *= -1;

  context.setValue(new KSValue(result));

  return true;
}

static bool kspreadfunc_lcd( KSContext & context )
{
  int result = 0;

  bool b = kspreadfunc_lcd_helper(context, context.value()->listValue(),
				  result);

  if (b)
    context.setValue(new KSValue(result));

  return b;
}

static bool kspreadfunc_lcm_helper( KSContext & context,
				    QValueList<KSValue::Ptr> & args,
				    double & result,
				    double & max,
				    double & inter,
				    int & signs)
{
  // calculating the LCM:
  // algorithm: choose the biggest value in the list
  // take this as interval for increasing the numbers you check with
  // go through the list, check every value if your proposal is a multiple
  // if not, increase your proposal value and start again.

  // when you have found your LCM: check if it has the right sign:
  // if the number of (-) signs in the list is , the result is positive
  // otherwise negative.

  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  // first loop: check for the biggest value
  // and count number of (-) signs:
  for (; it != end; ++it)
  {
    if (KSUtil::checkType(context, *it, KSValue::ListType, false))
    {
      if (!kspreadfunc_lcm_helper(context, (*it)->listValue(), result, max, inter, signs))
        return false;
    }
    else if (KSUtil::checkType(context, *it, KSValue::DoubleType, true))
    {
      double d = (double) (*it)->doubleValue();

      if (d < 0)
	++signs;

      if (result < d)
	result = d;
    }
  }

  inter = result;
  it    = args.begin();

  while (true)
  {
    if (KSUtil::checkType(context, *it, KSValue::ListType, false))
    {
      if (!kspreadfunc_lcm_helper( context, (*it)->listValue(), result, max, inter, signs))
	return false;
    }
    else if (KSUtil::checkType(context, *it, KSValue::DoubleType, true))
    {
      double d = inter / (*it)->doubleValue();

      // if it is not a multiple, increase you proposal value and start again
      if (!approx_equal(d, floor(d)))
      {
	inter += result;

	it = args.begin();
	continue;
      }
    } // end else if

    // ... otherwise check the next value if any
    ++it;

    if (it == end)
      break;
  }

  result = inter;

  // check if we have the correct sign (-/+)
  if (signs > 0)
  {
    if ((result < 0) && (signs % 2 == 0))
      result *= -1;
    else if ((result > 0) && (signs % 2 != 0))
      result *= -1;
  }

  return true;
}

static bool kspreadfunc_lcm( KSContext & context )
{
  double result = 0.0;
  double max    = 1.0;
  double inter  = 0.0;
  int    signs  = 0;

  bool b = kspreadfunc_lcm_helper(context, context.value()->listValue(),
				  result, max, inter, signs);

  if (b)
    context.setValue(new KSValue(result));

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

static bool kspreadfunc_eps( KSContext& context )
{
// #### This should adjust according to the actual number system used (float, double, long double, ...)
    if( !KSUtil::checkArgumentsCount( context, 0, "eps", true ) )
      return false;

    context.setValue( new KSValue(DBL_EPSILON));
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

    context.setValue( new KSValue((double)(((double)args[1]->intValue()-(double)args[0]->intValue())*rand()/RAND_MAX+((double)args[0]->intValue()))));

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

static double fact( double val, double end )
{
  /* fact =i*(i-1)*(i-2)*...*1 */
  if(val<0.0 || end<0.0)
    return (-1);
  if(val==0.0)
    return (1);
  else if (val==end)
    return(1);
  /*val==end => you don't multiplie it */
  else
    return (val*fact((double)(val-1),end));
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
  if (approx_equal(floor(args[0]->doubleValue()*::pow(10,digits)), args[0]->doubleValue()*::pow(10,digits)))
      result = args[0]->doubleValue();
  else
      result=floor(args[0]->doubleValue()*::pow(10,digits)+1)/::pow(10,digits);
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

  double arg=::pow(sqrt(pow(imag,2)+pow(real,2)),args[1]->intValue());
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

static bool kspreadfunc_AsciiToChar( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  int val = -1;
  QString str;

  for (unsigned int i = 0; i < args.count(); i++)
  {
    if ( KSUtil::checkType( context, args[i], KSValue::IntType, false ) )
    {
      val = (int)args[i]->intValue();
      QChar c(val);
      str = str + c;
    }
    else return false;
  }

  context.setValue( new KSValue(str));
  return true;
}

static bool kspreadfunc_CharToAscii( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if (args.count() == 1)
  {
    if ( KSUtil::checkType( context, args.first(), KSValue::StringType, false ) )
    {
      QString val = args[0]->stringValue();
      if (val.length() == 1)
      {
	QChar c = val[0];
	context.setValue( new KSValue(c.unicode() ));
	return true;
      }
    }
  }
  return false;
}

static bool kspreadfunc_inttobool( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if (args.count() == 1)
  {
    if (KSUtil::checkType(context, args[0],
                          KSValue::IntType, true))
    {
      bool result = (args[0]->intValue() == 1 ? true : false);

      context.setValue( new KSValue(result) );

      return true;
    }
  }

  return false;
}

static bool kspreadfunc_booltoint( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if (args.count() == 1)
  {
    if (KSUtil::checkType(context, args[0],
                          KSValue::BoolType, true))
    {
      int val = (args[0]->boolValue() ? 1 : 0);

      context.setValue( new KSValue(val));

      return true;
    }
  }

  return false;
}

static bool kspreadfunc_BoolToString( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if (args.count() == 1)
  {
    if ( KSUtil::checkType( context, args.first(), KSValue::BoolType, false ) )
    {
      QString val((args[0]->boolValue() ? "True" : "False"));

      context.setValue( new KSValue(val));

      return true;
    }
  }

  return false;
}

static bool kspreadfunc_NumberToString( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if (args.count() == 1)
  {
    if ( KSUtil::checkType( context, args.first(), KSValue::DoubleType, false ) )
    {
      QString val;
      val.setNum(args[0]->doubleValue(), 'g', 8);

      context.setValue( new KSValue(val));

      return true;
    }
  }

  return false;
}

static KSModule::Ptr kspreadCreateModule_KSpread( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "kspread" );

  module->addObject( "cos", new KSValue( new KSBuiltinFunction( module, "cos", kspreadfunc_cos ) ) );
  module->addObject( "sin", new KSValue( new KSBuiltinFunction( module, "sin", kspreadfunc_sin ) ) );
  module->addObject( "sum", new KSValue( new KSBuiltinFunction( module, "sum", kspreadfunc_sum ) ) );
  module->addObject( "DIV", new KSValue( new KSBuiltinFunction( module, "DIV", kspreadfunc_div ) ) );
  module->addObject( "PRODUCT", new KSValue( new KSBuiltinFunction( module, "PRODUCT", kspreadfunc_product ) ) );
  module->addObject( "sumsq", new KSValue( new KSBuiltinFunction( module, "sumsq", kspreadfunc_sumsq ) ) );
  module->addObject( "sqrt", new KSValue( new KSBuiltinFunction( module, "sqrt", kspreadfunc_sqrt ) ) );
  module->addObject( "fabs", new KSValue( new KSBuiltinFunction( module, "fabs", kspreadfunc_fabs ) ) );
  module->addObject( "abs", new KSValue( new KSBuiltinFunction( module, "abs", kspreadfunc_fabs ) ) );
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
  module->addObject( "LCM", new KSValue( new KSBuiltinFunction( module, "LCM", kspreadfunc_lcm ) ) );
  module->addObject( "LCD", new KSValue( new KSBuiltinFunction( module, "LCD", kspreadfunc_lcd ) ) );
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
  module->addObject( "median", new KSValue( new KSBuiltinFunction( module, "median", kspreadfunc_median ) ) );
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
  module->addObject( "COMPARE", new KSValue( new KSBuiltinFunction( module, "COMPARE", kspreadfunc_compare) ) );
  module->addObject( "PROPER", new KSValue( new KSBuiltinFunction( module, "PROPER", kspreadfunc_proper) ) );
  module->addObject( "INT", new KSValue( new KSBuiltinFunction( module, "INT",kspreadfunc_INT) ) );
  //compatibility with kspread1.0
  module->addObject( "ENT", new KSValue( new KSBuiltinFunction( module, "ENT",kspreadfunc_INT) ) );
  module->addObject( "PI", new KSValue( new KSBuiltinFunction( module, "PI",kspreadfunc_PI) ) );
  module->addObject( "eps", new KSValue( new KSBuiltinFunction( module, "eps",kspreadfunc_eps) ) );
  module->addObject( "rand", new KSValue( new KSBuiltinFunction( module, "rand",kspreadfunc_rand) ) );
  module->addObject( "REPLACE", new KSValue( new KSBuiltinFunction( module, "REPLACE",kspreadfunc_replace) ) );
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
  module->addObject( "TOGGLE", new KSValue( new KSBuiltinFunction( module,"TOGGLE",kspreadfunc_toggle) ) );
  module->addObject( "find", new KSValue( new KSBuiltinFunction( module,"find",kspreadfunc_find) ) );
  module->addObject( "CLEAN", new KSValue( new KSBuiltinFunction( module, "CLEAN", kspreadfunc_clean) ) );
  module->addObject( "SLEEK", new KSValue( new KSBuiltinFunction( module, "SLEEK", kspreadfunc_sleek) ) );
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
  module->addObject( "DAYS", new KSValue( new KSBuiltinFunction( module, "DAYS", kspreadfunc_days) ) );
  module->addObject( "WEEKS", new KSValue( new KSBuiltinFunction( module, "WEEKS", kspreadfunc_weeks) ) );
  module->addObject( "MONTHS", new KSValue( new KSBuiltinFunction( module, "MONTHS", kspreadfunc_months) ) );
  module->addObject( "YEARS", new KSValue( new KSBuiltinFunction( module, "YEARS", kspreadfunc_years) ) );
  module->addObject( "isLeapYear", new KSValue( new KSBuiltinFunction( module, "isLeapYear", kspreadfunc_isLeapYear) ) );
  module->addObject( "daysInMonth", new KSValue( new KSBuiltinFunction( module, "daysInMonth", kspreadfunc_daysInMonth) ) );
  module->addObject( "daysInYear", new KSValue( new KSBuiltinFunction( module, "daysInYear", kspreadfunc_daysInYear) ) );
  module->addObject( "weeksInYear", new KSValue( new KSBuiltinFunction( module, "weeksInYear", kspreadfunc_weeksInYear) ) );
  module->addObject( "seconds", new KSValue( new KSBuiltinFunction( module, "seconds", kspreadfunc_seconds) ) );
  module->addObject( "ROMAN", new KSValue( new KSBuiltinFunction( module, "ROMAN", kspreadfunc_roman) ) );
  module->addObject( "shortcurrentDate", new KSValue( new KSBuiltinFunction( module, "shortcurrentDate", kspreadfunc_shortcurrentDate) ) );
  module->addObject( "trim", new KSValue( new KSBuiltinFunction( module, "trim", kspreadfunc_trim) ) );

  //statistical stuff
  module->addObject( "GAUSS", new KSValue( new KSBuiltinFunction( module, "GAUSS", kspreadfunc_gauss) ) );
  module->addObject( "PHI", new KSValue( new KSBuiltinFunction( module, "PHI", kspreadfunc_phi) ) );
  module->addObject( "GAMMADIST", new KSValue( new KSBuiltinFunction( module, "GAMMADIST", kspreadfunc_gammadist) ) );
  module->addObject( "BETADIST", new KSValue( new KSBuiltinFunction( module, "BETADIST", kspreadfunc_betadist) ) );
  module->addObject( "FISHER", new KSValue( new KSBuiltinFunction( module, "FISHER", kspreadfunc_fisher) ) );
  module->addObject( "FISHERINV", new KSValue( new KSBuiltinFunction( module, "FISHERINV", kspreadfunc_fisherinv) ) );
  module->addObject( "NORMDIST", new KSValue( new KSBuiltinFunction( module, "NORMDIST", kspreadfunc_normdist) ) );
  module->addObject( "LOGNORMDIST", new KSValue( new KSBuiltinFunction( module, "LOGNORMDIST", kspreadfunc_lognormdist) ) );
  module->addObject( "NORMSDIST", new KSValue( new KSBuiltinFunction( module, "NORMSDIST", kspreadfunc_stdnormdist) ) );
  module->addObject( "EXPONDIST", new KSValue( new KSBuiltinFunction( module, "EXPONDIST", kspreadfunc_expondist) ) );
  module->addObject( "WEIBULL", new KSValue( new KSBuiltinFunction( module, "WEIBULL", kspreadfunc_weibull) ) );
  module->addObject( "NORMSINV", new KSValue( new KSBuiltinFunction( module, "NORMSINV", kspreadfunc_normsinv) ) );
  module->addObject( "NORMINV", new KSValue( new KSBuiltinFunction( module, "NORMINV", kspreadfunc_norminv) ) );
  module->addObject( "GAMMALN", new KSValue( new KSBuiltinFunction( module, "GAMMALN", kspreadfunc_gammaln) ) );
  module->addObject( "POISSON", new KSValue( new KSBuiltinFunction( module, "POISSON", kspreadfunc_poisson) ) );
  module->addObject( "CONFIDENCE", new KSValue( new KSBuiltinFunction( module, "CONFIDENCE", kspreadfunc_confidence) ) );
  module->addObject( "TDIST", new KSValue( new KSBuiltinFunction( module, "TDIST", kspreadfunc_tdist) ) );
  module->addObject( "FDIST", new KSValue( new KSBuiltinFunction( module, "FDIST", kspreadfunc_fdist) ) );
  module->addObject( "CHIDIST", new KSValue( new KSBuiltinFunction( module, "CHIDIST", kspreadfunc_chidist) ) );
  module->addObject( "CharToAscii", new KSValue( new KSBuiltinFunction( module, "CharToAscii", kspreadfunc_CharToAscii) ) );
  module->addObject( "AsciiToChar", new KSValue( new KSBuiltinFunction( module, "AsciiToChar", kspreadfunc_AsciiToChar) ) );
  module->addObject( "BOOL2STRING", new KSValue( new KSBuiltinFunction( module, "BOOL2STRING", kspreadfunc_BoolToString) ) );
  module->addObject( "NUM2STRING", new KSValue( new KSBuiltinFunction( module, "NUM2STRING", kspreadfunc_NumberToString) ) );
  module->addObject( "BOOL2INT", new KSValue( new KSBuiltinFunction( module, "BOOL2INT", kspreadfunc_booltoint) ) );
  module->addObject( "INT2BOOL", new KSValue( new KSBuiltinFunction( module, "INT2BOOL", kspreadfunc_inttobool) ) );

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
    else if ( cell->isNumeric() )
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
        else if ( cell->isNumeric() )
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
    Q_ASSERT( 0 );

  // Never reached
  return false;
}

KSParseNode* KSpreadInterpreter::parse( KSContext& context, KSpreadTable* table, const QString& formula, QPtrList<KSpreadDependency>& depends )
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
