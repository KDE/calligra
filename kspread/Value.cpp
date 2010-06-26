/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003,2004 Ariya Hidayat <ariya@kde.org>

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

#include "Value.h"
#include "CalculationSettings.h"
#include "CellStorage.h"
#include "ValueStorage.h"

#include <kdebug.h>
#include <klocale.h>

#include <QString>
#include <QTextStream>

#include <float.h>
#include <math.h>
#include <limits.h>

using namespace KSpread;

class ValueArray
{
public:
    ValueArray() : m_size(0, 0) {}
    ValueArray(const ValueStorage& storage, const QSize& size) : m_size(size), m_storage(storage) {}

    ValueStorage& storage() { return m_storage; }
    int rows() const { return qMax(m_size.height(), m_storage.rows()); }
    int columns() const { return qMax(m_size.width(), m_storage.columns()); }

    bool operator==(const ValueArray& a) const { return rows() == a.rows() && columns() == a.columns() && m_storage == a.m_storage; }
private:
    QSize m_size;
    ValueStorage m_storage;
};

class Value::Private : public QSharedData
{
public:

    Value::Type type: 4;
    Value::Format format: 4;

    union { // 64 bits at max!
        bool b;
        qint64 i;
        Number f;
        complex<Number>* pc;
        QString* ps;
        ValueArray* pa;
    };

    // create empty data
    Private() : type(Value::Empty), format(Value::fmt_None), ps(0) {}

    Private(const Private& o)
            : QSharedData(o)
            , type(o.type)
            , format(o.format) {
        switch (type) {
        case Value::Empty:
        default:
            ps = 0;
            break;
        case Value::Boolean:
            b = o.b;
            break;
        case Value::Integer:
            i = o.i;
            break;
        case Value::Float:
            f = o.f;
            break;
        case Value::Complex:
            pc = new complex<Number>(*o.pc);
            break;
        case Value::String:
        case Value::Error:
            ps = new QString(*o.ps);
            break;
        case Value::Array:
            pa = new ValueArray(*o.pa);
            break;
        }
    }

    // destroys data
    ~Private() {
        if (this == s_null)
            s_null = 0;
        clear();
    }

    // static empty data to be shared
    static Private* null() {
        if (!s_null) s_null = new Private; return s_null;
    }

    // true if it's null (which is shared)
    bool isNull() {
        return this == s_null;
    }

    /** Deletes all data. */
    void clear() {
        if (type == Value::Array)   delete pa;
        if (type == Value::Complex) delete pc;
        if (type == Value::Error)   delete ps;
        if (type == Value::String)  delete ps;
    }

    /** set most probable formatting based on the type */
    void setFormatByType();

private:
    void operator=(const Value::Private& o);

    static Private* s_null;
};

void Value::Private::setFormatByType()
{
    switch (type) {
    case Value::Empty:
        format = Value::fmt_None;
        break;
    case Value::Boolean:
        format = Value::fmt_Boolean;
        break;
    case Value::Integer:
    case Value::Float:
    case Value::Complex:
        format = Value::fmt_Number;
        break;
    case Value::String:
        format = Value::fmt_String;
        break;
    case Value::Array:
        format = Value::fmt_None;
        break;
    case Value::CellRange:
        format = Value::fmt_None;
        break;
    case Value::Error:
        format = Value::fmt_String;
        break;
    };
}

// to be shared between all empty value
Value::Private* Value::Private::s_null = 0;

// static things
Value ks_value_empty;
Value ks_error_circle;
Value ks_error_depend;
Value ks_error_div0;
Value ks_error_na;
Value ks_error_name;
Value ks_error_null;
Value ks_error_num;
Value ks_error_parse;
Value ks_error_ref;
Value ks_error_value;

// create an empty value
Value::Value()
        : d(Private::null())
{
}

// destructor
Value::~Value()
{
}

// create value of certain type
Value::Value(Value::Type _type)
        : d(Private::null())
{
    d->type = _type;
    d->setFormatByType();
}

// copy constructor
Value::Value(const Value& _value)
        : d(_value.d)
{
}

