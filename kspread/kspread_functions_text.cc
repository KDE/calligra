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


// built-in text functions
// please keep it in alphabetical order

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <qregexp.h>
#include <kdebug.h>
#include <klocale.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_functions.h"
#include "kspread_util.h"
#include "kspread_value.h"
#include "valueparser.h"
#include "valueconverter.h"

// prototypes
bool kspreadfunc_char( KSContext& context );
bool kspreadfunc_clean( KSContext& context );
bool kspreadfunc_code( KSContext& context );
bool kspreadfunc_compare( KSContext& context );
bool kspreadfunc_concatenate( KSContext& context );
bool kspreadfunc_dollar( KSContext& context );
bool kspreadfunc_exact( KSContext& context );
bool kspreadfunc_find( KSContext& context );
bool kspreadfunc_fixed( KSContext& context );
bool kspreadfunc_join( KSContext& context );
bool kspreadfunc_left( KSContext& context );
bool kspreadfunc_len( KSContext& context );
bool kspreadfunc_lower( KSContext& context );
bool kspreadfunc_mid( KSContext& context );
bool kspreadfunc_proper(KSContext & context);
bool kspreadfunc_regexp(KSContext & context);
bool kspreadfunc_regexpre(KSContext & context);
bool kspreadfunc_replace( KSContext& context );
bool kspreadfunc_rept( KSContext& context );
bool kspreadfunc_rot( KSContext& context );
bool kspreadfunc_right( KSContext& context );
bool kspreadfunc_search( KSContext& context );
bool kspreadfunc_sleek( KSContext& context );
bool kspreadfunc_substitute( KSContext& context );
bool kspreadfunc_t( KSContext& context );
bool kspreadfunc_text( KSContext& context );
bool kspreadfunc_toggle( KSContext& context );
bool kspreadfunc_trim(KSContext& context );
bool kspreadfunc_upper( KSContext& context );
bool kspreadfunc_value( KSContext& context );

// registers all text functions
void KSpreadRegisterTextFunctions()
{
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "CHAR",        kspreadfunc_char );
  repo->registerFunction( "CLEAN",       kspreadfunc_clean );
  repo->registerFunction( "CODE",        kspreadfunc_code );
  repo->registerFunction( "COMPARE",     kspreadfunc_compare ); // KSpread-specific
  repo->registerFunction( "CONCATENATE", kspreadfunc_concatenate );
  repo->registerFunction( "DOLLAR",      kspreadfunc_dollar );
  repo->registerFunction( "EXACT",       kspreadfunc_exact );
  repo->registerFunction( "FIND",        kspreadfunc_find );
  repo->registerFunction( "FIXED",       kspreadfunc_fixed );
  repo->registerFunction( "LEFT",        kspreadfunc_left );
  repo->registerFunction( "LEN",         kspreadfunc_len );
  repo->registerFunction( "LOWER",       kspreadfunc_lower );
  repo->registerFunction( "MID",         kspreadfunc_mid );
  repo->registerFunction( "PROPER",      kspreadfunc_proper );
  repo->registerFunction( "REGEXP",      kspreadfunc_regexp );
  repo->registerFunction( "REGEXPRE",    kspreadfunc_regexpre );
  repo->registerFunction( "REPLACE",     kspreadfunc_replace );
  repo->registerFunction( "REPT",        kspreadfunc_rept );
  repo->registerFunction( "ROT",         kspreadfunc_rot ); // KSpread-specific, like OpenOffice's ROT13
  repo->registerFunction( "RIGHT",       kspreadfunc_right );
  repo->registerFunction( "SEARCH",      kspreadfunc_search );
  repo->registerFunction( "SLEEK",       kspreadfunc_sleek );  // KSpread-specific
  repo->registerFunction( "SUBSTITUTE",  kspreadfunc_substitute );
  repo->registerFunction( "T",           kspreadfunc_t );
  repo->registerFunction( "TEXT",        kspreadfunc_text );
  repo->registerFunction( "TOGGLE",      kspreadfunc_toggle ); // KSpread-specific
  repo->registerFunction( "TRIM",        kspreadfunc_trim );
  repo->registerFunction( "UPPER",       kspreadfunc_upper );
  repo->registerFunction( "VALUE",       kspreadfunc_value );
}


// Function: CHAR
bool kspreadfunc_char( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "CHAR", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) &&
       !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
        return false;

  args[0]->cast( KSValue::IntType );
  int val = args[0]->intValue();
  QString str = QChar( val );

  context.setValue( new KSValue( str ) );
  return true;
}

