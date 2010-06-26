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


#ifndef __EDITORS_H__
#define __EDITORS_H__

#include <vector>

#include <QSyntaxHighlighter>
#include <QWidget>
//Added by qt3to4:
#include <QFocusEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QResizeEvent>

#include <kcombobox.h>
#include <klineedit.h>
#include <ksharedptr.h>
#include <KTextEdit>

#include "kspread_export.h"

class KTextEdit;
class QFont;
class QAbstractButton;
class QTextEdit;
class QListWidgetItem;

class KoViewConverter;

namespace KSpread
{
class Canvas;
class Cell;
class CellEditor;
class CellToolBase;
class LocationEditWidget;
class Region;
class Selection;
class TextEdit;
class Tokens;
class View;


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



/**
* Provides autocompletition facilities in formula editors.
* When the user types in the first few characters of a
* function name in a CellEditor which has a FunctionCompletion
* object installed on it, the FunctionCompletion object
* creates and displays a list of possible names which the user
* can select from. If the user selects a function name from the list,
* the @ref FunctionCompletion::selectedCompletion() signal is emitted
*/
class FunctionCompletion : public QObject
{
    Q_OBJECT

public:

    FunctionCompletion(CellEditor* editor);
    ~FunctionCompletion();

    /**
    * Handles various keyboard and mouse actions which may occur on the autocompletion popup list
    */
    bool eventFilter(QObject *o, QEvent *e);

    /**
    * Populates the autocompletion list box with the specified choices and shows it so that the user can view and select a function name.
    * @param choices A list of possible function names which match the characters that the user has already entered.
    */
    void showCompletion(const QStringList &choices);

public slots:
    /**
    * Hides the autocompletion list box if it is visible and emits the @ref selectedCompletion signal.
    */
    void doneCompletion();

private slots:
    void itemSelected(QListWidgetItem* item = 0);

signals:
    /**
    * Emitted, if the user selects a function name from the list.
    */
    void selectedCompletion(const QString& item);

private:
    class Private;
    Private * const d;
    FunctionCompletion(const FunctionCompletion&);
    FunctionCompletion& operator=(const FunctionCompletion&);
};



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
    void functionAutoComplete(const QString& item);

private:
    Q_DISABLE_COPY(CellEditor)

    class Private;
    Private * const d;
};



/**
 * \class LocationComboBox
 * \ingroup UI
 * The combobox, that shows the address string of the current cell selection.
 *
 * Depending on the sheet settings the address is displayed in normal form
 * (e.g. A1 or B1:C3) or in LC (Line/Column) form (e.g. L1xC1 or 3Lx2C).
 */
class LocationComboBox : public KComboBox
{
    Q_OBJECT
public:
    LocationComboBox(CellToolBase *cellTool, QWidget *parent = 0);

    void addCompletionItem(const QString &_item);
    void removeCompletionItem(const QString &_item);

    /**
     * Updates the address string according to the current cell selection
     * and the current address mode (normal or LC mode).
     */
    void updateAddress();

public slots:
    void slotAddAreaName( const QString & );
    void slotRemoveAreaName( const QString & );

protected: // reimplementations
    virtual void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
    void slotActivateItem();

private:
    bool activateItem();

private:
    CellToolBase *m_cellTool;
    KCompletion completionList;
};



class ExternalEditor : public KTextEdit
{
    Q_OBJECT
public:
    ExternalEditor(QWidget* parent = 0);
    ~ExternalEditor();

    virtual QSize sizeHint() const;

    void setCellTool(CellToolBase* cellTool);

Q_SIGNALS:
    void textChanged(const QString &text);

public Q_SLOTS:
    void applyChanges();
    void discardChanges();
    void setText(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private slots:
    void slotTextChanged();
    void slotCursorPositionChanged();

private:
    Q_DISABLE_COPY(ExternalEditor)

    class Private;
    Private * const d;
};

#if 0 // KSPREAD_DISCARD_FORMULA_BAR
/**
* The widget that appears above the sheet and allows to
* edit the cells content.
*/
class EditWidget : public KLineEdit
{
    Q_OBJECT
public:
    EditWidget(QWidget *parent, Canvas *canvas,
               QAbstractButton *cancelButton, QAbstractButton *okButton);

    virtual void setText(const QString& t);

    // Go into edit mode (enable the buttons)
    void setEditMode(bool mode);

    void showEditWidget(bool _show);

public slots:
    void slotAbortEdit();
    void slotDoneEdit();

protected:
    virtual void keyPressEvent(QKeyEvent* _ev);
    virtual void focusOutEvent(QFocusEvent* ev);

private:
    QAbstractButton* m_pCancelButton;
    QAbstractButton* m_pOkButton;
    Canvas* m_pCanvas;
    bool m_isArray;
};
#endif


/**
 * A minimizable line edit for choosing cell regions.
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
class KSPREAD_EXPORT RegionSelector : public QWidget
{
    Q_OBJECT

public:
    enum SelectionMode { SingleCell = 0, MultipleCells = 1 }; // TODO Stefan: merge with Selection::Mode
    enum DisplayMode { Widget, Dialog };

    RegionSelector(QWidget* parent = 0);
    ~RegionSelector();

    void setSelectionMode(SelectionMode mode);
    void setSelection(Selection* selection);
    void setDialog(QDialog* dialog);
    void setLabel(const QString& text);

    KTextEdit* textEdit() const;

protected:
    bool eventFilter(QObject* obj, QEvent* event);

protected Q_SLOTS:
    void switchDisplayMode(bool state);
    void choiceChanged();

private:
    Q_DISABLE_COPY(RegionSelector)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif
