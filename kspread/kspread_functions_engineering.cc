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

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <qmap.h>

#include <kdebug.h>
#include <klocale.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_functions.h"
#include "kspread_util.h"


// prototypes (sort alphabetically)
bool kspreadfunc_base( KSContext& context );
bool kspreadfunc_besseli( KSContext& context );
bool kspreadfunc_besselj( KSContext& context );
bool kspreadfunc_besselk( KSContext& context );
bool kspreadfunc_bessely( KSContext& context );
bool kspreadfunc_bin2dec( KSContext& context );
bool kspreadfunc_bin2oct( KSContext& context );
bool kspreadfunc_bin2hex( KSContext& context );
bool kspreadfunc_complex( KSContext& context );
bool kspreadfunc_complex_imag( KSContext& context );
bool kspreadfunc_complex_real( KSContext& context );
bool kspreadfunc_convert( KSContext& context );
bool kspreadfunc_dec2hex( KSContext& context );
bool kspreadfunc_dec2oct( KSContext& context );
bool kspreadfunc_dec2bin( KSContext& context );
bool kspreadfunc_delta( KSContext& context );
bool kspreadfunc_erf( KSContext& context );
bool kspreadfunc_erfc( KSContext& context );
bool kspreadfunc_gestep( KSContext& context );
bool kspreadfunc_hex2dec( KSContext& context );
bool kspreadfunc_hex2bin( KSContext& context );
bool kspreadfunc_hex2oct( KSContext& context );
bool kspreadfunc_imabs( KSContext& context );
bool kspreadfunc_imargument( KSContext& context );
bool kspreadfunc_imconjugate( KSContext& context );
bool kspreadfunc_imcos( KSContext& context );
bool kspreadfunc_imdiv( KSContext& context );
bool kspreadfunc_imexp( KSContext& context );
bool kspreadfunc_imln( KSContext& context );
bool kspreadfunc_impower( KSContext& context );
bool kspreadfunc_improduct( KSContext& context );
bool kspreadfunc_imsin( KSContext& context );
bool kspreadfunc_imsqrt( KSContext& context );
bool kspreadfunc_imsub( KSContext& context );
bool kspreadfunc_imsum( KSContext& context );
bool kspreadfunc_oct2dec( KSContext& context );
bool kspreadfunc_oct2bin( KSContext& context );
bool kspreadfunc_oct2hex( KSContext& context );

// registers all engineering functions
void KSpreadRegisterEngineeringFunctions()
{
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "BASE",        kspreadfunc_base );    // KSpread-specific, like in Quattro-Pro
  repo->registerFunction( "BESSELI",     kspreadfunc_besseli );
  repo->registerFunction( "BESSELJ",     kspreadfunc_besselj );
  repo->registerFunction( "BESSELK",     kspreadfunc_besselk );
  repo->registerFunction( "BESSELY",     kspreadfunc_bessely );
  repo->registerFunction( "BIN2DEC",     kspreadfunc_bin2dec );
  repo->registerFunction( "BIN2OCT",     kspreadfunc_bin2oct );
  repo->registerFunction( "BIN2HEX",     kspreadfunc_bin2hex );
  repo->registerFunction( "COMPLEX",     kspreadfunc_complex );
  repo->registerFunction( "CONVERT",     kspreadfunc_convert );
  repo->registerFunction( "DEC2HEX",     kspreadfunc_dec2hex );
  repo->registerFunction( "DEC2BIN",     kspreadfunc_dec2bin );
  repo->registerFunction( "DEC2OCT",     kspreadfunc_dec2oct );
  repo->registerFunction( "DELTA",       kspreadfunc_delta );
  repo->registerFunction( "ERF",         kspreadfunc_erf );
  repo->registerFunction( "ERFC",        kspreadfunc_erfc );
  repo->registerFunction( "GESTEP",      kspreadfunc_gestep );
  repo->registerFunction( "HEX2BIN",     kspreadfunc_hex2bin );
  repo->registerFunction( "HEX2DEC",     kspreadfunc_hex2dec );
  repo->registerFunction( "HEX2OCT",     kspreadfunc_hex2oct );
  repo->registerFunction( "IMABS",       kspreadfunc_imabs );
  repo->registerFunction( "IMAGINARY",   kspreadfunc_complex_imag );
  repo->registerFunction( "IMARGUMENT",  kspreadfunc_imargument );
  repo->registerFunction( "IMCONJUGATE", kspreadfunc_imconjugate );
  repo->registerFunction( "IMCOS",       kspreadfunc_imcos );
  repo->registerFunction( "IMDIV",       kspreadfunc_imdiv );
  repo->registerFunction( "IMEXP",       kspreadfunc_imexp );
  repo->registerFunction( "IMLN",        kspreadfunc_imln );
  repo->registerFunction( "IMPOWER",     kspreadfunc_impower );
  repo->registerFunction( "IMPRODUCT",   kspreadfunc_improduct );
  repo->registerFunction( "IMREAL",      kspreadfunc_complex_real );
  repo->registerFunction( "IMSIN",       kspreadfunc_imsin );
  repo->registerFunction( "IMSQRT",      kspreadfunc_imsqrt );
  repo->registerFunction( "IMSUB",       kspreadfunc_imsub );
  repo->registerFunction( "IMSUM",       kspreadfunc_imsum );
  repo->registerFunction( "OCT2BIN",     kspreadfunc_oct2bin );
  repo->registerFunction( "OCT2DEC",     kspreadfunc_oct2dec );
  repo->registerFunction( "OCT2HEX",     kspreadfunc_oct2hex );
}