// assignment operator
Value& Value::operator=(const Value & _value)
{
    d = _value.d;
    return *this;
}

// comparison operator - returns true only if strictly identical, unlike equal()/compare()
bool Value::operator==(const Value& o) const
{
    if (d->type != o.d->type)
        return false;
    switch (d->type) {
    case Empty:   return true;
    case Boolean: return o.d->b == d->b;
    case Integer: return o.d->i == d->i;
    case Float:   return compare(o.d->f, d->f) == 0;
    case Complex: return (!d->pc && !o.d->pc) || ((d->pc && o.d->pc) && (*o.d->pc == *d->pc));
    case String:  return (!d->ps && !o.d->ps) || ((d->ps && o.d->ps) && (*o.d->ps == *d->ps));
    case Array:   return (!d->pa && !o.d->pa) || ((d->pa && o.d->pa) && (*o.d->pa == *d->pa));
    case Error:   return (!d->ps && !o.d->ps) || ((d->ps && o.d->ps) && (*o.d->ps == *d->ps));
    default: break;
    }
    kWarning() << "Unhandled type in Value::operator==: " << d->type;
    return false;
}

// create a boolean value
Value::Value(bool b)
        : d(Private::null())
{
    d->type = Boolean;
    d->b = b;
    d->format = fmt_Boolean;
}

// create an integer value
Value::Value(qint64 i)
        : d(Private::null())
{
    d->type = Integer;
    d->i = i;
    d->format = fmt_Number;
}

// create an integer value
Value::Value(int i)
        : d(Private::null())
{
    d->type = Integer;
    d->i = static_cast<qint64>(i);
    d->format = fmt_Number;
}

// create a floating-point value
Value::Value(double f)
        : d(Private::null())
{
    d->type = Float;
    d->f = Number(f);
    d->format = fmt_Number;
}

// create a floating-point value
Value::Value(long double f)
        : d(Private::null())
{
    d->type = Float;
    d->f = Number(f);
    d->format = fmt_Number;
}


#ifdef KSPREAD_HIGH_PRECISION_SUPPORT
// create a floating-point value
Value::Value(Number f)
        : d(Private::null())
{
    d->type = Float;
    d->f = f;
    d->format = fmt_Number;
}
#endif // KSPREAD_HIGH_PRECISION_SUPPORT

// create a complex number value
Value::Value(const complex<Number>& c)
        : d(Private::null())
{
    d->type = Complex;
    d->pc = new complex<Number>(c);
    d->format = fmt_Number;
}

// create a string value
Value::Value(const QString& s)
        : d(Private::null())
{
    d->type = String;
    d->ps = new QString(s);
    d->format = fmt_String;
}

// create a string value
Value::Value(const char *s)
        : d(Private::null())
{
    d->type = String;
    d->ps = new QString(s);
    d->format = fmt_String;
}

// create a floating-point value from date/time
Value::Value(const QDateTime& dt, const CalculationSettings* settings)
        : d(Private::null())
{
    const QDate refDate(settings->referenceDate());
    const QTime refTime(0, 0);    // reference time is midnight
    d->type = Float;
    d->f = Number(refDate.daysTo(dt.date()));
    d->f += static_cast<double>(refTime.msecsTo(dt.time())) / 86400000.0;     // 24*60*60*1000
    d->format = fmt_DateTime;
}

// create a floating-point value from time
Value::Value(const QTime& time, const CalculationSettings* settings)
        : d(Private::null())
{
    Q_UNUSED(settings);
    const QTime refTime(0, 0);    // reference time is midnight

    d->type = Float;
    d->f = Number(static_cast<double>(refTime.msecsTo(time)) / 86400000.0);      // 24*60*60*1000
    d->format = fmt_Time;
}

// create a floating-point value from date
Value::Value(const QDate& date, const CalculationSettings* settings)
        : d(Private::null())
{
    const QDate refDate(settings->referenceDate());

    d->type = Integer;
    d->i = refDate.daysTo(date);
    d->format = fmt_Date;
}

