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

// built-in engineering functions

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

// Function: DECHEX
bool kspreadfunc_dec2hex( KSContext& context )
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

// Function: DEC2OCT
bool kspreadfunc_dec2oct( KSContext& context )
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

// Function: DEC2BIN
bool kspreadfunc_dec2bin( KSContext& context )
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

// Function: BIN2DEC
bool kspreadfunc_bin2dec( KSContext& context )
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

// Function: BIN2OCT
bool kspreadfunc_bin2oct( KSContext& context )
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

// Function: BIN2HEX
bool kspreadfunc_bin2hex( KSContext& context )
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

// Function: OCT2DEC
bool kspreadfunc_oct2dec( KSContext& context )
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

// Function: OCT2BIN
bool kspreadfunc_oct2bin( KSContext& context )
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

// Function: OCT2HEX
bool kspreadfunc_oct2hex( KSContext& context )
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

// Function: HEX2DEC
bool kspreadfunc_hex2dec( KSContext& context )
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

// Function: HEX2BIN
bool kspreadfunc_hex2bin( KSContext& context )
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

// Function: HEX2OCT
bool kspreadfunc_hex2oct( KSContext& context )
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

// Function: COMPLEX
bool kspreadfunc_complex( KSContext& context )
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

// Function: IMAGINARY
bool kspreadfunc_complex_imag( KSContext& context )
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

// Function: IMREAL
bool kspreadfunc_complex_real( KSContext& context )
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

// Function: IMSUM
bool kspreadfunc_imsum( KSContext& context )
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

// Function: IMSUB
bool kspreadfunc_imsub( KSContext& context )
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

// Function: IMPRODUCT
bool kspreadfunc_improduct( KSContext& context )
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

// Function: IMCONJUGATE
bool kspreadfunc_imconjugate( KSContext& context )
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

// Function: IMARGUMENT
bool kspreadfunc_imargument( KSContext& context )
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

// Function: IMABS
bool kspreadfunc_imabs( KSContext& context )
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

// Function: IMCOS
bool kspreadfunc_imcos( KSContext& context )
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

// Function: IMSIN
bool kspreadfunc_imsin( KSContext& context )
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

// Function: IMLN
bool kspreadfunc_imln( KSContext& context )
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

// Function: IMEXP
bool kspreadfunc_imexp( KSContext& context )
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

// Function: IMSQRT
bool kspreadfunc_imsqrt( KSContext& context )
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

// Function: IMPOWER
bool kspreadfunc_impower( KSContext& context )
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

// Function: IMDIV
bool kspreadfunc_imdiv( KSContext& context )
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

static bool approx_equal (double a, double b)
{
  if ( a == b )
    return TRUE;
  double x = a - b;
  return (x < 0.0 ? -x : x)  <  ((a < 0.0 ? -a : a) * DBL_EPSILON);
}

// Function: DELTA
bool kspreadfunc_delta( KSContext& context )
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

