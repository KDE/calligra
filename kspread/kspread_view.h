/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 1999-2003 Laurent Montel <montel@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002-2003 John Dailey <dailey@vt.edu>
             (C) 1999-2003 David Faure <faure@kde.org>
             (C) 1999-2001 Simon Hausmann <hausmann@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KSPREAD_VIEW
#define KSPREAD_VIEW

class QScrollBar;
class QButton;

class KSpreadView;
class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadScripts;
class KSpreadSheet;
class KSpreadDoc;
class KSpreadPaperLayout;
class KSpreadChildPicture;
class KSpreadChildFrame;
class KSpreadShell;
class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadChild;
class KSpreadCell;
class KSpreadLocationEditWidget;
class KoDocumentEntry;
class KoCharSelectDia;
class KStatusBarLabel;
class KSpreadSpell;
class KSpreadMacroUndoAction;
class KSpreadInsertHandler;

class DCOPObject;
class KReplace;
class KFind;

namespace KSpread
{
class TabBar;
};

#include <qptrlist.h>
#include <qpoint.h>
#include <qguardedptr.h>
#include <qstringlist.h>

#include <koView.h>
#include <koDocument.h>
#include <koToolBox.h>
#include <kdatatool.h>
#include <kglobalsettings.h>

#include "kspread_selection.h"


class ViewPrivate;

/**
 * KSpreadView is used to display a spreadsheet document and provide
 * the interface for the user to perform editing and data analysis.
 *
 * A view consists of several parts: canvas to display cells in a sheet,
 * column header to show columns, row header to show rows, horizontal
 * and vertical scrollbars for navigation, location editor to show
 * marker and/or selection, tab bar to select active worksheet and line
 * editor to display and edit cell contents.
 *
 */
class KSpreadView : public KoView
{
    friend class KSpreadCanvas;

    Q_OBJECT
public:

    /**
     * Creates a new view.
     */
    KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc *_doc );

    /**
     * Destroy the view.
     */
    ~KSpreadView();

    /**
     * Returns the corresponding document for this view.
     */
    KSpreadDoc* doc();

    /**
     * Returns the canvas of the view.
     */
    KSpreadCanvas* canvasWidget() const;

    /**
     * Returns the column header.
     */
    KSpreadHBorder* hBorderWidget() const;

    /**
     * Returns the row header.
     */
    KSpreadVBorder* vBorderWidget() const;

    /**
     * Returns the horizontal scrollbar.
     */
    QScrollBar* horzScrollBar() const;

    /**
     * Returns the vertical scrollbar.
     */
    QScrollBar* vertScrollBar() const;

    /**
     * Returns the editor widget.
     */
    KSpreadEditWidget* editWidget() const;

    /**
     * Returns the location widget.
     */
    KSpreadLocationEditWidget* posWidget() const;

    /**
     * Returns the tab bar.
     */
    KSpread::TabBar* tabBar() const;

    void changeZoomMenu( int zoom );
    void setZoom( int zoom, bool updateViews ); // change the zoom value

    void addTable( KSpreadSheet *_t );
    //void removeTable( KSpreadSheet *_t );
    void removeAllTables();
    void setActiveTable( KSpreadSheet *_t,bool updateTable=true );

    const KSpreadSheet* activeTable() const { return m_pTable; }
    KSpreadSheet* activeTable() { return m_pTable; }

    void openPopupMenu( const QPoint &_global );
    void popupRowMenu(const QPoint & _point ) ;
    void popupColumnMenu( const QPoint & _point);

    // void showFormulaToolBar( bool show );

    /**
     * Used by @ref KSpreadEditWidget. Sets the text of the active cell.
     */
    void setText( const QString& _text );

    void enableUndo( bool _b );
    void enableRedo( bool _b );
    void enableInsertColumn( bool _b );
    void enableInsertRow( bool _b );

    /**
     * Called by @ref KSpreadInsertHandler
     *
     * @param _geometry is the zoomed geometry of the new child.
     */
    void insertChart( const QRect& _geometry, KoDocumentEntry& _entry );
    /**
     * Called by @ref KSpreadInsertHandler
     *
     * @param _geometry is the geometry of the new child.
     */
    void insertChild( const QRect& _geometry, KoDocumentEntry& _entry );

    virtual void print( KPrinter &printer );
    virtual void setupPrinter( KPrinter &printer );

    /**
     * Fills the @ref KSpreadEditWidget with the current cells
     * content. This function is usually called after the
     * cursor moved.
     */
    void updateEditWidget();
    /**
     * Same as updateEditEidget() but no update of menus and toolbars
     */
    void updateEditWidgetOnPress();


    /**
     * Called before saving, to finish the current edition (if any)
     */
    void deleteEditor( bool saveChanges = true );
    void closeEditor();
    virtual DCOPObject* dcopObject();

    virtual QWidget *canvas();
    virtual int canvasXOffset() const;
    virtual int canvasYOffset() const;

    /**
     * @reimp
     */
    KoDocument *hitTest( const QPoint &pos );


    void initConfig();

    bool isLoading()const {return m_bLoading;}

    /**
     * write in statusBar result of calc (Min, or Max, average, sum, count)
     */
    void resultOfCalc();

    void initCalcMenu();

    void changeNbOfRecentFiles(int _nb);

    void updateBorderButton();
    void removeTable( KSpreadSheet *_t );
    void insertTable( KSpreadSheet* table );
    QColor borderColor() const;

    KSpreadSelection* selectionInfo()const { return m_selectionInfo; }
    QRect selection() const { return selectionInfo()->selection(); }
    QPoint marker() const { return selectionInfo()->marker(); }
    void updateShowTableMenu();

    /**
     * Repaint any cell with the paintDirty flag that is visible in this view
     */
    void paintUpdates();

    /**
     * Resets the internal handle pointer, called from KSpreadInsertHandler destructor
     */
    void resetInsertHandle() { m_pInsertHandle = 0L; }

    bool showTable(const QString& tableName);