// Function: CLEAN
bool kspreadfunc_clean( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "CLEAN", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    QString str(args[0]->stringValue());
    QString result;
    QChar   c;
    int     i;
    int     l = str.length();

    for (i = 0; i < l; ++i)
    {
      c = str[i];
      if (c.isPrint())
        result += c;
    }

    context.setValue(new KSValue(result));

    return true;
}

// Function: CODE
bool kspreadfunc_code( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "CODE", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString str = args[0]->stringValue();
  if( str.length() <= 0 )
     return false;

  context.setValue( new KSValue( str[0].unicode() ) );
  return true;
}

// Function: COMPARE
bool kspreadfunc_compare( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "COMPARE", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::BoolType, true ) )
    return false;

  int  result = 0;
  bool exact = args[2]->boolValue();

  QString s1 = args[0]->stringValue();
  QString s2 = args[1]->stringValue();

  if (!exact)
    result = s1.lower().localeAwareCompare(s2.lower());
  else
    result = s1.localeAwareCompare(s2);

  if (result < 0)
    result = -1;
  else if (result > 0)
    result = 1;

  context.setValue( new KSValue(result) );
  return true;
}

static bool kspreadfunc_concatenate_helper( KSContext& context, QValueList<KSValue::Ptr>& args, QString& tmp )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();
  QString tmp2;
  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_concatenate_helper( context, (*it)->listValue(), tmp ) )
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

// Function: CONCATENATE
bool kspreadfunc_concatenate( KSContext& context )
{
  QString tmp;
  bool b = kspreadfunc_concatenate_helper( context, context.value()->listValue(), tmp );

  if ( b )
    context.setValue( new KSValue( tmp ) );

  return b;
}

// Function: DOLLAR
bool kspreadfunc_dollar( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "DOLLAR", true ) &&
       !KSUtil::checkArgumentsCount( context, 2, "DOLLAR", true ) )
         return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double value = args[0]->doubleValue();
  int decimals = 2;

  if( KSUtil::checkArgumentsCount( context, 2, "DOLLAR", false ) )
    if ( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
      decimals = args[1]->intValue();

  // if decimals < 0, number is rounded
  if( decimals < 0 )
  {
    decimals = -decimals;
    value = floor( value/pow(10.0,decimals)+0.5 ) * pow(10.0,decimals);
    decimals = 0;
  }

  // do round, because formatMoney doesn't
  value = floor( value * pow(10.0,decimals)+0.5) / pow(10.0,decimals) ;

  // -30 should be displayed as (30)
  QString result = KGlobal::locale()->formatMoney( fabs( value ),
    QString::null, decimals );
  if( value < 0 )
    result = "(" + result + ")";

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: EXACT
bool kspreadfunc_exact( KSContext& context )
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

// Function: FIND
bool kspreadfunc_find( KSContext& context )
{
    QString find_text, within_text;
    int start_num = 1;

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "FIND", true ) &&
         !KSUtil::checkArgumentsCount( context, 3, "FIND", true ) )
           return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
      return false;

    if ( KSUtil::checkArgumentsCount( context, 3, "FIND", false ) )
      if ( KSUtil::checkType( context, args[2], KSValue::IntType, false ) )
         start_num = args[2]->intValue();

    find_text = args[0]->stringValue();
    within_text = args[1]->stringValue();

    // conforms to Excel behaviour
    if( start_num <= 0 ) return false;
    if( start_num > (int)within_text.length() ) return false;

    int pos = within_text.find( find_text, start_num-1 );
    if( pos < 0 ) return false;

    context.setValue( new KSValue( pos + 1 ) );
    return true;
}

