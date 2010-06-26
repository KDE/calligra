/* This file is part of the KDE project

   Copyright 1999-2006 The KSpread Team <koffice-devel@kde.org>

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
   Boston, MA 02110-1301, USA.
*/


#ifndef KSPREAD_FORMULA_EDITOR_HIGHLIGHTER
#define KSPREAD_FORMULA_EDITOR_HIGHLIGHTER

#include <QSyntaxHighlighter>

class QTextEdit;

namespace KSpread
{
class Selection;
class Tokens;

/**
 * Colors cell references in formulas.  Installed by CellEditor instances in
 * the constructor.
 */
class FormulaEditorHighlighter : public QSyntaxHighlighter
{
public:
    /**
     * Constructs a FormulaHighlighter to color-code cell references in a QTextEdit.
     *
     * @param textEdit The QTextEdit widget which the highlighter should operate on
     * @param selection The Selection object
     */
    FormulaEditorHighlighter(QTextEdit* textEdit, Selection* selection);
    virtual ~FormulaEditorHighlighter();


    /**
     * Called automatically by KTextEditor to highlight text when modified.
     */
    virtual void highlightBlock(const QString& text);
    /**
     *
     */
    const Tokens& formulaTokens() const;
    /**
     *
     */
    uint rangeCount() const;
    /**
     * Returns true if any of the ranges or cells in the Formula.have changed since the
     * last call to @ref FormulaEditorHighlighter::rangeChanged()
     */
    bool rangeChanged() const;

    /**
     * Sets the highlighter's range changed flag to false.
     */
    void resetRangeChanged();



protected:
    /**
    * Returns the position of the brace matching the one found at position pos
    */
    int findMatchingBrace(int pos);
    /**
    * Examines the brace (Token::LeftPar or Token::RightPar) operator token at the given index in the token vector
    * ( as returned by formulaTokens() ) and if the cursor is next to it, the token plus any matching brace will be highlighted
    */
    void handleBrace(uint index);

private:
    Q_DISABLE_COPY(FormulaEditorHighlighter)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_FORMULA_EDITOR_HIGHLIGHTER
