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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <q3ptrlist.h>

#include <kdebug.h>
#include <klocale.h>

#include <kformuladefs.h>
#include <kformuladocument.h>
#include <symboltable.h>

#include "fsparser.h"


using namespace std;

class ParserNode {
public:
    ParserNode() { debugCount++; }
    virtual ~ParserNode() { debugCount--; }
    //virtual void output( ostream& ) = 0;
    virtual void buildXML( QDomDocument& doc, QDomElement element ) = 0;
    virtual bool isSimple() { return false; }

    static int debugCount;
};

int ParserNode::debugCount = 0;

class PrimaryNode : public ParserNode {
public:
    PrimaryNode( QString primary ) : m_primary( primary ), m_functionName( false ) {}
    //virtual void output( ostream& stream ) { stream << "PrimaryNode {" << m_primary << "}" << endl; }
    virtual void buildXML( QDomDocument& doc, QDomElement element );
    virtual bool isSimple() { return true; }
    void setUnicode( QChar unicode ) { m_unicode = unicode; }
    void setFunctionName( bool functionName ) { m_functionName = functionName; }
    QString primary() const { return m_primary; }
private:
    QString m_primary;
    QChar m_unicode;
    bool m_functionName;
};

void PrimaryNode::buildXML( QDomDocument& doc, QDomElement element )
{
    if ( m_unicode != QChar::Null ) {
        QDomElement de = doc.createElement( "TEXT" );
        de.setAttribute( "CHAR", QString( m_unicode ) );
        de.setAttribute( "SYMBOL", "3" );
        element.appendChild( de );
    }
    else {
        if ( m_functionName ) {
            QDomElement namesequence = doc.createElement( "NAMESEQUENCE" );
            element.appendChild( namesequence );
            element = namesequence;
        }
        for ( int i = 0; i < m_primary.length(); i++ ) {
            QDomElement de = doc.createElement( "TEXT" );
            de.setAttribute( "CHAR", QString( m_primary[i] ) );
            element.appendChild( de );
        }
    }
}

class UnaryMinus : public ParserNode {
public:
    UnaryMinus( ParserNode* primary ) : m_primary( primary ) {}
    ~UnaryMinus() { delete m_primary; }
    virtual void buildXML( QDomDocument& doc, QDomElement element );
private:
    ParserNode* m_primary;
};

void UnaryMinus::buildXML( QDomDocument& doc, QDomElement element )
{
    QDomElement de = doc.createElement( "TEXT" );
    de.setAttribute( "CHAR", "-" );
    element.appendChild( de );
    m_primary->buildXML( doc, element );
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

class AssignNode : public OperatorNode {
public:
    AssignNode( QString type, ParserNode* lhs, ParserNode* rhs ) : OperatorNode( type, lhs, rhs ) {}
    virtual void buildXML( QDomDocument& doc, QDomElement element );
};

void AssignNode::buildXML( QDomDocument& doc, QDomElement element )
{
    m_lhs->buildXML( doc, element );
    QDomElement de = doc.createElement( "TEXT" );
    de.setAttribute( "CHAR", QString( m_type ) );
    element.appendChild( de );
    m_rhs->buildXML( doc, element );
}

class ExprNode : public OperatorNode {
public:
    ExprNode( QString type, ParserNode* lhs, ParserNode* rhs ) : OperatorNode( type, lhs, rhs ) {}
    virtual void buildXML( QDomDocument& doc, QDomElement element );
};

void ExprNode::buildXML( QDomDocument& doc, QDomElement element )
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
    virtual void buildXML( QDomDocument& doc, QDomElement element );
};

