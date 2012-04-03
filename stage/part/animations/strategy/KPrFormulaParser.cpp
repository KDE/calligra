#include "KPrFormulaParser.h"

#include <QStack>
#include <QString>
#include <math.h>
#include "../KPrAnimationCache.h"
#include "KoShape.h"
#include "KoTextBlockData.h"
#include <QTextLayout>
#include "KoTextShapeData.h"
#include <QTextDocument>
#include <QTextBlock>

static int opPrecedence(Token::Op op)
{
    int prec = -1;
    switch (op) {
    case Token::Caret        : prec = 7; break;
    case Token::Asterisk     : prec = 5; break;
    case Token::Slash        : prec = 6; break;
    case Token::Plus         : prec = 3; break;
    case Token::Minus        : prec = 3; break;
    case Token::RightPar     : prec = 0; break;
    case Token::LeftPar      : prec = -1; break;
    default: prec = -1; break;
    }
    return prec;
}

static Token::Op matchOperator(const QString& text)
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
        case '(': result = Token::LeftPar; break;
        case ')': result = Token::RightPar; break;
        default : result = Token::InvalidOp; break;
        }
    }
        return result;
}

/**********************
    KPrFormulaParser
 **********************/
KPrFormulaParser::KPrFormulaParser(QString formula, KoShape *shape, KoTextBlockData *textBlockData)
    : m_shape(shape)
    , m_textBlockData(textBlockData)
    , m_formula(formula)
    , m_fcompiled(false)
    , m_fvalid(false)
{
    compile(scan(formula));
    //qDebug() << "initial valid:" << m_fvalid;
    //qDebug() << "initial compiled: " << m_fcompiled;
}

QString KPrFormulaParser::formula() const
{
    return m_formula;
}

