// built-in statistical functions
#include "kspread_util.h"
#include "kspread_doc.h"
#include "kspread_table.h"

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <stdlib.h>
#include <math.h>

// Function: permut
bool kspreadfunc_arrang( KSContext& context )
{ /* arrang : util_fact(n)/(util_fact(n-m) */
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
        result=util_fact((double)args[0]->intValue(),
        ((double)args[0]->intValue()-(double)args[1]->intValue()));
        //In util_fact function val must be positive

        if(result==-1)
                context.setValue( new KSValue(tmp));
        else
                context.setValue( new KSValue(result ));
        }
  return true;
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
  }

  return true;
}

// Function: average
bool kspreadfunc_average( KSContext& context )
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

static bool kspreadfunc_median_helper
  (KSContext& context, QValueList<KSValue::Ptr>& args,
   QValueList<KSValue::Ptr>& sortedList)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();
  bool returnVal = true;

  /* loop through each value, adding it to the sorted list (recursing
     whenever necessary */
  while(it != end && returnVal)
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      /* try to recurse */
      returnVal = kspreadfunc_median_helper(context, (*it)->listValue(),
                                            sortedList);
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      /* insert it properly into the list */
      QValueList<KSValue::Ptr>::Iterator ptr = sortedList.begin();
      QValueList<KSValue::Ptr>::Iterator endPtr = sortedList.end();

      /* find the proper place */
      while (ptr != endPtr && (*it)->doubleValue() > (*ptr)->doubleValue())
      {
        ++ptr;
      }
      sortedList.insert(ptr, *it);
    }
    ++it;
  }

  return returnVal;
}

// Function: median
bool kspreadfunc_median( KSContext& context )
{
  double result = 0.0;
  bool worked;

  /* need a list to hold all the values in sorted order so we can pick out the
     median one */
  QValueList<KSValue::Ptr> sortedValues;

  worked = kspreadfunc_median_helper(context, context.value()->listValue(),
                                     sortedValues);

  if (worked && sortedValues.size() > 0)
  {
    /* get the median value */
    QValueList<KSValue::Ptr>::Iterator ptr =
      sortedValues.at((sortedValues.size() - 1) / 2);

    /* now we're halfway through the list, or if there is an even number of
       items, we're on the 'first' of the 2 in the middle */

    result = (*ptr)->doubleValue();
    if (sortedValues.size() % 2 == 0)
    {
      ++ptr;
      result = (result + (*ptr)->doubleValue()) / 2;
    }

  }

  context.setValue( new KSValue(result));

  return worked;
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
  }

  return true;
}

// Function: variance
bool kspreadfunc_variance( KSContext& context )
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
  }

  return true;
}

// Function: stddev
bool kspreadfunc_stddev( KSContext& context )
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
      context.setValue( new KSValue(sqrt(result/((double)(number - 1)) )) );
  }

  return b;
}

// Function: combin
bool kspreadfunc_combin( KSContext& context )
{ /*combin : util_fact(n)/(util_fact(n-m)*util_fact(m)) */
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
        result=(util_fact((double)args[0]->intValue(),
        ((double)args[0]->intValue()-(double)args[1]->intValue()))
        /util_fact((double)args[1]->intValue(),0));
        //In util_fact function val must be positive

        if(result==-1)
                context.setValue( new KSValue(tmp));
        else
                context.setValue( new KSValue(result ));
        }
  return true;
}

// Function: bino
bool kspreadfunc_bino( KSContext& context )
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
    result=(util_fact(args[0]->doubleValue(),
                 (args[0]->doubleValue()-args[1]->doubleValue()))
            /util_fact(args[1]->doubleValue(),0));
    //In util_fact function val must be positive

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

// Function: bino_inv
bool kspreadfunc_bino_inv( KSContext& context )
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
        result=(util_fact(args[0]->doubleValue(),
        (args[0]->doubleValue()-args[1]->doubleValue()))
        /util_fact(args[1]->doubleValue(),0));
        //In util_fact function val must be positive

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

static double phi_helper(double x)
{
  return 0.39894228040143268 * exp(-(x * x) / 2.0);
}