// Function: FIXED
bool kspreadfunc_fixed( KSContext& context )
{
  int decimals = 2;
  bool no_commas = FALSE;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "FIXED", true ) &&
       !KSUtil::checkArgumentsCount( context, 2, "FIXED", true ) &&
       !KSUtil::checkArgumentsCount( context, 3, "FIXED", true ) )
         return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( KSUtil::checkArgumentsCount( context, 2, "FIXED", false ) ||
       KSUtil::checkArgumentsCount( context, 3, "FIXED", false ) )
    if ( KSUtil::checkType( context, args[1], KSValue::IntType, false ) )
      decimals = args[1]->intValue();

  if ( KSUtil::checkArgumentsCount( context, 3, "FIXED", false ) )
    if ( KSUtil::checkType( context, args[2], KSValue::BoolType, false ) )
       no_commas = args[2]->boolValue();

  double number = args[0]->doubleValue();

  QString result;

  // unfortunately, we can't just use KLocale::formatNumber because
  // * if decimals < 0, number is rounded
  // * if no_commas is TRUE, thousand separators shouldn't show up

  if( decimals < 0 )
  {
    decimals = -decimals;
    number = floor( number/pow(10.0,decimals)+0.5 ) * pow(10.0,decimals);
    decimals = 0;
  }

  bool neg = number < 0;
  result = QString::number( neg ? -number:number, 'f', decimals );

  int pos = result.find('.');
  if (pos == -1) pos = result.length();
    else result.replace(pos, 1, KGlobal::locale()->decimalSymbol());
  if( !no_commas )
    while (0 < (pos -= 3))
      result.insert(pos, KGlobal::locale()->thousandsSeparator());

  result.prepend( neg ? KGlobal::locale()->negativeSign():
    KGlobal::locale()->positiveSign() );

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: LEFT
bool kspreadfunc_left( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    bool hasNoSecondParam = KSUtil::checkArgumentsCount( context, 1, "left", false );

    if ( !KSUtil::checkArgumentsCount( context, 2, "left", false ) &&
		 !hasNoSecondParam)
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

	int nb;

	if(hasNoSecondParam)
		nb = 1;
	else
	{
	    if( KSUtil::checkType( context, args[1], KSValue::DoubleType, false ) )
	      nb = (int) args[1]->doubleValue();
    	else if( KSUtil::checkType( context, args[1], KSValue::IntType, false ) )
	      nb = args[1]->intValue();
	    else
		  return false;
	}

    QString tmp = args[0]->stringValue().left(nb);
    context.setValue( new KSValue( tmp ) );
    return true;
}

// Function: LEN
bool kspreadfunc_len( KSContext& context )
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

// Function: LOWER
bool kspreadfunc_lower( KSContext& context )
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

// Function: MID
bool kspreadfunc_mid( KSContext& context )
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

	// Excel compatible
	pos--;

    QString tmp = args[0]->stringValue().mid( pos, len );
    context.setValue( new KSValue(tmp));
    return true;
}

// Function: PROPER
bool kspreadfunc_proper(KSContext & context)
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if (args.count() != 1)
    return false;

  QString str;

  if (KSUtil::checkType(context, args[0],
                        KSValue::StringType, true))
  {
    unsigned int i;
    str = args[0]->stringValue().lower();
    QChar f;
    bool  first = true;

    for (i = 0; i < str.length(); ++i)
    {
      if (first)
      {
        f = str[i];
        if (f.isNumber())
          continue;

        f = f.upper();

        str[i] = f;
        first = false;

        continue;
      }

      if (str[i] == ' ' || str[i] == '-')
        first = true;
    }
  }

  context.setValue(new KSValue(str));

  return true;
}

// Function: REGEXP
bool kspreadfunc_regexp(KSContext & context)
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  int ac = args.count ();
  if ((ac < 2) || (ac > 4))
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
    return false;
  if ( (ac >= 3) && ( !KSUtil::checkType( context, args[2], KSValue::StringType, true ) ))
    return false;
  if ( (ac == 4) && (!KSUtil::checkType( context, args[3], KSValue::IntType, true ) ) )
    return false;

  QRegExp exp( args[1]->stringValue() );
  if ( !exp.isValid() )
    return false;

  QString s( args[0]->stringValue() );
  QString defText( (ac >= 3) ? args[2]->stringValue() : QString::null );

  int bkref = (ac == 4) ? args[3]->intValue() : 0;
  if (bkref < 0)
    return false;  //strange back-reference
  
  QString returnValue;
  
  int pos = exp.search( s );
  if (pos == -1)
    returnValue = defText;
  else
    returnValue = exp.cap (bkref);

  context.setValue( new KSValue( returnValue ) );
  return true;
}

// Function: REGEXPRE
bool kspreadfunc_regexpre(KSContext & context)
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if (args.count() != 3)
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::StringType, true ) )
    return false;

  kdDebug() << "Got parameter" << endl;

  QRegExp exp( args[1]->stringValue() );
  //  exp.setWildcard( true );
  if ( !exp.isValid() )
    return false;

  QString s( args[0]->stringValue() );
  QString str( args[2]->stringValue() );

  kdDebug() << "Search: " << args[1]->stringValue() << " in " << s
            << ", Result: " << exp.search( s ) << endl;

  int pos = 0;
  while ( ( pos = exp.search( s, pos ) ) != -1 )
  {
    int i = exp.matchedLength();
    s = s.replace( pos, i, str );
    pos += str.length();
  }

  context.setValue( new KSValue( s ) );
  return true;
}

