/* This file is part of the KDE project
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

   Based on nexp.cpp : Parser module of Python-like language
   (C) 2001 Jarosław Staniek, MIMUW (www.mimuw.edu.pl)

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
 * Boston, MA 02110-1301, USA.
 */

#include "expression.h"
#include "utils.h"
#include "driver_p.h"
#include "parser/sqlparser.h"
#include "parser/parser_p.h"

#include <ctype.h>

#include <vector>
#include <algorithm>

#include <kdebug.h>
#include <klocale.h>

// Enable to add SQLite-specific functions
//#define KEXIDB_ENABLE_SQLITE_SPECIFIC_FUNCTIONS

CALLIGRADB_EXPORT QString KexiDB::exprClassName(int c)
{
    if (c == KexiDBExpr_Unary)
        return "Unary";
    else if (c == KexiDBExpr_Arithm)
        return "Arithm";
    else if (c == KexiDBExpr_Logical)
        return "Logical";
    else if (c == KexiDBExpr_Relational)
        return "Relational";
    else if (c == KexiDBExpr_SpecialBinary)
        return "SpecialBinary";
    else if (c == KexiDBExpr_Const)
        return "Const";
    else if (c == KexiDBExpr_Variable)
        return "Variable";
    else if (c == KexiDBExpr_Function)
        return "Function";
    else if (c == KexiDBExpr_Aggregation)
        return "Aggregation";
    else if (c == KexiDBExpr_TableList)
        return "TableList";
    else if (c == KexiDBExpr_ArgumentList)
        return "ArgumentList";
    else if (c == KexiDBExpr_QueryParameter)
        return "QueryParameter";

    return "Unknown";
}

using namespace KexiDB;

//=========================================

BaseExpr::BaseExpr(int token)
        : m_cl(KexiDBExpr_Unknown)
        , m_par(0)
        , m_token(token)
{
}

BaseExpr::~BaseExpr()
{
}

Field::Type BaseExpr::type()
{
    return Field::InvalidType;
}

bool BaseExpr::isTextType()
{
    return Field::isTextType(type());
}

bool BaseExpr::isIntegerType()
{
    return Field::isIntegerType(type());
}

bool BaseExpr::isNumericType()
{
    return Field::isNumericType(type());
}

bool BaseExpr::isFPNumericType()
{
    return Field::isFPNumericType(type());
}

bool BaseExpr::isDateTimeType()
{
    return Field::isDateTimeType(type());
}

QString BaseExpr::debugString()
{
    return QString("BaseExpr(%1,type=%1)").arg(m_token).arg(Driver::defaultSQLTypeName(type()));
}

bool BaseExpr::validate(ParseInfo& /*parseInfo*/)
{
    return true;
}

QString BaseExpr::tokenToDebugString(int token)
{
    if (token < 254) {
        if (isprint(token))
            return QString(QChar(uchar(token)));
        else
            return QString::number(token);
    }
    return QLatin1String(tokenName(token));
}

QString BaseExpr::tokenToString(const Driver *driver)
{
    Q_UNUSED(driver);
    if (m_token < 255 && isprint(m_token))
        return tokenToDebugString();
    return QString();
}

NArgExpr* BaseExpr::toNArg()
{
    return dynamic_cast<NArgExpr*>(this);
}
UnaryExpr* BaseExpr::toUnary()
{
    return dynamic_cast<UnaryExpr*>(this);
}
BinaryExpr* BaseExpr::toBinary()
{
    return dynamic_cast<BinaryExpr*>(this);
}
ConstExpr* BaseExpr::toConst()
{
    return dynamic_cast<ConstExpr*>(this);
}
VariableExpr* BaseExpr::toVariable()
{
    return dynamic_cast<VariableExpr*>(this);
}
FunctionExpr* BaseExpr::toFunction()
{
    return dynamic_cast<FunctionExpr*>(this);
}
QueryParameterExpr* BaseExpr::toQueryParameter()
{
    return dynamic_cast<QueryParameterExpr*>(this);
}

//=========================================

NArgExpr::NArgExpr(int aClass, int token)
        : BaseExpr(token)
{
    m_cl = aClass;
}

NArgExpr::NArgExpr(const NArgExpr& expr)
        : BaseExpr(expr)
{
    foreach(BaseExpr* e, expr.list) {
        add(e->copy());
    }
}

NArgExpr::~NArgExpr()
{
    qDeleteAll(list);
}

NArgExpr* NArgExpr::copy() const
{
    return new NArgExpr(*this);
}

Field::Type NArgExpr::type()
{
    switch (m_token) {
    case KEXIDB_TOKEN_BETWEEN_AND:
    case KEXIDB_TOKEN_NOT_BETWEEN_AND:
        foreach (BaseExpr* e, list) {
            Field::Type type = e->type();
            if (type == Field::InvalidType || type == Field::Null) {
                return type;
            }
        }

        return Field::Boolean;
    default:;
    }

    return BaseExpr::type();
}

bool NArgExpr::containsInvalidArgument()
{
    foreach (BaseExpr* e, list) {
        Field::Type type = e->type();
        if (type == Field::InvalidType) {
            return true;
        }
    }
    return false;
}

bool NArgExpr::containsNullArgument()
{
    foreach (BaseExpr* e, list) {
        Field::Type type = e->type();
        if (type == Field::Null) {
            return true;
        }
    }
    return false;
}

QString NArgExpr::debugString()
{
    QString s = QString("NArgExpr(")
                + tokenToString(0) + ", " + "class=" + exprClassName(m_cl);
    foreach(BaseExpr *expr, list) {
        s += ", " +
             expr->debugString();
    }
    s += QString::fromLatin1(",type=%1)").arg(Driver::defaultSQLTypeName(type()));
    return s;
}

QString NArgExpr::toString(const Driver *driver, QuerySchemaParameterValueListIterator* params)
{
    if (BaseExpr::token() == KEXIDB_TOKEN_BETWEEN_AND && list.count() == 3) {
        return list[0]->toString(driver) + " BETWEEN " + list[1]->toString(driver) + " AND " + list[2]->toString(driver);
    }
    if (BaseExpr::token() == KEXIDB_TOKEN_NOT_BETWEEN_AND && list.count() == 3) {
        return list[0]->toString(driver) + " NOT BETWEEN " + list[1]->toString(driver) + " AND " + list[2]->toString(driver);
    }

    QString s;
    s.reserve(256);
    foreach(BaseExpr* e, list) {
        if (!s.isEmpty())
            s += ", ";
        s += e->toString(driver, params);
    }
    return s;
}

void NArgExpr::getQueryParameters(QuerySchemaParameterList& params)
{
    foreach(BaseExpr *e, list) {
        e->getQueryParameters(params);
    }
}

BaseExpr* NArgExpr::arg(int nr)
{
    return list.at(nr);
}

void NArgExpr::add(BaseExpr *expr)
{
    list.append(expr);
    expr->setParent(this);
}

void NArgExpr::prepend(BaseExpr *expr)
{
    list.prepend(expr);
    expr->setParent(this);
}

int NArgExpr::args()
{
    return list.count();
}

bool NArgExpr::validate(ParseInfo& parseInfo)
{
    if (!BaseExpr::validate(parseInfo))
        return false;

    foreach(BaseExpr *e, list) {
        if (!e->validate(parseInfo))
            return false;
    }

    switch (m_token) {
    case KEXIDB_TOKEN_BETWEEN_AND:
    case KEXIDB_TOKEN_NOT_BETWEEN_AND: {
        if (list.count() != 3) {
            parseInfo.errMsg = i18n("Three arguments required");
            parseInfo.errDescr = i18nc("@info BETWEEN..AND error", "%1 operator requires exactly three arguments.", "BETWEEN...AND");
            return false;
        }
        const Field::Type type0 = list[0]->type(); // cache: evaluating type of expressions can be expensive
        const Field::Type type1 = list[1]->type();
        const Field::Type type2 = list[2]->type();
        if (!(!Field::isNumericType(type0) || !Field::isNumericType(type1) || !Field::isNumericType(type1))) {
            return true;
        } else if (!(!Field::isTextType(type0) || !Field::isTextType(type1) || !Field::isTextType(type2))) {
            return true;
        }

        if (type0 == type1 && type1 == type2) {
            return true;
        }

        parseInfo.errMsg = i18n("Incompatible types of arguments");
        parseInfo.errDescr = i18nc("@info BETWEEN..AND type error", "%1 operator requires compatible types of arguments.", "BETWEEN..AND");

        return false;
    }
    default:;
    }

    return true;
}

QString NArgExpr::tokenToString(const Driver *driver)
{
    switch (m_token) {
    case KEXIDB_TOKEN_BETWEEN_AND: return "BETWEEN_AND";
    case KEXIDB_TOKEN_NOT_BETWEEN_AND: return "NOT_BETWEEN_AND";
    default: {
        const QString s = BaseExpr::tokenToString(driver);
        if (!s.isEmpty()) {
            return QString("'%1'").arg(s);
        }
    }
    }
    return QString("{INVALID_N_ARG_OPERATOR#%1}").arg(m_token);
}

//=========================================
UnaryExpr::UnaryExpr(int token, BaseExpr *arg)
        : BaseExpr(token)
        , m_arg(arg)
{
    m_cl = KexiDBExpr_Unary;
    if (m_arg)
        m_arg->setParent(this);
}

UnaryExpr::UnaryExpr(const UnaryExpr& expr)
        : BaseExpr(expr)
        , m_arg(expr.m_arg ? expr.m_arg->copy() : 0)
{
    if (m_arg)
        m_arg->setParent(this);
}

UnaryExpr::~UnaryExpr()
{
    delete m_arg;
}

UnaryExpr* UnaryExpr::copy() const
{
    return new UnaryExpr(*this);
}