// Function: BASE
bool kspreadfunc_base( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  int base = 10;
  int prec = 0;

  if ( KSUtil::checkArgumentsCount( context, 3, "BASE", false ) )
  {
    if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) ) return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) ) return false;
    base = args[1]->intValue();
    prec = args[2]->intValue();
  }
  else
  if ( KSUtil::checkArgumentsCount( context, 2, "BASE", false ) )
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) ) return false;
    base = args[1]->intValue();
  }
  else
  if ( !KSUtil::checkArgumentsCount( context, 1, "BASE", true ) )
    return false;


  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) ) return false;

  if( ( base < 2 ) || ( base > 36 ) ) return false;
  if( prec < 0 ) prec = 2;

  double value = args[0]->doubleValue();
  QString result = QString::number( (int)value, base );

  if( prec > 0 )
  {
    result += "."; value = value - (int)value;

    int ix;
    for( int i = 0; i < prec; i++ )
    {
      ix = (int) value * base;

kdDebug() << "value " << value << "  ix " << ix << endl;

      result += "0123456789abcdefghijklmnopqrstuvwxyz"[ix];
      value = base * (value - (double)ix/base);
    }
  }

  context.setValue( new KSValue( result.upper() ) );

  return true;
}

/*
 *
 * The code for calculating Bessel functions is taken
 * from CCMATH, a mathematics library source.code.
 *
 * Original copyright follows:
 *
 *  Copyright (C)  2000   Daniel A. Atkinson    All rights reserved.
 *  This code may be redistributed under the terms of the GNU library
 *  public license (LGPL).
 */

static double ccmath_gaml(double x)
{ double g,h;
  for(g=1.; x<30. ;g*=x,x+=1.); h=x*x;
  g=(x-.5)*log(x)-x+.918938533204672-log(g);
  g+=(1.-(1./6.-(1./3.-1./(4.*h))/(7.*h))/(5.*h))/(12.*x);
  return g;
}

static double ccmath_psi(int m)
{ double s= -.577215664901533; int k;
  for(k=1; k<m ;++k) s+=1./k;
  return s;
}

static double ccmath_ibes(double v,double x)
{ double y,s,t,tp; int p,m;
  y=x-9.; if(y>0.) y*=y; tp=v*v*.2+25.;
  if(y<tp){ x/=2.; m=x;
    if(x>0.) s=t=exp(v*log(x)-ccmath_gaml(v+1.));
    else{ if(v>0.) return 0.; else if(v==0.) return 1.;}
    for(p=1,x*=x;;++p){ t*=x/(p*(v+=1.)); s+=t;
      if(p>m && t<1.e-13*s) break;
     }
   }
  else{ double u,a0=1.57079632679490;
    s=t=1./sqrt(x*a0); x*=2.; u=0.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>=tp) break;
      if(!(p&1)) s+=t; else u-=t;
     }
    x/=2.; s=cosh(x)*s+sinh(x)*u;
   }
  return s;
}