public slots:
    /**
    * refresh view when you hide/show vertical scrollbar
    */
    void refreshView();
    void initialPosition();
    /**
     * Actions
     */
    void createTemplate();
    void transformPart();
    void copySelection();
    void cutSelection();
    void deleteSelection();
    void clearTextSelection();
    void clearCommentSelection();
    void clearValiditySelection();
    void clearConditionalSelection();
    void fillRight();
    void fillLeft();
    void fillUp();
    void fillDown();
    void recalcWorkBook();
    void recalcWorkSheet();
    void paste();
    void specialPaste();
    void editCell();
    void setAreaName();
    void showAreaName();
    void undo();
    void redo();
    void adjust();
    void defaultSelection();
    void paperLayoutDlg();
    void styleDialog();
    void definePrintRange();
    void resetPrintRange();
    void insertObject();
    void insertFromDatabase();
    void insertFromTextfile();
    void insertFromClipboard();
    void insertSpecialChar();
    void editGlobalScripts();
    void editLocalScripts();
    void reloadScripts();
    void runLocalScript();
    void togglePageBorders( bool );
    void toggleProtectSheet( bool );
    void toggleProtectDoc( bool );
    void toggleRecordChanges( bool );
    void toggleProtectChanges( bool );
    void filterChanges();
    void acceptRejectChanges();
    void commentChanges();
    void mergeDocument();
    void viewZoom( const QString & );
    void find();
    void findNext();
    void findPrevious();
    void replace();
    void conditional();
    void validity();
    void insertSeries();
    void sort();
    void insertHyperlink();
    void goalSeek();
    void multipleOperations();
    void subtotals();
    void textToColumns();
    void consolidate();
    void insertTable();
    void removeTable();
    void hideTable();
    void showTable();
    void helpUsing();
    void insertChart();
    void moneyFormat(bool b);
    void alignLeft( bool b );
    void alignRight( bool b );
    void alignCenter( bool b );
    void alignTop( bool b );
    void alignMiddle( bool b );
    void alignBottom( bool b );
    void wrapText( bool b );
    void precisionMinus();
    void precisionPlus();
    void createStyleFromCell();
    void styleSelected( const QString & );
    void setSelectionPrecision(int delta);
    void percent(bool b);
    void fontSelected( const QString &_font );
    void fontSizeSelected( int size );
    void bold( bool b );
    void italic( bool b );
    void underline( bool b );
    void strikeOut( bool b );
    void deleteColumn();
    void insertColumn();
    void deleteRow();
    void insertRow();
    void hideRow();
    void showRow();
    void showSelRows();
    void hideColumn();
    void showColumn();
    void showSelColumns();
    void insertMathExpr();
    void formulaSelection( const QString &_math );
    void changeTextColor();
    void changeBackgroundColor();
    void sortInc();
    void sortDec();
    void layoutDlg();
    void borderBottom();
    void borderRight();
    void borderLeft();
    void borderTop();
    void borderOutline();
    void borderAll();
    void borderRemove();
    void changeBorderColor();
    void tableFormat();
    void autoSum();
    void resizeRow();
    void resizeColumn();
    void increaseFontSize();
    void decreaseFontSize();
    void setSelectionFontSize(int size);

    void setSelectionTextColor(const QColor &txtColor);
    void setSelectionBackgroundColor(const QColor &bgColor);
    void setSelectionBorderColor(const QColor &bdColor);

    void setSelectionLeftBorderColor(const QColor &color);
    void setSelectionRightBorderColor(const QColor &color);
    void setSelectionTopBorderColor(const QColor &color);
    void setSelectionBottomBorderColor(const QColor &color);
    void setSelectionAllBorderColor(const QColor &color);
    void setSelectionOutlineBorderColor(const QColor &color);

    void upper();
    void lower();
    void equalizeColumn();
    void equalizeRow();
    void preference();
    void firstLetterUpper();
    void verticalText(bool );
    void addModifyComment();
    void setSelectionComment(QString comment);
    void removeComment();
    void changeAngle();
    void setSelectionAngle(int angle);

    void mergeCell();
    void dissociateCell();
    void gotoCell();
    void increaseIndent();
    void decreaseIndent();
    void copyAsText();
    /**
     * @ref #tabBar is connected to this slot.
     * When the user selects a new table using the @ref #tabBar this slot
     * is signaled.
     */
    void changeTable( const QString& _name );
    void moveTable( unsigned table, unsigned target );

    void nextTable();
    void previousTable();
    void firstTable();
    void lastTable();

    void sortList();

    void statusBarClicked(int _id);
    void menuCalc(bool);

    /**
     * Shows context menu when tabbar is double-clicked.
     */
    void popupTabBarMenu( const QPoint& );