QString UnaryExpr::debugString()
{
    return "UnaryExpr('"
           + tokenToDebugString() + "', "
           + (m_arg ? m_arg->debugString() : QString("<NONE>"))
           + QString(",type=%1)").arg(Driver::defaultSQLTypeName(type()));
}

QString UnaryExpr::toString(const Driver *driver, QuerySchemaParameterValueListIterator* params)
{
    if (m_token == '(') //parentheses (special case)
        return '(' + (m_arg ? m_arg->toString(driver, params) : "<NULL>") + ')';
    if (m_token < 255 && isprint(m_token))
        return tokenToDebugString() + (m_arg ? m_arg->toString(driver, params) : "<NULL>");
    if (m_token == NOT)
        return "NOT " + (m_arg ? m_arg->toString(driver, params) : "<NULL>");
    if (m_token == SQL_IS_NULL)
        return (m_arg ? m_arg->toString(driver, params) : "<NULL>") + " IS NULL";
    if (m_token == SQL_IS_NOT_NULL)
        return (m_arg ? m_arg->toString(driver, params) : "<NULL>") + " IS NOT NULL";
    return QString("{INVALID_OPERATOR#%1} ").arg(m_token) + (m_arg ? m_arg->toString(driver, params) : "<NULL>");
}

void UnaryExpr::getQueryParameters(QuerySchemaParameterList& params)
{
    if (m_arg)
        m_arg->getQueryParameters(params);
}

Field::Type UnaryExpr::type()
{
    //NULL IS NOT NULL : BOOLEAN
    //NULL IS NULL : BOOLEAN
    switch (m_token) {
    case SQL_IS_NULL:
    case SQL_IS_NOT_NULL:
        return Field::Boolean;
    }
    const Field::Type t = m_arg->type();
    if (t == Field::Null)
        return Field::Null;
    if (m_token == NOT)
        return Field::Boolean;

    return t;
}

bool UnaryExpr::validate(ParseInfo& parseInfo)
{
    if (!BaseExpr::validate(parseInfo))
        return false;

    if (!m_arg->validate(parseInfo))
        return false;

//! @todo compare types... e.g. NOT applied to Text makes no sense...

    // update type
    if (m_arg->toQueryParameter()) {
        m_arg->toQueryParameter()->setType(type());
    }

    return true;
#if 0
    BaseExpr *n = l.at(0);

    n->check();
    /*typ wyniku:
        const bool dla "NOT <bool>" (negacja)
        int dla "# <str>" (dlugosc stringu)
        int dla "+/- <int>"
        */
    if (is(NOT) && n->nodeTypeIs(TYP_BOOL)) {
        node_type = new NConstType(TYP_BOOL);
    } else if (is('#') && n->nodeTypeIs(TYP_STR)) {
        node_type = new NConstType(TYP_INT);
    } else if ((is('+') || is('-')) && n->nodeTypeIs(TYP_INT)) {
        node_type = new NConstType(TYP_INT);
    } else {
        ERR("Niepoprawny argument typu '%s' dla operatora '%s'",
            n->nodeTypeName(), is(NOT) ? QString("not") : QChar(typ()));
    }
#endif
}

//=========================================
BinaryExpr::BinaryExpr(int aClass, BaseExpr *left_expr, int token, BaseExpr *right_expr)
        : BaseExpr(token)
        , m_larg(left_expr)
        , m_rarg(right_expr)
{
    m_cl = aClass;
    if (m_larg)
        m_larg->setParent(this);
    if (m_rarg)
        m_rarg->setParent(this);
}

BinaryExpr::BinaryExpr(const BinaryExpr& expr)
        : BaseExpr(expr)
        , m_larg(expr.m_larg ? expr.m_larg->copy() : 0)
        , m_rarg(expr.m_rarg ? expr.m_rarg->copy() : 0)
{
}

BinaryExpr::~BinaryExpr()
{
    delete m_larg;
    delete m_rarg;
}

BinaryExpr* BinaryExpr::copy() const
{
    return new BinaryExpr(*this);
}

bool BinaryExpr::validate(ParseInfo& parseInfo)
{
    if (!BaseExpr::validate(parseInfo))
        return false;

    if (!m_larg->validate(parseInfo))
        return false;
    if (!m_rarg->validate(parseInfo))
        return false;

//! @todo compare types..., BITWISE_SHIFT_RIGHT requires integers, etc...

    //update type for query parameters
    QueryParameterExpr * queryParameter = m_larg->toQueryParameter();
    if (queryParameter)
        queryParameter->setType(m_rarg->type());
    queryParameter = m_rarg->toQueryParameter();
    if (queryParameter)
        queryParameter->setType(m_larg->type());

    return true;
}

Field::Type BinaryExpr::type()
{
    const Field::Type lt = m_larg->type(), rt = m_rarg->type();
    if (lt == Field::InvalidType || rt == Field::InvalidType)
        return Field::InvalidType;
    if (lt == Field::Null || rt == Field::Null) {
        if (m_token != OR) //note that NULL OR something   != NULL
            return Field::Null;
    }

    switch (m_token) {
    case BITWISE_SHIFT_RIGHT:
    case BITWISE_SHIFT_LEFT:
    case CONCATENATION:
        return lt;
    }

    const bool ltInt = Field::isIntegerType(lt);
    const bool rtInt = Field::isIntegerType(rt);
    if (ltInt && rtInt)
        return KexiDB::maximumForIntegerTypes(lt, rt);

    if (Field::isFPNumericType(lt) && (rtInt || lt == rt))
        return lt;
    if (Field::isFPNumericType(rt) && (ltInt || lt == rt))
        return rt;

    return Field::Boolean;
}

QString BinaryExpr::debugString()
{
    return QLatin1String("BinaryExpr(")
           + "class=" + exprClassName(m_cl)
           + ',' + (m_larg ? m_larg->debugString() : QString::fromLatin1("<NONE>"))
           + ",'" + tokenToDebugString() + "',"
           + (m_rarg ? m_rarg->debugString() : QString::fromLatin1("<NONE>"))
           + QString::fromLatin1(",type=%1)").arg(Driver::defaultSQLTypeName(type()));
}

QString BinaryExpr::tokenToString(const Driver *driver)
{
    if (m_token < 255 && isprint(m_token))
        return tokenToDebugString();
    // other arithmetic operations: << >>
    switch (m_token) {
    case BITWISE_SHIFT_RIGHT: return ">>";
    case BITWISE_SHIFT_LEFT: return "<<";
        // other relational operations: <= >= <> (or !=) LIKE IN
    case NOT_EQUAL: return "<>";
    case NOT_EQUAL2: return "!=";
    case LESS_OR_EQUAL: return "<=";
    case GREATER_OR_EQUAL: return ">=";
    case LIKE: return driver ? driver->behaviour()->LIKE_OPERATOR : "LIKE";
    case NOT_LIKE: return driver ? (QString::fromLatin1("NOT ") + driver->behaviour()->LIKE_OPERATOR) : QString::fromLatin1("NOT LIKE");
    case SQL_IN: return "IN";
        // other logical operations: OR (or ||) AND (or &&) XOR
    case SIMILAR_TO: return "SIMILAR TO";
    case NOT_SIMILAR_TO: return "NOT SIMILAR TO";
    case OR: return "OR";
    case AND: return "AND";
    case XOR: return "XOR";
        // other string operations: || (as CONCATENATION)
    case CONCATENATION: return "||";
        // SpecialBinary "pseudo operators":
        /* not handled here */
    default:;
    }
    return QString("{INVALID_BINARY_OPERATOR#%1} ").arg(m_token);
}

QString BinaryExpr::toString(const Driver *driver, QuerySchemaParameterValueListIterator* params)
{
#define INFIX(a) \
    (m_larg ? m_larg->toString(driver, params) : "<NULL>") + ' ' + a + ' ' + (m_rarg ? m_rarg->toString(driver, params) : "<NULL>")
    return INFIX(tokenToString(driver));
}

void BinaryExpr::getQueryParameters(QuerySchemaParameterList& params)
{
    if (m_larg)
        m_larg->getQueryParameters(params);
    if (m_rarg)
        m_rarg->getQueryParameters(params);
}

//=========================================
ConstExpr::ConstExpr(int token, const QVariant& val)
        : BaseExpr(token)
        , value(val)
{
    m_cl = KexiDBExpr_Const;
}

ConstExpr::ConstExpr(const ConstExpr& expr)
        : BaseExpr(expr)
        , value(expr.value)
{
}

ConstExpr::~ConstExpr()
{
}

ConstExpr* ConstExpr::copy() const
{
    return new ConstExpr(*this);
}

Field::Type ConstExpr::type()
{
    if (m_token == SQL_NULL)
        return Field::Null;
    else if (m_token == INTEGER_CONST) {
//TODO ok?
//TODO: add sign info?
        if (value.type() == QVariant::Int || value.type() == QVariant::UInt) {
            qint64 v = value.toInt();
            if (v <= 0xff && v > -0x80)
                return Field::Byte;
            if (v <= 0xffff && v > -0x8000)
                return Field::ShortInteger;
            return Field::Integer;
        }
        return Field::BigInteger;
    } else if (m_token == CHARACTER_STRING_LITERAL) {
//TODO: Field::defaultTextLength() is hardcoded now!
        if (Field::defaultMaxLength() > 0
            && uint(value.toString().length()) > Field::defaultMaxLength())
        {
            return Field::LongText;
        }
        else
            return Field::Text;
    } else if (m_token == REAL_CONST) {
        return Field::Double;
    } else if (m_token == HEX_LITERAL) {
        return Field::BLOB;
    } else if (m_token == DATE_CONST)
        return Field::Date;
    else if (m_token == DATETIME_CONST)
        return Field::DateTime;
    else if (m_token == TIME_CONST)
        return Field::Time;

    return Field::InvalidType;
}

