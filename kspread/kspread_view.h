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

class KoToolEntry;

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <koFrame.h>
#include <koScanPlugins.h>
#include <koQueryTypes.h>
#include <openparts_ui.h>
#include <datatools.h>

#include <qlist.h>
#include <qscrbar.h>
#include <qlabel.h> 
#include <qbutton.h>
#include <qpoint.h>

#include "kspread_tabbar.h"
#include "kspread_table.h"
#include "kspread.h"

#define YBORDER_WIDTH 50
#define XBORDER_HEIGHT 14

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
class KSpreadEditWidget : public QLineEdit
{
    Q_OBJECT
public:
    KSpreadEditWidget( QWidget *_parent, KSpreadView *_gui );

    void publicKeyPressEvent ( QKeyEvent* _ev );

public slots:
    void slotAbortEdit();
    void slotDoneEdit();            

protected:    
    virtual void keyPressEvent ( QKeyEvent* _ev );
    
    KSpreadView* m_pView;
};

/**
 */
class KSpreadCanvas : public QWidget
{
    friend KSpreadHBorder;
    friend KSpreadVBorder;
    friend KSpreadView;
  
    Q_OBJECT
public:
    enum MouseActions { Mark = 1, ResizeCell = 2, NoAction = 0, AutoFill = 3, ChildGeometry = 4 };
    /**
     * The possible actions that we expect the user to do.
     * Usually this is 'Default' and tells us that the user may edit
     * the table. If @ref #action is 'InsertChild' then the user must draw
     * a rectangle in order of telling us where to insert the new child.
     */
    enum Actions { DefaultAction, InsertChild, InsertChart };

    KSpreadCanvas( QWidget *_parent, KSpreadView *_gui );

    void setAction( Actions _act );
    /**
     * This is usually called with '_act' equal KSpreadCanvas::InsertChild.
     */
    void setAction( Actions _act, KoDocumentEntry& _entry );
  
    void updateCellRect( const QRect &_rect );

    /**
     * Used by @ref KSpreadView
     */
    void setEditDirtyFlag( bool _flag ) { m_bEditDirtyFlag = _flag; }
    bool editDirtyFlag() { return m_bEditDirtyFlag; }
  
protected:
    virtual void keyPressEvent ( QKeyEvent* _ev );    
    virtual void paintEvent ( QPaintEvent* _ev );    
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );

    KSpreadView *m_pView;

    bool m_bEditDirtyFlag;
  
    /**
     * If the user is dragging around with the mouse then this tells us what he is doing.
     * The user may want to mark cells or he started in the lower right corner
     * of the marker which is something special. The values for the 2 above
     * methods are called 'Mark' and 'ResizeCell' or 'AutoFill' depending
     * on the mouse button used. By default this variable holds
     * the value 'NoAction'.
     */
    MouseActions m_eMouseAction;

    /**
     * Tells us what we are doing right now.
     * The default value is 'Default'.
     */
    Actions m_eAction;
    /**
     * If the @ref m_eAction is InsertChild or InsertChart, then this record
     * holds informations about which component we should use here.
     */
    KoDocumentEntry m_actionArgument;
  
    /**
     * Used to indicate wether the user started drawing a rubber band rectangle.
     */
    bool m_bGeometryStarted;
    QPoint m_ptGeometryStart;
    QPoint m_ptGeometryEnd;

    /**
     * The column in which a mouse drag started.
     */
    int m_iMouseStartColumn;
    /**
     * The row in which a mouse drag started.
     */
    int m_iMouseStartRow;

    /**
     * Tells wether the user selected more than one cell.
     * If the user presses the left mouse button and if he marks more
     * than one cell until he releases the button again, then this flag
     * is set. If this flag is set, then one should repaint all visible
     * cells once the button is released.
     */
    bool m_bMouseMadeSelection;

    /**
     * If we use the lower right corner of the marker to start autofilling, then this
     * rectangle conatins all cells that were already marker when the user started
     * to mark the rectangle which he wants to become autofilled.
     *
     * @see #mousePressEvent
     * @see #mouseReleeaseEvent
     */
    QRect m_rctAutoFillSrc;
};

