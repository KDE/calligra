#ifndef __kspread_canvas_h__
#define __kspread_canvas_h__

#include <qlineedit.h>
#include <qwidget.h>
#include <qpoint.h>
#include <qrect.h>
#include <qstring.h>
#include <qpen.h>

#include <koQueryTypes.h>

class KSpreadView;
class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadTable;
class KSpreadDoc;
class KSpreadPoint;
class KSpreadCellEditor;
class KSpreadCell;

class QLabel;
class QPainter;

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

    bool isActivate() { return active; }
    void setActivate( bool _active) { active= _active;}

public slots:
    void slotAbortEdit();
    void slotDoneEdit();

protected:
    virtual void keyPressEvent ( QKeyEvent* _ev );
    bool active;
    KSpreadView* m_pView;
};

/**
 * The canvas builds a part of the GUI of KSpread.
 * It contains the borders, scrollbars,
 * editwidget and of course it displays the table.
 * Especially most of the user interface logic is implemented here.
 * That means that this class knows what to do when a key is pressed
 * or if the mouse button was clicked.
 */
class KSpreadCanvas : public QWidget
{
    friend KSpreadHBorder;
    friend KSpreadVBorder;
    friend KSpreadView;

    Q_OBJECT
public:
    /**
     * The current action associated with the mouse.
     * Default is 'NoAction'.
     */
    enum MouseActions { NoAction = 0, Mark = 1, ResizeCell = 2, AutoFill = 3 };
    enum EditorType { CellEditor, FormulaEditor };
    /**
     * The possible actions that we expect the user to do.
     * Usually this is 'Default' and tells us that the user may edit
     * the table. If @ref #action is 'InsertChild' or 'InsertChart' then the user must draw
     * a rectangle in order of telling us where to insert the new child.
     */
    enum Actions { DefaultAction, InsertChart };

    KSpreadCanvas( QWidget *_parent, KSpreadView *_view, KSpreadDoc* _doc );

    /**
     * Called from @ref KSpreadView to complete the construction. Has to
     * be called before any other method on this object may be invoced.
     */
    void init();

    void insertFormulaChar(int c);

    KSpreadCellEditor* editor() { return m_pEditor ; }
    void chooseCell( QMouseEvent * _ev );
    void setgotohorz(bool _gotohorz){m_gotohorz=_gotohorz;}
    bool isgotohorz(){return m_gotohorz;}
    void setgotovert(bool _gotovert){m_gotovert=_gotovert;}
    bool isgotovert(){return m_gotovert;}

    						
    int chooseMarkerColumn() { return m_i_chooseMarkerColumn; }
    int chooseMarkerRow() { return m_i_chooseMarkerRow; }
    void setChooseMarkerColumn( int _c ) {  m_i_chooseMarkerColumn = _c; }
    void setChooseMarkerRow( int _r ) {  m_i_chooseMarkerRow = _r;  }


    void showChooseCell(){if(choose_visible==true) {return;}
    			drawChooseMarker();choose_visible=true;}
    void hideChooseCell(){if(choose_visible==false) {return;}
    			drawChooseMarker();choose_visible=false;}			

    void drawChooseMarker( );

    QPoint marker() { return QPoint( m_iMarkerColumn, m_iMarkerRow ); }
    bool isMarkerVisible() { return ( m_iMarkerVisible == 1 ); }
    int markerColumn() { return m_iMarkerColumn; }
    int markerRow() { return m_iMarkerRow; }
    /**
     * Changes the position of the marker. if it is visible, it is hidden, moved and shown again.
     */
    void setMarkerColumn( int _c );
    /**
     * Changes the position of the marker. if it is visible, it is hidden, moved and shown again.
     */
    void setMarkerRow( int _r );
    /**
     * Hides the marker. Hiding it multiple times means that it has to be shown ( using @ref #showMarker ) multiple times
     * to become visible again.
     */
    void hideMarker() { if ( m_iMarkerVisible == 1 ) drawMarker(); m_iMarkerVisible--; }
    void showMarker() { if ( m_iMarkerVisible == 1 ) return; m_iMarkerVisible++; if ( m_iMarkerVisible == 1 ) drawMarker(); }

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
     * This is usually called with '_act' equal KSpreadCanvas::InsertChart.
     */
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