// Function: phi
bool kspreadfunc_phi(KSContext& context)
{
  //distribution function for a standard normal distribution

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "PHI", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();

  context.setValue( new KSValue(phi_helper(x)) );

  return true;
}

static double taylor_helper (double* pPolynom, uint nMax, double x)
{
  double nVal = pPolynom[nMax];
  for (int i = nMax-1; i >= 0; i--) {
    nVal = pPolynom[i] + (nVal * x);
  }
  return nVal;
}

static double gauss_helper(double x) {
  double t0[] =
    { 0.39894228040143268, -0.06649038006690545,  0.00997355701003582,
     -0.00118732821548045,  0.00011543468761616, -0.00000944465625950,
      0.00000066596935163, -0.00000004122667415,  0.00000000227352982,
      0.00000000011301172,  0.00000000000511243, -0.00000000000021218 };
  double t2[] =
    { 0.47724986805182079,  0.05399096651318805, -0.05399096651318805,
      0.02699548325659403, -0.00449924720943234, -0.00224962360471617,
      0.00134977416282970, -0.00011783742691370, -0.00011515930357476,
      0.00003704737285544,  0.00000282690796889, -0.00000354513195524,
      0.00000037669563126,  0.00000019202407921, -0.00000005226908590,
     -0.00000000491799345,  0.00000000366377919, -0.00000000015981997,
     -0.00000000017381238,  0.00000000002624031,  0.00000000000560919,
     -0.00000000000172127, -0.00000000000008634,  0.00000000000007894 };
  double t4[] =
    { 0.49996832875816688,  0.00013383022576489, -0.00026766045152977,
      0.00033457556441221, -0.00028996548915725,  0.00018178605666397,
     -0.00008252863922168,  0.00002551802519049, -0.00000391665839292,
     -0.00000074018205222,  0.00000064422023359, -0.00000017370155340,
      0.00000000909595465,  0.00000000944943118, -0.00000000329957075,
      0.00000000029492075,  0.00000000011874477, -0.00000000004420396,
      0.00000000000361422,  0.00000000000143638, -0.00000000000045848 };
  double asympt[] = { -1.0, 1.0, -3.0, 15.0, -105.0 };

  double xAbs = fabs(x);
  uint xShort = static_cast<uint>(floor(xAbs));
  double nVal = 0.0;
  if (xShort == 0)
    nVal = taylor_helper(t0, 11, (xAbs * xAbs)) * xAbs;
  else if ((xShort >= 1) && (xShort <= 2))
    nVal = taylor_helper(t2, 23, (xAbs - 2.0));
  else if ((xShort >= 3) && (xShort <= 4))
    nVal = taylor_helper(t4, 20, (xAbs - 4.0));
  else
    nVal = 0.5 + phi_helper(xAbs) * taylor_helper(asympt, 4, 1.0 / (xAbs * xAbs)) / xAbs;
  if (x < 0.0)
    return -nVal;
  else
    return nVal;
}

// Function: gauss
bool kspreadfunc_gauss(KSContext& context)
{
  //returns the integral values of the standard normal cumulative distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "GAUSS", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();

  double tmp = gauss_helper(x);

  context.setValue( new KSValue(tmp) );

  return true;
}

//helper for kspreadfunc_gammadist
static double GammaHelp(double& x, bool& bReflect)
{
  double c[6] = {76.18009173, -86.50532033, 24.01409822,
                 -1.231739516, 0.120858003E-2, -0.536382E-5};
  if (x >= 1.0)
    {
      bReflect = FALSE;
      x -= 1.0;
    }
  else
    {
      bReflect = TRUE;
      x = 1.0 - x;
    }
  double s, anum;
  s = 1.0;
  anum = x;
  for (uint i = 0; i < 6; i++)
    {
      anum += 1.0;
      s += c[i]/anum;
    }
  s *= 2.506628275;   // sqrt(2*PI)
  return s;
}

//helper for kspreadfunc_gammadist
static double GetGamma(double x)
{
  bool bReflect;
  double G = GammaHelp(x, bReflect);
  G = pow(x+5.5,x+0.5)*G/exp(x+5.5);
  if (bReflect)
    G = M_PI*x/(G*sin(M_PI*x));
  return G;
}

