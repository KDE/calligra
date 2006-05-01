/* This file is part of the KDE project

   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2001,2003 David Faure <faure@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KSPREAD_CANVAS
#define KSPREAD_CANVAS

#include <vector>

#include <qlineedit.h>
#include <qtooltip.h>
#include <qpen.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QPaintEvent>
#include <QList>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QEvent>
#include <QDropEvent>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>

#include <ksharedptr.h>

#include <koffice_export.h>

#include "kspread_util.h"

#define YBORDER_WIDTH 50
#define XBORDER_HEIGHT 20

class QWidget;
class QTimer;
class QPainter;
class QLabel;
class QScrollBar;
class KoRect;
class KoPoint;

namespace KSpread
{

class Cell;
class EditWidget;
class Canvas;
class HBorder;
class VBorder;
class Sheet;
class Doc;
class Point;
class Range;
class Region;
class View;
class Selection;
class CellEditor;
class LocationEditWidget;
class ComboboxLocationEditWidget;
class EmbeddedObject;


/**
 * The canvas builds a part of the GUI of KSpread.
 * It contains the borders, scrollbars,
 * editwidget and of course it displays the sheet.
 * Especially most of the user interface logic is implemented here.
 * That means that this class knows what to do when a key is pressed
 * or if the mouse button was clicked.
 */
class KSPREAD_EXPORT Canvas : public QWidget
{
    friend class HBorder;
    friend class VBorder;
    friend class View;

    Q_OBJECT
public:
    /**
     * The current action associated with the mouse.
     * Default is 'NoAction'.
     */
    enum MouseActions
    {
      NoAction,       /**< No mouse action (default) */
      Mark,           /**< Marking action */
      ResizeCell,     /**< Merging cell */
      AutoFill,       /**< Autofilling */
      ResizeSelection /**< Resizing the selection */
    };
    enum EditorType { CellEditor, EditWidget };

    Canvas (View *_view);
    ~Canvas( );

    View* view() const;
    Doc* doc() const;

    KSpread::EditWidget* editWidget() const;
    KSpread::CellEditor* editor() const;

    /**
     * @return the usual selection of cells
     */
    Selection* selectionInfo() const;

    /**
     * @return a selection of cells used in formulas
     */
    Selection* choice() const;

    /**
     * convenience function.
     * @return selection's marker
     * @see Selection::marker()
     */
    QPoint marker() const;

    /**
     * convenience function.
     * @return selection's marker's column
     * @see Selection::marker()
     */
    int markerColumn() const;

    /**
     * convenience function.
     * @return selection's marker's row
     * @see Selection::marker()
     */
    int markerRow() const;

    /**
     * @return the pen, the default grid is painted with (light gray)
     */
    const QPen& defaultGridPen() const;

    /**
     * convenience function
     * @see View::zoom()
     */
    double zoom() const;

    /**
     * @return the width of the columns before the current screen
     */
    double xOffset() const;

    /**
     * @return the height of the rows before the current screen
     */
    double yOffset() const;

    /**
     * Sets the width of the columns before the current screen
     */
    void  setXOffset( double _xOffset );

    /**
     * Sets the height of the rows before the current screen
     */
    void  setYOffset( double _yOffset );

    /**
     * @return a rect indicating which cell range is currently visible onscreen
     */
    QRect visibleCells() const;

    /**
     * @return a pointer to the active sheet
     */
    Sheet* activeSheet() const;

    /**
     * convenience function
     * @see Map::findSheet()
     */
    Sheet* findSheet( const QString& _name ) const;


    /**
     * Validates the selected cell.
     */
    void validateSelection();


    /**
     * Paints all visible cells that have a paint dirty flag set.
     * @see paintEvent()
     */
    void paintUpdates();


    /**
     * Makes sure a cell is visible onscreen by scrolling up/down and left/right
     * @param location the cell coordinates to scroll to
     */
    void scrollToCell(QPoint location) const;

    /**
     * Chooses the correct EditorType by looking at
     * the current cells value. By default CellEditor is chosen.
     */
    void createEditor( bool captureArrowKeys=false );
    bool createEditor( EditorType type, bool addFocus = true, bool captureArrowKeys=false );

    /**
     * Deletes the current cell editor.
     * @see createEditor
     * @see editor
     * @param saveChanges if @c true , the edited text is stored in the cell.
     *                    if @c false , the changes are discarded.
     * @param array if @c true , array formula was entered
     */
    void deleteEditor(bool saveChanges, bool array = false);