protected slots:
    /**
     * Popup menu
     */
    void slotActivateTool( int _id );
    void slotInsert();
    void slotInsertCellCopy();
    void slotRemove();
    void slotRename();

    /**
     * Invoked if the popup menu for an embedded document should be opened.
     */
    void popupChildMenu( KoChild*, const QPoint& global_pos );
    void slotPopupDeleteChild();

    /**
     * Border popup menu
     */
    void slotPopupAdjustColumn();
    void slotPopupAdjustRow();

    /**
     * list from list choose
     */
    void slotItemSelected( int );
    void slotListChoosePopupMenu( );

    /**
     * Called by find/replace (findNext) when it found a match
     */
    void slotHighlight( const QString &text, int matchingIndex, int matchedLength );
    /**
     * Called when replacing text in a cell
     */
    void slotReplace( const QString &newText, int, int, int );

    void slotSpecialChar( QChar c, const QString & _font );
    void slotSpecialCharDlgClosed();

protected slots:

    void slotChildSelected( KoDocumentChild* ch );
    void slotChildUnselected( KoDocumentChild* );

public slots:
    // Document signals
    void slotRefreshView();
    void slotUpdateView( KSpreadSheet *_table );
    void slotUpdateView( KSpreadSheet *_table, const QRect& );
    void slotUpdateHBorder( KSpreadSheet *_table );
    void slotUpdateVBorder( KSpreadSheet *_table );
    void slotChangeSelection( KSpreadSheet *_table, const QRect &oldSelection,
                              const QPoint &_oldMarker );
    void slotAddTable( KSpreadSheet *_table );
    void slotRemoveChild( KSpreadChild *_child );
    void slotUpdateChildGeometry( KSpreadChild *_child );
    void slotTableRenamed( KSpreadSheet* table, const QString& old_name );
    void slotTableHidden( KSpreadSheet*_table );
    void slotTableShown( KSpreadSheet*_table );
    void slotTableRemoved( KSpreadSheet*_table );
    void refreshLocale();
    void extraSpelling();

    void spellCheckerReady();
    void spellCheckerMisspelling( const QString &, const QStringList &, unsigned int);
    void spellCheckerCorrected( const QString &, const QString &, unsigned int);
    void spellCheckerDone( const QString & );
    void spellCheckerFinished( );
    void spellCheckerIgnoreAll( const QString & word);
    void spellCheckerReplaceAll( const QString &,  const QString &);
    void startKSpell();

    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

signals:
    void sig_selectionChanged( KSpreadSheet* _table, const QRect& _selection );
    void sig_chooseSelectionChanged(KSpreadSheet *table, const QRect& _selection);

protected:

    virtual void keyPressEvent ( QKeyEvent * _ev );
    virtual void resizeEvent( QResizeEvent *_ev );

    virtual QWMatrix matrix() const;

    /**
     * Activates the formula editor for the current cell.
     * This function is usually called if the user presses
     * a button in the formula toolbar.
     */
    void activateFormulaEditor();

    virtual void updateReadWrite( bool readwrite );

    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    void initFindReplace();
    KSpreadCell* findNextCell();

