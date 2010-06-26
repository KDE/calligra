/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>
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

#include "Formula.h"

#include "CalculationSettings.h"
#include "Cell.h"
#include "CellStorage.h"
#include "Function.h"
#include "FunctionRepository.h"
#include "Sheet.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Region.h"
#include "Value.h"

#include "ValueCalc.h"
#include "ValueConverter.h"
#include "ValueParser.h"

#include <limits.h>

#include <QRegExp>
#include <QStack>
#include <QString>
#include <QTextStream>

#include <klocale.h>

#define KSPREAD_UNICODE_OPERATORS

/*
  To understand how this formula engine works, please refer to the documentation
  in file DESIGN.html.

  Useful references:
   - "Principles of Compiler Design", A.V.Aho, J.D.Ullman, Addison Wesley, 1978
   - "Writing Interactive Compilers and Interpreters", P.J. Brown,
     John Wiley and Sons, 1979.
   - "The Theory and Practice of Compiler Writing", J.Tremblay, P.G.Sorenson,
     McGraw-Hill, 1985.
   - "The Java(TM) Virtual Machine Specification", T.Lindholm, F.Yellin,
     Addison-Wesley, 1997.
   - "Java Virtual Machine", J.Meyer, T.Downing, O'Reilly, 1997.

 */


/*
TODO - features:
- handle Intersection
- cell reference is made relative (absolute now)
- shared formula (different owner, same data)
- relative internal representation (independent of owner)
- OASIS support
TODO - optimizations:
- handle initial formula marker = (and +)
- reuse constant already in the pool
- reuse references already in the pool
- expression optimization (e.g. 1+2+A1 becomes 3+A1)
*/

namespace KSpread
{

class Opcode
{
public:

    enum { Nop = 0, Load, Ref, Cell, Range, Function, Add, Sub, Neg, Mul, Div,
           Pow, Concat, Intersect, Not, Equal, Less, Greater, Array, Union
         };

    unsigned type;
    unsigned index;

    Opcode(): type(Nop), index(0) {}
    Opcode(unsigned t): type(t), index(0) {}
    Opcode(unsigned t, unsigned i): type(t), index(i) {}
};

class Formula::Private : public QSharedData
{
public:
    Cell cell;
    Sheet *sheet;
    mutable bool dirty;
    mutable bool valid;
    QString expression;
    mutable QVector<Opcode> codes;
    mutable QVector<Value> constants;

    Value valueOrElement(const Map* map, int index, FuncExtra &fe, const Value &v) const;
};

class TokenStack : public QVector<Token>
{
public:
    TokenStack();
    bool isEmpty() const;
    unsigned itemCount() const;
    void push(const Token& token);
    Token pop();
    const Token& top();
    const Token& top(unsigned index);
private:
    void ensureSpace();
    unsigned topIndex;
};

}

using namespace KSpread;

// for null token
const Token Token::null;

// helper function: return operator of given token text
// e.g. '*' yields Operator::Asterisk, and so on
Token::Op KSpread::matchOperator(const QString& text)
{
    Token::Op result = Token::InvalidOp;

    if (text.length() == 1) {
        QChar p = text[0];
        switch (p.unicode()) {
        case '+': result = Token::Plus; break;
        case '-': result = Token::Minus; break;
        case '*': result = Token::Asterisk; break;
        case '/': result = Token::Slash; break;
        case '^': result = Token::Caret; break;
        case ',': result = Token::Comma; break;
        case ';': result = Token::Semicolon; break;
        case ' ': result = Token::Intersect; break;
        case '(': result = Token::LeftPar; break;
        case ')': result = Token::RightPar; break;
        case '&': result = Token::Ampersand; break;
        case '=': result = Token::Equal; break;
        case '<': result = Token::Less; break;
        case '>': result = Token::Greater; break;
        case '%': result = Token::Percent; break;
        case '~': result = Token::Union; break;
#ifdef KSPREAD_INLINE_ARRAYS
        case '{': result = Token::CurlyBra; break;
        case '}': result = Token::CurlyKet; break;
        case '|': result = Token::Pipe; break;
#endif
#ifdef KSPREAD_UNICODE_OPERATORS
        case 0x2212: result = Token::Minus; break;
        case 0x00D7: result = Token::Asterisk; break;
        case 0x00F7: result = Token::Slash; break;
        case 0x2215: result = Token::Slash; break;
#endif
        default : result = Token::InvalidOp; break;
        }
    }

    if (text.length() == 2) {
        if (text == "<>") result = Token::NotEqual;
        if (text == "!=") result = Token::NotEqual;
        if (text == "<=") result = Token::LessEqual;
        if (text == ">=") result = Token::GreaterEqual;
        if (text == "==") result = Token::Equal;
    }

    return result;
}

// helper function: give operator precedence
// e.g. '+' is 1 while '*' is 3
static int opPrecedence(Token::Op op)
{
    int prec = -1;
    switch (op) {
    case Token::Percent      : prec = 8; break;
    case Token::Caret        : prec = 7; break;
    case Token::Asterisk     : prec = 5; break;
    case Token::Slash        : prec = 6; break;
    case Token::Plus         : prec = 3; break;
    case Token::Minus        : prec = 3; break;
    case Token::Union        : prec = 2; break;
    case Token::Ampersand    : prec = 2; break;
    case Token::Intersect    : prec = 2; break;
    case Token::Equal        : prec = 1; break;
    case Token::NotEqual     : prec = 1; break;
    case Token::Less         : prec = 1; break;
    case Token::Greater      : prec = 1; break;
    case Token::LessEqual    : prec = 1; break;
    case Token::GreaterEqual : prec = 1; break;
#ifdef KSPREAD_INLINE_ARRAYS
        // FIXME Stefan: I don't know whether zero is right for this case. :-(
    case Token::CurlyBra     : prec = 0; break;
    case Token::CurlyKet     : prec = 0; break;
    case Token::Pipe         : prec = 0; break;
#endif
    case Token::Semicolon    : prec = 0; break;
    case Token::RightPar     : prec = 0; break;
    case Token::LeftPar      : prec = -1; break;
    default: prec = -1; break;
    }
    return prec;
}