QString ConstExpr::debugString()
{
    return QString("ConstExpr('") + tokenToDebugString() + "'," + toString(0)
           + QString(",type=%1)").arg(Driver::defaultSQLTypeName(type()));
}

QString ConstExpr::toString(const Driver *driver, QuerySchemaParameterValueListIterator* params)
{
    Q_UNUSED(driver);
    Q_UNUSED(params);
    if (m_token == SQL_NULL)
        return "NULL";
    else if (m_token == CHARACTER_STRING_LITERAL)
//TODO: better escaping!
        return QLatin1Char('\'') + value.toString() + QLatin1Char('\'');
    else if (m_token == REAL_CONST)
        return QString::fromLatin1(value.toByteArray());
    else if (m_token == HEX_LITERAL) {
        return driver ? driver->escapeBLOB(value.toByteArray())
                      :  KexiDB::escapeBLOB(value.toByteArray(), KexiDB::BLOBEscapeXHex);
    } else if (m_token == DATE_CONST)
        return QLatin1Char('\'') + value.toDate().toString(Qt::ISODate) + QLatin1Char('\'');
    else if (m_token == DATETIME_CONST)
        return QLatin1Char('\'') + value.toDateTime().date().toString(Qt::ISODate)
               + QLatin1Char(' ') + value.toDateTime().time().toString(Qt::ISODate) + QLatin1Char('\'');
    else if (m_token == TIME_CONST)
        return QLatin1Char('\'') + value.toTime().toString(Qt::ISODate) + QLatin1Char('\'');

    return value.toString();
}

void ConstExpr::getQueryParameters(QuerySchemaParameterList& params)
{
    Q_UNUSED(params);
}

bool ConstExpr::validate(ParseInfo& parseInfo)
{
    if (!BaseExpr::validate(parseInfo))
        return false;

    return type() != Field::InvalidType;
}

//=========================================
QueryParameterExpr::QueryParameterExpr(const QString& message)
        : ConstExpr(QUERY_PARAMETER, message)
        , m_type(Field::Text)
{
    m_cl = KexiDBExpr_QueryParameter;
}

QueryParameterExpr::QueryParameterExpr(const QueryParameterExpr& expr)
        : ConstExpr(expr)
        , m_type(expr.m_type)
{
}

QueryParameterExpr::~QueryParameterExpr()
{
}

QueryParameterExpr* QueryParameterExpr::copy() const
{
    return new QueryParameterExpr(*this);
}

Field::Type QueryParameterExpr::type()
{
    return m_type;
}

void QueryParameterExpr::setType(Field::Type type)
{
    m_type = type;
}

QString QueryParameterExpr::debugString()
{
    return QString("QueryParameterExpr('") + QString::fromLatin1("[%2]").arg(value.toString())
           + QString("',type=%1)").arg(Driver::defaultSQLTypeName(type()));
}

QString QueryParameterExpr::toString(const Driver *driver, QuerySchemaParameterValueListIterator* params)
{
    Q_UNUSED(driver);
    return params ? params->getPreviousValueAsString(type()) : QString::fromLatin1("[%2]").arg(value.toString());
}

void QueryParameterExpr::getQueryParameters(QuerySchemaParameterList& params)
{
    QuerySchemaParameter param;
    param.message = value.toString();
    param.type = type();
    params.append(param);
}

bool QueryParameterExpr::validate(ParseInfo& parseInfo)
{
    Q_UNUSED(parseInfo);
    return type() != Field::InvalidType;
}

//=========================================
VariableExpr::VariableExpr(const QString& _name)
        : BaseExpr(0/*undefined*/)
        , name(_name)
        , field(0)
        , tablePositionForField(-1)
        , tableForQueryAsterisk(0)
{
    m_cl = KexiDBExpr_Variable;
}

VariableExpr::VariableExpr(const VariableExpr& expr)
        : BaseExpr(expr)
        , name(expr.name)
        , field(expr.field)
        , tablePositionForField(expr.tablePositionForField)
        , tableForQueryAsterisk(expr.tableForQueryAsterisk)
{
}

VariableExpr::~VariableExpr()
{
}

VariableExpr* VariableExpr::copy() const
{
    return new VariableExpr(*this);
}

QString VariableExpr::debugString()
{
    return QString("VariableExpr(") + name
           + QString(",type=%1)").arg(field ? Driver::defaultSQLTypeName(type()) : QString("FIELD NOT DEFINED YET"));
}

QString VariableExpr::toString(const Driver *driver, QuerySchemaParameterValueListIterator* params)
{
    Q_UNUSED(driver);
    Q_UNUSED(params);
    return name;
}

void VariableExpr::getQueryParameters(QuerySchemaParameterList& params)
{
    Q_UNUSED(params);
}

//! We're assuming it's called after VariableExpr::validate()
Field::Type VariableExpr::type()
{
    if (field)
        return field->type();

    //BTW, asterisks are not stored in VariableExpr outside of parser, so ok.
    return Field::InvalidType;
}

#define IMPL_ERROR(errmsg) parseInfo.errMsg = "Implementation error"; parseInfo.errDescr = errmsg

bool VariableExpr::validate(ParseInfo& parseInfo)
{
    if (!BaseExpr::validate(parseInfo))
        return false;
    field = 0;
    tablePositionForField = -1;
    tableForQueryAsterisk = 0;

    /* taken from parser's addColumn(): */
    KexiDBDbg << "checking variable name: " << name;
    QString tableName, fieldName;
    if (!KexiDB::splitToTableAndFieldParts(name, tableName, fieldName, KexiDB::SetFieldNameIfNoTableName)) {
        return false;
    }
//! @todo shall we also support db name?
    if (tableName.isEmpty()) {//fieldname only
        if (fieldName == "*") {
//   querySchema->addAsterisk( new QueryAsterisk(querySchema) );
            return true;
        }

        //find first table that has this field
        Field *firstField = 0;
        foreach(TableSchema *table, *parseInfo.querySchema->tables()) {
            Field *f = table->field(fieldName);
            if (f) {
                if (!firstField) {
                    firstField = f;
                } else if (f->table() != firstField->table()) {
                    //ambiguous field name
                    parseInfo.errMsg = i18n("Ambiguous field name");
                    parseInfo.errDescr = i18nc("@info",
                        "Both table <resource>%1</resource> and <resource>%2</resource> have "
                        "defined <resource>%3</resource> field. "
                        "Use <resource><placeholder>tableName</placeholder>.%4</resource> notation to specify table name.",
                        firstField->table()->name(), f->table()->name(),
                        fieldName, fieldName);
                    return false;
                }
            }
        }
        if (!firstField) {
            parseInfo.errMsg = i18n("Field not found");
            parseInfo.errDescr = i18n("Table containing \"%1\" field not found.", fieldName);
            return false;
        }
        //ok
        field = firstField; //store
//  querySchema->addField(firstField);
        return true;
    }

    //table.fieldname or tableAlias.fieldname
    TableSchema *ts = parseInfo.querySchema->table(tableName);
    int tablePosition = -1;
    if (ts) {//table.fieldname
        //check if "table" is covered by an alias
        const QList<int> tPositions = parseInfo.querySchema->tablePositions(tableName);
        QByteArray tableAlias;
        bool covered = true;
        foreach(int position, tPositions) {
            tableAlias = parseInfo.querySchema->tableAlias(position);
            if (tableAlias.isEmpty() || tableAlias.toLower() == tableName.toLatin1()) {
                covered = false; //uncovered
                break;
            }
            KexiDBDbg << " --" << "covered by " << tableAlias << " alias";
        }
        if (covered) {
            parseInfo.errMsg = i18n("Could not access the table directly using its name");
            parseInfo.errDescr = i18n("Table name <resource>%1</resource> is covered by aliases. "
                                      "Instead of <resource>%2</resource>, "
                                      "you can write <resource>%3</resource>.",
                                      tableName, tableName + "." + fieldName,
                                      tableAlias + "." + QString(fieldName));
            return false;
        }
        if (!tPositions.isEmpty()) {
            tablePosition = tPositions.first();
        }
    }
    else {//try to find tableAlias
        tablePosition = parseInfo.querySchema->tablePositionForAlias(tableName.toLatin1());
        if (tablePosition >= 0) {
            ts = parseInfo.querySchema->tables()->at(tablePosition);
            if (ts) {
//    KexiDBDbg << " --it's a tableAlias.name";
            }
        }
    }

    if (!ts) {
        parseInfo.errMsg = i18n("Table not found");
        parseInfo.errDescr = i18n("Unknown table \"%1\".", tableName);
        return false;
    }

    if (!parseInfo.repeatedTablesAndAliases.contains(tableName)) {  //for sanity
        IMPL_ERROR(tableName + "." + fieldName + ", !positionsList ");
        return false;
    }
    const QList<int> positionsList(parseInfo.repeatedTablesAndAliases.value(tableName));

    //it's a table.*
    if (fieldName == "*") {
        if (positionsList.count() > 1) {
            parseInfo.errMsg = i18n("Ambiguous \"%1.*\" expression", tableName);
            parseInfo.errDescr = i18n("More than one \"%1\" table or alias defined.", tableName);
            return false;
        }
        tableForQueryAsterisk = ts;
//   querySchema->addAsterisk( new QueryAsterisk(querySchema, ts) );
        return true;
    }

// KexiDBDbg << " --it's a table.name";
    Field *realField = ts->field(fieldName);
    if (!realField) {
        parseInfo.errMsg = i18n("Field not found");
        parseInfo.errDescr = i18n("Table \"%1\" has no \"%2\" field.", tableName, fieldName);
        return false;
    }

    // check if table or alias is used twice and both have the same column
    // (so the column is ambiguous)
    if (positionsList.count() > 1) {
        parseInfo.errMsg = i18n("Ambiguous \"%1.%2\" expression", tableName, fieldName);
        parseInfo.errDescr = i18n("More than one \"%1\" table or alias defined containing \"%2\" field.",
                                  tableName, fieldName);
        return false;
    }
    field = realField; //store
    tablePositionForField = tablePosition;
//    querySchema->addField(realField, tablePosition);

    return true;
}