// create an array value
Value::Value(const ValueStorage& array, const QSize& size)
        : d(Private::null())
{
    d->type = Array;
    d->pa = new ValueArray(array, size);
    d->format = fmt_None;
}

// return type of the value
Value::Type Value::type() const
{
    return d ? d->type : Empty;
}

// get the value as boolean
bool Value::asBoolean() const
{
    bool result = false;

    if (type() == Value::Boolean)
        result = d->b;

    return result;
}

// get the value as integer
qint64 Value::asInteger() const
{
    qint64 result = 0;
    if (type() == Integer)
        result = d->i;
    else if (type() == Float)
        result = static_cast<qint64>(floor(numToDouble(d->f)));
    else if (type() == Complex)
        result = static_cast<qint64>(floor(numToDouble(d->pc->real())));
    return result;
}

// get the value as floating-point
Number Value::asFloat() const
{
    Number result = 0.0;
    if (type() == Float)
        result = d->f;
    else if (type() == Integer)
        result = static_cast<Number>(d->i);
    else if (type() == Complex)
        result = d->pc->real();
    return result;
}

// get the value as complex number
complex<Number> Value::asComplex() const
{
    complex<Number> result(0.0, 0.0);
    if (type() == Complex)
        result = *d->pc;
    else if (type() == Float)
        result = d->f;
    else if (type() == Integer)
        result = static_cast<Number>(d->i);
    return result;
}

// get the value as string
QString Value::asString() const
{
    QString result;

    if (type() == Value::String)
        if (d->ps)
            result = QString(*d->ps);

    return result;
}

// get the value as QVariant
QVariant Value::asVariant() const
{
    QVariant result;

    switch (d->type) {
    case Value::Empty:
    default:
        result = 0;
        break;
    case Value::Boolean:
        result = d->b;
        break;
    case Value::Integer:
        result = d->i;
        break;
    case Value::Float:
        result = (double) numToDouble(d->f);
        break;
    case Value::Complex:
        // FIXME: add support for complex numbers
        // pc = new complex<Number>( *o.pc );
        break;
    case Value::String:
    case Value::Error:
        result = *d->ps;
        break;
    case Value::Array:
        // FIXME: not supported yet
        //result = ValueArray( d->pa );
        break;
    }

    return result;
}

// set error message
void Value::setError(const QString& msg)
{
    d->clear();
    d->type = Error;
    d->ps = new QString(msg);
}

// get error message
QString Value::errorMessage() const
{
    QString result;

    if (type() == Value::Error)
        if (d->ps)
            result = QString(*d->ps);

    return result;
}

// get the value as date/time
QDateTime Value::asDateTime(const CalculationSettings* settings) const
{
    QDateTime datetime(settings->referenceDate(), QTime(), Qt::UTC);

    const int days = asInteger();
    const int msecs = qRound((numToDouble(asFloat() - double(days))) * 86400000.0);      // 24*60*60*1000
    datetime = datetime.addDays(days);
    datetime = datetime.addMSecs(msecs);

    return datetime;
}

// get the value as date
QDate Value::asDate(const CalculationSettings* settings) const
{
    QDate dt(settings->referenceDate());

    int i = asInteger();
    dt = dt.addDays(i);

    return dt;
}

// get the value as time
QTime Value::asTime(const CalculationSettings* settings) const
{
    Q_UNUSED(settings);
    QTime dt;

    const int days = asInteger();
    const int msecs = qRound(numToDouble(asFloat() - double(days)) * 86400000.0);      // 24*60*60*1000
    dt = dt.addMSecs(msecs);

    return dt;
}

Value::Format Value::format() const
{
    return d ? d->format : fmt_None;
}

void Value::setFormat(Value::Format fmt)
{
    d->format = fmt;
}

Value Value::element(unsigned column, unsigned row) const
{
    if (d->type != Array) return *this;
    if (!d->pa) return empty();
    return d->pa->storage().lookup(column + 1, row + 1);
}

Value Value::element(unsigned index) const
{
    if (d->type != Array) return *this;
    if (!d->pa) return empty();
    return d->pa->storage().data(index);
}