// helper function
static Value tokenAsValue(const Token& token)
{
    Value value;
    if (token.isBoolean()) value = Value(token.asBoolean());
    else if (token.isInteger()) value = Value(token.asInteger());
    else if (token.isFloat()) value = Value(token.asFloat());
    else if (token.isString()) value = Value(token.asString());
    else if (token.isError()) {
        const QString error = token.asError();
        if (error == Value::errorCIRCLE().errorMessage())
            value = Value::errorCIRCLE();
        else if (error == Value::errorDEPEND().errorMessage())
            value = Value::errorDEPEND();
        else if (error == Value::errorDIV0().errorMessage())
            value = Value::errorDIV0();
        else if (error == Value::errorNA().errorMessage())
            value = Value::errorNA();
        else if (error == Value::errorNAME().errorMessage())
            value = Value::errorNAME();
        else if (error == Value::errorNUM().errorMessage())
            value = Value::errorNUM();
        else if (error == Value::errorNULL().errorMessage())
            value = Value::errorNULL();
        else if (error == Value::errorPARSE().errorMessage())
            value = Value::errorPARSE();
        else if (error == Value::errorREF().errorMessage())
            value = Value::errorREF();
        else if (error == Value::errorVALUE().errorMessage())
            value = Value::errorVALUE();
        else {
            value = Value(Value::Error);
            value.setError(error);
        }
    }
    return value;
}

/**********************
    Token
 **********************/

// creates a token
Token::Token(Type type, const QString& text, int pos)
{
    m_type = type;
    m_text = text;
    m_pos = pos;
}

// copy constructor
Token::Token(const Token& token)
{
    m_type = token.m_type;
    m_text = token.m_text;
    m_pos = token.m_pos;
}

// assignment operator
Token& Token::operator=(const Token & token)
{
    m_type = token.m_type;
    m_text = token.m_text;
    m_pos = token.m_pos;
    return *this;
}

bool Token::asBoolean() const
{
    if (!isBoolean()) return false;
    return m_text.toLower() == "true";
    // FIXME check also for i18n version
}

qint64 Token::asInteger() const
{
    if (isInteger()) return m_text.toLongLong();
    else return 0;
}

double Token::asFloat() const
{
    if (isFloat()) return m_text.toDouble();
    else return 0.0;
}

QString Token::asString() const
{
    if (isString()) return m_text.mid(1, m_text.length() - 2);
    else return QString();
}

QString Token::asError() const
{
    if (isError())
        return m_text;
    else
        return QString();
}

Token::Op Token::asOperator() const
{
    if (isOperator()) return matchOperator(m_text);
    else return InvalidOp;
}

QString Token::sheetName() const
{
    if (!isCell() && !isRange()) return QString();
    int i = m_text.indexOf('!');
    if (i < 0) return QString();
    QString sheet = m_text.left(i);
    return sheet;
}

QString Token::description() const
{
    QString desc;

    switch (m_type) {
    case  Boolean:    desc = "Boolean"; break;
    case  Integer:    desc = "Integer"; break;
    case  Float:      desc = "Float"; break;
    case  String:     desc = "String"; break;
    case  Identifier: desc = "Identifier"; break;
    case  Cell:       desc = "Cell"; break;
    case  Range:      desc = "Range"; break;
    case  Operator:   desc = "Operator"; break;
    case  Error:      desc = "Error"; break;
    default:          desc = "Unknown"; break;
    }

    while (desc.length() < 10) desc.prepend(' ');
    desc.prepend("  ");
    desc.prepend(QString::number(m_pos));
    desc.append(" : ").append(m_text);

    return desc;
}


/**********************
    TokenStack
 **********************/

TokenStack::TokenStack(): QVector<Token>()
{
    topIndex = 0;
    ensureSpace();
}

bool TokenStack::isEmpty() const
{
    return topIndex == 0;
}

unsigned TokenStack::itemCount() const
{
    return topIndex;
}

void TokenStack::push(const Token& token)
{
    ensureSpace();
    insert(topIndex++, token);
}

Token TokenStack::pop()
{
    return (topIndex > 0) ? Token(at(--topIndex)) : Token();
}

const Token& TokenStack::top()
{
    return top(0);
}

const Token& TokenStack::top(unsigned index)
{
    if (topIndex > index)
        return at(topIndex -index - 1);
    return Token::null;
}

void TokenStack::ensureSpace()
{
    while ((int) topIndex >= size())
        resize(size() + 10);
}

/**********************
    FormulaPrivate
 **********************/

// helper function: return true for valid identifier character
bool KSpread::isIdentifier(QChar ch)
{
    return (ch.unicode() == '_') || (ch.unicode() == '$') || (ch.unicode() == '.') || (ch.isLetter());
}




/**********************
    Formula
 **********************/

// Constructor

Formula::Formula(Sheet *sheet, const Cell& cell)
        : d(new Private)
{
    d->cell = cell;
    d->sheet = sheet;
    clear();
}

Formula::Formula(Sheet *sheet)
        : d(new Private)
{
    d->cell = Cell();
    d->sheet = sheet;
    clear();
}

Formula::Formula()
        : d(new Private)
{
    d->cell = Cell();
    d->sheet = 0;
    clear();
}

Formula::Formula(const Formula& other)
        : d(other.d)
{
}

// Destructor

Formula::~Formula()
{
}

const Cell& Formula::cell() const
{
    return d->cell;
}

Sheet* Formula::sheet() const
{
    return d->sheet;
}

// Sets a new expression for this formula.
// note that both the real lex and parse processes will happen later on
// when needed (i.e. "lazy parse"), for example during formula evaluation.

void Formula::setExpression(const QString& expr)
{
    d->expression = expr;
    d->dirty = true;
    d->valid = false;
}

// Returns the expression associated with this formula.

QString Formula::expression() const
{
    return d->expression;
}

// Returns the validity of the formula.
// note: empty formula is always invalid.

bool Formula::isValid() const
{
    if (d->dirty) {
        KLocale* locale = !d->cell.isNull() ? d->cell.locale() : 0;
        if ((!locale) && d->sheet)
            locale = d->sheet->map()->calculationSettings()->locale();
        Tokens tokens = scan(d->expression, locale);
        if (tokens.valid())
            compile(tokens);
        else
            d->valid = false;
    }
    return d->valid;
}

// Clears everything, also mark the formula as invalid.

void Formula::clear()
{
    d->expression.clear();
    d->dirty = true;
    d->valid = false;
    d->constants.clear();
    d->codes.clear();
}

// Returns list of token for the expression.
// this triggers again the lexical analysis step. it is however preferable
// (even when there's small performance penalty) because otherwise we need to
// store parsed tokens all the time which serves no good purpose.

