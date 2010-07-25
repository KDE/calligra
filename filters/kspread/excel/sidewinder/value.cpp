/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#include "value.h"
#include "ustring.h"

#include <iostream>
#include <sstream>

namespace Swinder
{

// helper class for Value
class ValueData
{
public:

    Value::Type type;

    // someday move to use union to reduce memory consumption
    union {
        bool b;
        int i;
        double f;
    };
    UString s;
    std::map<unsigned, FormatFont> formatRuns;

    // create empty data
    ValueData() {
        count = 0;
        b = false;
        i = 0;
        f = 0.0;
        s = UString::null;
        type = Value::Empty;
        ref();
    }

    // destroys data
    ~ValueData() {
        if (this == s_null) s_null = 0;
    }

    void ref() {
        count++;
    }

    // static empty data to be shared
    static ValueData* null() {
        if (!s_null) s_null = new ValueData; else s_null->ref(); return s_null;
    }

    // decrease reference count
    void unref() {
        --count; if (!count) delete this;
    }

    // true if it's null (which is shared)
    bool isNull() {
        return this == s_null;
    }

    unsigned count; // reference count

private:

    static ValueData* s_null;

};

}

using namespace Swinder;

// to be shared between all empty value
ValueData* ValueData::s_null = 0;

// static things
Value ks_value_empty;
Value ks_error_div0;
Value ks_error_na;
Value ks_error_name;
Value ks_error_null;
Value ks_error_num;
Value ks_error_ref;
Value ks_error_value;

// create an empty value
Value::Value()
{
    d = ValueData::null();
}

// destructor
Value::~Value()
{
    d->unref();
}

// create value of certain type
Value::Value(Value::Type _type)
{
    d = new ValueData;
    d->type = _type;
}

// copy constructor
Value::Value(const Value& _value)
{
    d = ValueData::null();
    assign(_value);
}

// assignment operator
Value& Value::operator=(const Value & _value)
{
    return assign(_value);
}

bool Value::operator==(const Value &other) const
{
    return type() == other.type() && asString() == other.asString();
}

bool Value::operator!=(const Value &other) const
{
    return ! (*this == other);
}
    
// create a boolean value
Value::Value(bool b)
{
    d = ValueData::null();
    setValue(b);
}

// create an integer value
Value::Value(int i)
{
    d = ValueData::null();
    setValue(i);
}

// create a floating-point value
Value::Value(double f)
{
    d = ValueData::null();
    setValue(f);
}

// create a string value
Value::Value(const UString& s)
{
    d = ValueData::null();
    setValue(s);
}

// create a richtext value
Value::Value(const UString& s, const std::map<unsigned, FormatFont>& formatRuns)
{
    d = ValueData::null();
    setValue(s, formatRuns);
}

// assign value from other
// shallow copy: only copy the data pointer
Value& Value::assign(const Value& _value)
{
    d->unref();
    d = _value.d;
    d->ref();
    return *this;
}

// return type of the value
Value::Type Value::type() const
{
    return d ? d->type : Empty;
}

// set the value to boolean
void Value::setValue(bool b)
{
    detach();
    d->type = Boolean;
    d->b = b;
}

// get the value as boolean
bool Value::asBoolean() const
{
    bool result = false;

    if (type() == Value::Boolean)
        result = d->b;

    return result;
}

// set the value to integer
void Value::setValue(int i)
{
    detach();
    d->type = Integer;
    d->i = i;
}

// get the value as integer
int Value::asInteger() const
{
    int result = 0;

    if (type() == Value::Integer)
        result = d->i;

    if (type() == Value::Float)
        result = static_cast<int>(d->f);

    return result;
}

void Value::setValue(const Value& v)
{
    assign(v);
}

// set the value as floating-point
void Value::setValue(double f)
{
    detach();
    d->type = Float;
    d->f = f;
}

// get the value as floating-point
double Value::asFloat() const
{
    double result = 0.0;

    if (type() == Value::Float)
        result = d->f;

    if (type() == Value::Integer)
        result = static_cast<double>(d->i);

    return result;
}