//helper for kspreadfunc_gammadist and others
static double GetGammaDist(double x, double alpha, double beta)
{
  if (x == 0.0)
    return 0.0;

  x /= beta;
  double gamma = alpha;

  double c = 0.918938533204672741;
  double d[10] = {
    0.833333333333333333E-1,
    -0.277777777777777778E-2,
    0.793650793650793651E-3,
    -0.595238095238095238E-3,
    0.841750841750841751E-3,
    -0.191752691752691753E-2,
    0.641025641025641025E-2,
    -0.295506535947712418E-1,
    0.179644372368830573,
    -0.139243221690590111E1
  };

  double dx = x;
  double dgamma = gamma;
  int maxit = 10000;

  double z = dgamma;
  double den = 1.0;
  while ( z < 10.0 ) {
    den *= z;
    z += 1.0;
  }

  double z2 = z*z;
  double z3 = z*z2;
  double z4 = z2*z2;
  double z5 = z2*z3;
  double a = ( z - 0.5 ) * log(z) - z + c;
  double b = d[0]/z + d[1]/z3 + d[2]/z5 + d[3]/(z2*z5) + d[4]/(z4*z5) +
    d[5]/(z*z5*z5) + d[6]/(z3*z5*z5) + d[7]/(z5*z5*z5) + d[8]/(z2*z5*z5*z5);
  // double g = exp(a+b) / den;

  double sum = 1.0 / dgamma;
  double term = 1.0 / dgamma;
  double cut1 = dx - dgamma;
  double cut2 = dx * 10000000000.0;

  for ( int i=1; i<=maxit; i++ ) {
    double ai = i;
    term = dx * term / ( dgamma + ai );
    sum += term;
    double cutoff = cut1 + ( cut2 * term / sum );
    if ( ai > cutoff ) {
      double t = sum;
      // return pow( dx, dgamma ) * exp( -dx ) * t / g;
      return exp( dgamma * log(dx) - dx - a - b ) * t * den;
    }
  }

  return 1.0;             // should not happen ...
}

// Function: gammadist
bool kspreadfunc_gammadist( KSContext& context )
{
  //returns the gamma distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "GAMMADIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::IntType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double alpha = args[1]->doubleValue();
  double beta = args[2]->doubleValue();
  int kum = args[3]->intValue();  // 0 or 1
  double result;

  if (x < 0.0  || alpha <= 0.0 || beta <= 0.0)
    //SetIllegalArgument();
    //TODO error?
    return false;
  else if (kum == 0) {  //density
    double G = GetGamma(alpha);
    result = pow(x,alpha-1.0)/exp(x/beta)/pow(beta,alpha)/G;
  }
  else
    result = GetGammaDist(x, alpha, beta);

  context.setValue( new KSValue(result) );
  return true;
}

static double GetLogGamma(double x)
{
  bool bReflect;
  double G = GammaHelp(x, bReflect);
  G = (x+0.5)*log(x+5.5)+log(G)-(x+5.5);
  if (bReflect)
    G = log(M_PI*x)-G-log(sin(M_PI*x));
  return G;
}


static double beta_helper(double x, double alpha, double beta) {
  if (beta == 1.0)
    return pow(x, alpha);
  else if (alpha == 1.0)
    return 1.0 - pow(1.0-x,beta);

  double fEps = 1.0E-8;
  bool bReflect;
  double cf, fA, fB;

  if (x < (alpha+1.0)/(alpha+beta+1.0)) {
    bReflect = FALSE;
    fA = alpha;
    fB = beta;
  }
  else {
    bReflect = TRUE;
    fA = beta;
    fB = alpha;
    x = 1.0 - x;
  }
  if (x < fEps)
    cf = 0.0;
  else {
    double a1, b1, a2, b2, fnorm, rm, apl2m, d2m, d2m1, cfnew;
    a1 = 1.0; b1 = 1.0;
    b2 = 1.0 - (fA+fB)*x/(fA+1.0);
    if (b2 == 0.0) {
      a2 = b2;
      fnorm = 1.0;
      cf = 1.0;
    }
    else {
      a2 = 1.0;
      fnorm = 1.0/b2;
      cf = a2*fnorm;
    }
    cfnew = 1.0;
    for (uint j = 1; j <= 100; j++) {
      rm = (double) j;
      apl2m = fA + 2.0*rm;
      d2m = rm*(fB-rm)*x/((apl2m-1.0)*apl2m);
      d2m1 = -(fA+rm)*(fA+fB+rm)*x/(apl2m*(apl2m+1.0));
      a1 = (a2+d2m*a1)*fnorm;
      b1 = (b2+d2m*b1)*fnorm;
      a2 = a1 + d2m1*a2*fnorm;
      b2 = b1 + d2m1*b2*fnorm;
      if (b2 != 0.0) {
        fnorm = 1.0/b2;
        cfnew = a2*fnorm;
        if (fabs(cf-cfnew)/cf < fEps)
          j = 101;
        else
          cf = cfnew;
      }
    }
    if (fB < fEps)
      b1 = 1.0E30;
    else
      b1 = exp(GetLogGamma(fA)+GetLogGamma(fB)-GetLogGamma(fA+fB));

    cf *= pow(x, fA)*pow(1.0-x,fB)/(fA*b1);
  }
  if (bReflect)
    return 1.0-cf;
  else
    return cf;
}