/**
 */
class KSpreadHBorder : public QWidget
{
    Q_OBJECT
public:
    KSpreadHBorder( QWidget *_parent, KSpreadView *_gui );
    
protected:
    virtual void paintEvent ( QPaintEvent* _ev ); 
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );

    KSpreadView *m_pView;

    /**
     * Flag that inidicates wether the user wants to mark columns.
     * The user may mark columns by dragging the mouse around in th XBorder widget.
     * If he is doing that right now, this flag is TRUE. Mention that the user may
     * also resize columns by dragging the mouse. This case is not covered by this flag.
     */
    bool m_bSelection;
    
    /**
     * The column over which the user pressed the mouse button.
     * If the user marks columns in the XBorder widget, then this is the initial
     * column on which he pressed the mouse button.
     */
    int m_iSelectionAnchor;
    
    /**
     * Flag that indicates wether the user resizes a column
     * The user may resize columns by dragging the mouse around in th XBorder widget.
     * If he is doing that right now, this flag is TRUE. Mention that the user may
     */
    bool m_bResize;
    /**
     * The column over which the user pressed the mouse button.
     * The user may resize columns by dragging the mouse around un the XBorder widget.
     * This is the column over which he pressed the mouse button. This column is going
     * to be resized.
      */
    int m_iResizeAnchor;
    /**
     * Last position of the mouse.
     */
    int m_iResizePos;
};

/**
 */
class KSpreadVBorder : public QWidget
{
    Q_OBJECT
public:
    KSpreadVBorder( QWidget *_parent, KSpreadView *_gui );
    
protected:
    virtual void paintEvent ( QPaintEvent* _ev ); 
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );

    KSpreadView *m_pView;

    bool m_bSelection;
    int m_iSelectionAnchor;
    bool m_bResize;
    int m_iResizeAnchor;
    int m_iResizePos;
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
    KSpreadCanvas* canvasWidget() { return m_pCanvasWidget; }
    QWidget* hBorderWidget() { return m_pHBorderWidget; }    
    QWidget* vBorderWidget() { return m_pVBorderWidget; }    
    QScrollBar* horzScrollBar() { return m_pHorzScrollBar; }    
    QScrollBar* vertScrollBar() { return m_pVertScrollBar; }    
    KSpreadEditWidget* editWidget() { return m_pEditWidget; }
    QLabel* posWidget() { return m_pPosWidget; }  

    KSpreadDoc* doc() { return m_pDoc; }
  
    float zoom() { return m_fZoom; }

    int xOffset() { return m_iXOffset; }
    int yOffset() { return m_iYOffset; }
    
    void addTable( KSpreadTable *_t );
    void removeTable( KSpreadTable *_t );
    void removeAllTables();
    
    void setActiveTable( KSpreadTable *_t );

    KSpreadTable* findTable( const char *_name );
    KSpreadTable* activeTable() { return m_pTable; }   

    void openPopupMenu( const QPoint &_global );
  
    QPoint marker();  
    bool isMarkerVisible() { return ( m_iMarkerVisible == 1 ); }
    int markerColumn() { return m_iMarkerColumn; }
    int markerRow() { return m_iMarkerRow; }
    void setMarkerColumn( int _c ) { m_iMarkerColumn = _c; }
    void setMarkerRow( int _r ) { m_iMarkerRow = _r; }
    void hideMarker() { if ( m_iMarkerVisible == 1 ) drawMarker(); m_iMarkerVisible--; }
    void showMarker() { if ( m_iMarkerVisible == 1 ) return; m_iMarkerVisible++; if ( m_iMarkerVisible == 1 ) drawMarker(); }
    void hideMarker( QPainter& );
    void showMarker( QPainter& );
  
    const QPen& defaultGridPen() { return m_defaultGridPen; }

    /**
     * Used by @ref KSpreadEditWidget. Sets the text of the active cell.
     */
    void setText( const char *_text );

    void enableUndo( bool _b );
    void enableRedo( bool _b );

    // Drawing Engine
    /**
     * Redraws a single object.
     * This is a convenience function that
     * creates a new @ref QPainter for you. Dont use if another @ref QPainter is active!
     *
     * @param _cell is the @ref KSpreadCell to draw.
     * @param _col is the column this object is assumed to reside in.
     * @param _row is the row this object is assumed to reside in.
     */
    void drawCell( KSpreadCell *_cell, int _col, int _row );
    /**
     * Redraws a single object.
     */
    void drawCell( QPainter &_painter, KSpreadCell *_cell, int _col, int _row );
    /**
     * Draw all currently visible objects.
     */
    void drawVisibleCells();
    void drawMarker( QPainter * _painter = 0L );

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

    void setFocus( CORBA::Boolean mode );