//=========================================

static const char* const FunctionExpr_builtIns_[] = {"SUM", "MIN", "MAX", "AVG", "COUNT", "STD", "STDDEV", "VARIANCE", 0 };

//! A set of names of aggregation SQL functions.
class BuiltInAggregates : public QSet<QByteArray>
{
public:
    BuiltInAggregates() : QSet<QByteArray>() {
        for (const char * const *p = FunctionExpr_builtIns_; *p; p++)
            insert(QByteArray::fromRawData(*p, qstrlen(*p)));
    }
};

K_GLOBAL_STATIC(BuiltInAggregates, _builtInAggregates)

//! Type of a single function argument, used with Field::Type values.
//! Used to indicate that multiple types are allowed.
enum BuiltInFunctionArgumentType
{
    AnyText = Field::LastType + 1,
    AnyInt,
    AnyFloat,
    AnyNumber,
    Any
};

//! Declaration of a single built-in function. It can offer multiple signatures.
class BuiltInFunctionDeclaration
{
public:
    inline BuiltInFunctionDeclaration()
        : defaultReturnType(Field::InvalidType), copyReturnTypeFromArg(-1)
    {
    }
    virtual ~BuiltInFunctionDeclaration() {}
    virtual Field::Type returnType(FunctionExpr* f, ParseInfo* parseInfo) const {
        Q_UNUSED(parseInfo);
        if (f->args->containsNullArgument()) {
            return Field::Null;
        }
        if (copyReturnTypeFromArg >= 0 && copyReturnTypeFromArg < f->args->args()) {
            return f->args->arg(copyReturnTypeFromArg)->type();
        }
        return defaultReturnType;
    }
    std::vector<int**> signatures;
protected:
    Field::Type defaultReturnType;
    int copyReturnTypeFromArg;
    friend class BuiltInFunctions;
};

//! Declaration of a single built-in function COALESCE() and similar ones.
class CoalesceFunctionDeclaration : public BuiltInFunctionDeclaration
{
public:
    CoalesceFunctionDeclaration() {}
    virtual Field::Type returnType(FunctionExpr* f, ParseInfo* parseInfo) const {
        Q_UNUSED(parseInfo);
        for(int i = 0; i < f->args->args(); ++i) {
            BaseExpr *expr = f->args->arg(i);
            const Field::Type t = expr->type();
            if (t != Field::Null) {
                return t;
            }
        }
        return Field::Null;
    }
};

//! Declaration of a single built-in function MIN(), MAX() and similar ones.
//! Its return type is:
//! - NULL is any argument is NULL
//! - valid type if types of all arguments are compatible (e.g. text, numeric, date...)
//! - InvalidType if types of any two are incompatible
class MinMaxFunctionDeclaration : public BuiltInFunctionDeclaration
{
public:
    MinMaxFunctionDeclaration() {}
    virtual Field::Type returnType(FunctionExpr* f, ParseInfo* parseInfo) const {
        if (f->args->args() == 0) {
            return Field::Null;
        }
        const Field::Type type0 = f->args->arg(0)->type(); // cache: evaluating type of expressions can be expensive
        if (nullOrInvalid(type0)) {
            return type0;
        }
        Field::TypeGroup prevTg = Field::typeGroup(type0); // use typegroup for simplicity
        for(int i = 1; i < f->args->args(); ++i) {
            BaseExpr *expr = f->args->arg(i);
            const Field::Type t = expr->type();
            if (nullOrInvalid(t)) {
                return t;
            }
            const Field::TypeGroup tg = Field::typeGroup(t);
            if ((prevTg == Field::IntegerGroup || prevTg == Field::FloatGroup)
                && (tg == Field::IntegerGroup || tg == Field::FloatGroup))
            {
                if (prevTg == Field::IntegerGroup && tg == Field::FloatGroup) {
                    prevTg = Field::FloatGroup; // int -> float
                }
                continue;
            }
            if (prevTg == tg) {
                continue;
            }
            if (parseInfo) {
                parseInfo->errMsg = QString("Incompatible types in %1() function").arg(f->name);
                parseInfo->errDescr = QString("Argument #%1 of type \"%2\" in function %3() is not "
                                           "compatible with previous arguments of type \"%4\".")
                                           .arg(i+1)
                                           .arg(Field::typeName(simpleTypeForGroup(tg)))
                                           .arg(f->name)
                                           .arg(Field::typeName(simpleTypeForGroup(prevTg)));
            }
            return Field::InvalidType;
        }
        return safeTypeForGroup(prevTg);
    }
private:
    static bool nullOrInvalid(Field::Type type) {
        return type == Field::Null || type == Field::InvalidType;
    }
    //! @return safe default type for type group @a tg (too big sizes better than too small)
    static Field::Type safeTypeForGroup(Field::TypeGroup tg) {
        switch (tg) {
        case Field::TextGroup: return Field::LongText;
        case Field::IntegerGroup: return Field::BigInteger;
        case Field::FloatGroup: return Field::Double;
        case Field::BooleanGroup: return Field::Boolean;
        case Field::DateTimeGroup: return Field::DateTime;
        case Field::BLOBGroup: return Field::BLOB;
        default: break;
        }
        return Field::InvalidType;
    }
    //! @return resonable default type for type group @a tg (used for displaying in error message)
    static Field::Type simpleTypeForGroup(Field::TypeGroup tg) {
        switch (tg) {
        case Field::TextGroup: return Field::Text;
        case Field::IntegerGroup: return Field::Integer;
        case Field::FloatGroup: return Field::Double;
        case Field::BooleanGroup: return Field::Boolean;
        case Field::DateTimeGroup: return Field::DateTime;
        case Field::BLOBGroup: return Field::BLOB;
        default: break;
        }
        return Field::InvalidType;
    }
};

//! Declaration of a single built-in function RANDOM() and RANDOM(X,Y).
//! Its return type is:
//! - Double when number of arguments is zero
//! - integer if there are two integer arguments (see KexiDB::maximumForIntegerTypes())
//! - InvalidType for other number of arguments
class RandomFunctionDeclaration : public BuiltInFunctionDeclaration
{
public:
    RandomFunctionDeclaration() {}
    virtual Field::Type returnType(FunctionExpr* f, ParseInfo* parseInfo) const {
        Q_UNUSED(parseInfo);
        if (f->args->args() == 0) {
            return Field::Double;
        }
        if (f->args->args() == 2) {
            ConstExpr *const0 = f->args->arg(0)->toConst();
            ConstExpr *const1 = f->args->arg(1)->toConst();
            if (const0 && const1) {
                bool ok0;
                const qlonglong val0 = const0->value.toLongLong(&ok0);
                bool ok1;
                const qlonglong val1 = const1->value.toLongLong(&ok1);
                if (ok0 && ok1) {
                    if (val0 >= val1) {
                        if (parseInfo) {
                            parseInfo->errMsg = QString("Invalid arguments of %1() function").arg(f->name);
                            parseInfo->errDescr = QString("Value of the first argument should be less than value of the second argument.");
                        }
                        return Field::InvalidType;
                    }
                }
            }
            const Field::Type t0 = f->args->arg(0)->type();
            const Field::Type t1 = f->args->arg(1)->type();
            return KexiDB::maximumForIntegerTypes(t0, t1);
        }
        return Field::InvalidType;
    }
};

//! Declaration of a single built-in function CEILING(X) and FLOOR(X).
//! Its return type is:
//! - Double when number of arguments is zero
//! - integer if there are two integer arguments (see KexiDB::maximumForIntegerTypes())
//! - InvalidType for other number of arguments
class CeilingFloorFunctionDeclaration : public BuiltInFunctionDeclaration
{
public:
    CeilingFloorFunctionDeclaration() {}
    virtual Field::Type returnType(FunctionExpr* f, ParseInfo* parseInfo) const {
        Q_UNUSED(parseInfo);
        if (f->args->args() == 1) {
            const Field::Type type = f->args->arg(0)->type(); // cache: evaluating type of expressions can be expensive
            if (Field::isFPNumericType(type)) {
                return Field::BigInteger;
            }
            switch (type) {
            case Field::Byte: return Field::ShortInteger;
            case Field::ShortInteger: return Field::Integer;
            case Field::Integer: return Field::BigInteger;
            case Field::Null: return Field::Null;
            case Field::InvalidType: return Field::InvalidType;
            default:;
            }
        }
        return Field::InvalidType;
    }
};

//! A map of built-in SQL functions
//! See https://community.kde.org/Kexi/Plugins/Queries/SQL_Functions for the status.
class BuiltInFunctions : public QHash<QString, BuiltInFunctionDeclaration*>
{
public:
    BuiltInFunctions();
    ~BuiltInFunctions() {
        qDeleteAll(*this);
    }

    //! @return function declaration's structure for name @a name
    //! If @a name is alias of the function, e.g. "MIN" for "LEAST", the original
    //! function's declaration is returned.
    BuiltInFunctionDeclaration* value(const QString &name) const;

    //! @return a list of function aliases.
    QStringList aliases() const;

    static int multipleArgs[];
private:
    QHash<QString, BuiltInFunctionDeclaration*> m_aliases;
};

int BuiltInFunctions::multipleArgs[] = {};