void Value::setElement(unsigned column, unsigned row, const Value& v)
{
    if (d->type != Array) return;
    if (!d->pa) d->pa = new ValueArray();
    d->pa->storage().insert(column + 1, row + 1, v);
}

unsigned Value::columns() const
{
    if (d->type != Array) return 1;
    if (!d->pa) return 1;
    return d->pa->columns();
}

unsigned Value::rows() const
{
    if (d->type != Array) return 1;
    if (!d->pa) return 1;
    return d->pa->rows();
}

unsigned Value::count() const
{
    if (d->type != Array) return 1;
    if (!d->pa) return 1;
    return d->pa->storage().count();
}

// reference to empty value
const Value& Value::empty()
{
    return ks_value_empty;
}

// reference to #CIRCLE! error
const Value& Value::errorCIRCLE()
{
    if (!ks_error_circle.isError())
        ks_error_circle.setError(i18nc("Error: circular formula dependency", "#CIRCLE!"));
    return ks_error_circle;
}

// reference to #DEPEND! error
const Value& Value::errorDEPEND()
{
    if (!ks_error_depend.isError())
        ks_error_depend.setError(i18nc("Error: broken cell reference", "#DEPEND!"));
    return ks_error_depend;
}

// reference to #DIV/0! error
const Value& Value::errorDIV0()
{
    if (!ks_error_div0.isError())
        ks_error_div0.setError(i18nc("Error: division by zero", "#DIV/0!"));
    return ks_error_div0;
}

// reference to #N/A error
const Value& Value::errorNA()
{
    if (!ks_error_na.isError())
        ks_error_na.setError(i18nc("Error: not available", "#N/A"));
    return ks_error_na;
}

// reference to #NAME? error
const Value& Value::errorNAME()
{
    if (!ks_error_name.isError())
        ks_error_name.setError(i18nc("Error: unknown function name", "#NAME?"));
    return ks_error_name;
}

// reference to #NUM! error
const Value& Value::errorNUM()
{
    if (!ks_error_num.isError())
        ks_error_num.setError(i18nc("Error: number out of range", "#NUM!"));
    return ks_error_num;
}

// reference to #NULL! error
const Value& Value::errorNULL()
{
    if (!ks_error_null.isError())
        ks_error_null.setError(i18nc("Error: empty intersecting area", "#NULL!"));
    return ks_error_null;
}

// reference to #PARSE! error
const Value& Value::errorPARSE()
{
    if (!ks_error_parse.isError())
        ks_error_parse.setError(i18nc("Error: formula not parseable", "#PARSE!"));
    return ks_error_parse;
}

// reference to #REF! error
const Value& Value::errorREF()
{
    if (!ks_error_ref.isError())
        ks_error_ref.setError(i18nc("Error: invalid cell/array reference", "#REF!"));
    return ks_error_ref;
}

// reference to #VALUE! error
const Value& Value::errorVALUE()
{
    if (!ks_error_value.isError())
        ks_error_value.setError(i18nc("Error: wrong (number of) function argument(s)", "#VALUE!"));
    return ks_error_value;
}

int Value::compare(Number v1, Number v2)
{
    Number v3 = v1 - v2;
    if (v3 > DBL_EPSILON) return 1;
    if (v3 < -DBL_EPSILON) return -1;
    return 0;
}

bool Value::isZero(Number v)
{
    return abs(v) < DBL_EPSILON;
}

bool Value::isZero() const
{
    if (!isNumber()) return false;
    return isZero(asFloat());
}

