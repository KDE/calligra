/* This file is part of the KDE project

   SPDX-FileCopyrightText: 1999-2006 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FORMULA_EDITOR_HIGHLIGHTER
#define CALLIGRA_SHEETS_FORMULA_EDITOR_HIGHLIGHTER

#include <QSyntaxHighlighter>

class QTextEdit;

namespace Calligra
{
namespace Sheets
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
    FormulaEditorHighlighter(QTextEdit *textEdit, Selection *selection);
    ~FormulaEditorHighlighter() override;

    /**
     * Called automatically by KTextEditor to highlight text when modified.
     */
    void highlightBlock(const QString &text) override;
    /**
     *
     */
    const Tokens &formulaTokens() const;
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
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FORMULA_EDITOR_HIGHLIGHTER