// Function: betadist
bool kspreadfunc_betadist( KSContext& context ) {
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  double fA, fB;  //lower, upper bound
  fA = 0.0;
  fB = 1.0;

  if ( KSUtil::checkArgumentsCount( context, 5, "BETADIST", false ) ) {
    if( KSUtil::checkType( context, args[3], KSValue::DoubleType, false ) )
      fA = args[3]->doubleValue();
    if( KSUtil::checkType( context, args[4], KSValue::DoubleType, false ) )
      fB = args[4]->doubleValue();
  }
  else if ( KSUtil::checkArgumentsCount( context, 4, "BETADIST", false ) ) {
    if( KSUtil::checkType( context, args[3], KSValue::DoubleType, false ) )
      fA = args[3]->doubleValue();
  }
  else if (!KSUtil::checkArgumentsCount( context, 3, "BETADIST", false ) )
    return false;

  double x, alpha, beta;
  x = args[0]->doubleValue();
  alpha = args[1]->doubleValue();
  beta = args[2]->doubleValue();

  if (x < fA || x > fB || fA == fB || alpha <= 0.0 || beta <= 0.0) { //checks
    return false;
  }
  x = (x-fA)/(fB-fA); //scaling

  context.setValue( new KSValue( beta_helper(x, alpha, beta) ));
  return true;
}

// Function: fisher
bool kspreadfunc_fisher( KSContext& context ) {
  //returns the Fisher transformation for x
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "FISHER", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double fVal = args[0]->doubleValue();

  context.setValue( new KSValue(0.5*log((1.0+fVal)/(1.0-fVal))));
  return true;
}

// Function: fisherinv
bool kspreadfunc_fisherinv( KSContext& context ) {
  //returns the inverse of the Fisher transformation for x
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "FISHERINV", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double fVal = args[0]->doubleValue();

  context.setValue( new KSValue((exp(2.0*fVal)-1.0)/(exp(2.0*fVal)+1.0)));
  return true;
}

// Function: normdist
bool kspreadfunc_normdist(KSContext& context ) {
  //returns the normal cumulative distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "NORMDIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::IntType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double mue = args[1]->doubleValue();
  double sigma = args[2]->doubleValue();
  double k = args[3]->intValue();

  if (sigma <= 0.0)
    return false;
  else if (k == 0)	// density
    context.setValue( new KSValue(phi_helper((x-mue)/sigma)/sigma));
  else			// distribution
    context.setValue( new KSValue(0.5 + gauss_helper((x-mue)/sigma)));

  return true;
}

// Function: lognormdist
bool kspreadfunc_lognormdist(KSContext& context ) {
  //returns the cumulative lognormal distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "LOGNORMDIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double mue = args[1]->doubleValue();
  double sigma = args[2]->doubleValue();

  if (sigma <= 0.0 || x <= 0.0)
    return false;
  else
    context.setValue( new KSValue(0.5 + gauss_helper((log(x)-mue)/sigma)));

  return true;
}

// Function: normsdist
bool kspreadfunc_stdnormdist(KSContext& context ) {
  //returns the cumulative lognormal distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "NORMSDIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();

  context.setValue( new KSValue(0.5 + gauss_helper(x)));
  return true;
}

