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

// defined in kspread_functions_math.cc
bool kspreadfunc_sqrt( KSContext& context );
bool kspreadfunc_sqrtn( KSContext& context );
bool kspreadfunc_cur( KSContext& context );
bool kspreadfunc_fabs( KSContext& context );
bool kspreadfunc_exp( KSContext& context );
bool kspreadfunc_ceil( KSContext& context );
bool kspreadfunc_floor( KSContext& context );
bool kspreadfunc_ln( KSContext& context );
bool kspreadfunc_logn( KSContext& context );
bool kspreadfunc_log( KSContext& context );
bool kspreadfunc_sum( KSContext& context );
bool kspreadfunc_product( KSContext& context );
bool kspreadfunc_div( KSContext& context );
bool kspreadfunc_sumsq( KSContext& context );
bool kspreadfunc_max( KSContext& context );
bool kspreadfunc_lcd( KSContext & context );
bool kspreadfunc_lcm( KSContext & context );
bool kspreadfunc_min( KSContext& context );
bool kspreadfunc_mult( KSContext& context );
bool kspreadfunc_INT( KSContext& context );
bool kspreadfunc_eps( KSContext& context );
bool kspreadfunc_rand( KSContext& context );
bool kspreadfunc_randbetween( KSContext& context );
bool kspreadfunc_pow( KSContext& context );
bool kspreadfunc_mod( KSContext& context );
bool kspreadfunc_fact( KSContext& context );
bool kspreadfunc_sign( KSContext& context );
bool kspreadfunc_inv( KSContext& context );
bool kspreadfunc_rounddown( KSContext& context );
bool kspreadfunc_roundup( KSContext& context );
bool kspreadfunc_round( KSContext& context );
bool kspreadfunc_complex( KSContext& context );
bool kspreadfunc_complex_imag( KSContext& context );
bool kspreadfunc_complex_real( KSContext& context );
bool kspreadfunc_imsum( KSContext& context );
bool kspreadfunc_imsub( KSContext& context );
bool kspreadfunc_improduct( KSContext& context );
bool kspreadfunc_imconjugate( KSContext& context );
bool kspreadfunc_imargument( KSContext& context );
bool kspreadfunc_imabs( KSContext& context );
bool kspreadfunc_imcos( KSContext& context );
bool kspreadfunc_imsin( KSContext& context );
bool kspreadfunc_imln( KSContext& context );
bool kspreadfunc_imexp( KSContext& context );
bool kspreadfunc_imsqrt( KSContext& context );
bool kspreadfunc_impower( KSContext& context );
bool kspreadfunc_imdiv( KSContext& context );
bool kspreadfunc_delta( KSContext& context );
bool kspreadfunc_even( KSContext& context );
bool kspreadfunc_odd( KSContext& context );
bool kspreadfunc_count( KSContext& context );

// defined in kspread_functions_datetime.cc
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
bool kspreadfunc_xor( KSContext& context );
bool kspreadfunc_if( KSContext& context );
bool kspreadfunc_islogic( KSContext& context );
bool kspreadfunc_isempty( KSContext& context );
bool kspreadfunc_istext( KSContext& context );
bool kspreadfunc_isnottext( KSContext& context );
bool kspreadfunc_isnum( KSContext& context );
bool kspreadfunc_istime( KSContext& context );
bool kspreadfunc_isdate( KSContext& context );
bool kspreadfunc_isodd( KSContext& context );
bool kspreadfunc_iseven( KSContext& context );