#ifdef USE_PICTURE
    QListIterator<KSpreadChildPicture> pictures() { return QListIterator<KSpreadChildPicture>( m_lstPictures ); }

    void markChildPicture( KSpreadChildPicture *_pic );
#endif
    // IDL
    /* virtual void setMode( OPParts::Part::Mode _mode );
    virtual void setFocus( CORBA::Boolean mode ); */
    virtual CORBA::Boolean printDlg();

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
    void fontSelected( const char *_font );
    /**
     * ToolBar
     */
    void fontSizeSelected( const char *_size );
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
    void slotDelete();
    /**
     * Popup menu
     */
    void slotLayoutDlg();

    void slotMaxColumn( int _max_column );
    void slotMaxRow( int _max_row );
    /**
     * @ref #tabBar is connected to this slot.
     * When the user selects a new table using the @ref #tabBar this slot
     * is signaled.
     */
    void slotChangeTable( const char *_name );
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

    void slotScrollVert( int _value );
    void slotScrollHorz( int _value );

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
    virtual CORBA::ULong leftGUISize();
    virtual CORBA::ULong rightGUISize();
    virtual CORBA::ULong topGUISize();
    virtual CORBA::ULong bottomGUISize();

signals:
    void sig_selectionChanged( KSpreadTable* _table, const QRect& _selection );
  
