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

class KoDocumentEntry;
class KoToolEntry;

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <koFrame.h>
#include <koScanPlugins.h>
#include <openparts_ui.h>
#include <datatools.h>

#include <qlist.h>
#include <qscrbar.h>
#include <qlabel.h>
#include <qbutton.h>
#include <qpoint.h>

#include "kspread.h"

#ifdef USE_PICTURES
class KSpreadChildPicture : public KoDocumentChildPicture
{
public:
  KSpreadChildPicture( KSpreadView*, KSpreadChild* );
  virtual ~KSpreadChildPicture();

protected:
  KSpreadView *m_pView;
};
#endif

class KSpreadChildFrame : public KoFrame
{
  Q_OBJECT
public:
  KSpreadChildFrame( KSpreadView*, KSpreadChild* );

  KSpreadChild* child() { return m_pChild; }
  /**
   * @return the view owning this frame.
   */
  KSpreadView* spreadView() { return m_pView; }

protected:
  KSpreadChild *m_pChild;
  KSpreadView *m_pView;
};


/**
 */
class KSpreadView : public QWidget,
		    virtual public KoViewIf,
		    virtual public KSpread::View_skel
{
    Q_OBJECT
public:
    KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc *_doc );
    ~KSpreadView();

    // IDL
    virtual KSpread::Book_ptr book();

    // C++
    KSpreadCanvas* canvasWidget() { return m_pCanvas; }
    KSpreadHBorder* hBorderWidget() { return m_pHBorderWidget; }
    KSpreadVBorder* vBorderWidget() { return m_pVBorderWidget; }
    QScrollBar* horzScrollBar() { return m_pHorzScrollBar; }
    QScrollBar* vertScrollBar() { return m_pVertScrollBar; }
    KSpreadEditWidget* editWidget() { return m_pEditWidget; }
    QLabel* posWidget() { return m_pPosWidget; }

    KSpreadDoc* doc() { return m_pDoc; }

    void addTable( KSpreadTable *_t );
    void removeTable( KSpreadTable *_t );
    void removeAllTables();
    void changeTable( const QString& _name );
    void setTextColor(QColor c );
    void setbgColor(QColor c );
    void setActiveTable( KSpreadTable *_t );

    KSpreadTable* activeTable() { return m_pTable; }
    KSpreadTabBar* tabBar() { return  m_pTabBar;}

    void openPopupMenu( const QPoint &_global );
    void set_bg_color(QColor _bgcolor) { bgColor=_bgcolor;}
    void set_text_color(QColor _tbcolor) { tbColor=_tbcolor;}
    void PopupMenuRow(const QPoint & _point ) ;
    void PopupMenuColumn( const QPoint & _point);
    QString setRichTextFond(QString type_font);
    void hide_show_formulatools(bool look);

    /**
     * Used by @ref KSpreadEditWidget. Sets the text of the active cell.
     */
    void setText( const QString& _text );

    void enableUndo( bool _b );
    void enableRedo( bool _b );

    /**
     * Called by @ref KSpreadCanvas if its action is @ref KSpreadCanvas::InsertChart.
     *
     * @param _geometry is the zoomed geometry of the new child.
     */
    void insertChart( const QRect& _geometry );
    /**
     * Called by @ref KSpreadCanvas if its action is @ref KSpreadCanvas::InsertObject.
     *
     * @param _geometry is the zoomed geometry of the new child.
     */
    void insertChild( const QRect& _geometry, KoDocumentEntry& _entry );

    void setFocus( bool mode );

#ifdef USE_PICTURE
    QListIterator<KSpreadChildPicture> pictures() { return QListIterator<KSpreadChildPicture>( m_lstPictures ); }

    void markChildPicture( KSpreadChildPicture *_pic );