Tokens Formula::tokens() const
{
    KLocale* locale = !d->cell.isNull() ? d->cell.locale() : 0;
    if ((!locale) && d->sheet)
        locale = d->sheet->map()->calculationSettings()->locale();
    return scan(d->expression, locale);
}

Tokens Formula::scan(const QString& expr, const KLocale* locale) const
{
    // to hold the result
    Tokens tokens;

    // parsing state
    enum { Start, Finish, InNumber, InDecimal, InExpIndicator, InExponent,
           InString, InIdentifier, InCell, InRange, InSheetOrAreaName, InError
         } state;

    // use locale settings if specified
    QString thousand = locale ? locale->thousandsSeparator() : "";
    QString decimal = locale ? locale->decimalSymbol() : ".";

    // initialize variables
    state = Start;
    bool parseError = false;
    int i = 0;
    QString ex = expr;
    QString tokenText;
    int tokenStart = 0;

    // first character must be equal sign (=)
    if (ex[0] != '=')
        return tokens;

    // but the scanner should not see this equal sign
    ex.remove(0, 1);

    // force a terminator
    ex.append(QChar());

    // main loop
    while ((state != Finish) && (i < ex.length())) {
        QChar ch = ex[i];

        switch (state) {

        case Start:

            tokenStart = i;

            // Whitespaces can be used as intersect-operator for two arrays.
            if (ch.isSpace()) { //TODO && (tokens.isEmpty() || !tokens.last().isRange())) {
                i++;
                break;
            }

            // check for number
            if (ch.isDigit()) {
                state = InNumber;
            }

            // a string ?
            else if (ch == '"') {
                tokenText.append(ex[i++]);
                state = InString;
            }

            // decimal dot ?
            else if (ch == decimal[0]) {
                tokenText.append(ex[i++]);
                state = InDecimal;
            }

            // beginning with alphanumeric ?
            // could be identifier, cell, range, or function...
            else if (isIdentifier(ch)) {
                state = InIdentifier;
            }

            // aposthrophe (') marks sheet name for 3-d cell, e.g 'Sales Q3'!A4, or a named range
            else if (ch.unicode() == '\'') {
                i++;
                state = InSheetOrAreaName;
            }

            // error value?
            else if (ch == '#') {
                tokenText.append(ex[i++]);
                state = InError;
            }

            // terminator character
            else if (ch == QChar::Null)
                state = Finish;

            // look for operator match
            else {
                int op;
                QString s;

                // check for two-chars operator, such as '<=', '>=', etc
                s.append(ch).append(ex[i+1]);
                op = matchOperator(s);

                // check for one-char operator, such as '+', ';', etc
                if (op == Token::InvalidOp) {
                    s = QString(ch);
                    op = matchOperator(s);
                }

                // any matched operator ?
                if (op != Token::InvalidOp) {
                    int len = s.length();
                    i += len;
                    tokens.append(Token(Token::Operator, s.left(len), tokenStart));
                } else {
                    // not matched an operator, add an Unknown token and remember whe had a parse error
                    parseError = true;
                    tokens.append(Token(Token::Unknown, s.left(1), tokenStart));
                    i++;
                }
            }
            break;

        case InIdentifier:

            // consume as long as alpha, dollar sign, underscore, or digit
            if (isIdentifier(ch)  || ch.isDigit()) tokenText.append(ex[i++]);

            // a '!' ? then this must be sheet name, e.g "Sheet4!", unless the next character is '='
            else if (ch == '!' && ex[i+1] != '=') {
                tokenText.append(ex[i++]);
                state = InCell;
            }

            // a '(' ? then this must be a function identifier
            else if (ch == '(') {
                tokens.append(Token(Token::Identifier, tokenText, tokenStart));
                tokenStart = i;
                tokenText.clear();
                state = Start;
            }

            // we're done with identifier
            else {
                // check for cell reference,  e.g A1, VV123, ...
                QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$");
                int n = exp.indexIn(tokenText);
                if (n >= 0)
                    state = InCell;
                else {
                    if (isNamedArea(tokenText))
                        tokens.append(Token(Token::Range, tokenText, tokenStart));
                    else
                        tokens.append(Token(Token::Identifier, tokenText, tokenStart));
                    tokenStart = i;
                    tokenText.clear();
                    state = Start;
                }
            }
            break;

        case InCell:

            // consume as long as alpha, dollar sign, underscore, or digit
            if (isIdentifier(ch)  || ch.isDigit()) tokenText.append(ex[i++]);

            // we're done with cell ref, possibly with sheet name (like "Sheet2!B2")
            // note that "Sheet2!TotalSales" is also possible, in which "TotalSales" is a named area
            else {

                // check if it's a cell ref like A32, not named area
                QString cell;
                for (int j = tokenText.length() - 1; j >= 0; j--)
                    if (tokenText[j] == '!')
                        break;
                    else
                        cell.prepend(tokenText[j]);
                QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$");
                if (exp.indexIn(cell) != 0) {
                    // regexp failed, means we have something like "Sheet2!TotalSales"
                    // and not "Sheet2!A2"
                    // thus, assume so far that it's a named area
                    tokens.append(Token(Token::Range, tokenText, tokenStart));
                    tokenText.clear();
                    state = Start;
                }

                else {

                    // so up to now we've got something like A2 or Sheet2!F4
                    // check for range reference
                    if (ch == ':') {
                        tokenText.append(ex[i++]);
                        state = InRange;
                    } else {
                        // we're done with cell reference
                        tokens.append(Token(Token::Cell, tokenText, tokenStart));
                        tokenText.clear();
                        state = Start;
                    }
                }
            }
            break;

        case InRange:

            // consume as long as alpha, dollar sign, underscore, or digit or !
            if (isIdentifier(ch) || ch.isDigit() || ch == '!') tokenText.append(ex[i++]);

            // we're done with range reference
            else {
                tokens.append(Token(Token::Range, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            }
            break;

        case InSheetOrAreaName:

            // consume until '
            if (ch.unicode() != '\'') tokenText.append(ex[i++]);

            else {
                // eat the aposthrophe itself
                ++i;
                // must be followed by '!' to be sheet name
                if (ex[i] == '!') {
                    tokenText.append(ex[i++]);
                    state = InCell;
                } else {
                    if (isNamedArea(tokenText))
                        tokens.append(Token(Token::Range, tokenText, tokenStart));
                    else {
                        // for compatibility with oocalc (and the openformula spec), don't parse single-quoted
                        // text as an identifier, instead add an Unknown token and remember we had an error
                        parseError = true;
                        tokens.append(Token(Token::Unknown, '\'' + tokenText + '\'', tokenStart));
                    }
                    tokenStart = i;
                    tokenText.clear();
                    state = Start;
                }
            }
            break;

        case InNumber:

            // consume as long as it's digit
            if (ch.isDigit()) tokenText.append(ex[i++]);

            // skip thousand separator
            else if (!thousand.isEmpty() && (ch == thousand[0])) i++;

            // convert decimal separator to '.', also support '.' directly
            // we always support '.' because of bug #98455
            else if ((!decimal.isEmpty() && (ch == decimal[0])) || (ch == '.')) {
                tokenText.append('.');
                i++;
                state = InDecimal;
            }

            // exponent ?
            else if (ch.toUpper() == 'E') {
                tokenText.append('E');
                i++;
                state = InExpIndicator;
            }

            // reference sheet delimiter?
            else if (ch == '!') {
                tokenText.append(ex[i++]);
                state = InCell;
            }

            // identifier?
            else if (isIdentifier(ch)) {
                // has to be a sheet or area name then
                state = InIdentifier;
            }

            // we're done with integer number
            else {
                tokens.append(Token(Token::Integer, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            };
            break;

        case InDecimal:

            // consume as long as it's digit
            if (ch.isDigit()) tokenText.append(ex[i++]);

            // exponent ?
            else if (ch.toUpper() == 'E') {
                tokenText.append('E');
                i++;
                state = InExpIndicator;
            }

            // we're done with floating-point number
            else {
                tokens.append(Token(Token::Float, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            };
            break;

        case InExpIndicator:

            // possible + or - right after E, e.g 1.23E+12 or 4.67E-8
            if ((ch == '+') || (ch == '-')) tokenText.append(ex[i++]);

            // consume as long as it's digit
            else if (ch.isDigit()) state = InExponent;

            // invalid thing here
            else {
                parseError = true;
                tokenText.append(ex[i++]);
                tokens.append(Token(Token::Unknown, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            }

            break;

        case InExponent:

            // consume as long as it's digit
            if (ch.isDigit()) tokenText.append(ex[i++]);

            // we're done with floating-point number
            else {
                tokens.append(Token(Token::Float, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            };
            break;

        case InString:

            // consume until "
            if (ch != '"') tokenText.append(ex[i++]);

            else {
                tokenText.append(ch); i++;
                tokens.append(Token(Token::String, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            }
            break;

        case InError:

            // consume until !
            if (ch != '!')
                tokenText.append(ex[i++]);
            else {
                tokenText.append(ex[i++]);
                tokens.append(Token(Token::Error, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            }
            break;

        default:
            break;
        };
    };

    // parse error if any text remains
    if (state != Finish) {
        tokens.append(Token(Token::Unknown, ex.mid(tokenStart, ex.length() - tokenStart - 1), tokenStart));
        parseError = true;
    }

    if (parseError)
        tokens.setValid(false);

    return tokens;
}

// will affect: dirty, valid, codes, constants
void Formula::compile(const Tokens& tokens) const
{
    // initialize variables
    d->dirty = false;
    d->valid = false;
    d->codes.clear();
    d->constants.clear();

    // sanity check
    if (tokens.count() == 0) return;

    TokenStack syntaxStack;
    QStack<int> argStack;
    unsigned argCount = 1;

    for (int i = 0; i <= tokens.count(); i++) {
        // helper token: InvalidOp is end-of-formula
        Token token = (i < tokens.count()) ? tokens[i] : Token(Token::Operator);
        Token::Type tokenType = token.type();

        // unknown token is invalid
        if (tokenType == Token::Unknown) break;

        // are we entering a function ?
        // if stack already has: id (
        if (syntaxStack.itemCount() >= 2) {
            Token par = syntaxStack.top();
            Token id = syntaxStack.top(1);
            if (par.asOperator() == Token::LeftPar)
                if (id.isIdentifier()) {
                    argStack.push(argCount);
                    argCount = 1;
                }
        }

#ifdef KSPREAD_INLINE_ARRAYS
        // are we entering an inline array ?
        // if stack already has: {
        if (syntaxStack.itemCount() >= 1) {
            Token bra = syntaxStack.top();
            if (bra.asOperator() == Token::CurlyBra) {
                argStack.push(argCount);
                argStack.push(1);   // row count
                argCount = 1;
            }
        }
#endif

        // for constants, push immediately to stack
        // generate code to load from a constant
        if ((tokenType == Token::Integer) || (tokenType == Token::Float) ||
                (tokenType == Token::String) || (tokenType == Token::Boolean) ||
                (tokenType == Token::Error)) {
            syntaxStack.push(token);
            d->constants.append(tokenAsValue(token));
            d->codes.append(Opcode(Opcode::Load, d->constants.count() - 1));
        }

        // for cell, range, or identifier, push immediately to stack
        // generate code to load from reference
        if ((tokenType == Token::Cell) || (tokenType == Token::Range) ||
                (tokenType == Token::Identifier)) {
            syntaxStack.push(token);
            d->constants.append(Value(token.text()));
            if (tokenType == Token::Cell)
                d->codes.append(Opcode(Opcode::Cell, d->constants.count() - 1));
            else if (tokenType == Token::Range)
                d->codes.append(Opcode(Opcode::Range, d->constants.count() - 1));
            else
                d->codes.append(Opcode(Opcode::Ref, d->constants.count() - 1));
        }

        // special case for percentage
        if (tokenType == Token::Operator)
            if (token.asOperator() == Token::Percent)
                if (syntaxStack.itemCount() >= 1)
                    if (!syntaxStack.top().isOperator()) {
                        d->constants.append(Value(0.01));
                        d->codes.append(Opcode(Opcode::Load, d->constants.count() - 1));
                        d->codes.append(Opcode(Opcode::Mul));
                    }

        // for any other operator, try to apply all parsing rules
        if (tokenType == Token::Operator)
            if (token.asOperator() != Token::Percent) {
                // repeat until no more rule applies
                for (; ;) {
                    bool ruleFound = false;

                    // rule for function arguments, if token is ; or )
                    // id ( arg1 ; arg2 -> id ( arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 5)
                            if ((token.asOperator() == Token::RightPar) ||
                                    (token.asOperator() == Token::Semicolon)) {
                                Token arg2 = syntaxStack.top();
                                Token sep = syntaxStack.top(1);
                                Token arg1 = syntaxStack.top(2);
                                Token par = syntaxStack.top(3);
                                Token id = syntaxStack.top(4);
                                if (!arg2.isOperator())
                                    if (sep.asOperator() == Token::Semicolon)
                                        if (!arg1.isOperator())
                                            if (par.asOperator() == Token::LeftPar)
                                                if (id.isIdentifier()) {
                                                    ruleFound = true;
                                                    syntaxStack.pop();
                                                    syntaxStack.pop();
                                                    argCount++;
                                                }
                            }

                    // rule for empty function arguments, if token is ; or )
                    // id ( arg ; -> id ( arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3)
                            if ((token.asOperator() == Token::RightPar) ||
                                    (token.asOperator() == Token::Semicolon)) {
                                Token sep = syntaxStack.top();
                                Token arg = syntaxStack.top(1);
                                Token par = syntaxStack.top(2);
                                Token id = syntaxStack.top(3);
                                if (sep.asOperator() == Token::Semicolon)
                                    if (!arg.isOperator())
                                        if (par.asOperator() == Token::LeftPar)
                                            if (id.isIdentifier()) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                d->constants.append(Value());
                                                d->codes.append(Opcode(Opcode::Load, d->constants.count() - 1));
                                                argCount++;
                                            }
                            }

                    // rule for function last argument:
                    //  id ( arg ) -> arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 4) {
                            Token par2 = syntaxStack.top();
                            Token arg = syntaxStack.top(1);
                            Token par1 = syntaxStack.top(2);
                            Token id = syntaxStack.top(3);
                            if (par2.asOperator() == Token::RightPar)
                                if (!arg.isOperator())
                                    if (par1.asOperator() == Token::LeftPar)
                                        if (id.isIdentifier()) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(arg);
                                            d->codes.append(Opcode(Opcode::Function, argCount));
                                            Q_ASSERT(!argStack.empty());
                                            argCount = argStack.empty() ? 0 : argStack.pop();
                                        }
                        }

                    // rule for function call with parentheses, but without argument
                    // e.g. "2*PI()"
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            Token par2 = syntaxStack.top();
                            Token par1 = syntaxStack.top(1);
                            Token id = syntaxStack.top(2);
                            if (par2.asOperator() == Token::RightPar)
                                if (par1.asOperator() == Token::LeftPar)
                                    if (id.isIdentifier()) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(Token(Token::Integer));
                                        d->codes.append(Opcode(Opcode::Function, 0));
                                        Q_ASSERT(!argStack.empty());
                                        argCount = argStack.empty() ? 0 : argStack.pop();
                                    }
                        }

#ifdef KSPREAD_INLINE_ARRAYS
                    // rule for inline array elements, if token is ; or | or }
                    // { arg1 ; arg2 -> { arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 4)
                            if ((token.asOperator() == Token::Semicolon) ||
                                    (token.asOperator() == Token::CurlyKet) ||
                                    (token.asOperator() == Token::Pipe)) {
                                Token arg2 = syntaxStack.top();
                                Token sep = syntaxStack.top(1);
                                Token arg1 = syntaxStack.top(2);
                                Token bra = syntaxStack.top(3);
                                if (!arg2.isOperator())
                                    if (sep.asOperator() == Token::Semicolon)
                                        if (!arg1.isOperator())
                                            if (bra.asOperator() == Token::CurlyBra) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                argCount++;
                                            }
                            }

                    // rule for last array row element, if token is ; or | or }
                    //  { arg1 | arg2 -> { arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 4)
                            if ((token.asOperator() == Token::Semicolon) ||
                                    (token.asOperator() == Token::CurlyKet) ||
                                    (token.asOperator() == Token::Pipe)) {
                                Token arg2 = syntaxStack.top();
                                Token sep = syntaxStack.top(1);
                                Token arg1 = syntaxStack.top(2);
                                Token bra = syntaxStack.top(3);
                                if (!arg2.isOperator())
                                    if (sep.asOperator() == Token::Pipe)
                                        if (!arg1.isOperator())
                                            if (bra.asOperator() == Token::CurlyBra) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                int rowCount = argStack.pop();
                                                argStack.push(++rowCount);
                                                argCount = 1;
                                            }
                            }

                    // rule for last array element:
                    //  { arg } -> arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            Token ket = syntaxStack.top();
                            Token arg = syntaxStack.top(1);
                            Token bra = syntaxStack.top(2);
                            if (ket.asOperator() == Token::CurlyKet)
                                if (!arg.isOperator())
                                    if (bra.asOperator() == Token::CurlyBra) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(arg);
                                        const int rowCount = argStack.pop();
                                        d->constants.append(Value((int)argCount));     // cols
                                        d->constants.append(Value(rowCount));
                                        d->codes.append(Opcode(Opcode::Array, d->constants.count() - 2));
                                        Q_ASSERT(!argStack.empty());
                                        argCount = argStack.empty() ? 0 : argStack.pop();
                                    }
                        }
#endif
                    // rule for parenthesis:  ( Y ) -> Y
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            Token right = syntaxStack.top();
                            Token y = syntaxStack.top(1);
                            Token left = syntaxStack.top(2);
                            if (right.isOperator())
                                if (!y.isOperator())
                                    if (left.isOperator())
                                        if (right.asOperator() == Token::RightPar)
                                            if (left.asOperator() == Token::LeftPar) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.push(y);
                                            }
                        }

                    // rule for binary operator:  A (op) B -> A
                    // conditions: precedence of op >= precedence of token
                    // action: push (op) to result
                    // e.g. "A * B" becomes 'A' if token is operator '+'
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            Token b = syntaxStack.top();
                            Token op = syntaxStack.top(1);
                            Token a = syntaxStack.top(2);
                            if (!a.isOperator())
                                if (!b.isOperator())
                                    if (op.isOperator())
                                        if (token.asOperator() != Token::LeftPar)
                                            if (opPrecedence(op.asOperator()) >= opPrecedence(token.asOperator())) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.push(b);
                                                switch (op.asOperator()) {
                                                    // simple binary operations
                                                case Token::Plus:         d->codes.append(Opcode::Add); break;
                                                case Token::Minus:        d->codes.append(Opcode::Sub); break;
                                                case Token::Asterisk:     d->codes.append(Opcode::Mul); break;
                                                case Token::Slash:        d->codes.append(Opcode::Div); break;
                                                case Token::Caret:        d->codes.append(Opcode::Pow); break;
                                                case Token::Ampersand:    d->codes.append(Opcode::Concat); break;
                                                case Token::Intersect:    d->codes.append(Opcode::Intersect); break;
                                                case Token::Union:        d->codes.append(Opcode::Union); break;

                                                    // simple value comparisons
                                                case Token::Equal:        d->codes.append(Opcode::Equal); break;
                                                case Token::Less:         d->codes.append(Opcode::Less); break;
                                                case Token::Greater:      d->codes.append(Opcode::Greater); break;

                                                    // NotEqual is Equal, followed by Not
                                                case Token::NotEqual:
                                                    d->codes.append(Opcode::Equal);
                                                    d->codes.append(Opcode::Not);
                                                    break;

                                                    // LessOrEqual is Greater, followed by Not
                                                case Token::LessEqual:
                                                    d->codes.append(Opcode::Greater);
                                                    d->codes.append(Opcode::Not);
                                                    break;

                                                    // GreaterOrEqual is Less, followed by Not
                                                case Token::GreaterEqual:
                                                    d->codes.append(Opcode::Less);
                                                    d->codes.append(Opcode::Not);
                                                    break;
                                                default: break;
                                                };
                                            }
                        }

                    // rule for unary operator:  (op1) (op2) X -> (op1) X
                    // conditions: op2 is unary, token is not '('
                    // action: push (op2) to result
                    // e.g.  "* - 2" becomes '*'
                    if (!ruleFound)
                        if (token.asOperator() != Token::LeftPar)
                            if (syntaxStack.itemCount() >= 3) {
                                Token x = syntaxStack.top();
                                Token op2 = syntaxStack.top(1);
                                Token op1 = syntaxStack.top(2);
                                if (!x.isOperator())
                                    if (op1.isOperator())
                                        if (op2.isOperator())
                                            if ((op2.asOperator() == Token::Plus) ||
                                                    (op2.asOperator() == Token::Minus)) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.push(x);
                                                if (op2.asOperator() == Token::Minus)
                                                    d->codes.append(Opcode(Opcode::Neg));
                                            }
                            }

                    // auxiliary rule for unary operator:  (op) X -> X
                    // conditions: op is unary, op is first in syntax stack, token is not '('
                    // action: push (op) to result
                    if (!ruleFound)
                        if (token.asOperator() != Token::LeftPar)
                            if (syntaxStack.itemCount() == 2) {
                                Token x = syntaxStack.top();
                                Token op = syntaxStack.top(1);
                                if (!x.isOperator())
                                    if (op.isOperator())
                                        if ((op.asOperator() == Token::Plus) ||
                                                (op.asOperator() == Token::Minus)) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(x);
                                            if (op.asOperator() == Token::Minus)
                                                d->codes.append(Opcode(Opcode::Neg));
                                        }
                            }

                    if (!ruleFound) break;
                }

                // can't apply rules anymore, push the token
                if (token.asOperator() != Token::Percent)
                    syntaxStack.push(token);
            }
    }

    // syntaxStack must left only one operand and end-of-formula (i.e. InvalidOp)
    d->valid = false;
    if (syntaxStack.itemCount() == 2)
        if (syntaxStack.top().isOperator())
            if (syntaxStack.top().asOperator() == Token::InvalidOp)
                if (!syntaxStack.top(1).isOperator())
                    d->valid = true;

    // bad parsing ? clean-up everything
    if (!d->valid) {
        d->constants.clear();
        d->codes.clear();
    }
}

