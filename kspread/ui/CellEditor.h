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


#ifndef KSPREAD_CELL_EDITOR
#define KSPREAD_CELL_EDITOR

#include <KGlobalSettings>
#include <KTextEdit>

#include "kspread_export.h"

class KoViewConverter;

namespace KSpread
{
class CellToolBase;
class Selection;

/**
 * class CellEditor
 */
class CellEditor : public KTextEdit
{
    Q_OBJECT
public:
    /**
    * Creates a new CellEditor.
    * \param cellTool the cell tool
    * \param parent the parent widget
    */
    explicit CellEditor(CellToolBase *cellTool, QWidget *parent = 0);
    ~CellEditor();

    Selection* selection() const;

    void setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter);

    int cursorPosition() const;
    void setCursorPosition(int pos);

    QPoint globalCursorPosition() const;

    /**
     * Replaces the current formula token(/reference) with the name of the
     * selection's active sub-region name.
     * This is called after selection changes to sync the formula expression.
     */
    void selectionChanged();

    /**
     * Activates the sub-region belonging to the \p index 'th range.
     */
    void setActiveSubRegion(int index);

Q_SIGNALS:
    void textChanged(const QString &text);

public slots:
    void setText(const QString& text, int cursorPos = -1);

    /**
     * Permutes the fixation of the reference, at which the editor's cursor
     * is placed. It is only active, if a formula is edited.
     */
    void permuteFixation();

private slots:
    void  slotTextChanged();
    void  slotCompletionModeChanged(KGlobalSettings::Completion _completion);
    void  slotCursorPositionChanged();

protected: // reimplementations
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);

protected slots:
    void checkFunctionAutoComplete();
    void triggerFunctionAutoComplete();
    void functionAutoComplete( const QString& item );

private:
    Q_DISABLE_COPY( CellEditor )

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_CELL_EDITOR