protected:
    // C++
    virtual void init();
    // IDL
    virtual bool event( const char* _event, const CORBA::Any& _value );
    // C++
    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );
    virtual bool mappingToolDone( DataTools::Answer& _answer );
    virtual bool mappingEventSetText( KSpread::EventSetText& _event );
    virtual bool mappingEventKeyPressed( KSpread::EventKeyPressed& _event );
    virtual bool mappingEventChartInserted( KSpread::EventChartInserted& _event );
  
    virtual void keyPressEvent ( QKeyEvent * _ev );
    virtual void resizeEvent( QResizeEvent *_ev );

    /**
     * Used to draw the grey grid that is usually only visible on the
     * screen, but not by printing on paper.
     */
    QPen m_defaultGridPen;

    int m_iMarkerColumn;
    int m_iMarkerRow;
    /**
     * A value of 1 means that it is visible, every lower value means it is
     * made invisible multiple times.
     *
     * @see #hideMarker
     * @see #showMarker
     */
    int m_iMarkerVisible;

    // GUI stuff
    QButton* newIconButton( const char *_file, bool _kbutton = false, QWidget *_parent = 0L );
  
    QScrollBar *m_pHorzScrollBar;
    QScrollBar *m_pVertScrollBar;
    KSpreadCanvas *m_pCanvasWidget;
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
    CORBA::Long m_idButtonEdit_Copy;
    CORBA::Long m_idButtonEdit_Paste;
    CORBA::Long m_idButtonEdit_Cut;
    CORBA::Long m_idButtonEdit_DelRow;
    CORBA::Long m_idButtonEdit_DelCol;
    CORBA::Long m_idButtonEdit_InsRow;
    CORBA::Long m_idButtonEdit_InsCol;
    OpenPartsUI::ToolBar_var m_vToolBarLayout;
    CORBA::Long m_idComboLayout_Font;
    CORBA::Long m_idComboLayout_FontSize;
    CORBA::Long m_idButtonLayout_Bold;
    CORBA::Long m_idButtonLayout_Italic;
    CORBA::Long m_idButtonLayout_Money;
    CORBA::Long m_idButtonLayout_Percent;
    CORBA::Long m_idButtonLayout_Left;
    CORBA::Long m_idButtonLayout_Center;
    CORBA::Long m_idButtonLayout_Right;
    CORBA::Long m_idButtonLayout_MultiRows;
    CORBA::Long m_idButtonLayout_PrecMinus;
    CORBA::Long m_idButtonLayout_PrecPlus;
    CORBA::Long m_idButtonLayout_Chart;

    OpenPartsUI::Menu_var m_vMenuEdit;
    CORBA::Long m_idMenuEdit_Undo;
    CORBA::Long m_idMenuEdit_Redo;
    CORBA::Long m_idMenuEdit_Cut;
    CORBA::Long m_idMenuEdit_Copy;
    CORBA::Long m_idMenuEdit_Paste;
    CORBA::Long m_idMenuEdit_Cell;
    CORBA::Long m_idMenuEdit_Layout;
    OpenPartsUI::Menu_var m_vMenuEdit_Insert;
    CORBA::Long m_idMenuEdit_Insert_Table;
    CORBA::Long m_idMenuEdit_Insert_Chart;
    CORBA::Long m_idMenuEdit_Insert_Image;
    CORBA::Long m_idMenuEdit_Insert_Object;
    OpenPartsUI::Menu_var m_vMenuView;
    CORBA::Long m_idMenuView_NewView;
    CORBA::Long m_idMenuView_ShowPageBorders;
    OpenPartsUI::Menu_var m_vMenuData;
    CORBA::Long m_idMenuData_Consolidate;
    OpenPartsUI::Menu_var m_vMenuFolder;
    CORBA::Long m_idMenuFolder_NewTable;
    OpenPartsUI::Menu_var m_vMenuFormat;
    CORBA::Long m_idMenuFormat_AutoFill;
    OpenPartsUI::Menu_var m_vMenuScripts;
    CORBA::Long m_idMenuScripts_EditGlobal;
    CORBA::Long m_idMenuScripts_EditLocal;
    CORBA::Long m_idMenuScripts_Reload;
    CORBA::Long m_idMenuScripts_Run;
    OpenPartsUI::Menu_var m_vMenuHelp;
    CORBA::Long m_idMenuHelp_About;
    CORBA::Long m_idMenuHelp_Using;

    /**
     * Pointer to the last popup menu.
     * Since only one popup menu can be opened at once, its pointer is stored here.
     * Delete the old one before you store a pointer to anotheron here.
     * May be 0L.
     */
    QPopupMenu *m_pPopupMenu;
    int m_popupMenuFirstToolId;
 
    /**
     * Tells wether the user modfied the current cell.
     * Some key events are passed to the @ref EditWindow. When this flag is set and you
     * want to leave the cell with the marker then you must first save the new text
     * in the cell before moving the marker.
     */
    bool m_bEditDirtyFlag;

    /**
     * The active KSpreadTable. This table has the input focus. It may be 0L, too.
     */
    KSpreadTable* m_pTable;
    
    float m_fZoom;
    
    KSpreadDoc *m_pDoc;
  
    QPopupMenu *viewMenu;
    int m_pShowPageBordersID;

  // AutoFillDialog *_autoFillDialog;             

  // KFileSelect *openDlg;
  // KFileSelect *saveDlg;

    static KSpreadScripts *m_pGlobalScriptsDialog;

    /**
     * Tells wether undo is possible right now or not.
     *
     * @see #enableUndo
     */
    bool m_bUndo;
    /**
     * Tells wether redo is possible right now or not.
     *
     * @see #enableRedo
     */
    bool m_bRedo;

    /**
     * Flags that indicate wether we should display additional
     * GUI stuff like rulers and scrollbars.
     *
     * @see #showGUI
     */
    bool m_bShowGUI;

    int m_iXOffset;
    int m_iYOffset;

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

   KoPluginManager m_pluginManager;
};

#endif


