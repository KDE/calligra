#ifndef __kspread_canvas_h__
#define __kspread_canvas_h__

#include <qlineedit.h>
#include <qwidget.h>
#include <qpoint.h>
#include <qrect.h>
#include <qstring.h>
#include <qpen.h>
#include <qtooltip.h>

#include <koQueryTrader.h>
#include <kspread_view.h>

class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadTable;
class KSpreadDoc;
class KSpreadPoint;
class KSpreadRange;
class KSpreadCellEditor;
class KSpreadCell;


class QPainter;
class QLabel;

#define YBORDER_WIDTH 50
#define XBORDER_HEIGHT 20


 /**
 * A widget that allows the user to enter an arbitrary
 * cell location to goto or cell selection to highlight
 */
class KSpreadLocationEditWidget : public QLineEdit
{
	Q_OBJECT
public:
	KSpreadLocationEditWidget( QWidget *_parent, KSpreadView * _canvas );

protected:
	virtual void keyPressEvent( QKeyEvent * _ev );
private:
	KSpreadView * m_pView;
signals:
	void gotoLocation( int, int );
};

/**
 * The widget that appears above the table and allows to
 * edit the cells content.
 */
class KSpreadEditWidget : public QLineEdit
{
    Q_OBJECT
public:
    KSpreadEditWidget( QWidget *parent, KSpreadCanvas *canvas,
                       QButton *cancelButton, QButton *okButton);

    virtual void setText( const QString& t );

    // Go into edit mode (enable the buttons)
    void setEditMode( bool mode );

public slots:
    void slotAbortEdit();
    void slotDoneEdit();

protected:
    virtual void keyPressEvent ( QKeyEvent* _ev );
    virtual void focusOutEvent( QFocusEvent* ev );

private:
    QButton* m_pCancelButton;
    QButton* m_pOkButton;
    KSpreadCanvas* m_pCanvas;
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
    enum EditorType { CellEditor, FormulaEditor, EditWidget };

    KSpreadCanvas( QWidget *_parent, KSpreadView *_view, KSpreadDoc* _doc );

    /**
     * Called from @ref KSpreadView to complete the construction. Has to
     * be called before any other method on this object may be invoced.
     */
    void init();

    void insertFormulaChar(int c);

    KSpreadCellEditor* editor() { return m_pEditor ; }

    // ###### Torben: Many of these functions are not used or can be made private
    QPoint chooseMarker() const { return QPoint( m_i_chooseMarkerColumn, m_i_chooseMarkerRow ); }
    int chooseMarkerColumn() const { return m_i_chooseMarkerColumn; }
    int chooseMarkerRow() const { return m_i_chooseMarkerRow; }
    /**
     * Sets the marker and redraws it if it is visible.
     * It does not scroll the canvas. If you want to do that, use
     * @ref #chooseGotoLocation.
     *
     * In addition it does not handel selection
     * of multiple cells. After calling setChooseMarker() the selection will
     * consist of the single cell given by @p p.
     */
    void setChooseMarker( const QPoint& p );
    /**
     * Internal. DONT USE.
     */
    void setChooseMarkerColumn( int _c ) {  m_i_chooseMarkerColumn = _c; }
    /**
     * Internal. DONT USE.
     */
    void setChooseMarkerRow( int _r ) {  m_i_chooseMarkerRow = _r;  }
    /**
     * Move the choose selection. That may include switching the table.
     * The canvas is scrolled to the appropriate position if needed.
     *
     * @param make_select determines wether this move if the marker is part of a
     *                    selection, that means: The user holds the shift key and
     *                    moves the cursor keys.
     */
    void chooseGotoLocation( int x, int y, KSpreadTable* table = 0, bool make_select = FALSE );
    /**
     * Internal. DONT USE.
     */
    void showChooseMarker(){if(choose_visible==true) {return;}
    			drawChooseMarker();choose_visible=true;}
    /**
     * Internal. DONT USE.
     */
    void hideChooseMarker(){if(choose_visible==false) {return;}
    			drawChooseMarker();choose_visible=false;}
    /**
     * Internal. DONT USE.
     */
    bool isChooseMarkerVisible() const { return choose_visible; }

