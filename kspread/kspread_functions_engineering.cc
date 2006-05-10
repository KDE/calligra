/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// built-in engineering functions

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

// used by the CONVERT function
#include <QMap>

// these are needed for complex functions, while we handle them in the old way
#include <kglobal.h>
#include <klocale.h>
#include <math.h>

using namespace KSpread;

// prototypes (sort alphabetically)
Value func_base (valVector args, ValueCalc *calc, FuncExtra *);
Value func_besseli (valVector args, ValueCalc *calc, FuncExtra *);
Value func_besselj (valVector args, ValueCalc *calc, FuncExtra *);
Value func_besselk (valVector args, ValueCalc *calc, FuncExtra *);
Value func_bessely (valVector args, ValueCalc *calc, FuncExtra *);
Value func_bin2dec (valVector args, ValueCalc *calc, FuncExtra *);
Value func_bin2oct (valVector args, ValueCalc *calc, FuncExtra *);
Value func_bin2hex (valVector args, ValueCalc *calc, FuncExtra *);
Value func_complex (valVector args, ValueCalc *calc, FuncExtra *);
Value func_complex_imag (valVector args, ValueCalc *calc, FuncExtra *);
Value func_complex_real (valVector args, ValueCalc *calc, FuncExtra *);
Value func_convert (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dec2hex (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dec2oct (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dec2bin (valVector args, ValueCalc *calc, FuncExtra *);
Value func_delta (valVector args, ValueCalc *calc, FuncExtra *);
Value func_erf (valVector args, ValueCalc *calc, FuncExtra *);
Value func_erfc (valVector args, ValueCalc *calc, FuncExtra *);
Value func_gestep (valVector args, ValueCalc *calc, FuncExtra *);
Value func_hex2dec (valVector args, ValueCalc *calc, FuncExtra *);
Value func_hex2bin (valVector args, ValueCalc *calc, FuncExtra *);
Value func_hex2oct (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imabs (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imargument (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imconjugate (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imcos (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imdiv (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imexp (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imln (valVector args, ValueCalc *calc, FuncExtra *);
Value func_impower (valVector args, ValueCalc *calc, FuncExtra *);
Value func_improduct (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imsin (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imsqrt (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imsub (valVector args, ValueCalc *calc, FuncExtra *);
Value func_imsum (valVector args, ValueCalc *calc, FuncExtra *);
Value func_oct2dec (valVector args, ValueCalc *calc, FuncExtra *);
Value func_oct2bin (valVector args, ValueCalc *calc, FuncExtra *);
Value func_oct2hex (valVector args, ValueCalc *calc, FuncExtra *);

// registers all engineering functions
void RegisterEngineeringFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("BASE",        func_base);    // KSpread-specific, like in Quattro-Pro
  f->setParamCount (1, 3);
  repo->add (f);
  f = new Function ("BESSELI",     func_besseli);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("BESSELJ",     func_besselj);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("BESSELK",     func_besselk);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("BESSELY",     func_bessely);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("BIN2DEC",     func_bin2dec);
  repo->add (f);
  f = new Function ("BIN2OCT",     func_bin2oct);
  repo->add (f);
  f = new Function ("BIN2HEX",     func_bin2hex);
  repo->add (f);
  f = new Function ("COMPLEX",     func_complex);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("CONVERT",     func_convert);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("DEC2HEX",     func_dec2hex);
  repo->add (f);
  f = new Function ("DEC2BIN",     func_dec2bin);
  repo->add (f);
  f = new Function ("DEC2OCT",     func_dec2oct);
  repo->add (f);
  f = new Function ("DELTA",       func_delta);
  f->setParamCount (1, 2);
  repo->add (f);
  f = new Function ("ERF",         func_erf);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("ERFC",        func_erfc);
  f->setParamCount (1, 2);
  repo->add (f);
  f = new Function ("GESTEP",      func_gestep);
  f->setParamCount (1, 2);
  repo->add (f);
  f = new Function ("HEX2BIN",     func_hex2bin);
  repo->add (f);
  f = new Function ("HEX2DEC",     func_hex2dec);
  repo->add (f);
  f = new Function ("HEX2OCT",     func_hex2oct);
  repo->add (f);
  f = new Function ("IMABS",       func_imabs);
  repo->add (f);
  f = new Function ("IMAGINARY",   func_complex_imag);
  repo->add (f);
  f = new Function ("IMARGUMENT",  func_imargument);
  repo->add (f);
  f = new Function ("IMCONJUGATE", func_imconjugate);
  repo->add (f);
  f = new Function ("IMCOS",       func_imcos);
  repo->add (f);
  f = new Function ("IMDIV",       func_imdiv);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("IMEXP",       func_imexp);
  repo->add (f);
  f = new Function ("IMLN",        func_imln);
  repo->add (f);
  f = new Function ("IMPOWER",     func_impower);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("IMPRODUCT",   func_improduct);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("IMREAL",      func_complex_real);
  repo->add (f);
  f = new Function ("IMSIN",       func_imsin);
  repo->add (f);
  f = new Function ("IMSQRT",      func_imsqrt);
  repo->add (f);
  f = new Function ("IMSUB",       func_imsub);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("IMSUM",       func_imsum);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("OCT2BIN",     func_oct2bin);
  repo->add (f);
  f = new Function ("OCT2DEC",     func_oct2dec);
  repo->add (f);
  f = new Function ("OCT2HEX",     func_oct2hex);
  repo->add (f);
}

// Function: BASE
Value func_base (valVector args, ValueCalc *calc, FuncExtra *)
{
  int base = 10;
  int prec = 0;
  if (args.count() > 1)
    base = calc->conv()->asInteger (args[1]).asInteger();
  if (args.count() == 3)
    prec = calc->conv()->asInteger (args[2]).asInteger();

  if ((base < 2) || (base > 36))
    return Value::errorVALUE();
  if (prec < 0) prec = 2;

  return calc->base (args[0], base, prec);
}

// Function: BESSELI
Value func_besseli (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value y = args[1];
  return calc->besseli (y, x);
}

// Function: BESSELJ
Value func_besselj (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value y = args[1];
  return calc->besselj (y, x);
}

// Function: BESSELK
Value func_besselk (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value y = args[1];
  return calc->besselk (y, x);
}

// Function: BESSELY
Value func_bessely (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value y = args[1];
  return calc->besseln (y, x);
}

// Function: DEC2HEX
Value func_dec2hex (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (args[0], 16);
}

// Function: DEC2OCT
Value func_dec2oct (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (args[0], 8);
}

// Function: DEC2BIN
Value func_dec2bin (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (args[0], 2);
}

// Function: BIN2DEC
Value func_bin2dec (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->fromBase (args[0], 2);
}

// Function: BIN2OCT
Value func_bin2oct (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (calc->fromBase (args[0], 2), 8);
}

// Function: BIN2HEX
Value func_bin2hex (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (calc->fromBase (args[0], 2), 16);
}

// Function: OCT2DEC
Value func_oct2dec (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->fromBase (args[0], 8);
}

// Function: OCT2BIN
Value func_oct2bin (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (calc->fromBase (args[0], 8), 2);
}

// Function: OCT2HEX
Value func_oct2hex (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (calc->fromBase (args[0], 8), 16);
}

// Function: HEX2DEC
Value func_hex2dec (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->fromBase (args[0], 16);
}

// Function: HEX2BIN
Value func_hex2bin (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (calc->fromBase (args[0], 16), 2);
}

// Function: HEX2OCT
Value func_hex2oct (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->base (calc->fromBase (args[0], 16), 8);
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
  char prefix = unit[0].toLatin1();
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
    powerMap[ "PS" ]  = 1.359622e-3; // Pferdest�ke (German)
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
    volumeMap[ "tbs" ]    = 67.6133333333333; // sheetspoon
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
Value func_convert (valVector args, ValueCalc *calc, FuncExtra *)
{
  // This function won't support arbitrary precision.

  double value = calc->conv()->asFloat (args[0]).asFloat ();
  QString fromUnit = calc->conv()->asString (args[1]).asString();
  QString toUnit = calc->conv()->asString (args[2]).asString();

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
                        return Value::errorNA();

  return Value (result);
}


// functions operating over complex numbers ...
// these may eventually end up being merged into ValueCalc and friends
// then complex numbers will be handled transparently in most functions

static QString func_create_complex( double real,double imag )
{
  QString tmp,tmp2;
  if(imag ==0)
        {
        return KGlobal::locale()->formatNumber( real);
        }
  if(real!=0)
        tmp=KGlobal::locale()->formatNumber(real);
  else
    return KGlobal::locale()->formatNumber(imag)+'i';
  if (imag >0)
        tmp=tmp+'+'+KGlobal::locale()->formatNumber(imag)+'i';
  else
        tmp=tmp+KGlobal::locale()->formatNumber(imag)+'i';
  return tmp;

}

// Function: COMPLEX
Value func_complex (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (calc->isZero (args[1]))
    return args[0];
  double re = calc->conv()->asFloat (args[0]).asFloat ();
  double im = calc->conv()->asFloat (args[1]).asFloat ();
  QString tmp=func_create_complex (re, im);
  bool ok;
  double result = KGlobal::locale()->readNumber(tmp, &ok);
  if (ok)
    return Value (result);
  return Value (tmp);
}


static double imag_complexe(QString str, bool &ok)
{
QString tmp=str;
if(tmp.indexOf('i')==-1)
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
        if((pos1=tmp.indexOf('+'))!=-1&& pos1==0)
                {
                ok=true;
                return 1;
                }
        else if( (pos1=tmp.indexOf('-'))!=-1 && pos1==0 )
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
        if((pos1=tmp.indexOf('i'))!=-1)
                {
                double val;
                QString tmpStr;

                if((pos2=tmp.lastIndexOf('+'))!=-1 && pos2!=0)
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
                else if( (pos2=tmp.lastIndexOf('-'))!=-1&& pos2!=0)
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
Value func_complex_imag (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString ();
  bool good;
  double result=imag_complexe(tmp, good);
  if (good)
    return Value (result);
  return Value::errorVALUE();
}


static double real_complexe(QString str, bool &ok)
{
double val;
int pos1,pos2;
QString tmp=str;
QString tmpStr;
if((pos1=tmp.indexOf('i'))==-1)
        { //12.5
        val=KGlobal::locale()->readNumber(tmp, &ok);
        if(!ok)
                val=0;
        return val;
        }
else
        { //15-xi
        if((pos2=tmp.lastIndexOf('-'))!=-1 && pos2!=0)
                {
                tmpStr=tmp.left(pos2);
                val=KGlobal::locale()->readNumber(tmpStr, &ok);
                if(!ok)
                        val=0;
                return val;
                } //15+xi
        else if((pos2=tmp.lastIndexOf('+'))!=-1)
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
Value func_complex_real (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString ();
  bool good;
  double result=real_complexe(tmp, good);
  if (good)
    return Value (result);
  return Value::errorVALUE();
}

void ImHelper (ValueCalc *c, Value res, Value val,
    double &imag, double &real, double &imag1, double &real1)
{
  bool ok;
  imag=imag_complexe(res.asString(), ok);
  real=real_complexe(res.asString(), ok);
  if (val.isString())
  {
    imag1 = imag_complexe (val.asString(), ok);
    real1 = real_complexe (val.asString(), ok);
  } else {
    imag1=0;
    real1=c->conv()->asFloat (val).asFloat();
  }
}

void awImSum (ValueCalc *c, Value &res, Value val, Value)
{
  double imag,real,imag1,real1;
  ImHelper (c, res, val, imag, real, imag1, real1);
  res=func_create_complex(real+real1,imag+imag1);
}

void awImSub (ValueCalc *c, Value &res, Value val, Value)
{
  double imag,real,imag1,real1;
  ImHelper (c, res, val, imag, real, imag1, real1);
  res=func_create_complex(real-real1,imag-imag1);
}

void awImMul (ValueCalc *c, Value &res, Value val, Value)
{
  double imag,real,imag1,real1;
  ImHelper (c, res, val, imag, real, imag1, real1);
  res=func_create_complex(real*real1+(imag*imag1)*-1,real*imag1+real1*imag);
}

void awImDiv (ValueCalc *c, Value &res, Value val, Value)
{
  double imag,real,imag1,real1;
  ImHelper (c, res, val, imag, real, imag1, real1);
  res=func_create_complex((real*real1+imag*imag1)/(real1*real1+imag1*imag1),
      (real1*imag-real*imag1)/(real1*real1+imag1*imag1));
}

// Function: IMSUM
Value func_imsum (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->arrayWalk (args, result, awImSum, 0);

  bool ok;
  QString res = calc->conv()->asString (result).asString();
  double val=KGlobal::locale()->readNumber(res, &ok);
  if (ok)
    return Value (val);
  return Value (result);
}

// Function: IMSUB
Value func_imsub (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->arrayWalk (args, result, awImSub, 0);

  bool ok;
  QString res = calc->conv()->asString (result).asString();
  double val=KGlobal::locale()->readNumber(res, &ok);
  if (ok)
    return Value (val);
  return Value (result);
}

// Function: IMPRODUCT
Value func_improduct (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->arrayWalk (args, result, awImMul, 0);

  bool ok;
  QString res = calc->conv()->asString (result).asString();
  double val=KGlobal::locale()->readNumber(res, &ok);
  if (ok)
    return Value (val);
  return Value (result);
}

// Function: IMDIV
Value func_imdiv (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->arrayWalk (args, result, awImDiv, 0);

  bool ok;
  QString res = calc->conv()->asString (result).asString();
  double val=KGlobal::locale()->readNumber(res, &ok);
  if (ok)
    return Value (val);
  return Value (result);
}

// Function: IMCONJUGATE
Value func_imconjugate (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if (!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();

  tmp=func_create_complex(real,-imag);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
    return Value (result);

  return Value (tmp);
}

// Function: IMARGUMENT
Value func_imargument (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if (!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if (!ok)
    return Value::errorVALUE();
  if(imag==0)
    return Value::errorDIV0();
  double arg=atan2(imag,real);

  return Value (arg);
}

// Function: IMABS
Value func_imabs (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double arg=sqrt(pow(imag,2)+pow(real,2));

  return Value (arg);
}

// Function: IMCOS
Value func_imcos (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag_res=sin(real)*sinh(imag);
  double real_res=cos(real)*cosh(imag);


  tmp=func_create_complex(real_res,-imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
    return Value (result);

  return Value (tmp);
}

// Function: IMSIN
Value func_imsin (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag_res=cos(real)*sinh(imag);
  double real_res=sin(real)*cosh(imag);


  tmp=func_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
    return Value (result);

  return Value (tmp);
}

// Function: IMLN
Value func_imln (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();

  double arg=sqrt(pow(imag,2)+pow(real,2));
  double real_res=log(arg);
  double imag_res=atan(imag/real);
  tmp=func_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
    return Value (result);

  return Value (tmp);
}

// Function: IMEXP
Value func_imexp (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag_res=exp(real)*sin(imag);
  double real_res=exp(real)*cos(imag);


  tmp=func_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
    return Value (result);

  return Value (tmp);
}

// Function: IMSQRT
Value func_imsqrt (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double arg=sqrt(sqrt(pow(imag,2)+pow(real,2)));
  double angle=atan(imag/real);

  double real_res=arg*cos((angle/2));
  double imag_res=arg*sin((angle/2));

  tmp=func_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
    return Value (result);

  return Value (tmp);
}

// Function: IMPOWER
Value func_impower (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp = calc->conv()->asString (args[0]).asString();
  double val2 = calc->conv()->asFloat (args[1]).asFloat();
  bool ok;
  double real=real_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();
  double imag=imag_complexe(tmp,ok);
  if(!ok)
    return Value::errorVALUE();

  double arg=::pow(sqrt(pow(imag,2)+pow(real,2)),val2);
  double angle=atan(imag/real);

  double real_res=arg*cos(angle*val2);
  double imag_res=arg*sin(angle*val2);

  tmp=func_create_complex(real_res,imag_res);

  double result=KGlobal::locale()->readNumber(tmp, &ok);
  if(ok)
    return Value (result);

  return Value (tmp);
}

// Function: DELTA
Value func_delta (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value val1 = args[0];
  Value val2 = 0.0;
  if (args.count() == 2)
    val2 = args[1];

  return Value (calc->approxEqual (val1, val2) ? 1 : 0);
}

// Function: ERF
Value func_erf (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (args.count() == 2)
    return calc->sub (calc->erf (args[1]), calc->erf (args[0]));
  return calc->erf (args[0]);
}

// Function: ERFC
Value func_erfc (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (args.count() == 2)
    return calc->sub (calc->erfc (args[1]), calc->erfc (args[0]));
  return calc->erfc (args[0]);
}

// Function: GESTEP
Value func_gestep (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value y = 0.0;
  if (args.count() == 2)
    y = args[1];

  int result = 0;
  if (calc->greater (x, y) || calc->approxEqual (x, y))
    result = 1;

  return Value (result);
}

