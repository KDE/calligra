/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __kspread_gui_h__
#define __kspread_gui_h__

class QFrame;
class QScrollBar;
class QButton;

class KSpreadView;
class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadScripts;
class KSpreadTable;
class KSpreadDoc;
class KSpreadPaperLayout;
class KSpreadChildPicture;
class KSpreadChildFrame;
class KSpreadShell;
class KSpreadTabBar;
class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadChild;
class KSpreadCell;
class KSpreadLocationEditWidget;
class KoDocumentEntry;

class KSelectColorAction;

class KAction;
class KSelectAction;
class KFontAction;
class KFontSizeAction;
class KToggleAction;

class KActionMenu;

class DCOPObject;

#include <qlist.h>
#include <qpoint.h>
#include <qguardedptr.h>

#include <koView.h>
#include <koDocument.h>
#include <koToolBox.h>
#include <koDataTool.h>
#include <kglobalsettings.h>

/**
 */
class KSpreadView : public KoView
{
    friend KSpreadCanvas;

    Q_OBJECT
public:
    KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc *_doc );
    ~KSpreadView();

    KSpreadCanvas* canvasWidget() const { return m_pCanvas; }
    KSpreadHBorder* hBorderWidget() { return m_pHBorderWidget; }
    KSpreadVBorder* vBorderWidget() { return m_pVBorderWidget; }
    QScrollBar* horzScrollBar() { return m_pHorzScrollBar; }
    QScrollBar* vertScrollBar() { return m_pVertScrollBar; }
    KSpreadEditWidget* editWidget() { return m_pEditWidget; }
    //QLabel* posWidget() { return m_pPosWidget; }
    KSpreadLocationEditWidget* posWidget() { return m_pPosWidget; }

    KSpreadDoc* doc() { return m_pDoc; }

    void addTable( KSpreadTable *_t );
    //void removeTable( KSpreadTable *_t );
    void removeAllTables();
    void setActiveTable( KSpreadTable *_t,bool updateTable=true );

    const KSpreadTable* activeTable() const { return m_pTable; }
    KSpreadTable* activeTable() { return m_pTable; }
    KSpreadTabBar* tabBar() { return  m_pTabBar;}

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

    virtual void print( QPrinter &printer );

    void paintContent( QPainter& painter, const QRect& rect, bool transparent );

    /**
     * Enables/Disables all actions of the formula toolbar.
     */
    void enableFormulaToolBar( bool );

    /**
     * Fills the @ref KSpreadEditWidget with the current cells
     * content. This function is usually called after the
     * cursor moved.
     */
    void updateEditWidget();


    /**
     * Called before saving, to finish the current edition (if any)
     */
    void deleteEditor( bool saveChanges = true );

    virtual DCOPObject* dcopObject();

    virtual QWidget *canvas();
    virtual int canvasXOffset() const;
    virtual int canvasYOffset() const;

    /**
     * @reimp
     */
    KoDocument *hitTest( const QPoint &pos );


    void initConfig();

    /**
    * refresh view when you hide/show vertical scrollbar
    */
    void refreshView();

    bool isLoading() {return m_bLoading;}

    /**
     * write in statusBar result of calc (Min, or Max, average, sum, count)
     */
    void resultOfCalc();

public slots:
    void initialPosition();
    /**
     * Actions
     */
    void transformPart();
    void copySelection();
    void cutSelection();
    void deleteSelection();
    void clearTextSelection();
    void clearCommentSelection();
    void clearValiditySelection();
    void clearConditionalSelection();
    void RecalcWorkBook();
    void RecalcWorkSheet();
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
    void insertObject();
    void editGlobalScripts();
    void editLocalScripts();
    void reloadScripts();
    void runLocalScript();
    void togglePageBorders( bool );
    void replace();
    void conditional();
    void validity();
    void insertSeries();
    void sort();
    void insertHyperlink();
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
    void multiRow( bool b );
    void precisionMinus();
    void precisionPlus();
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
    void hideColumn();
    void showColumn();
    void insertFormula();
    void insertMathExpr();
    void formulaSelection( const QString &_math );
    void changeTextColor();
    void changeBackgroundColor();
    void sortInc();
    void sortDec();
    void layoutDlg();
    void formulaPower();
    void formulaSubscript();
    void formulaParentheses();
    void formulaAbsValue();
    void formulaBrackets();
    void formulaFraction();
    void formulaRoot();
    void formulaIntegral();
    void formulaMatrix();
    void formulaLeftSuper();
    void formulaLeftSub();
    void formulaSum();
    void formulaProduct();
    void borderBottom();
    void borderRight();
    void borderLeft();
    void borderTop();
    void borderOutline();
    void borderAll();
    void borderRemove();
    void changeBorderColor();
    void tableFormat();
    // void oszilloscope();
    void autoSum();
    void resizeRow();
    void resizeColumn();
    void increaseFontSize();
    void decreaseFontSize();
    void upper();
    void lower();
    void equalizeColumn();
    void equalizeRow();
    void preference();
    void firstLetterUpper();
    void verticalText(bool );
    void addModifyComment();
    void removeComment();
    void changeAngle();
    void mergeCell();
    void dissociateCell();
    void gotoCell();
    void  increaseIndent();
    void  decreaseIndent();
    /**
     * @ref #tabBar is connected to this slot.
     * When the user selects a new table using the @ref #tabBar this slot
     * is signaled.
     */
    void changeTable( const QString& _name );
    void nextTable();
    void previousTable();

    void sortList();

