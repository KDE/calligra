/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qptrlist.h>

#include <kdebug.h>

#include <kformuladefs.h>
#include <symboltable.h>

#include "fsparser.h"


using namespace std;

class ParserNode {
public:
    ParserNode() { debugCount++; }
    virtual ~ParserNode() { debugCount--; }
    //virtual void output( ostream& ) = 0;
    virtual void buildXML( QDomDocument doc, QDomElement element ) = 0;

    static int debugCount;
};

int ParserNode::debugCount = 0;

class PrimaryNode : public ParserNode {
public:
    PrimaryNode( QString primary ) : m_primary( primary ), m_functionName( false ) {}
    //virtual void output( ostream& stream ) { stream << "PrimaryNode {" << m_primary << "}" << endl; }
    virtual void buildXML( QDomDocument doc, QDomElement element );
    void setUnicode( QChar unicode ) { m_unicode = unicode; }
    void setFunctionName( bool functionName ) { m_functionName = functionName; }
private:
    QString m_primary;
    QChar m_unicode;
    bool m_functionName;
};

void PrimaryNode::buildXML( QDomDocument doc, QDomElement element )
{
    if ( m_unicode != QChar::null ) {
        QDomElement de = doc.createElement( "TEXT" );
        de.setAttribute( "CHAR", QString( m_unicode ) );
        element.setAttribute( "SYMBOL", "3" );
        element.appendChild( de );
    }
    else {
        if ( m_functionName ) {
            QDomElement namesequence = doc.createElement( "NAMESEQUENCE" );
            element.appendChild( namesequence );
            element = namesequence;
        }
        for ( uint i = 0; i < m_primary.length(); i++ ) {
            QDomElement de = doc.createElement( "TEXT" );
            de.setAttribute( "CHAR", QString( m_primary[i] ) );
            element.appendChild( de );
        }
    }
}

class OperatorNode : public ParserNode {
public:
    OperatorNode( QString type, ParserNode* lhs, ParserNode* rhs )
        : m_type( type ), m_lhs( lhs ), m_rhs( rhs ) {}
    ~OperatorNode() { delete m_rhs; delete m_lhs; }
//     virtual void output( ostream& stream ) {
//         stream << "OperatorNode {";
//         m_lhs->output( stream ); stream << m_type; m_rhs->output( stream );
//         stream << "}" << endl; }
protected:
    QString m_type;
    ParserNode* m_lhs;
    ParserNode* m_rhs;
};

class ExprNode : public OperatorNode {
public:
    ExprNode( QString type, ParserNode* lhs, ParserNode* rhs ) : OperatorNode( type, lhs, rhs ) {}
    virtual void buildXML( QDomDocument doc, QDomElement element );
};

void ExprNode::buildXML( QDomDocument doc, QDomElement element )
{
    m_lhs->buildXML( doc, element );
    QDomElement de = doc.createElement( "TEXT" );
    de.setAttribute( "CHAR", QString( m_type ) );
    element.appendChild( de );
    m_rhs->buildXML( doc, element );
}

class TermNode : public OperatorNode {
public:
    TermNode( QString type, ParserNode* lhs, ParserNode* rhs ) : OperatorNode( type, lhs, rhs ) {}
    virtual void buildXML( QDomDocument doc, QDomElement element );
};

void TermNode::buildXML( QDomDocument doc, QDomElement element )
{
    if ( m_type == "*" ) {
        QDomElement bracket = doc.createElement( "BRACKET" );
        bracket.setAttribute( "LEFT", '(' );
        bracket.setAttribute( "RIGHT", ')' );
        QDomElement content = doc.createElement( "CONTENT" );
        QDomElement sequence = doc.createElement( "SEQUENCE" );

        m_lhs->buildXML( doc, sequence );
        QDomElement de = doc.createElement( "TEXT" );
        de.setAttribute( "CHAR", QString( m_type ) );
        sequence.appendChild( de );
        m_rhs->buildXML( doc, sequence );

        content.appendChild( sequence );
        bracket.appendChild( content );
        element.appendChild( bracket );
    }
    else {
        QDomElement fraction = doc.createElement( "FRACTION" );
        QDomElement numerator = doc.createElement( "NUMERATOR" );
        QDomElement sequence = doc.createElement( "SEQUENCE" );
        m_lhs->buildXML( doc, sequence );
        numerator.appendChild( sequence );
        fraction.appendChild( numerator );
        QDomElement denominator = doc.createElement( "DENOMINATOR" );
        sequence = doc.createElement( "SEQUENCE" );
        m_rhs->buildXML( doc, sequence );
        denominator.appendChild( sequence );
        fraction.appendChild( denominator );
        element.appendChild( fraction );
    }
}