    /**
     * Called from EditWidget and CellEditor
     * if they loose the focus because the user started a "choose selection".
     * This is done because the editor wants to get its focus back afterwards.
     * But somehow Canvas must know whether the EditWidget or the CellEditor
     * lost the focus when the user clicked on the canvas.
     */
    void setLastEditorWithFocus( EditorType type );

    /**
     * Switches to choose mode and sets the initial selection to the
     * position returned by marker().
     * Clears the choice.
     */
    void startChoose();

    /**
     * Switches to choose mode and sets the initial @p selection.
     */
    void startChoose( const QRect& selection );

    /**
     * Switches to selection mode.
     * Clear the choice.
     */
    void endChoose();

    /**
     * Switches the choose mode on and off.
     * Does not clear the choice.
     */
    void setChooseMode(bool state);

    /**
     * @return @c true if choose mode is enabled, @c false otherwise
     */
    bool chooseMode() const;

    void equalizeRow();
    void equalizeColumn();

    /**
     * Updates the position widget.
     */
    void updatePosWidget();

    /**
     * Close the cell editor and saves changes.
     * @see deleteEditor()
     */
    void closeEditor();

    // Created by the view since it's layout is managed there,
    // but is in fact a sibling of the canvas, which needs to know about it.
    void setEditWidget( KSpread::EditWidget * ew );

    virtual bool focusNextPrevChild( bool );

    /**
     * Depending on the offset in "zoomed" screen pixels
     * for the horizontal direction,
     * the function returns the steps in unzoomed points
     * for the autoscroll acceleration
     */
    double autoScrollAccelerationX( int offset );

    /**
     * Depending on the offset in "zoomed" screen pixels
     * for the vertical direction,
     * the function returns the steps in unzoomed points
     * for the autoscroll acceleration
     */
    double autoScrollAccelerationY( int offset );

    //TODO: These embedded-object related methods need API documentation!
    EmbeddedObject* getObject( const QPoint &pos, Sheet *_sheet );
    void selectAllObjects();
    void deselectAllObjects();
    void selectObject( EmbeddedObject* );
    void deselectObject( EmbeddedObject* );
    void setMouseSelectedObject(bool b);
    bool isObjectSelected();

    /**
     * @brief Move object by mouse
     *
     * @param pos The position of the mouse
     * @param keepXorYunchanged if true keep x or y position unchanged
     */
    void moveObjectsByMouse( KoPoint &pos, bool keepXorYunchanged );

    //---- stuff needed for resizing ----
    /// resize the m_resizeObject
    void resizeObject( ModifyType _modType, const KoPoint & point, bool keepRatio );

    /// create KPrResizeCmd
    void finishResizeObject( const QString &name, bool layout = true );

    /**
     * @brief Display object above the other objects in editiong mode
     *
     * This is used to bring a single slected object to front, so it is easier
     * to modify.
     *
     * @param object which should be displayed above the other objects
     */
    void raiseObject( EmbeddedObject *object );

    /**
     * @brief Don't display an object above the others
     */
    void lowerObject();

    /**
     * @brief Get the list of objects in the order they should be displayed.
     *
     * This takes into acount the object set in raiseObject so that it is
     * the last one in the list returned (the one that is displayed above all
     * the others).
     *
     * @return List of objects
     */
    void displayObjectList( QList<EmbeddedObject*> &list );

    KoRect objectRect( bool all ) const;

    void repaintObject( EmbeddedObject *obj );

    /**
     * This is intended to copy the selected objects to the clipboard so that
     * they can be pasted into other applications. However, until at least
     * KWord, KSpread, KPresenter, KChart and KFormula have consistant
     * support for copying and pasting of OASIS objects the selected objects
     * will just be copied in the form of raster graphics
     */
    void copyOasisObjects();
    //void insertOasisData();

public slots:
    void slotScrollVert( int _value );
    void slotScrollHorz( int _value );

    void slotMaxColumn( int _max_column );
    void slotMaxRow( int _max_row );

signals:
    void objectSelectedChanged();
    void objectSizeChanged();

protected:
    virtual void keyPressEvent ( QKeyEvent* _ev );