protected slots:
    /**
     * Popup menu
     */
    void slotActivateTool( int _id );
    void slotInsert();
    void slotInsertCellCopy();
    void slotRemove();

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
     * Scroll @ref #tabBar.
     */
    void slotScrollToFirstTable();
    /**
     * Scroll @ref #tabBar.
     */
    void slotScrollToLeftTable();
    /**
     * Scroll @ref #tabBar.
     */
    void slotScrollToRightTable();
    /**
     * Scroll @ref #tabBar.
     */
    void slotScrollToLastTable();

    /**
     * list from list choose
     */
    void slotItemSelected( int );
    void slotListChoosePopupMenu( );
   
protected slots:
    void repaintPolygon( const QPointArray& );

    void slotChildSelected( KoDocumentChild* ch );
    void slotChildUnselected( KoDocumentChild* );

public slots:
    // Document signals
    void slotUnselect( KSpreadTable *_table, const QRect& _old );
    void slotUpdateView( KSpreadTable *_table );
    void slotUpdateView( KSpreadTable *_table, const QRect& );
    void slotUpdateHBorder( KSpreadTable *_table );
    void slotUpdateVBorder( KSpreadTable *_table );
    void slotChangeSelection( KSpreadTable *_table, const QRect &_old, const QRect &_old_marker );
    void slotChangeChooseSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new );
    void slotAddTable( KSpreadTable *_table );
    void slotRemoveChild( KSpreadChild *_child );
    void slotUpdateChildGeometry( KSpreadChild *_child );
    void slotTableRenamed( KSpreadTable* table, const QString& old_name );
    void slotTableHidden( KSpreadTable*_table );
    void slotTableShown( KSpreadTable*_table );
    void slotTableRemoved( KSpreadTable*_table );
    void slotTableActivated( KSpreadTable* table );
    void slotRefreshView( );

    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

signals:
    void sig_selectionChanged( KSpreadTable* _table, const QRect& _selection );
    void sig_chooseSelectionChanged( KSpreadTable* _table, const QRect& _selection );

protected:
    // bool eventKeyPressed( QKeyEvent* _event, bool choose );

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