bool Formula::isNamedArea(const QString& expr) const
{
    return d->sheet ? d->sheet->map()->namedAreaManager()->contains(expr) : false;
}


// Evaluates the formula, returns the result.

struct stackEntry {
    void reset() {
        row1 = col1 = row2 = col2 = -1;
        reg = KSpread::Region();
    }
    Value val;
    KSpread::Region reg;
    int row1, col1, row2, col2;
};

// evaluate the cellIndirections
Value Formula::eval(CellIndirection cellIndirections) const
{
    QHash<Cell, Value> values;
    return evalRecursive(cellIndirections, values);
}

// We need to unroll arrays. Do use the same logic to unroll like OpenOffice.org and Excel are using.
Value Formula::Private::valueOrElement(const Map* map, int index, FuncExtra &fe, const Value &v) const
{
    if(v.isArray()) {
        if(v.count() == 1) // if there is only one item, use that one
            return v.element(0);
        QString c = constants[index].asString();
        const Region region(c, map, sheet);
        if(region.isValid()) {
            const QPoint position = region.firstRange().topLeft();
            const int idx = fe.myrow - position.y(); // do we need to do the same for columns?
            if(idx >= 0 && idx < int(v.count()))
                return v.element(idx); // within the range returns the selected element
        }
    }
    return v;
}