static double ccmath_kbes(double v,double x)
{ double y,s,t,tp,f,a0=1.57079632679490;
  int p,k,m;
  if(x==0.) return HUGE_VAL;
  y=x-10.5; if(y>0.) y*=y; tp=25.+.185*v*v;
  if(y<tp && modf(v+.5,&t)!=0.){ y=1.5+.5*v;
    if(x<y){ x/=2.; m=x; tp=t=exp(v*log(x)-ccmath_gaml(v+1.));
      if(modf(v,&y)==0.){ k=y; tp*=v;
        f=2.*log(x)-ccmath_psi(1)-ccmath_psi(k+1);
        t/=2.; if(!(k&1)) t= -t; s=f*t;
        for(p=1,x*=x;;++p){ f-=1./p+1./(v+=1.);
          t*=x/(p*v); s+=(y=t*f);
          if(p>m && fabs(y)<1.e-14) break; }
        if(k>0){ x= -x; s+=(t=1./(tp*2.));
          for(p=1,--k; k>0 ;++p,--k) s+=(t*=x/(p*k)); }
       }
      else{ f=1./(t*v*2.); t*=a0/sin(2.*a0*v); s=f-t;
        for(p=1,x*=x,tp=v;;++p){
          t*=x/(p*(v+=1.)); f*= -x/(p*(tp-=1.));
          s+=(y=f-t); if(p>m && fabs(y)<1.e-14) break; }
       }
     }
    else{ double tq,h,w,z,r;
      t=12./pow(x,.333); k=t*t; y=2.*(x+k);
      m=v; v-=m; tp=v*v-.25; v+=1.; tq=v*v-.25;
      for(s=h=1.,r=f=z=w=0.; k>0 ;--k,y-=2.){
        t=(y*h-(k+1)*z)/(k-1-tp/k); z=h; f+=(h=t);
        t=(y*s-(k+1)*w)/(k-1-tq/k); w=s; r+=(s=t);  }
      t=sqrt(a0/x)*exp(-x); s*=t/r; h*=t/f; x/=2.; if(m==0) s=h;
      for(k=1; k<m ;++k){ t=v*s/x+h; h=s; s=t; v+=1.;}
     }
   }
  else{ s=t=sqrt(a0/x); x*=2.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>=tp) break; s+=t; }
    s*=exp(-x/2.);
   }
  return s;
}

static double ccmath_jbes(double v,double x)
{ double y,s,t,tp; int p,m;
  y=x-8.5; if(y>0.) y*=y; tp=v*v/4.+13.69;
  if(y<tp){ x/=2.; m=x;
    if(x>0.) s=t=exp(v*log(x)-ccmath_gaml(v+1.));
    else{ if(v>0.) return 0.; else if(v==0.) return 1.;}
    for(p=1,x*= -x;;++p){ t*=x/(p*(v+=1.)); s+=t;
      if(p>m && fabs(t)<1.e-13) break;
     }
   }
  else{ double u,a0=1.57079632679490;
    s=t=1./sqrt(x*a0); x*=2.; u=0.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>=tp) break;
      if(!(p&1)){ t= -t; s+=t;} else u-=t;
     }
    y=x/2.-(v+.5)*a0; s=cos(y)*s+sin(y)*u;
   }
  return s;
}

static double ccmath_nbes(double v,double x)
{ double y,s,t,tp,u,f,a0=3.14159265358979;
  int p,k,m;
  y=x-8.5; if(y>0.) y*=y; tp=v*v/4.+13.69;
  if(y<tp){ if(x==0.) return HUGE_VAL;
    x/=2.; m=x; u=t=exp(v*log(x)-ccmath_gaml(v+1.));
    if(modf(v,&y)==0.){ k=y; u*=v;
      f=2.*log(x)-ccmath_psi(1)-ccmath_psi(k+1);
      t/=a0; x*= -x; s=f*t;
      for(p=1;;++p){ f-=1./p+1./(v+=1.);
        t*=x/(p*v); s+=(y=t*f); if(p>m && fabs(y)<1.e-13) break; }
      if(k>0){ x= -x; s-=(t=1./(u*a0));
        for(p=1,--k; k>0 ;++p,--k) s-=(t*=x/(p*k)); }
     }
    else{ f=1./(t*v*a0); t/=tan(a0*v); s=t-f;
      for(p=1,x*=x,u=v;;++p){
        t*= -x/(p*(v+=1.)); f*=x/(p*(u-=1.));
        s+=(y=t-f); if(p>m && fabs(y)<1.e-13) break; }
     }
   }
  else{ x*=2.; s=t=2./sqrt(x*a0); u=0.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>tp) break;
      if(!(p&1)){ t= -t; s+=t;} else u+=t;
     }
    y=(x-(v+.5)*a0)/2.; s=sin(y)*s+cos(y)*u;
   }
  return s;
}