class PowerNode : public OperatorNode {
public:
    PowerNode( QString type, ParserNode* lhs, ParserNode* rhs ) : OperatorNode( type, lhs, rhs ) {}
    virtual void buildXML( QDomDocument doc, QDomElement element );
};

void PowerNode::buildXML( QDomDocument doc, QDomElement element )
{
    QDomElement index = doc.createElement( "INDEX" );
    QDomElement content = doc.createElement( "CONTENT" );
    QDomElement sequence = doc.createElement( "SEQUENCE" );
    m_lhs->buildXML( doc, sequence );
    content.appendChild( sequence );
    index.appendChild( content );
    QDomElement upperRight = doc.createElement( "UPPERRIGHT" );
    sequence = doc.createElement( "SEQUENCE" );
    m_rhs->buildXML( doc, sequence );
    upperRight.appendChild( sequence );
    index.appendChild( upperRight );
    element.appendChild( index );
}

class SqrtNode : public ParserNode {
public:
    SqrtNode( ParserNode* expr ) : m_expr( expr ) {}
    ~SqrtNode() { delete m_expr; }
    virtual void buildXML( QDomDocument doc, QDomElement element );
private:
    ParserNode* m_expr;
};

void SqrtNode::buildXML( QDomDocument doc, QDomElement element )
{
    QDomElement root = doc.createElement( "ROOT" );
    QDomElement content = doc.createElement( "CONTENT" );
    QDomElement sequence = doc.createElement( "SEQUENCE" );
    m_expr->buildXML( doc, sequence );
    content.appendChild( sequence );
    root.appendChild( content );
    element.appendChild( root );
}

class FunctionNode : public ParserNode {
public:
    FunctionNode( PrimaryNode* name, QPtrList<ParserNode>& args ) : m_name( name ), m_args( args ) {
        m_args.setAutoDelete( true );
    }
    ~FunctionNode() { delete m_name; }
    //virtual void output( ostream& stream );
    virtual void buildXML( QDomDocument doc, QDomElement element );
private:
    PrimaryNode* m_name;
    QPtrList<ParserNode> m_args;
};

void FunctionNode::buildXML( QDomDocument doc, QDomElement element )
{
    m_name->buildXML( doc, element );
    QDomElement bracket = doc.createElement( "BRACKET" );
    bracket.setAttribute( "LEFT", '(' );
    bracket.setAttribute( "RIGHT", ')' );
    QDomElement content = doc.createElement( "CONTENT" );
    QDomElement sequence = doc.createElement( "SEQUENCE" );

    for ( uint i = 0; i < m_args.count(); i++ ) {
        m_args.at( i )->buildXML( doc, sequence );
    }

    content.appendChild( sequence );
    bracket.appendChild( content );
    element.appendChild( bracket );
}

// void FunctionNode::output( ostream& stream )
// {
//     m_name->output( stream );
//     for ( uint i = 0; i < m_args.count(); i++ ) {
//         m_args.at( i )->output( stream );
//     }
// }

class RowNode : public ParserNode {
public:
    RowNode( QPtrList<ParserNode> row ) : m_row( row ) { m_row.setAutoDelete( true ); }
    //virtual void output( ostream& stream );
    virtual void buildXML( QDomDocument doc, QDomElement element );
    uint colunms() const { return m_row.count(); }
    void setRequiredColumns( uint requiredColumns ) { m_requiredColumns = requiredColumns; }
private:
    QPtrList<ParserNode> m_row;
    uint m_requiredColumns;
};

void RowNode::buildXML( QDomDocument doc, QDomElement element )
{
    for ( uint i = 0; i < m_requiredColumns; i++ ) {
        QDomElement sequence = doc.createElement( "SEQUENCE" );
        if ( i < m_row.count() ) {
            m_row.at( i )->buildXML( doc, sequence );
        }
        element.appendChild( sequence );
    }
}

// void RowNode::output( ostream& stream )
// {
//     stream << "[";
//     for ( uint i = 0; i < m_row.count(); i++ ) {
//         m_row.at( i )->output( stream );
//         if ( i < m_row.count()-1 ) {
//             stream << ", ";
//         }
//     }
//     stream << "]";
// }