// defined in kspread_functions_text.cc
bool kspreadfunc_char( KSContext& context );
bool kspreadfunc_clean( KSContext& context );
bool kspreadfunc_code( KSContext& context );
bool kspreadfunc_compare( KSContext& context ); // KSpread specific
bool kspreadfunc_concatenate( KSContext& context ); 
bool kspreadfunc_dollar( KSContext& context ); 
bool kspreadfunc_exact( KSContext& context );
bool kspreadfunc_find( KSContext& context );
bool kspreadfunc_fixed( KSContext& context ); 
bool kspreadfunc_join( KSContext& context );    // obsolete, use CONCATENATE
bool kspreadfunc_left( KSContext& context );
bool kspreadfunc_len( KSContext& context );
bool kspreadfunc_lower( KSContext& context );
bool kspreadfunc_mid( KSContext& context );
bool kspreadfunc_proper(KSContext & context);
bool kspreadfunc_replace( KSContext& context );
bool kspreadfunc_rept( KSContext& context );
bool kspreadfunc_right( KSContext& context );
bool kspreadfunc_search( KSContext& context ); //TODO
bool kspreadfunc_sleek( KSContext& context );   // KSpread-specific
bool kspreadfunc_substitute( KSContext& context ); //TODO
bool kspreadfunc_t( KSContext& context ); 
bool kspreadfunc_text( KSContext& context ); //TODO
bool kspreadfunc_toggle( KSContext& context );  // KSpread-specific
bool kspreadfunc_trim(KSContext& context );
bool kspreadfunc_upper( KSContext& context );
bool kspreadfunc_value( KSContext& context );

// defined in kspread_functions_conversion.cc
bool kspreadfunc_dec2hex( KSContext& context );
bool kspreadfunc_dec2oct( KSContext& context );
bool kspreadfunc_dec2bin( KSContext& context );
bool kspreadfunc_bin2dec( KSContext& context );
bool kspreadfunc_bin2oct( KSContext& context );
bool kspreadfunc_bin2hex( KSContext& context );
bool kspreadfunc_oct2dec( KSContext& context );
bool kspreadfunc_oct2bin( KSContext& context );
bool kspreadfunc_oct2hex( KSContext& context );
bool kspreadfunc_hex2dec( KSContext& context );
bool kspreadfunc_hex2bin( KSContext& context );
bool kspreadfunc_hex2oct( KSContext& context );
bool kspreadfunc_polr( KSContext& context );
bool kspreadfunc_pola( KSContext& context );
bool kspreadfunc_carx( KSContext& context );
bool kspreadfunc_cary( KSContext& context );
bool kspreadfunc_decsex( KSContext& context );
bool kspreadfunc_sexdec( KSContext& context );
bool kspreadfunc_roman( KSContext& context );
bool kspreadfunc_AsciiToChar( KSContext& context );
bool kspreadfunc_CharToAscii( KSContext& context );
bool kspreadfunc_inttobool( KSContext & context );
bool kspreadfunc_booltoint( KSContext & context );
bool kspreadfunc_BoolToString( KSContext& context );
bool kspreadfunc_NumberToString( KSContext& context );

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
bool kspreadfunc_sln( KSContext& context );
bool kspreadfunc_syd( KSContext& context );
bool kspreadfunc_db( KSContext& context );
bool kspreadfunc_euro( KSContext& context );

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
  KSParseNodeExtraRange( const QString& s, KSpreadMap* m, KSpreadTable* t ) 
    : m_range( s, m, t ) { }

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
      kdDebug(36001) << "-------- Got dep " << util_cellName( extra->point()->pos.x(), extra->point()->pos.y() ) << endl;
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

typedef struct
{
  const char *name;
  bool (*function)( KSContext& );
} functionEntry;