// Function: REPLACE
bool kspreadfunc_replace( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "REPLACE", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[3], KSValue::StringType, true ) )
    return false;

  QString text = args[0]->stringValue();
  int pos = args[1]->intValue();
  int len = args[2]->intValue();
  QString new_text = args[3]->stringValue();

  if( pos < 0 ) pos = 0;

  QString result = text.replace( pos-1, len, new_text );
  context.setValue( new KSValue( result ) );

  return true;
}

// Function: REPT
bool kspreadfunc_rept( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "REPT", true ) )
    return false;

  if( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  QString s;

  // this is code duplication, can be rewritten once we have KSpreadFormat
  if( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    s = args[0]->stringValue();

  else if( KSUtil::checkType( context, args[0], KSValue::BoolType, false ) )
    s = args[0]->boolValue() ? i18n("True") : i18n("False");

  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    s = KGlobal::locale()->formatNumber( args[0]->doubleValue() );

  else if( KSUtil::checkType( context, args[0], KSValue::TimeType, false ) )
    s = KGlobal::locale()->formatTime( args[0]->timeValue() );

  else if( KSUtil::checkType( context, args[0], KSValue::DateType, false ) )
    s = KGlobal::locale()->formatDate( args[0]->dateValue() );

  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    s = KGlobal::locale()->formatNumber( args[0]->intValue() );

  else return false;

  int nb=(int) args[1]->doubleValue();
  QString result;
  for (int i=0 ;i<nb;i++) result += s;
  context.setValue( new KSValue( result ) );
  return true;
}

// Function: RIGHT
bool kspreadfunc_right( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

	bool hasNoSecondParam = KSUtil::checkArgumentsCount( context, 1, "right", false );

    if ( !KSUtil::checkArgumentsCount( context, 2, "right", false ) &&
		 !hasNoSecondParam)
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    int nb;

	if(hasNoSecondParam)
		nb = 1;
	else
	{
	    if( KSUtil::checkType( context, args[1], KSValue::DoubleType, false ) )
    	  nb = (int) args[1]->doubleValue();
	    else if( KSUtil::checkType( context, args[1], KSValue::IntType, false ) )
    	  nb = args[1]->intValue();
	    else
		  return false;
	}

    QString tmp = args[0]->stringValue().right(nb);
    context.setValue( new KSValue(tmp));
    return true;
}

// Function: ROT
bool kspreadfunc_rot( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ROT", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString text = args[0]->stringValue();

  for( unsigned i=0; i<text.length(); i++ )
  {
    unsigned c = text[i].upper().unicode();
    if( ( c >= 'A' ) && ( c <= 'M' ) )
      text[i] = QChar( text[i].unicode() + 13);
    if( ( c >= 'N' ) && ( c <= 'Z' ) )
      text[i] = QChar( text[i].unicode() - 13);
  }

  context.setValue( new KSValue( text ) );
  return true;
}

// Function: SEARCH
bool kspreadfunc_search( KSContext& context )
{
    QString find_text, within_text;
    int start_num = 1;

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( KSUtil::checkArgumentsCount( context, 3, "SEARCH", false ) )
    {
      if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
        return false;
      start_num = args[2]->intValue();
    }
    else if ( !KSUtil::checkArgumentsCount( context, 2, "SEARCH", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
      return false;

    find_text = args[0]->stringValue();
    within_text = args[1]->stringValue();

    // conforms to Excel behaviour
    if( start_num <= 0 ) return false;
    if( start_num > (int)within_text.length() ) return false;

    // use globbing feature of QRegExp
    QRegExp regex( find_text, false, true );
    int pos = within_text.find( regex, start_num-1 );
    if( pos < 0 ) return false;

    context.setValue( new KSValue( pos + 1 ) );
    return true;
}

// Function: SLEEK
bool kspreadfunc_sleek( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "SLEEK", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    QString str(args[0]->stringValue());
    QString result;
    QChar   c;
    int     i;
    int     l = str.length();

    for (i = 0; i < l; ++i)
    {
      c = str[i];
      if (!c.isSpace())
        result += c;
    }

    context.setValue(new KSValue(result));

    return true;
}

// Function: SUBSTITUTE
bool kspreadfunc_substitute( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  int num = 1;
  bool all = true;

  if ( KSUtil::checkArgumentsCount( context, 4, "SUBSTITUTE", false ) )
  {
    if ( !KSUtil::checkType( context, args[3], KSValue::IntType, true ) )
      return false;
    num = args[3]->intValue();
    all = false;
  }
  else
  if ( !KSUtil::checkArgumentsCount( context, 3, "SUBSTITUTE", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
      return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::StringType, true ) )
      return false;

  QString text = args[0]->stringValue();
  QString old_text = args[1]->stringValue();
  QString new_text = args[2]->stringValue();

  if( num <= 0 ) return false;

  QString result = text;

  int p = result.find( old_text );
  while ( ( p != -1 ) && ( num > 0 ) )
  {
    result.replace( p, old_text.length(), new_text );
    p = result.find( old_text );
    if( !all ) num--;
  }

  context.setValue( new KSValue( result ) );

  return true;


  context.setValue( new KSValue( result ));
  return true;
}

// Function: T
bool kspreadfunc_t( KSContext& context )
{
  // almost identical with the TEXT funcion
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "T", true ) )
    return false;

  KSpreadValue val;
  
  if( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    val.setValue (args[0]->stringValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::TimeType, false ) )
    val.setValue (args[0]->timeValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::DateType, false ) )
    val.setValue (args[0]->dateValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    val.setValue (args[0]->doubleValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    val.setValue (args[0]->intValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::BoolType, false ) )
    val.setValue (args[0]->boolValue());
  
  // temporary hack for value conversion  
  KSpread::ValueParser *parser = new KSpread::ValueParser( KGlobal::locale() );
  KSpread::ValueConverter *converter = new KSpread::ValueConverter( parser );
  val = converter->asString (val);
  delete converter;
  delete parser;

  context.setValue( new KSValue( val.asString() ));
  return true;
}

// Function: TEXT
bool kspreadfunc_text( KSContext& context )
{
  //currently the same as the T function. Well, almost...
  
  QString format_text;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( KSUtil::checkArgumentsCount( context, 2, "TEXT", false ) )
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
      return false;
    format_text = args[1]->stringValue();
  }
  else if ( !KSUtil::checkArgumentsCount( context, 1, "TEXT", true ) )
    return false;

  // not yet Excel-compatible because format_text is omitted

  // currently the same as T
  
  KSpreadValue val;
  
  if( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    val.setValue (args[0]->stringValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    val.setValue (args[0]->doubleValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::TimeType, false ) )
    val.setValue (args[0]->timeValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::DateType, false ) )
    val.setValue (args[0]->dateValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    val.setValue (args[0]->intValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::BoolType, false ) )
    val.setValue (args[0]->boolValue());
    
  // temporary hack for value conversion  
  KSpread::ValueParser *parser = new KSpread::ValueParser( KGlobal::locale() );
  KSpread::ValueConverter *converter = new KSpread::ValueConverter( parser );
  val = converter->asString (val);
  delete converter;
  delete parser;

  context.setValue( new KSValue( val.asString() ));
  return true;
}

