/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include "FormulaParser.h"
#include "XlsxXmlWorksheetReader_p.h"

#include <kspread/Util.h>

QString MSOOXML::convertFormula(const QString& formula)
{
    if (formula.isEmpty())
        return QString();
    enum { Start, InArguments, InParenthesizedArgument, InString, InSheetOrAreaName } state;
    state = Start;
    QString result = '=' + formula;
    for(int i = 1; i < result.length(); ++i) {
        QChar ch = result[i];
        switch (state) {
        case Start:
            if(ch == '(')
                state = InArguments;
            break;
        case InArguments:
            if (ch == '"')
                state = InString;
            else if (ch.unicode() == '\'')
                state = InSheetOrAreaName;
            else if (ch == ',')
                result[i] = ';'; // replace argument delimiter
            else if (ch == '(' && !result[i-1].isLetterOrNumber())
                state = InParenthesizedArgument;
            break;
        case InParenthesizedArgument:
            if (ch == ',')
                result[i] = '~'; // union operator
            else if (ch == ')')
                state = InArguments;
            break;
        case InString:
            if (ch == '"')
                state = InArguments;
            break;
        case InSheetOrAreaName:
            if (ch == '\'')
                state = InArguments;
            break;
        };
    };
    return result;
}

static bool isCellnameCharacter(const QChar &c)
{
    return c.isDigit() || c.isLetter() || c == '$';
}

static void replaceFormulaReference(Cell* referencedCell, Cell* thisCell, QString &result, int cellReferenceStart, int cellReferenceLength)
{
    const QString ref = result.mid(cellReferenceStart, cellReferenceLength);
    QRegExp rx("(|\\$)[A-Za-z]+[0-9]+");
    if (rx.exactMatch(ref)) {
        const int c = KSpread::Util::decodeColumnLabelText(ref) + thisCell->column - referencedCell->column;
        const int r = KSpread::Util::decodeRowLabelText(ref) + thisCell->row - referencedCell->row;
        result = result.replace(cellReferenceStart,
                                cellReferenceLength, 
                                KSpread::Util::encodeColumnLabelText(c) + QString::number(r) );
    }
}

QString MSOOXML::convertFormulaReference(Cell* referencedCell, Cell* thisCell)
{
    QString result = referencedCell->formula;
    if (result.isEmpty())
        return QString();
    enum { InStart, InCellReference, InString, InSheetOrAreaName } state;
    state = InStart;
    int cellReferenceStart = 0;
    for(int i = 1; i < result.length(); ++i) {
        QChar ch = result[i];
        switch (state) {
        case InStart:
            if (ch == '"')
                state = InString;
            else if (ch.unicode() == '\'')
                state = InSheetOrAreaName;
            else if (isCellnameCharacter(ch)) {
                state = InCellReference;
                cellReferenceStart = i;
            }
            break;
        case InString:
            if (ch == '"')
                state = InStart;
            break;
        case InSheetOrAreaName:
            if (ch == '\'')
                state = InStart;
            break;
        case InCellReference:
            if (!isCellnameCharacter(ch)) {
                // We need to update cell-references according to the position of the referenced cell and this
                // cell. This means that if the referenced cell is for example at C5 and contains the formula
                // "=SUM(K22)" and if thisCell is at E6 then thisCell will get the formula "=SUM(L23)".
                if (ch != '(') /* skip formula-names */ {
                    replaceFormulaReference(referencedCell, thisCell, result, cellReferenceStart, i - cellReferenceStart);
                }
                state = InStart;
            }
            break;
        };
    };
    if(state == InCellReference) {
        replaceFormulaReference(referencedCell, thisCell, result, cellReferenceStart, result.length() - cellReferenceStart);
    }
    return result;
}
