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
 * Boston, MA 02110-1301, USA.
*/


#ifndef __kspread_editors_h__
#define __kspread_editors_h__

#include <vector>

#include <qsyntaxhighlighter.h>
#include <QWidget>
//Added by qt3to4:
#include <QFocusEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QResizeEvent>

#include <kcombobox.h>
#include <klineedit.h>
#include <ksharedptr.h>

class KTextEdit;
class QFont;
class QAbstractButton;
class QTextCursor;
class QTextEdit;

namespace KSpread
{
class Canvas;
class Cell;
class CellEditor;
class LocationEditWidget;
class Region;
class Sheet;
class Tokens;
class View;


/**
 * Colours cell references in formulas.  Installed by CellEditor instances in
 * the constructor.
 */
class FormulaEditorHighlighter : public QSyntaxHighlighter
{
public:
  /**
   * Constructs a FormulaHighlighter to colour-code cell references in a QTextEdit.
   *
   * @param textEdit The QTextEdit widget which the highlighter should operate on
   * @param canvas The Canvas object
   */
  FormulaEditorHighlighter(QTextEdit* textEdit, Canvas* canvas);
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
   * Returns true if any of the ranges or cells in the formula have changed since the
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
  class Private;
  Private* d;
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

    FunctionCompletion( CellEditor* editor );
    ~FunctionCompletion();

    /**
    * Handles various keyboard and mouse actions which may occur on the autocompletion popup list
    */
    bool eventFilter( QObject *o, QEvent *e );

    /**
    * Hides the autocompletion list box if it is visible and emits the @ref selectedCompletion signal.
    */
    void doneCompletion();

    /**
    * Populates the autocompletion list box with the specified choices and shows it so that the user can view and select a function name.
    * @param choices A list of possible function names which match the characters that the user has already entered.
    */
    void showCompletion( const QStringList &choices );

private slots:
    void itemSelected( const QString& item );

signals:
    /**
    * Emitted, if the user selects a function name from the list.
    */
    void selectedCompletion( const QString& item );

private:
    class Private;
    Private* d;
    FunctionCompletion( const FunctionCompletion& );
    FunctionCompletion& operator=( const FunctionCompletion& );
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
    * @param cell The spreadsheet cell to associate the cell text editor with
    * @param _parent The Canvas object to associate this cell text editor with
    * @param captureAllKeyEvents Controls whether or not the text editor swallows arrow key events or sends them to the parent canvas instead.  If this is set to true, pressing the arrow keys will navigate backwards and forwards through the text in the editor.  If it is false, the key events will be sent to the parent canvas which will change the cell being edited (depending on the direction of the arrow pressed).  Generally this should be set to true if the user double clicks on the cell to edit it, and false if the user initiates editing by typing whilst the cell is selected.
    * @param _name This parameter is sent to the QObject constructor
    */
    CellEditor( Cell* cell, Canvas* _parent = 0, bool captureAllKeyEvents = false, const char* _name = 0 );
    ~CellEditor();

    Cell* cell() const;
    Canvas* canvas() const;

    void handleKeyPressEvent( QKeyEvent* _ev );
    void handleIMEvent( QInputMethodEvent  * _ev );
    void setEditorFont(QFont const & font, bool updateSize);

    int cursorPosition() const;
    void setCursorPosition(int pos);

    void setText(QString text);

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

private slots:
    void  slotTextChanged();
    void  slotCompletionModeChanged(KGlobalSettings::Completion _completion);
    void  slotCursorPositionChanged(int para,int pos);
    void  slotTextCursorChanged();

protected:
    void resizeEvent( QResizeEvent* );
    /**
     * Steals some key events from the QLineEdit and sends
     * it to the KSpread::Canvas ( its parent ) instead.
     */
    bool eventFilter( QObject* o, QEvent* e );

protected slots:
    void checkFunctionAutoComplete();
    void triggerFunctionAutoComplete();
    void functionAutoComplete( const QString& item );

private:
    class Private;
    Private* d;
};



/**
 * ComboboxLocationEditWidget
 */
class ComboboxLocationEditWidget : public KComboBox
{
    Q_OBJECT
public:
    ComboboxLocationEditWidget( QWidget *_parent, View * _canvas );

public slots:
    void slotAddAreaName( const QString & );
    void slotRemoveAreaName( const QString & );
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
    LocationEditWidget( QWidget *_parent, View * _canvas );
    View * view() const { return m_pView;}

    void addCompletionItem( const QString &_item );
    void removeCompletionItem( const QString &_item );

private slots:
    void slotActivateItem();

protected:
    virtual void keyPressEvent( QKeyEvent * _ev );
private:
    View * m_pView;
    KCompletion completionList;
    bool activateItem();
};



/**
 * The widget that appears above the sheet and allows to
 * edit the cells content.
 */
class EditWidget : public QLineEdit
{
    Q_OBJECT
public:
    EditWidget( QWidget *parent, Canvas *canvas,
                QAbstractButton *cancelButton, QAbstractButton *okButton);

    virtual void setText( const QString& t );

    // Go into edit mode (enable the buttons)
    void setEditMode( bool mode );

    void showEditWidget(bool _show);
public slots:
    void slotAbortEdit();
    void slotDoneEdit();

protected:
    virtual void keyPressEvent ( QKeyEvent* _ev );
    virtual void focusOutEvent( QFocusEvent* ev );

private:
    QAbstractButton* m_pCancelButton;
    QAbstractButton* m_pOkButton;
    Canvas* m_pCanvas;
    bool isArray;
};

} // namespace KSpread

#endif