/* ---------- end of CCMATH code ---------- */

// Function: BESSELI
bool kspreadfunc_besseli( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "BESSELI",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double y = args[1]->doubleValue();

  context.setValue( new KSValue( ccmath_ibes( y, x ) ) );

  return true;
}

// Function: BESSELJ
bool kspreadfunc_besselj( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "BESSELJ",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double y = args[1]->doubleValue();

  context.setValue( new KSValue( ccmath_jbes( y, x ) ) );

  return true;
}

// Function: BESSELK
bool kspreadfunc_besselk( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "BESSELK",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double y = args[1]->doubleValue();

  context.setValue( new KSValue( ccmath_kbes( y, x ) ) );

  return true;
}

// Function: BESSELY
bool kspreadfunc_bessely( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,2, "BESSELY",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double y = args[1]->doubleValue();

  context.setValue( new KSValue( ccmath_nbes( y, x ) ) );

  return true;
}

// Function: DEC2HEX
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

  QString str;
  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    str = args[0]->stringValue();
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    str = QString::number( args[0]->intValue() );
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    str = QString::number( args[0]->intValue() );
  else
    return false;

  bool ok = true;
  long val = str.toLong( &ok, 2 );
  if( !ok )
    context.setValue( new KSValue( i18n("Err") ));
  else
    context.setValue( new KSValue( val ) );

  return true;
}

// Function: BIN2OCT
bool kspreadfunc_bin2oct( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "BIN2OCT", true ) )
    return false;

  QString str;
  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    str = args[0]->stringValue();
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    str = QString::number( args[0]->intValue() );
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    str = QString::number( args[0]->intValue() );
  else
    return false;

  bool ok = true;
  long val = str.toLong( &ok, 2 );
  if( !ok )
    context.setValue( new KSValue( i18n("Err") ));
  else
    context.setValue( new KSValue( QString::number( val, 8 ) ) );

  return true;
}

// Function: BIN2HEX
bool kspreadfunc_bin2hex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "BIN2HEX", true ) )
    return false;

  QString str;
  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    str = args[0]->stringValue();
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    str = QString::number( args[0]->intValue() );
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    str = QString::number( args[0]->intValue() );
  else
    return false;

  bool ok = true;
  long val = str.toLong( &ok, 2 );
  if( !ok )
    context.setValue( new KSValue( i18n("Err") ));
  else
    context.setValue( new KSValue( QString::number( val, 16 ).upper() ) );

  return true;
}

// Function: OCT2DEC
bool kspreadfunc_oct2dec( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "OCT2DEC", true ) )
    return false;

  QString str;
  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    str = args[0]->stringValue();
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    str = QString::number( args[0]->intValue() );
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    str = QString::number( args[0]->intValue() );
  else
    return false;

  bool ok = true;
  long val = str.toLong( &ok, 8 );
  if( !ok )
    context.setValue( new KSValue( i18n("Err") ));
  else
    context.setValue( new KSValue( val ) );

  return true;
}

// Function: OCT2BIN
bool kspreadfunc_oct2bin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "OCT2BIN", true ) )
    return false;

  QString str;
  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    str = args[0]->stringValue();
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    str = QString::number( args[0]->intValue() );
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    str = QString::number( args[0]->intValue() );
  else
    return false;

  bool ok = true;
  long val = str.toLong( &ok, 8 );
  if( !ok )
    context.setValue( new KSValue( i18n("Err") ));
  else
    context.setValue( new KSValue( QString::number( val, 2 ) ) );

  return true;
}

