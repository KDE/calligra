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

#ifndef FSPARSER_H
#define FSPARSER_H

#include <qstring.h>
#include <qstringlist.h>
#include <qdom.h>

namespace KFormula { class SymbolTable; }
class ParserNode;

/**
 * The parser for simple (C style) formula strings.
 */
class FormulaStringParser {
public:
    FormulaStringParser( const KFormula::SymbolTable& symbolTable, QString formula );
    ~FormulaStringParser();

    QDomDocument parse();

    QStringList errorList() const { return m_errorList; }

private:

    enum TokenType { NUMBER, NAME, PLUS, SUB, MUL, DIV, POW, INDEX, LP, RP, LB, RB,
                     ASSIGN, COMMA, SEMIC, NEWLINE, OTHER, EOL };

    ParserNode* parseAssign();
    ParserNode* parseExpr();
    ParserNode* parseTerm();
    ParserNode* parsePower();
    ParserNode* parsePrimary();
    void expect( TokenType type, QString msg );

    QString nextToken();
    bool eol() { return m_formula.length() == pos; }

    void readNumber();
    void readDigits();

    void error( QString err );
    QStringList m_errorList;

    const KFormula::SymbolTable& m_symbolTable;
    QString m_formula;
    uint pos;
    uint line;
    uint column;
    bool m_newlineIsSpace;

    TokenType currentType;
    QString current;

    ParserNode* head;
};

#endif