    /**
     * If no cells are marked as dirty before, this method was _not_ called
     * from the end of a KSpread 'operation'. In this case all visible cells
     * will be marked as dirty.
     * Calls paintUpdates(), which repaints the dirty cells.
     * @see Doc::emitBeginOperation(bool)
     * @see Doc::emitEndOperation()
     * @see Sheet::setRegionPaintDirty()
     * @see paintUpdates()
     * @reimp
     */
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusInEvent( QFocusEvent* );
    virtual void focusOutEvent( QFocusEvent* );
    virtual void resizeEvent( QResizeEvent * _ev );
    virtual void dragMoveEvent(QDragMoveEvent * _ev);
    virtual void dropEvent(QDropEvent * _ev);
    virtual void dragLeaveEvent(QDragLeaveEvent * _ev);

    /**
     * Checks to see if there is a size grip for a highlight range at a given position.
     * Note that both X and Y coordinates are UNZOOMED.  To translate from a zoomed coordinate (eg. position of a mouse event) to
     * an unzoomed coordinate, use Doc::unzoomItX and Doc::unzoomItY.  The document object
     * can be accessed via view()->doc()
     * @param x Unzoomed x coordinate to check
     * @param y Unzoomed y coordinate to check
     * @return @c true if there is a size grip at the specified position, @c false otherwise.
     */
    bool highlightRangeSizeGripAt(double x, double y);

private slots:

    /**
     * Scroll canvas when receiving this signal
     */
    void slotAutoScroll(const QPoint &scrollDist);

    void doAutoScroll();

private:
    virtual bool eventFilter( QObject *o, QEvent *e );

    HBorder* hBorderWidget() const;
    VBorder* vBorderWidget() const;
    QScrollBar* horzScrollBar() const;
    QScrollBar* vertScrollBar() const;

    /**
     * Returns the area of the document currently visible in a painter's
     * window, calculated by taking the painter's window() property and
     * translating it by the current x and y offset of the Canvas (taking
     * the zoom level into account)
     */
    QRect painterWindowGeometry( const QPainter& painter ) const;

    /**
     * Enables clipping and removes the areas on the canvas widget occupied by embedded objects from
     * the clip region.  This ensures that subsequent drawing operations using the given painter
     * don't paint over the area occupied by embedded objects
     */
    void clipoutChildren( QPainter& painter ) const;

    /**
     * Returns the range of cells which appear in the specified area of the Canvas widget
     * For example, cellsInArea( QRect(0,0,width(),height()) ) returns a range containing all visible cells
     *
     * @param area The area (in pixels) on the Canvas widget
     */
    QRect cellsInArea( const QRect area ) const;

    /**
     * Paints the children
     */
    void paintChildren( QPainter& painter, QMatrix& matrix );

    /**
     * @see #setLastEditorWithFocus
     */
    EditorType lastEditorWithFocus() const;

private:
  void moveObject( int x, int y, bool key );

  void startTheDrag();

  /* helpers for the paintUpdates function */
  void paintNormalMarker(QPainter& painter, const KoRect &viewRect);

  /**
  * Paint the highlighted ranges of cells.  When the user is editing a formula in a text box, cells and ranges referenced
  * in the formula are highlighted on the canvas.
  * @param painter The painter on which to draw the highlighted ranges
  * @param viewRect The area currently visible on the canvas
  */
  void paintHighlightedRanges(QPainter& painter, const KoRect& viewRect);

  /**
  * Calculates the visible region on the canvas occupied by a range of cells on the currently active sheet.
  * This is used for drawing the thick border around the current selection or highlights around cell range
  * references.
  * The results do not take into account the current zoom factor of the sheet,
  * use Doc::zoomRect on @p visibleRect after calling this function to get a new rectangle taking
  * the zoom level into account.
  * @param sheetArea The range of cells on the current sheet
  * @param visibleRect This is set to the visible region occupied by the given range of cells
  *
  */
  void sheetAreaToVisibleRect( const QRect& sheetArea,
			       KoRect& visibleRect );

  /**
  * Calculates the physical region on the canvas widget occupied by a range of cells on
  * the currently active sheet.
  * Unlike @see sheetAreaToVisibleRect , scrolling the view does not affect sheetAreaToRect.
  *
  * @param sheetArea The range of cells on the current sheet
  * @param visibleRect This is set to the physical region occupied by the given range of cells
  */
  void sheetAreaToRect( const QRect& sheetArea, KoRect& rect );


