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
class CellEditor : public QWidget
{
    Q_OBJECT
public:

    /**
    * Creates a new CellEditor.
    * @param parent The parent widget.
    * @param selection The Selection object to associate this cell text editor with
    * @param captureAllKeyEvents Controls whether or not the text editor swallows arrow key events or sends them to the parent canvas instead.  If this is set to true, pressing the arrow keys will navigate backwards and forwards through the text in the editor.  If it is false, the key events will be sent to the parent canvas which will change the cell being edited (depending on the direction of the arrow pressed).  Generally this should be set to true if the user double clicks on the cell to edit it, and false if the user initiates editing by typing whilst the cell is selected.
    * @param _name This parameter is sent to the QObject constructor
    */
    explicit CellEditor(QWidget* parent, Selection* selection, bool captureAllKeyEvents = false);
    ~CellEditor();

    const Cell& cell() const;
    Selection* selection() const;

    void handleKeyPressEvent(QKeyEvent* _ev);
    void handleInputMethodEvent(QInputMethodEvent  * _ev);
    void setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter);

    int cursorPosition() const;
    void setCursorPosition(int pos);

    /** wrapper to KTextEdit::text() */
    QString text() const;

    /** wrapper to KTextEdit::cut() */
    void cut();
    /** wrapper to KTextEdit::paste() */
    void paste();
    /** wrapper to KTextEdit::copy() */
    void copy();

    QPoint globalCursorPosition() const;

    bool checkChoice();
    void setCheckChoice(bool b);

    void updateChoice();
    void setUpdateChoice(bool);

    void setCursorToRange(uint);

Q_SIGNALS:
    void textChanged(const QString &text);
    void modificationChanged(bool changed);

public slots:
    void setText(const QString& text, int cursorPos = -1);

private slots:
    void  slotTextChanged();
    void  slotCompletionModeChanged(KGlobalSettings::Completion _completion);
    void  slotCursorPositionChanged();
    void  slotSelectionDestroyed();

protected:
    void resizeEvent(QResizeEvent*);
    /**
     * Steals some key events from the KLineEdit and sends
     * it to the KSpread::Canvas ( its parent ) instead.
     */
    bool eventFilter(QObject* o, QEvent* e);

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
 * LocationComboBox
 */
class LocationComboBox : public KComboBox
{
    Q_OBJECT
public:
    LocationComboBox(CellToolBase *cellTool, QWidget *parent = 0);

public slots:
    void slotAddAreaName(const QString &);
    void slotRemoveAreaName(const QString &);
private:
    LocationEditWidget *m_locationWidget;
};



/**
 * A widget that allows the user to enter an arbitrary
 * cell location to goto or cell selection to highlight
 */
class LocationEditWidget : public KLineEdit
{
    Q_OBJECT
public:
    LocationEditWidget(CellToolBase *cellTool, QWidget *parent = 0);

    void addCompletionItem(const QString &_item);
    void removeCompletionItem(const QString &_item);

private slots:
    void slotActivateItem();

protected:
    virtual void keyPressEvent(QKeyEvent * _ev);
private:
    CellToolBase *m_cellTool;
    KCompletion completionList;
    bool activateItem();
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
