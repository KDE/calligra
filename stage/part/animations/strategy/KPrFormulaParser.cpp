/* This file is part of the KDE project
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
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

#include <QStack>
#include <QString>
#include <math.h>
#include <qmath.h>
#include "../KPrAnimationCache.h"
#include "KoShape.h"
#include "KoTextBlockData.h"
#include <QTextLayout>
#include "KoTextShapeData.h"
#include <QTextDocument>
#include <QTextBlock>

static int fopPrecedence(FToken::Op op)
{
    int prec = -1;
    switch (op) {
    case FToken::Caret        : prec = 7; break;
    case FToken::Asterisk     : prec = 5; break;
    case FToken::Slash        : prec = 6; break;
    case FToken::Plus         : prec = 3; break;
    case FToken::Minus        : prec = 3; break;
    case FToken::RightPar     : prec = 0; break;
    case FToken::Comma        : prec = 0; break;
    case FToken::LeftPar      : prec = -1; break;
    default: prec = -1; break;
    }
    return prec;
}

static FToken::Op matchOperator(const QString& text)
{
    FToken::Op result = FToken::InvalidOp;

    if (text.length() == 1) {
        QChar p = text[0];
        switch (p.unicode()) {
        case '+': result = FToken::Plus; break;
        case '-': result = FToken::Minus; break;
        case '*': result = FToken::Asterisk; break;
        case '/': result = FToken::Slash; break;
        case '^': result = FToken::Caret; break;
        case '(': result = FToken::LeftPar; break;
        case ')': result = FToken::RightPar; break;
        case ',': result = FToken::Comma; break;
        default : result = FToken::InvalidOp; break;
        }
    }
        return result;
}

// for null token
const FToken FToken::null;

/**********************
    FToken
 **********************/

FToken::FToken(Type type, const QString &text, int pos)
{
    m_type = type;
    m_text = text;
    m_pos = pos;
}

// copy constructor
FToken::FToken(const FToken &ftoken)
{
    m_type = ftoken.m_type;
    m_text = ftoken.m_text;
    m_pos = ftoken.m_pos;
}

// assignment operator
FToken& FToken::operator=(const FToken &ftoken)
{
    m_type = ftoken.m_type;
    m_text = ftoken.m_text;
    m_pos = ftoken.m_pos;
    return *this;
}

QString FToken::asIdentifierName() const
{
    if (isIdentifierName()) {
        return m_text;
    }
    else {
        return QString();
    }
}

FToken::Op FToken::asOperator() const
{
    return matchOperator(m_text);
}

qreal FToken::asNumber() const
{
        return m_text.toDouble();
}

/**********************
    FTokenStack
 **********************/

FTokenStack::FTokenStack(): QVector<FToken>()
{
    topIndex = 0;
    ensureSpace();
}

bool FTokenStack::isEmpty() const
{
    return topIndex == 0;
}

unsigned FTokenStack::itemCount() const
{
    return topIndex;
}

void FTokenStack::push(const FToken &ftoken)
{
    ensureSpace();
    insert(topIndex++, ftoken);
}

FToken FTokenStack::pop()
{
    return (topIndex > 0) ? FToken(at(--topIndex)) : FToken();
}

const FToken& FTokenStack::top()
{
    return top(0);
}

const FToken& FTokenStack::top(unsigned index)
{
    if (topIndex > index)
        return at(topIndex -index - 1);
    return FToken::null;
}

void FTokenStack::ensureSpace()
{
    while ((int) topIndex >= size()) {
        resize(size() + 10);
    }
}

/**********************
    KPrFormulaParser
 **********************/
KPrFormulaParser::KPrFormulaParser(QString formula, KoShape *shape, KoTextBlockData *textBlockData, ParseType type)
    : m_shape(shape)
    , m_textBlockData(textBlockData)
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

