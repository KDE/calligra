/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


// built-in text functions
#include "TextModule.h"

// please keep it in alphabetical order
#include <QRegExp>
#include <kdebug.h>
#include <math.h>

#include "CalculationSettings.h"
#include "Function.h"
#include "FunctionModuleRegistry.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <KGenericFactory>
#include <KLocale>

using namespace KSpread;

// Functions DOLLAR and FIXED convert data to double, hence they will not
// support arbitrary precision, when it will be introduced.

// prototypes
Value func_asc(valVector args, ValueCalc *calc, FuncExtra *);
Value func_char(valVector args, ValueCalc *calc, FuncExtra *);
Value func_clean(valVector args, ValueCalc *calc, FuncExtra *);
Value func_code(valVector args, ValueCalc *calc, FuncExtra *);
Value func_compare(valVector args, ValueCalc *calc, FuncExtra *);
Value func_concatenate(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dollar(valVector args, ValueCalc *calc, FuncExtra *);
Value func_exact(valVector args, ValueCalc *calc, FuncExtra *);
Value func_find(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fixed(valVector args, ValueCalc *calc, FuncExtra *);
Value func_jis(valVector args, ValueCalc *calc, FuncExtra *);
Value func_left(valVector args, ValueCalc *calc, FuncExtra *);
Value func_len(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lower(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mid(valVector args, ValueCalc *calc, FuncExtra *);
Value func_proper(valVector args, ValueCalc *calc, FuncExtra *);
Value func_regexp(valVector args, ValueCalc *calc, FuncExtra *);
Value func_regexpre(valVector args, ValueCalc *calc, FuncExtra *);
Value func_replace(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rept(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rot13(valVector args, ValueCalc *calc, FuncExtra *);
Value func_right(valVector args, ValueCalc *calc, FuncExtra *);
Value func_search(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sleek(valVector args, ValueCalc *calc, FuncExtra *);
Value func_substitute(valVector args, ValueCalc *calc, FuncExtra *);
Value func_t (valVector args, ValueCalc *calc, FuncExtra *);
Value func_text(valVector args, ValueCalc *calc, FuncExtra *);
Value func_toggle(valVector args, ValueCalc *calc, FuncExtra *);
Value func_trim(valVector args, ValueCalc *calc, FuncExtra *);
Value func_unichar(valVector args, ValueCalc *calc, FuncExtra *);
Value func_unicode(valVector args, ValueCalc *calc, FuncExtra *);
Value func_upper(valVector args, ValueCalc *calc, FuncExtra *);
Value func_value(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bahttext(valVector args, ValueCalc *calc, FuncExtra *);


KSPREAD_EXPORT_FUNCTION_MODULE("text", TextModule)


TextModule::TextModule(QObject* parent, const QVariantList&)
        : FunctionModule(parent)
{
    Function *f;

    // one-parameter functions
    f = new Function("ASC", func_asc);
    add(f);
    f = new Function("CHAR", func_char);
    add(f);
    f = new Function("CLEAN", func_clean);
    add(f);
    f = new Function("CODE", func_code);
    add(f);
    f = new Function("JIS", func_jis);
    add(f);
    f = new Function("LEN", func_len);
    add(f);
    f = new Function("LOWER", func_lower);
    add(f);
    f = new Function("PROPER", func_proper);
    add(f);
    f = new Function("ROT13", func_rot13);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.DATEFUNCTIONS.GETROT13");
    add(f);
    f = new Function("SLEEK", func_sleek);
    add(f);
    f = new Function("T", func_t);
    add(f);
    f = new Function("TOGGLE", func_toggle);
    add(f);
    f = new Function("TRIM", func_trim);
    add(f);
    f = new Function("UNICHAR", func_unichar);
    add(f);
    f = new Function("UNICODE", func_unicode);
    add(f);
    f = new Function("UPPER", func_upper);
    add(f);
    f = new Function("VALUE", func_value);
    add(f);

    // other functions
    f = new Function("COMPARE", func_compare);
    f->setParamCount(3);
    add(f);
    f = new Function("CONCATENATE", func_concatenate);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("DOLLAR", func_dollar);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("EXACT", func_exact);
    f->setParamCount(2);
    add(f);
    f = new Function("FIND", func_find);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("FIXED", func_fixed);
    f->setParamCount(1, 3);
    add(f);
    f = new Function("LEFT", func_left);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("MID", func_mid);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("REGEXP", func_regexp);
    f->setParamCount(2, 4);
    add(f);
    f = new Function("REGEXPRE", func_regexpre);
    f->setParamCount(3);
    add(f);
    f = new Function("REPLACE", func_replace);
    f->setParamCount(4);
    add(f);
    f = new Function("REPT", func_rept);
    f->setParamCount(2);
    add(f);
    f = new Function("RIGHT", func_right);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("SEARCH", func_search);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("SUBSTITUTE", func_substitute);
    f->setParamCount(3, 4);
    add(f);
    f = new Function("TEXT", func_text);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("BAHTTEXT", func_bahttext);
    f->setAlternateName("COM.MICROSOFT.BAHTTEXT");
    f->setParamCount(1);
    add(f);
}

QString TextModule::descriptionFileName() const
{
    return QString("text.xml");
}


// Function: ASC
Value func_asc(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString s = calc->conv()->asString(args[0]).asString();
    return Value(QString(s));
}

// Function: CHAR
Value func_char(valVector args, ValueCalc *calc, FuncExtra *)
{
    int val = calc->conv()->asInteger(args[0]).asInteger();
    if (val >= 0)
        return Value(QString(QChar(val)));
    else
        return Value::errorNUM();
}

// Function: CLEAN
Value func_clean(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str(calc->conv()->asString(args[0]).asString());
    QString result;
    QChar   c;
    int     i;
    int     l = str.length();

    for (i = 0; i < l; ++i) {
        c = str[i];
        if (c.isPrint())
            result += c;
    }

    return Value(result);
}

// Function: CODE
Value func_code(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str(calc->conv()->asString(args[0]).asString());
    if (str.length() <= 0)
        return Value::errorVALUE();

    return Value(str[0].unicode());
}

// Function: COMPARE
Value func_compare(valVector args, ValueCalc *calc, FuncExtra *)
{
    int  result = 0;
    bool exact = calc->conv()->asBoolean(args[2]).asBoolean();

    QString s1 = calc->conv()->asString(args[0]).asString();
    QString s2 = calc->conv()->asString(args[1]).asString();

    if (!exact)
        result = s1.toLower().localeAwareCompare(s2.toLower());
    else
        result = s1.localeAwareCompare(s2);

    if (result < 0)
        result = -1;
    else if (result > 0)
        result = 1;

    return Value(result);
}

void func_concatenate_helper(Value val, ValueCalc *calc,
                             QString& tmp)
{
    if (val.isArray()) {
        for (unsigned int row = 0; row < val.rows(); ++row)
            for (unsigned int col = 0; col < val.columns(); ++col)
                func_concatenate_helper(val.element(col, row), calc, tmp);
    } else
        tmp += calc->conv()->asString(val).asString();
}

// Function: CONCATENATE
Value func_concatenate(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString tmp;
    for (int i = 0; i < args.count(); ++i)
        func_concatenate_helper(args[i], calc, tmp);

    return Value(tmp);
}

// Function: DOLLAR
Value func_dollar(valVector args, ValueCalc *calc, FuncExtra *)
{
    // ValueConverter doesn't support money directly, hence we need to
    // use the locale. This code has the same effect as the output
    // of ValueFormatter for money format.

    // This function converts data to double/int, hence it won't support
    // larger precision.

    double value = numToDouble(calc->conv()->toFloat(args[0]));
    int precision = 2;
    if (args.count() == 2)
        precision = calc->conv()->asInteger(args[1]).asInteger();

    // do round, because formatMoney doesn't
    value = floor(value * pow(10.0, precision) + 0.5) / pow(10.0, precision);

    const KLocale *locale = calc->settings()->locale();
    QString s = locale->formatMoney(value, locale->currencySymbol(), precision);

    return Value(s);
}

// Function: EXACT
Value func_exact(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString s1 = calc->conv()->asString(args[0]).asString();
    QString s2 = calc->conv()->asString(args[1]).asString();
    bool exact = (s1 == s2);
    return Value(exact);
}

// Function: FIND
Value func_find(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString find_text, within_text;
    int start_num = 1;

    find_text = calc->conv()->asString(args[0]).asString();
    within_text = calc->conv()->asString(args[1]).asString();
    if (args.count() == 3)
        start_num = calc->conv()->asInteger(args[2]).asInteger();

    // conforms to Excel behaviour
    if (start_num <= 0) return Value::errorVALUE();
    if (start_num > (int)within_text.length()) return Value::errorVALUE();

    int pos = within_text.indexOf(find_text, start_num - 1);
    if (pos < 0) return Value::errorVALUE();

    return Value(pos + 1);
}

// Function: FIXED
Value func_fixed(valVector args, ValueCalc *calc, FuncExtra *)
{
    // uses double, hence won't support big precision

    int decimals = 2;
    bool decimalsIsNegative = false;
    bool no_commas = false;

    double number = numToDouble(calc->conv()->toFloat(args[0]));
    if (args.count() > 1) {
        if (args[1].less(Value(0))) {
            decimalsIsNegative = true;
            decimals = -1 * ((calc->roundUp(args[1])).asInteger());
        } else {
            decimals = calc->conv()->asInteger(args[1]).asInteger();
        }
    }
    if (args.count() == 3)
        no_commas = calc->conv()->asBoolean(args[2]).asBoolean();

    QString result;
    const KLocale *locale = calc->settings()->locale();

    // unfortunately, we can't just use KLocale::formatNumber because
    // * if decimals < 0, number is rounded
    // * if no_commas is true, thousand separators shouldn't show up

    if (decimalsIsNegative) {
        number = floor(number / pow(10.0, decimals) + 0.5) * pow(10.0, decimals);
        decimals = 0;
    }

    bool neg = number < 0;
    result = QString::number(neg ? -number : number, 'f', decimals);

    int pos = result.indexOf('.');
    if (pos == -1) pos = result.length();
    else result.replace(pos, 1, locale->decimalSymbol());
    if (!no_commas)
        while (0 < (pos -= 3))
            result.insert(pos, locale->thousandsSeparator());

    result.prepend(neg ? locale->negativeSign() :
                   locale->positiveSign());

    return Value(result);
}

// Function: JIS
Value func_jis(valVector args, ValueCalc *calc, FuncExtra *)
{
    Q_UNUSED(args);
    Q_UNUSED(calc);
    return Value(QString("FIXME JIS()"));
}

// Function: LEFT
Value func_left(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();
    int nb = 1;
    if (args.count() == 2)
        nb = calc->conv()->asInteger(args[1]).asInteger();
    if (nb < 0)
        return Value::errorVALUE();

    return Value(str.left(nb));
}

// Function: LEN
Value func_len(valVector args, ValueCalc *calc, FuncExtra *)
{
    int nb = calc->conv()->asString(args[0]).asString().length();
    return Value(nb);
}

// Function: LOWER
Value func_lower(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->conv()->asString(args[0]).asString().toLower());
}

// Function: MID
Value func_mid(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();

    int pos = calc->conv()->asInteger(args[1]).asInteger();
    if (pos < 0) {
        return Value::errorVALUE();
    }

    int len = 0x7fffffff;
    if (args.count() == 3) {
        len = (uint) calc->conv()->asInteger(args[2]).asInteger();
        // the length cannot be less than zero
        if (len < 0)
            return Value::errorVALUE();
    }

    // Excel compatible
    pos--;

    // workaround for Qt bug
    if (len > 0x7fffffff - pos) len = 0x7fffffff - pos;

    return Value(str.mid(pos, len));
}

// Function: PROPER
Value func_proper(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString().toLower();

    QChar f;
    bool  first = true;

    for (int i = 0; i < str.length(); ++i) {
        if (first) {
            f = str[i];
            if (f.isNumber())
                continue;

            f = f.toUpper();

            str[i] = f;
            first = false;

            continue;
        }

        if (str[i].isSpace() || str[i].isPunct())
            first = true;
    }

    return Value(str);
}

// Function: REGEXP
Value func_regexp(valVector args, ValueCalc *calc, FuncExtra *)
{
    // ensure that we got a valid regular expression
    QRegExp exp(calc->conv()->asString(args[1]).asString());
    if (!exp.isValid())
        return Value::errorVALUE();

    QString s = calc->conv()->asString(args[0]).asString();
    QString defText;
    if (args.count() > 2)
        defText = calc->conv()->asString(args[2]).asString();
    int bkref = 0;
    if (args.count() == 4)
        bkref = calc->conv()->asInteger(args[3]).asInteger();
    if (bkref < 0)   // strange back-reference
        return Value::errorVALUE();

    QString returnValue;

    int pos = exp.indexIn(s);
    if (pos == -1)
        returnValue = defText;
    else
        returnValue = exp.cap(bkref);

    return Value(returnValue);
}

// Function: REGEXPRE
Value func_regexpre(valVector args, ValueCalc *calc, FuncExtra *)
{
    // ensure that we got a valid regular expression
    QRegExp exp(calc->conv()->asString(args[1]).asString());
    if (!exp.isValid())
        return Value::errorVALUE();

    QString s = calc->conv()->asString(args[0]).asString();
    QString str = calc->conv()->asString(args[2]).asString();

    int pos = 0;
    while ((pos = exp.indexIn(s, pos)) != -1) {
        int i = exp.matchedLength();
        s = s.replace(pos, i, str);
        pos += str.length();
    }

    return Value(s);
}

// Function: REPLACE
Value func_replace(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString text = calc->conv()->asString(args[0]).asString();
    int pos = calc->conv()->asInteger(args[1]).asInteger();
    int len = calc->conv()->asInteger(args[2]).asInteger();
    QString new_text = calc->conv()->asString(args[3]).asString();

    if (pos < 0) pos = 0;

    QString result = text.replace(pos - 1, len, new_text);
    return Value(result);
}

// Function: REPT
Value func_rept(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString s = calc->conv()->asString(args[0]).asString();
    int nb = calc->conv()->asInteger(args[1]).asInteger();

    if (nb < 0)
        return Value::errorVALUE();

    QString result;
    for (int i = 0; i < nb; i++) result += s;
    return Value(result);
}

// Function: RIGHT
Value func_right(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();
    int nb = 1;
    if (args.count() == 2)
        nb = calc->conv()->asInteger(args[1]).asInteger();

    if (nb < 0)
        return Value::errorVALUE();

    return Value(str.right(nb));
}

// Function: ROT13
Value func_rot13(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString text = calc->conv()->asString(args[0]).asString();

    for (int i = 0; i < text.length(); i++) {
        unsigned c = text[i].toUpper().unicode();
        if ((c >= 'A') && (c <= 'M'))
            text[i] = QChar(text[i].unicode() + 13);
        if ((c >= 'N') && (c <= 'Z'))
            text[i] = QChar(text[i].unicode() - 13);
    }

    return Value(text);
}

// Function: SEARCH
Value func_search(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString find_text = calc->conv()->asString(args[0]).asString();
    QString within_text = calc->conv()->asString(args[1]).asString();
    int start_num = 1;
    if (args.count() == 3)
        start_num = calc->conv()->asInteger(args[2]).asInteger();

    // conforms to Excel behaviour
    if (start_num <= 0) return Value::errorVALUE();
    if (start_num > (int)within_text.length()) return Value::errorVALUE();

    // use globbing feature of QRegExp
    QRegExp regex(find_text, Qt::CaseInsensitive, QRegExp::Wildcard);
    int pos = within_text.indexOf(regex, start_num - 1);
    if (pos < 0) return Value::errorNA();

    return Value(pos + 1);
}

// Function: SLEEK
Value func_sleek(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();
    QString result;
    QChar   c;
    int     i;
    int     l = str.length();

    for (i = 0; i < l; ++i) {
        c = str[i];
        if (!c.isSpace())
            result += c;
    }

    return Value(result);
}

// Function: SUBSTITUTE
Value func_substitute(valVector args, ValueCalc *calc, FuncExtra *)
{
    int occurrence = 1;
    bool all = true;

    if (args.count() == 4) {
        occurrence = calc->conv()->asInteger(args[3]).asInteger();
        all = false;
    }

    QString text = calc->conv()->asString(args[0]).asString();
    QString old_text = calc->conv()->asString(args[1]).asString();
    QString new_text = calc->conv()->asString(args[2]).asString();

    if (occurrence <= 0) return Value::errorVALUE();
    if (old_text.length() == 0) return Value(text);

    QString result = text;

    if (all) {
        result.replace(old_text, new_text);   // case-sensitive
    } else {
        // We are only looking to modify a single value, by position.
        int position = -1;
        for (int i = 0; i < occurrence; ++i) {
            position = result.indexOf(old_text, position + 1);
        }
        result.replace(position, old_text.size(), new_text);
    }

    return Value(result);
}

// Function: T
Value func_t (valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args[0].isString())
        return calc->conv()->asString(args[0]);
    else
        return Value("");
}

// Function: TEXT
Value func_text(valVector args, ValueCalc *calc, FuncExtra *)
{
    //Second parameter is format_text. It is currently ignored.
    return calc->conv()->asString(args[0]);
}

// Function: TOGGLE
Value func_toggle(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();
    int i;
    int l = str.length();

    for (i = 0; i < l; ++i) {
        QChar c = str[i];
        QChar lc = c.toLower();
        QChar uc = c.toUpper();

        if (c == lc) // it is in lowercase
            str[i] = c.toUpper();
        else if (c == uc) // it is in uppercase
            str[i] = c.toLower();
    }

    return Value(str);
}

// Function: TRIM
Value func_trim(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(
               calc->conv()->asString(args[0]).asString().simplified());
}

// Function: UNICHAR
Value func_unichar(valVector args, ValueCalc *calc, FuncExtra *)
{
    ushort val = calc->conv()->asInteger(args[0]).asInteger();
    if (val > 0) {
        QString str;
        str.setUtf16(&val, 1);
        return Value(str);
    } else
        return Value::errorNUM();
}

// Function: UNICODE
Value func_unicode(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str(calc->conv()->asString(args[0]).asString());
    if (str.length() <= 0)
        return Value::errorVALUE();

    return Value((int)str.toUcs4().at(0));
}

// Function: UPPER
Value func_upper(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->conv()->asString(args[0]).asString().toUpper());
}

// Function: VALUE
Value func_value(valVector args, ValueCalc *calc, FuncExtra *)
{
    // same as the N function
    return calc->conv()->asFloat(args[0]);
}

#define UTF8_TH_0       "\340\270\250\340\270\271\340\270\231\340\270\242\340\271\214"
#define UTF8_TH_1       "\340\270\253\340\270\231\340\270\266\340\271\210\340\270\207"
#define UTF8_TH_2       "\340\270\252\340\270\255\340\270\207"
#define UTF8_TH_3       "\340\270\252\340\270\262\340\270\241"
#define UTF8_TH_4       "\340\270\252\340\270\265\340\271\210"
#define UTF8_TH_5       "\340\270\253\340\271\211\340\270\262"
#define UTF8_TH_6       "\340\270\253\340\270\201"
#define UTF8_TH_7       "\340\271\200\340\270\210\340\271\207\340\270\224"
#define UTF8_TH_8       "\340\271\201\340\270\233\340\270\224"
#define UTF8_TH_9       "\340\271\200\340\270\201\340\271\211\340\270\262"
#define UTF8_TH_10      "\340\270\252\340\270\264\340\270\232"
#define UTF8_TH_11      "\340\271\200\340\270\255\340\271\207\340\270\224"
#define UTF8_TH_20      "\340\270\242\340\270\265\340\271\210"
#define UTF8_TH_1E2     "\340\270\243\340\271\211\340\270\255\340\270\242"
#define UTF8_TH_1E3     "\340\270\236\340\270\261\340\270\231"
#define UTF8_TH_1E4     "\340\270\253\340\270\241\340\270\267\340\271\210\340\270\231"
#define UTF8_TH_1E5     "\340\271\201\340\270\252\340\270\231"
#define UTF8_TH_1E6     "\340\270\245\340\271\211\340\270\262\340\270\231"
#define UTF8_TH_DOT0    "\340\270\226\340\271\211\340\270\247\340\270\231"
#define UTF8_TH_BAHT    "\340\270\232\340\270\262\340\270\227"
#define UTF8_TH_SATANG  "\340\270\252\340\270\225\340\270\262\340\270\207\340\270\204\340\271\214"
#define UTF8_TH_MINUS   "\340\270\245\340\270\232"

inline void lclSplitBlock(double& rfInt, qint32& rnBlock, double fValue, double fSize)
{
    rnBlock = static_cast< qint32 >(modf((fValue + 0.1) / fSize, &rfInt) * fSize + 0.1);
}

/** Appends a digit (0 to 9) to the passed string. */
void lclAppendDigit(QString& rText, qint32 nDigit)
{
    switch (nDigit) {
    case 0: rText += QString::fromUtf8(UTF8_TH_0); break;
    case 1: rText += QString::fromUtf8(UTF8_TH_1); break;
    case 2: rText += QString::fromUtf8(UTF8_TH_2); break;
    case 3: rText += QString::fromUtf8(UTF8_TH_3); break;
    case 4: rText += QString::fromUtf8(UTF8_TH_4); break;
    case 5: rText += QString::fromUtf8(UTF8_TH_5); break;
    case 6: rText += QString::fromUtf8(UTF8_TH_6); break;
    case 7: rText += QString::fromUtf8(UTF8_TH_7); break;
    case 8: rText += QString::fromUtf8(UTF8_TH_8); break;
    case 9: rText += QString::fromUtf8(UTF8_TH_9); break;
    default: kDebug() << "lclAppendDigit - illegal digit"; break;
    }
}

/** Appends a value raised to a power of 10: nDigit*10^nPow10.
    @param nDigit  A digit in the range from 1 to 9.
    @param nPow10  A value in the range from 2 to 5.
 */
void lclAppendPow10(QString& rText, qint32 nDigit, qint32 nPow10)
{
    Q_ASSERT((1 <= nDigit) && (nDigit <= 9));   // illegal digit?
    lclAppendDigit(rText, nDigit);
    switch (nPow10) {
    case 2: rText += QString::fromUtf8(UTF8_TH_1E2); break;
    case 3: rText += QString::fromUtf8(UTF8_TH_1E3); break;
    case 4: rText += QString::fromUtf8(UTF8_TH_1E4); break;
    case 5: rText += QString::fromUtf8(UTF8_TH_1E5); break;
    default: kDebug() << "lclAppendPow10 - illegal power"; break;
    }
}

/** Appends a block of 6 digits (value from 1 to 999,999) to the passed string. */
void lclAppendBlock(QString& rText, qint32 nValue)
{
    Q_ASSERT((1 <= nValue) && (nValue <= 999999));   // illegal value?
    if (nValue >= 100000) {
        lclAppendPow10(rText, nValue / 100000, 5);
        nValue %= 100000;
    }
    if (nValue >= 10000) {
        lclAppendPow10(rText, nValue / 10000, 4);
        nValue %= 10000;
    }
    if (nValue >= 1000) {
        lclAppendPow10(rText, nValue / 1000, 3);
        nValue %= 1000;
    }
    if (nValue >= 100) {
        lclAppendPow10(rText, nValue / 100, 2);
        nValue %= 100;
    }
    if (nValue > 0) {
        qint32 nTen = nValue / 10;
        qint32 nOne = nValue % 10;
        if (nTen >= 1) {
            if (nTen >= 3)
                lclAppendDigit(rText, nTen);
            else if (nTen == 2)
                rText += QString::fromUtf8(UTF8_TH_20);
            rText += QString::fromUtf8(UTF8_TH_10);
        }
        if ((nTen > 0) && (nOne == 1))
            rText += QString::fromUtf8(UTF8_TH_11);
        else if (nOne > 0)
            lclAppendDigit(rText, nOne);
    }
}

// Function: BAHTTEXT
Value func_bahttext(valVector args, ValueCalc *calc, FuncExtra *)
{
    double value = numToDouble(calc->conv()->toFloat(args[0]));

    // sign
    bool bMinus = value < 0.0;
    value = fabs(value);

    // round to 2 digits after decimal point, value contains Satang as integer
    value = floor(value * 100.0 + 0.5);

    // split Baht and Satang
    double fBaht = 0.0;
    qint32 nSatang = 0;
    lclSplitBlock(fBaht, nSatang, value, 100.0);

    QString aText;

    // generate text for Baht value
    if (fBaht == 0.0) {
        if (nSatang == 0)
            aText += QString::fromUtf8(UTF8_TH_0);
    } else while (fBaht > 0.0) {
            QString aBlock;
            qint32 nBlock = 0;
            lclSplitBlock(fBaht, nBlock, fBaht, 1.0e6);
            if (nBlock > 0)
                lclAppendBlock(aBlock, nBlock);
            // add leading "million", if there will come more blocks
            if (fBaht > 0.0)
                aBlock = QString::fromUtf8(UTF8_TH_1E6) + aBlock;
            aText.insert(0, aBlock);
        }
    if (aText.length() > 0)
        aText += QString::fromUtf8(UTF8_TH_BAHT);

    // generate text for Satang value
    if (nSatang == 0) {
        aText += QString::fromUtf8(UTF8_TH_DOT0);
    } else {
        lclAppendBlock(aText, nSatang);
        aText += QString::fromUtf8(UTF8_TH_SATANG);
    }

    // add the minus sign
    if (bMinus)
        aText = QString::fromUtf8(UTF8_TH_MINUS) + aText;

    return Value(aText);
}

#include "TextModule.moc"