// On OO.org Calc and MS Excel operations done with +, -, * and / do fail if one of the values is
// non-numeric. This differs from formulas like SUM which just ignores non numeric values.
Value numericOrError(const ValueConverter* converter, const Value &v)
{
    switch (v.type()) {
        case Value::Empty:
        case Value::Boolean:
        case Value::Integer:
        case Value::Float:
        case Value::Complex:
        case Value::Error:
            return v;
        case Value::String: {
            if( v.asString().isEmpty() )
                return v;
            bool ok;
            converter->asNumeric(v, &ok);
            if(ok)
                return v;
        } break;
        case Value::Array:
        case Value::CellRange:
            return v;
    }
    return Value::errorVALUE();
}

Value Formula::evalRecursive(CellIndirection cellIndirections, QHash<Cell, Value>& values) const
{
    QStack<stackEntry> stack;
    stackEntry entry;
    int index;
    Value val1, val2;
    QString c;
    QVector<Value> args;

    const Map* map = d->sheet ? d->sheet->map() : new Map(0 /*document*/);
    const ValueConverter* converter = map->converter();
    ValueCalc* calc = map->calc();

    QSharedPointer<Function> function;
    FuncExtra fe;
    fe.mycol = fe.myrow = 0;
    if (!d->cell.isNull()) {
        fe.mycol = d->cell.column();
        fe.myrow = d->cell.row();
    }

    if (d->dirty) {
        Tokens tokens = scan(d->expression);
        d->valid = tokens.valid();
        if (tokens.valid())
            compile(tokens);
    }

    if (!d->valid)
        return Value::errorPARSE();

    for (int pc = 0; pc < d->codes.count(); pc++) {
        Value ret;   // for the function caller
        Opcode& opcode = d->codes[pc];
        index = opcode.index;
        switch (opcode.type) {
            // no operation
        case Opcode::Nop:
            break;

            // load a constant, push to stack
        case Opcode::Load:
            entry.reset();
            entry.val = d->constants[index];
            stack.push(entry);
            break;

            // unary operation
        case Opcode::Neg:
            entry.reset();
            entry.val = d->valueOrElement(map, index, fe, stack.pop().val);
            if (!entry.val.isError()) // do nothing if we got an error
                entry.val = calc->mul(entry.val, -1);
            stack.push(entry);
            break;

            // binary operation: take two values from stack, do the operation,
            // push the result to stack
        case Opcode::Add:
            entry.reset();
            val2 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val1 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val2 = calc->add(val1, val2);
            entry.reset();
            entry.val = val2;
            stack.push(entry);
            break;

        case Opcode::Sub:
            val2 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val1 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val2 = calc->sub(val1, val2);
            entry.reset();
            entry.val = val2;
            stack.push(entry);
            break;

        case Opcode::Mul:
            val2 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val1 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val2 = calc->mul(val1, val2);
            entry.reset();
            entry.val = val2;
            stack.push(entry);
            break;

        case Opcode::Div:
            val2 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val1 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val2 = calc->div(val1, val2);
            entry.reset();
            entry.val = val2;
            stack.push(entry);
            break;

        case Opcode::Pow:
            val2 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val1 = numericOrError(converter, d->valueOrElement(map, index, fe, stack.pop().val));
            val2 = calc->pow(val1, val2);
            entry.reset();
            entry.val = val2;
            stack.push(entry);
            break;

            // string concatenation
        case Opcode::Concat:
            val1 = converter->asString(stack.pop().val);
            val2 = converter->asString(stack.pop().val);
            if (val1.isError() || val2.isError())
                val1 = Value::errorVALUE();
            else
                val1 = Value(val2.asString().append(val1.asString()));
            entry.reset();
            entry.val = val1;
            stack.push(entry);
            break;

           // array intersection
        case Opcode::Intersect: {
            val1 = stack.pop().val;
            val2 = stack.pop().val;
            Region r1(d->constants[index].asString(), map, d->sheet);
            Region r2(d->constants[index+1].asString(), map, d->sheet);
            if(!r1.isValid() || !r2.isValid()) {
                val1 = Value::errorNULL();
            } else {
                Region r = r1.intersected(r2);
                QRect rect = r.boundingRect();
                Cell cell;
                if(rect.top() == rect.bottom())
                    cell = Cell(r.firstSheet(), fe.mycol, rect.top());
                else if(rect.left() == rect.right())
                    cell = Cell(r.firstSheet(), rect.left(), fe.mycol);
                if(cell.isNull())
                    val1 = Value::errorNULL();
                else if(cell.isEmpty())
                    val1 = Value::errorNULL();
                else
                    val1 = cell.value();
            }
            entry.reset();
            entry.val = val1;
            stack.push(entry);
        } break;

          // region union
        case Opcode::Union: {
            Region r = stack.pop().reg;
            Region r2 = stack.pop().reg;
            entry.reset();
            if (!r.isValid() || !r2.isValid()) {
                val1 = Value::errorVALUE();
                r = Region();
            } else {
                r.add(r2);
                r.firstSheet()->cellStorage()->valueRegion(r);
                // store the reference, so we can use it within functions (not entirely correct)
                entry.col1 = r.boundingRect().left();
                entry.row1 = r.boundingRect().top();
                entry.col2 = r.boundingRect().right();
                entry.row2 = r.boundingRect().bottom();
            }
            entry.val = val1;
            entry.reg = r;
            stack.push(entry);
        } break;

            // logical not
        case Opcode::Not:
            val1 = converter->asBoolean(d->valueOrElement(map, index, fe, stack.pop().val));
            if (val1.isError())
                val1 = Value::errorVALUE();
            else
                val1 = Value(!val1.asBoolean());
            entry.reset();
            entry.val = val1;
            stack.push(entry);
            break;

            // comparison
        case Opcode::Equal:
            val1 = d->valueOrElement(map, index, fe, stack.pop().val);
            val2 = d->valueOrElement(map, index, fe, stack.pop().val);
            if (val1.isError())
                ;
            else if (val2.isError())
                val1 = val2;
            else if (val2.compare(val1) == 0)
                val1 = Value(true);
            else
                val1 = Value(false);
            entry.reset();
            entry.val = val1;
            stack.push(entry);
            break;

            // less than
        case Opcode::Less:
            val1 = d->valueOrElement(map, index, fe, stack.pop().val);
            val2 = d->valueOrElement(map, index, fe, stack.pop().val);
            if (val1.isError())
                ;
            else if (val2.isError())
                val1 = val2;
            else if (val2.compare(val1) < 0)
                val1 = Value(true);
            else
                val1 = Value(false);
            entry.reset();
            entry.val = val1;
            stack.push(entry);
            break;

            // greater than
        case Opcode::Greater: {
            val1 = d->valueOrElement(map, index, fe, stack.pop().val);
            val2 = d->valueOrElement(map, index, fe, stack.pop().val);
            if (val1.isError())
                ;
            else if (val2.isError())
                val1 = val2;
            else if (val2.compare(val1) > 0)
                val1 = Value(true);
            else
                val1 = Value(false);
            entry.reset();
            entry.val = val1;
            stack.push(entry);
        }
        break;

        // cell in a sheet
        case Opcode::Cell: {
            c = d->constants[index].asString();
            val1 = Value::empty();
            entry.reset();

            const Region region(c, map, d->sheet);
            if( ! region.isValid() ) {
                val1 = Value::errorREF();
            } else if ( region.isSingular()) {
                const QPoint position = region.firstRange().topLeft();
                if (cellIndirections.isEmpty())
                    val1 = Cell(region.firstSheet(), position).value();
                else {
                    Cell cell(region.firstSheet(), position);
                    cell = cellIndirections.value(cell, cell);
                    if (values.contains(cell))
                        val1 = values.value(cell);
                    else {
                        values[cell] = Value::errorCIRCLE();
                        if (cell.isFormula())
                            val1 = cell.formula().evalRecursive(cellIndirections, values);
                        else
                            val1 = cell.value();
                        values[cell] = val1;
                    }
                }
                // store the reference, so we can use it within functions
                entry.col1 = entry.col2 = position.x();
                entry.row1 = entry.row2 = position.y();
                entry.reg = region;
            } else {
                kWarning() << "Unhandled non singular region in Opcode::Cell with rects=" << region.rects();
            }
            entry.val = val1;
            stack.push(entry);
        }
        break;

        // selected range in a sheet
        case Opcode::Range: {
            c = d->constants[index].asString();
            val1 = Value::empty();
            entry.reset();

            const Region region(c, map, d->sheet);
            if (region.isValid()) {
                val1 = region.firstSheet()->cellStorage()->valueRegion(region);
                // store the reference, so we can use it within functions
                entry.col1 = region.firstRange().left();
                entry.row1 = region.firstRange().top();
                entry.col2 = region.firstRange().right();
                entry.row2 = region.firstRange().bottom();
                entry.reg = region;
            }

            entry.val = val1; // any array is valid here
            stack.push(entry);
        }
        break;

        // reference
        case Opcode::Ref:
            val1 = d->constants[index];
            entry.reset();
            entry.val = val1;
            stack.push(entry);
            break;

            // calling function
        case Opcode::Function:
            // sanity check, this should not happen unless opcode is wrong
            // (i.e. there's a bug in the compile() function)
            if (stack.count() < index)
                return Value::errorVALUE(); // not enough arguments

            args.clear();
            fe.ranges.clear();
            fe.ranges.resize(index);
            fe.regions.clear();
            fe.regions.resize(index);
            fe.sheet = d->sheet;
            for (; index; index--) {
                stackEntry e = stack.pop();
                args.insert(args.begin(), e.val);
                // fill the FunctionExtra object
                fe.ranges[index - 1].col1 = e.col1;
                fe.ranges[index - 1].row1 = e.row1;
                fe.ranges[index - 1].col2 = e.col2;
                fe.ranges[index - 1].row2 = e.row2;
                fe.regions[index - 1] = e.reg;
            }

            // function name as string value
            val1 = converter->asString(stack.pop().val);
            if (val1.isError())
                return val1;
            function = FunctionRepository::self()->function(val1.asString());
            if (!function)
                return Value::errorNAME(); // no such function

            ret = function->exec(args, calc, &fe);
            entry.reset();
            entry.val = ret;
            stack.push(entry);

            break;

#ifdef KSPREAD_INLINE_ARRAYS
            // creating an array
        case Opcode::Array: {
            const int cols = d->constants[index].asInteger();
            const int rows = d->constants[index+1].asInteger();
            // check if enough array elements are available
            if (stack.count() < cols * rows)
                return Value::errorVALUE();
            Value array(Value::Array);
            for (int row = rows - 1; row >= 0; --row) {
                for (int col = cols - 1; col >= 0; --col) {
                    array.setElement(col, row, stack.pop().val);
                }
            }
            entry.reset();
            entry.val = array;
            stack.push(entry);
            break;
        }
#endif
        default:
            break;
        }
    }

    if (!d->sheet)
        delete map;

    // more than one value in stack ? unsuccessful execution...
    if (stack.count() != 1)
        return Value::errorVALUE();

    return stack.pop().val;
}