    const QPen& defaultGridPen() { return m_defaultGridPen; }

    float zoom() { return m_fZoom; }

    int xOffset() { return m_iXOffset; }
    int yOffset() { return m_iYOffset; }

    KSpreadTable* activeTable();
    KSpreadTable* findTable( const QString& _name );

    void gotoLocation( const KSpreadPoint& _cell );

    /**
     * Chooses the correct @ref #EditorType by looking at
     * the current cells value. By default CellEditor is chosen.
     */
    void createEditor();
    void createEditor( EditorType type );
    /**
     * Deletes the current cell editor.
     *
     * @see #createEditor
     * @see #editor
     */
    void deleteEditor();

    void startChoose();
    void endChoose();
    /**
    * Ajust a area in height and width
    */
    void ajustArea();
	
public slots:
    void slotScrollVert( int _value );
    void slotScrollHorz( int _value );

    void slotMaxColumn( int _max_column );
    void slotMaxRow( int _max_row );

protected:
    virtual void keyPressEvent ( QKeyEvent* _ev );
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent*);
    virtual void focusInEvent( QFocusEvent* );
    virtual void focusOutEvent( QFocusEvent* );

    KSpreadHBorder* hBorderWidget();
    KSpreadVBorder* vBorderWidget();
    QScrollBar* horzScrollBar();
    QScrollBar* vertScrollBar();
    KSpreadEditWidget* editWidget() { return m_pEditWidget; }

private:
    /**
     * Hides the marker. Hiding it multiple times means that it has to be shown ( using @ref #showMarker ) multiple times
     * to become visible again. This function is optimized since it does not create a new painter.
     */
    void hideMarker( QPainter& );
    void showMarker( QPainter& );

    void drawMarker( QPainter * _painter = 0L );
    /**
     * Redraws a single object.
     */
    void drawCell( QPainter &_painter, KSpreadCell *_cell, int _col, int _row );

    bool choose_visible;
    int  length_namecell;
    int  length_text;
    QString name_tab;

    KSpreadView *m_pView;
    KSpreadDoc* m_pDoc;

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
     * If the @ref m_eAction is InsertChild then this record
     * holds informations about which component we should use here.
     */
    KoDocumentEntry m_actionArgument;

    /**
     * Used to indicate whether the user started drawing a rubber band rectangle.
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
     * Tells whether the user selected more than one cell.
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

    /**
     * If the mouse is over some anchor ( in the sense of HTML anchors )
     * then this one is stored here.
     */
    QString m_strAnchor;

    float m_fZoom;

    /**
     * Horizontal scroll offset of the view.
     */
    int m_iXOffset;
    /**
     * Vertical scroll offset of the view.
     */
    int m_iYOffset;

    QLabel *m_pPosWidget;
    KSpreadEditWidget *m_pEditWidget;
    KSpreadCellEditor *m_pEditor;

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

    int m_i_chooseMarkerRow;
    int m_i_chooseMarkerColumn;
    bool m_gotohorz;
    bool m_gotovert;

    /**
     * Is true if the user is to choose a cell.
     *
     * @ref #startChoose
     * @ref #endChoose
     * @ref KSpreadAssistant2
     */
    bool m_bChoose;
};

/**
 */
class KSpreadHBorder : public QWidget
{
    Q_OBJECT
public:
    KSpreadHBorder( QWidget *_parent, KSpreadCanvas *_canvas, KSpreadView *_view  );
    int markerColumn(){return  m_iSelectionAnchor;}
    void resizeColumn(int resize,int nb=-1);
    void ajustColumn(int _col=-1);
protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );

    KSpreadCanvas *m_pCanvas;
    KSpreadView *m_pView;
    /**
     * Flag that inidicates whether the user wants to mark columns.
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
     * Flag that indicates whether the user resizes a column
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
    KSpreadVBorder( QWidget *_parent, KSpreadCanvas *_canvas, KSpreadView *_view );
    int markerRow(){return  m_iSelectionAnchor;}
    void resizeRow(int resize,int nb=-1);
    void ajustRow(int _row=-1);
protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );

    KSpreadCanvas *m_pCanvas;
    KSpreadView *m_pView;
    bool m_bSelection;
    int m_iSelectionAnchor;
    bool m_bResize;
    int m_iResizeAnchor;
    int m_iResizePos;
};

#endif