// Function: OCT2HEX
bool kspreadfunc_oct2hex( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "OCT2HEX", true ) )
    return false;

  QString str;
  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    str = args[0]->stringValue();
  else if( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
    str = QString::number( args[0]->intValue() );
  else if( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
    str = QString::number( args[0]->intValue() );
  else
    return false;

  bool ok = true;
  long val = str.toLong( &ok, 8 );
  if( !ok )
    context.setValue( new KSValue( i18n("Err") ));
  else
    context.setValue( new KSValue( QString::number( val, 16 ).upper() ) );

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
        context.setValue( new KSValue( i18n("Err") ));
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
        context.setValue( new KSValue( i18n("Err") ));
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
        context.setValue( new KSValue( i18n("Err") ));
  else
        {
        tmp=tmp.setNum(val,8);
        context.setValue( new KSValue(tmp));
        }

  return true;
}


// check if unit may contain prefix, for example "kPa" is "Pa" with
// return prefix factor found in unit, or 1.0 for no prefix
// also modify the unit, i.e stripping the prefix from it
// example: "kPa" will return 1e3 and change unit into "Pa"
static double kspread_convert_prefix( QMap<QString,double> map, QString& unit )
{
  if( map.contains( unit ) )
    return 1.0;

  // initialize prefix mapping if necessary
  static QMap<char,double> prefixMap;
  if( prefixMap.isEmpty() )
  {
     prefixMap[ 'E' ] = 1e18;  //  exa
     prefixMap[ 'P' ] = 1e15;  //  peta
     prefixMap[ 'T' ] = 1e12;  // tera
     prefixMap[ 'G' ] = 1e9;   // giga
     prefixMap[ 'M' ] = 1e6;   // mega
     prefixMap[ 'k' ] = 1e3;   // kilo
     prefixMap[ 'h' ] = 1e2;   // hecto
     prefixMap[ 'e' ] = 1e1;   // dekao
     prefixMap[ 'd' ] = 1e1;   // deci
     prefixMap[ 'c' ] = 1e2;   // centi
     prefixMap[ 'm' ] = 1e3;   // milli
     prefixMap[ 'u' ] = 1e6;   // micro
     prefixMap[ 'n' ] = 1e9;   // nano
     prefixMap[ 'p' ] = 1e12;  // pico
     prefixMap[ 'f' ] = 1e15;  // femto
     prefixMap[ 'a' ] = 1e18;  // atto
  }

  // check for possible prefix
  char prefix = unit[0].latin1();
  if( prefixMap.contains( prefix ) )
  {
    unit.remove( 0, 1 );
    return prefixMap[ prefix ];
  }

  // fail miserably
  return 0.0;
}

static bool kspread_convert_mass( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> massMap;

  // first-time initialization
  if( massMap.isEmpty() )
  {
    massMap[ "g" ]        = 1.0; // Gram (the reference )
    massMap[ "sg" ]       = 6.8522050005347800E-05; // Pieces
    massMap[ "lbm" ]      = 2.2046229146913400E-03; // Pound
    massMap[ "u" ]        = 6.0221370000000000E23; // U (atomic mass)
    massMap[ "ozm" ]      = 3.5273971800362700E-02; // Ounce
    massMap[ "stone" ]    = 1.574730e-04; // Stone
    massMap[ "ton" ]      = 1.102311e-06; // Ton
    massMap[ "grain" ]    = 1.543236E01;  // Grain
    massMap[ "pweight" ]  = 7.054792E-01; // Pennyweight
    massMap[ "hweight" ]  = 1.968413E-05; // Hundredweight
    massMap[ "shweight" ] = 2.204623E-05; // Shorthundredweight
    massMap[ "brton" ]    = 9.842065E-07; // Gross Registered Ton
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( massMap, fromU );
  double toPrefix = kspread_convert_prefix( massMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !massMap.contains( fromU ) ) return false;
  if( !massMap.contains( toU ) ) return false;

  result = value * fromPrefix * massMap[toU] / (massMap[fromU] * toPrefix);

  return true;
}


static bool kspread_convert_distance( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> distanceMap;

  // first-time initialization
  if( distanceMap.isEmpty() )
  {
    distanceMap[ "m" ]         = 1.0;  // meter (the reference)
    distanceMap[ "in" ]        = 1.0 / 0.0254; // inch
    distanceMap[ "ft" ]        = 1.0 / (12.0 * 0.0254); // feet
    distanceMap[ "yd" ]        = 1.0 / (3.0 * 12.0 * 0.0254); // yar
    distanceMap[ "mi" ]        = 6.2137119223733397e-4; // mile
    distanceMap[ "Nmi" ]       = 5.3995680345572354e-04; // nautical mile
    distanceMap[ "ang" ]       = 1e10; // Angstrom
    distanceMap[ "parsec" ]    = 3.240779e-17; // Parsec
    distanceMap[ "lightyear" ] = 1.057023455773293e-16; // lightyear
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( distanceMap, fromU );
  double toPrefix = kspread_convert_prefix( distanceMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !distanceMap.contains( fromU ) ) return false;
  if( !distanceMap.contains( toU ) ) return false;

  result = value * fromPrefix * distanceMap[toU] / (distanceMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_pressure( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> pressureMap;

  // first-time initialization
  if( pressureMap.isEmpty() )
  {
    pressureMap[ "Pa" ] = 1.0;
    pressureMap[ "atm" ] = 0.9869233e-5;
    pressureMap[ "mmHg" ] = 0.00750061708;
    pressureMap[ "psi" ] = 1 / 6894.754;
    pressureMap[ "Torr" ] = 1 / 133.32237;
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( pressureMap, fromU );
  double toPrefix = kspread_convert_prefix( pressureMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !pressureMap.contains( fromU ) ) return false;
  if( !pressureMap.contains( toU ) ) return false;

  result = value * fromPrefix * pressureMap[toU] / (pressureMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_force( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> forceMap;

  // first-time initialization
  if( forceMap.isEmpty() )
  {
    forceMap[ "N" ]      = 1.0; // Newton (reference)
    forceMap[ "dyn" ]    = 1.0e5; // dyn
    forceMap[ "pond" ]   = 1.019716e2; // pond
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( forceMap, fromU );
  double toPrefix = kspread_convert_prefix( forceMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !forceMap.contains( fromU ) ) return false;
  if( !forceMap.contains( toU ) ) return false;

  result = value * fromPrefix * forceMap[toU] / (forceMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_energy( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> energyMap;

  // first-time initialization
  if( energyMap.isEmpty() )
  {
    energyMap[ "J" ]   = 1.0; // Joule (the reference)
    energyMap[ "e" ]   = 1.0e7; //erg
    energyMap[ "c" ]   = 0.239006249473467; // thermodynamical calorie
    energyMap[ "cal" ] = 0.238846190642017; // calorie
    energyMap[ "eV" ]  = 6.241457e+18; // electronvolt
    energyMap[ "HPh" ] = 3.72506111e-7; // horsepower-hour
    energyMap[ "Wh" ]  = 0.000277778; // watt-hour
    energyMap[ "flb" ] = 23.73042222;
    energyMap[ "BTU" ] = 9.47815067349015e-4; // British Thermal Unit
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( energyMap, fromU );
  double toPrefix = kspread_convert_prefix( energyMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !energyMap.contains( fromU ) ) return false;
  if( !energyMap.contains( toU ) ) return false;

  result = value * fromPrefix * energyMap[toU] / (energyMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_power( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> powerMap;

  // first-time initialization
  if( powerMap.isEmpty() )
  {
    powerMap[ "W" ]   = 1.0; // Watt (the reference)
    powerMap[ "HP" ]  = 1.341022e-3; // Horsepower
    powerMap[ "PS" ]  = 1.359622e-3; // Pferdestärke (German)
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( powerMap, fromU );
  double toPrefix = kspread_convert_prefix( powerMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !powerMap.contains( fromU ) ) return false;
  if( !powerMap.contains( toU ) ) return false;

  result = value * fromPrefix * powerMap[toU] / (powerMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_magnetism( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> magnetismMap;

  // first-time initialization
  if( magnetismMap.isEmpty() )
  {
    magnetismMap[ "T" ]   = 1.0;    // Tesla (the reference)
    magnetismMap[ "ga" ]   = 1.0e4; // Gauss
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( magnetismMap, fromU );
  double toPrefix = kspread_convert_prefix( magnetismMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !magnetismMap.contains( fromU ) ) return false;
  if( !magnetismMap.contains( toU ) ) return false;

  result = value * fromPrefix * magnetismMap[toU] / (magnetismMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_temperature( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> tempFactorMap;
  static QMap<QString, double> tempOffsetMap;

  // first-time initialization
  if( tempFactorMap.isEmpty() || tempOffsetMap.isEmpty() )
  {
    tempFactorMap[ "C" ] = 1.0; tempOffsetMap[ "C" ] = 0.0;
    tempFactorMap[ "F" ] = 5.0/9.0; tempOffsetMap[ "F" ] = -32.0;
    tempFactorMap[ "K" ] = 1.0; tempOffsetMap[ "K" ] = -273.15;
  }

  if( !tempFactorMap.contains( fromUnit ) ) return false;
  if( !tempOffsetMap.contains( fromUnit ) ) return false;
  if( !tempFactorMap.contains( toUnit ) ) return false;
  if( !tempOffsetMap.contains( toUnit ) ) return false;

  result = ( value + tempOffsetMap[ fromUnit ] )* tempFactorMap[ fromUnit ];
  result = ( result / tempFactorMap[ toUnit ] ) - tempOffsetMap[ toUnit ];

  return true;
}

static bool kspread_convert_volume( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> volumeMap;

  // first-time initialization
  if( volumeMap.isEmpty() )
  {
    volumeMap[ "l" ]      = 1.0; // Liter (the reference)
    volumeMap[ "tsp" ]    = 202.84; // teaspoon
    volumeMap[ "tbs" ]    = 67.6133333333333; // tablespoon
    volumeMap[ "oz" ]     = 33.8066666666667; // ounce liquid
    volumeMap[ "cup" ]    = 4.22583333333333; // cup
    volumeMap[ "pt" ]     = 2.11291666666667; // pint
    volumeMap[ "qt" ]     = 1.05645833333333; // quart
    volumeMap[ "gal" ]    = 0.26411458333333; // gallone
    volumeMap[ "m3" ]     = 1.0e-3; // cubic meter
    volumeMap[ "mi3" ]    = 2.3991275857892772e-13; // cubic mile
    volumeMap[ "Nmi3" ]   = 1.5742621468581148e-13; // cubic Nautical mile
    volumeMap[ "in3" ]    = 6.1023744094732284e1; // cubic inch
    volumeMap[ "ft3" ]    = 3.5314666721488590e-2; // cubic foot
    volumeMap[ "yd3" ]    = 1.3079506193143922; // cubic yard
    volumeMap[ "barrel" ] = 6.289811E-03; // barrel
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( volumeMap, fromU );
  double toPrefix = kspread_convert_prefix( volumeMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !volumeMap.contains( fromU ) ) return false;
  if( !volumeMap.contains( toU ) ) return false;

  result = value * fromPrefix * volumeMap[toU] / (volumeMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_area( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> areaMap;

  // first-time initialization
  if( areaMap.isEmpty() )
  {
    areaMap[ "m2" ]   = 1.0; // square meter (the reference)
    areaMap[ "mi2" ]  = 3.8610215854244585e-7; // square mile
    areaMap[ "Nmi2" ] = 2.9155334959812286e-7; // square Nautical mile
    areaMap[ "in2" ]  = 1.5500031000062000e3; // square inch
    areaMap[ "ft2" ]  = 1.0763910416709722e1; // square foot
    areaMap[ "yd2" ]  = 1.0936132983377078; // square yard
    areaMap[ "acre" ] = 4.046856e3; // acre
    areaMap[ "ha" ]   = 1.0e4; // hectare
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( areaMap, fromU );
  double toPrefix = kspread_convert_prefix( areaMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !areaMap.contains( fromU ) ) return false;
  if( !areaMap.contains( toU ) ) return false;

  result = value * fromPrefix * areaMap[toU] / (areaMap[fromU] * toPrefix);

  return true;
}

static bool kspread_convert_speed( const QString& fromUnit,
  const QString& toUnit, double value, double& result )
{
  static QMap<QString, double> speedMap;

  // first-time initialization
  if( speedMap.isEmpty() )
  {
    speedMap[ "m/s" ] = 1.0; // meters per second (the reference)
    speedMap[ "m/h" ] = 3.6e3; // meters per hour
    speedMap[ "mph" ] = 2.2369362920544023; // miles per hour
    speedMap[ "kn" ]  = 1.9438444924406048; // knot
  }

  QString fromU = fromUnit;
  QString toU = toUnit;
  double fromPrefix = kspread_convert_prefix( speedMap, fromU );
  double toPrefix = kspread_convert_prefix( speedMap, toU );
  if( fromPrefix == 0.0 ) return false;
  if( toPrefix == 0.0 ) return false;
  if( !speedMap.contains( fromU ) ) return false;
  if( !speedMap.contains( toU ) ) return false;

  result = value * fromPrefix * speedMap[toU] / (speedMap[fromU] * toPrefix);

  return true;
}


// Function: CONVERT
bool kspreadfunc_convert( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "CONVERT", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::StringType, true ) )
    return false;

  double value = args[0]->doubleValue();
  QString fromUnit = args[1]->stringValue();
  QString toUnit = args[2]->stringValue();

  double result = value;

  if( !kspread_convert_mass( fromUnit, toUnit, value, result ) )
    if( !kspread_convert_distance( fromUnit, toUnit, value, result ) )
      if( !kspread_convert_pressure( fromUnit, toUnit, value, result ) )
        if( !kspread_convert_force( fromUnit, toUnit, value, result ) )
          if( !kspread_convert_energy( fromUnit, toUnit, value, result ) )
            if( !kspread_convert_power( fromUnit, toUnit, value, result ) )
              if( !kspread_convert_magnetism( fromUnit, toUnit, value, result ) )
                if( !kspread_convert_temperature( fromUnit, toUnit, value, result ) )
                  if( !kspread_convert_volume( fromUnit, toUnit, value, result ) )
                    if( !kspread_convert_area( fromUnit, toUnit, value, result ) )
                      if( !kspread_convert_speed( fromUnit, toUnit, value, result ) )
                        return false;

  context.setValue( new KSValue( result ) );

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

static bool approx_equal_delta (double a, double b)
{
  if ( a == b )
    return TRUE;
  double x = a - b;
  return (x < 0.0 ? -x : x)  <  ((a < 0.0 ? -a : a) * DBL_EPSILON);
}

// Function: DELTA
bool kspreadfunc_delta( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  short result;
  double val1 = 0.0;
  double val2 = 0.0;

  if ( !KSUtil::checkArgumentsCount( context, 2, "DELTA", false ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 1, "DELTA", true ) )
      return false;
  }
  else
  {
    kdDebug() << "Here2" << endl;

    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, false ) )
    {
      if ( !KSUtil::checkType( context, args[1], KSValue::BoolType, true ) )
        return false;
      val2 = ( args[1]->boolValue() ? 1.0 : 0.0 );
    }
    else
      val2 = args[1]->doubleValue();
  }

  kdDebug() << "Here1" << endl;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
  {
    if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
      return false;
    val1 = ( args[0]->boolValue() ? 1.0 : 0.0 );
  }
  else
    val1 = args[0]->doubleValue();

  kdDebug() << "Here3" << endl;

  if ( approx_equal_delta( val1, val2 ) )
    result = 1;
  else
    result = 0;

  kdDebug() << "Here4" << endl;
  context.setValue( new KSValue( result ) );

  return true;
}

// Function: ERF
bool kspreadfunc_erf( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  double result = 0.0;

  if ( KSUtil::checkArgumentsCount( context, 2, "ERF", false ) )
  {
    double lower_limit = args[0]->doubleValue();
    double upper_limit = args[1]->doubleValue();
    result = erf( upper_limit ) - erf( lower_limit );
  }
  else
  if ( KSUtil::checkArgumentsCount( context, 1, "ERF", false ) )
  {
    double limit = args[0]->doubleValue();
    result = erf( limit );
  }
  else
    return false;

  context.setValue( new KSValue( result ) );

  return true;
}

// Function: ERFC
bool kspreadfunc_erfc( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  double result = 0.0;

  if ( KSUtil::checkArgumentsCount( context, 2, "ERFC", false ) )
  {
    double lower_limit = args[0]->doubleValue();
    double upper_limit = args[1]->doubleValue();
    result = erfc( upper_limit ) - erfc( lower_limit );
  }
  else
  if ( KSUtil::checkArgumentsCount( context, 1, "ERFC", false ) )
  {
    double limit = args[0]->doubleValue();
    result = erfc( limit );
  }
  else
    return false;

  context.setValue( new KSValue( result ) );

  return true;
}

// Function: GESTEP
bool kspreadfunc_gestep( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  short result;
  double val1 = 0.0;
  double val2 = 0.0;

  if ( !KSUtil::checkArgumentsCount( context, 2, "GESTEP", false ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 1, "GESTEP", true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, false ) )
    {
      if ( !KSUtil::checkType( context, args[1], KSValue::BoolType, true ) )
        return false;
      val2 = ( args[1]->boolValue() ? 1.0 : 0.0 );
    }
    else
      val2 = args[1]->doubleValue();
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
  {
    if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
      return false;
    val1 = ( args[0]->boolValue() ? 1.0 : 0.0 );
  }
  else
    val1 = args[0]->doubleValue();

  if ( ( val1 > val2 ) || approx_equal_delta( val1, val2 ) )
    result = 1;
  else
    result = 0;

  context.setValue( new KSValue( result ) );

  return true;
}

