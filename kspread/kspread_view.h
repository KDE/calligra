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

#include <part_frame_impl.h>
#include <view_impl.h>
#include <document_impl.h>
#include <menu_impl.h>
#include <toolbar_impl.h>

#include <qlist.h>
#include <qscrbar.h>
#include <qlabel.h> 
#include <qbutton.h>

#include "kspread_tabbar.h"
#include "kspread_table.h"
#include "kspread.h"

class KSpreadDoc;

#define YBORDER_WIDTH 50
#define XBORDER_HEIGHT 14

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
    enum MouseActions { Mark = 1, ResizeCell = 2, NoAction = 0, AutoFill = 3, ChartGeometry = 4 };
    /**
     * The possible actions that we expect the user to do.
     * Usually this is 'Default' and tells us that the user may edit
     * the table. If @ref #action is 'Chart' then the user must draw
     * a rectangle in order of telling us where to insert the chart.
     */
    enum Actions { DefaultAction, Chart };

    KSpreadCanvas( QWidget *_parent, KSpreadView *_gui );

    void setAction( Actions _act );
  
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
		    virtual public View_impl,
		    virtual public KSpread::View_skel
{
    Q_OBJECT
public:
    KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc *_doc );
    ~KSpreadView();

    virtual void createGUI();
  
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
  
    void marker( int &row, int &column);
    bool isMarkerVisible() { return m_bMarkerVisible; }
    int markerColumn() { return m_iMarkerColumn; }
    int markerRow() { return m_iMarkerRow; }
    void setMarkerColumn( int _c ) { m_iMarkerColumn = _c; }
    void setMarkerRow( int _r ) { m_iMarkerRow = _r; }

    const QPen& defaultGridPen() { return m_defaultGridPen; }

    /**
     * Used by @ref KSpreadEditWidget. Sets the text of the active cell.
     */
    void setText( const char *_text );

    void enableUndo( bool _b ) {} // { editMenu->setItemEnabled( editMenu->idAt( 0 ), _b ); bUndo = _b; }    
    void enableRedo( bool _b ) {} // { editMenu->setItemEnabled( editMenu->idAt( 1 ), _b ); bRedo = _b; }    

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
     * Menu View->New View
     */
    void newView();
    /**
     * Menu View->Show Page Borders
     */
    void togglePageBorders();
    
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
    void helpAbout();
    /**
     * Menu for help menu
     */
    void helpUsing();

    /**
     * ToolBar
     */
    void print();
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
      
protected slots:
    // C++
    /**
     * Needed to communicate with @ref KSpreadPaperLayout dialog.
     */
    void slotChangePaperLayout();
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
  
protected:
    // C++
    virtual void keyPressEvent ( QKeyEvent * _ev );
    virtual void resizeEvent( QResizeEvent *_ev );

    virtual int leftGUISize() { return YBORDER_WIDTH; }
    virtual int rightGUISize() { return 20; }
    virtual int topGUISize() { return XBORDER_HEIGHT + 30; }
    virtual int bottomGUISize() { return 20; }

    /**
     * Used to draw the grey grid that is usually only visible on the
     * screen, but not by printing on paper.
     */
    QPen m_defaultGridPen;

    int m_iMarkerColumn;
    int m_iMarkerRow;
    bool m_bMarkerVisible;

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

    OPParts::ToolBarFactory_var m_vToolBarFactory;
  /* ToolBar_ref m_rToolBarFile;
    CORBA::Long m_idButtonFile_Open;
    CORBA::Long m_idButtonFile_Save;
    CORBA::Long m_idButtonFile_Print;
    CORBA::Long m_idButtonFile_ZoomOut;
    CORBA::Long m_idButtonFile_ZoomIn; */
    ToolBar_ref m_rToolBarEdit;
    CORBA::Long m_idButtonEdit_Copy;
    CORBA::Long m_idButtonEdit_Paste;
    CORBA::Long m_idButtonEdit_Cut;
    CORBA::Long m_idButtonEdit_DelRow;
    CORBA::Long m_idButtonEdit_DelCol;
    CORBA::Long m_idButtonEdit_InsRow;
    CORBA::Long m_idButtonEdit_InsCol;
    ToolBar_ref m_rToolBarLayout;
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

    OPParts::MenuBarFactory_var m_vMenuBarFactory;
    MenuBar_ref m_rMenuBar;
    CORBA::Long m_idMenuEdit;
    CORBA::Long m_idMenuEdit_Undo;
    CORBA::Long m_idMenuEdit_Redo;
    CORBA::Long m_idMenuEdit_Cut;
    CORBA::Long m_idMenuEdit_Copy;
    CORBA::Long m_idMenuEdit_Paste;
    CORBA::Long m_idMenuEdit_Cell;
    CORBA::Long m_idMenuEdit_Layout;
    CORBA::Long m_idMenuView;
    CORBA::Long m_idMenuView_NewView;
    CORBA::Long m_idMenuView_ShowPageBorders;
    CORBA::Long m_idMenuFolder;
    CORBA::Long m_idMenuFolder_NewTable;
    CORBA::Long m_idMenuFormat;
    CORBA::Long m_idMenuFormat_AutoFill;
    CORBA::Long m_idMenuScripts;
    CORBA::Long m_idMenuScripts_EditGlobal;
    CORBA::Long m_idMenuScripts_EditLocal;
    CORBA::Long m_idMenuScripts_Reload;
    CORBA::Long m_idMenuHelp;
    CORBA::Long m_idMenuHelp_About;
    CORBA::Long m_idMenuHelp_Using;

    /**
     * Pointer to the last popup menu.
     * Since only one popup menu can be opened at once, its pointer is stored here.
     * Delete the old one before you store a pointer to anotheron here.
     * May be 0L.
     */
    QPopupMenu *m_pPopupMenu;

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
    
    /**
     * Pointer to the part that this GUI is belonging to.
     */
    Part_impl *m_pPart;
    KSpreadDoc *m_pDoc;
  
    QPopupMenu *viewMenu;
    int m_pShowPageBordersID;

  // AutoFillDialog *_autoFillDialog;             

  // KFileSelect *openDlg;
  // KFileSelect *saveDlg;

    static KSpreadScripts *m_pGlobalScriptsDialog;
    static KSpreadPaperLayout* m_pPaperLayoutDialog;

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
};

#endif