class MatrixNode : public ParserNode {
public:
    MatrixNode( QPtrList<RowNode> rows ) : m_rows( rows ) { m_rows.setAutoDelete( true ); }
    //virtual void output( ostream& stream );
    virtual void buildXML( QDomDocument doc, QDomElement element );
private:
    QPtrList<RowNode> m_rows;
};

void MatrixNode::buildXML( QDomDocument doc, QDomElement element )
{
    uint colunms = 0;
    for ( uint i = 0; i < m_rows.count(); i++ ) {
        colunms = QMAX( colunms, m_rows.at( i )->colunms() );
    }
    QDomElement matrix = doc.createElement( "MATRIX" );
    matrix.setAttribute( "ROWS", m_rows.count() );
    matrix.setAttribute( "COLUMNS", colunms );
    for ( uint i = 0; i < m_rows.count(); i++ ) {
        m_rows.at( i )->setRequiredColumns( colunms );
        m_rows.at( i )->buildXML( doc, matrix );
        matrix.appendChild( doc.createComment( "end of row" ) );
    }
    element.appendChild( matrix );
}

// void MatrixNode::output( ostream& stream )
// {
//     stream << "[";
//     for ( uint i = 0; i < m_rows.count(); i++ ) {
//         m_rows.at( i )->output( stream );
//         if ( i < m_rows.count()-1 ) {
//             stream << ", ";
//         }
//     }
//     stream << "]";
// }


FormulaStringParser::FormulaStringParser( const KFormula::SymbolTable& symbolTable, QString formula )
    : m_symbolTable( symbolTable ), m_formula( formula ), pos( 0 )
{
}

FormulaStringParser::~FormulaStringParser()
{
    delete head;
    if ( ParserNode::debugCount != 0 ) {
        kdDebug( KFormula::DEBUGID ) << "ParserNode::debugCount = " << ParserNode::debugCount << endl;
    }
}

QDomDocument FormulaStringParser::parse()
{
    nextToken();
    head = parseExpr();
    //head->output( cout );
    kdDebug( KFormula::DEBUGID ) << endl;

    QDomDocument doc("KFORMULA");
    QDomElement de = doc.createElement("FORMULA");
    // here comes the current version of FormulaElement
    de.setAttribute( "VERSION", "4" );
    head->buildXML( doc, de );
    doc.appendChild(de);

    kdDebug( KFormula::DEBUGID ) << doc.toString() << endl;
    return doc;
}

ParserNode* FormulaStringParser::parseExpr()
{
    ParserNode* lhs = parseTerm();
    for ( ;; ) {
        switch ( currentType ) {
        case PLUS:
        case SUB: {
            QString c = current;
            nextToken();
            lhs = new ExprNode( c, lhs, parseTerm() );
            break;
        }
        default:
            return lhs;
        }
    }
}

ParserNode* FormulaStringParser::parseTerm()
{
    ParserNode* lhs = parsePower();
    for ( ;; ) {
        switch ( currentType ) {
        case MUL:
        case DIV: {
            QString c = current;
            nextToken();
            lhs = new TermNode( c, lhs, parsePower() );
            break;
        }
        default:
            return lhs;
        }
    }
}

ParserNode* FormulaStringParser::parsePower()
{
    ParserNode* lhs = parsePrimary();
    for ( ;; ) {
        switch ( currentType ) {
        case POW: {
            QString c = current;
            nextToken();
            lhs = new PowerNode( c, lhs, parsePrimary() );
            break;
        }
        default:
            return lhs;
        }
    }
}