void TermNode::buildXML( QDomDocument& doc, QDomElement element )
{
    if ( m_type == "*" ) {
        m_lhs->buildXML( doc, element );
        QDomElement de = doc.createElement( "TEXT" );
        de.setAttribute( "CHAR", QString( m_type ) );
        element.appendChild( de );
        m_rhs->buildXML( doc, element );
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
    virtual void buildXML( QDomDocument& doc, QDomElement element );
};

void PowerNode::buildXML( QDomDocument& doc, QDomElement element )
{
    QDomElement index = doc.createElement( "INDEX" );
    QDomElement content = doc.createElement( "CONTENT" );
    QDomElement sequence = doc.createElement( "SEQUENCE" );
    content.appendChild( sequence );
    index.appendChild( content );

    if ( !m_lhs->isSimple() ) {
        QDomElement bracket = doc.createElement( "BRACKET" );
        bracket.setAttribute( "LEFT", '(' );
        bracket.setAttribute( "RIGHT", ')' );
        sequence.appendChild( bracket );

        content = doc.createElement( "CONTENT" );
        bracket.appendChild( content );

        sequence = doc.createElement( "SEQUENCE" );
        content.appendChild( sequence );
    }
    m_lhs->buildXML( doc, sequence );
    if ( m_type == "_" ) {
        QDomElement lowerRight = doc.createElement( "LOWERRIGHT" );
        sequence = doc.createElement( "SEQUENCE" );
        m_rhs->buildXML( doc, sequence );
        lowerRight.appendChild( sequence );
        index.appendChild( lowerRight );
    }
    else {
        QDomElement upperRight = doc.createElement( "UPPERRIGHT" );
        sequence = doc.createElement( "SEQUENCE" );
        m_rhs->buildXML( doc, sequence );
        upperRight.appendChild( sequence );
        index.appendChild( upperRight );
    }
    element.appendChild( index );
}


class FunctionNode : public ParserNode {
public:
    FunctionNode( PrimaryNode* name, Q3PtrList<ParserNode>& args ) : m_name( name ), m_args( args ) {
        m_args.setAutoDelete( true );
    }
    ~FunctionNode() { delete m_name; }
    //virtual void output( ostream& stream );
    virtual void buildXML( QDomDocument& doc, QDomElement element );
private:
    void buildSymbolXML( QDomDocument& doc, QDomElement element, KFormula::SymbolType type );
    PrimaryNode* m_name;
    Q3PtrList<ParserNode> m_args;
};

void FunctionNode::buildSymbolXML( QDomDocument& doc, QDomElement element, KFormula::SymbolType type )
{
    QDomElement symbol = doc.createElement( "SYMBOL" );
    symbol.setAttribute( "TYPE", type );
    QDomElement content = doc.createElement( "CONTENT" );
    QDomElement sequence = doc.createElement( "SEQUENCE" );
    m_args.at( 0 )->buildXML( doc, sequence );
    content.appendChild( sequence );
    symbol.appendChild( content );
    if ( m_args.count() > 2 ) {
        ParserNode* lowerLimit = m_args.at( m_args.count()-2 );
        ParserNode* upperLimit = m_args.at( m_args.count()-1 );

        QDomElement lower = doc.createElement( "LOWER" );
        sequence = doc.createElement( "SEQUENCE" );
        lowerLimit->buildXML( doc, sequence );
        lower.appendChild( sequence );
        symbol.appendChild( lower );

        QDomElement upper = doc.createElement( "UPPER" );
        sequence = doc.createElement( "SEQUENCE" );
        upperLimit->buildXML( doc, sequence );
        upper.appendChild( sequence );
        symbol.appendChild( upper );
    }
    element.appendChild( symbol );
}

void FunctionNode::buildXML( QDomDocument& doc, QDomElement element )
{
    if ( ( m_name->primary() == "sqrt" ) && ( m_args.count() == 1 ) ) {
        QDomElement root = doc.createElement( "ROOT" );
        QDomElement content = doc.createElement( "CONTENT" );
        QDomElement sequence = doc.createElement( "SEQUENCE" );
        m_args.at( 0 )->buildXML( doc, sequence );
        content.appendChild( sequence );
        root.appendChild( content );
        element.appendChild( root );
    }
    else if ( ( m_name->primary() == "pow" ) && ( m_args.count() == 2 ) ) {
        QDomElement index = doc.createElement( "INDEX" );
        QDomElement content = doc.createElement( "CONTENT" );
        QDomElement sequence = doc.createElement( "SEQUENCE" );
        m_args.at( 0 )->buildXML( doc, sequence );
        content.appendChild( sequence );
        index.appendChild( content );
        QDomElement upperRight = doc.createElement( "UPPERRIGHT" );
        sequence = doc.createElement( "SEQUENCE" );
        m_args.at( 1 )->buildXML( doc, sequence );
        upperRight.appendChild( sequence );
        index.appendChild( upperRight );
        element.appendChild( index );
    }
    else if ( ( m_name->primary() == "sum" ) && ( m_args.count() > 0 ) ) {
        buildSymbolXML( doc, element, KFormula::Sum );
    }
    else if ( ( m_name->primary() == "prod" ) && ( m_args.count() > 0 ) ) {
        buildSymbolXML( doc, element, KFormula::Product );
    }
    else if ( ( ( m_name->primary() == "int" ) ||
                ( m_name->primary() == "integrate" ) ||
                ( m_name->primary() == "quad" ) )
              && ( m_args.count() > 0 ) ) {
        buildSymbolXML( doc, element, KFormula::Integral );
    }
    else {
        m_name->buildXML( doc, element );
        QDomElement bracket = doc.createElement( "BRACKET" );
        bracket.setAttribute( "LEFT", '(' );
        bracket.setAttribute( "RIGHT", ')' );
        QDomElement content = doc.createElement( "CONTENT" );
        QDomElement sequence = doc.createElement( "SEQUENCE" );

        for ( int i = 0; i < m_args.count(); i++ ) {
            m_args.at( i )->buildXML( doc, sequence );
            if ( i < m_args.count()-1 ) {
                QDomElement de = doc.createElement( "TEXT" );
                de.setAttribute( "CHAR", "," );
                sequence.appendChild( de );
            }
        }

        content.appendChild( sequence );
        bracket.appendChild( content );
        element.appendChild( bracket );
    }
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
    RowNode( Q3PtrList<ParserNode> row ) : m_row( row ) { m_row.setAutoDelete( true ); }
    //virtual void output( ostream& stream );
    virtual void buildXML( QDomDocument& doc, QDomElement element );
    uint columns() const { return m_row.count(); }
    void setRequiredColumns( uint requiredColumns ) { m_requiredColumns = requiredColumns; }
private:
    Q3PtrList<ParserNode> m_row;
    uint m_requiredColumns;
};

void RowNode::buildXML( QDomDocument& doc, QDomElement element )
{
    for ( int i = 0; i < m_requiredColumns; i++ ) {
        QDomElement sequence = doc.createElement( "SEQUENCE" );
        if ( i < m_row.count() ) {
            m_row.at( i )->buildXML( doc, sequence );
        }
        else {
            QDomElement de = doc.createElement( "TEXT" );
            de.setAttribute( "CHAR", "0" );
            sequence.appendChild( de );
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
    MatrixNode( Q3PtrList<RowNode> rows ) : m_rows( rows ) { m_rows.setAutoDelete( true ); }
    //virtual void output( ostream& stream );
    virtual void buildXML( QDomDocument& doc, QDomElement element );
    virtual bool isSimple() { return true; }
    uint columns();
    uint rows() { return m_rows.count(); }
private:
    Q3PtrList<RowNode> m_rows;
};

uint MatrixNode::columns()
{
    uint columns = 0;
    for ( uint i = 0; i < m_rows.count(); i++ ) {
        columns = qMax( columns, m_rows.at( i )->columns() );
    }
    return columns;
}

void MatrixNode::buildXML( QDomDocument& doc, QDomElement element )
{
    QDomElement bracket = doc.createElement( "BRACKET" );
    bracket.setAttribute( "LEFT", '(' );
    bracket.setAttribute( "RIGHT", ')' );
    QDomElement content = doc.createElement( "CONTENT" );
    QDomElement sequence = doc.createElement( "SEQUENCE" );

    uint cols = columns();
    QDomElement matrix = doc.createElement( "MATRIX" );
    matrix.setAttribute( "ROWS", m_rows.count() );
    matrix.setAttribute( "COLUMNS", cols );
    for ( uint i = 0; i < m_rows.count(); i++ ) {
        m_rows.at( i )->setRequiredColumns( cols );
        m_rows.at( i )->buildXML( doc, matrix );
        matrix.appendChild( doc.createComment( "end of row" ) );
    }
    sequence.appendChild( matrix );
    content.appendChild( sequence );
    bracket.appendChild( content );
    element.appendChild( bracket );
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
    : m_symbolTable( symbolTable ), m_formula( formula ),
      pos( 0 ), line( 1 ), column( 1 ), m_newlineIsSpace( true )
{
}

FormulaStringParser::~FormulaStringParser()
{
    delete head;
    if ( ParserNode::debugCount != 0 ) {
        kDebug( KFormula::DEBUGID ) << "ParserNode::debugCount = " << ParserNode::debugCount << endl;
    }
}

QDomDocument FormulaStringParser::parse()
{
    nextToken();
    head = parseAssign();
    //head->output( cout );
    if ( !eol() ) {
        error( QString( i18n( "Aborted parsing at %1:%2" ) ).arg( line ).arg( column ) );
    }

    QDomDocument doc = KFormula::Document::createDomDocument();
    QDomElement root = doc.documentElement();
    QDomElement de = doc.createElement( "FORMULA" );
    // here comes the current version of FormulaElement
    //de.setAttribute( "VERSION", "4" );
    head->buildXML( doc, de );
    root.appendChild(de);

    kDebug( 39001 ) << doc.toString() << endl;
    return doc;
}

ParserNode* FormulaStringParser::parseAssign()
{
    ParserNode* lhs = parseExpr();
    for ( ;; ) {
        switch ( currentType ) {
        case ASSIGN: {
            QString c = current;
            nextToken();
            lhs = new AssignNode( c, lhs, parseExpr() );
            break;
        }
        default:
            return lhs;
        }
    }
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
        case INDEX:
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
        PrimaryNode* node = new PrimaryNode( current );
        node->setUnicode( m_symbolTable.unicode( current ) );
        nextToken();
        if ( currentType == LP ) {
            nextToken();
            Q3PtrList<ParserNode> args;
            args.setAutoDelete( false );
            while ( ( currentType != EOL ) && ( currentType != RP ) ) {
                ParserNode* node = parseExpr();
                args.append( node );
                if ( currentType == COMMA ) {
                    nextToken();
                }
            }
            expect( RP, QString( i18n( "'%3' expected at %1:%2" ) ).arg( line ).arg( column ).arg( ")" ) );
            node->setFunctionName( true );
            return new FunctionNode( node, args );
        }
        return node;
    }
    case SUB: {
        nextToken();
        //ParserNode* node = new UnaryMinus( parsePrimary() );
        ParserNode* node = new UnaryMinus( parseTerm() );
        return node;
    }
    case LP: {
        nextToken();
        ParserNode* node = parseExpr();
        expect( RP, QString( i18n( "'%3' expected at %1:%2" ) ).arg( line ).arg( column ).arg( ")" ) );
        return node;
    }
    case LB: {
        nextToken();
        Q3PtrList<RowNode> rows;
        rows.setAutoDelete( false );
        bool innerBrackets = currentType == LB;
        m_newlineIsSpace = innerBrackets;
        while ( ( currentType != EOL ) && ( currentType != RB ) ) {
            if ( innerBrackets ) {
                expect( LB, QString( i18n( "'%3' expected at %1:%2" ) ).arg( line ).arg( column ).arg( "[" ) );
            }
            Q3PtrList<ParserNode> row;
            row.setAutoDelete( false );
            while ( ( currentType != EOL ) && ( currentType != RB ) &&
                    ( innerBrackets || ( currentType != SEMIC && currentType != NEWLINE ) ) ) {
                row.append( parseExpr() );
                if ( currentType == COMMA ) {
                    nextToken();
                }
            }
            if ( innerBrackets ) {
                expect( RB, QString( i18n( "'%3' expected at %1:%2" ) ).arg( line ).arg( column ).arg( "]" ) );
                if ( currentType == COMMA ) {
                    nextToken();
                }
            }
            else {
                if ( currentType != RB ) {
                    if ( currentType == NEWLINE ) {
                        nextToken();
                    }
                    else {
                        expect( SEMIC, QString( i18n( "'%3' expected at %1:%2" ) ).arg( line ).arg( column ).arg( ";" ) );
                    }
                }
            }
            rows.append( new RowNode( row ) );
        }
        m_newlineIsSpace = true;
        expect( RB, QString( i18n( "'%3' expected at %1:%2" ) ).arg( line ).arg( column ).arg( "]" ) );
        MatrixNode* node = new MatrixNode( rows );
        if ( node->columns() == 0 ) {
            error( QString( i18n( "Null columns in Matrix at %1:%2" ) ).arg( line ).arg( column ) );
        }
        if ( node->rows() == 0 ) {
            error( QString( i18n( "Null rows in Matrix at %1:%2" ) ).arg( line ).arg( column ) );
        }
        return node;
    }
    case OTHER: {
        ParserNode* node = new PrimaryNode( current );
        nextToken();
        return node;
    }
    default:
        error( QString( i18n( "Unexpected token at %1:%2" ) ).arg( line ).arg( column ) );
        return new PrimaryNode( "?" );
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
    // We skip any ' or " so that we can parse string literals.
    while ( !eol() && ( m_formula[pos].isSpace() ||
                        ( m_formula[pos] == '"' ) ||
                        ( m_formula[pos] == '\'' ) ) ) {
        if ( m_formula[pos] == '\n' ) {
            line++;
            if ( m_newlineIsSpace ) {
                column = 0;
            }
            else {
                pos++;
                column = 1;
                currentType = NEWLINE;
                return current = "\n";
            }
        }
        pos++; column++;
    }
    if ( eol() ) {
        currentType = EOL;
        return QString::null;
    }
    if ( m_formula[pos].isDigit() || m_formula[pos] == '.' ) {
        uint begin = pos;
        readNumber();
        currentType = NUMBER;
        current = m_formula.mid( begin, pos-begin );
        if ( current[0] == '.' ) {
            current = "0" + current;
        }
        if ( current[current.length()-1] == '.' ) {
            current = current + "0";
        }
        return current;
    }
    else if ( m_formula[pos].isLetter() ) {
        uint begin = pos;
        pos++; column++;
        while ( !eol() && m_formula[pos].isLetter() ) {
            pos++; column++;
        }
        currentType = NAME;
        return current = m_formula.mid( begin, pos-begin );
    }
    else {
        switch ( m_formula[pos].toLatin1() ) {
        case '+':
            pos++; column++;
            currentType = PLUS;
            return current = "+";
        case '-':
            pos++; column++;
            currentType = SUB;
            return current = "-";
        case '*':
            pos++; column++;
            if ( !eol() && m_formula[pos] == '*' ) {
                pos++; column++;
                currentType = POW;
                return current = "**";
            }
            currentType = MUL;
            return current = "*";
        case '/':
            pos++; column++;
            currentType = DIV;
            return current = "/";
        case '^':
            pos++; column++;
            currentType = POW;
            return current = "**";
        case '_':
            pos++; column++;
            currentType = INDEX;
            return current = "_";
        case '(':
            pos++; column++;
            currentType = LP;
            return current = "(";
        case ')':
            pos++; column++;
            currentType = RP;
            return current = ")";
        case '[':
            pos++; column++;
            currentType = LB;
            return current = "[";
        case ']':
            pos++; column++;
            currentType = RB;
            return current = "]";
        case ',':
            pos++; column++;
            currentType = COMMA;
            return current = ",";
        case ';':
            pos++; column++;
            currentType = SEMIC;
            return current = ";";
        case '=':
            pos++; column++;
            currentType = ASSIGN;
            return current = "=";
        default:
            pos++; column++;
            currentType = OTHER;
            return current = m_formula.mid( pos-1, 1 );
        }
    }
}

void FormulaStringParser::readNumber()
{
    bool digitsBeforeDot = m_formula[pos] != '.';

    readDigits();
    if ( pos < m_formula.length()-1 ) {
        QChar ch = m_formula[pos];

        // Look for a dot.
        if ( ch == '.' ) {
            pos++;
            column++;
            ch = m_formula[pos];
            if ( ch.isDigit() ) {
                readDigits();
            }
            else if ( !digitsBeforeDot ) {
                error( QString( i18n( "A single '.' is not a number at %1:%2" ) ).arg( line ).arg( column ) );
                return;
            }
        }

        // there might as well be an exponent
        if ( pos < m_formula.length()-1 ) {
            ch = m_formula[pos];
            if ( ( ch == 'E' ) || ( ch == 'e' ) ) {
                pos++;
                column++;
                ch = m_formula[pos];

                // signs are allowed after the exponent
                if ( ( ( ch == '+' ) || ( ch == '-' ) ) &&
                     ( pos < m_formula.length()-1 ) ) {
                    pos++;
                    column++;
                    ch = m_formula[pos];
                    if ( ch.isDigit() ) {
                        readDigits();
                    }
                    else {
                        pos -= 2;
                        column -= 2;
                        return;
                    }
                }
                else if ( ch.isDigit() ) {
                    readDigits();
                }
                else {
                    pos--;
                    column--;
                }
            }
        }
    }
}


void FormulaStringParser::readDigits()
{
    while ( !eol() && m_formula[pos].isDigit() ) {
        pos++;
        column++;
    }
}

void FormulaStringParser::error( QString err )
{
    kDebug( KFormula::DEBUGID ) << err << " (" << currentType << "; " << current << ")" << endl;
    m_errorList.push_back( err );
}