#endif
    // IDL
    virtual bool printDlg();

    // IDL Slots
    /**
     * Menu Edit->Copy
     */
    void copySelection();
    /**
     * Menu Edit->Cut
     */
    void cutSelection();
    /**
     * Menu Edit->Patse
     */
    void paste();
    /**
     * Menu Edit->Special Paste
     */
     void Specialpaste();

    /**
     * Menu Edit->Cell
     */
    void editCell();
    /**
     * Menu Edit->Undo
     */
    void undo();
    /**
     * Menu Edit->Redo
     */
    void redo();
    /**
     * Menu Edit->Page Layout
     */
    void paperLayoutDlg();
    /**
     * Menu Edit->Insert->Table
     */
    void insertTable();
    /**
     * Menu Edit->Insert->Object
     */
    void insertObject();
    /**
     * Menu Edit->Insert->Image
     */
    void insertImage();
     /**
     * Menu Edit->Remove->Table
     */
     void RemoveTable();


    /**
     * Menu Scripts->Edit Global Scripts
     */
    void editGlobalScripts();
    /**
     * Menu Scripts->Edit Local Script
     */
    void editLocalScripts();
    /**
     * Menu Scripts->Reload Scripts
     */
    void reloadScripts();
    /**
     * Menu Scripts->Run Local Script
     */
    void runLocalScript();

    /**
     * Menu View->New View
     */
    void newView();
    /**
     * Menu View->Show Page Borders
     */
    void togglePageBorders();
    /**
     * Menu Data
     */
     void gotocell();
    /**
     * Menu Data
     */
     void replace();
    /**
     * Menu Data
     */

      void sort();
    /**
     * Menu Data
     */
      void createanchor();
    /**
     * Menu Data
     */

    void consolidate();

    /**
     * Menu Folder
     */
    void insertNewTable();

    /**
     * Menu Auto Fill
     */
    void autoFill();
     /**
     * Menu Auto Fill
     */
     void resizeheight();
     /**
     * Menu Format
     */
     void resizewidth();
      /**
     * Menu Format
     */
     void renametable();
     /**
     * Menu Format
     */
     void hidetable();
     /**
     * Menu Format
     */
     void showtable();
     /**
     * Menu Format
     */
    void layoutcell();
    /**
     * Menu for help menu
     */
    void helpUsing();

    /**
     * ToolBar
     */
    // void print();
    /**
     * ToolBar
     */
    void insertChart();

    /**
     * ToolBar
     */
  // void zoomMinus();
    /**
     * ToolBar
     */
  // void zoomPlus();

    /**
     * ToolBar
     */
    void moneyFormat();

    /**
     * ToolBar
     */
    void alignLeft();
    /**
     * ToolBar
     */
    void alignRight();
    /**
     * ToolBar
     */
    void alignCenter();
    /**
     * ToolBar
     */
    void multiRow();

    /**
     * ToolBar
     */
    void precisionMinus();
    /**
     * ToolBar
     */
    void precisionPlus();

    /**
     * ToolBar
     */
    void percent();

    /**
     * ToolBar
     */
    void fontSelected( const QString &_font );
    /**
     * ToolBar
     */
    void fontSizeSelected( const QString &_size );
    /**
     * ToolBar
     */
    void bold();
    /**
     * ToolBar
     */
    void italic();

    /**
     * ToolBar
     */
    void deleteColumn();
    /**
     * ToolBar
     */
    void deleteRow();
    /**
     * ToolBar
     */
    void insertColumn();
    /**
     * ToolBar
     */
    void insertRow();
    /**
     *Toolbar
     */

    void formulaselection( const QString &_math );

    void TextColor();
    void BackgroundColor();
    void bordercolor();
    void sortincr();
    void sortdecrease();


    void funct();
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
    void borderbottom();
    void borderright();
    void borderleft();
    void bordertop();
    void borderoutline();
    void borderall();
    void borderremove();

    virtual void cleanUp();

protected slots:
    // C++
    /**
     * Popup menu
     */
    void slotActivateTool( int _id );
    /**
     * Popup menu
     */
    void slotCopy();
    /**
     * Popup menu
     */
    void slotCut();
    /**
     * Popup menu
     */
    void slotPaste();
    /**
     * Popup menu
     */
     void slotSpecialPaste();
    /**
     * Popup menu
     */
    void slotDelete();
    void slotAjust();
    void slotClear();
    void slotInsert();
    void slotRemove();
    /**
     * Popup menu
     */
    void slotLayoutDlg();



    /**
     * @ref #tabBar is connected to this slot.
     * When the user selects a new table using the @ref #tabBar this slot
     * is signaled.
     */
    void slotChangeTable( const QString& _name );
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

    void slotInsertRow();

    void slotRemoveRow();

    void slotInsertColumn();

    void slotRemoveColumn();

    void slotResizeColumn();
    void slotResizeRow();
    void slotAjustColumn();
    void slotAjustRow();