ParserNode* FormulaStringParser::parsePrimary()
{
    switch ( currentType ) {
    case NUMBER: {
        PrimaryNode* node = new PrimaryNode( current );
        nextToken();
        return node;
    }
    case NAME: {
        if ( current == "sqrt" ) {
            nextToken();
            expect( LP, "'(' expected" );
            ParserNode* node = parseExpr();
            expect( RP, "')' expected" );
            return new SqrtNode( node );
        }
        PrimaryNode* node = new PrimaryNode( current );
        node->setUnicode( m_symbolTable.unicode( current ) );
        nextToken();
        if ( currentType == LP ) {
            nextToken();
            QPtrList<ParserNode> args;
            args.setAutoDelete( false );
            while ( ( currentType != EOL ) && ( currentType != RP ) ) {
                ParserNode* node = parseExpr();
                args.append( node );
                if ( currentType == COMMA ) {
                    nextToken();
                }
            }
            expect( RP, "')' expected" );
            node->setFunctionName( true );
            return new FunctionNode( node, args );
        }
        return node;
    }
    case LP: {
        nextToken();
        ParserNode* node = parseExpr();
        expect( RP, "')' expected" );
        return node;
    }
    case LB: {
        nextToken();
        QPtrList<RowNode> rows;
        rows.setAutoDelete( false );
        while ( currentType == LB ) {
            nextToken();
            QPtrList<ParserNode> row;
            row.setAutoDelete( false );
            while ( ( currentType != EOL ) && ( currentType != RB ) ) {
                row.append( parseExpr() );
                if ( currentType == COMMA ) {
                    nextToken();
                }
            }
            expect( RB, "']' expected" );
            rows.append( new RowNode( row ) );
            if ( currentType == COMMA ) {
                nextToken();
            }
        }
        expect( RB, "']' expected" );
        return new MatrixNode( rows );
    }
    default:
        error( "Unexpected token" );
        return new PrimaryNode( current );
    }
}

void FormulaStringParser::expect( TokenType type, QString msg )
{
    if ( currentType == type ) {
        nextToken();
    }
    else {
        error( msg );
    }
}

QString FormulaStringParser::nextToken()
{
    while ( !eol() && m_formula[pos].isSpace() ) {
        pos++;
    }
    if ( eol() ) {
        currentType = EOL;
        return QString::null;
    }
    if ( m_formula[pos].isDigit() ) {
        uint begin = pos;
        readNumber();
        currentType = NUMBER;
        return current = m_formula.mid( begin, pos-begin );
    }
    else if ( m_formula[pos].isLetter() ) {
        uint begin = pos;
        pos++;
        while ( !eol() && m_formula[pos].isLetter() ) {
            pos++;
        }
        currentType = NAME;
        return current = m_formula.mid( begin, pos-begin );
    }
    else {
        switch ( m_formula[pos].latin1() ) {
        case '+':
            pos++;
            currentType = PLUS;
            return current = "+";
        case '-':
            pos++;
            currentType = SUB;
            return current = "-";
        case '*':
            pos++;
            if ( !eol() && m_formula[pos] == '*' ) {
                pos++;
                currentType = POW;
                return current = "**";
            }
            currentType = MUL;
            return current = "*";
        case '/':
            pos++;
            currentType = DIV;
            return current = "/";
        case '^':
            pos++;
            currentType = POW;
            return current = "**";
        case '(':
            pos++;
            currentType = LP;
            return current = "(";
        case ')':
            pos++;
            currentType = RP;
            return current = ")";
        case '[':
            pos++;
            currentType = LB;
            return current = "[";
        case ']':
            pos++;
            currentType = RB;
            return current = "]";
        case ',':
            pos++;
            currentType = COMMA;
            return current = ",";
        default:
            pos++;
            currentType = OTHER;
            return current = m_formula.mid( pos-1, 1 );
        }
    }
}

void FormulaStringParser::readNumber()
{
    readDigits();
    if ( pos < m_formula.length()-1 ) {
        QChar ch = m_formula[pos];

        // Look for a dot.
        if ( ch == '.' ) {
            pos++;
            ch = m_formula[pos];
            if ( ch.isDigit() ) {
                readDigits();
            }
            else {
                pos--;
                return;
            }
        }

        // there might as well be an exponent
        if ( pos < m_formula.length()-1 ) {
            ch = m_formula[pos];
            if ( ( ch == 'E' ) || ( ch == 'e' ) ) {
                pos++;
                ch = m_formula[pos];

                // signs are allowed after the exponent
                if ( ( ( ch == '+' ) || ( ch == '-' ) ) &&
                     ( pos < m_formula.length()-1 ) ) {
                    pos++;
                    ch = m_formula[pos];
                    if ( ch.isDigit() ) {
                        readDigits();
                    }
                    else {
                        pos -= 2;
                        return;
                    }
                }
                else if ( ch.isDigit() ) {
                    readDigits();
                }
                else {
                    pos--;
                }
            }
        }
    }
}


void FormulaStringParser::readDigits()
{
    while ( !eol() && m_formula[pos].isDigit() ) {
        pos++;
    }
}

void FormulaStringParser::error( QString err )
{
    kdDebug( KFormula::DEBUGID ) << err << " at " << pos << " (" << currentType << "; " << current << ")" << endl;
}
