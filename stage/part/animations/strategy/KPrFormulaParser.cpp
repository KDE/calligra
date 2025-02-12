/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>
 * SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/* heavily based on Ariya's work (see kspread/formula) -- all errors are my own! */

#include "KPrFormulaParser.h"

#include "../KPrAnimationCache.h"
#include "KoShape.h"
#include "KoTextBlockData.h"
#include "KoTextShapeData.h"
#include <QStack>
#include <QString>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <math.h>
#include <qmath.h>

static int fopPrecedence(Token::Op op)
{
    int prec = -1;
    switch (op) {
    case Token::Caret:
        prec = 7;
        break;
    case Token::Asterisk:
        prec = 5;
        break;
    case Token::Slash:
        prec = 6;
        break;
    case Token::Plus:
        prec = 3;
        break;
    case Token::Minus:
        prec = 3;
        break;
    case Token::RightPar:
        prec = 0;
        break;
    case Token::Comma:
        prec = 0;
        break;
    case Token::LeftPar:
        prec = -1;
        break;
    default:
        prec = -1;
        break;
    }
    return prec;
}

static Token::Op matchOperator(const QString &text)
{
    Token::Op result = Token::InvalidOp;

    if (text.length() == 1) {
        QChar p = text[0];
        switch (p.unicode()) {
        case '+':
            result = Token::Plus;
            break;
        case '-':
            result = Token::Minus;
            break;
        case '*':
            result = Token::Asterisk;
            break;
        case '/':
            result = Token::Slash;
            break;
        case '^':
            result = Token::Caret;
            break;
        case '(':
            result = Token::LeftPar;
            break;
        case ')':
            result = Token::RightPar;
            break;
        case ',':
            result = Token::Comma;
            break;
        default:
            result = Token::InvalidOp;
            break;
        }
    }
    return result;
}

// for null token
const Token Token::null;

/**********************
    Token
 **********************/

Token::Token(Type type, const QString &text, int pos)
{
    m_type = type;
    m_text = text;
    m_pos = pos;
}

// copy constructor
Token::Token(const Token &token)
{
    m_type = token.m_type;
    m_text = token.m_text;
    m_pos = token.m_pos;
}

// assignment operator
Token &Token::operator=(const Token &token) = default;

QString Token::asIdentifierName() const
{
    if (isIdentifierName()) {
        return m_text;
    } else {
        return QString();
    }
}

Token::Op Token::asOperator() const
{
    return matchOperator(m_text);
}

qreal Token::asNumber() const
{
    return m_text.toDouble();
}

/**********************
    TokenStack
 **********************/

TokenStack::TokenStack()
    : QVector<Token>()
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

void TokenStack::push(const Token &token)
{
    ensureSpace();
    insert(topIndex++, token);
}

Token TokenStack::pop()
{
    return (topIndex > 0) ? Token(at(--topIndex)) : Token();
}

const Token &TokenStack::top() const
{
    return top(0);
}

const Token &TokenStack::top(unsigned index) const
{
    if (topIndex > index)
        return at(topIndex - index - 1);
    return Token::null;
}

void TokenStack::ensureSpace()
{
    while ((int)topIndex >= size()) {
        resize(size() + 10);
    }
}

/**********************
    KPrFormulaParser
 **********************/
KPrFormulaParser::KPrFormulaParser(const QString &formula, KoShape *shape, QTextBlockUserData *textBlockUserData, ParseType type)
    : m_shape(shape)
    , m_textBlockData(textBlockUserData)
    , m_formula(formula)
    , m_fcompiled(false)
    , m_fvalid(false)
    , m_type(type)
{
    compile(scan(formula));
}

QString KPrFormulaParser::formula() const
{
    return m_formula;
}