bool Value::allowComparison(const Value& v) const
{
    Value::Type t1 = d->type;
    Value::Type t2 = v.type();

    if ((t1 == Empty) && (t2 == Empty)) return true;
    if ((t1 == Empty) && (t2 == String)) return true;

    if ((t1 == Boolean) && (t2 == Boolean)) return true;
    if ((t1 == Boolean) && (t2 == Integer)) return true;
    if ((t1 == Boolean) && (t2 == Float)) return true;
    if ((t1 == Boolean) && (t2 == String)) return true;

    if ((t1 == Integer) && (t2 == Boolean)) return true;
    if ((t1 == Integer) && (t2 == Integer)) return true;
    if ((t1 == Integer) && (t2 == Float)) return true;
    if ((t1 == Integer) && (t2 == String)) return true;

    if ((t1 == Float) && (t2 == Boolean)) return true;
    if ((t1 == Float) && (t2 == Integer)) return true;
    if ((t1 == Float) && (t2 == Float)) return true;
    if ((t1 == Float) && (t2 == String)) return true;

    if ((t1 == Complex) && (t2 == Boolean)) return true;
    if ((t1 == Complex) && (t2 == Integer)) return true;
    if ((t1 == Complex) && (t2 == Float)) return true;
    if ((t1 == Complex) && (t2 == String)) return true;

    if ((t1 == String) && (t2 == Empty)) return true;
    if ((t1 == String) && (t2 == Boolean)) return true;
    if ((t1 == String) && (t2 == Integer)) return true;
    if ((t1 == String) && (t2 == Float)) return true;
    if ((t1 == String) && (t2 == Complex)) return true;
    if ((t1 == String) && (t2 == String)) return true;

    // errors can be compared too ...
    if ((t1 == Error) && (t2 == Error)) return true;

    return false;
}

// compare values. looks strange in order to be compatible with Excel
int Value::compare(const Value& v) const
{
    Value::Type t1 = d->type;
    Value::Type t2 = v.type();

    // errors always less than everything else
    if ((t1 == Error) && (t2 != Error))
        return -1;
    if ((t2 == Error) && (t1 != Error))
        return 1;

    // comparing errors only yields 0 if they are the same
    if ((t1 == Error) && (t2 == Error))
        return errorMessage() != v.errorMessage();

    // empty == empty
    if ((t1 == Empty) && (t2 == Empty))
        return 0;

    // empty value is always less than string
    // (except when the string is empty)
    if ((t1 == Empty) && (t2 == String))
        return(v.asString().isEmpty()) ? 0 : -1;

    // boolean vs boolean
    if ((t1 == Boolean) && (t2 == Boolean)) {
        bool p = asBoolean();
        bool q = v.asBoolean();
        if (p) return q ? 0 : 1;
        else return q ? -1 : 0;
    }

    // boolean is always greater than integer
    if ((t1 == Boolean) && (t2 == Integer))
        return 1;

    // boolean is always greater than float
    if ((t1 == Boolean) && (t2 == Float))
        return 1;

    // boolean is always greater than string
    if ((t1 == Boolean) && (t2 == String))
        return 1;

    // integer is always less than boolean
    if ((t1 == Integer) && (t2 == Boolean))
        return -1;

    // integer vs integer
    if ((t1 == Integer) && (t2 == Integer)) {
        qint64 p = asInteger();
        qint64 q = v.asInteger();
        return (p == q) ? 0 : (p < q) ? -1 : 1;
    }

    // integer vs float
    if ((t1 == Integer) && (t2 == Float))
        return compare(asFloat(), v.asFloat());

    // integer is always less than string
    if ((t1 == Integer) && (t2 == String))
        return -1;

    // float is always less than boolean
    if ((t1 == Float) && (t2 == Boolean))
        return -1;

    // float vs integer
    if ((t1 == Float) && (t2 == Integer))
        return compare(asFloat(), v.asFloat());

    // float vs float
    if ((t1 == Float) && (t2 == Float))
        return compare(asFloat(), v.asFloat());

    // float is always less than string
    if ((t1 == Float) && (t2 == String))
        return -1;

    // TODO Stefan: Complex

    // string is always greater than empty value
    // (except when the string is empty)
    if ((t1 == String) && (t2 == Empty))
        return(asString().isEmpty()) ? 0 : 1;

    // string is always less than boolean
    if ((t1 == String) && (t2 == Boolean))
        return -1;

    // string is always greater than integer
    if ((t1 == String) && (t2 == Integer))
        return 1;

    // string is always greater than float
    if ((t1 == String) && (t2 == Float))
        return 1;

    // The-Real-String comparison
    if ((t1 == String) && (t2 == String))
        return asString().compare(v.asString());

    // Undefined, actually allowComparison would return false
    return 0;
}

