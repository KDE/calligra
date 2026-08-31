// This file is part of the KDE project
// SPDX-FileCopyrightText: 1998-2002 The KSpread Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// built-in text functions
#include "TextModule.h"

#include "JapaneseWidthFolding.h"
#include "engine/CalculationSettings.h"
#include "engine/Function.h"
#include "engine/Localization.h"
#include "engine/MapBase.h"
#include "engine/SheetBase.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"
#include "engine/ValueParser.h"

#include <cmath>

// #include "SheetsDebug.h"
// #include "FunctionModuleRegistry.h"
// #include "ValueFormatter.h"

using namespace Calligra::Sheets;
using namespace Qt::StringLiterals;

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
Value func_findb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fixed(valVector args, ValueCalc *calc, FuncExtra *);
Value func_jis(valVector args, ValueCalc *calc, FuncExtra *);
Value func_left(valVector args, ValueCalc *calc, FuncExtra *);
Value func_leftb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_len(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lenb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lower(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mid(valVector args, ValueCalc *calc, FuncExtra *);
Value func_midb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_numbervalue(valVector args, ValueCalc *calc, FuncExtra *);
Value func_proper(valVector args, ValueCalc *calc, FuncExtra *);
Value func_regexp(valVector args, ValueCalc *calc, FuncExtra *);
Value func_regexpre(valVector args, ValueCalc *calc, FuncExtra *);
Value func_replace(valVector args, ValueCalc *calc, FuncExtra *);
Value func_replaceb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rept(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rot13(valVector args, ValueCalc *calc, FuncExtra *);
Value func_right(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rightb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_search(valVector args, ValueCalc *calc, FuncExtra *);
Value func_searchb(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sleek(valVector args, ValueCalc *calc, FuncExtra *);
Value func_substitute(valVector args, ValueCalc *calc, FuncExtra *);
Value func_t(valVector args, ValueCalc *calc, FuncExtra *);
Value func_text(valVector args, ValueCalc *calc, FuncExtra *);
Value func_toggle(valVector args, ValueCalc *calc, FuncExtra *);
Value func_trim(valVector args, ValueCalc *calc, FuncExtra *);
Value func_unichar(valVector args, ValueCalc *calc, FuncExtra *);
Value func_unicode(valVector args, ValueCalc *calc, FuncExtra *);
Value func_upper(valVector args, ValueCalc *calc, FuncExtra *);
Value func_value(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bahttext(valVector args, ValueCalc *calc, FuncExtra *);

CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("kspreadtextmodule.json", TextModule)

TextModule::TextModule(QObject *parent, const QVariantList &)
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
    f = new Function("LENB", func_lenb);
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
    f = new Function("FINDB", func_findb);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("FIXED", func_fixed);
    f->setParamCount(1, 3);
    add(f);
    f = new Function("LEFT", func_left);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("LEFTB", func_leftb);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("MID", func_mid);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("MIDB", func_midb);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("NUMBERVALUE", func_numbervalue);
    f->setParamCount(1, 3);
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
    f = new Function("REPLACEB", func_replaceb);
    f->setParamCount(4);
    add(f);
    f = new Function("REPT", func_rept);
    f->setParamCount(2);
    add(f);
    f = new Function("RIGHT", func_right);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("RIGHTB", func_rightb);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("SEARCH", func_search);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("SEARCHB", func_searchb);
    f->setParamCount(2, 3);
    add(f);
    f = new Function("SUBSTITUTE", func_substitute);
    f->setParamCount(3, 4);
    add(f);
    f = new Function("TEXT", func_text);
    f->setParamCount(2);
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
    return Value(JapaneseWidthFolding::toHalfWidthForAsc(s));
}

// Function: CHAR
Value func_char(valVector args, ValueCalc *calc, FuncExtra *)
{
    int val = calc->conv()->asInteger(args[0]).asInteger();
    if (val < 0 || val > 255)
        return Value::errorVALUE();
    return Value(QString(QChar(val)));
}

// Function: CLEAN
Value func_clean(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str(calc->conv()->asString(args[0]).asString());
    QString result;
    result.reserve(str.length());

    // CLEAN strips C0/C1 control characters and Unicode noncharacters, but not e.g.
    // private-use code points that QChar::isPrint() would also (wrongly) flag as non-printable
    for (int i = 0; i < str.length(); ++i) {
        const QChar c = str.at(i);
        char32_t cp = c.unicode();
        int width = 1;
        if (c.isHighSurrogate() && i + 1 < str.length() && str.at(i + 1).isLowSurrogate()) {
            cp = QChar::surrogateToUcs4(c, str.at(i + 1));
            width = 2;
        }
        const bool removable = cp < 0x20 || (cp >= 0x7F && cp <= 0x9F) || (cp & 0xFFFE) == 0xFFFE || (cp >= 0xFDD0 && cp <= 0xFDEF);
        if (!removable) {
            result += c;
            if (width == 2)
                result += str.at(i + 1);
        }
        i += width - 1;
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
    int result = 0;
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

void func_concatenate_helper(Value val, ValueCalc *calc, QString &tmp)
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

    const Localization *locale = calc->settings()->locale();
    // always a leading minus sign, never locale-specific parentheses
    QString s = locale->formatCurrency(fabs(value), locale->currencySymbol(), qMax(0, precision));
    if (value < 0)
        s.prepend('-');

    return Value(s);
}

// per-codepoint UTF-16 index plus cumulative "unit" count, with a trailing sentinel at
// (text.length(), totalUnits). unitWidth(cp) is 1 for character-based positions (LEFT, MID,
// LEN, FIND, SEARCH), or the DBCS byte width (1 or 2) for the "B" variants.
struct UnitOffset {
    int utf16;
    int unit;
};

static int codepointWidth(char32_t)
{
    return 1;
}

// Unicode ranges that DBCS byte-oriented functions (LEFTB/RIGHTB/MIDB/LENB/FINDB/SEARCHB/
// REPLACEB) count as 2 bytes (roughly East Asian Wide/Fullwidth); everything else is 1 byte.
static int dbcsWidth(char32_t cp)
{
    static const std::pair<char32_t, char32_t> wideRanges[] = {
        {0x1100, 0x115F},
        {0x2E80, 0x303E},
        {0x3041, 0x33FF},
        {0x3400, 0x4DBF},
        {0x4E00, 0x9FFF},
        {0xA000, 0xA4CF},
        {0xAC00, 0xD7A3},
        {0xF900, 0xFAFF},
        {0xFE30, 0xFE4F},
        {0xFF00, 0xFF60},
        {0xFFE0, 0xFFE6},
        {0x1F300, 0x1FAFF},
        {0x20000, 0x3FFFD},
    };
    for (const auto &r : wideRanges)
        if (cp >= r.first && cp <= r.second)
            return 2;
    return 1;
}

template<typename WidthFn>
static QList<UnitOffset> unitOffsets(const QString &text, WidthFn unitWidth)
{
    QList<UnitOffset> offsets;
    int unit = 0;
    for (int i = 0; i < text.length();) {
        offsets << UnitOffset{i, unit};
        char32_t cp = text.at(i).unicode();
        int w16 = 1;
        if (text.at(i).isHighSurrogate() && i + 1 < text.length() && text.at(i + 1).isLowSurrogate()) {
            cp = QChar::surrogateToUcs4(text.at(i), text.at(i + 1));
            w16 = 2;
        }
        unit += unitWidth(cp);
        i += w16;
    }
    offsets << UnitOffset{int(text.length()), unit};
    return offsets;
}

// 1-based unit position of the first match of `needle` (a literal QString or a
// QRegularExpression) in `haystack` at or after unit startUnit (1-based); 0 if startUnit is
// out of range or there's no match.
template<typename Needle, typename WidthFn>
static int findUnitPos(const QString &haystack, const Needle &needle, int startUnit, WidthFn unitWidth)
{
    const QList<UnitOffset> offsets = unitOffsets(haystack, unitWidth);
    int idx = 0;
    while (idx < offsets.size() && offsets[idx].unit < startUnit - 1)
        ++idx;
    if (idx >= offsets.size())
        return 0;
    const int pos = haystack.indexOf(needle, offsets[idx].utf16);
    if (pos < 0)
        return 0;
    for (const UnitOffset &o : offsets)
        if (o.utf16 == pos)
            return o.unit + 1;
    return 0;
}

static int findCodepointPos(const QString &haystack, const QString &needle, int startCodepoint)
{
    return findUnitPos(haystack, needle, startCodepoint, codepointWidth);
}

// largest codepoint count whose cumulative unit width is <= maxUnits
static int unitCountFitting(const QList<UnitOffset> &offsets, int maxUnits)
{
    int m = 0;
    while (m + 1 < offsets.size() && offsets[m + 1].unit <= maxUnits)
        ++m;
    return m;
}

// number of codepoints, starting at codepoint index fromIdx, whose cumulative unit width
// (relative to fromIdx) is <= maxUnits
static int unitCountFromFitting(const QList<UnitOffset> &offsets, int fromIdx, int maxUnits)
{
    const int base = offsets[fromIdx].unit;
    int m = 0;
    while (fromIdx + m + 1 < offsets.size() && offsets[fromIdx + m + 1].unit - base <= maxUnits)
        ++m;
    return m;
}

// smallest codepoint index whose suffix (through the end) has cumulative unit width <= maxUnits
static int unitSuffixStart(const QList<UnitOffset> &offsets, int maxUnits)
{
    const int target = offsets.last().unit - maxUnits;
    int k = 0;
    while (k < offsets.size() - 1 && offsets[k].unit < target)
        ++k;
    return k;
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
    if (start_num <= 0)
        return Value::errorVALUE();
    if (start_num > (int)within_text.length())
        return Value::errorVALUE();

    int pos = findCodepointPos(within_text, find_text, start_num);
    if (pos == 0)
        return Value::errorVALUE();

    return Value(pos);
}

// Function: FINDB (byte-position variant of FIND, wide/CJK characters count as 2 bytes)
Value func_findb(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString find_text = calc->conv()->asString(args[0]).asString();
    QString within_text = calc->conv()->asString(args[1]).asString();
    int start_num = 1;
    if (args.count() == 3)
        start_num = calc->conv()->asInteger(args[2]).asInteger();

    if (start_num <= 0)
        return Value::errorVALUE();
    const QList<UnitOffset> offsets = unitOffsets(within_text, dbcsWidth);
    if (start_num > offsets.last().unit)
        return Value::errorVALUE();

    int pos = findUnitPos(within_text, find_text, start_num, dbcsWidth);
    if (pos == 0)
        return Value::errorVALUE();

    return Value(pos);
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
    const Localization *locale = calc->settings()->locale();

    if (decimalsIsNegative) {
        number = floor(number / pow(10.0, decimals) + 0.5) * pow(10.0, decimals);
        decimals = 0;
    }

    bool neg = number < 0;
    result = QString::number(neg ? -number : number, 'f', decimals);

    int pos = result.indexOf('.');
    if ((pos == -1) && decimals) {
        result += '.';
        pos = result.length() - 1;
    }
    if (pos == -1)
        pos = result.length();
    else {
        result.replace(pos, 1, locale->decimalSymbol());
        // add missing decimals
        while (result.length() - pos < 1 + decimals)
            result += "0";
    }
    if (!no_commas)
        while (0 < (pos -= 3))
            result.insert(pos, locale->thousandsSeparator());

    result.prepend(neg ? locale->negativeSign() : QString());

    return Value(result);
}

// Function: JIS
Value func_jis(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString s = calc->conv()->asString(args[0]).asString();
    return Value(JapaneseWidthFolding::toFullWidthForJis(s));
}

// codepoint-based QString::fromUcs4, since Qt's toUcs4() still returns QList<uint>
static QString fromCodePoints(const QList<uint> &cps, int pos, int len)
{
    return QString::fromUcs4(reinterpret_cast<const char32_t *>(cps.constData()) + pos, len);
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

    // by codepoint, not UTF-16 unit, to not split a surrogate pair
    const QList<uint> cps = str.toUcs4();
    return Value(fromCodePoints(cps, 0, qMin(nb, cps.size())));
}

// Function: LEFTB (byte-position variant of LEFT, wide/CJK characters count as 2 bytes)
Value func_leftb(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();
    int nb = 1;
    if (args.count() == 2)
        nb = calc->conv()->asInteger(args[1]).asInteger();
    if (nb < 0)
        return Value::errorVALUE();

    const QList<UnitOffset> offsets = unitOffsets(str, dbcsWidth);
    const QList<uint> cps = str.toUcs4();
    return Value(fromCodePoints(cps, 0, unitCountFitting(offsets, nb)));
}

// Function: LEN
Value func_len(valVector args, ValueCalc *calc, FuncExtra *)
{
    const int nb = calc->conv()->asString(args[0]).asString().toUcs4().size();
    return Value(nb);
}

// Function: LENB (byte-length variant of LEN, wide/CJK characters count as 2 bytes)
Value func_lenb(valVector args, ValueCalc *calc, FuncExtra *)
{
    const QString str = calc->conv()->asString(args[0]).asString();
    return Value(unitOffsets(str, dbcsWidth).last().unit);
}

// Function: LOWER
// asString() renders a date/time-tagged value using its display format (e.g. "Saturday, ..."),
// but text functions like LOWER/UPPER are specified to just stringify the raw serial number.
static Value stripDateTimeFormat(const Value &value)
{
    if (value.format() != Value::fmt_Date && value.format() != Value::fmt_DateTime && value.format() != Value::fmt_Time)
        return value;
    Value copy(value);
    copy.setFormat(Value::fmt_None);
    return copy;
}

Value func_lower(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->conv()->asString(stripDateTimeFormat(args[0])).asString().toLower());
}

// Function: MID
Value func_mid(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();

    int pos = calc->conv()->asInteger(args[1]).asInteger();
    if (pos < 1) {
        return Value::errorVALUE();
    }

    int len = 0x7fffffff;
    if (args.count() == 3) {
        len = (uint)calc->conv()->asInteger(args[2]).asInteger();
        // the length cannot be less than zero
        if (len < 0)
            return Value::errorVALUE();
    }

    // Excel compatible
    pos--;

    // by codepoint, not UTF-16 unit, to not split a surrogate pair
    const QList<uint> cps = str.toUcs4();
    pos = qBound(0, pos, cps.size());
    len = qMin(len, cps.size() - pos);

    return Value(fromCodePoints(cps, pos, len));
}

// Function: MIDB (byte-position variant of MID, wide/CJK characters count as 2 bytes)
Value func_midb(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();

    int pos = calc->conv()->asInteger(args[1]).asInteger();
    if (pos < 1)
        return Value::errorVALUE();

    int len = 0x7fffffff;
    if (args.count() == 3) {
        len = calc->conv()->asInteger(args[2]).asInteger();
        if (len < 0)
            return Value::errorVALUE();
    }
    pos--;

    const QList<UnitOffset> offsets = unitOffsets(str, dbcsWidth);
    const int startIdx = unitCountFitting(offsets, pos);
    const int count = unitCountFromFitting(offsets, startIdx, len);

    const QList<uint> cps = str.toUcs4();
    return Value(fromCodePoints(cps, startIdx, count));
}

// Function: NUMBERVALUE
Value func_numbervalue(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString text = calc->conv()->asString(args[0]).asString().trimmed();

    QString decimalSep = calc->settings()->locale()->decimalSymbol();
    QString groupSep;
    if (args.count() > 1) {
        decimalSep = calc->conv()->asString(args[1]).asString();
    }
    if (args.count() > 2) {
        groupSep = calc->conv()->asString(args[2]).asString();
    }
    if (!groupSep.isEmpty() && decimalSep == groupSep) {
        return Value::errorVALUE();
    }

    if (!groupSep.isEmpty()) {
        text.replace(groupSep, QString());
    }

    const bool percent = text.endsWith(u'%');
    if (percent) {
        text.chop(1);
    }
    text = text.trimmed();

    QString sign;
    if (text.startsWith(u'+') || text.startsWith(u'-')) {
        sign = text.left(1);
        text = text.mid(1);
    }

    QString intPart = text;
    QString fracPart;
    if (!decimalSep.isEmpty()) {
        const int decPos = text.indexOf(decimalSep);
        if (decPos >= 0) {
            intPart = text.left(decPos);
            fracPart = text.mid(decPos + decimalSep.length());
        }
    }

    // anything besides digits, the sign, and the one decimal separator is not a number
    const auto allDigits = [](const QString &s) {
        return std::all_of(s.begin(), s.end(), [](QChar c) {
            return c.isDigit();
        });
    };
    if (!allDigits(intPart) || !allDigits(fracPart)) {
        return Value::errorVALUE();
    }

    QString composed = sign + (intPart.isEmpty() ? u"0"_s : intPart);
    if (!fracPart.isEmpty()) {
        composed += u'.' + fracPart;
    }

    bool ok = false;
    double v = composed.toDouble(&ok);
    if (!ok) {
        return Value::errorVALUE();
    }
    if (percent) {
        v /= 100.0;
    }

    return Value(v);
}

// Function: PROPER
Value func_proper(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString().toLower();

    bool startOfWord = true;
    for (int i = 0; i < str.length(); ++i) {
        if (str[i].isLetter()) {
            if (startOfWord)
                str[i] = str[i].toUpper();
            startOfWord = false;
        } else {
            startOfWord = true;
        }
    }

    return Value(str);
}

// Function: REGEXP
Value func_regexp(valVector args, ValueCalc *calc, FuncExtra *)
{
    // ensure that we got a valid regular expression
    QRegularExpression exp(calc->conv()->asString(args[1]).asString());
    if (!exp.isValid())
        return Value::errorVALUE();

    QString s = calc->conv()->asString(args[0]).asString();
    QString defText;
    if (args.count() > 2)
        defText = calc->conv()->asString(args[2]).asString();
    int bkref = 0;
    if (args.count() == 4)
        bkref = calc->conv()->asInteger(args[3]).asInteger();
    if (bkref < 0) // strange back-reference
        return Value::errorVALUE();

    QString returnValue;

    QRegularExpressionMatch match;
    int pos = s.indexOf(exp, 0, &match);
    if (pos == -1)
        returnValue = defText;
    else
        returnValue = match.captured(bkref);

    return Value(returnValue);
}

// Function: REGEXPRE
Value func_regexpre(valVector args, ValueCalc *calc, FuncExtra *)
{
    // ensure that we got a valid regular expression
    QRegularExpression exp(calc->conv()->asString(args[1]).asString());
    if (!exp.isValid())
        return Value::errorVALUE();

    QString s = calc->conv()->asString(args[0]).asString();
    QString str = calc->conv()->asString(args[2]).asString();

    int pos = 0;
    QRegularExpressionMatch match;
    while ((pos = s.indexOf(exp, pos, &match)) != -1) {
        int i = match.capturedLength();
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

    if (pos < 1 || len < 0)
        return Value::errorVALUE();
    pos--;

    // by codepoint, not UTF-16 unit, to not split a surrogate pair
    const QList<uint> cps = text.toUcs4();
    pos = qMin(pos, cps.size());
    len = qMin(len, cps.size() - pos);

    QString result = fromCodePoints(cps, 0, pos) + new_text + fromCodePoints(cps, pos + len, cps.size() - pos - len);
    return Value(result);
}

// Function: REPLACEB (byte-position variant of REPLACE, wide/CJK characters count as 2 bytes)
Value func_replaceb(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString text = calc->conv()->asString(args[0]).asString();
    int pos = calc->conv()->asInteger(args[1]).asInteger();
    int len = calc->conv()->asInteger(args[2]).asInteger();
    QString new_text = calc->conv()->asString(args[3]).asString();

    if (pos < 1 || len < 0)
        return Value::errorVALUE();

    const QList<UnitOffset> offsets = unitOffsets(text, dbcsWidth);
    const int start = pos - 1;
    const int end = start + len;
    if (end > offsets.last().unit)
        return Value::errorVALUE();

    const QList<uint> cps = text.toUcs4();

    // a byte boundary that splits a wide character turns that character into a single space
    const int prefixCount = unitCountFitting(offsets, start);
    QString prefix = fromCodePoints(cps, 0, prefixCount);
    if (offsets[prefixCount].unit < start)
        prefix += QLatin1Char(' ');

    int suffixStart = unitCountFitting(offsets, end);
    QString suffix;
    if (offsets[suffixStart].unit < end) {
        suffix += QLatin1Char(' ');
        ++suffixStart;
    }
    suffix += fromCodePoints(cps, suffixStart, cps.size() - suffixStart);

    return Value(prefix + new_text + suffix);
}

// Function: REPT
Value func_rept(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString s = calc->conv()->asString(args[0]).asString();
    int nb = calc->conv()->asInteger(args[1]).asInteger();

    if (nb < 0)
        return Value::errorVALUE();

    QString result;
    for (int i = 0; i < nb; i++)
        result += s;
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

    // by codepoint, not UTF-16 unit, to not split a surrogate pair
    const QList<uint> cps = str.toUcs4();
    const int start = qMax(0, cps.size() - nb);
    return Value(fromCodePoints(cps, start, cps.size() - start));
}

// Function: RIGHTB (byte-position variant of RIGHT, wide/CJK characters count as 2 bytes)
Value func_rightb(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();
    int nb = 1;
    if (args.count() == 2)
        nb = calc->conv()->asInteger(args[1]).asInteger();

    if (nb < 0)
        return Value::errorVALUE();

    const QList<UnitOffset> offsets = unitOffsets(str, dbcsWidth);
    const int start = unitSuffixStart(offsets, nb);

    const QList<uint> cps = str.toUcs4();
    return Value(fromCodePoints(cps, start, cps.size() - start));
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
    if (start_num <= 0)
        return Value::errorVALUE();
    if (start_num > (int)within_text.length())
        return Value::errorVALUE();

    // use globbing feature of QRegExp
    auto regex = QRegularExpression::fromWildcard(find_text, Qt::CaseInsensitive, QRegularExpression::UnanchoredWildcardConversion);
    int pos = findUnitPos(within_text, regex, start_num, codepointWidth);
    if (pos == 0)
        return Value::errorNA();

    return Value(pos);
}

// Function: SEARCHB (byte-position variant of SEARCH, wide/CJK characters count as 2 bytes)
Value func_searchb(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString find_text = calc->conv()->asString(args[0]).asString();
    QString within_text = calc->conv()->asString(args[1]).asString();
    int start_num = 1;
    if (args.count() == 3)
        start_num = calc->conv()->asInteger(args[2]).asInteger();

    if (start_num <= 0)
        return Value::errorVALUE();
    const QList<UnitOffset> offsets = unitOffsets(within_text, dbcsWidth);
    if (start_num > offsets.last().unit)
        return Value::errorVALUE();

    auto regex = QRegularExpression::fromWildcard(find_text, Qt::CaseInsensitive, QRegularExpression::UnanchoredWildcardConversion);
    int pos = findUnitPos(within_text, regex, start_num, dbcsWidth);
    if (pos == 0)
        return Value::errorNA();

    return Value(pos);
}

// Function: SLEEK
Value func_sleek(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString str = calc->conv()->asString(args[0]).asString();
    QString result;
    QChar c;
    int i;
    int l = str.length();

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

    if (occurrence <= 0)
        return Value::errorVALUE();
    if (old_text.length() == 0)
        return Value(text);

    QString result = text;

    if (all) {
        result.replace(old_text, new_text); // case-sensitive
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
Value func_t(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args[0].isError())
        return args[0];
    if (args[0].isString())
        return calc->conv()->asString(args[0]);
    else
        return Value("");
}

// --- TEXT() number-format-code engine ---------------------------------------------------

// Renders `digits` (a plain decimal digit string, no sign) against `mask`'s 0/# placeholder
// positions: a placeholder with no matching digit shows '0' for '0', nothing for '#'; every
// other character is passed through literally. Extra leading digits beyond the placeholder
// count all land on the first placeholder (no truncation).
static bool isDigitPlaceholder(QChar c)
{
    return c == QLatin1Char('0') || c == QLatin1Char('#') || c == QLatin1Char('?');
}

static QString renderIntegerMask(const QString &digits, const QString &mask)
{
    QList<int> slotPositions;
    for (int i = 0; i < mask.size(); ++i)
        if (isDigitPlaceholder(mask.at(i)))
            slotPositions << i;

    const int slotCount = slotPositions.size();
    const int shift = slotCount - digits.size();

    QStringList slotOutputs;
    for (int i = 0; i < slotCount; ++i) {
        const int digitIndex = i - shift;
        if (digitIndex < 0) {
            const QChar type = mask.at(slotPositions[i]);
            slotOutputs << (type == QLatin1Char('0') ? u"0"_s : type == QLatin1Char('?') ? u" "_s : QString());
        } else if (i == 0 && digitIndex > 0) {
            slotOutputs << digits.left(digitIndex + 1);
        } else {
            slotOutputs << QString(digits.at(digitIndex));
        }
    }

    QString result;
    int slot = 0;
    for (int i = 0; i < mask.size(); ++i)
        result += isDigitPlaceholder(mask.at(i)) ? slotOutputs[slot++] : QString(mask.at(i));
    return result;
}

// Rounds |value| to fracDigits decimals and splits it into integer/fractional digit strings.
static void splitRoundedDigits(double value, int fracDigits, QString &intDigits, QString &fracDigitsStr)
{
    const double scale = std::pow(10.0, fracDigits);
    const qint64 scaled = qRound64(qAbs(value) * scale);
    QString all = QString::number(scaled);
    if (all.size() <= fracDigits)
        all = all.rightJustified(fracDigits + 1, QLatin1Char('0'));
    intDigits = all.left(all.size() - fracDigits);
    fracDigitsStr = fracDigits > 0 ? all.right(fracDigits) : QString();
}

// Renders the integer part with thousands grouping for a mask containing a ',' among its
// placeholders, e.g. "$#,##0". Literals outside the placeholder run become a plain prefix/suffix.
static QString renderGroupedInteger(const QString &intDigits, const QString &mask)
{
    int first = 0;
    int last = mask.size() - 1;
    while (first < mask.size() && mask.at(first) != QLatin1Char('0') && mask.at(first) != QLatin1Char('#'))
        ++first;
    while (last >= 0 && mask.at(last) != QLatin1Char('0') && mask.at(last) != QLatin1Char('#'))
        --last;
    const QString prefix = mask.left(first);
    const QString suffix = mask.mid(last + 1);

    QString maskNoComma = mask.mid(first, last - first + 1);
    maskNoComma.remove(QLatin1Char(','));
    int minDigits = 0;
    while (minDigits < maskNoComma.size() && maskNoComma.at(maskNoComma.size() - 1 - minDigits) == QLatin1Char('0'))
        ++minDigits;

    const QString digits = intDigits.size() < minDigits ? intDigits.rightJustified(minDigits, QLatin1Char('0')) : intDigits;
    QString grouped;
    int count = 0;
    for (int i = digits.size() - 1; i >= 0; --i) {
        grouped.prepend(digits.at(i));
        if (++count % 3 == 0 && i != 0)
            grouped.prepend(QLatin1Char(','));
    }
    return prefix + grouped + suffix;
}

// Renders a "# ?/?"-style fraction section: an optional integer-part mask/separator, then a
// numerator/denominator pair approximating the fractional part, denominator width bounded by
// the number of '?' after the '/'.
static QString formatFractionSection(double value, const QString &format)
{
    const int slash = format.indexOf(QLatin1Char('/'));
    const QString beforeSlash = format.left(slash);
    const QString denMask = format.mid(slash + 1);

    int numStart = beforeSlash.size();
    while (numStart > 0 && beforeSlash.at(numStart - 1) == QLatin1Char('?'))
        --numStart;
    const QString numMask = beforeSlash.mid(numStart);
    const QString intMaskAndSep = beforeSlash.left(numStart);

    int maxDenominator = 1;
    for (int i = 0; i < denMask.count(QLatin1Char('?')); ++i)
        maxDenominator *= 10;
    maxDenominator -= 1;
    if (maxDenominator < 1)
        maxDenominator = 1;

    const bool neg = value < 0;
    double v = qAbs(value);
    qint64 intPart = qint64(v);
    double frac = v - intPart;

    int bestNum = 0;
    int bestDen = 1;
    double bestErr = frac;
    for (int den = 1; den <= maxDenominator; ++den) {
        const int num = qRound(frac * den);
        const double err = qAbs(frac - double(num) / den);
        if (err < bestErr) {
            bestErr = err;
            bestNum = num;
            bestDen = den;
        }
    }
    if (bestDen != 0 && bestNum == bestDen) { // rounded up to the next whole number
        ++intPart;
        bestNum = 0;
    }

    QString intOnly;
    QString sep;
    for (QChar c : intMaskAndSep) {
        if (c == QLatin1Char('0') || c == QLatin1Char('#'))
            intOnly += c;
        else
            sep += c;
    }
    QString intText;
    if (intPart != 0 || intOnly.contains(QLatin1Char('0')))
        intText = renderIntegerMask(QString::number(intPart), intOnly.isEmpty() ? u"#"_s : intOnly);

    const QString result =
        intText + sep + renderIntegerMask(QString::number(bestNum), numMask) + QLatin1Char('/') + renderIntegerMask(QString::number(bestDen), denMask);
    return neg ? QLatin1Char('-') + result : result;
}

// Renders a "0.00E+00"-style scientific-notation section.
static QString formatScientificSection(double value, const QString &format, int eIdx)
{
    const QString mantissaMask = format.left(eIdx);
    const QChar signChar = format.at(eIdx + 1);
    const QString expMask = format.mid(eIdx + 2);

    const int dot = mantissaMask.indexOf(QLatin1Char('.'));
    const QString intMask = dot >= 0 ? mantissaMask.left(dot) : mantissaMask;
    const QString fracMask = dot >= 0 ? mantissaMask.mid(dot + 1) : QString();
    int intSlots = 0;
    for (QChar c : intMask)
        if (c == QLatin1Char('0') || c == QLatin1Char('#'))
            ++intSlots;
    if (intSlots < 1)
        intSlots = 1;
    const int fracDigits = fracMask.size();

    const bool neg = value < 0;
    double v = qAbs(value);
    int exponent = 0;
    if (v != 0) {
        exponent = int(std::floor(std::log10(v))) - (intSlots - 1);
        v /= std::pow(10.0, exponent);
        const double rounded = qRound(v * std::pow(10.0, fracDigits)) / std::pow(10.0, fracDigits);
        if (rounded >= std::pow(10.0, intSlots)) {
            v = rounded / 10.0;
            ++exponent;
        } else {
            v = rounded;
        }
    }

    QString intDigits;
    QString fracDigitsStr;
    splitRoundedDigits(v, fracDigits, intDigits, fracDigitsStr);

    QString mantissa = renderIntegerMask(intDigits, intMask);
    if (!fracMask.isEmpty())
        mantissa += QLatin1Char('.') + renderIntegerMask(fracDigitsStr, fracMask);

    QString expSign;
    if (exponent < 0)
        expSign = u"-"_s;
    else if (signChar == QLatin1Char('+'))
        expSign = u"+"_s;
    const QString expDigits = renderIntegerMask(QString::number(qAbs(exponent)), expMask);

    return (neg ? u"-"_s : QString()) + mantissa + QLatin1Char('E') + expSign + expDigits;
}

// Renders one number-format section (percent/scientific/fraction/plain, with optional thousands
// grouping) against `value`.
static QString formatNumberSection(double value, QString format)
{
    const bool percent = format.contains(QLatin1Char('%'));
    if (percent) {
        value *= 100;
        format.remove(QLatin1Char('%'));
    }

    const int eIdx = format.indexOf(QLatin1Char('E'), 0, Qt::CaseInsensitive);
    if (eIdx >= 0 && eIdx + 1 < format.size() && (format.at(eIdx + 1) == QLatin1Char('+') || format.at(eIdx + 1) == QLatin1Char('-')))
        return formatScientificSection(value, format, eIdx);

    if (format.contains(QLatin1Char('?')) && format.contains(QLatin1Char('/')))
        return formatFractionSection(value, format);

    const int dot = format.indexOf(QLatin1Char('.'));
    const QString intMask = dot >= 0 ? format.left(dot) : format;
    const QString fracMask = dot >= 0 ? format.mid(dot + 1) : QString();
    int fracDigits = 0;
    for (QChar c : fracMask)
        if (c == QLatin1Char('0') || c == QLatin1Char('#'))
            ++fracDigits;

    QString intDigits;
    QString fracDigitsStr;
    splitRoundedDigits(value, fracDigits, intDigits, fracDigitsStr);

    QString result = intMask.contains(QLatin1Char(',')) ? renderGroupedInteger(intDigits, intMask) : renderIntegerMask(intDigits, intMask);
    if (!fracMask.isEmpty()) {
        QString fracPart = renderIntegerMask(fracDigitsStr, fracMask);
        // '#' placeholders suppress trailing insignificant zeros, unlike '0'
        int end = fracMask.size();
        while (end > 0 && fracMask.at(end - 1) == QLatin1Char('#') && fracPart.at(end - 1) == QLatin1Char('0'))
            --end;
        fracPart = fracPart.left(end);
        if (!fracPart.isEmpty())
            result += QLatin1Char('.') + fracPart;
    }
    if (percent)
        result += QLatin1Char('%');
    return (value < 0 ? u"-"_s : QString()) + result;
}

struct FormatSection {
    QString condition;
    QString format;
};

static QList<FormatSection> splitFormatSections(const QString &format)
{
    QList<FormatSection> sections;
    const auto parts = format.split(QLatin1Char(';'));
    for (const QString &raw : parts) {
        FormatSection section;
        QString s = raw;
        if (s.startsWith(QLatin1Char('['))) {
            const int end = s.indexOf(QLatin1Char(']'));
            if (end > 0) {
                section.condition = s.mid(1, end - 1);
                s = s.mid(end + 1);
            }
        }
        section.format = s;
        sections << section;
    }
    return sections;
}

static bool evaluateCondition(double value, const QString &condition)
{
    static const QRegularExpression re(u"^(<=|>=|<>|<|>|=)?(-?\\d+(?:\\.\\d+)?)$"_s);
    const QRegularExpressionMatch m = re.match(condition);
    if (!m.hasMatch())
        return false;
    const QString op = m.captured(1);
    const double bound = m.captured(2).toDouble();
    if (op.isEmpty() || op == "="_L1)
        return value == bound;
    if (op == "<"_L1)
        return value < bound;
    if (op == "<="_L1)
        return value <= bound;
    if (op == ">"_L1)
        return value > bound;
    if (op == ">="_L1)
        return value >= bound;
    return value != bound; // "<>"
}

// Picks which section of a (possibly multi-section, possibly conditional) format string applies
// to `value`: sections with an explicit [condition] are matched first, in order; otherwise the
// standard positive/negative/zero convention applies.
static QString selectFormatSection(double value, const QList<FormatSection> &sections)
{
    for (const FormatSection &s : sections)
        if (!s.condition.isEmpty() && evaluateCondition(value, s.condition))
            return s.format;

    QStringList unconditional;
    for (const FormatSection &s : sections)
        if (s.condition.isEmpty())
            unconditional << s.format;

    if (unconditional.isEmpty())
        return sections.isEmpty() ? QString() : sections.last().format;
    if (unconditional.size() == 1)
        return unconditional.first();
    if (unconditional.size() >= 3 && value == 0)
        return unconditional.at(2);
    return (value < 0 && unconditional.size() >= 2) ? unconditional.at(1) : unconditional.first();
}

// A run of Y/M/D/H/S format letters (case-insensitive), or a literal span kept as-is.
struct DateToken {
    QChar kind; // 'Y'/'M'/'m' (minutes)/'D'/'H'/'S', or a null QChar for a literal
    QString text; // literal text (only meaningful when kind is null)
    int length = 0;
};

static QList<DateToken> tokenizeDateFormat(const QString &format)
{
    static const QString dateLetters = u"YMDHS"_s;
    QList<DateToken> tokens;
    int i = 0;
    while (i < format.size()) {
        const QChar u = format.at(i).toUpper();
        if (dateLetters.contains(u)) {
            int j = i;
            while (j < format.size() && format.at(j).toUpper() == u)
                ++j;
            tokens << DateToken{u, QString(), j - i};
            i = j;
        } else {
            int j = i;
            while (j < format.size() && !dateLetters.contains(format.at(j).toUpper()))
                ++j;
            tokens << DateToken{QChar(), format.mid(i, j - i), 0};
            i = j;
        }
    }
    return tokens;
}

// An "M" run means minutes rather than month when it sits next to an H (before) or S (after)
// token; mark those by lower-casing their kind.
static void resolveMinuteTokens(QList<DateToken> &tokens)
{
    for (int i = 0; i < tokens.size(); ++i) {
        if (tokens[i].kind != QLatin1Char('M'))
            continue;
        bool minute = false;
        for (int j = i - 1; j >= 0 && !minute; --j) {
            if (tokens[j].kind == QChar())
                continue;
            minute = tokens[j].kind == QLatin1Char('H');
            break;
        }
        if (!minute) {
            for (int j = i + 1; j < tokens.size(); ++j) {
                if (tokens[j].kind == QChar())
                    continue;
                minute = tokens[j].kind == QLatin1Char('S');
                break;
            }
        }
        if (minute)
            tokens[i].kind = QLatin1Char('m');
    }
}

static QString renderDateTimeFormat(const QDateTime &dt, const QString &format)
{
    QList<DateToken> tokens = tokenizeDateFormat(format);
    resolveMinuteTokens(tokens);

    QString result;
    for (const DateToken &t : tokens) {
        if (t.kind == QChar()) {
            result += t.text;
            continue;
        }
        int value = 0;
        switch (t.kind.unicode()) {
        case 'Y':
            value = dt.date().year();
            if (t.length <= 2)
                value %= 100;
            break;
        case 'M':
            value = dt.date().month();
            break;
        case 'm':
            value = dt.time().minute();
            break;
        case 'D':
            value = dt.date().day();
            break;
        case 'H':
            value = dt.time().hour();
            break;
        case 'S':
            value = dt.time().second();
            break;
        }
        result += QString::number(value).rightJustified(t.length, QLatin1Char('0'));
    }
    return result;
}

static bool isDateTimeFormat(const QString &format)
{
    for (QChar c : format) {
        const QChar u = c.toUpper();
        if (u == QLatin1Char('Y') || u == QLatin1Char('D') || u == QLatin1Char('H') || u == QLatin1Char('S'))
            return true;
    }
    return false;
}

// Function: TEXT
Value func_text(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args[0].isError())
        return args[0];
    // text values pass through unformatted
    if (args[0].isString())
        return args[0];

    const QString format = calc->conv()->asString(args[1]).asString();

    if (isDateTimeFormat(format)) {
        // use asFloat (blank -> 0) rather than asDateTime, which treats a blank value as "now"
        const double serial = calc->conv()->asFloat(args[0]).asFloat();
        QDateTime dt(calc->settings()->referenceDate(), QTime(), Qt::UTC);
        const int days = int(serial);
        dt = dt.addDays(days).addMSecs(qRound64((serial - days) * 86400000.0));
        return Value(renderDateTimeFormat(dt, format));
    }

    const double value = calc->conv()->asFloat(args[0]).asFloat();
    const QString section = selectFormatSection(value, splitFormatSections(format));
    return Value(formatNumberSection(value, section));
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
    return Value(calc->conv()->asString(args[0]).asString().simplified());
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
    return Value(calc->conv()->asString(stripDateTimeFormat(args[0])).asString().toUpper());
}

// Function: VALUE
Value func_value(valVector args, ValueCalc *calc, FuncExtra *extra)
{
    if (args[0].isNumber())
        return calc->conv()->asFloat(args[0]);

    const QString text = calc->conv()->asString(args[0]).asString();
    if (extra && extra->sheet) {
        const Value parsed = extra->sheet->map()->parser()->parse(text);
        if (!parsed.isString())
            return calc->conv()->asFloat(parsed);
        return Value::errorVALUE();
    }

    bool ok = false;
    Value result = calc->conv()->asFloat(args[0], &ok);
    return ok ? result : Value::errorVALUE();
}

#define UTF8_TH_0 "\340\270\250\340\270\271\340\270\231\340\270\242\340\271\214"
#define UTF8_TH_1 "\340\270\253\340\270\231\340\270\266\340\271\210\340\270\207"
#define UTF8_TH_2 "\340\270\252\340\270\255\340\270\207"
#define UTF8_TH_3 "\340\270\252\340\270\262\340\270\241"
#define UTF8_TH_4 "\340\270\252\340\270\265\340\271\210"
#define UTF8_TH_5 "\340\270\253\340\271\211\340\270\262"
#define UTF8_TH_6 "\340\270\253\340\270\201"
#define UTF8_TH_7 "\340\271\200\340\270\210\340\271\207\340\270\224"
#define UTF8_TH_8 "\340\271\201\340\270\233\340\270\224"
#define UTF8_TH_9 "\340\271\200\340\270\201\340\271\211\340\270\262"
#define UTF8_TH_10 "\340\270\252\340\270\264\340\270\232"
#define UTF8_TH_11 "\340\271\200\340\270\255\340\271\207\340\270\224"
#define UTF8_TH_20 "\340\270\242\340\270\265\340\271\210"
#define UTF8_TH_1E2 "\340\270\243\340\271\211\340\270\255\340\270\242"
#define UTF8_TH_1E3 "\340\270\236\340\270\261\340\270\231"
#define UTF8_TH_1E4 "\340\270\253\340\270\241\340\270\267\340\271\210\340\270\231"
#define UTF8_TH_1E5 "\340\271\201\340\270\252\340\270\231"
#define UTF8_TH_1E6 "\340\270\245\340\271\211\340\270\262\340\270\231"
#define UTF8_TH_DOT0 "\340\270\226\340\271\211\340\270\247\340\270\231"
#define UTF8_TH_BAHT "\340\270\232\340\270\262\340\270\227"
#define UTF8_TH_SATANG "\340\270\252\340\270\225\340\270\262\340\270\207\340\270\204\340\271\214"
#define UTF8_TH_MINUS "\340\270\245\340\270\232"

inline void lclSplitBlock(double &rfInt, qint32 &rnBlock, double fValue, double fSize)
{
    rnBlock = static_cast<qint32>(modf((fValue + 0.1) / fSize, &rfInt) * fSize + 0.1);
}

/** Appends a digit (0 to 9) to the passed string. */
void lclAppendDigit(QString &rText, qint32 nDigit)
{
    switch (nDigit) {
    case 0:
        rText += QString::fromUtf8(UTF8_TH_0);
        break;
    case 1:
        rText += QString::fromUtf8(UTF8_TH_1);
        break;
    case 2:
        rText += QString::fromUtf8(UTF8_TH_2);
        break;
    case 3:
        rText += QString::fromUtf8(UTF8_TH_3);
        break;
    case 4:
        rText += QString::fromUtf8(UTF8_TH_4);
        break;
    case 5:
        rText += QString::fromUtf8(UTF8_TH_5);
        break;
    case 6:
        rText += QString::fromUtf8(UTF8_TH_6);
        break;
    case 7:
        rText += QString::fromUtf8(UTF8_TH_7);
        break;
    case 8:
        rText += QString::fromUtf8(UTF8_TH_8);
        break;
    case 9:
        rText += QString::fromUtf8(UTF8_TH_9);
        break;
    default:
        debugSheets << "lclAppendDigit - illegal digit";
        break;
    }
}

/** Appends a value raised to a power of 10: nDigit*10^nPow10.
    @param rText   The result text.
    @param nDigit  A digit in the range from 1 to 9.
    @param nPow10  A value in the range from 2 to 5.
 */
void lclAppendPow10(QString &rText, qint32 nDigit, qint32 nPow10)
{
    Q_ASSERT((1 <= nDigit) && (nDigit <= 9)); // illegal digit?
    lclAppendDigit(rText, nDigit);
    switch (nPow10) {
    case 2:
        rText += QString::fromUtf8(UTF8_TH_1E2);
        break;
    case 3:
        rText += QString::fromUtf8(UTF8_TH_1E3);
        break;
    case 4:
        rText += QString::fromUtf8(UTF8_TH_1E4);
        break;
    case 5:
        rText += QString::fromUtf8(UTF8_TH_1E5);
        break;
    default:
        debugSheets << "lclAppendPow10 - illegal power";
        break;
    }
}

/** Appends a block of 6 digits (value from 1 to 999,999) to the passed string. */
void lclAppendBlock(QString &rText, qint32 nValue)
{
    Q_ASSERT((1 <= nValue) && (nValue <= 999999)); // illegal value?
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
    } else
        while (fBaht > 0.0) {
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

#include "text.moc"