Tokens KPrFormulaParser::scan(const QString &formula) const
{
    Tokens tokens;
    // parsing state
    enum {
        Start,
        Finish,
        InNumber,
        InIdentifierName
    } state;
    int i = 0;
    state = Start;
    bool parseError = false;
    QString tokenText;
    int tokenStart = 0;
    QString f = formula;
    f.append(QChar());
    while (state != Finish && i < f.length()) {
        QChar c = f[i];
        switch (state) {
        case Start:
            tokenStart = i;
            if (c.isLetter()) {
                tokenText.append(c);
                state = InIdentifierName;
                i++;
            } else if (c.isNumber() || c == '.') {
                tokenText.append(c);
                state = InNumber;
                i++;
            } else if (c.isSpace()) {
                i++;
            } else if (c == QChar::Null) {
                state = Finish;
            } else if (c == '$') {
                tokenText.append(c);
                state = InIdentifierName;
                i++;
            } else {
                Token::Op op = matchOperator(c);
                if (op == Token::InvalidOp) {
                    parseError = true;
                    tokens.append(Token(Token::Unknown, c, tokenStart));
                } else {
                    tokens.append(Token(Token::Operator, c, tokenStart));
                }
                i++;
            }
            break;
        case InNumber:
            if (c.isNumber() || c == '.') {
                tokenText.append(c);
                i++;
            } else {
                tokens.append(Token(Token::Number, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            }
            break;
        case InIdentifierName:
            if (c.isLetter()) {
                tokenText.append(c);
                i++;
            } else {
                Token t(Token::IdentifierName, tokenText, tokenStart);
                tokens.append(Token(Token::IdentifierName, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            }
            break;
        case Finish:
            i++;
            break;
        }
    }
    if (parseError) {
        tokens.setValid(false);
    }

    return tokens;
}

void KPrFormulaParser::compile(const Tokens &tokens)
{
    // initialize variables
    m_fvalid = false;
    m_codes.clear();
    m_constants.clear();
    m_identifier.clear();
    m_functions.clear();
    if (!tokens.valid() || tokens.count() == 0) {
        return;
    }

    TokenStack syntaxStack;
    QStack<int> argStack;
    unsigned argCount = 1;

    for (int i = 0; i <= tokens.count(); i++) {
        // helper token: InvalidOp is end-of-formula
        Token token = (i < tokens.count()) ? tokens[i] : Token(Token::Operator);
        Token::Type tokenType = token.type();

        // for constants, push immediately to stack
        // generate code to load from a constant
        if (tokenType == Token::Number) {
            syntaxStack.push(token);
            m_constants.append(token.asNumber());
            m_codes.append(Opcode(Opcode::Load, m_constants.count() - 1));
        }

        // for identifier push immediately to stack
        // generate code to load from reference
        else if (tokenType == Token::IdentifierName) {
            syntaxStack.push(token);
            m_identifier.append(token.asIdentifierName());
            m_codes.append(Opcode(Opcode::Identifier, m_identifier.count() - 1));
        }

        // for any other operator, try to apply all parsing rules
        else if (tokenType == Token::Operator) {
            // repeat until no more rule applies
            for (;;) {
                bool ruleFound = false;

                // are we entering a function ?
                // if stack already has: id (
                if (syntaxStack.itemCount() >= 2) {
                    Token par = syntaxStack.top();
                    Token id = syntaxStack.top(1);
                    if (par.asOperator() == Token::LeftPar)
                        if (id.isIdentifierName()) {
                            argStack.push(argCount);
                        }
                }

                // rule for function arguments, if token is , or )
                // id ( arg1 , arg2 -> id ( arg
                if (!ruleFound)
                    if (syntaxStack.itemCount() >= 5)
                        if ((token.asOperator() == Token::RightPar) || (token.asOperator() == Token::Comma)) {
                            Token arg2 = syntaxStack.top();
                            Token sep = syntaxStack.top(1);
                            Token arg1 = syntaxStack.top(2);
                            Token par = syntaxStack.top(3);
                            Token id = syntaxStack.top(4);
                            if (!arg2.isOperator())
                                if (sep.asOperator() == Token::Comma)
                                    if (!arg1.isOperator())
                                        if (par.asOperator() == Token::LeftPar)
                                            if (id.isIdentifierName()) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                argCount++;
                                            }
                        }

                // rule for function last argument:
                //  id ( arg ) -> arg
                if (!ruleFound)
                    if (syntaxStack.itemCount() >= 4) {
                        Token par2 = syntaxStack.top();
                        Token argu = syntaxStack.top(1);
                        Token par1 = syntaxStack.top(2);
                        Token id = syntaxStack.top(3);
                        if (par2.asOperator() == Token::RightPar)
                            if (!argu.isOperator())
                                if (par1.asOperator() == Token::LeftPar)
                                    if (id.isIdentifierName()) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(argu);
                                        m_codes.append(Opcode(Opcode::Function, argCount));
                                        m_functions.append(id.text());
                                        argCount = argStack.empty() ? 0 : argStack.pop();
                                        argCount = 1;
                                    }
                    }

                // rule for parenthesis:  ( Y ) -> Y
                if (!ruleFound) {
                    if (syntaxStack.itemCount() >= 3) {
                        Token right = syntaxStack.top();
                        Token y = syntaxStack.top(1);
                        Token left = syntaxStack.top(2);
                        if (right.isOperator()) {
                            if (!y.isOperator()) {
                                if (left.isOperator()) {
                                    if (right.asOperator() == Token::RightPar) {
                                        if (left.asOperator() == Token::LeftPar) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(y);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // rule for binary operator:  A (op) B -> A
                // conditions: precedence of op >= precedence of token
                // action: push (op) to result
                // e.g. "A * B" becomes 'A' if token is operator '+'
                if (!ruleFound) {
                    if (syntaxStack.itemCount() >= 3) {
                        Token b = syntaxStack.top();
                        Token op = syntaxStack.top(1);
                        Token a = syntaxStack.top(2);
                        if (!a.isOperator()) {
                            if (!b.isOperator()) {
                                if (op.isOperator()) {
                                    if ((token.asOperator() != Token::LeftPar) && (token.asOperator() != Token::Comma)) {
                                        if (fopPrecedence(op.asOperator()) >= fopPrecedence(token.asOperator())) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(b);
                                            switch (op.asOperator()) {
                                                // simple binary operations
                                            case Token::Plus:
                                                m_codes.append(Opcode::Add);
                                                break;
                                            case Token::Minus:
                                                m_codes.append(Opcode::Sub);
                                                break;
                                            case Token::Asterisk:
                                                m_codes.append(Opcode::Mul);
                                                break;
                                            case Token::Slash:
                                                m_codes.append(Opcode::Div);
                                                break;
                                            case Token::Caret:
                                                m_codes.append(Opcode::Pow);
                                                break;

                                            default:
                                                break;
                                            };
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // rule for unary operator:  (op1) (op2) X -> (op1) X
                // conditions: op2 is unary, token is not '('
                // action: push (op2) to result
                // e.g.  "* - 2" becomes '*'
                if (!ruleFound) {
                    if (token.asOperator() != Token::LeftPar) {
                        if (syntaxStack.itemCount() >= 3) {
                            Token x = syntaxStack.top();
                            Token op2 = syntaxStack.top(1);
                            Token op1 = syntaxStack.top(2);
                            if (!x.isOperator()) {
                                if (op1.isOperator()) {
                                    if (op2.isOperator()) {
                                        if ((op2.asOperator() == Token::Plus) || (op2.asOperator() == Token::Minus)) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(x);
                                            if (op2.asOperator() == Token::Minus) {
                                                m_codes.append(Opcode(Opcode::Neg));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // auxiliary rule for unary operator:  (op) X -> X
                // conditions: op is unary, op is first in syntax stack, token is not '('
                // action: push (op) to result
                if (!ruleFound) {
                    if (token.asOperator() != Token::LeftPar) {
                        if (syntaxStack.itemCount() == 2) {
                            Token x = syntaxStack.top();
                            Token op = syntaxStack.top(1);
                            if (!x.isOperator()) {
                                if (op.isOperator()) {
                                    if ((op.asOperator() == Token::Plus) || (op.asOperator() == Token::Minus)) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(x);
                                        if (op.asOperator() == Token::Minus) {
                                            m_codes.append(Opcode(Opcode::Neg));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (!ruleFound) {
                    break;
                }
            }
            // can't apply rules anymore, push the token
            syntaxStack.push(token);
        }
        // unknown token is invalid
        else if (tokenType == Token::Unknown) {
            break;
        }
    }

    // syntaxStack must left only one operand and end-of-formula (i.e. InvalidOp)
    m_fvalid = false;
    if (syntaxStack.itemCount() == 2) {
        if (syntaxStack.top().asOperator() == Token::InvalidOp) {
            if (!syntaxStack.top(1).isOperator()) {
                m_fvalid = true;
            }
        }
    }

    // bad parsing ? clean-up everything
    m_fcompiled = true;
    if (!m_fvalid) {
        m_constants.clear();
        m_codes.clear();
        m_identifier.clear();
        m_functions.clear();
        m_fcompiled = false;
    }
}

qreal KPrFormulaParser::eval(KPrAnimationCache *cache, const qreal time) const
{
    QStack<qreal> stack;
    qreal val1, val2;
    int funcCount = 0;
    if (!m_fvalid || !m_fcompiled) {
        return 0.0;
    }
    if ((m_type == KPrFormulaParser::Formula) && (time < 0)) {
        return 0.0;
    }

    for (int pc = 0; pc < m_codes.count(); pc++) {
        Opcode &opcode = m_codes[pc];
        switch (opcode.type) {
        // load a constant, push to stack
        case Opcode::Load:
            stack.push(m_constants[opcode.index]);
            break;

        // unary operation
        case Opcode::Neg:
            stack.push(stack.pop() * -1);
            break;

        // binary operation: take two values from stack, do the operation,
        // push the result to stack
        case Opcode::Add:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 + val2);
            break;

        case Opcode::Sub:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 - val2);
            break;

        case Opcode::Mul:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 * val2);
            break;

        case Opcode::Div:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 / val2);
            break;

        case Opcode::Pow:
            val2 = stack.pop();
            val1 = stack.pop();
            val2 = ::pow(val1, val2);
            stack.push(val2);
            break;

        case Opcode::Function: {
            val1 = stack.pop();
            if (opcode.index > 1) {
                val2 = stack.pop();
                stack.push(formulaToValue(m_functions[funcCount], val1, val2));
            } else {
                stack.push(formulaToValue(m_functions[funcCount], val1));
            }
            funcCount++;
            break;
        }

        case Opcode::Identifier: {
            if (m_functions.contains(m_identifier[opcode.index])) {
                break;
            }

            stack.push(identifierToValue(m_identifier[opcode.index], cache, time));
        } break;
        default:
            break;
        }
    }

    // more than one value in stack ? unsuccessful execution...
    if (stack.count() != 1) {
        m_fvalid = false;
        return 0.0;
    }

    return stack.pop();
}

bool KPrFormulaParser::valid() const
{
    return m_fvalid;
}

qreal KPrFormulaParser::identifierToValue(const QString &identifier, KPrAnimationCache *cache, const qreal time) const
{
    if (identifier == "width") {
        if (m_textBlockData) {
            if (KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData *>(m_shape->userData())) {
                QTextDocument *textDocument = textShapeData->document();
                for (int i = 0; i < textDocument->blockCount(); i++) {
                    QTextBlock textBlock = textDocument->findBlockByNumber(i);
                    if (textBlock.userData() == m_textBlockData) {
                        QTextLayout *layout = textBlock.layout();
                        return layout->minimumWidth() / cache->pageSize().width();
                    }
                }
            }
        } else {
            return m_shape->size().width() / cache->pageSize().width();
        }
    } else if (identifier == "height") {
        if (m_textBlockData) {
            if (KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData *>(m_shape->userData())) {
                QTextDocument *textDocument = textShapeData->document();
                for (int i = 0; i < textDocument->blockCount(); i++) {
                    QTextBlock textBlock = textDocument->findBlockByNumber(i);
                    if (textBlock.userData() == m_textBlockData) {
                        QTextLayout *layout = textBlock.layout();
                        return layout->boundingRect().height() / cache->pageSize().height();
                    }
                }
            }
        } else {
            return m_shape->size().height() / cache->pageSize().height();
        }
    } else if (identifier == "x") {
        return m_shape->position().x() / cache->pageSize().width();
    } else if (identifier == "y") {
        return m_shape->position().y() / cache->pageSize().height();
    } else if ((identifier == "$") && (m_type == KPrFormulaParser::Formula)) {
        return time;
    } else if (identifier == "pi") {
        return M_PI;
    } else if (identifier == "e") {
        return exp(1.0);
    }
    return 0.0;
}

qreal KPrFormulaParser::formulaToValue(const QString &identifier, qreal arg1) const
{
    if (identifier == "sin") {
        return sin(arg1);
    } else if (identifier == "cos") {
        return cos(arg1);
    } else if (identifier == "abs") {
        return qAbs(arg1);
    } else if (identifier == "sqrt") {
        return sqrt(arg1);
    } else if (identifier == "tan") {
        return tan(arg1);
    } else if (identifier == "atan") {
        return atan(arg1);
    } else if (identifier == "acos") {
        return acos(arg1);
    } else if (identifier == "asin") {
        return asin(arg1);
    } else if (identifier == "exp") {
        return exp(arg1);
    } else if (identifier == "log") {
        return log(arg1);
    }
    return 0.0;
}

qreal KPrFormulaParser::formulaToValue(const QString &identifier, qreal arg1, qreal arg2) const
{
    if (identifier == "min") {
        return qMin(arg1, arg2);
    } else if (identifier == "max") {
        return qMax(arg1, arg2);
    }
    return 0.0;
}