Tokens KPrFormulaParser::scan(QString formula)
{
    //qDebug() << "KPrFormulaParser:scan" << formula;
    Tokens tokens;
    // parsing state
    enum { Start, Finish, InNumber, InIdentifierName } state;
    int i = 0;
    state = Start;
    bool parseError = false;
    QString tokenText;
    int tokenStart = 0;
    QString f = formula;
    f.append(QChar());
    while( state != Finish && i < f.length()) {
        QChar c = f[i];
        switch (state) {
        case Start:
            tokenStart = i;
            if (c.isLetter()) {
                tokenText.append(c);
                state = InIdentifierName;
                i++;
            }
            else if (c.isNumber() || c == '.') {
                tokenText.append(c);
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
                tokenText.append(c);
                state = InIdentifierName;
                i++;
            }
            else {
                Token::Op op = matchOperator(c);
                if (op == Token::InvalidOp) {
                    parseError = true;
                    //qDebug() << "parse error" << c;
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
            }
            else {
                tokens.append(Token(Token::Number, tokenText, tokenStart));
                tokenText.clear();
                state = Start;
            }
            break;
        case InIdentifierName:
            if (c.isLetter()) {
                tokenText.append(c);
                i++;
            }
            else {
                Token t(Token::IdentifierName, tokenText, tokenStart);
                tokens.append(Token(Token::IdentifierName, tokenText, tokenStart));
                //qDebug() << "tokenTest: " << tokenText;
                tokenText.clear();
                state = Start;
            }
            break;
        case Finish:
            i++;
            break;
        }
    }
    if (parseError)
        tokens.setValid(false);

    return tokens;
}

void KPrFormulaParser::compile(const Tokens& tokens) const
{
    // initialize variables
    //qDebug() << "init compiler";
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
    unsigned funcCount = 1;
    //qDebug() << "Token count" << tokens.count();

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
            //qDebug() << "Append identifier: "<< token.text();
            m_identifier.append(token.asIdentifierName());
            m_codes.append(Opcode(Opcode::Identifier, m_identifier.count() - 1));
        }

        // for any other operator, try to apply all parsing rules
        else if (tokenType == Token::Operator) {
            // repeat until no more rule applies
            //qDebug () << "token operator compiler: " << token.text();
            for (; ;) {
                bool ruleFound = false;

                // are we entering a function ?
                // if stack already has: id (
                if (syntaxStack.itemCount() >= 2) {
                    Token par = syntaxStack.top();
                    Token id = syntaxStack.top(1);
                    //qDebug() << "is a function" << par.text() << id.text();
                    if (par.asOperator() == Token::LeftPar)
                        if (id.isIdentifierName()) {
                            //qDebug() << "yes is a function";
                            argStack.push(argCount);
                            argCount = 1;
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
                        //qDebug() << "last argument" << par2.text() << argu.text() << par1.text() << id.text();
                        if (par2.asOperator() == Token::RightPar)
                            if (!argu.isOperator())
                                if (par1.asOperator() == Token::LeftPar)
                                    if (id.isIdentifierName()) {
                                        ruleFound = true;
                                        //qDebug() << "rule for functions accepted";
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(argu);
                                        m_codes.append(Opcode(Opcode::Function, funcCount));
                                        m_functions.append(id.text());
                                        //Q_ASSERT(!argStack.empty());
                                        argCount = argStack.empty() ? 0 : argStack.pop();
                                        funcCount++;
                                    }
                    }

                // rule for parenthesis:  ( Y ) -> Y
                if (!ruleFound) {
                    if (syntaxStack.itemCount() >= 3) {
                        Token right = syntaxStack.top();
                        Token y = syntaxStack.top(1);
                        Token left = syntaxStack.top(2);
                        //qDebug() << "rule for parenthesis: " << right.text() << y.text() << left.text();
                        if (right.isOperator()) {
                            if (!y.isOperator()) {
                                if (left.isOperator()) {
                                    if (right.asOperator() == Token::RightPar) {
                                        if (left.asOperator() == Token::LeftPar) {
                                            //qDebug() << "rule for parenthesis accepted";
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
                        //qDebug() << "rule for b operators: " << b.text() << op.text() << a.text();
                        if (!a.isOperator()) {
                            if (!b.isOperator()) {
                                if (op.isOperator()) {
                                    if (token.asOperator() != Token::LeftPar) {
                                        if (opPrecedence(op.asOperator()) >= opPrecedence(token.asOperator())) {
                                            ruleFound = true;
                                            //qDebug() << "rule for b operator accepted";
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(b);
                                            switch (op.asOperator()) {
                                                // simple binary operations
                                            case Token::Plus:        m_codes.append(Opcode::Add); break;
                                            case Token::Minus:       m_codes.append(Opcode::Sub); break;
                                            case Token::Asterisk:    m_codes.append(Opcode::Mul); break;
                                            case Token::Slash:       m_codes.append(Opcode::Div); break;
                                            case Token::Caret:       m_codes.append(Opcode::Pow); break;

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
                // conditions: op2 is unary, token is not '('
                // action: push (op2) to result
                // e.g.  "* - 2" becomes '*'
                if (!ruleFound) {
                    if (token.asOperator() != Token::LeftPar) {
                        if (syntaxStack.itemCount() >= 3) {
                            Token x = syntaxStack.top();
                            Token op2 = syntaxStack.top(1);
                            Token op1 = syntaxStack.top(2);
                            //qDebug() << "rule for unary operators: " << x.text() << op2.text() << op1.text();
                            if (!x.isOperator()) {
                                if (op1.isOperator()) {
                                    if (op2.isOperator()) {
                                        if ((op2.asOperator() == Token::Plus) ||
                                            (op2.asOperator() == Token::Minus)) {
                                            ruleFound = true;
                                            //qDebug() << "rule for u operator accepted";
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
                            //qDebug() << "auxiliary rule for unit" << x.text() << op.text();
                            if (!x.isOperator()) {
                                if (op.isOperator()) {
                                    if ((op.asOperator() == Token::Plus) ||
                                        (op.asOperator() == Token::Minus)) {
                                        ruleFound = true;
                                        //qDebug() << "rule for a u operator accepted";
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
    //qDebug() << "antes de validar compilacion";
    m_fvalid = false;
    //qDebug() << "final syntax count: " << syntaxStack.itemCount();
    if (syntaxStack.itemCount() == 2) {
        if (syntaxStack.top().asOperator() == Token::InvalidOp) {
            if (!syntaxStack.top(1).isOperator()) {
                //qDebug() << "set mvalid true";
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
    //Print stack

    //qDebug() << "end of compilation" << m_fcompiled;
}

qreal KPrFormulaParser::eval(KPrAnimationCache * cache, qreal time) const
{
    QStack<qreal> stack;
    qreal val1, val2;
    //qDebug() << "time: " << time;
    //qDebug() << m_fvalid;
    //qDebug() << m_fcompiled;
    if (!m_fvalid || !m_fcompiled) {
        //qDebug() << "sale";
        return 0.0;
    }
    //qDebug() << "code count: " << m_codes.count();
    /*
    for (int i=0; i < m_codes.count(); i++) {
        qDebug() << m_codes.at(i).index;
    }
    for (int i=0; i < m_identifier.count(); i++) {
        qDebug() << m_identifier[i];
    }

    for (int i=0; i < m_constants.count(); i++) {
        qDebug() << m_constants.at(i);
    }

    qDebug() << "lista de formulas: ";
    for (int i=0; i < m_functions.count(); i++) {
        qDebug() << m_functions.at(i);
    }*/

    for (int pc = 0; pc < m_codes.count(); pc++) {
        Opcode& opcode = m_codes[pc];
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
            //qDebug() << "case add: " << val1 + val2;
            break;

        case Opcode::Sub:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 - val2);
            //qDebug() << "case sub: " << val1 << val2 << val1 - val2;
            break;

        case Opcode::Mul:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 * val2);
            //qDebug() << "cmul: " << val1 * val2;
            break;

        case Opcode::Div:
            val2 = stack.pop();
            val1 = stack.pop();
            stack.push(val1 / val2);
            //qDebug() << "case div: " << val1 / val2;
            break;

        case Opcode::Pow:
            val2 = stack.pop();
            val1 = stack.pop();
            val2 = ::pow(val1, val2);
            stack.push(val2);
            //qDebug() << "case pow: " << val2;
            break;

        case Opcode::Function: {
            //qDebug() << "case formula";
            val1 = stack.pop();
            //qDebug() << val1;// << val2;
            //qDebug() << formulaToValue(m_functions[opcode.index-1],cache,val1);
            stack.push(formulaToValue(m_functions[opcode.index-1],cache, val1)) ;
            break;
        }

        case Opcode::Identifier: {
            //qDebug() << "case identifier" << m_identifier[opcode.index];
            //qDebug() << "value: " << identifierToValue(m_identifier[opcode.index], cache, time);
            if (m_functions.contains(m_identifier[opcode.index])) {
                //qDebug() << "formula found";
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
        //qDebug() << "bad eexecution";
        m_fvalid = false;
        return 0.0;
    }

    return stack.pop();
}

bool KPrFormulaParser::valid() const
{
    return m_fvalid;
}

qreal KPrFormulaParser::identifierToValue(QString identifier, KPrAnimationCache * cache, qreal time) const
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
    } else if (identifier == "height") {
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
    } else if (identifier == "x") {
        return m_shape->position().x() / cache->pageSize().width();
    } else if (identifier == "y") {
        //qDebug() << "case y" << m_shape->position().y();
        //qDebug() << cache->pageSize().height();
        return m_shape->position().y() / cache->pageSize().height();
    } else if (identifier == "$") {
        //qDebug() << "time called";
        return time;
    } else if (identifier == "pi") {
        //qDebug() << "Pi called";
        return M_PI;
    }

    return 0.0;
}

qreal KPrFormulaParser::formulaToValue(QString identifier, KPrAnimationCache *cache, qreal arg) const
{
    Q_UNUSED(cache);
    //qDebug() << "formulaTovalue: " << identifier << arg;
    if (identifier == "sin") {
        //qDebug() << "sin called with arg" << arg;
        return sin(arg);
    } else if (identifier == "cos") {
        return cos(arg);
    }

    return 0.0;
}