    /**
     * Called from @ref KSpreadView::slotChangeChooseSelection to
     * draw the selection with a rubber band.
     */
    void updateChooseMarker( const QRect& _old, const QRect& _new );
    /**
     * If the user chooses some cells during editing a formular, then
     * this function returns the length of the textual representation.
     * For example the user selects "Table1!A1:B2" then this function
     * returns 12.
     */
    int chooseTextLen() const { return length_namecell; }

    QPoint marker() const;
    int markerColumn() const;
    int markerRow() const;

    void updateCellRect( const QRect &_rect );
    void updateSelection( const QRect& _old, const QRect& _old_marker );

    const QPen& defaultGridPen() { return m_defaultGridPen; }

    double zoom() { return m_pView->zoom(); }

    int xOffset() { return m_iXOffset; }
    int yOffset() { return m_iYOffset; }

    const KSpreadTable* activeTable() const;
    KSpreadTable* activeTable();
    KSpreadTable* findTable( const QString& _name );

    /**
     * A convenience function.
     */
    void gotoLocation( const KSpreadRange & _range );
    /**
     * A convenience function.
     */
    void gotoLocation( const KSpreadPoint& _cell );
    /**
     * Move the cursor to the specified cell. This may include switching
     * the table. In addition @ref #KSpreadView::updateEditWidget is called.
     *
     * @param make_select determines wether this move of the marker is part of a
     *                    selection, that means: The user holds the shift key and
     *                    moves the cursor keys. In this case the selection is
     *                    updated accordingly.
     */
    void gotoLocation( int x, int y, KSpreadTable* table = 0, bool make_select = FALSE,bool move_into_area=false);

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
     * @param saveChanges if true, the edited text is stored in the cell.
     *                    if false, the changes are discarded.
     */
    void deleteEditor( bool saveChanges );

    /**
     * Called from @ref KSpreadEditWidget and KSpreadCellEditor
     * if they loose the focus becuase the user started a "choose selection".
     * This is done because the editor wants to get its focus back afterwards.
     * But somehow KSpreadCanvas must know whether the EditWidget or the CellEditor
     * lost the focus when the user clicked on the canvas.
     */
    void setLastEditorWithFocus( EditorType type ) { m_focusEditorType = type; }

    /**
     * Switches to choose mode and sets the inital selection to the
     * position returned by @ref #marker.
     */
    void startChoose();
    /**
     * Switches to choose mode and sets the inital @p selection.
     */
    void startChoose( const QRect& selection );
    void endChoose();

    /**
    * Adjust a area in height and width
    */
    void adjustArea();

    void equalizeRow();
    void equalizeColumn();

    void updatePosWidget();

    // Created by the view since it's layout is managed there,
    // but is in fact a sibling of the canvas, which needs to know about it.
    void setEditWidget( KSpreadEditWidget * ew ) { m_pEditWidget = ew; }

    KSpreadView* view() { return m_pView; }
    KSpreadDoc* doc() { return m_pDoc; }

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
    virtual void mouseDoubleClickEvent( QMouseEvent* );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusInEvent( QFocusEvent* );
    virtual void focusOutEvent( QFocusEvent* );

private:
    virtual void chooseMousePressEvent( QMouseEvent* _ev );
    virtual void chooseMouseReleaseEvent( QMouseEvent* _ev );
    virtual void chooseMouseMoveEvent( QMouseEvent* _ev );

    KSpreadHBorder* hBorderWidget();
    KSpreadVBorder* vBorderWidget();
    QScrollBar* horzScrollBar();
    QScrollBar* vertScrollBar();
    KSpreadEditWidget* editWidget() { return m_pEditWidget; }

    void drawChooseMarker( );
    void drawChooseMarker( const QRect& );