Formula& Formula::operator=(const Formula & other)
{
    d = other.d;
    return *this;
}

bool Formula::operator==(const Formula& other) const
{
    return (d->expression == other.d->expression);
}

// Debugging aid

QString Formula::dump() const
{
    QString result;

    if (d->dirty) {
        Tokens tokens = scan(d->expression);
        compile(tokens);
    }

    result = QString("Expression: [%1]\n").arg(d->expression);
#if 0
    Value value = eval();
    result.append(QString("Result: %1\n").arg(
                      converter->asString(value).asString()));
#endif

    result.append("  Constants:\n");
    for (int c = 0; c < d->constants.count(); c++) {
        QString vtext;
        Value val = d->constants[c];
        if (val.isString()) vtext = QString("[%1]").arg(val.asString());
        else if (val.isNumber()) vtext = QString("%1").arg((double) numToDouble(val.asFloat()));
        else if (val.isBoolean()) vtext = QString("%1").arg(val.asBoolean() ? "True" : "False");
        else if (val.isError()) vtext = "error";
        else vtext = "???";
        result += QString("    #%1 = %2\n").arg(c).arg(vtext);
    }

    result.append("\n");
    result.append("  Code:\n");
    for (int i = 0; i < d->codes.count(); i++) {
        QString ctext;
        switch (d->codes[i].type) {
        case Opcode::Load:      ctext = QString("Load #%1").arg(d->codes[i].index); break;
        case Opcode::Ref:       ctext = QString("Ref #%1").arg(d->codes[i].index); break;
        case Opcode::Function:  ctext = QString("Function (%1)").arg(d->codes[i].index); break;
        case Opcode::Add:       ctext = "Add"; break;
        case Opcode::Sub:       ctext = "Sub"; break;
        case Opcode::Mul:       ctext = "Mul"; break;
        case Opcode::Div:       ctext = "Div"; break;
        case Opcode::Neg:       ctext = "Neg"; break;
        case Opcode::Concat:    ctext = "Concat"; break;
        case Opcode::Pow:       ctext = "Pow"; break;
        case Opcode::Intersect: ctext = "Intersect"; break;
        case Opcode::Equal:     ctext = "Equal"; break;
        case Opcode::Not:       ctext = "Not"; break;
        case Opcode::Less:      ctext = "Less"; break;
        case Opcode::Greater:   ctext = "Greater"; break;
        case Opcode::Array:     ctext = QString("Array (%1x%2)").arg(d->constants[d->codes[i].index].asInteger()).arg(d->constants[d->codes[i].index+1].asInteger()); break;
        case Opcode::Nop:       ctext = "Nop"; break;
        case Opcode::Cell:      ctext = "Cell"; break;
        case Opcode::Range:     ctext = "Range"; break;
        default: ctext = "Unknown"; break;
        }
        result.append("   ").append(ctext).append("\n");
    }

    return result;
}

QTextStream& operator<<(QTextStream& ts, Formula formula)
{
    ts << formula.dump();
    return ts;
}