BuiltInFunctions::BuiltInFunctions()
    : QHash<QString, BuiltInFunctionDeclaration*>()
{
    BuiltInFunctionDeclaration *decl;
#define _TYPES(name, args...) static int name[] = { args, Field::InvalidType }
    _TYPES(argAnyTextOrNull, AnyText, Field::Null);
    _TYPES(argAnyIntOrNull, AnyInt, Field::Null);
    _TYPES(argAnyNumberOrNull, AnyNumber, Field::Null);
    _TYPES(argAnyFloatOrNull, AnyFloat, Field::Null);
    Q_UNUSED(argAnyFloatOrNull);
    _TYPES(argAnyOrNull, Any, Field::Null);
    _TYPES(argBLOBOrNull, Field::BLOB, Field::Null);
    Q_UNUSED(argBLOBOrNull);
    _TYPES(argAnyTextBLOBOrNull, AnyText, Field::BLOB, Field::Null);
#undef _TYPES

//! Adds a signature named @a name with specified arguments to declaration decl
#define _SIG(name, args...) \
    static int* name[] = { args, 0 }; \
    decl->signatures.push_back(name)

//! Adds a signature with no arguments to declaration decl
#define _SIG0 \
    decl->signatures.push_back(sig0)

    static int* sig0[] = { 0 };

    insert(QLatin1String("ABS"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The abs(X) function returns the absolute value of the numeric argument X.
     Abs(X) returns NULL if X is NULL. Abs(X) returns 0.0 if X is a string or blob that
     cannot be converted to a numeric value. If X is the integer -9223372036854775808
     then abs(X) throws an integer overflow error since there is no equivalent positive
     64-bit two complement value. */
    // example: SELECT ABS(-27), ABS(-3.1415), ABS(NULL + 1)
    // result: 27, 3.1415, NULL
    decl->copyReturnTypeFromArg = 0;
    _SIG(abs_1, argAnyNumberOrNull);

    insert(QLatin1String("CEILING"), decl = new CeilingFloorFunctionDeclaration);
    /* ceiling(X) returns the largest integer value not less than X. */
    // See also https://dev.mysql.com/doc/refman/5.1/en/mathematical-functions.html#function_ceiling
    // See also http://www.postgresql.org/docs/9.5/static/functions-math.html#FUNCTIONS-MATH-FUNC-TABLE
    // SQLite has no equivalent of ceiling() so this is used:
    // (CASE WHEN X = CAST(X AS INT) THEN CAST(X AS INT) WHEN X >= 0 THEN CAST(X AS INT) + 1 ELSE CAST(X AS INT) END)
    //! @todo add a custom function to SQLite to optimize/simplify things
    // example: SELECT CEILING(3.14), CEILING(-99.001)
    // result: 4, -99
    _SIG(ceiling, argAnyNumberOrNull);

    insert(QLatin1String("CHAR"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The char(X1,X2,...,XN) function returns a string composed of characters having
     the unicode code point values of integers X1 through XN, respectively. */
    // example: SELECT CHAR(75,69,88,73), CHAR()
    // result: "KEXI" ""
    decl->defaultReturnType = Field::LongText;
    static int char_min_args[] = { 0 };
    _SIG(char_N, argAnyIntOrNull, multipleArgs, char_min_args);

    insert(QLatin1String("COALESCE"), decl = new CoalesceFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The coalesce() function returns a copy of its first non-NULL argument, or NULL if
     all arguments are NULL. Coalesce() must have at least 2 arguments. */
    // example: SELECT COALESCE(NULL, 17, NULL, "A")
    // result: 17
    static int coalesce_min_args[] = { 2 };
    _SIG(coalesce_N, argAnyOrNull, multipleArgs, coalesce_min_args);

    insert(QLatin1String("FLOOR"), decl = new CeilingFloorFunctionDeclaration);
    /* floor(X) returns the largest integer value not greater than X. */
    // See also https://dev.mysql.com/doc/refman/5.1/en/mathematical-functions.html#function_floor
    // See also http://www.postgresql.org/docs/9.5/static/functions-math.html#FUNCTIONS-MATH-FUNC-TABLE
    // SQLite has no equivalent of floor() so this is used:
    // (CASE WHEN X >= 0 OR X = CAST(X AS INT) THEN CAST(X AS INT) ELSE CAST(X AS INT) - 1 END)
    //! @todo add a custom function to SQLite to optimize/simplify things
    // example: SELECT FLOOR(3.14), FLOOR(-99.001)
    // result: 3, -100
    _SIG(floor, argAnyNumberOrNull);

    insert(QLatin1String("GREATEST"), decl = new MinMaxFunctionDeclaration);
    m_aliases.insert("MAX", decl);
    // From https://www.sqlite.org/lang_corefunc.html
    // For SQLite MAX() is used.
    // If arguments are of text type, to each argument default (unicode) collation
    // is assigned that is configured for SQLite by KexiDB.
    // Example: SELECT MAX('ą' COLLATE '', 'z' COLLATE '').
    // Example: SELECT MAX('ą' COLLATE '', 'z' COLLATE '').
    /* The multi-argument max() function returns the argument with the maximum value, or
    return NULL if any argument is NULL. The multi-argument max() function searches its
    arguments from left to right for an argument that defines a collating function and
    uses that collating function for all string comparisons. If none of the arguments to
    max() define a collating function, then the BINARY collating function is used. Note
    that max() is a simple function when it has 2 or more arguments but operates as an
    aggregate function if given only a single argument. */
    // For pgsql GREATEST() function ignores NULL values, it only returns NULL
    // if all the expressions evaluate to NULL. So this is used for MAX(v0,..,vN):
    // (CASE WHEN (v0) IS NULL OR .. OR (vN) IS NULL THEN NULL ELSE GREATEST(v0,..,vN) END)
    // See also http://www.postgresql.org/docs/9.5/static/functions-conditional.html#FUNCTIONS-GREATEST-LEAST
    //! @todo for pgsql CREATE FUNCTION can be used to speed up and simplify things
    // For mysql GREATEST() is used.
    // See https://dev.mysql.com/doc/refman/5.1/en/comparison-operators.html#function_greatest
    // Note: Before MySQL 5.0.13, GREATEST() returns NULL only if all arguments are NULL
    // (like pgsql). As of 5.0.13, it returns NULL if any argument is NULL (like sqlite's MAX()).
    // See also https://bugs.mysql.com/bug.php?id=15610
    //! @todo MySQL: check for server version and don't use the pgsql's approach for ver >= 5.0.13
    //!       We cannot do that now because we only have access to driver, not the connection.
    // example: SELECT GREATEST("Z", "ą", "AA"), MAX(0.1, 7.1, 7), GREATEST(9, NULL, -1)
    // result: "Z", 7.1, NULL
    static int greatest_min_args[] = { 2 };
    _SIG(greatest_N, argAnyOrNull, multipleArgs, greatest_min_args);

    insert(QLatin1String("HEX"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_hex
    /* The hex() function interprets its argument as a BLOB and returns a string which is
    the upper-case hexadecimal rendering of the content of that blob. */
    /* For pgsql UPPER(ENCODE(val, 'hex')) is used,
       See http://www.postgresql.org/docs/9.5/static/functions-string.html#FUNCTIONS-STRING-OTHER */
    // example: SELECT HEX(X'BEEF'), HEX('DEAD')
    // result: "BEEF", "44454144"
    //! @todo HEX(int) for SQLite is not the same as HEX(int) for MySQL so we disable it
    //!       -- maybe can be wrapped?
    decl->defaultReturnType = Field::LongText;
    _SIG(hex_1, argAnyTextBLOBOrNull);

    insert(QLatin1String("IFNULL"), decl = new CoalesceFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The ifnull() function returns a copy of its first non-NULL argument, or NULL if
     both arguments are NULL. Ifnull() must have exactly 2 arguments. The ifnull() function
     is equivalent to coalesce() with two arguments. */
    // For postgresql coalesce() is used.
    // example: SELECT IFNULL(NULL, 17), IFNULL(NULL, NULL)
    // result: 17, NULL
    _SIG(ifnull_2, argAnyOrNull, argAnyOrNull);

    insert(QLatin1String("INSTR"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The instr(X,Y) function finds the first occurrence of string Y within string X and
     returns the number of prior characters plus 1, or 0 if Y is nowhere found within X.
     If both arguments X and Y to instr(X,Y) are non-NULL and are not BLOBs then both are
     interpreted as strings. If either X or Y are NULL in instr(X,Y) then the result
     is NULL. */
    //! @todo PostgreSQL does not have instr() but CREATE FUNCTION can be used,
    //!       see http://www.postgresql.org/docs/9.5/static/plpgsql-porting.html
    //! @todo support (BLOB, BLOB)?
    /* From the same docs:
     Or, if X and Y are both BLOBs, then instr(X,Y) returns one more than the number bytes
     prior to the first occurrence of Y, or 0 if Y does not occur anywhere within X. */
    // example: SELECT INSTR("KEXI", "X"), INSTR("KEXI", "ZZ")
    // result: 3, 0
    decl->defaultReturnType = Field::Integer;
    _SIG(instr_2, argAnyTextOrNull, argAnyTextOrNull);

    insert(QLatin1String("LEAST"), decl = new MinMaxFunctionDeclaration);
    m_aliases.insert("MIN", decl);
    // From https://www.sqlite.org/lang_corefunc.html
    // For SQLite uses MIN().
    /* The multi-argument min() function returns the argument with the minimum value, or
    return NULL if any argument is NULL. The multi-argument min() function searches its
    arguments from left to right for an argument that defines a collating function and
    uses that collating function for all string comparisons. If none of the arguments to
    max() define a collating function, then the BINARY collating function is used. Note
    that max() is a simple function when it has 2 or more arguments but operates as an
    aggregate function if given only a single argument. */
    // For pgsql LEAST() function ignores NULL values, it only returns NULL
    // if all the expressions evaluate to NULL. So this is used for MAX(v0,..,vN):
    // (CASE WHEN (v0) IS NULL OR .. OR (vN) IS NULL THEN NULL ELSE LEAST(v0,..,vN) END)
    // See also http://www.postgresql.org/docs/9.5/static/functions-conditional.html#FUNCTIONS-GREATEST-LEAST
    //! @todo for pgsql CREATE FUNCTION can be used to speed up and simplify things
    // For mysql LEAST() is used.
    // See https://dev.mysql.com/doc/refman/5.1/en/comparison-operators.html#function_least
    // Note: Before MySQL 5.0.13, LEAST() returns NULL only if all arguments are NULL
    // (like pgsql). As of 5.0.13, it returns NULL if any argument is NULL (like sqlite's MIN()).
    //! @todo MySQL: check for server version and don't use the pgsql's approach for ver >= 5.0.13
    //!       We cannot do that now because we only have access to driver, not the connection.
    // See also https://bugs.mysql.com/bug.php?id=15610
    // example: SELECT LEAST("Z", "ą", "AA"), MIN(0.1, 7.1, 7), LEAST(9, NULL, -1)
    // result: "ą", 0.1, NULL
    static int least_min_args[] = { 2 };
    _SIG(least_N, argAnyOrNull, multipleArgs, least_min_args);

    insert(QLatin1String("LENGTH"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_length
    /* For a string value X, the length(X) function returns the number of characters (not
    bytes) in X prior to the first NUL character. Since SQLite strings do not normally
    contain NUL characters, the length(X) function will usually return the total number
    of characters in the string X. For a blob value X, length(X) returns the number of
    bytes in the blob. If X is NULL then length(X) is NULL. If X is numeric then
    length(X) returns the length of a string representation of X. */
    /* For postgres octet_length(val) is used if val is a of BLOB type.
       length(val) for BLOB cannot be used because it returns number of bits. */
    /* For mysql char_length(val) is used.
       This is because length(val) in mysql returns number of bytes, what is not right for
       multibyte (unicode) encodings. */
    // example: SELECT LENGTH('Straße'), LENGTH(X'12FE')
    // result: 6, 2
    decl->defaultReturnType = Field::Integer;
    _SIG(length_1, argAnyTextBLOBOrNull);

    insert(QLatin1String("LOWER"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The lower(X) function returns a copy of string X with all characters converted
     to lower case. */
    // Note: SQLite such as 3.8 without ICU extension does not convert non-latin1 characters
    // too well; Kexi uses ICU extension by default so the results are very good.
    // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_lower
    // See also http://www.postgresql.org/docs/9.5/static/functions-string.html#FUNCTIONS-STRING-SQL
    // example: SELECT LOWER("MEGSZENTSÉGTELENÍTHETETLENSÉGESKEDÉSEITEKÉRT")
    // result: "megszentségteleníthetetlenségeskedéseitekért"
    decl->defaultReturnType = Field::LongText;
    _SIG(lower_1, argAnyTextOrNull);

    insert(QLatin1String("LTRIM"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The ltrim(X,Y) function returns a string formed by removing any and all characters
     that appear in Y from the left side of X. If the Y argument is omitted, ltrim(X)
     removes spaces from the left side of X.*/
    // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_ltrim
    //! @todo MySQL's LTRIM only supports one arg. TRIM() does not work too
    //! https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_trim
    // See also http://www.postgresql.org/docs/9.5/static/functions-string.html#FUNCTIONS-STRING-SQL
    // example: SELECT LTRIM("  John Smith")
    // result: "John Smith"
    // example: SELECT LTRIM("a b or c", "ab ")
    // result: "or c"
    decl->defaultReturnType = Field::LongText;
    _SIG(ltrim_1, argAnyTextOrNull);
    _SIG(ltrim_2, argAnyTextOrNull, argAnyTextOrNull);

    insert(QLatin1String("NULLIF"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The nullif(X,Y) function returns its first argument if the arguments are different
     and NULL if the arguments are the same. The nullif(X,Y) function searches its
     arguments from left to right for an argument that defines a collating function and
     uses that collating function for all string comparisons. If neither argument to
     nullif() defines a collating function then the BINARY is used. */
    // See also https://dev.mysql.com/doc/refman/5.1/en/control-flow-functions.html#function_nullif
    // See also http://www.postgresql.org/docs/9.5/static/functions-conditional.html#FUNCTIONS-NULLIF
    // example: SELECT NULLIF("John", "Smith"), NULLIF(177, 177)
    // result: "John", NULL
    decl->copyReturnTypeFromArg = 0;
    _SIG(nullif_2, argAnyOrNull, argAnyOrNull);

    insert(QLatin1String("RANDOM"), decl = new RandomFunctionDeclaration);
    /* RANDOM() returns a random floating-point value v in the range 0 <= v < 1.0.
     RANDOM(X,Y) - returns returns a random integer that is equal or greater than X
     and less than Y. */
    // For MySQL RANDOM() is equal to RAND().
    // For MySQL RANDOM(X,Y) is equal to (X + FLOOR(RAND() * (Y - X))
    // For PostreSQL RANDOM() is equal to RANDOM().
    // For PostreSQL RANDOM(X,Y) is equal to (X + FLOOR(RANDOM() * (Y - X))
    // Because SQLite returns integer between -9223372036854775808 and +9223372036854775807,
    // so RANDOM() for SQLite is equal to (RANDOM()+9223372036854775807)/18446744073709551615.
    // Similarly, RANDOM(X,Y) for SQLite is equal
    // to (X + CAST((Y - X) * (RANDOM()+9223372036854775807)/18446744073709551615 AS INT)).
    // See also https://dev.mysql.com/doc/refman/5.1/en/mathematical-functions.html#function_rand
    // See also http://www.postgresql.org/docs/9.5/static/functions-math.html#FUNCTIONS-MATH-RANDOM-TABLE
    //! @note rand(X) (where X is a seed value to set) isn't portable between MySQL and PostgreSQL,
    //! and does not exist in SQLite, so we don't support it.
    // example: SELECT RANDOM(), RANDOM(2, 5)
    // result: (some random floating-point value v where 0 <= v < 1.0)
    // example: SELECT RANDOM(2, 5)
    // result: (some random integer value v where 2 <= v < 5)
    decl->defaultReturnType = Field::Double;
    _SIG0;
    _SIG(random_2, argAnyIntOrNull, argAnyIntOrNull);

    insert(QLatin1String("ROUND"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The round(X,Y) function returns a floating-point value X rounded to Y digits to the
     right of the decimal point. If the Y argument is omitted, it is assumed to be 0. */
    // See also https://dev.mysql.com/doc/refman/5.1/en/mathematical-functions.html#function_round
    // See also http://www.postgresql.org/docs/9.5/static/functions-math.html#FUNCTIONS-MATH-FUNC-TABLE
    //! @note round(X,Y) where Y < 0 is supported only by MySQL so we ignore this case
    // example: SELECT ROUND(-1.13), ROUND(-5.51), ROUND(5.51), ROUND(1.298, 1), ROUND(1.298, 0), ROUND(7)
    // result: -1, -6, 6, 1.3, 1, 7
    decl->copyReturnTypeFromArg = 0;
    _SIG(round_1, argAnyNumberOrNull);
    _SIG(round_2, argAnyNumberOrNull, argAnyIntOrNull);

    insert(QLatin1String("RTRIM"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The rtrim(X,Y) function returns a string formed by removing any and all characters
     that appear in Y from the right side of X. If the Y argument is omitted, rtrim(X)
     removes spaces from the right side of X. */
    // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_ltrim
    //! @todo MySQL's RTRIM only supports one arg. TRIM() does not work too
    //! https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_trim
    // See also http://www.postgresql.org/docs/9.5/static/functions-string.html#FUNCTIONS-STRING-SQL
    // example: SELECT RTRIM("John Smith   ")
    // result: "John Smith"
    // example: SELECT RTRIM("a b or c", "orc ")
    // result: "a b"
    decl->defaultReturnType = Field::LongText;
    _SIG(rtrim_1, argAnyTextOrNull);
    _SIG(rtrim_2, argAnyTextOrNull, argAnyTextOrNull);

    insert(QLatin1String("SOUNDEX"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The soundex(X) function returns a string that is the soundex encoding of the string
     X. The string "?000" is returned if the argument is NULL or contains non-ASCII
     alphabetic characters. */
    // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_soundex
    // See also http://www.postgresql.org/docs/9.5/static/fuzzystrmatch.html#AEN165853
    //! @todo we call drv_executeSQL("CREATE EXTENSION fuzzystrmatch") on connection, do that on first use of SOUNDEX()
    // example: SELECT SOUNDEX("John")
    // result: "J500"
    decl->defaultReturnType = Field::Text;
    _SIG(soundex, argAnyTextOrNull);

    insert(QLatin1String("SUBSTR"), decl = new BuiltInFunctionDeclaration);
    // From https://www.sqlite.org/lang_corefunc.html
    /* The substr(X,Y) returns all characters through the end of the string X beginning with
    the Y-th. The left-most character of X is number 1. If Y is negative then the
    first character of the substring is found by counting from the right rather than
    the left. If Z is negative then the abs(Z) characters preceding the Y-th
    character are returned. If X is a string then characters indices refer to actual
    UTF-8 characters. If X is a BLOB then the indices refer to bytes. */
    _SIG(substr_2, argAnyTextOrNull, argAnyIntOrNull);
    /* The substr(X,Y,Z) function returns a substring of input string X that begins
    with the Y-th character and which is Z characters long. */
    _SIG(substr_3, argAnyTextOrNull, argAnyIntOrNull, argAnyIntOrNull);
    decl->copyReturnTypeFromArg = 0;

     insert(QLatin1String("TRIM"), decl = new BuiltInFunctionDeclaration);
     // From https://www.sqlite.org/lang_corefunc.html
     /* The trim(X,Y) function returns a string formed by removing any and all characters
      that appear in Y from both ends of X. If the Y argument is omitted, trim(X) removes
      spaces from both ends of X. */
     // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_trim
     //! @todo MySQL's TRIM only supports one arg. TRIM() does not work too
     //! https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_trim
     // See also http://www.postgresql.org/docs/9.5/static/functions-string.html#FUNCTIONS-STRING-SQL
     // example: SELECT TRIM("  John Smith   ")
     // result: "John Smith"
     // example: SELECT TRIM("a b or c", "orca ")
     // result: "b"
     decl->defaultReturnType = Field::LongText;
     _SIG(trim_1, argAnyTextOrNull);
     _SIG(trim_2, argAnyTextOrNull, argAnyTextOrNull);

     insert(QLatin1String("UNICODE"), decl = new BuiltInFunctionDeclaration);
     // From https://www.sqlite.org/lang_corefunc.html
     /* The unicode(X) function returns the numeric unicode code point corresponding to
      the first character of the string X. If the argument to unicode(X) is not a string
      then the result is undefined. */
     // For MySQL ORD(CONVERT(X USING UTF16)) is used (ORD(X) returns a UTF-16 number)
     // For PostreSQL ASCII(X) is used.
     // example: SELECT UNICODE('A'), UNICODE('ą'), UNICODE('Δ'), UNICODE('葉')
     // result: 65, 261, 916, 33865
     decl->defaultReturnType = Field::Integer;
     _SIG(unicode_1, argAnyTextOrNull);

     insert(QLatin1String("UPPER"), decl = new BuiltInFunctionDeclaration);
     // From https://www.sqlite.org/lang_corefunc.html
     /* The upper(X) function returns a copy of string X with all characters converted
      to upper case. */
     // Note: SQLite such as 3.8 without ICU extension does not convert non-latin1 characters
     // too well; Kexi uses ICU extension by default so the results are very good.
     // See also https://dev.mysql.com/doc/refman/5.1/en/string-functions.html#function_upper
     // See also http://www.postgresql.org/docs/9.5/static/functions-string.html#FUNCTIONS-STRING-SQL
     // example: SELECT UPPER("megszentségteleníthetetlenségeskedéseitekért")
     // result: "MEGSZENTSÉGTELENÍTHETETLENSÉGESKEDÉSEITEKÉRT"
     decl->defaultReturnType = Field::LongText;
     _SIG(upper_1, argAnyTextOrNull);

#ifdef KEXIDB_ENABLE_SQLITE_SPECIFIC_FUNCTIONS
    insert(QLatin1String("GLOB"), decl = new BuiltInFunctionDeclaration);
    //! @todo GLOB(X,Y) is SQLite-specific and is not present in MySQL so we don't expose it; use GLOB operator instead.
    //! We may want to address it in raw SQL generation time.
    // From https://www.sqlite.org/lang_corefunc.html
    /* The glob(X,Y) function is equivalent to the expression "Y GLOB X". Note that the
     X and Y arguments are reversed in the glob() function relative to the infix GLOB
     operator. */
    // example: SELECT GLOB("Foo*", "FooBar"), GLOB("Foo*", "foobar")
    // result: TRUE, FALSE
    decl->defaultReturnType = Field::Boolean;
    _SIG(glob_2, argAnyTextOrNull, argAnyOrNull /* will be casted to text */);

    insert(QLatin1String("LIKE"), decl = new BuiltInFunctionDeclaration);
    //! @todo LIKE(X,Y,[Z]) not present in MySQL so we don't expose it; use LIKE operator instead.
    //! We may want to address it in raw SQL generation time.
    // From https://www.sqlite.org/lang_corefunc.html
    /* The like() function is used to implement the "Y LIKE X [ESCAPE Z]" expression. If the
    optional ESCAPE clause is present, then the like() function is invoked with three
    arguments. Otherwise, it is invoked with two arguments only. Note that the X and Y
    parameters are reversed in the like() function relative to the infix LIKE operator.*/
    decl->defaultReturnType = Field::Boolean;
    _SIG(like_2, argAnyTextOrNull, argAnyTextOrNull);
    _SIG(like_3, argAnyTextOrNull, argAnyTextOrNull, argAnyTextOrNull);
#endif
}

BuiltInFunctionDeclaration* BuiltInFunctions::value(const QString &name) const
{
    BuiltInFunctionDeclaration* f = QHash<QString, BuiltInFunctionDeclaration*>::value(name);
    if (!f) {
        f = m_aliases.value(name);
    }
    return f;
}

QStringList BuiltInFunctions::aliases() const
{
    return m_aliases.keys();
}

K_GLOBAL_STATIC(BuiltInFunctions, _builtInFunctions)

//=========================================

FunctionExpr::FunctionExpr(const QString& name_, NArgExpr* args_)
        : BaseExpr(0/*undefined*/)
        , name(name_.toUpper())
        , args(args_)
{
    if (_builtInAggregates->contains(name.toLatin1())) {
        m_cl = KexiDBExpr_Aggregation;
    }
    else {
        m_cl = KexiDBExpr_Function;
    }
    if (args)
        args->setParent(this);
}

FunctionExpr::FunctionExpr(const FunctionExpr& expr)
        : BaseExpr(0/*undefined*/)
        , name(expr.name)
        , args(expr.args ? expr.args->copy() : 0)
{
    if (args)
        args->setParent(this);
}

FunctionExpr::~FunctionExpr()
{
    delete args;
}

FunctionExpr* FunctionExpr::copy() const
{
    return new FunctionExpr(*this);
}

QString FunctionExpr::debugString()
{
    QString res;
    res.append(QString("FunctionExpr(") + name);
    if (args)
        res.append(QString(",") + args->debugString());
    res.append(QString(",type=%1)").arg(Driver::defaultSQLTypeName(type())));
    return res;
}

//static
QString FunctionExpr::toString(const QString &name, const Driver *driver, NArgExpr *args, QuerySchemaParameterValueListIterator* params)
{
    return name + '(' + (args ? args->toString(driver, params) : QString()) + ')';
}

// static
QString FunctionExpr::greatestOrLeastFunctionUsingCaseToString(const QString &name,
                                                               const Driver *driver,
                                                               NArgExpr *args,
                                                               QuerySchemaParameterValueListIterator* params)
{
    // (CASE WHEN (v0) IS NULL OR .. OR (vN) IS NULL THEN NULL ELSE F(v0,..,vN) END)
    if (args->args() >= 2) {
        QString whenSQL;
        whenSQL.reserve(256);
        foreach(BaseExpr* e, args->list) {
            if (!whenSQL.isEmpty())
                whenSQL += " OR ";
            whenSQL += QLatin1Char('(') + e->toString(driver, params) + QLatin1String(") IS NULL");
        }
        return QLatin1String("(CASE WHEN (") + whenSQL + QLatin1String(") THEN NULL ELSE (")
               + FunctionExpr::toString(name, driver, args, params) + QLatin1String(") END)");
    }
    return FunctionExpr::toString(name, driver, args, params);
}

static QByteArray maxString("MAX");
static QByteArray minString("MIN");
static QByteArray greatestString("GREATEST");
static QByteArray leastString("LEAST");
static QByteArray greatestOrLeastName(const QByteArray &name)
{
    if (name == maxString) {
        return greatestString;
    }
    if (name == minString) {
        return leastString;
    }
    return name;
}

QString FunctionExpr::toString(const Driver *driver, QuerySchemaParameterValueListIterator* params)
{
    if (name == QLatin1String("HEX")) {
        if (driver) {
            return driver->hexFunctionToString(args, params);
        }
    }
    else if (name == QLatin1String("IFNULL")) {
        if (driver) {
            return driver->ifnullFunctionToString(args, params);
        }
    }
    else if (name == QLatin1String("LENGTH")) {
        if (driver) {
            return driver->lengthFunctionToString(args, params);
        }
    }
    else if (name == QLatin1String("GREATEST") || name == QLatin1String("MAX")
             || name == QLatin1String("LEAST") || name == QLatin1String("MIN"))
    {
        if (driver) {
            return driver->greatestOrLeastFunctionToString(
                QString::fromLatin1(greatestOrLeastName(name.toLatin1())), args, params);
        }
        // else: don't change MIN/MAX
    }
    else if (name == QLatin1String("RANDOM")) {
        if (driver) {
            return driver->randomFunctionToString(args, params);
        }
    }
    else if (name == QLatin1String("CEILING") || name == QLatin1String("FLOOR")) {
        if (driver) {
            return driver->ceilingOrFloorFunctionToString(name, args, params);
        }
    }
    else if (name == QLatin1String("UNICODE")) {
        if (driver) {
            return driver->unicodeFunctionToString(args, params);
        }
    }
    return FunctionExpr::toString(name, driver, args, params);
}

void FunctionExpr::getQueryParameters(QuerySchemaParameterList& params)
{
    args->getQueryParameters(params);
}

Field::Type FunctionExpr::type()
{
    const BuiltInFunctionDeclaration *decl = _builtInFunctions->value(name);
    if (decl) {
        return decl->returnType(this, 0);
    }
    //! @todo
    return Field::InvalidType;
}

static void setIncorrectNumberOfArgumentsErrorMessage(ParseInfo *parseInfo, int count,
                                                      const std::vector<int> &argCounts,
                                                      const QString &name)
{
    parseInfo->errMsg = QString("Incorrect number of arguments (%1)").arg(count);
    const int maxArgCount = argCounts[argCounts.size() - 1];
    const int minArgCount = argCounts[0];
    QString firstSentence;
    if (count > maxArgCount) {
        firstSentence = QString("Too many arguments.%1").arg(" ");
    }
    if (count < minArgCount) {
        firstSentence = QString("Too few arguments.%1").arg(" ");
    }
    if (argCounts.size() == 1) {
        const int c = argCounts[0];
        if (c == 0) {
            parseInfo->errDescr = QString("%1%2() function does not accept any arguments.")
                                          .arg(firstSentence).arg(name);
        }
        else if (c == 1) {
            parseInfo->errDescr = QString("%1%2() function requires 1 argument.")
                                          .arg(firstSentence).arg(name);
        }
        else {
            parseInfo->errDescr = QString("%1%2() function requires %3 arguments.")
                                          .arg(firstSentence).arg(name).arg(c);
        }
    }
    else if (argCounts.size() == 2) {
        const int c1 = argCounts[0];
        const int c2 = argCounts[1];
        if (c2 == 1) {
            parseInfo->errDescr = QString("%1%2() function requires %3 or %4 argument.")
                                          .arg(firstSentence).arg(name).arg(c1).arg(c2);
        }
        else {
            parseInfo->errDescr = QString("%1%2() function requires %3 or %4 arguments.")
                                          .arg(firstSentence).arg(name).arg(c1).arg(c2);
        }
    }
    else {
        QString listCounts;
        for(std::vector<int>::const_iterator it(argCounts.begin()); it != argCounts.end(); ++it) {
            if (!listCounts.isEmpty()) {
                listCounts += QString(" or ");
            }
            listCounts += QString::number(*it);
        }
        parseInfo->errDescr = QString("%1%2() function requires %3 arguments.")
                                      .arg(firstSentence).arg(name).arg(listCounts);
    }
}

static void setIncorrectTypeOfArgumentsErrorMessage(ParseInfo *parseInfo, int argNum, Field::Type type,
                                                    int *argTypes, const QString &name)
{
    QString listTypes;
    int *argType = argTypes;
    while(*argType != Field::InvalidType) {
        if (!listTypes.isEmpty()) {
            listTypes += QString(" or ");
        }
        if (*argType <= Field::LastType || *argType == Field::Null) {
            listTypes += QString("\"%1\"").arg(Field::typeName(*argType));
        }
        else if (*argType == AnyText) {
            listTypes += QString("\"%1\"").arg(Field::typeName(Field::Text));
        }
        else if (*argType == AnyInt) {
            listTypes += QString("\"%1\"").arg(Field::typeName(Field::Integer));
        }
        else if (*argType == AnyFloat) {
            listTypes += QString("\"%1\"").arg(Field::typeGroupName(Field::FloatGroup)); // better than typeName() in this case
        }
        else if (*argType == AnyNumber) {
            listTypes += QString("\"Number\"");
        }
        else if (*argType == Any) {
            listTypes += QString("\"Any\"");
        }
        ++argType;
    }
    parseInfo->errMsg = "Incorrect type of argument";
    QString lastSentence = QString("Specified argument is of type \"%1\".").arg(Field::typeName(type));
    if (argNum == 0) {
        parseInfo->errDescr = QString("%1() function's first argument should be of type %2. %3")
                                      .arg(name).arg(listTypes).arg(lastSentence);
    }
    else if (argNum == 1) {
        parseInfo->errDescr = QString("%1() function's second argument should be of type %2. %3")
                                      .arg(name).arg(listTypes).arg(lastSentence);
    }
    else if (argNum == 2) {
        parseInfo->errDescr = QString("%1() function's third argument should be of type %2. %3")
                                      .arg(name).arg(listTypes).arg(lastSentence);
    }
    else if (argNum == 3) {
        parseInfo->errDescr = QString("%1() function's fourth argument should be of type %2. %3")
                                      .arg(name).arg(listTypes).arg(lastSentence);
    }
    else if (argNum == 4) {
        parseInfo->errDescr = QString("%1() function's fifth argument should be of type %2. %3")
                                      .arg(name).arg(listTypes).arg(lastSentence);
    }
    else {
        parseInfo->errDescr = QString("%1() function's %2 argument should be of type %3. %4")
                                      .arg(name).arg(argNum + 1).arg(listTypes).arg(lastSentence);
    }
}

static bool typeMatches(int argType, Field::Type actualType)
{
    if (argType == AnyText) {
        if (Field::isTextType(actualType)) {
            return true;
        }
    }
    else if (argType == AnyInt) {
        if (Field::isIntegerType(actualType)) {
            return true;
        }
    }
    else if (argType == AnyFloat) {
        if (Field::isFPNumericType(actualType)) {
            return true;
        }
    }
    else if (argType == AnyNumber) {
        if (Field::isNumericType(actualType)) {
            return true;
        }
    }
    else if (argType == Any) {
        return true;
    }
    else {
        if (argType == actualType) {
            return true;
        }
    }
    return false;
}

static int findMatchingType(int *argTypePtr, Field::Type actualType)
{
    for (; *argTypePtr != Field::InvalidType; ++argTypePtr) {
        if (typeMatches(*argTypePtr, actualType)) {
            break;
        }
    }
    return *argTypePtr;
}

bool FunctionExpr::validate(ParseInfo& parseInfo)
{
    if (!BaseExpr::validate(parseInfo)) {
        return false;
    }
    if (args->token() != ',') { // arguments required: NArgExpr with token ','
        return false;
    }
    if (args->args() > KEXIDB_MAX_FUNCTION_ARGS) {
        parseInfo.errMsg = QString("Maximum number of function arguments exceeded.");
        parseInfo.errDescr = QString("Too many arguments for function (%1). Maximum allowed is %2.")
                                     .arg(args->args()).arg(KEXIDB_MAX_FUNCTION_ARGS);
        return false;
    }
    if (!args->validate(parseInfo)) {
        return false;
    }
    const BuiltInFunctionDeclaration *decl = _builtInFunctions->value(name);
    if (!decl) {
        return false;
    }

    if (args->containsInvalidArgument()) {
        return false;
    }

    // Find matching signature
    const int count = args->args();
    bool properArgCount = false;
    std::vector<int> argCounts;
    int i = 0;
    argCounts.resize(decl->signatures.size());
    int **signature = 0;
    bool multipleArgs = false; // special case, e.g. for CHARS(v1, ... vN)
    for(std::vector<int**>::const_iterator it(decl->signatures.begin());
        it != decl->signatures.end(); ++it, ++i)
    {
        signature = *it;
        int **arg = signature;
        int expectedCount = 0;
        while(*arg && *arg != BuiltInFunctions::multipleArgs) {
            ++arg;
            ++expectedCount;
        }
        multipleArgs = *arg == BuiltInFunctions::multipleArgs;
        if (multipleArgs) {
            ++arg;
            const int minArgs = arg[0][0];
            properArgCount = count >= minArgs;
            if (!properArgCount) {
                parseInfo.errMsg = QString("Incorrect number of arguments (%1)").arg(count);
                if (minArgs == 1) {
                    parseInfo.errDescr = QString("Too few arguments. %1() function requires at least one argument.")
                                                  .arg(name);
                }
                else if (minArgs == 2) {
                    parseInfo.errDescr = QString("Too few arguments. %1() function requires at least two arguments.")
                                                  .arg(name);
                }
                else if (minArgs == 3) {
                    parseInfo.errDescr = QString("Too few arguments. %1() function requires at least three arguments.")
                                                  .arg(name);
                }
                else {
                    parseInfo.errDescr = QString("Too few arguments. %1() function requires at least %2 arguments.")
                                                  .arg(name).arg(minArgs);
                }
                return false;
            }
            break;
        }
        else if (count == expectedCount) { // arg # matches
            properArgCount = true;
            break;
        }
        else {
            argCounts[i] = expectedCount;
        }
    }
    if (!properArgCount) {
        std::unique(argCounts.begin(), argCounts.end());
        std::sort(argCounts.begin(), argCounts.end()); // sort so we can easier check the case
        setIncorrectNumberOfArgumentsErrorMessage(&parseInfo, count, argCounts, name);
        return false;
    }

    // Verify types
    if (multipleArgs) { // special signature: {typesForAllArgs, [multipleArgs-token], MIN, 0}
        int **arg = signature;
        int *typesForAllArgs = arg[0];
        for(int i = 0; i < count; ++i) {
            BaseExpr *expr = args->arg(i);
            const Field::Type exprType = expr->type(); // cache: evaluating type of expressions can be expensive
            const int matchingType = findMatchingType(typesForAllArgs, exprType);
            if (matchingType == Field::InvalidType) {
                setIncorrectTypeOfArgumentsErrorMessage(&parseInfo, i, exprType, typesForAllArgs, name);
                return false;
            }
        }
    }
    else { // typical signature: array of type-lists
        int **arg = signature;
        for(int i = 0; i < count; ++i, ++arg) {
            BaseExpr *expr = args->arg(i);
            const Field::Type exprType = expr->type(); // cache: evaluating type of expressions can be expensive
            const int matchingType = findMatchingType(arg[0], exprType);
            if (matchingType == Field::InvalidType) {
                setIncorrectTypeOfArgumentsErrorMessage(&parseInfo, i, exprType, arg[0], name);
                return false;
            }
        }
    }

    // Check type just now. If we checked earlier, possible error message would be less informative.
    if (decl->returnType(this, &parseInfo) == Field::InvalidType) {
        return false;
    }
    return true;
}

bool FunctionExpr::isBuiltInAggregate(const QByteArray& fname)
{
    return _builtInAggregates->contains(fname.toUpper());
}
