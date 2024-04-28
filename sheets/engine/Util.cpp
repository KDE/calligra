/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Util.h"
#include "CellBase.h"
#include "SheetsDebug.h"

#include <QRegularExpression>
#include <cmath>

using namespace Qt::StringLiterals;

int Calligra::Sheets::Util::decodeColumnLabelText(const QString &labelText)
{
    int col = 0;
    const int offset = 'a' - 'A';
    int counterColumn = 0;
    const uint totalLength = labelText.length();
    uint labelTextLength = 0;
    for (; labelTextLength < totalLength; labelTextLength++) {
        const char c = labelText[labelTextLength].toLatin1();
        if (labelTextLength == 0 && c == '$')
            continue; // eat an absolute reference char that could be at the beginning only
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
            break;
    }
    if (labelTextLength == 0) {
        warnSheets << "No column label text found for col:" << labelText;
        return 0;
    }
    for (uint i = 0; i < labelTextLength; i++) {
        const char c = labelText[i].toLatin1();
        counterColumn = (int)::pow(26.0, static_cast<int>(labelTextLength - i - 1));
        if (c >= 'A' && c <= 'Z')
            col += counterColumn * (c - 'A' + 1); // okay here (Werner)
        else if (c >= 'a' && c <= 'z')
            col += counterColumn * (c - 'A' - offset + 1);
    }
    return col;
}

int Calligra::Sheets::Util::decodeRowLabelText(const QString &labelText)
{
    static QRegularExpression rx(u"^(|\\$)([A-Za-z]+)(|\\$)([0-9]+)$"_s);
    auto match = rx.match(labelText);
    if (match.hasMatch())
        return match.captured(4).toInt();
    return 0;
}

bool Calligra::Sheets::Util::isCellReference(const QString &text, int startPos)
{
    int length = text.length();
    if (length < 1 || startPos >= length)
        return false;

    const QChar *data = text.constData();

    if (startPos > 0) {
        data += startPos;
    }

    if (*data == QChar('$', 0)) {
        ++data;
    }

    bool letterFound = false;
    while (1) {
        if (data->isNull()) {
            return false;
        }

        ushort c = data->unicode();
        if ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z'))
            break;

        letterFound = true;
        ++data;
    }

    if (!letterFound) {
        return false;
    }

    if (*data == QChar('$', 0)) {
        ++data;
    }

    bool numberFound = false;
    while (!data->isNull()) {
        ushort c = data->unicode();
        if (c < '0' || c > '9')
            break;
        numberFound = true;
        ++data;
    }

    return numberFound && data->isNull(); // we found the number and reached end
}

// Return true when it's a reference to cell from sheet.
bool Calligra::Sheets::Util::localReferenceAnchor(const QString &_ref)
{
    bool isLocalRef = (_ref.indexOf("http://") != 0 && _ref.indexOf("https://") != 0 && _ref.indexOf("mailto:") != 0 && _ref.indexOf("ftp://") != 0
                       && _ref.indexOf("file:") != 0);
    return isLocalRef;
}

bool Calligra::Sheets::Util::isCellnameCharacter(const QChar &c)
{
    return c.isDigit() || c.isLetter() || c == '$';
}

// used by adjustFormulaReference
static void
replaceFormulaReference(int referencedRow, int referencedColumn, int thisRow, int thisColumn, QString &result, int cellReferenceStart, int cellReferenceLength)
{
    const QString ref = result.mid(cellReferenceStart, cellReferenceLength);
    QRegularExpression rx(u"^(|\\$)[A-Za-z]+(|\\$)[0-9]+$"_s);
    auto match = rx.match(ref);
    if (match.hasMatch()) {
        int c = Calligra::Sheets::Util::decodeColumnLabelText(ref);
        int r = Calligra::Sheets::Util::decodeRowLabelText(ref);
        if (match.captured(1) != "$") // absolute or relative column?
            c += thisColumn - referencedColumn;
        if (match.captured(2) != "$") // absolute or relative row?
            r += thisRow - referencedRow;
        result.replace(cellReferenceStart,
                       cellReferenceLength,
                       match.captured(1) + Calligra::Sheets::CellBase::columnName(c) + match.captured(2) + QString::number(r));
    }
}

QString Calligra::Sheets::Util::adjustFormulaReference(const QString &formula, int referencedRow, int referencedColumn, int thisRow, int thisColumn)
{
    QString result = formula;
    if (result.isEmpty())
        return QString();
    enum { InStart, InCellReference, InString, InSheetOrAreaName } state;
    state = InStart;
    int cellReferenceStart = 0;
    for (int i = 1; i < result.length(); ++i) {
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
                    replaceFormulaReference(referencedRow, referencedColumn, thisRow, thisColumn, result, cellReferenceStart, i - cellReferenceStart);
                }
                state = InStart;
                --i; // decrement again to handle the current char in the InStart-switch.
            }
            break;
        };
    }
    if (state == InCellReference) {
        replaceFormulaReference(referencedRow, referencedColumn, thisRow, thisColumn, result, cellReferenceStart, result.length() - cellReferenceStart);
    }
    return result;
}