FTokens KPrFormulaParser::scan(QString formula) const
{
    FTokens ftokens;
    // parsing state
    enum { Start, Finish, InNumber, InIdentifierName } state;
    int i = 0;
    state = Start;
    bool parseError = false;
    QString ftokenText;
    int ftokenStart = 0;
    QString f = formula;
    f.append(QChar());
    while( state != Finish && i < f.length()) {
        QChar c = f[i];
        switch (state) {
        case Start:
            ftokenStart = i;
            if (c.isLetter()) {
                ftokenText.append(c);
                state = InIdentifierName;
                i++;
            }
            else if (c.isNumber() || c == '.') {
                ftokenText.append(c);
                state = InNumber;
                i++;
            }
            else if (c.isSpace()) {
                i++;
            }
            else if (c == QChar::Null) {
                state = Finish;
            }
            else if (c == '$') {
                ftokenText.append(c);
                state = InIdentifierName;
                i++;
            }
            else {
                FToken::Op op = matchOperator(c);
                if (op == FToken::InvalidOp) {
                    parseError = true;
                    ftokens.append(FToken(FToken::Unknown, c, ftokenStart));
                } else {
                    ftokens.append(FToken(FToken::Operator, c, ftokenStart));
                }
                i++;
            }
            break;
        case InNumber:
            if (c.isNumber() || c == '.') {
                ftokenText.append(c);
                i++;
            }
            else {
                ftokens.append(FToken(FToken::Number, ftokenText, ftokenStart));
                ftokenText.clear();
                state = Start;
            }
            break;
        case InIdentifierName:
            if (c.isLetter()) {
                ftokenText.append(c);
                i++;
            }
            else {
                FToken t(FToken::IdentifierName, ftokenText, ftokenStart);
                ftokens.append(FToken(FToken::IdentifierName, ftokenText, ftokenStart));
                ftokenText.clear();
                state = Start;
            }
            break;
        case Finish:
            i++;
            break;
        }
    }
    if (parseError) {
        ftokens.setValid(false);
    }

    return ftokens;
}