    /**
     * @see #setLastEditorWithFocus
     */
    EditorType lastEditorWithFocus() const { return m_focusEditorType; }

    /**
     * Hides the marker. Hiding it multiple times means that it has to be shown ( using @ref #showMarker ) multiple times
     * to become visible again. This function is optimized since it does not create a new painter.
     */
    // void hideMarker( QPainter& );
    // void showMarker( QPainter& );

    // void drawMarker( QPainter * _painter = 0L );

    bool choose_visible;
    int  length_namecell;
    int  length_text;

    KSpreadView *m_pView;
    KSpreadDoc* m_pDoc;

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
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;

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

    KSpreadLocationEditWidget *m_pPosWidget;
    KSpreadEditWidget *m_pEditWidget;
    KSpreadCellEditor *m_pEditor;

    /**
     * Used to draw the grey grid that is usually only visible on the
     * screen, but not by printing on paper.
     */
    QPen m_defaultGridPen;

    // int m_iMarkerColumn;
    // int m_iMarkerRow;
    /**
     * A value of 1 means that it is visible, every lower value means it is
     * made invisible multiple times.
     *
     * @see #hideMarker
     * @see #showMarker
     */
    // int m_iMarkerVisible;

    int m_i_chooseMarkerRow;
    int m_i_chooseMarkerColumn;

    /**
     * Is true if the user is to choose a cell.
     *
     * @see #startChoose
     * @see #endChoose
     * @see KSpreadAssistant2
     */
    bool m_bChoose;
    /**
     * If a choose selection is started (@ref #startChoose) the current
     * table is saved here.
     */
    KSpreadTable* m_chooseStartTable;

    // int m_choosePos;

    /**
     * @see #setLastEditorWithFocus
     * @see #lastEditorWithFocus
     */
    EditorType m_focusEditorType;
};

/**
 */
class KSpreadHBorder : public QWidget
{
    Q_OBJECT
public:
    KSpreadHBorder( QWidget *_parent, KSpreadCanvas *_canvas, KSpreadView *_view  );

    int markerColumn() { return  m_iSelectionAnchor; }
    void resizeColumn( int resize, int nb = -1,bool makeUndo=true );
    void adjustColumn( int _col = -1,bool makeUndo=true );
    void equalizeColumn( int resize);

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    void paintSizeIndicator( int mouseX, bool firstTime );

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
     * The user may resize columns by dragging the mouse around in the HBorder widget.
     * If he is doing that right now, this flag is TRUE.
     */
    bool m_bResize;

    /**
     * The column over which the user pressed the mouse button.
     * The user may resize columns by dragging the mouse around the XBorder widget.
     * This is the column over which he pressed the mouse button. This column is going
     * to be resized.
      */
    int m_iResizedColumn;

    /**
     * Last position of the mouse, when resizing.
     */
    int m_iResizePos;

    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;
};

/**
 */
class KSpreadVBorder : public QWidget
{
    Q_OBJECT
public:
    KSpreadVBorder( QWidget *_parent, KSpreadCanvas *_canvas, KSpreadView *_view );

    int markerRow() { return  m_iSelectionAnchor; }
    void resizeRow( int resize, int nb = -1,bool makeUndo=true );
    void adjustRow( int _row = -1,bool makeUndo=true );
    void equalizeRow( int resize);

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    void paintSizeIndicator( int mouseY, bool firstTime );

    KSpreadCanvas *m_pCanvas;
    KSpreadView *m_pView;
    bool m_bSelection;
    int m_iSelectionAnchor;
    bool m_bResize;
    int m_iResizedRow;
    int m_iResizePos;
    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;
};

class KSpreadToolTip : public QToolTip
{
public:
    KSpreadToolTip( KSpreadCanvas* canvas );

protected:
    /**
     * @reimp
     */
    void maybeTip( const QPoint& p );

private:
    KSpreadCanvas* m_canvas;
};

#endif