private:

    ViewPrivate* d;

    // don't allow copy or assignment
    KSpreadView( const KSpreadView& );
    KSpreadView& operator=( const KSpreadView& );

    // GUI stuff
    QButton* newIconButton( const char *_file, bool _kbutton = false, QWidget *_parent = 0L );
    void updateButton(KSpreadCell *cell, int column, int row);

    KoCharSelectDia * m_specialCharDlg;

    struct
    {
      KSpreadSpell *   kspell;
      KSpreadSheet *  firstSpellTable;
      KSpreadSheet *  currentSpellTable;
      KSpreadCell  *  currentCell;
      KSpreadMacroUndoAction *macroCmdSpellCheck;
      unsigned int    spellCurrCellX;
      unsigned int    spellCurrCellY;
      unsigned int    spellStartCellX;
      unsigned int    spellStartCellY;
      unsigned int    spellEndCellX;
      unsigned int    spellEndCellY;
      bool            spellCheckSelection;
        QStringList replaceAll;
    } m_spell;

    bool spellSwitchToOtherTable();
    void spellCleanup();

    /**
     * Pointer to the last popup menu.
     * Since only one popup menu can be opened at once, its pointer is stored here.
     * Delete the old one before you store a pointer to anotheron here.
     * May be 0L.
     */
    QPopupMenu *m_pPopupMenu;
    int m_popupMenuFirstToolId;

    QPopupMenu *m_pPopupRow;
    QPopupMenu *m_pPopupColumn;

    /**
     * Used for embedded children.
     */
    QPopupMenu* m_popupChild;

    /**
     * used for list of choose
     */
    QPopupMenu* m_popupListChoose;

    /**
     * Holds a pointer of the child for which the popup menu has been opened.
     */
    KSpreadChild* m_popupChildObject;

    /**
     * Tells whether the user modfied the current cell.
     * Some key events are passed to the @ref EditWindow. When this flag is set and you
     * want to leave the cell with the marker then you must first save the new text
     * in the cell before moving the marker.
     */
    bool m_bEditDirtyFlag;

    /**
     * The active KSpreadSheet. This table has the input focus. It may be 0L, too.
     */
    KSpreadSheet* m_pTable;

    /**
     * Flags that indicate whether we should display additional
     * GUI stuff like rulers and scrollbars.
     *
     * @see #showGUI
     */
    bool m_bShowGUI;

    /**
     * If @ref #updateEditWidget is called it changes some KToggleActions.
     * That causes them to emit a signal. If this lock is TRUE, then these
     * signals are ignored.
     */
    bool m_toolbarLock;

   struct ToolEntry
   {
     QString command;
     KDataToolInfo info;
   };
   QPtrList<ToolEntry> m_lstTools;

   static KSpreadScripts *m_pGlobalScriptsDialog;
   //used to allow to refresh menubar
   //otherwise kspread crash when I try to refresh menubar
   //when I start kspread
   bool m_bLoading;

    /**
     * Holds a guarded pointer to the transformation toolbox.
     */
    QGuardedPtr<KoTransformToolBox> m_transformToolBox;

    /**
     * Find and Replace context. We remember the options and the strings used
     * previously.
     */
    long m_findOptions;
    QStringList m_findStrings;
    QStringList m_replaceStrings;
    /**
     * Current "find" operation
     */
    KFind* m_find;
    KReplace* m_replace;
    int m_findLeftColumn;
    int m_findRightColumn;
    QPoint m_findPos;
    QPoint m_findEnd;

    KStatusBarLabel* m_sbCalcLabel;

  KSpreadSelection* m_selectionInfo;
  QMap<KSpreadSheet*, QPoint> savedAnchors;
  QMap<KSpreadSheet*, QPoint> savedMarkers;

  KSpreadInsertHandler* m_pInsertHandle;

  /* helper functions */
  void initializeCalcActions();
  void initializeInsertActions();
  void initializeEditActions();
  void initializeAreaOperationActions();
  void initializeGlobalOperationActions();
  void initializeTableActions();
  void initializeSpellChecking();
  void adjustMapActions( bool mode );
  void adjustActions( bool mode );
  void adjustActions( KSpreadSheet const * const table,
                      KSpreadCell const * const cell );
  bool checkChangeRecordPassword();
  void endOperation( QRect const & rect );

};

#endif // KSPREAD_VIEW