// set the value as string
void Value::setValue(const UString& s)
{
    detach();
    d->type = String;
    d->s = s;
}

// get the value as string
UString Value::asString() const
{
    UString result;

    switch(type()) {
        case Value::Error:
        case Value::String:
        case Value::RichText:
            result = d->s;
            break;
        case Value::Boolean:
            result = (asBoolean() ? "True" : "False");
            break;
        case Value::Integer: {
            std::stringstream out;
            out << asInteger();
            result = out.str().c_str();
        } break;
        case Value::Float: {
            std::stringstream out;
            out << asFloat();
            result = out.str().c_str();
        } break;
        case Value::CellRange: // not used yet
        case Value::Array: // not used yet
        case Value::Empty:
            break;
    }
    
    return result;
}

// set the value as rich text
void Value::setValue(const UString& s, const std::map<unsigned, FormatFont>& formatRuns)
{
    detach();
    d->type = RichText;
    d->s = s;
    d->formatRuns = formatRuns;
}

// get the format runs
std::map<unsigned, FormatFont> Value::formatRuns() const
{
    std::map<unsigned, FormatFont> result;

    if (type() == Value::RichText)
        result = d->formatRuns;

    return result;
}

// set error message
void Value::setError(const UString& msg)
{
    detach();
    d->type = Error;
    d->s = msg;
}

// get error message
UString Value::errorMessage() const
{
    UString result;

    if (type() == Value::Error)
        result = d->s;

    return result;
}

// reference to empty value
const Value& Value::empty()
{
    return ks_value_empty;
}

// reference to #DIV/0! error
const Value& Value::errorDIV0()
{
    if (!ks_error_div0.isError())
        ks_error_div0.setError(UString("#DIV/0!"));
    return ks_error_div0;
}

// reference to #N/A error
const Value& Value::errorNA()
{
    if (!ks_error_na.isError())
        ks_error_na.setError(UString("#N/A"));
    return ks_error_na;
}

// reference to #NAME? error
const Value& Value::errorNAME()
{
    if (!ks_error_name.isError())
        ks_error_name.setError(UString("#NAME?"));
    return ks_error_name;
}

// reference to #NUM! error
const Value& Value::errorNUM()
{
    if (!ks_error_num.isError())
        ks_error_num.setError(UString("#NUM!"));
    return ks_error_num;
}

// reference to #NULL! error
const Value& Value::errorNULL()
{
    if (!ks_error_null.isError())
        ks_error_null.setError(UString("#NULL!"));
    return ks_error_null;
}

// reference to #REF! error
const Value& Value::errorREF()
{
    if (!ks_error_ref.isError())
        ks_error_ref.setError(UString("#REF!"));
    return ks_error_ref;
}

// reference to #VALUE! error
const Value& Value::errorVALUE()
{
    if (!ks_error_value.isError())
        ks_error_value.setError(UString("#VALUE!"));
    return ks_error_value;
}

// detach, create deep copy of ValueData
void Value::detach()
{
    if (d->isNull() || (d->count > 1)) {
        ValueData* n;
        n = new ValueData;

        n->type = d->type;
        switch (n->type) {
        case Empty: break;
        case Boolean: n->b = d->b; break;
        case Integer: n->i = d->i; break;
        case Float:   n->f = d->f; break;
        case String:  n->s = d->s; break;
        case Error:   n->s = d->s; break;
        default: break;
        }

        d->unref();
        d = n;
    }
}

std::ostream& Swinder::operator<<(std::ostream& s, Swinder::Value value)
{
    switch (value.type()) {
    case Value::Empty:
        s << "Empty";
        break;
    case Value::Boolean:
        s << "Boolean: " << (value.asBoolean() ? "True" : "False");
        break;
    case Value::Integer:
        s << "Integer: " << value.asInteger();
        break;
    case Value::Float:
        s << "Float: " << value.asFloat();
        break;
    case Value::String:
        s << "String: " << value.asString().ascii();
        break;
    case Value::RichText:
        s << "RichText: " << value.asString().ascii();
        break;
    case Value::Error:
        s << "Error: " << value.errorMessage().ascii();
        break;
    default:
        break;
    };

    return s;
}
