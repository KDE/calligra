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

// built-in conversion functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>
#include <klocale.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_functions.h"
#include "kspread_util.h"


// prototypes
bool kspreadfunc_arabic( KSContext& context );
bool kspreadfunc_carx( KSContext& context );
bool kspreadfunc_cary( KSContext& context );
bool kspreadfunc_decsex( KSContext& context );
bool kspreadfunc_polr( KSContext& context );
bool kspreadfunc_pola( KSContext& context );
bool kspreadfunc_roman( KSContext& context );
bool kspreadfunc_sexdec( KSContext& context );
bool kspreadfunc_AsciiToChar( KSContext& context );
bool kspreadfunc_CharToAscii( KSContext& context );
bool kspreadfunc_inttobool( KSContext & context );
bool kspreadfunc_booltoint( KSContext & context );
bool kspreadfunc_BoolToString( KSContext& context );
bool kspreadfunc_NumberToString( KSContext& context );

// registers all conversion functions
void KSpreadRegisterConversionFunctions()
{
   KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();
   repo->registerFunction( "ARABIC",  kspreadfunc_arabic );
   repo->registerFunction( "CARX",    kspreadfunc_carx );
   repo->registerFunction( "CARY",    kspreadfunc_cary );
   repo->registerFunction( "DECSEX",  kspreadfunc_decsex );
   repo->registerFunction( "POLR",    kspreadfunc_polr );
   repo->registerFunction( "POLA",    kspreadfunc_pola );
   repo->registerFunction( "ROMAN",   kspreadfunc_roman );
   repo->registerFunction( "SEXDEC",  kspreadfunc_sexdec );
}

// Function: POLR
bool kspreadfunc_polr( KSContext& context )
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

// Function: POLA
bool kspreadfunc_pola( KSContext& context )
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

// Function: CARX
bool kspreadfunc_carx( KSContext& context )
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

// Function: CARY
bool kspreadfunc_cary( KSContext& context )
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

// Function: DECSEX
bool kspreadfunc_decsex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "DECSEX",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  int inter;
  double val=args[0]->doubleValue();
  int hours,minutes,seconds;
  if(val>0)
    inter=1;
  else
    inter=-1;
  hours=inter*(int)(fabs(val));

  double workingVal = (val - (double)hours) * inter;

  /* try to do this without rounding errors */
  workingVal *= 60.0;
  minutes = (int)(floor(workingVal));

  workingVal -= minutes;
  workingVal *= 60;

  seconds = (int)(floor(workingVal));
  workingVal -= seconds;

  /* now we need to try to round up the seconds if that makes sense */
  if (workingVal >= 0.5)
  {
    seconds++;
    while (seconds >= 60)
    {
      minutes++;
      seconds -= 60;
    }

    while (minutes >= 60)
    {
      hours++;
      minutes -= 60;
    }
  }

  QTime _time(hours,minutes,seconds);
  context.setValue( new KSValue(_time));

  return true;
}

// Function: SEXDEC
bool kspreadfunc_sexdec( KSContext& context )
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

// Function: ROMAN
bool kspreadfunc_roman( KSContext& context )
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

// convert single roman character to deciman
// return < 0 if invalid
int kspreadfunc_arabic_helper( QChar c )
{
  switch( c.upper().unicode() )
  {
    case 'M': return 1000;
    case 'D': return 500;
    case 'C': return 100;
    case 'L': return 50;
    case 'X': return 10;
    case 'V': return 5;
    case 'I': return 1;
  }
  return -1;
}

// Function: ARABIC
bool kspreadfunc_arabic( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "ARABIC", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString roman = args[0]->stringValue();
  if( roman.isEmpty() ) return false;

  int val = 0;
  int lastd = 0;
  int d = 0;

  for( unsigned i=0; i < roman.length(); i++ )
  {
    d = kspreadfunc_arabic_helper( roman[i] );

    if( d < 0 )  return false;

    if( lastd < d ) val -= lastd;
      else val += lastd;
    lastd = d;
  }
  if( lastd < d ) val -= lastd;
    else val += lastd;

  context.setValue( new KSValue( val ) );
  return true;
}


// Function: AsciiToChar
bool kspreadfunc_AsciiToChar( KSContext& context )
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

// Function: CharToAscii
bool kspreadfunc_CharToAscii( KSContext& context )
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

// Function: inttobool
bool kspreadfunc_inttobool( KSContext & context )
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

// Function: booltoint
bool kspreadfunc_booltoint( KSContext & context )
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

// Function: BoolToString
bool kspreadfunc_BoolToString( KSContext& context )
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

// Function: NumberToString
bool kspreadfunc_NumberToString( KSContext& context )
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
