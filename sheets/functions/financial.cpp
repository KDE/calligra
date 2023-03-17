// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Sascha Pfau <MrPeacock@gmail.com>
// SPDX-FileCopyrightText: 2002, 2006 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-FileCopyrightText: 2002 Norbert Andres <nandres@web.de>
// SPDX-FileCopyrightText: 1999-2000 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 1999-2000 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 1999-2000 Jonathan Singer <jsinger@genome.wi.mit.edu>
// SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

// built-in financial functions

#include "FinancialModule.h"

#include "engine/Function.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"


using namespace Calligra::Sheets;

// prototypes (sorted)
Value func_accrint(valVector args, ValueCalc *calc, FuncExtra *);
Value func_accrintm(valVector args, ValueCalc *calc, FuncExtra *);
Value func_amordegrc(valVector args, ValueCalc *calc, FuncExtra *);
Value func_amorlinc(valVector args, ValueCalc *calc, FuncExtra *);
Value func_compound(valVector args, ValueCalc *calc, FuncExtra *);
Value func_continuous(valVector args, ValueCalc *calc, FuncExtra *);
Value func_coupdaybs (valVector args, ValueCalc *calc, FuncExtra *);
Value func_coupdays (valVector args, ValueCalc *calc, FuncExtra *);
Value func_coupdaysnc (valVector args, ValueCalc *calc, FuncExtra *);
Value func_coupncd (valVector args, ValueCalc *calc, FuncExtra *);
Value func_coupnum(valVector args, ValueCalc *calc, FuncExtra *);
Value func_couppcd (valVector args, ValueCalc *calc, FuncExtra *);
Value func_cumipmt(valVector args, ValueCalc *calc, FuncExtra *);
Value func_cumprinc(valVector args, ValueCalc *calc, FuncExtra *);
Value func_db(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ddb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_disc(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dollarde(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dollarfr(valVector args, ValueCalc *calc, FuncExtra *);
Value func_duration(valVector args, ValueCalc *calc, FuncExtra *);
Value func_duration_add(valVector args, ValueCalc *calc, FuncExtra *);
Value func_effective(valVector args, ValueCalc *calc, FuncExtra *);
Value func_euro(valVector args, ValueCalc *calc, FuncExtra *);
Value func_euroconvert(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fvschedule(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fv_annuity(valVector args, ValueCalc *calc, FuncExtra *);
Value func_intrate(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ipmt(valVector args, ValueCalc *calc, FuncExtra *);
Value func_irr (valVector args, ValueCalc *calc, FuncExtra *);
Value func_ispmt(valVector args, ValueCalc *calc, FuncExtra *);
Value func_level_coupon(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mduration(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mirr(valVector args, ValueCalc *calc, FuncExtra *);
Value func_nominal(valVector args, ValueCalc *calc, FuncExtra *);
Value func_nper(valVector args, ValueCalc *calc, FuncExtra *);
Value func_npv(valVector args, ValueCalc *calc, FuncExtra *);
// Value func_oddfprice (valVector args, ValueCalc *calc, FuncExtra *);
// Value func_oddfyield (valVector args, ValueCalc *calc, FuncExtra *);
Value func_oddlprice(valVector args, ValueCalc *calc, FuncExtra *);
Value func_oddlyield(valVector args, ValueCalc *calc, FuncExtra *);
Value func_pmt(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ppmt(valVector args, ValueCalc *calc, FuncExtra *);
// Value func_price (valVector args, ValueCalc *calc, FuncExtra *);
// Value func_pricedisc (valVector args, ValueCalc *calc, FuncExtra *);
Value func_pricemat(valVector args, ValueCalc *calc, FuncExtra *);
Value func_pv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_pv_annuity(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rate(valVector args, ValueCalc *calc, FuncExtra *);
Value func_received(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rri(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sln(valVector args, ValueCalc *calc, FuncExtra *);
Value func_syd(valVector args, ValueCalc *calc, FuncExtra *);
Value func_tbilleq(valVector args, ValueCalc *calc, FuncExtra *);
Value func_tbillprice(valVector args, ValueCalc *calc, FuncExtra *);
Value func_tbillyield(valVector args, ValueCalc *calc, FuncExtra *);
Value func_vdb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_xirr(valVector args, ValueCalc *calc, FuncExtra *);
Value func_xnpv(valVector args, ValueCalc *calc, FuncExtra *);
// Value func_yield (valVector args, ValueCalc *calc, FuncExtra *);
Value func_yielddisc(valVector args, ValueCalc *calc, FuncExtra *);
Value func_yieldmat(valVector args, ValueCalc *calc, FuncExtra *);
Value func_zero_coupon(valVector args, ValueCalc *calc, FuncExtra *);


CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("kspreadfinancialmodule.json", FinancialModule)

FinancialModule::FinancialModule(QObject* parent, const QVariantList&)
        : FunctionModule(parent)
{
    Function *f;

    f = new Function("ACCRINT", func_accrint);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETACCRINT");
    f->setParamCount(6, 7);
    add(f);
    f = new Function("ACCRINTM", func_accrintm);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETACCRINTM");
    f->setParamCount(3, 5);
    add(f);
    f = new Function("AMORDEGRC", func_amordegrc);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETAMORDEGRC");
    f->setParamCount(6, 7);
    add(f);
    f = new Function("AMORLINC", func_amorlinc);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETAMORLINC");
    f->setParamCount(6, 7);
    add(f);
    f = new Function("COMPOUND", func_compound);
    f->setParamCount(4);
    add(f);
    f = new Function("CONTINUOUS", func_continuous);
    f->setParamCount(3);
    add(f);
    f = new Function ("COUPDAYBS", func_coupdaybs);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOUPDAYBS");
    f->setParamCount (3, 4);
    add(f);
    f = new Function ("COUPDAYS", func_coupdays);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOUPDAYS");
    f->setParamCount (3, 4);
    add(f);
    f = new Function ("COUPDAYSNC", func_coupdaysnc);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOUPDAYSNC");
    f->setParamCount (3, 4);
    add(f);
    f = new Function ("COUPNCD", func_coupncd);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOUPNCD");
    f->setParamCount (3, 4);
    add(f);
    f = new Function("COUPNUM", func_coupnum);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOUPNUM");
    f->setParamCount(3, 5);
    add(f);
    f = new Function ("COUPPCD", func_couppcd);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOUPPCD");
    f->setParamCount (3, 4);
    add(f);
    f = new Function("CUMIPMT", func_cumipmt);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCUMIPMT");
    f->setParamCount(6);
    add(f);
    f = new Function("CUMPRINC", func_cumprinc);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCUMPRINC");
    f->setParamCount(6);
    add(f);
    f = new Function("DB", func_db);
    f->setParamCount(4, 5);
    add(f);
    f = new Function("DDB", func_ddb);
    f->setParamCount(4, 5);
    add(f);
    f = new Function("DISC", func_disc);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDISC");
    f->setParamCount(4, 5);
    add(f);
    f = new Function("DOLLARDE", func_dollarde);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDOLLARDE");
    f->setParamCount(2);
    add(f);
    f = new Function("DOLLARFR", func_dollarfr);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDOLLARFR");
    f->setParamCount(2);
    add(f);
    f = new Function("DURATION", func_duration);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDURATION");
    f->setParamCount(3);
    add(f);
    f = new Function("DURATION_ADD", func_duration_add);
    f->setParamCount(5, 6);
    add(f);
    f = new Function("EFFECT", func_effective);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETEFFECT");
    f->setParamCount(2);
    add(f);
    f = new Function("EFFECTIVE", func_effective);
    f->setParamCount(2);
    add(f);
    f = new Function("EURO", func_euro);   // KSpread-specific, Gnumeric-compatible
    f->setParamCount(1);
    add(f);
    f = new Function("EUROCONVERT", func_euroconvert);
    f->setParamCount(3);
    add(f);
    f = new Function("FV", func_fv);
    f->setParamCount(3, 5);
    add(f);
    f = new Function("FVSCHEDULE", func_fvschedule);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETFVSCHEDULE");
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("FV_ANNUITY", func_fv_annuity);
    f->setParamCount(3);
    add(f);
    f = new Function("INTRATE", func_intrate);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETINTRATE");
    f->setParamCount(4, 5);
    add(f);
    f = new Function("IPMT", func_ipmt);
    f->setParamCount(4, 6);
    add(f);
    f = new Function ("IRR", func_irr);
    f->setParamCount (1, 2);
    f->setAcceptArray();
  add(f);
    f = new Function("ISPMT", func_ispmt);
    f->setParamCount(4);
    add(f);
    f = new Function("LEVEL_COUPON", func_level_coupon);
    f->setParamCount(5);
    add(f);
    f = new Function("MDURATION", func_mduration);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETMDURATION");
    f->setParamCount(5, 6);
    add(f);
    f = new Function("MIRR", func_mirr);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("NOMINAL", func_nominal);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETNOMINAL");
    f->setParamCount(2);
    add(f);
    f = new Function("NPER", func_nper);
    f->setParamCount(3, 5);
    add(f);
    f = new Function("NPV", func_npv);
    f->setParamCount(2, -1);
    f->setAcceptArray();
    add(f);
//   f = new Function ("ODDFPRICE", func_oddfprice);
//   f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETODDFPRICE");
//   f->setParamCount (8, 9);
//   add(f);
//   f = new Function ("ODDFYIELD", func_oddfyield);
//   f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETODDFYIELD");
//   f->setParamCount (8, 9);
//   add(f);
    f = new Function("ODDLPRICE", func_oddlprice);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETODDLPRICE");
    f->setParamCount(7, 8);
    add(f);
    f = new Function("ODDLYIELD", func_oddlyield);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETODDLYIELD");
    f->setParamCount(7, 8);
    add(f);
    f = new Function("PDURATION", func_duration);
    f->setParamCount(3);
    add(f);
    f = new Function("PMT", func_pmt);
    f->setParamCount(3, 5);
    add(f);
    f = new Function("PPMT", func_ppmt);
    f->setParamCount(4, 6);
    add(f);
//   f = new Function ("PRICE", func_price);
//   f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETPRICE");
//   f->setParamCount (6, 7);
//   add(f);
//   f = new Function ("PRICEDISC", func_pricedisc);
//   f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETPRICEDISC");
//   f->setParamCount (4, 5);
//   add(f);
    f = new Function("PRICEMAT", func_pricemat);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETPRICEMAT");
    f->setParamCount(5, 6);
    add(f);
    f = new Function("PV", func_pv);
    f->setParamCount(3, 5);
    add(f);
    f = new Function("PV_ANNUITY", func_pv_annuity);
    f->setParamCount(3);
    add(f);
    f = new Function ("RATE", func_rate);
    f->setParamCount (3, 6);
    add(f);
    f = new Function("RECEIVED", func_received);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETRECEIVED");
    f->setParamCount(4, 5);
    add(f);
    f = new Function("RRI", func_rri);
    f->setParamCount(3);
    add(f);
    f = new Function("SLN", func_sln);
    f->setParamCount(3);
    add(f);
    f = new Function("SYD", func_syd);
    f->setParamCount(4);
    add(f);
    f = new Function("TBILLEQ", func_tbilleq);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETTBILLEQ");
    f->setParamCount(3);
    add(f);
    f = new Function("TBILLPRICE", func_tbillprice);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETTBILLPRICE");
    f->setParamCount(3);
    add(f);
    f = new Function("TBILLYIELD", func_tbillyield);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETTBILLYIELD");
    f->setParamCount(3);
    add(f);
    f = new Function("VDB", func_vdb);
    f->setParamCount(5, 7);
    add(f);
    f = new Function("XIRR", func_xirr);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETXIRR");
    f->setParamCount(2, 3);
    f->setAcceptArray();
    add(f);
    f = new Function("XNPV", func_xnpv);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETXNPV");
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
//   f = new Function ("YIELD", func_yield);
//   f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETYIELD");
//   f->setParamCount (6, 7);
//   add(f);
    f = new Function("YIELDDISC", func_yielddisc);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETYIELDDISC");
    f->setParamCount(4, 5);
    add(f);
    f = new Function("YIELDMAT", func_yieldmat);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETYIELDMAT");
    f->setParamCount(5, 6);
    add(f);
    f = new Function("ZERO_COUPON", func_zero_coupon);
    f->setParamCount(3);
    add(f);
}

QString FinancialModule::descriptionFileName() const
{
    return QString("financial.xml");
}


// helper
/* 0: US 30 / 360
 * 1: real days
 * 2: real days / 360
 * 3: real days / 365
 * 4: European 30 / 360
 */

int daysBetweenDates(QDate const & date1, QDate const & date2, int basis)
{
    int day1, day2, month1, month2, year1, year2;
    bool isLeapYear = false;
    int days, months, years;

    day1   = date1.day();
    month1 = date1.month();
    year1  = date1.year();
    day2   = date2.day();
    month2 = date2.month();
    year2  = date2.year();

    years  = year2  - year1;
    months = month2 - month1 + years * 12;
    days   = day2   - day1;

    isLeapYear = QDate::isLeapYear(year1);

    switch (basis) {
    case 0:
        if (month1 == 2 && month2 != 2 && year1 == year2) {
            if (isLeapYear)
                return months * 30 + days - 1;
            else
                return months * 30 + days - 2;
        }
        return months * 30 + days;

    case 1: // TODO: real days for difference between months!
        //    return ( month2 - month1 ) * 30 + years * 360 + days;

    case 2: // TODO: real days for difference between months!
        //    return ( month2 - month1 ) * 30 + years * 365 + days;

    case 3:
        return date1.daysTo(date2);

    case 4:
        return months * 30 + days;
    }

    return -1;
}

int daysPerYear(QDate const & date, int basis)
{
    switch (basis) {
    case 0:
        return 360;

    case 1:
        if (QDate::isLeapYear(date.year()))
            return 366;
        return 365;

    case 2:
        return 360;
    case 3:
        return 365;
    case 4:
        return 360;
    }

    return -1;
}



///////////////////////////////////////////////////////////////////////////

//
// helper: awNpv
//
void awNpv(ValueCalc *c, Value &res, Value val, Value rate)
{
    Value result = c->conv()->asFloat(res.element(0, 0));
    Value value = c->conv()->asFloat(val);
    Value i = c->conv()->asFloat(res.element(1, 0));
    res.setElement(0, 0, c->add(result, c->div(value, c->pow(c->add(Value(1.0), rate), i))));
    res.setElement(1, 0, c->add(i, Value(1.0)));     // increment counter
}

// pow1p calculate (1+x)^y accurately
static long double pow1p(const long double& x, const long double& y)
{
    if (fabs(x) > 0.5)
        return pow(1 + x, y);
    else
        return exp(y * log1p(x));
}

// pow1pm1 calculate ((1+x)^y)-1 accurately
static long double pow1pm1(const long double& x, const long double& y)
{
    if (x <= -1)
        return pow(1 + x, y) - 1;
    else
        return expm1(y * log1p(x));
}


//
// helper: calc_fvifa
//
static Value calc_fvifa(ValueCalc * /*calc*/, Value rate,
                        Value nper)
{
    Value res;

    if (rate.isZero())
        return (nper);
    else
        return Value(pow1pm1(rate.asFloat(), nper.asFloat()) / rate.asFloat());
}


//
// helper: getPay
//
static Value getPay(ValueCalc *calc, Value rate,
                    Value nper, Value pv, Value fv, Value type)
{
    Value pvif, fvifa;

    // TODO This is for PMT. Check constraints of the other function using this
    //if (calc->isZero (rate)) return Value::errorVALUE();
    if (calc->isZero(rate))
        return calc->mul(calc->div(calc->add(pv, fv), nper), Value(-1.0));

    //pvif  = pow( 1 + rate, nper );
    //fvifa = ( pvif - 1 ) / rate;
    pvif = Value(pow1p(rate.asFloat(), nper.asFloat()));
//   fvifa = calc->div (calc->sub (pvif, 1), rate);
    fvifa = calc_fvifa(calc, rate, nper);

    // ( -pv * pvif - fv ) / ( ( 1.0 + rate * type ) * fvifa );
    Value val1 = calc->sub(calc->mul(calc->mul(Value(-1), pv), pvif), fv);
    Value val2 = calc->mul(calc->add(Value(1.0), calc->mul(rate, type)),
                           fvifa);


    debugSheets << "(1.0 + " << rate << "*" << type << ")*" << fvifa << ") =" << val2;
    return calc->div(val1, val2);
}

#if 0 // Not used?
//
// helper: getPrinc
//
static Value getPrinc(ValueCalc *calc, Value start,
                      Value pay, Value rate, Value period)
{
    // val1 = pow( 1 + rate, period )
    Value val1 = calc->pow(calc->add(rate, 1), period);
    // val2 = start * val1
    Value val2 = calc->mul(start, val1);
    // val3 = pay * ( ( val1 - 1 ) / rate )
    Value val3 = calc->mul(pay, calc->div(calc->sub(val1, Value(1)), rate));
    // result = val2 + val3
    return calc->add(val2, val3);
}
#endif
//
// helper: eurofactor
//
static double helper_eurofactor(const QString& currency)
{
    QString cur = currency.toUpper();
    double result = -1;

    if (cur == "ATS") result = 13.7603;        // Austria
    else if (cur == "BEF") result = 40.3399;   // Belgium
    else if (cur == "CYP") result = 0.585274;  // Cyprus
    else if (cur == "DEM") result = 1.95583;   // Germany
    else if (cur == "EEK") result = 15.6466;   // Estonia
    else if (cur == "ESP") result = 166.386;   // Spain
    else if (cur == "EUR") result = 1.0;       // Euro
    else if (cur == "FIM") result = 5.94573;   // Finland
    else if (cur == "FRF") result = 6.55957;   // France
    else if (cur == "GRD") result = 340.75;    // Greece
    else if (cur == "IEP") result = 0.787564;  // Ireland
    else if (cur == "ITL") result = 1936.27;   // Italy
    else if (cur == "LTL") result = 3.45280;   // Lithuania
    else if (cur == "LUX") result = 40.3399;   // Luxembourg <-- This is the wrong code only here for compatibility with old versions
    else if (cur == "LUF") result = 40.3399;   // Luxembourg
    else if (cur == "LVL") result = 0.702804;  // Latvia
    else if (cur == "MTL") result = 0.4293;    // Malta
    else if (cur == "NLG") result = 2.20371;   // Netherlands
    else if (cur == "PTE") result = 200.482;   // Portugal
    else if (cur == "SIT") result = 239.64;    // Slovenia
    else if (cur == "SKK") result = 30.126;    // Slovakia

    return result;
}

//
// helper for IPMT and CUMIPMT is calculation of IPMT
//
static Value helper_ipmt(ValueCalc* calc, Value rate, Value per, Value nper, Value pv, Value fv, Value type)
{
//     const Value payment = getPay (calc, rate, nper, pv, fv, type);
//     const Value ineg = getPrinc (calc, pv, payment, rate, calc->sub (per, Value(1)));
//     // -ineg * rate
//     return calc->mul (calc->mul (ineg, Value(-1)), rate);

    const Value pmt = getPay(calc, rate, nper, pv, fv, Value(0));  // Type 0

    // pow1p (rate, per-1)
    const Value val1(pow1p(rate.asFloat(), calc->sub(per, Value(1)).asFloat()));
    // pow1pm1 (rate, per-1)
    const Value val2(pow1pm1(rate.asFloat(), calc->sub(per, Value(1)).asFloat()));

    Value ipmt;
    // -1*(pv * pow1p(rate, per-1)*rate + pmt* pow1pm1(rate, per-1))
    ipmt = calc->mul(Value(-1), calc->add(calc->mul(calc->mul(pv, val1), rate), calc->mul(pmt, val2)));

    return (type.isZero()) ? ipmt : calc->div(ipmt, calc->add(Value(1), rate));
}


//
// helper vdbGetGDA
//
static double vdbGetGDA(const double cost, const double salvage, const double life,
                        const double period, const double depreciationFactor)
{
    double res, rate, oldCost, newCost;

    rate = depreciationFactor / life;
    if (rate >= 1.0) {
        rate = 1.0;
        if (period == 1.0)
            oldCost = cost;
        else
            oldCost = 0.0;
    } else
        oldCost = cost * pow(1.0 - rate, period - 1.0);

    newCost = cost * pow(1.0 - rate, period);

    if (newCost < salvage)
        res = oldCost - salvage;
    else
        res = oldCost - newCost;

    if (res < 0.0)
        res = 0.0;

    return (res);
}


//
// helper vdbInterVDB
//
static double vdbInterVDB(const double cost, const double salvage,
                          const double life, const double life1,
                          const double period, const double depreciationFactor)
{
    double res = 0.0;

    double intEnd         = ceil(period);
    unsigned long loopEnd = (unsigned long) intEnd;

    double term, lia = 0;
    double balance = cost - salvage;
    bool nowLia = false;
    double gda;
    unsigned long i;

    for (i = 1; i <= loopEnd; ++i) {
        if (!nowLia) {
            gda = vdbGetGDA(cost, salvage, life, (double) i, depreciationFactor);
            lia = balance / (life1 - (double)(i - 1));

            if (lia > gda) {
                term = lia;
                nowLia = true;
            } else {
                term = gda;
                balance -= gda;
            }
        } else {
            term = lia;
        }

        if (i == loopEnd)
            term *= (period + 1.0 - intEnd);

        res += term;
    }

    return (res);
}


//
// helper: xirrResult
//
// args[0] = values
// args[1] = dates
//
static double xirrResult(valVector& args, ValueCalc *calc, double& rate)
{
    QDate date;

    QDate date0 = calc->conv()->asDate(args[1].element(0), nullptr).asDate(calc->settings()); // FIXME locale

    double r = rate + 1.0;
    double res = calc->conv()->asFloat(args[0].element(0)).asFloat();

    for (int i = 1, count = args[0].count(); i < count; ++i) {
        date = calc->conv()->asDate(args[1].element(i), nullptr).asDate(calc->settings()); // FIXME locale
        double e_i = (date0.daysTo(date)) / 365.0;
        double val = calc->conv()->asFloat(args[0].element(i)).asFloat();

        res += val / pow(r, e_i);
    }

    return res;
}


//
// helper: xirrResultDerive
//
// args[0] = values
// args[1] = dates
//
static double xirrResultDerive(valVector& args, ValueCalc *calc, double& rate)
{
    QDate date;

    QDate date0 = calc->conv()->asDate(args[1].element(0), nullptr).asDate(calc->settings()); // FIXME locale

    double r = rate + 1.0;
    double res = 0.0;

    for (int i = 1, count = args[0].count(); i < count; ++i) {
        date = calc->conv()->asDate(args[1].element(i), nullptr).asDate(calc->settings()); // FIXME locale
        double e_i = (date0.daysTo(date)) / 365.0;
        double val = calc->conv()->asFloat(args[0].element(i)).asFloat();

        res -= e_i * val / pow(r, e_i + 1.0);
    }

    return res;
}


///////////////////////////////////////////////////////////////////////////////////////////////


//
// Function: ACCRINT
//
Value func_accrint(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate maturity = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate settlement = calc->conv()->asDate(args[2], nullptr).asDate(calc->settings()); // FIXME locale

    Value rate = args[3];
    Value par = args[4];
    int frequency = calc->conv()->asInteger(args[5]).asInteger();

    int basis = 0;
    if (args.count() == 7)
        basis = calc->conv()->asInteger(args[6]).asInteger();

    if (basis < 0 || basis > 4 || (calc->isZero(Value(frequency))) ||
            (12 % frequency != 0))
        return Value::errorVALUE();

    if (maturity >= settlement) {
        debugSheets << "maturity >= settlement";
        return Value::errorVALUE();
    }

    double d = daysBetweenDates(maturity, settlement, basis);
    double y = daysPerYear(maturity, basis);

    if (d < 0 || y <= 0 || calc->lower(par, Value(0)) || calc->lower(rate, Value(0)) ||
            calc->isZero(rate))
        return Value::errorVALUE();

    Value coeff = calc->div(calc->mul(par, rate), frequency);
    double n = d / y;

    return calc->mul(coeff, n * frequency);
}


//
// Function: ACCRINTM
//
Value func_accrintm(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate issue = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    Value rate = args[2];

    Value par = Value(1000);
    int basis = 0;
    if (args.count() > 3)
        par = args[3];
    if (args.count() == 5)
        basis = calc->conv()->asInteger(args[4]).asInteger();

    double d = daysBetweenDates(issue, maturity, basis);
    double y = daysPerYear(issue, basis);

    if (d < 0 || y <= 0 || calc->isZero(par) || calc->isZero(rate) ||
            calc->lower(par, Value(0)) || calc->lower(rate, Value(0)) || basis < 0 || basis > 4)
        return Value::errorVALUE();

    // par*date * d/y
    return calc->mul(calc->mul(par, rate), d / y);
}


//
// Function: AMORDEGRC
//
// AMORDEGRC( Cost; purchaseDate; firstPeriodEndDate; salvage; period; basis)
//
Value func_amordegrc(valVector args, ValueCalc *calc, FuncExtra *)
{
    double cost = calc->conv()->asFloat(args[0]).asFloat();
    QDate purchaseDate = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    QDate firstPeriodEndDate = calc->conv()->asDate(args[2], nullptr).asDate(calc->settings()); // FIXME locale
    double salvage = calc->conv()->asFloat(args[3]).asFloat();
    int period = calc->conv()->asInteger(args[4]).asInteger();
    double rate = calc->conv()->asFloat(args[5]).asFloat();

    int basis = 0;
    if (args.count() > 6)
        basis = calc->conv()->asInteger(args[6]).asInteger();

    int n;
    double amorCoeff, nRate, rest, usePer;

    usePer = 1.0 / rate;

    if (usePer < 3.0)
        amorCoeff = 1.0;
    else if (usePer < 5.0)
        amorCoeff = 1.5;
    else if (usePer <= 6.0)
        amorCoeff = 2.0;
    else
        amorCoeff = 2.5;

    rate *= amorCoeff;
    nRate = calc->round(calc->mul(calc->mul(calc->yearFrac(purchaseDate, firstPeriodEndDate, basis), rate), cost), 0).asFloat();
    cost -= nRate;
    rest = cost - salvage;

    for (n = 0 ; n < period ; ++n) {
        nRate = calc->round(Value(rate * cost), 0).asFloat();
        rest -= nRate;

        if (rest < 0.0) {
            switch (period - n) {
            case 0:
            case 1:
                return calc->round(Value(cost * 0.5), 0);
            default:
                return Value(0.0);
            }
        }

        cost -= nRate;
    }

    return Value(nRate);
}

//
// Function: AMORLINC
//
// AMORLINC( Cost; purchaseDate; firstPeriodEndDate; salvage; period; basis)
//
Value func_amorlinc(valVector args, ValueCalc *calc, FuncExtra *)
{
    double cost = calc->conv()->asFloat(args[0]).asFloat();
    QDate purchaseDate = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    QDate firstPeriodEndDate = calc->conv()->asDate(args[2], nullptr).asDate(calc->settings()); // FIXME locale
    double salvage = calc->conv()->asFloat(args[3]).asFloat();
    int period = calc->conv()->asInteger(args[4]).asInteger();
    double rate = calc->conv()->asFloat(args[5]).asFloat();

    int basis = 0;
    if (args.count() > 6)
        basis = calc->conv()->asInteger(args[6]).asInteger();

    double oneRate = cost * rate;
    double costDelta = cost - salvage;
    double nullRate = calc->yearFrac(purchaseDate, firstPeriodEndDate, basis).asFloat() * rate * cost;
    int numOfFullPeriods = (cost - salvage - nullRate) / oneRate;

    double res ;

    if (period == 0)
        res = nullRate;
    else if (period <= numOfFullPeriods)
        res = oneRate;
    else if (period == numOfFullPeriods + 1)
        res = costDelta - oneRate * numOfFullPeriods - nullRate;
    else
        res = 0.0;

    return Value(res);
}

//
// Function: compound
//
// Returns value after compounded interest, given principal, rate, periods
// per year and year
//
Value func_compound(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value principal = args[0];
    Value interest = args[1];
    Value periods = args[2];
    Value years = args[3];

    // principal * pow(1+ (interest / periods), periods*years);
    Value base = calc->add(calc->div(interest, periods), 1);
    return calc->mul(principal, calc->pow(base, calc->mul(periods, years)));
}


//
// Function: continuous
//
// Returns value after continuous compounding of interest, given principal,
// rate and years
//
Value func_continuous(valVector args, ValueCalc *calc, FuncExtra *)
{
    // If you still don't understand this, let me know!  ;-)  jsinger@leeta.net
    Value principal = args[0];
    Value interest = args[1];
    Value years = args[2];

    // principal * exp(interest * years)
    return calc->mul(principal, calc->exp(calc->mul(interest, years)));
}


enum CoupBasis {
    BASIS_MSRB_30_360     = 0,
    BASIS_ACT_ACT         = 1,
    BASIS_ACT_360         = 2,
    BASIS_ACT_365         = 3,
    BASIS_30E_360         = 4,
    BASIS_30Ep_360        = 5
};

struct CoupSettings {
    int frequency; // times-per-year
    CoupBasis basis;
    bool eom; // not sure what this is...
};

static Value coup_checkparams(valVector args, ValueCalc *calc, QDate& settlement, QDate& maturity, CoupSettings& conf)
{
    settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    conf.frequency = calc->conv()->asInteger(args[2]).asInteger();

    conf.basis = BASIS_MSRB_30_360;
    conf.eom = true;

    if (args.count() > 3)
        conf.basis = static_cast<CoupBasis>(calc->conv()->asInteger(args[3]).asInteger());
    if (args.count() > 4)
        conf.eom = calc->conv()->asBoolean(args[4]).asBoolean();

    if (conf.basis < 0 || conf.basis > 5 || (conf.frequency == 0) || (12 % conf.frequency != 0)
            || settlement.daysTo(maturity) <= 0)
        return Value::errorVALUE();

    return Value();
}


static QDate coup_cd(const QDate& settlement, const QDate& maturity, int freq, bool eom, bool next)
{
    bool is_eom_special = eom && maturity.day() == maturity.daysInMonth();

    int months = 12 / freq;
    int periods = maturity.year() - settlement.year();
    if (periods > 0)
        periods = (periods - 1) * freq;

    QDate result;
    do {
        ++periods;
        result = maturity.addMonths(-periods * months);
        if (is_eom_special) {
            result.setDate(result.year(), result.month(), result.daysInMonth());
        }
    } while (settlement < result);

    if (next) {
        periods--;
        result = maturity.addMonths(-periods * months);
        if (is_eom_special) {
            result.setDate(result.year(), result.month(), result.daysInMonth());
        }
    }

    return result;
}

static int daysBetween_30E_360(const QDate& from, const QDate& to)
{
    int y1 = from.year();
    int m1 = from.month();
    int d1 = from.day();
    int y2 = to.year();
    int m2 = to.month();
    int d2 = to.day();

    if (d1 == 31)
            d1 = 30;
    if (d2 == 31)
            d2 = 30;

    return (y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1);
}

static int daysBetween_30Ep_360(const QDate& from, const QDate& to)
{
    int y1 = from.year();
    int m1 = from.month();
    int d1 = from.day();
    int y2 = to.year();
    int m2 = to.month();
    int d2 = to.day();

    if (d1 == 31)
            d1 = 30;
    if (d2 == 31) {
            d2 = 1;
            ++m2;
            /* No need to check for m2 == 13 since 12*30 == 360 */
    }

    return (y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1);
}

static int daysBetween_MSRB_30_360(const QDate& from, const QDate& to)
{
    int y1 = from.year();
    int m1 = from.month();
    int d1 = from.day();
    int y2 = to.year();
    int m2 = to.month();
    int d2 = to.day();

    if (m1 == 2 && d1 == from.daysInMonth())
            d1 = 30;
    if (m2 == 2 && d2 == to.daysInMonth())
            d2 = 30;
    if (d2 == 31 && d1 >= 30)
            d2 = 30;
    if (d1 == 31)
            d1 = 30;

    return (y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1);
}

static int daysBetweenBasis(const QDate& from, const QDate& to, CoupBasis basis)
{
    const int sign = from < to ? 1 : -1;
    const QDate f = qMin(from, to);
    const QDate t = qMax(from, to);

    switch (basis) {
    case BASIS_ACT_ACT:
    case BASIS_ACT_360:
    case BASIS_ACT_365:
        return sign * f.daysTo(t);
    case BASIS_30E_360:
        return sign * daysBetween_30E_360(f, t);
    case BASIS_30Ep_360:
        return sign * daysBetween_30Ep_360(f, t);
    case BASIS_MSRB_30_360:
    default:
        return sign * daysBetween_MSRB_30_360(f, t);
    }
}


//
// Function: COUPDAYBS
//
Value func_coupdaybs(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement, maturity;
    CoupSettings conf;

    Value res = coup_checkparams(args, calc, settlement, maturity, conf);
    if (res.isError()) {
        return res;
    }

    QDate d = coup_cd(settlement, maturity, conf.frequency, conf.eom, false);

    return Value(daysBetweenBasis(d, settlement, conf.basis));
}


static double coupdays(const QDate& settlement, const QDate& maturity, const CoupSettings& conf)
{
    switch (conf.basis) {
    case BASIS_MSRB_30_360:
    case BASIS_ACT_360:
    case BASIS_30E_360:
    case BASIS_30Ep_360:
        return 360.0 / conf.frequency;
    case BASIS_ACT_365:
        return 365.0 / conf.frequency;
    case BASIS_ACT_ACT:
    default:
            QDate next = coup_cd(settlement, maturity, conf.frequency, conf.eom, true);
            QDate prev = coup_cd(settlement, maturity, conf.frequency, conf.eom, false);
            return daysBetweenBasis(prev, next, BASIS_ACT_ACT);
    }
}

//
// Function: COUPDAYS
//
Value func_coupdays(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement, maturity;
    CoupSettings conf;

    Value res = coup_checkparams(args, calc, settlement, maturity, conf);
    if (res.isError()) {
        return res;
    }


    return Value(coupdays(settlement, maturity, conf));
}


//
// Function: COUPDAYSNC
//
Value func_coupdaysnc(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement, maturity;
    CoupSettings conf;

    Value res = coup_checkparams(args, calc, settlement, maturity, conf);
    if (res.isError()) {
        return res;
    }

    QDate d = coup_cd(settlement, maturity, conf.frequency, conf.eom, true);

    return Value(daysBetweenBasis(settlement, d, conf.basis));
}


//
// Function: COUPNCD
//
Value func_coupncd(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement, maturity;
    CoupSettings conf;

    Value res = coup_checkparams(args, calc, settlement, maturity, conf);
    if (res.isError()) {
        return res;
    }

    return Value(coup_cd(settlement, maturity, conf.frequency, conf.eom, true), calc->settings());
}


//
// Function: COUPNUM - taken from GNUMERIC
//
//
// COUPNUM ( settlement, maturity, freq, [ basis = 0 ], [ eom ] )
//
Value func_coupnum(valVector args, ValueCalc *calc, FuncExtra *)
{
    // dates and integers only - don't need high-precision for this
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    int   frequency = calc->conv()->asInteger(args[2]).asInteger();

    // defaults
    int   basis = 0;
    bool  eom   = true;

    if (args.count() > 3)
        basis = calc->conv()->asInteger(args[3]).asInteger();
    if (args.count() == 5)
        eom = calc->conv()->asBoolean(args[4]).asBoolean();

    if (basis < 0 || basis > 5 || (frequency == 0) || (12 % frequency != 0)
            || settlement.daysTo(maturity) <= 0)
        return Value::errorVALUE();

    double result;
    QDate cDate(maturity);

    int months = maturity.month() - settlement.month()
                 + 12 * (maturity.year() - settlement.year());

    cDate = cDate.addMonths(-months);

    if (eom && maturity.daysInMonth() == maturity.day()) {
        while (cDate.daysInMonth() != cDate.day())
            cDate = cDate.addDays(1);
    }

    if (settlement.day() >= cDate.day())
        --months;

    result = (1 + months / (12 / frequency));

    return Value(result);
}


//
// Function: COUPPCD
//
Value func_couppcd(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement, maturity;
    CoupSettings conf;

    Value res = coup_checkparams(args, calc, settlement, maturity, conf);
    if (res.isError()) {
        return res;
    }

    return Value(coup_cd(settlement, maturity, conf.frequency, conf.eom, false), calc->settings());
}


//
// Function: CUMIPMT
//
Value func_cumipmt(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Value rate = args[0];
    if (rate.asFloat() <= 0.0)
        return Value::errorVALUE();
    const Value nper = args[1];
    const int periods = nper.asInteger();
    if (periods <= 0)
        return Value::errorVALUE();
    const Value pv = args[2];
    if (pv.asFloat() <= 0.0)
        return Value::errorVALUE();
    const Value v1(calc->conv()->asInteger(args[3]));
    if (v1.isError())
        return Value::errorVALUE();
    const int start = v1.asInteger();
    if (start <= 0 || start > periods)
        return Value::errorVALUE();
    const Value v2(calc->conv()->asInteger(args[4]));
    if (v2.isError())
        return Value::errorVALUE();
    const int end = v2.asInteger();
    if (end < start || end > periods)
        return Value::errorVALUE();
    const Value type(calc->conv()->asInteger(args[5]));
    if (type.isError())
        return Value::errorVALUE();

    Value result(0.0);
    for (int per = start; per <= end; ++per)
        result = calc->add(result, helper_ipmt(calc, rate, Value(per), nper, pv, Value(0.0), type));

    return result;
}


//
// Function: CUMPRINC
//
Value func_cumprinc(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Value rate = args[0];
    if (rate.asFloat() <= 0.0)
        return Value::errorVALUE();
    const Value nper = args[1];
    const int periods = nper.asInteger();
    if (periods <= 0)
        return Value::errorVALUE();
    const Value pv = args[2];
    if (pv.asFloat() <= 0.0)
        return Value::errorVALUE();
    const Value v1(calc->conv()->asInteger(args[3]));
    if (v1.isError())
        return Value::errorVALUE();
    const int start = v1.asInteger();
    if (start <= 0 || start > periods)
        return Value::errorVALUE();
    const Value v2(calc->conv()->asInteger(args[4]));
    if (v2.isError())
        return Value::errorVALUE();
    const int end = v2.asInteger();
    if (end <= 0 || end < start || end > periods)
        return Value::errorVALUE();
    const Value type(calc->conv()->asInteger(args[5]));
    if (type.isError())
        return Value::errorVALUE();

    const Value pay = getPay(calc, rate, nper, pv, Value(0.0), type);
    const Value cumipmt = func_cumipmt(args, calc, 0);

    return calc->sub(calc->mul(pay, Value(end - start + 1)), cumipmt);
}


//
// Function: DB
//
// fixed-declining depreciation
//
Value func_db(valVector args, ValueCalc *calc, FuncExtra *)
{
    // This function doesn't support extended datatypes, it simply
    // converts everything to double - because it does quite a bit
    // of computing, and, well, I'm lazy to convert it all (Tomas)
    double cost = numToDouble(calc->conv()->toFloat(args[0]));
    double salvage = numToDouble(calc->conv()->toFloat(args[1]));
    double life = numToDouble(calc->conv()->toFloat(args[2]));
    double period = numToDouble(calc->conv()->toFloat(args[3]));
    double month = 12;
    if (args.count() == 5)
        month = numToDouble(calc->conv()->toFloat(args[4]));

    // sentinel check
    if (cost == 0 || life <= 0.0 || period == 0)
        return Value::errorNUM();

    if (calc->lower(calc->div(Value(salvage), Value(cost)), Value(0)))
        return Value::errorNUM();

    double rate = 1000 * (1 - pow((salvage / cost), (1 / life)));
    rate = floor(rate + 0.5)  / 1000;

    double total = cost * rate * month / 12;

    if (period == 1)
        return Value(total);

    for (int i = 1; i < life; ++i)
        if (i == period - 1)
            return Value(rate *(cost - total));
        else total += rate * (cost - total);

    return Value((cost - total) * rate *(12 - month) / 12);
}


//
// Function: DDB
//
// depreciation per period
//
Value func_ddb(valVector args, ValueCalc *calc, FuncExtra *)
{
    double cost = numToDouble(calc->conv()->toFloat(args[0]));
    double salvage = numToDouble(calc->conv()->toFloat(args[1]));
    double life = numToDouble(calc->conv()->toFloat(args[2]));
    double period = numToDouble(calc->conv()->toFloat(args[3]));
    double factor = 2;
    if (args.count() == 5)
        factor = numToDouble(calc->conv()->toFloat(args[4]));

    if (cost < 0.0 || salvage < 0.0 || life <= 0.0 || period < 0.0 || factor < 0.0)
        return Value::errorVALUE();

    double result = 0.0;

    if (factor >= life)
        // special case: amazingly gigantic depreciating rate
        result = (period > 1) ? 0 : (cost < salvage) ? 0 : cost - salvage;
    else {
        // depreciation is the value between two periods
        double invrate = (life - factor) / life;
        double current = (period == 1) ? invrate : pow(invrate, period);
        double previous = current / invrate;
        current *= cost;
        previous *= cost;
        result = previous - current;

        // should not be more than the salvage
        if (current < salvage)
            result = previous - salvage;
    }

    // can't be negative
    if (result < 0.0)
        result = 0.0;

    return Value(result);
}


//
// Function: DISC
//
Value func_disc(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale

// TODO sascha fix error value checking
//   // check dates
//   if ( settlement > maturity || )
//     return Value(false);

    Value par = args[2];
    Value redemp = args[3];

    // check parameters
    if (settlement > maturity || redemp.asFloat() <= 0.0 || par.asFloat() <= 0.0)
        return Value(false);

    int basis = 0;
    if (args.count() == 5)
        basis = calc->conv()->asInteger(args[4]).asInteger();

    /*  double y = daysPerYear (settlement, basis);
      double d = daysBetweenDates (settlement, maturity, basis);

      if ( y <= 0 || d <= 0 || basis < 0 || basis > 4 || calc->isZero (redemp) )
        return Value(false);*/

    // res=(1-(price/redemption)/yearfrac)
    return calc->div (calc->sub(Value(1.0), calc->div(par, redemp)), calc->yearFrac(settlement, maturity, basis));
}


//
// Function: DOLLARDE
//
Value func_dollarde(valVector args, ValueCalc *calc, FuncExtra *)
{
    double dollarFrac = args[0].asFloat();
    double frac = calc->conv()->asInteger(args[1]).asInteger();

    if (frac <= 0)
        return Value::errorVALUE();

    double fl;
    double res = modf(dollarFrac, &fl);

    res /= frac;
    res *= pow(10.0, ceil(log10(frac)));
    res += fl;

    return Value(res);
}


//
// Function: DOLLARFR
//
Value func_dollarfr(valVector args, ValueCalc *calc, FuncExtra *)
{
    double dollarFrac = args[0].asFloat();
    double frac = calc->conv()->asInteger(args[1]).asInteger();

    if (frac <= 0)
        return Value::errorVALUE();

    double fl;
    double res = modf(dollarFrac, &fl);

    res *= frac;
    res *= pow(10.0, -ceil(log10(frac)));
    res += fl;

    return Value(res);
}


// helper
long double duration(const QDate& settlement, const QDate& maturity,
                              const long double& coup_, const long double& yield_, const int& freq, const int& basis, const long double& numOfCoups, ValueCalc *calc)
{
    long double yield = yield_;
    long double coup = coup_;

//   debugSheetsFormula<<"DURATION_HELPER";
//   debugSheetsFormula<<"sett ="<<settlement<<" mat ="<<maturity<<" coup ="<<coup<<" yield ="<<yield<<" freq ="<<freq<<" basis ="<<basis;


    long double yearfrac = calc->yearFrac(settlement, maturity, basis).asFloat();
    long double res = 0.0l;
    const long double f100 = 100.0l;
    coup *= f100 / (long double)(freq);

    yield /= freq;
    yield += 1.0;

    long double diff = yearfrac * freq - numOfCoups;

    long double t;

    for (t = 1.0l ; t < numOfCoups ; t += 1.0l)
        res += (t + diff) * (coup) / pow(yield, t + diff);

    res += (numOfCoups + diff) * (coup + f100) / pow(yield, numOfCoups + diff);

    long double p = 0.0l;
    for (t = 1.0l ; t < numOfCoups ; t += 1.0l)
        p += coup / pow(yield, t + diff);

    p += (coup + f100) / pow(yield, numOfCoups + diff);

    res /= p;
    res /= (long double)(freq);

    return(res);
}

//
// Function: DURATION
//
//
// duration( rate, pv, fv )
//
Value func_duration(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value rate = args[0];
    Value pv   = args[1];
    Value fv   = args[2];

    if (!calc->greater(rate, Value(0.0)))
        return Value::errorVALUE();
    if (calc->isZero(fv) || calc->isZero(pv))
        return Value::errorDIV0();

    if (calc->lower(calc->div(fv, pv), Value(0)))
        return Value::errorVALUE();

    // log(fv / pv) / log(1.0 + rate)
    return calc->div(calc->ln(calc->div(fv, pv)),
                     calc->ln(calc->add(rate, Value(1.0))));
}


//
// Function: DURATION
//
//
// duration( settlement, maturity, coup, yield, freq, [basis = 0] )
//
Value func_duration_add(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale

    double coup = numToDouble(calc->conv()->toFloat(args[2]));
    double yield = numToDouble(calc->conv()->toFloat(args[3]));
    int freq = calc->conv()->asInteger(args[4]).asInteger();

    int basis = 0;
    if (args.count() > 5)
        basis = calc->conv()->asInteger(args[3]).asInteger();

    // TODO add chk_freq
    if (coup < 0.0 || yield < 0.0)
        return Value::errorVALUE();

    valVector param;
    param.append(args[0]);
    param.append(args[1]);
    param.append(args[4]);
    param.append(Value(basis));

    int numOfCoups = Value(func_coupnum(param, calc, 0)).asInteger();

    debugSheetsFormula << "DURATION";
    debugSheetsFormula << "numOfCoup =" << numOfCoups;


    return Value(duration(settlement, maturity, coup, yield, freq, basis, numOfCoups, calc));
}


//
// Function: effective
//
Value func_effective(valVector args, ValueCalc *calc, FuncExtra *)
{
    // Returns effective interest rate given nominal rate and periods per year

    Value nominal = args[0];
    Value periods = args[1];

    // base = 1 + (nominal / periods)
    // result = pow (base, periods) - 1
    Value base = calc->add(calc->div(nominal, periods), 1);
    return calc->sub(calc->pow(base, periods), 1);
}


//
// Function: EURO
//
Value func_euro(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString currency = calc->conv()->asString(args[0]).asString();
    double result = helper_eurofactor(currency);
    if (result < 0)
        return Value::errorNUM();

    return Value(result);
}


//
// EUROCONVERT(number,source,target)
//
Value func_euroconvert(valVector args, ValueCalc *calc, FuncExtra *)
{
    double number = numToDouble(calc->conv()->toFloat(args[0]));
    QString source = calc->conv()->asString(args[1]).asString();
    QString target = calc->conv()->asString(args[2]).asString();

    double factor1 = helper_eurofactor(source);
    double factor2 = helper_eurofactor(target);

    if (factor1 < 0)
        return Value::errorNUM();
    if (factor2 < 0)
        return Value::errorNUM();

    double result = number * factor2 / factor1;

    return Value(result);
}


//
// Function: FV
//
// Returns future value, given current value, interest rate and time
//
Value func_fv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value rate = args[0];
    Value nper = args[1];
    Value pmt  = args[2];

    // defaults
    Value pv(0.0);
    int type = 0;

    if (args.count() > 3)
        pv = Value(calc->conv()->asFloat(args[3]).asFloat());

    if (args.count() == 5)
        type = calc->conv()->asInteger(args[4]).asInteger();

    //TODO check payType

    Value pvif = Value(pow1p(rate.asFloat(), nper.asFloat()));
    Value fvifa = calc_fvifa(calc, rate, nper);

    Value res(calc->mul(Value(-1), calc->add(calc->mul(pv, pvif), calc->mul(pmt, calc->mul(calc->add(Value(1), calc->mul(rate, type)), fvifa)))));

    return (res);
    // present * pow (1 + interest, periods)
//   return calc->mul (present, calc->pow (calc->add (interest, 1), periods));
}

//
// Function: FVSCHEDULE
//
// Returns future value
//
Value func_fvschedule(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value pv       = args[0];
    Value schedule = args[1];

    int n = schedule.count();
    int i;

    Value v;
    Value res(pv);

    for (i = 0; i < n; ++i) {
        v = args[1].element(i);
        res = Value(calc->mul(res, calc->add(Value(1), v)));
    }

    return (res);
}


//
// Function: FV_annuity
//
Value func_fv_annuity(valVector args, ValueCalc *calc, FuncExtra *)
{
    /* Returns future value of an annuity or cash flow, given payment, interest
       rate and periods */

    Value amount = args[0];
    Value interest = args[1];
    Value periods = args[2];

    // pw = pow (1 + interest, periods)
    // result = amount * ((pw - 1) / interest)
    Value pw = calc->pow(calc->add(interest, 1), periods);
    return calc->mul(amount, calc->div(calc->sub(pw, 1), interest));
}


//
// Function: INTRATE
//
Value func_intrate(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale

    Value invest = args[2];
    Value redemption = args[3];

    int basis = 0;
    if (args.count() == 5)
        basis = calc->conv()->asInteger(args[4]).asInteger();

    double d = daysBetweenDates(settlement, maturity, basis);
    double y = daysPerYear(settlement, basis);

    if (d <= 0 || y <= 0 || calc->isZero(invest) || basis < 0 || basis > 4)
        return Value::errorVALUE();

    // (redemption - invest) / invest * (y / d)
    return calc->mul(calc->div(calc->sub(redemption, invest), invest), y / d);
}


//
// Function: IPMT
//
Value func_ipmt(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value rate = args[0];
    Value per  = args[1];
    Value nper = args[2];
    Value pv   = args[3];

    Value fv = Value(0.0);
    Value type = Value(0);
    if (args.count() > 4) fv = args[4];
    if (args.count() == 6) type = args[5];

    return helper_ipmt(calc, rate, per, nper, pv, fv, type);
}

static double irrResult(Value sec, ValueCalc *calc, double rate)
{
    double res = 0;
    for (unsigned i = 0; i < sec.count(); ++i) {
        double val = calc->conv()->asFloat(sec.element(i)).asFloat();
        res += val / pow(1.0 + rate, double(i));
    }
    return res;
}

static double irrResultDerive(Value sec, ValueCalc *calc, double rate)
{
    double res = 0;
    for (unsigned i = 0; i < sec.count(); ++i) {
        double val = calc->conv()->asFloat(sec.element(i)).asFloat();
        res += -double(i) * val / pow(1.0 + rate, double(i + 1));
    }
    return res;
}

//
// Function: IRR
//
Value func_irr(valVector args, ValueCalc *calc, FuncExtra *)
{
    static const double maxEpsilon = 1e-10;
    static const int maxIter = 50;

    Value seq = args[0];

    double rate = 0.1;
    if (args.count() > 1) rate = calc->conv()->asFloat(args[1]).asFloat();

    bool contLoop;
    int i = 0;
    do {
        double newRate = rate - irrResult(seq, calc, rate) / irrResultDerive(seq, calc, rate);
        double rateEpsilon = fabs(newRate - rate);
        rate = newRate;
        contLoop = (rateEpsilon > maxEpsilon) && (fabs(rate) > maxEpsilon);
    } while (contLoop && (++i < maxIter));

    return Value(rate);
}

//
// Function: ISPMT
//
Value func_ispmt(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value rate = args[0];
    Value per  = args[1];
    Value nper = args[2];
    Value pv   = args[3];

    if (calc->lower(per, Value(1)) || calc->greater(per, nper))
        return Value::errorVALUE();

    // d = -pv * rate
    Value d = calc->mul(calc->mul(pv, Value(-1)), rate);

    // d - (d / nper * per)
    return calc->sub(d, calc->mul(calc->div(d, nper), per));
}


//
// Function: MDURATION
//
//
// duration( settlement, maturity, coup, yield, freq, [basis = 0] )
//
Value func_mduration(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale

    double coup = numToDouble(calc->conv()->toFloat(args[2]));
    double yield = numToDouble(calc->conv()->toFloat(args[3]));
    int freq = calc->conv()->asInteger(args[4]).asInteger();

    int basis = 0;
    if (args.count() > 5)
        basis = calc->conv()->asInteger(args[3]).asInteger();

    // TODO add chk_freq
    if (coup < 0.0 || yield < 0.0)
        return Value::errorVALUE();

    valVector param;
    param.append(args[0]);
    param.append(args[1]);
    param.append(args[4]);
    param.append(Value(basis));

    int numOfCoups = Value(func_coupnum(param, calc, 0)).asInteger();

    double res = duration(settlement, maturity, coup, yield, freq, basis, numOfCoups, calc);
    res /= 1.0 + (yield / double(freq));

    return Value(res);
}


//
// Function: MIRR
//
Value func_mirr(valVector args, ValueCalc *calc, FuncExtra *)
{
    long double inv = calc->conv()->asFloat(args[1]).asFloat();
    long double reInvRate = calc->conv()->asFloat(args[2]).asFloat();

    long double npv_pos, npv_neg;
    Value v;
    int n = args[0].count();
    int i;

    for (i = 0, npv_pos = npv_neg = 0; i < n; ++i) {
        v = args[0].element(i);
        if (v.asFloat() >= 0)
            npv_pos += v.asFloat() / pow1p(reInvRate, i);
        else
            npv_neg += v.asFloat() / pow1p(inv, i);
    }

    if (npv_neg == 0 || npv_pos == 0 || reInvRate <= -1.0l)
        return Value::errorVALUE();

    long double res = std::pow((-npv_pos * pow1p(reInvRate, n)) / (npv_neg * (1 + reInvRate)), (1.0l / (n - 1))) - 1.0l;

    return Value(res);
}


//
// Function: level_coupon
//
Value func_level_coupon(valVector args, ValueCalc *calc, FuncExtra *)
{
    // Returns effective interest rate given nominal rate and periods per year
    Value face = args[0];
    Value coupon_rate = args[1];
    Value coupon_year = args[2];
    Value years = args[3];
    Value market_rate = args[4];

    Value coupon, interest, pw, pv_annuity;
    // coupon = coupon_rate * face / coupon_year
    // interest = market_rate / coupon_year
    // pw = pow(1 + interest, years * coupon_year)
    // pv_annuity = (1 - 1 / pw) / interest
    // result = coupon * pv_annuity + face / pw
    coupon = calc->mul(coupon_rate, calc->div(face, coupon_year));
    interest = calc->div(market_rate, coupon_year);
    pw = calc->pow(calc->add(interest, Value(1)), calc->mul(years, coupon_year));
    pv_annuity = calc->div(calc->sub(Value(1), calc->div(Value(1), pw)), interest);
    return calc->add(calc->mul(coupon, pv_annuity), calc->div(face, pw));
}


//
// Function: nominal
//
Value func_nominal(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value effective = args[0];
    Value periods = args[1];

    // sentinel checks
    if (calc->isZero(periods))
        return Value::errorDIV0();
    if (!calc->greater(periods, Value(0.0)))
        return Value::errorVALUE();
    if (calc->isZero(effective))
        return Value::errorVALUE();
    if (!calc->greater(effective, Value(0.0)))
        return Value::errorVALUE();

    // pw = pow (effective + 1, 1 / periods)
    // result = periods * (pw - 1);
    Value pw;
    pw = calc->pow(calc->add(effective, Value(1)), calc->div(Value(1), periods));
    return calc->mul(periods, calc->sub(pw, Value(1)));
}


//
// Function: NPER
//
Value func_nper(valVector args, ValueCalc *calc, FuncExtra *)
{
    double rate = calc->conv()->asFloat(args[0]).asFloat();
    double pmt = calc->conv()->asFloat(args[1]).asFloat();
    double pv = calc->conv()->asFloat(args[2]).asFloat();

    // defaults
    double fv = 0.0;
    double type = 0;

    // opt. params
    if (args.count() > 3) fv = calc->conv()->asFloat(args[3]).asFloat();
    if (args.count() == 5) type = calc->conv()->asFloat(args[4]).asFloat();

    // if rate is 0, then NPER solves this
    //   PV = -FV -( Payment*NPER )
    if (rate == 0.0)
        return Value(-(pv + fv) / pmt);

    if (type > 0)
        return Value(log(-(rate*fv - pmt*(1.0 + rate)) / (rate*pv + pmt*(1.0 + rate))) / log(1.0 + rate));
    else
        return Value(log(-(rate*fv - pmt) / (rate*pv + pmt)) / log(1.0 + rate));
}


//
// NPV
//
Value func_npv(valVector args, ValueCalc* calc, FuncExtra*)
{
    Value result(Value::Array);
    result.setElement(0, 0, Value(0.0));     // actual result
    result.setElement(1, 0, Value(1.0));     // counter
    if (args.count() == 2) {
        Value vector = args[1]; // may be an array
        calc->arrayWalk(vector, result, awNpv, calc->conv()->asFloat(args[0]));
    } else {
        valVector vector = args.mid(1);
        calc->arrayWalk(vector, result, awNpv, calc->conv()->asFloat(args[0]));
    }
    return result.element(0, 0);
}

static double date_ratio(const QDate& d1, const QDate& d2, const QDate& d3, const CoupSettings& conv)
{
    QDate next = coup_cd(d1, d3, conv.frequency, conv.eom, true);
    QDate prev = coup_cd(d1, d3, conv.frequency, conv.eom, false);

    if (next >= d2) {
        return daysBetweenBasis(d1, d2, conv.basis) /
                coupdays(prev, next, conv);
    }

    double res = daysBetweenBasis(d1, next, conv.basis) /
            coupdays(prev, next, conv);

    while (true) {
        prev = next;
        next = next.addMonths(12 / conv.frequency);
        //if (!next.isValid())
            //
        if (next >= d2) {
            res += daysBetweenBasis(prev, d2, conv.basis) /
                        coupdays(prev, next, conv);
            return res;
        }
        res += 1;
    }
}

//
// ODDLPRICE
//
Value func_oddlprice(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    QDate last = calc->conv()->asDate(args[2], nullptr).asDate(calc->settings()); // FIXME locale
    double rate = calc->conv()->asFloat(args[3]).asFloat();
    double yield = calc->conv()->asFloat(args[4]).asFloat();
    double redemp = calc->conv()->asFloat(args[5]).asFloat();
    double freq = calc->conv()->asFloat(args[6]).asFloat();

    // opt. basis
    int basis = 0;
    if (args.count() > 7)
        basis = calc->conv()->asInteger(args[7]).asInteger();

    CoupSettings conv;
    conv.basis = static_cast<CoupBasis>(basis);
    conv.frequency = freq;
    conv.eom = true;

//   debugSheetsFormula<<"ODDLPRICE";
//   debugSheetsFormula<<"settlement ="<<settlement<<" maturity="<<maturity<<" last="<<last<<" rate="<<rate<<" yield="<<yield<<" redemp="<<redemp<<" freq="<<freq<<" basis="<<basis;

    // TODO check frequency
    if (yield <= 0.0 || rate <= 0.0 || maturity <= settlement || settlement <= last)
        return Value::errorVALUE();

    QDate d = last;
    do {
        d = d.addMonths(12 / conv.frequency);
    } while (d.isValid() && d < maturity);

    double x1 = date_ratio(last, settlement, d, conv);
    double x2 = date_ratio(last, maturity, d, conv);
    double x3 = date_ratio(settlement, maturity, d, conv);

    return Value((redemp * conv.frequency +
              100 * rate * (x2 - x1 * (1 + yield * x3 / conv.frequency))) /
              (yield * x3 + conv.frequency));
}


//
// ODDLYIELD
//
Value func_oddlyield(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    QDate last = calc->conv()->asDate(args[2], nullptr).asDate(calc->settings()); // FIXME locale
    double rate = calc->conv()->asFloat(args[3]).asFloat();
    double price = calc->conv()->asFloat(args[4]).asFloat();
    double redemp = calc->conv()->asFloat(args[5]).asFloat();
    double freq = calc->conv()->asFloat(args[6]).asFloat();

    // opt. basis
    int basis = 0;
    if (args.count() > 7)
        basis = calc->conv()->asInteger(args[7]).asInteger();

    CoupSettings conv;
    conv.basis = static_cast<CoupBasis>(basis);
    conv.frequency = freq;
    conv.eom = true;

//   debugSheetsFormula<<"ODDLYIELD";
//   debugSheetsFormula<<"settlement ="<<settlement<<" maturity="<<maturity<<" last="<<last<<" rate="<<rate<<" price="<<price<<" redemp="<<redemp<<" freq="<<freq<<" basis="<<basis;

    // TODO check frequency
    if (rate < 0.0 || price <= 0.0 || maturity <= settlement || settlement <= last)
        return Value::errorVALUE();


    QDate d = last;
    do {
        d = d.addMonths(12 / conv.frequency);
    } while (d.isValid() && d < maturity);

    double x1 = date_ratio(last, settlement, d, conv);
    double x2 = date_ratio(last, maturity, d, conv);
    double x3 = date_ratio(settlement, maturity, d, conv);


    return Value((conv.frequency * (redemp - price) + 100 * rate * (x2 - x1)) /
            (x3 * price + 100 * rate * x1 * x3 / conv.frequency));
}


//
// Function: PMT
//
Value func_pmt(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value rate = args[0];
    Value nper = args[1];
    Value pv   = args[2];
    Value fv = Value(0.0);
    Value type = Value(0);
    if (args.count() > 3) fv = args[3];
    if (args.count() == 5) type = args[4];

    return getPay(calc, rate, nper, pv, fv, type);
}


//
// Function: PPMT
//
// Uses IPMT.
//
Value func_ppmt(valVector args, ValueCalc *calc, FuncExtra *)
{
    // Docs partly copied from OO.
    //
    // PPMT(Rate;Period;NPER;PV;FV;Type)
    //
    // Rate is the periodic interest rate.
    // Period is the amortizement period. P=1 for the first and P=NPER for the last period.
    // NPER is the total number of periods during which annuity is paid.
    // PV is the present value in the sequence of payments.
    // FV (optional) is the desired (future) value.
    // Type (optional) defines the due date. F=1 for payment at the beginning of a period and F=0 for payment at the end of a period.
    //

    Value rate = args[0];
    Value per  = args[1];
    Value nper = args[2];
    Value pv   = args[3];

    // defaults
    Value fv = Value(0.0);
    Value type = Value(0);

    if (args.count() > 4) fv = args[4];
    if (args.count() == 6) type = args[5];

    debugSheets << "Type=" << type;

    Value pay  = getPay(calc, rate, nper, pv, fv, type);
    Value ipmt = func_ipmt(args, calc, 0);
    return calc->sub(pay, ipmt);
}


//
// PRICEMAT
//
Value func_pricemat(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    QDate issue = calc->conv()->asDate(args[2], nullptr).asDate(calc->settings()); // FIXME locale
    double rate = calc->conv()->asFloat(args[3]).asFloat();
    double yield = calc->conv()->asFloat(args[4]).asFloat();

    // opt. basis
    int basis = 0;
    if (args.count() > 5)
        basis = calc->conv()->asInteger(args[5]).asInteger();

    //debugSheetsFormula<<"PRICEMAT";
    //debugSheetsFormula<<"settlement ="<<settlement<<" maturity="<<maturity<<" issue="<<issue<<" rate="<<rate<<" yield="<<yield<<" basis="<<basis;

    if (rate < 0.0 || yield < 0.0 || settlement >= maturity)
        return Value::errorVALUE();

    long double y = daysPerYear(settlement, basis);
    if (!y) return Value::errorVALUE();
    long double issMat = daysBetweenDates(issue, maturity, basis) / y;
    long double issSet = daysBetweenDates(issue, settlement, basis) / y;
    long double setMat = daysBetweenDates(settlement, maturity, basis) / y;

    long double res = 1.0l + issMat * rate;
    res /= 1.0l + setMat * yield;
    res -= issSet * rate;
    res *= 100.0l;

    return Value(res);
}


//
// Function: PV
//
// Returns present value, given future value, interest rate and years
//
Value func_pv(valVector args, ValueCalc *calc, FuncExtra *)
{
    double rate = calc->conv()->asFloat(args[0]).asFloat();
    double nper = calc->conv()->asFloat(args[1]).asFloat();
    double pmt = calc->conv()->asFloat(args[2]).asFloat();

    double fv = 0;
    int type = 0;

    if (args.count() > 3)
        fv = calc->conv()->asFloat(args[3]).asFloat();
    if (args.count() > 4)
        type = calc->conv()->asInteger(args[4]).asInteger();
    // TODO error Value checking for type


    double pvif = pow(1 + rate, nper);
    double fvifa = (pvif - 1) / rate;

    if (pvif == 0)
        return Value::errorDIV0();

    double res = (-fv - pmt * (1.0 + rate * type) * fvifa) / pvif;

    return Value(res);
}


//
// Function: PV_annuity
//
Value func_pv_annuity(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value amount = args[0];
    Value interest = args[1];
    Value periods = args[2];

    // recpow = 1 / pow (1 + interest, periods)
    // result = amount * (1 - recpow) / interest;
    Value recpow;
    recpow = calc->div(Value(1), calc->pow(calc->add(interest, Value(1)), periods));
    return calc->mul(amount, calc->div(calc->sub(Value(1), recpow), interest));
}


//
// Function: RATE
//
Value func_rate(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Value nper = args[0];
    double fNper = calc->conv()->asFloat(nper).asFloat();
    double fPayment = calc->conv()->asFloat(args[1]).asFloat();
    double fPv = calc->conv()->asFloat(args[2]).asFloat();
    double fFv = (args.count() > 3) ? calc->conv()->asFloat(args[3]).asFloat() : 0;
    double fPayType = (args.count() > 4) ? calc->conv()->asFloat(args[4]).asFloat() : 0;
    double fGuess = (args.count() > 5) ? calc->conv()->asFloat(args[5]).asFloat() : 0.1;

    if (fNper <= 0.0)
        return Value::errorVALUE();

    bool bValid = true, bFound = false;
    double fX, fXnew, fTerm, fTermDerivation;
    double fGeoSeries, fGeoSeriesDerivation;
    const unsigned short nIterationsMax = 150;
    unsigned short nCount = 0;
    const double fEpsilonSmall = 1.0E-14;
    const double fEpsilon = 1.0E-7;

    fFv -= fPayment * fPayType;
    fPv += fPayment * fPayType;
    if (nper.isInteger()) {
        fX = fGuess;
        double fPowN, fPowNminus1;
        while (!bFound && nCount < nIterationsMax) {
            fPowNminus1 = pow( 1.0+fX, fNper-1.0);
            fPowN = fPowNminus1 * (1.0+fX);
            if (calc->approxEqual(Value(fabs(fX)), Value(0.0))) {
                fGeoSeries = fNper;
                fGeoSeriesDerivation = fNper * (fNper-1.0)/2.0;
            } else {
                fGeoSeries = (fPowN-1.0)/fX;
                fGeoSeriesDerivation = fNper * fPowNminus1 / fX - fGeoSeries / fX;
            }
            fTerm = fFv + fPv *fPowN+ fPayment * fGeoSeries;
            fTermDerivation = fPv * fNper * fPowNminus1 + fPayment * fGeoSeriesDerivation;
            if (fabs(fTerm) < fEpsilonSmall) {
                bFound = true;
            } else {
                if (calc->approxEqual(Value(fabs(fTermDerivation)), Value(0.0)))
                    fXnew = fX + 1.1 * fEpsilon;
                else
                    fXnew = fX - fTerm / fTermDerivation;
                ++nCount;
                bFound = (fabs(fXnew - fX) < fEpsilon);
                fX = fXnew;
            }
        }
        bValid =(fX >=-1.0);
    } else {
        fX = (fGuess < -1.0) ? -1.0 : fGuess;
        while (bValid && !bFound && nCount < nIterationsMax) {
            if (calc->approxEqual(Value(fabs(fX)), Value(0.0))) {
                fGeoSeries = fNper;
                fGeoSeriesDerivation = fNper * (fNper-1.0)/2.0;
            } else {
                fGeoSeries = (pow( 1.0+fX, fNper) - 1.0) / fX;
                fGeoSeriesDerivation = fNper * pow( 1.0+fX, fNper-1.0) / fX - fGeoSeries / fX;
            }
            fTerm = fFv + fPv *pow(1.0 + fX,fNper)+ fPayment * fGeoSeries;
            fTermDerivation = fPv * fNper * pow(1.0+fX, fNper-1.0) + fPayment * fGeoSeriesDerivation;
            if (fabs(fTerm) < fEpsilonSmall) {
                bFound = true;
            } else {
                if (calc->approxEqual(Value(fabs(fTermDerivation)), Value(0.0)))
                    fXnew = fX + 1.1 * fEpsilon;
                else
                    fXnew = fX - fTerm / fTermDerivation;
                ++nCount;
                bFound = (fabs(fXnew - fX) < fEpsilon);
                fX = fXnew;
                bValid = (fX >= -1.0);
            }
        }
    }
    fGuess = fX;
    // ODF specs do not say that we should return an error in that case
    //if (!bValid || !bFound) return Value::errorVALUE();
    return Value(fGuess);
}


//
// Function: RECEIVED
//
Value func_received(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale

    Value investment = args[2];
    Value discount = args[3];

    int basis = 0;
    if (args.count() == 5)
        basis = calc->conv()->asInteger(args[4]).asInteger();

    double d = daysBetweenDates(settlement, maturity, basis);
    double y = daysPerYear(settlement, basis);

    if (d <= 0 || y <= 0 || basis < 0 || basis > 4)
        return Value(false);

    // 1.0 - ( discount * d / y )
    Value x = calc->sub(Value(1.0), (calc->mul(discount, d / y)));

    if (calc->isZero(x))
        return Value::errorVALUE();
    return calc->div(investment, x);
}


//
// Function: RRI
//
Value func_rri(valVector args, ValueCalc *calc, FuncExtra *)
{
    double p = calc->conv()->asFloat(args[0]).asFloat();
    double pv = calc->conv()->asFloat(args[1]).asFloat();
    double fv = calc->conv()->asFloat(args[2]).asFloat();

    // constraints N>0
    if (p < 1)
        return Value::errorVALUE();

    double res = pow((fv / pv), 1 / p) - 1;
    return Value(res);
}


//
// Function: SLN
//
// straight-line depreciation for a single period
//
Value func_sln(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value cost = args[0];
    Value salvage_value = args[1];
    Value life = args[2];

    // sentinel check
    if (!calc->greater(life, Value(0.0)))
        return Value::errorVALUE();

    // (cost - salvage_value) / life
    return calc->div(calc->sub(cost, salvage_value), life);
}


//
// Function: SYD
//
// sum-of-years digits depreciation
//
Value func_syd(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value cost = args[0];
    Value salvage_value = args[1];
    Value life = args[2];
    Value period = args[3];

    // sentinel check
    if (!calc->greater(life, Value(0.0)))
        return Value::errorVALUE();

    // v1 = cost - salvage_value
    // v2 = life - period + 1
    // v3 = life * (life + 1.0)
    // result = (v1 * v2 * 2) / v3
    Value v1, v2, v3;
    v1 = calc->sub(cost, salvage_value);
    v2 = calc->add(calc->sub(life, period), 1);
    v3 = calc->mul(life, calc->add(life, 1.0));
    return calc->div(calc->mul(calc->mul(v1, v2), 2), v3);
}



//
// Function: TBILLEQ
//
Value func_tbilleq(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    double discount = calc->conv()->asFloat(args[2]).asFloat();

    maturity = maturity.addDays(1);
    int days = calc->days360(settlement, maturity, false);  // false -> US

    if (settlement >= maturity || discount <= 0.0 || days > 360)
        return Value::errorVALUE();

    double res = (365 * discount) / (360 - (discount * double(days)));

    return Value(res);
}


//
// Function: TBILLPRICE
//
Value func_tbillprice(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    Value discount = args[2];

    Value fraction = calc->yearFrac(settlement, maturity.addDays(1), 0); // basis: USA 30/360
    double dummy;

    if (modf(fraction.asFloat(), &dummy) == 0.0)
        return Value::errorVALUE();

//   double days = settlement.daysTo( maturity );
// TODO error value checking
//   if (settlement > maturity || calc->lower (discount, Value(0)) || days > 265)
//     return Value::errorVALUE();

    return calc->mul(Value(100.0), (calc->sub(Value(1.0), calc->mul(discount, fraction))));
}


//
// Function: TBILLYIELD
//
Value func_tbillyield(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    double price = calc->conv()->asFloat(args[2]).asFloat();

    double days = calc->days360(settlement, maturity, false);  // false -> US
    ++days;

    if (settlement >= maturity || days > 360 || price <= 0.0)
        return Value::errorVALUE();

    double res = 100.0;
    res /= price;
    res--;
    res /= days;
    res *= 360.0;

    return Value(res);
}


//
// Function: VDB
//
// VDB( cost; salvage; life; startPeriod; endPeriod [; depreciation-factor = 2 [; straight-line depreciation = TRUE ] )
//
Value func_vdb(valVector args, ValueCalc *calc, FuncExtra *)
{
    double cost = calc->conv()->asFloat(args[0]).asFloat();
    double salvage = calc->conv()->asFloat(args[1]).asFloat();
    double life = calc->conv()->asFloat(args[2]).asFloat();
    double startPeriod = calc->conv()->asFloat(args[3]).asFloat();
    double endPeriod = calc->conv()->asFloat(args[4]).asFloat();

    // defaults
    double depreciationFactor = 2;
    bool flag = false;

    // opt. parameter
    if (args.count() > 6)
        flag = calc->conv()->asInteger(args[6]).asInteger();
    if (args.count() >= 5)
        depreciationFactor = calc->conv()->asFloat(args[5]).asFloat();

    // check if parameters are valid
    if (cost < 0.0 || endPeriod < startPeriod || endPeriod > life || cost < 0.0 || salvage > cost || depreciationFactor <= 0.0)
        return Value::errorVALUE();

    // calc loop start and end
    double         intStart = floor(startPeriod);
    double         intEnd   = ceil(endPeriod);
    unsigned long loopStart = (unsigned long) intStart;
    unsigned long loopEnd   = (unsigned long) intEnd;

    double res = 0.0;

    if (flag) {
        // no straight-line depreciation
        for (unsigned long i = loopStart + 1; i <= loopEnd; ++i) {
            double term = vdbGetGDA(cost, salvage, life, (double) i, depreciationFactor);

            //
            if (i == loopStart + 1)
                term *= (fmin(endPeriod, intStart + 1.0) - startPeriod);
            else if (i == loopEnd)
                term *= (endPeriod + 1.0 - intEnd);

            res += term;
        }
    } else {
        double life1 = life;
        double part;

        if (startPeriod != floor(startPeriod)) {
            if (depreciationFactor > 1) {
                if (startPeriod >= life / 2 || startPeriod == life / 2) {
                    part        = startPeriod - life / 2;
                    startPeriod = life / 2;
                    endPeriod  -= part;
                    life1      += 1;
                }
            }
        }

        cost -= vdbInterVDB(cost, salvage, life, life1, startPeriod, depreciationFactor);
        res   = vdbInterVDB(cost, salvage, life, life - startPeriod, endPeriod - startPeriod, depreciationFactor);
    } // end not flag

    return Value(res);
}


//
// Function: XIRR
//
// Compute the internal rate of return for a non-periodic series of cash flows.
//
// XIRR ( Values; Dates; [ Guess = 0.1 ] )
//

// TODO sascha check dates
Value func_xirr(valVector args, ValueCalc *calc, FuncExtra *)
{
    double resultRate = 0.1;
    if (args.count() > 2)
        resultRate = calc->conv()->asFloat(args[2]).asFloat();

    // check pairs and count >= 2 and guess > -1.0
    if (args[0].count() != args[1].count() || args[1].count() < 2 || resultRate <= -1.0)
        return Value::errorVALUE();

    // define max epsilon
    static const double maxEpsilon = 1e-10;

    // max number of iterations
    static const int maxIter = 50;

    // Newton's method - try to find a res, with a accuracy of maxEpsilon
    double newRate, rateEpsilon, resultValue;
    int i = 0;
    bool contLoop;

    do {
        resultValue = xirrResult(args, calc, resultRate);
        newRate =  resultRate - resultValue / xirrResultDerive(args, calc, resultRate);
        rateEpsilon = fabs(newRate - resultRate);
        resultRate = newRate;
        contLoop = (rateEpsilon > maxEpsilon) && (fabs(resultValue) > maxEpsilon);
    } while (contLoop && (++i < maxIter));

    if (contLoop)
        return Value::errorVALUE();

    return Value(resultRate);
}

//
// Function: xnpv
//
// Compute the net present value of a series of cash flows.
//
// XNPV ( Rate; Values; Dates )
//
Value func_xnpv(valVector args, ValueCalc *calc, FuncExtra *)
{
    double rate = calc->conv()->asFloat(args[0]).asFloat();
    ++rate;

    int numValues = args[1].count();
    int numDates  = args[2].count();

    double res = 0;

    // check pairs
    if (numValues != numDates || numValues < 2)
        return Value::errorVALUE();
    // check rate
    if (rate < -1.0)
        return Value::errorNUM();

    QDate date0 = calc->conv()->asDate(args[2].element(0), nullptr).asDate(calc->settings()); // FIXME locale
    double val;
    QDate date;

    for (int i = 0; i < numValues; ++i) {
        val  = calc->conv()->asFloat(args[1].element(i)).asFloat();
        Value tmpDate(calc->conv()->asDate(args[2].element(i), nullptr)); // FIXME locale

        if (tmpDate.isError()) return tmpDate;
        date = tmpDate.asDate(calc->settings());

        // check if date is valid
        if (!date.isValid())
            return Value::errorNUM();

        int days = date0.daysTo(date);

        res += val / pow(rate, days / 365.0);
    }

    return Value(res);
}


//
// YIELDDISC
//
Value func_yielddisc(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    double price = calc->conv()->asFloat(args[2]).asFloat();
    double redemp = calc->conv()->asFloat(args[3]).asFloat();

    // opt. basis
    int basis = 0;
    if (args.count() > 4)
        basis = calc->conv()->asInteger(args[4]).asInteger();

    if (price <= 0.0 || redemp <= 0.0 || settlement >= maturity)
        return Value::errorVALUE();

    Value res = Value((redemp / price) - 1.0);
    res = calc->div(res, calc->yearFrac(settlement, maturity, basis));

    return Value(res);
}


//
// YIELDMAT
//
Value func_yieldmat(valVector args, ValueCalc *calc, FuncExtra *)
{
    QDate settlement = calc->conv()->asDate(args[0], nullptr).asDate(calc->settings()); // FIXME locale
    QDate maturity = calc->conv()->asDate(args[1], nullptr).asDate(calc->settings()); // FIXME locale
    QDate issue = calc->conv()->asDate(args[2], nullptr).asDate(calc->settings()); // FIXME locale
    long double rate = calc->conv()->asFloat(args[3]).asFloat();
    long double price = calc->conv()->asFloat(args[4]).asFloat();

    // opt. basis
    int basis = 0;
    if (args.count() > 5)
        basis = calc->conv()->asInteger(args[5]).asInteger();

    if (price <= 0.0 || rate <= 0.0 || settlement >= maturity)
        return Value::errorVALUE();

    Value issMat = calc->yearFrac(issue, maturity, basis);
    Value issSet = calc->yearFrac(issue, settlement, basis);
    Value setMat = calc->yearFrac(settlement, maturity, basis);

    Value res = calc->add(Value(1.0l), calc->mul(issMat, rate));
    res = calc->div(res, calc->add(calc->div(price, Value(100.0l)), calc->mul(issSet, rate)));
    res = calc->sub (res, 1);
    res = calc->div(res, setMat);

    return res;
}

//
// Function: zero_coupon
//
Value func_zero_coupon(valVector args, ValueCalc *calc, FuncExtra *)
{
    // Returns effective interest rate given nominal rate and periods per year

    Value face = args[0];
    Value rate = args[1];
    Value years = args[2];

    // face / pow(1 + rate, years)
    return calc->div(face, calc->pow(calc->add(rate, 1), years));
}

#include "financial.moc"