// Function: expondist
bool kspreadfunc_expondist(KSContext& context ) {
  //returns the exponential distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "EXPONDIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double lambda = args[1]->doubleValue();
  double kum = args[2]->intValue();

  double result;

  if (lambda <= 0.0)
    return false;
  else if (kum == 0) {	//density
    if (x >= 0.0)
      result = lambda * exp(-lambda*x);
    else
      result = 0;
  }
  else {  //distribution
    if (x > 0.0)
      result = 1.0 - exp(-lambda*x);
    else
      result = 0;
  }

  context.setValue( new KSValue(result));
  return true;
}

// Function: weibull
bool kspreadfunc_weibull( KSContext& context ) {
  //returns the Weibull distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "WEIBULL", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::IntType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double alpha = args[1]->doubleValue();
  double beta = args[2]->doubleValue();
  double kum = args[3]->intValue();

  double result;

  if (alpha <= 0.0 || beta <= 0.0 || x < 0.0)
    return false;
  else if (kum == 0)  // density
    result = alpha / pow(beta,alpha) * pow(x,alpha-1.0) * exp(-pow(x/beta,alpha));
  else	// distribution
    result = 1.0 - exp(-pow(x/beta,alpha));

  context.setValue( new KSValue(result));
  return true;
}

static double gaussinv_helper (double x) {
  double c0, c1, c2, d1, d2, d3, q, t, z;
  c0 = 2.515517;
  c1 = 0.802853;
  c2 = 0.010328;
  d1 = 1.432788;
  d2 = 0.189269;
  d3 = 0.001308;
  if (x < 0.5)
    q = x;
  else
    q = 1.0-x;
  t = sqrt(-log(q*q));
  z = t - (c0 + t*(c1 + t*c2)) / (1.0 + t*(d1 + t*(d2 + t*d3)));
  if (x < 0.5)
    z *= -1.0;
  return z;
}

// Function: normsinv
bool kspreadfunc_normsinv( KSContext& context ) {
  //returns the inverse of the standard normal cumulative distribution

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "NORMSINV", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();

  if (x <= 0.0 || x >= 1.0)
    return false;
  else
    context.setValue( new KSValue(gaussinv_helper(x)));

  return true;
}

// Function: norminv
bool kspreadfunc_norminv( KSContext& context ) {
  //returns the inverse of the normal cumulative distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "NORMINV", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double mue = args[1]->doubleValue();
  double sigma = args[2]->doubleValue();

  if (sigma <= 0.0 || x <= 0.0 || x >= 1.0)
    return false;
  else
    context.setValue( new KSValue((gaussinv_helper(x)*sigma + mue)));

  return true;
}

// Function: gammaln
bool kspreadfunc_gammaln( KSContext& context ) {
  //returns the natural logarithm of the gamma function
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "GAMMALN", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double x = args[0]->doubleValue();

  if (x > 0.0)
    context.setValue( new KSValue(GetLogGamma(x)));
  else
    return false;

  return true;
}

// Function: poisson
bool kspreadfunc_poisson( KSContext& context ) {
  //returns the Poisson distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "POISSON", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  double x = args[0]->doubleValue();
  double lambda = args[1]->doubleValue();
  double kum = args[2]->intValue();

  double result;

  if (lambda < 0.0 || x < 0.0)
    return false;
  else if (kum == 0)  { // density
    if (lambda == 0.0)
      result = 0;
    else
      result = exp(-lambda) * pow(lambda,x) / util_fact(x,0);
    }
  else { // distribution
    if (lambda == 0.0)
      result = 1;
    else {
      double sum = 1.0;
      double fFak = 1.0;
      unsigned long nEnd = static_cast<unsigned long>(x);
      for (unsigned long i = 1; i <= nEnd; i++) {
        fFak *= static_cast<double>(i);
        sum += pow( lambda, static_cast<double>(i) ) / fFak;
      }
      sum *= exp(-lambda);
      result = sum;
    }
  }

  context.setValue( new KSValue(result));
  return true;
}