static const functionEntry funcTab[] = {

  // trigonometric
  { "COS", kspreadfunc_cos },
  { "SIN", kspreadfunc_sin },
  { "TAN", kspreadfunc_tan },
  { "ATAN", kspreadfunc_atan },
  { "ASIN", kspreadfunc_asin },
  { "ACOS", kspreadfunc_acos },
  { "COSH", kspreadfunc_cosh },
  { "SINH", kspreadfunc_sinh },
  { "TANH", kspreadfunc_tanh },
  { "ACOSH", kspreadfunc_acosh },
  { "ASINH", kspreadfunc_asinh },
  { "ATANH", kspreadfunc_atanh },
  { "ATAN2", kspreadfunc_atan2 },

  // math
  { "SUM", kspreadfunc_sum },
  { "SUMSQ", kspreadfunc_sumsq },
  { "DIV", kspreadfunc_div },
  { "PRODUCT", kspreadfunc_product },
  { "SQRT", kspreadfunc_sqrt },
  { "FABS", kspreadfunc_fabs },
  { "ABS", kspreadfunc_fabs },
  { "MAX", kspreadfunc_max },
  { "MIN", kspreadfunc_min },
  { "FLOOR", kspreadfunc_floor },
  { "CEIL", kspreadfunc_ceil },
  { "INT", kspreadfunc_INT },
  { "EXP", kspreadfunc_exp },
  { "LN", kspreadfunc_ln },
  { "LOG", kspreadfunc_log },
  { "LCM", kspreadfunc_lcm },
  { "LCD", kspreadfunc_lcd },
  { "MULTIPLY", kspreadfunc_mult },
  { "PI", kspreadfunc_PI },
  { "EPS", kspreadfunc_eps },
  { "RAND", kspreadfunc_rand },
  { "POW", kspreadfunc_pow },
  { "MOD", kspreadfunc_mod },
  { "FACT", kspreadfunc_fact },
  { "SIGN", kspreadfunc_sign },
  { "INV", kspreadfunc_inv },
  { "RANDBETWEEN", kspreadfunc_randbetween },
  { "LOGN", kspreadfunc_logn },
  { "SQRTN", kspreadfunc_sqrtn },
  { "COUNT", kspreadfunc_count },
  { "CUR", kspreadfunc_cur },
  { "ROUND", kspreadfunc_round },
  { "ROUNDDOWN", kspreadfunc_rounddown },
  { "ROUNDUP", kspreadfunc_roundup },
  { "DELTA", kspreadfunc_delta },
  { "EVEN", kspreadfunc_even },
  { "ODD", kspreadfunc_odd },
  { "COMPLEX", kspreadfunc_complex },
  { "IMAGINARY", kspreadfunc_complex_imag },
  { "IMREAL", kspreadfunc_complex_real },
  { "IMSUM", kspreadfunc_imsum },
  { "IMSUB", kspreadfunc_imsub },
  { "IMPRODUCT", kspreadfunc_improduct },
  { "IMCONJUGATE", kspreadfunc_imconjugate },
  { "IMARGUMENT", kspreadfunc_imargument },
  { "IMABS", kspreadfunc_imabs },
  { "IMDIV", kspreadfunc_imdiv },
  { "IMCOS", kspreadfunc_imcos },
  { "IMSIN", kspreadfunc_imsin },
  { "IMEXP", kspreadfunc_imexp },
  { "IMLN", kspreadfunc_imln },
  { "IMSQRT", kspreadfunc_imsqrt },
  { "IMPOWER", kspreadfunc_impower },

  // date & time
  { "DATE", kspreadfunc_date },
  { "DAY", kspreadfunc_day },
  { "MONTH", kspreadfunc_month },
  { "TIME", kspreadfunc_time },
  { "CURRENTTIME", kspreadfunc_currentTime },
  { "CURRENTDATE", kspreadfunc_currentDate },
  { "CURRENTDATETIME", kspreadfunc_currentDateTime },
  { "DAYOFYEAR", kspreadfunc_dayOfYear },
  { "HOURS", kspreadfunc_hours },
  { "MINUTES", kspreadfunc_minutes },
  { "DAYS", kspreadfunc_days },
  { "WEEKS", kspreadfunc_weeks },
  { "MONTHS", kspreadfunc_months },
  { "YEARS", kspreadfunc_years },
  { "ISLEAPYEAR", kspreadfunc_isLeapYear },
  { "DAYSINMONTH", kspreadfunc_daysInMonth },
  { "DAYSINYEAR", kspreadfunc_daysInYear },
  { "WEEKSINYEAR", kspreadfunc_weeksInYear },
  { "SECONDS", kspreadfunc_seconds },
  { "SHORTCURRENTDATE", kspreadfunc_shortcurrentDate },

  // conversion
  { "DEGREE", kspreadfunc_degree },
  { "RADIAN", kspreadfunc_radian },
  { "DEC2HEX", kspreadfunc_dec2hex },
  { "DEC2BIN", kspreadfunc_dec2bin },
  { "DEC2OCT", kspreadfunc_dec2oct },
  { "BIN2DEC", kspreadfunc_bin2dec },
  { "BIN2OCT", kspreadfunc_bin2oct },
  { "BIN2HEX", kspreadfunc_bin2hex },
  { "OCT2BIN", kspreadfunc_oct2bin },
  { "OCT2DEC", kspreadfunc_oct2dec },
  { "OCT2HEX", kspreadfunc_oct2hex },
  { "HEX2BIN", kspreadfunc_hex2bin },
  { "HEX2DEC", kspreadfunc_hex2dec },
  { "HEX2OCT", kspreadfunc_hex2oct },
  { "CHARTOASCII", kspreadfunc_CharToAscii },
  { "ASCIITOCHAR", kspreadfunc_AsciiToChar },
  { "BOOL2STRING", kspreadfunc_BoolToString },
  { "NUM2STRING", kspreadfunc_NumberToString },
  { "BOOL2INT", kspreadfunc_booltoint },
  { "INT2BOOL", kspreadfunc_inttobool },
  { "DECSEX", kspreadfunc_decsex },
  { "SEXDEC", kspreadfunc_sexdec },
  { "ROMAN", kspreadfunc_roman },
  { "POLR", kspreadfunc_polr },
  { "POLA", kspreadfunc_pola },
  { "CARX", kspreadfunc_carx },
  { "CARY", kspreadfunc_cary },

  // logical
  { "IF", kspreadfunc_if },
  { "OR", kspreadfunc_or },
  { "AND", kspreadfunc_and },
  { "NOR", kspreadfunc_nor },
  { "NAND", kspreadfunc_nand },
  { "XOR", kspreadfunc_xor },
  { "NOT", kspreadfunc_not },
  { "ISLOGIC", kspreadfunc_islogic },
  { "ISEMPTY", kspreadfunc_isempty },
  { "ISTEXT", kspreadfunc_istext },
  { "ISNUM", kspreadfunc_isnum },
  { "ISNOTTEXT", kspreadfunc_isnottext },
  { "ISODD", kspreadfunc_isodd },
  { "ISEVEN", kspreadfunc_iseven },
  { "ISDATE", kspreadfunc_isdate },
  { "ISTIME", kspreadfunc_istime },

  // statistical
  { "AVERAGE", kspreadfunc_average },
  { "MEDIAN", kspreadfunc_median },
  { "VARIANCE", kspreadfunc_variance },
  { "STDDEV", kspreadfunc_stddev },
  { "COMBIN", kspreadfunc_combin },
  { "PERMUT", kspreadfunc_arrang },
  { "BINO", kspreadfunc_bino },
  { "INVBINO", kspreadfunc_bino_inv },
  { "SUMPRODUCT", kspreadfunc_sumproduct },
  { "SUMX2PY2", kspreadfunc_sumx2py2 },
  { "SUMX2MY2", kspreadfunc_sumx2my2 },
  { "SUM2XMY", kspreadfunc_sumxmy2 },
  { "GAUSS", kspreadfunc_gauss },
  { "PHI", kspreadfunc_phi },
  { "GAMMADIST", kspreadfunc_gammadist },
  { "BETADIST", kspreadfunc_betadist },
  { "FISHER", kspreadfunc_fisher },
  { "FISHERINV", kspreadfunc_fisherinv },
  { "NORMDIST", kspreadfunc_normdist },
  { "LOGNORMDIST", kspreadfunc_lognormdist },
  { "EXPONDIST", kspreadfunc_expondist },
  { "WEIBULL", kspreadfunc_weibull },
  { "NORMSINV", kspreadfunc_normsinv },
  { "NORMINV", kspreadfunc_norminv },
  { "GAMMALN", kspreadfunc_gammaln },
  { "POISSON", kspreadfunc_poisson },
  { "CONFIDENCE", kspreadfunc_confidence },
  { "TDIST", kspreadfunc_tdist },
  { "FDIST", kspreadfunc_fdist },
  { "CHIDIST", kspreadfunc_chidist },

  // financial
  { "COMPOUND", kspreadfunc_compound },
  { "CONTINUOUS", kspreadfunc_continuous },
  { "EFFECTIVE", kspreadfunc_effective },
  { "NOMINAL", kspreadfunc_nominal },
  { "FV", kspreadfunc_fv },
  { "FV_ANNUITY", kspreadfunc_fv_annuity },
  { "PV", kspreadfunc_pv },
  { "PV_ANNUITY", kspreadfunc_pv_annuity },
  { "ZERO_COUPON", kspreadfunc_zero_coupon },
  { "LEVEL_COUPON", kspreadfunc_level_coupon },
  { "SLN", kspreadfunc_sln },
  { "SYD", kspreadfunc_syd },
  { "DB", kspreadfunc_db },
  { "EURO", kspreadfunc_euro },

  // text
  { "CHAR", kspreadfunc_char },
  { "CLEAN", kspreadfunc_clean },
  { "CODE", kspreadfunc_code },
  { "COMPARE", kspreadfunc_compare }, // KSpread-specific
  { "CONCATENATE", kspreadfunc_concatenate },
  { "DOLLAR", kspreadfunc_dollar },
  { "EXACT", kspreadfunc_exact },
  { "FIND", kspreadfunc_find },
  { "FIXED", kspreadfunc_fixed },
  { "JOIN", kspreadfunc_join }, // obsolete, use CONCATENATE
  { "LEFT", kspreadfunc_left },
  { "LEN", kspreadfunc_len },
  { "LOWER", kspreadfunc_lower },
  { "MID", kspreadfunc_mid },
  { "PROPER", kspreadfunc_proper },
  { "REPLACE", kspreadfunc_replace },
  { "REPT", kspreadfunc_rept },
  { "RIGHT", kspreadfunc_right },
  //{ "SEARCH", kspreadfunc_search },
  { "SLEEK", kspreadfunc_sleek },
  //{ "SUBSTITUTE", kspreadfunc_substitute },
  { "T", kspreadfunc_t },
  //{ "TEXT", kspreadfunc_text },
  { "TOGGLE", kspreadfunc_toggle },
  { "TRIM", kspreadfunc_trim },
  { "UPPER", kspreadfunc_upper },
  { "VALUE", kspreadfunc_value },

  // misc
  { "CELL", kspreadfunc_cell },
  { "SELECT", kspreadfunc_select },

  // compatibility with KSpread < 1.2
  // somehow should marked "obsolete" in the manual
  { "not", kspreadfunc_not },
  { "if", kspreadfunc_if },
  { "ENT", kspreadfunc_INT },
  { "DECHEX", kspreadfunc_dec2hex },
  { "DECBIN", kspreadfunc_dec2bin },
  { "DECOCT", kspreadfunc_dec2oct },

  // end  marker
  { NULL, NULL }
};

static KSModule::Ptr kspreadCreateModule_KSpread( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "kspread" );

  unsigned count = sizeof(funcTab)/sizeof(funcTab[0]);
  for( unsigned i=0; i<count; i++ )
  {
    QString name = funcTab[i].name;
    bool (*function)(KSContext&) = funcTab[i].function;
    if( function ) module->addObject( name, new KSValue(
      new KSBuiltinFunction( module, name, function ) ) );
  }

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

    if ( r->range.left() == r->range.right() 
         && r->range.top() == r->range.bottom() )
    {
      KSpreadCell * cell = r->table->cellAt( r->range.x(), r->range.y() );

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