bool Value::equal(const Value& v) const
{
    if (!allowComparison(v)) return false;
    return compare(v) == 0;
}

bool Value::less(const Value& v) const
{
    if (!allowComparison(v)) return false;
    return compare(v) < 0;
}

bool Value::greater(const Value& v) const
{
    if (!allowComparison(v)) return false;
    return compare(v) > 0;
}

QTextStream& operator<<(QTextStream& ts, Value::Type type)
{
    switch (type) {
    case Value::Empty:   ts << "Empty"; break;
    case Value::Boolean: ts << "Boolean"; break;
    case Value::Integer: ts << "Integer"; break;
    case Value::Float:   ts << "Float"; break;
    case Value::Complex: ts << "Complex"; break;
    case Value::String:  ts << "String"; break;
    case Value::Array:   ts << "Array"; break;
    case Value::Error:   ts << "Error"; break;
    default: ts << "Unknown!"; break;
    };
    return ts;
}

QTextStream& operator<<(QTextStream& ts, Value value)
{
    ts << value.type();
    switch (value.type()) {
    case Value::Empty:   break;

    case Value::Boolean:
        ts << ": ";
        if (value.asBoolean()) ts << "TRUE";
        else ts << "FALSE"; break;

    case Value::Integer:
        ts << ": " << value.asInteger(); break;

    case Value::Float:
        ts << ": " << (double) numToDouble(value.asFloat()); break;

    case Value::Complex: {
        const complex<Number> complex(value.asComplex());
        ts << ": " << (double) numToDouble(complex.real());
        if (complex.imag() >= 0.0)
            ts << '+';
        ts << (double) numToDouble(complex.imag()) << 'i';
        break;
    }

    case Value::String:
        ts << ": " << value.asString(); break;

    case Value::Array: {
        ts << ": {" << value.asString();
        const int cols = value.columns();
        const int rows = value.rows();
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                ts << value.element(col, row);
                if (col < cols - 1)
                    ts << ';';
            }
            if (row < rows - 1)
                ts << '|';
        }
        ts << '}';
        break;
    }

    case Value::Error:
        ts << '(' << value.errorMessage() << ')'; break;

    default: break;
    }
    return ts;
}

/***************************************************************************
  QHash/QSet support
****************************************************************************/

namespace KSpread
{
uint qHash(const Value& value)
{
    switch (value.type()) {
    case Value::Empty:
    case Value::CellRange:
        return 0;
    case Value::Boolean:
        return ::qHash(value.asBoolean());
    case Value::Integer:
        return ::qHash(value.asInteger());
    case Value::Float:
        return ::qHash((qint64)numToDouble(value.asFloat()));
    case Value::Complex:
        return ::qHash((qint64)value.asComplex().real());
    case Value::String:
        return ::qHash(value.asString());
    case Value::Array:
        return qHash(value.element(0, 0));
    case Value::Error:
        return ::qHash(value.errorMessage());
    }
    return 0;
}
} // namespace KSpread

/***************************************************************************
  kDebug support
****************************************************************************/

kdbgstream operator<<(kdbgstream str, const KSpread::Value& v)
{
    QString string;
    QTextStream stream(&string);
    stream << v;
    str << string;
    return str;
}

kdbgstream operator<<(kdbgstream stream, const KSpread::Value::Format& f)
{
    switch (f) {
    case KSpread::Value::fmt_None:     stream << "None";     break;
    case KSpread::Value::fmt_Boolean:  stream << "Boolean";  break;
    case KSpread::Value::fmt_Number:   stream << "Number";   break;
    case KSpread::Value::fmt_Percent:  stream << "Percent";  break;
    case KSpread::Value::fmt_Money:    stream << "Money";    break;
    case KSpread::Value::fmt_DateTime: stream << "DateTime"; break;
    case KSpread::Value::fmt_Date:     stream << "Date";     break;
    case KSpread::Value::fmt_Time:     stream << "Time";     break;
    case KSpread::Value::fmt_String:   stream << "String";   break;
    }
    return stream;
}