private:
    // GUI stuff
    QButton* newIconButton( const char *_file, bool _kbutton = false, QWidget *_parent = 0L );

    QScrollBar *m_pHorzScrollBar;
    QScrollBar *m_pVertScrollBar;
    KSpreadCanvas *m_pCanvas;
    KSpreadVBorder *m_pVBorderWidget;
    KSpreadHBorder *m_pHBorderWidget;
    KSpreadEditWidget *m_pEditWidget;
    QWidget *m_pFrame;
    QFrame *m_pToolWidget;
    QButton *m_pTabBarFirst;
    QButton *m_pTabBarLeft;
    QButton *m_pTabBarRight;
    QButton *m_pTabBarLast;
    QButton *m_pOkButton;
    QButton *m_pCancelButton;
    KSpreadTabBar *m_pTabBar;
    //QLabel *m_pPosWidget;
    KSpreadLocationEditWidget *m_pPosWidget;

    KToggleAction* m_bold;
    KToggleAction* m_italic;
    KToggleAction* m_underline;
    KToggleAction* m_strikeOut;
    KToggleAction* m_percent;
    KAction* m_precplus;
    KAction* m_precminus;
    KToggleAction* m_money;
    KToggleAction* m_alignLeft;
    KToggleAction* m_alignCenter;
    KToggleAction* m_alignRight;
    KToggleAction* m_alignTop;
    KToggleAction* m_alignMiddle;
    KToggleAction* m_alignBottom;
    KAction* m_transform;
    KAction* m_copy;
    KAction* m_paste;
    KAction* m_cut;
    KAction* m_specialPaste;
    KAction* m_delete;
    KAction* m_clearText;
    KAction* m_clearComment;
    KAction* m_clearValidity;
    KAction* m_clearConditional;
    KAction* m_recalc_workbook;
    KAction* m_recalc_worksheet;
    KAction* m_adjust;
    KAction* m_editCell;
    KAction* m_undo;
    KAction* m_redo;
    KAction* m_paperLayout;
    KAction* m_insertTable;
    KAction* m_removeTable;
    KAction* m_editGlobalScripts;
    KAction* m_editLocalScripts;
    KAction* m_reloadScripts;
    KAction* m_replace;
    KAction* m_conditional;
    KAction* m_validity;
    KAction* m_sort;
    KAction* m_consolidate;
    KAction* m_help;
    KAction* m_insertCellCopy;
    KToggleAction* m_multiRow;
    KFontAction* m_selectFont;
    KFontSizeAction* m_selectFontSize;
    KAction* m_deleteColumn;
    KAction* m_hideColumn;
    KAction* m_showColumn;
    KAction* m_insertColumn;
    KAction* m_deleteRow;
    KAction* m_insertRow;
    KAction* m_hideRow;
    KAction* m_showRow;
    KAction* m_formulaPower;
    KAction* m_formulaSubscript;
    KAction* m_formulaParantheses;
    KAction* m_formulaAbsValue;
    KAction* m_formulaBrackets;
    KAction* m_formulaFraction;
    KAction* m_formulaRoot;
    KAction* m_formulaIntegral;
    KAction* m_formulaMatrix;
    KAction* m_formulaLeftSuper;
    KAction* m_formulaLeftSub;
    KAction* m_formulaSum;
    KAction* m_formulaProduct;
    KSelectAction* m_formulaSelection;
    KAction* m_sortDec;
    KAction* m_sortInc;
    KSelectColorAction* m_textColor;
    KSelectColorAction* m_bgColor;
    KAction* m_cellLayout;
    KAction* m_hideTable;
    KAction* m_showTable;
    KAction* m_borderLeft;
    KAction* m_borderRight;
    KAction* m_borderTop;
    KAction* m_borderBottom;
    KAction* m_borderAll;
    KAction* m_borderOutline;
    KAction* m_borderRemove;
    KSelectColorAction* m_borderColor;
    KAction* m_tableFormat;
    // KAction* m_oszi;
    KAction* m_autoSum;
    KToggleAction* m_showPageBorders;
    KActionMenu* m_scripts;
    KAction* m_default;
    KAction* m_areaName;
    KAction* m_showArea;
    KAction* m_resizeRow;
    KAction* m_resizeColumn;
    KAction* m_fontSizeUp;
    KAction* m_fontSizeDown;
    KAction* m_upper;
    KAction* m_lower;
    KAction* m_equalizeRow;
    KAction* m_equalizeColumn;
    KAction* m_preference;
    KAction* m_firstLetterUpper;
    KToggleAction* m_verticalText;
    KAction* m_addModifyComment;
    KAction* m_removeComment;
    KAction* m_insertCell;
    KAction* m_removeCell;
    KAction* m_changeAngle;
    KAction* m_mergeCell;
    KAction* m_dissociateCell;
    KAction* m_gotoCell;
    KAction* m_increaseIndent;
    KAction* m_decreaseIndent;
    KAction* m_sortList;

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
     * This DCOP object represents the view.
     */
    DCOPObject* m_dcop;


    /**
     * Tells whether the user modfied the current cell.
     * Some key events are passed to the @ref EditWindow. When this flag is set and you
     * want to leave the cell with the marker then you must first save the new text
     * in the cell before moving the marker.
     */
    bool m_bEditDirtyFlag;

    /**
     * The active KSpreadTable. This table has the input focus. It may be 0L, too.
     */
    KSpreadTable* m_pTable;

    KSpreadDoc *m_pDoc;

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
     KoDataToolInfo info;
   };
   QList<ToolEntry> m_lstTools;

   static KSpreadScripts *m_pGlobalScriptsDialog;
   //used to allow to refresh menubar
   //otherwise kspread crash when I try to refresh menubar
   //when I start kspread
   bool m_bLoading;

    /**
     * Holds a guarded pointer to the transformation toolbox.
     */
    QGuardedPtr<KoTransformToolBox> m_transformToolBox;
};

#endif