  /**
   * helper function in drawing the marker and choose marker.
   * @param marker the rectangle that represents the marker being drawn
   *               (cell coordinates)
   * @param viewRect the visible area on the canvas
   * @param positions output parameter where the viewable left, top, right, and
   *                  bottom of the marker will be.  They are stored in the array
   *                  in that order, and take into account cropping due to part
   *                  of the marker being off screen.  This array should have
   *                  at least a size of 4 pre-allocated.
   * @param paintSides booleans indicating whether a particular side is visible.
   *                   Again, these are in the order left, top, right, bottom.
   *                   This should be preallocated with a size of at least 4.
   */
  void retrieveMarkerInfo( const QRect &marker, const KoRect &viewRect,
                           double positions[], bool paintSides[] );




  bool formatKeyPress( QKeyEvent * _ev );

  /** helper method for formatKeyPress */
  bool formatCellByKey(Cell *cell, int key, const QRect &rect);

  void processClickSelectionHandle(QMouseEvent *event);
  void processLeftClickAnchor();


  /** current cursor position, be it marker or choose marker */
  QPoint cursorPos();

  /**
   * returns the rect that needs to be redrawn
   */
  QRect moveDirection(KSpread::MoveTo direction, bool extendSelection);

  void processEnterKey(QKeyEvent *event);
  void processArrowKey(QKeyEvent *event);
  void processEscapeKey(QKeyEvent *event);
  bool processHomeKey(QKeyEvent *event);
  bool processEndKey(QKeyEvent *event);
  bool processPriorKey(QKeyEvent *event);
  bool processNextKey(QKeyEvent *event);
  void processDeleteKey(QKeyEvent *event);
  void processF2Key(QKeyEvent *event);
  void processF4Key(QKeyEvent *event);
  void processOtherKey(QKeyEvent *event);
  bool processControlArrowKey(QKeyEvent *event);

  //void processIMEvent( QIMEvent * event );

  /**
   * Used in choose mode. Shows/hides the editor depending on the selected
   * sheet. Triggers an update of the regions shown in the CellEditor.
   * @see CellEditor::updateChoice()
   */
  void updateEditor();

  /**
   * This function sets the paint dirty flag for a @p changedRegion in a
   * @p sheet .
   * The calculation which cells really should look different with the new
   * selection rather than repainting the entire area has to be done before.
   * @param sheet the sheet, which contains the cells
   * @param changedRegion the cell region to be set as dirty
   */
  void setSelectionChangePaintDirty(Sheet* sheet, const Region& changedRegion);

  /**
   * Determines the cell at @p point and shows its tooltip.
   * @param point the position for which a tooltip is requested
   */
  void showToolTip( const QPoint& point );

private:
  class Private;
  Private* d;
};



/**
 * HBorder
 */
class HBorder : public QWidget
{
    Q_OBJECT
public:
    HBorder( QWidget *_parent, Canvas *_canvas, View *_view  );
    ~HBorder();

    int markerColumn() const { return  m_iSelectionAnchor; }
    void equalizeColumn( double resize );

    void updateColumns( int from, int to );

    QSize sizeHint() const;

private slots:
    void doAutoScroll();

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusOutEvent( QFocusEvent* ev );
    virtual void resizeEvent( QResizeEvent * _ev );
    void paintSizeIndicator( int mouseX, bool firstTime );

private:
    Canvas *m_pCanvas;
    View *m_pView;
    QTimer * m_scrollTimer;

    /**
     * Flag that inidicates whether the user wants to mark columns.
     * The user may mark columns by dragging the mouse around in th XBorder widget.
     * If he is doing that right now, this flag is true. Mention that the user may
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
     * If he is doing that right now, this flag is true.
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

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;

private:
};



/**
 * VBorder
 */
class VBorder : public QWidget
{
    Q_OBJECT
public:
    VBorder( QWidget *_parent, Canvas *_canvas, View *_view );
    ~VBorder();

    int markerRow() const { return  m_iSelectionAnchor; }
    void equalizeRow( double resize );
    void updateRows( int from, int to );

    QSize sizeHint() const;

private slots:
    void doAutoScroll();

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusOutEvent( QFocusEvent* ev );
    void paintSizeIndicator( int mouseY, bool firstTime );

private:
    Canvas *m_pCanvas;
    View *m_pView;
    QTimer * m_scrollTimer;

    bool m_bSelection;
    int m_iSelectionAnchor;
    bool m_bResize;
    int m_iResizedRow;
    int m_iResizePos;
    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;
};

} // namespace KSpread

#endif // KSPREAD_CANVAS