public slots:
    // Document signals
    void slotUnselect( KSpreadTable *_table, const QRect& _old );
    void slotUpdateView( KSpreadTable *_table );
    void slotUpdateView( KSpreadTable *_table, const QRect& );
    void slotUpdateCell( KSpreadTable *_table, KSpreadCell* _cell, int _col, int _row );
    void slotUpdateHBorder( KSpreadTable *_table );
    void slotUpdateVBorder( KSpreadTable *_table );
    void slotChangeSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new );
    void slotAddTable( KSpreadTable *_table );
    void slotInsertChild( KSpreadChild *_child );
    void slotRemoveChild( KSpreadChild *_child );
    void slotUpdateChildGeometry( KSpreadChild *_child );

    // KSpreadChildFrame signals
    void slotChildGeometryEnd( KoFrame* );
    void slotChildMoveEnd( KoFrame* );

    // IDL
    virtual unsigned long int leftGUISize();
    virtual unsigned long int rightGUISize();
    virtual unsigned long int topGUISize();
    virtual unsigned long int bottomGUISize();

signals:
    void sig_selectionChanged( KSpreadTable* _table, const QRect& _selection );

protected:
    // C++
    virtual void init();
    // IDL
    virtual bool event( const QCString & _event, const CORBA::Any& _value );
    // C++
    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );
    virtual bool mappingToolDone( DataTools::Answer& _answer );
    virtual bool mappingEventSetText( KSpread::EventSetText& _event );
    virtual bool mappingEventKeyPressed( KSpread::EventKeyPressed& _event );
    virtual bool mappingEventChartInserted( KSpread::EventChartInserted& _event );

    virtual void keyPressEvent ( QKeyEvent * _ev );
    virtual void resizeEvent( QResizeEvent *_ev );

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
    QLabel *m_pPosWidget;


    OpenPartsUI::ToolBar_var m_vToolBarEdit;
    long int m_idButtonEdit_Copy;
    long int m_idButtonEdit_Paste;
    long int m_idButtonEdit_Cut;
    long int m_idButtonEdit_DelRow;
    long int m_idButtonEdit_DelCol;
    long int m_idButtonEdit_InsRow;
    long int m_idButtonEdit_InsCol;
    OpenPartsUI::ToolBar_var m_vToolBarLayout;
    long int m_idComboLayout_Font;
    long int m_idComboLayout_FontSize;
    long int m_idButtonLayout_Bold;
    long int m_idButtonLayout_Italic;
    long int m_idButtonLayout_Money;
    long int m_idButtonLayout_Percent;
    long int m_idButtonLayout_Left;
    long int m_idButtonLayout_Center;
    long int m_idButtonLayout_Right;
    long int m_idButtonLayout_MultiRows;
    long int m_idButtonLayout_PrecMinus;
    long int m_idButtonLayout_PrecPlus;
    long int m_idButtonLayout_Chart;
    long int m_idButtonLayout_Text_Color;
    long int m_idButtonLayout_bg_Color;
    long int m_idButtonLayout_sort_incr;
    long int m_idButtonLayout_sort_decrease;
    long int m_idButtonLayout_funct;
    long int m_idButtonLayout_borderbottom;
    long int m_idButtonLayout_borderright;
    long int m_idButtonLayout_borderleft;
    long int m_idButtonLayout_bordertop;
    long int m_idButtonLayout_borderoutline;
    long int m_idButtonLayout_borderall;
    long int m_idButtonLayout_border_Color;
    long int m_idButtonLayout_borderremove;

    OpenPartsUI::ToolBar_var m_vToolBarFormula;
    long int m_idButtonFormula_Power;
    long int m_idButtonFormula_Subscript;
    long int m_idButtonFormula_Parentheses;
    long int m_idButtonFormula_AbsValue;
    long int m_idButtonFormula_Brackets;
    long int m_idButtonFormula_Fraction;
    long int m_idButtonFormula_Root;
    long int m_idButtonFormula_Integral;
    long int m_idButtonFormula_Matrix;
    long int m_idButtonFormula_LeftSuper;
    long int m_idButtonFormula_LeftSub;
    long m_idButtonFormula_Sum;
    long m_idButtonFormula_Product;

    OpenPartsUI::Menu_var m_vMenuEdit;
    long int m_idMenuEdit_Undo;
    long int m_idMenuEdit_Redo;
    long int m_idMenuEdit_Cut;
    long int m_idMenuEdit_Copy;
    long int m_idMenuEdit_Paste;
    long int m_idMenuEdit_Cell;
    long int m_idMenuEdit_Layout;
    long int m_idMenuEdit_Special;

    OpenPartsUI::Menu_var m_vMenuEdit_Insert;
    long int m_idMenuEdit_Insert_Table;
    long int m_idMenuEdit_Insert_Chart;
    long int m_idMenuEdit_Insert_Image;
    long int m_idMenuEdit_Insert_Object;

    OpenPartsUI::Menu_var m_vMenuEdit_Remove;
    long int m_idMenuEdit_Remove_Table;

    OpenPartsUI::Menu_var m_vMenuView;
    long int m_idMenuView_NewView;
    long int m_idMenuView_ShowPageBorders;
    OpenPartsUI::Menu_var m_vMenuData;
    long int m_idMenuData_Consolidate;
    long int m_idMenuData_replace;
    long int m_idMenuData_sort;
    long int m_idMenuData_goto;
    long int m_idMenuData_anchor;



    OpenPartsUI::Menu_var m_vMenuFolder;
    long int m_idMenuFolder_NewTable;

    OpenPartsUI::Menu_var m_vMenuFormat;
    long int m_idMenuFormat_AutoFill;
    OpenPartsUI::Menu_var m_vMenuFormat_ResizeRow;
    long int m_idMenuFormat_Height;
    OpenPartsUI::Menu_var m_vMenuFormat_ResizeColumn;
    long int m_idMenuFormat_Width;
    long int m_idMenuFormat_Cell;

    OpenPartsUI::Menu_var m_vMenuFormat_Table;
    long int m_idMenuFormat_Rename;
    long int m_idMenuFormat_Hide;
    long int m_idMenuFormat_Show;

    OpenPartsUI::Menu_var m_vMenuScripts;
    long int m_idMenuScripts_EditGlobal;
    long int m_idMenuScripts_EditLocal;
    long int m_idMenuScripts_Reload;
    long int m_idMenuScripts_Run;
    OpenPartsUI::Menu_var m_vMenuHelp;
    long int m_idMenuHelp_About;
    long int m_idMenuHelp_Using;
    OpenPartsUI::ToolBar_var m_vToolBarMath ;
    long int m_idComboMath;


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

     QColor tbColor;
     QColor bgColor;
     QColor borderColor;

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
     * Tells whether undo is possible right now or not.
     *
     * @see #enableUndo
     */
    bool m_bUndo;
    /**
     * Tells whether redo is possible right now or not.
     *
     * @see #enableRedo
     */
    bool m_bRedo;

    /**
     * Flags that indicate whether we should display additional
     * GUI stuff like rulers and scrollbars.
     *
     * @see #showGUI
     */
    bool m_bShowGUI;

    QList<KSpreadChildFrame> m_lstFrames;
#ifdef USE_PICTURES
    QList<KSpreadChildPicture> m_lstPictures;
#endif

  /**
   * Set to true if the function @ref #init is entered. The start value if false.
   */
   bool m_bInitialized;

   struct ToolEntry
   {
     QString command;
     KoToolEntry* entry;
   };
   QList<ToolEntry> m_lstTools;

   KoPluginManager *m_pluginManager;

   static KSpreadScripts *m_pGlobalScriptsDialog;
};

#endif