// Function: confidence
bool kspreadfunc_confidence( KSContext& context ) {
  //returns the confidence interval for a population mean
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "CONFIDENCE", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  double alpha = args[0]->doubleValue();
  double sigma = args[1]->doubleValue();
  double n = args[2]->intValue();

  if (sigma <= 0.0 || alpha <= 0.0 || alpha >= 1.0 || n < 1)
    return false;
  else
    context.setValue( new KSValue(gaussinv_helper(1.0-alpha/2.0) * sigma/sqrt(n)));

  return true;
}

static double GetFDist(double x, double fF1, double fF2) {
  double arg = fF2/(fF2+fF1*x);
  double alpha = fF2/2.0;
  double beta = fF1/2.0;
  return beta_helper(arg, alpha, beta);
}

static double GetTDist(double T, double fDF) {
  return 0.5 * beta_helper(fDF/(fDF+T*T), fDF/2.0, 0.5);
}

static double GetChiDist(double fChi, double fDF) {
  return 1.0 - GetGammaDist(fChi/2.0, fDF/2.0, 1.0);
}

// Function: tdist
bool kspreadfunc_tdist( KSContext& context ) {
  //returns the t-distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "TDIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  double T = args[0]->doubleValue();
  double deg = args[1]->intValue();
  double flag = args[2]->intValue();

  if (deg < 1 || T < 0.0 || (flag != 1 && flag != 2) )
    return false;

  double R = GetTDist(T, deg);
  if (flag == 1)
    context.setValue( new KSValue(R));
  else
    context.setValue( new KSValue(2.0*R));

  return true;
}

// Function: fdist
bool kspreadfunc_fdist( KSContext& context ) {
  //returns the f-distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "FDIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  double fF = args[0]->doubleValue();
  double fF1 = args[1]->intValue();
  double fF2 = args[2]->intValue();

  if (fF < 0.0 || fF1 < 1 || fF2 < 1 || fF1 >= 1.0E10 || fF2 >= 1.0E10) {
    return false;
  }

  context.setValue( new KSValue(GetFDist(fF, fF1, fF2)));

  return true;
}

// Function: chidist
bool kspreadfunc_chidist( KSContext& context ) {
  //returns the chi-distribution
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "CHIDIST", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  double fChi = args[0]->doubleValue();
  double fDF = args[1]->intValue();

  if (fDF < 1 || fDF >= 1.0E5 || fChi < 0.0 )
    return false;

  context.setValue( new KSValue(GetChiDist(fChi, fDF)));

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

// Function: sumproduct
bool kspreadfunc_sumproduct( KSContext& context )
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

// Function: sumx2py2
bool kspreadfunc_sumx2py2( KSContext& context )
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

// Function: sumx2my2
bool kspreadfunc_sumx2my2( KSContext& context )
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

// Function: sum2xmy
bool kspreadfunc_sumxmy2( KSContext& context )
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

static bool kspreadfunc_avedev_helper(KSContext &context, QValueList<KSValue::Ptr> &args, double &result, double temp)
{
	QValueList<KSValue::Ptr>::Iterator it = args.begin();
	QValueList<KSValue::Ptr>::Iterator end = args.end();

	for(; it != end; ++it)
	{
		if(KSUtil::checkType(context, *it, KSValue::ListType, false))
		{
			if(!kspreadfunc_avedev_helper(context, (*it)->listValue(), result, temp))
				return false;
		}
		else if(KSUtil::checkType(context, *it, KSValue::DoubleType, true))
			result += fabs((*it)->doubleValue() - temp);
	}

	return true;
}

// Function: avedev
bool kspreadfunc_avedev(KSContext &context)
{
	double temp = 0.0, result = 0.0;
	int number = 0;

	// First sum the range into one double
	bool b = kspreadfunc_average_helper(context, context.value()->listValue(), temp, number);

	if(number == 0)
	{
		context.setValue(new KSValue(i18n("#DIV/0")));
		return true;
	}

	if(!b)
		return false;

	// Devide by the number of values
	temp /= number;
	
	bool finish = kspreadfunc_avedev_helper(context, context.value()->listValue(), result, temp);
	
	if(!finish)
		return false;
	
	// Devide by the number of values
	result /= number;

	context.setValue(new KSValue(result));

	return b;
}