void KPrFormulaParser::compile(const FTokens &ftokens)
{
    // initialize variables
    m_fvalid = false;
    m_codes.clear();
    m_constants.clear();
    m_identifier.clear();
    m_functions.clear();
    if (!ftokens.valid() || ftokens.count() == 0) {
        return;
    }

    FTokenStack syntaxStack;
    QStack<int> argStack;
    unsigned argCount = 1;

    for (int i = 0; i <= ftokens.count(); i++) {
        // helper ftoken: InvalidOp is end-of-formula
        FToken ftoken = (i < ftokens.count()) ? ftokens[i] : FToken(FToken::Operator);
        FToken::Type ftokenType = ftoken.type();


        // for constants, push immediately to stack
        // generate code to load from a constant
        if (ftokenType == FToken::Number) {
            syntaxStack.push(ftoken);
            m_constants.append(ftoken.asNumber());
            m_codes.append(FOpcode(FOpcode::Load, m_constants.count() - 1));
        }

        // for identifier push immediately to stack
        // generate code to load from reference
        else if (ftokenType == FToken::IdentifierName) {
            syntaxStack.push(ftoken);
            m_identifier.append(ftoken.asIdentifierName());
            m_codes.append(FOpcode(FOpcode::Identifier, m_identifier.count() - 1));
        }

        // for any other operator, try to apply all parsing rules
        else if (ftokenType == FToken::Operator) {
            // repeat until no more rule applies
            for (; ;) {
                bool ruleFound = false;

                // are we entering a function ?
                // if stack already has: id (
                if (syntaxStack.itemCount() >= 2) {
                    FToken par = syntaxStack.top();
                    FToken id = syntaxStack.top(1);
                    if (par.asOperator() == FToken::LeftPar)
                        if (id.isIdentifierName()) {
                            argStack.push(argCount);

                        }
                }

                // rule for function arguments, if ftoken is , or )
                // id ( arg1 , arg2 -> id ( arg
                if (!ruleFound)
                    if (syntaxStack.itemCount() >= 5)
                        if ((ftoken.asOperator() == FToken::RightPar) ||
                                (ftoken.asOperator() == FToken::Comma)) {
                            FToken arg2 = syntaxStack.top();
                            FToken sep = syntaxStack.top(1);
                            FToken arg1 = syntaxStack.top(2);
                            FToken par = syntaxStack.top(3);
                            FToken id = syntaxStack.top(4);
                            if (!arg2.isOperator())
                                if (sep.asOperator() == FToken::Comma)
                                    if (!arg1.isOperator())
                                        if (par.asOperator() == FToken::LeftPar)
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
                        FToken par2 = syntaxStack.top();
                        FToken argu = syntaxStack.top(1);
                        FToken par1 = syntaxStack.top(2);
                        FToken id = syntaxStack.top(3);
                        if (par2.asOperator() == FToken::RightPar)
                            if (!argu.isOperator())
                                if (par1.asOperator() == FToken::LeftPar)
                                    if (id.isIdentifierName()) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(argu);
                                        m_codes.append(FOpcode(FOpcode::Function, argCount));
                                        m_functions.append(id.text());
                                        argCount = argStack.empty() ? 0 : argStack.pop();
                                        argCount = 1;
                                    }
                    }

                // rule for parenthesis:  ( Y ) -> Y
                if (!ruleFound) {
                    if (syntaxStack.itemCount() >= 3) {
                        FToken right = syntaxStack.top();
                        FToken y = syntaxStack.top(1);
                        FToken left = syntaxStack.top(2);
                        if (right.isOperator()) {
                            if (!y.isOperator()) {
                                if (left.isOperator()) {
                                    if (right.asOperator() == FToken::RightPar) {
                                        if (left.asOperator() == FToken::LeftPar) {
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
                // conditions: precedence of op >= precedence of ftoken
                // action: push (op) to result
                // e.g. "A * B" becomes 'A' if ftoken is operator '+'
                if (!ruleFound) {
                    if (syntaxStack.itemCount() >= 3) {
                        FToken b = syntaxStack.top();
                        FToken op = syntaxStack.top(1);
                        FToken a = syntaxStack.top(2);
                        if (!a.isOperator()) {
                            if (!b.isOperator()) {
                                if (op.isOperator()) {
                                    if ((ftoken.asOperator() != FToken::LeftPar) && (ftoken.asOperator() != FToken::Comma)) {
                                        if (fopPrecedence(op.asOperator()) >= fopPrecedence(ftoken.asOperator())) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(b);
                                            switch (op.asOperator()) {
                                                // simple binary operations
                                            case FToken::Plus:        m_codes.append(FOpcode::Add); break;
                                            case FToken::Minus:       m_codes.append(FOpcode::Sub); break;
                                            case FToken::Asterisk:    m_codes.append(FOpcode::Mul); break;
                                            case FToken::Slash:       m_codes.append(FOpcode::Div); break;
                                            case FToken::Caret:       m_codes.append(FOpcode::Pow); break;

                                            default: break;
                                            };
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // rule for unary operator:  (op1) (op2) X -> (op1) X
                // conditions: op2 is unary, ftoken is not '('
                // action: push (op2) to result
                // e.g.  "* - 2" becomes '*'
                if (!ruleFound) {
                    if (ftoken.asOperator() != FToken::LeftPar) {
                        if (syntaxStack.itemCount() >= 3) {
                            FToken x = syntaxStack.top();
                            FToken op2 = syntaxStack.top(1);
                            FToken op1 = syntaxStack.top(2);
                            if (!x.isOperator()) {
                                if (op1.isOperator()) {
                                    if (op2.isOperator()) {
                                        if ((op2.asOperator() == FToken::Plus) ||
                                            (op2.asOperator() == FToken::Minus)) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(x);
                                            if (op2.asOperator() == FToken::Minus) {
                                                m_codes.append(FOpcode(FOpcode::Neg));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // auxiliary rule for unary operator:  (op) X -> X
                // conditions: op is unary, op is first in syntax stack, ftoken is not '('
                // action: push (op) to result
                if (!ruleFound) {
                    if (ftoken.asOperator() != FToken::LeftPar) {
                        if (syntaxStack.itemCount() == 2) {
                            FToken x = syntaxStack.top();
                            FToken op = syntaxStack.top(1);
                            if (!x.isOperator()) {
                                if (op.isOperator()) {
                                    if ((op.asOperator() == FToken::Plus) ||
                                        (op.asOperator() == FToken::Minus)) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(x);
                                        if (op.asOperator() == FToken::Minus) {
                                            m_codes.append(FOpcode(FOpcode::Neg));
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
            // can't apply rules anymore, push the ftoken
            syntaxStack.push(ftoken);
        }
        // unknown ftoken is invalid
        else if (ftokenType == FToken::Unknown) {
            break;
        }
    }

    // syntaxStack must left only one operand and end-of-formula (i.e. InvalidOp)
    m_fvalid = false;
    if (syntaxStack.itemCount() == 2) {
        if (syntaxStack.top().asOperator() == FToken::InvalidOp) {
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
        m_fcompiled=false;
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
        FOpcode &opcode = m_codes[pc];
        switch (opcode.type) {
        // load a constant, push to stack
        case FOpcode::Load:
            stack.push(m_constants[opcode.index]);
            break;

        // unary operation
        case FOpcode::Neg:
            stack.push(stack.pop() * -1);
            break;

        // binary operation: take two values from stack, do the operation,
        // push the result to stack
        case FOpcode::Add:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 + val2);
            break;

        case FOpcode::Sub:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 - val2);
            break;

        case FOpcode::Mul:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 * val2);
            break;

        case FOpcode::Div:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 / val2);
            break;

        case FOpcode::Pow:
            val2 = stack.pop();
            val1 = stack.pop();
            val2 = ::pow(val1, val2);
            stack.push(val2);
            break;

        case FOpcode::Function: {
            val1 = stack.pop();
            if (opcode.index > 1) {
                val2 = stack.pop();
                stack.push(formulaToValue(m_functions[funcCount], val1, val2));
            } else {
                stack.push(formulaToValue(m_functions[funcCount], val1, -999));
            }
            funcCount++;
            break;
        }

        case FOpcode::Identifier: {
            if (m_functions.contains(m_identifier[opcode.index])) {
                break;
            }

            stack.push(identifierToValue(m_identifier[opcode.index], cache, time));
        }
            break;
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

qreal KPrFormulaParser::identifierToValue(QString identifier, KPrAnimationCache *cache, const qreal time) const
{
    if (identifier == "width") {
        if (m_textBlockData) {
            if (KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData*>(m_shape->userData())) {
                QTextDocument *textDocument = textShapeData->document();
                for (int i = 0; i < textDocument->blockCount(); i++) {
                    QTextBlock textBlock = textDocument->findBlockByNumber(i);
                    if (textBlock.userData() == m_textBlockData) {
                        QTextLayout *layout = textBlock.layout();
                        return layout->minimumWidth() / cache->pageSize().width();
                    }
                }
            }
        }
        else {
            return m_shape->size().width() / cache->pageSize().width();
        }
    }
    else if (identifier == "height") {
        if (m_textBlockData) {
            if (KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData*>(m_shape->userData())) {
                QTextDocument *textDocument = textShapeData->document();
                for (int i = 0; i < textDocument->blockCount(); i++) {
                    QTextBlock textBlock = textDocument->findBlockByNumber(i);
                    if (textBlock.userData() == m_textBlockData) {
                        QTextLayout *layout = textBlock.layout();
                        return layout->boundingRect().height() / cache->pageSize().height();
                    }
                }
            }
        }
        else {
            return m_shape->size().height() / cache->pageSize().height();
        }
    }
    else if (identifier == "x") {
        return m_shape->position().x() / cache->pageSize().width();
    }
    else if (identifier == "y") {
        return m_shape->position().y() / cache->pageSize().height();
    }
    else if ((identifier == "$") && (m_type == KPrFormulaParser::Formula) ) {
        return time;
    }
    else if (identifier == "pi") {
        return M_PI;
    }
    else if (identifier == "e") {
        return exp(1);
    }
    return 0.0;
}

qreal KPrFormulaParser::formulaToValue(QString identifier, qreal arg1, qreal arg2) const
{
    if (identifier == "sin") {
        return sin(arg1);
    }
    else if (identifier == "cos") {
        return cos(arg1);
    }
    else if (identifier == "abs") {
        return qAbs(arg1);
    }
    else if (identifier == "sqrt") {
        return sqrt(arg1);
    }
    else if (identifier == "tan") {
        return tan(arg1);
    }
    else if (identifier == "atan") {
        return atan(arg1);
    }
    else if (identifier == "acos") {
        return acos(arg1);
    }
    else if (identifier == "asin") {
        return asin(arg1);
    }
    else if (identifier == "exp") {
        return exp(arg1);
    }
    else if (identifier == "log") {
        return log(arg1);
    }
    else if (identifier == "min") {
        return qMin(arg1, arg2);
    }
    else if (identifier == "max") {
        return qMax(arg1, arg2);
    }
    return 0.0;
}

