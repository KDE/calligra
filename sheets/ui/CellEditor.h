/* This file is part of the KDE project

   Copyright 1999-2006 The KSpread Team <calligra-devel@kde.org>

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


#ifndef CALLIGRA_SHEETS_CELL_EDITOR
#define CALLIGRA_SHEETS_CELL_EDITOR

#include <KCompletion>
#include <ktextedit.h>

#include "sheets_common_export.h"
#include "CellEditorBase.h"

#include <QCompleter>
#include <QAbstractItemModel>
#include <QThread>
#include <QHash>
class KoViewConverter;

namespace Calligra
{
namespace Sheets
{
class CellToolBase;
class Selection;

/**
 * class CellEditor
 */
class CellEditor : public KTextEdit, public CellEditorBase
{
    Q_OBJECT
public:
    /**
    * Creates a new CellEditor.
    * \param cellTool the cell tool
    * \param wordList the word list
    * \param parent the parent widget
    */
    explicit CellEditor(CellToolBase *cellTool, QHash<int, QString> &wordList, QWidget *parent = 0);
    ~CellEditor() override;

    Selection* selection() const;

    void setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter) override;

    int cursorPosition() const override;
    void setCursorPosition(int pos) override;

    bool captureArrowKeys() const override;
    void setCaptureArrowKeys(bool capture) override;

    QPoint globalCursorPosition() const;
    QAbstractItemModel *model();

    /**
     * Replaces the current formula token(/reference) with the name of the
     * selection's active sub-region name.
     * This is called after selection changes to sync the formula expression.
     */
    void selectionChanged() override;

    /**
     * Activates the sub-region belonging to the \p index 'th range.
     */
    void setActiveSubRegion(int index) override;

    // CellEditorBase interface
    QWidget* widget() override { return this; }
    void cut() override { KTextEdit::cut(); }
    void copy() override { KTextEdit::copy(); }
    void paste() override { KTextEdit::paste(); }
    QString toPlainText() const override { return KTextEdit::toPlainText(); }
Q_SIGNALS:
    void textChanged(const QString &text);

public Q_SLOTS:
    void setText(const QString& text, int cursorPos = -1) override;

    /**
     * Permutes the fixation of the reference, at which the editor's cursor
     * is placed. It is only active, if a formula is edited.
     */
    void permuteFixation();
    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

private Q_SLOTS:
    void  slotTextChanged();
    void  slotCompletionModeChanged(KCompletion::CompletionMode _completion);
    void  slotCursorPositionChanged();
    void insertCompletion(const QString &completion);

protected: // reimplementations
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    Q_DISABLE_COPY(CellEditor)
    QString textUnderCursor() const;

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_EDITOR