// Function: TOGGLE
bool kspreadfunc_toggle( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "TOGGLE", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
      return false;

    QString str(args[0]->stringValue());
    int i;
    int l = str.length();

    for (i = 0; i < l; ++i)
    {
      QChar c = str[i];
      QChar lc = c.lower();
      QChar uc = c.upper();

      if (c == lc) // it is in lowercase
        str[i] = c.upper();
      else if (c == uc) // it is in uppercase
        str[i] = c.lower();
    }

    context.setValue( new KSValue( str ) );

    return true;
}

// Function: TRIM
bool kspreadfunc_trim(KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "trim", true ) )
        return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        return false;
    QString text=args[0]->stringValue().simplifyWhiteSpace();
    context.setValue( new KSValue(text));
    return true;
}

// Function: UPPER
bool kspreadfunc_upper( KSContext& context )
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

// Function: VALUE
bool kspreadfunc_value( KSContext& context )
{
  //currently the same as the N function
  
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "VALUE", true ) )
    return false;

  KSpreadValue val;
  
  if( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    val.setValue (args[0]->stringValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    val.setValue (args[0]->doubleValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::TimeType, false ) )
    val.setValue (args[0]->timeValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::DateType, false ) )
    val.setValue (args[0]->dateValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    val.setValue (args[0]->intValue());
  
  else if( KSUtil::checkType( context, args[0], KSValue::BoolType, false ) )
    val.setValue (args[0]->boolValue());
    
  // temporary hack for value conversion  
  KSpread::ValueParser *parser = new KSpread::ValueParser( KGlobal::locale() );
  KSpread::ValueConverter *converter = new KSpread::ValueConverter( parser );
  val = converter->asString (val);
  delete converter;
  delete parser;

  context.setValue( new KSValue( val.asFloat() ));
  
  return true;
}
