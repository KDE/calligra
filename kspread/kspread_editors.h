/* This file is part of the KDE project

   Copyright 1999-2004 The KSpread Team <koffice-devel@kde.org>

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
#include <qwidget.h>

#include <kcombobox.h>
#include <klineedit.h>


class KTextEdit;
class QFont;
class QButton;
class QTextCursor;

namespace KSpread
{
class Cell;
class HighlightRange;
class Sheet;
class Canvas;
class View;
class TextEditor;
class LocationEditWidget;


class CellEditor : public QWidget
{
    Q_OBJECT
public:
    CellEditor( Cell*, Canvas* _parent = 0, const char* _name = 0 );
    ~CellEditor();

    Cell* cell()const { return m_pCell; }

    virtual void handleKeyPressEvent( QKeyEvent* _ev ) = 0;
    virtual void handleIMEvent( QIMEvent * _ev ) = 0;
    virtual void setEditorFont(QFont const & font, bool updateSize) = 0;
    virtual QString text() const = 0;
    virtual void setText(QString text) = 0;
    virtual int cursorPosition() const = 0;
    virtual void setCursorPosition(int pos) = 0;
    // virtual void setFocus() = 0;
    virtual void insertFormulaChar(int c) = 0;
    virtual void cut(){};
    virtual void paste(){};
    virtual void copy(){};
    Canvas* canvas()const { return m_pCanvas; }

private:
    Cell* m_pCell;
    Canvas* m_pCanvas;
};

/**
	Colours cell references in formulas.  Installed by TextEditor instances in
	the constructor.
 */
class FormulaEditorHighlighter : public QSyntaxHighlighter
{
	//Q_OBJECT

	public:
	/**
	 * Constructs a KSpreadTextEditorHighlighter to colour-code cell references in a QTextEdit.
	 *
	 * @param textEdit The QTextEdit widget which the highlighter should operate on
	 * @param sheet The active KSpreadSheet object
	 */
		FormulaEditorHighlighter(QTextEdit* textEdit,Sheet* sheet);
		virtual ~FormulaEditorHighlighter(){}


	/**
	* Called automatically by KTextEditor to highlight text when modified.
	*/
		virtual int highlightParagraph(const QString& text, int endStateOfLastPara);
		
	/**
	*
	*
	*/
		void getReferences(std::vector<KSpread::HighlightRange>* ranges);


	/**	Set spread sheet used for cell reference checking
		 *	This should be called if the active spreadsheet is changed after the highlighter is constructed.
	 */
		void setSheet(Sheet* sheet) {_sheet=sheet;}
		Sheet* sheet() {return _sheet;}

	/**
	*	Returns true if the cell references in the formula have changed since the last call
	*	to referencesChanged().
	*	The first call always returns true.
	 */
		bool referencesChanged();

	protected:

	//Array of references already found in text.  This is so that all references to the same cell
	//share the same colour
		std::vector<QString> _refs;

	//These are the default colours used to
	//highlight cell references
		std::vector<QColor> _colors;

	//Source for cell reference checking
		Sheet* _sheet;

	//Have cell references changed since last call to referencesChanged()?
		bool _refsChanged;
};


/**
* Provides autocompletition facilities in formula editors.
* When the user types in the first few characters of a function name in a @ref KSpreadTextEditor which has a @ref FunctionCompletion
* object installed on it, the @ref FunctionCompletion object creates and displays a list of possible names which the user can select from.
* If the user selects a function name from the list, the @ref selectedCompletion signal is emitted
*/
class FunctionCompletion : public QObject
{
    Q_OBJECT
  
public:

    FunctionCompletion( TextEditor* editor );
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
    
signals:
    void selectedCompletion( const QString& item );
    
private:
    class Private;
    Private* d;
    FunctionCompletion( const FunctionCompletion& );
    FunctionCompletion& operator=( const FunctionCompletion& );    
};


class TextEditor : public CellEditor
{
    Q_OBJECT
public:

    /**
    * Creates a new KSpreadTextEditor.
    * @param cell The spreadsheet cell to associate the cell text editor with
    * @param _parent The @ref KSpreadCanvas object to associate this cell text editor with
    * @param captureAllKeyEvents Controls whether or not the text editor swallows arrow key events or sends them to the parent canvas instead.  If this is set to true, pressing the arrow keys will navigate backwards and forwards through the text in the editor.  If it is false, the key events will be sent to the parent canvas which will change the cell being edited (depending on the direction of the arrow pressed).  Generally this should be set to true if the user double clicks on the cell to edit it, and false if the user initiates editing by typing whilst the cell is selected.
    * @param _name This parameter is sent to the QObject constructor
    */
    TextEditor( KSpread::Cell* cell, Canvas* _parent = 0, bool captureAllKeyEvents = false, const char* _name = 0 );
    ~TextEditor();


    virtual void handleKeyPressEvent( QKeyEvent* _ev );
    virtual void handleIMEvent( QIMEvent * _ev );
    virtual void setEditorFont(QFont const & font, bool updateSize);
    virtual QString text() const;
    virtual void setText(QString text);
    virtual int cursorPosition() const;
    virtual void setCursorPosition(int pos);
    // virtual void setFocus();
    virtual void insertFormulaChar(int c);
    virtual void cut();
    virtual void paste();
    virtual void copy();

    QPoint globalCursorPosition() const;

    //Colour-code references to cells in formulas
    void colorCellReferences();

    bool checkChoose();
    void blockCheckChoose( bool b ) { m_blockCheck = b; }
    bool sizeUpdate() const { return m_sizeUpdate; }

private slots:
    void  slotTextChanged();
    void  slotCompletionModeChanged(KGlobalSettings::Completion _completion);
    void  slotCursorPositionChanged(int para,int pos);
    void  slotTextCursorChanged(QTextCursor*);

protected:
    void resizeEvent( QResizeEvent* );
    /**
     * Steals some key events from the QLineEdit and sends
     * it to the @ref Cancvas ( its parent ) instead.
     */
    bool eventFilter( QObject* o, QEvent* e );

protected slots:   
    void checkFunctionAutoComplete();
    void triggerFunctionAutoComplete();
    void functionAutoComplete( const QString& item );


private:
    //QLineEdit* m_pEdit;
   // KLineEdit* m_pEdit;
	KTextEdit* m_pEdit;

    bool m_captureAllKeyEvents;
    bool m_blockCheck;
    bool m_sizeUpdate;
    uint m_length;
    int  m_fontLength;


    FormulaEditorHighlighter* m_highlighter;


    FunctionCompletion* functionCompletion;
    QTimer* functionCompletionTimer;
    QPoint globalCursorPos;
};


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
signals:
    void gotoLocation( int, int );
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
                       QButton *cancelButton, QButton *okButton);

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
    QButton* m_pCancelButton;
    QButton* m_pOkButton;
    Canvas* m_pCanvas;
    bool isArray;
};

} // namespace KSpread

#endif
