/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEVIEW_H
#define KEXITABLEVIEW_H

#include <Q3ScrollView>
#include <QVariant>
#include <QToolTip>
#include <q3header.h>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include <kdebug.h>

#include <widget/dataviewcommon/kexitableviewdata.h>
#include "kexitableedit.h"
#include <kexiutils/tristate.h>
#include <widget/utils/kexirecordnavigator.h>
#include <widget/utils/kexisharedactionclient.h>
#include <widget/dataviewcommon/kexidataawareobjectiface.h>
#include <core/KexiRecordNavigatorHandler.h>

class QPrinter;
class QPrintDialog;

class KAction;

class KexiTableView;
class KexiTableEdit;
class KexiTableViewPrivate;


//! minimum column width in pixels
#define KEXITV_MINIMUM_COLUMN_WIDTH 10

//! @short KexiTableView class provides a widget for displaying data in a tabular view.
/*! @see KexiFormScrollView
*/
class KEXIDATATABLE_EXPORT KexiTableView :
            public Q3ScrollView,
            public KexiRecordNavigatorHandler,
            public KexiSharedActionClient,
            public KexiDataAwareObjectInterface,
            public KexiDataItemChangesListener
{
    Q_OBJECT
    KEXI_DATAAWAREOBJECTINTERFACE
public:

    /*! Defines table view's detailed appearance settings. */
    class KEXIDATATABLE_EXPORT Appearance
    {
    public:
        Appearance(QWidget *widget = 0);

        /*! base color for cells, default is "Base" color for application's
         current active palette */
        QColor baseColor;

        /*! text color for cells, default is "Text" color for application's
         current active palette */
        QColor textColor;

        /*! border color for cells, default is QColor(200,200,200) */
        QColor borderColor;

        /*! empty area color, default is "Base" color for application's
         current active palette */
        QColor emptyAreaColor;

        /*! alternate background color, default is KGlobalSettings::alternateBackgroundColor() */
        QColor alternateBackgroundColor;

        /*! true if background altering should be enabled, true by default */
        bool backgroundAltering;

        /*! true if full-record-selection mode is set,
         what means that all cells of the current record are always selected, instead of single cell.
         This mode is usable for read-only table views, when we're interested only in navigating
         by records. False by default, even for read-only table views.
        */
        bool fullRecordSelection;

        /*! true if fullgrid is enabled. True by default.
         It is set to false for comboboxpopup table, to mimic original
         combobox look and feel. */
        bool gridEnabled;

        /*! \if the navigation panel is enabled (visible) for the view.
         True by default. */
        bool navigatorEnabled;

        /*! true if "record highlight" behaviour is enabled. False by default. */
        bool recordHighlightingEnabled;

        /*! true if "record highlight over " behaviour is enabled. False by default. */
        bool recordMouseOverHighlightingEnabled;

        /*! true if selection of a record should be kept when a user moved mouse
         pointer over other records. Makes only sense when recordMouseOverHighlightingEnabled is true.
         True by default. It is set to false for comboboxpopup table, to mimic original
         combobox look and feel. */
        bool persistentSelections;

        /*! color for record highlight, default is intermediate (33%/60%) between
         active highlight and base color. */
        QColor recordHighlightingColor;

        /*! color for text under record highlight, default is the same as textColor.
         Used when recordHighlightingEnabled is true; */
        QColor recordHighlightingTextColor;

        /*! color for record highlight for mouseover, default is intermediate (20%/80%) between
         active highlight and base color. Used when recordMouseOverHighlightingEnabled is true. */
        QColor recordMouseOverHighlightingColor;

        /*! color for text under record highlight for mouseover, default is the same as textColor.
         Used when recordMouseOverHighlightingEnabled is true; */
        QColor recordMouseOverHighlightingTextColor;

        /*! Like recordMouseOverHighlightingColor but for areas painted with alternate color.
         This is computed using active highlight color and alternateBackgroundColor. */
        QColor recordMouseOverAlternateHighlightingColor;
    };

    KexiTableView(KexiTableViewData* data = 0, QWidget* parent = 0, const char* name = 0);
    virtual ~KexiTableView();

    //! redeclared to avoid conflict with private QWidget::data
    inline KexiTableViewData *data() const {
        return KexiDataAwareObjectInterface::data();
    }

    /*! \return current appearance settings */
    const Appearance& appearance() const;

    /*! Sets appearance settings. Table view is updated automatically. */
    void setAppearance(const Appearance& a);

    /*! \return string displayed for column's header \a colNum */
    QString columnCaption(int colNum) const;

    /*! Convenience function.
     \return field object that define column \a colNum or NULL if there is no such column */
    KexiDB::Field* field(int colNum) const;

    /*! Reimplementation for KexiDataAwareObjectInterface */
    virtual void setSpreadSheetMode();

    /*! \return true if vertical scrollbar's tooltips are enabled (true by default). */
//moved bool scrollbarToolTipsEnabled() const;

    /*! Enables or disables vertical scrollbar's. */
//moved void setScrollbarToolTipsEnabled(bool set);

    /*! \return maximum number of records that can be displayed per one "page"
     for current table view's size. */
    virtual int rowsPerPage() const;

    QRect cellGeometry(int row, int col) const;
    int columnWidth(int col) const;
    int rowHeight() const;
    int columnPos(int col) const;
    int rowPos(int row) const;
    int columnAt(int pos) const;
    int rowAt(int pos, bool ignoreEnd = false) const;

    /*! \return last row visible on the screen (counting from 0).
     The returned value is guaranteed to be smaller or equal to currentRow() or -1
     if there are no rows. */
    virtual int lastVisibleRow() const;

    /*! Redraws specified cell. */
    virtual void updateCell(int row, int col);

    /*! Redraws the current cell. Implemented after KexiDataAwareObjectInterface. */
    virtual void updateCurrentCell();

    /*! Redraws all cells of specified row. */
    virtual void updateRow(int row);

    bool editableOnDoubleClick() const;
    void setEditableOnDoubleClick(bool set);

    //! \return true if the vertical header is visible
    bool verticalHeaderVisible() const;

    //! Sets vertical header's visibility
    void setVerticalHeaderVisible(bool set);

    //! \return true if the horizontal header is visible
    bool horizontalHeaderVisible() const;

    //! Sets horizontal header's visibility
    void setHorizontalHeaderVisible(bool set);

#ifndef KEXI_NO_PRINT
    // printing
// void  setupPrinter(KPrinter &printer, QPrintDialog &printDialog);
    void  print(QPrinter &printer, QPrintDialog &printDialog);
#endif

    // reimplemented for internal reasons
    virtual QSizePolicy sizePolicy() const;
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    /*! Reimplemented to update cached fonts and row sizes for the painter. */
    void setFont(const QFont &f);

    virtual QSize tableSize() const;

    void emitSelected();

    //! single shot after 1ms for contents updating
    void triggerUpdate();

    enum ScrollDirection {
        ScrollUp,
        ScrollDown,
        ScrollLeft,
        ScrollRight
    };

    virtual bool eventFilter(QObject *o, QEvent *e);

    //! Initializes standard editor cell editor factories. This is called internally, once.
    static void initCellEditorFactories();

    /*! \return highlighted record number or -1 if no record is highlighted.
     Makes sense if record highlighting is enabled.
     @see Appearance::recordHighlightingEnabled setHighlightedRecord() */
    int highlightedRecord() const;

    KexiDB::RecordData *highlightedItem() const;

    /*! \return vertical scrollbar. Implemented for KexiDataAwareObjectInterface. */
    virtual QScrollBar* verticalScrollBar() const {
        return Q3ScrollView::verticalScrollBar();
    }

public slots:
    virtual void setData(KexiTableViewData *data, bool owner = true) {
        KexiDataAwareObjectInterface::setData(data, owner);
    }

    virtual void clearColumnsInternal(bool repaint);

    /*! Adjusts \a colNum column's width to its (current) contents.
     If \a colNum == -1, all columns' width is adjusted. */
    void adjustColumnWidthToContents(int colNum = -1);

    //! Sets width of column width to \a width.
    void setColumnWidth(int col, int width);

    /*! If \a set is true, \a colNum column is resized to take full possible width.
     If \a set is false, no automatic resize will be performed.
     If \a colNum is -1, all columns are equally resized, when needed, to take full possible width.
     This method behaves like QHeader::setStretchEnabled ( bool b, int section ). */
    void setColumnStretchEnabled(bool set, int colNum);

    /*! Maximizes widths of columns selected by \a columnList, so the horizontal
     header has maximum overall width. Each selected column's width will be increased
     by the same value. Does nothing if \a columnList is empty or there is no free space
     to resize columns. If this table view is not visible, resizing will be performed on showing. */
    void maximizeColumnsWidth(const QList<int> &columnList);

    /*! Adjusts the size of the sections to fit the size of the horizontal header
     as completely as possible. Only sections for which column stretch is enabled will be resized.
     \sa setColumnStretchEnabled() QHeader::adjustHeaderSize() */
    void adjustHorizontalHeaderSize();

    /*! Sets highlighted record number or -1 if no record has to be highlighted.
     Makes sense if record highlighting is enabled.
     @see Appearance::recordHighlightingEnabled */
    void setHighlightedRecord(int record);

    /*! Sets no record that will be highlighted. Equivalent to setHighlightedRecord(-1). */
    inline void clearHighlightedRecord() {
        setHighlightedRecord(-1);
    }

    /*! Ensures that cell at \a row and \a col is visible.
     If \a col is -1, current column number is used. \a row and \a col (if not -1) must
     be between 0 and rows() (or cols() accordingly). */
    virtual void ensureCellVisible(int row, int col/*=-1*/);

// void   gotoNext();
//js int   findString(const QString &string);

    /*! Deletes currently selected record; does nothing if no record
     is currently selected. If record is in edit mode, editing
     is cancelled before deleting.  */
    virtual void deleteCurrentRow() {
        KexiDataAwareObjectInterface::deleteCurrentRow();
    }

    /*! Inserts one empty row above row \a row. If \a row is -1 (the default),
     new row is inserted above the current row (or above 1st row if there is no current).
     A new item becomes current if row is -1 or if row is equal currentRow().
     This method does nothing if:
     -inserting flag is disabled (see isInsertingEnabled())
     -read-only flag is set (see isReadOnly())
     \ return inserted row's data
    */
    virtual KexiDB::RecordData *insertEmptyRow(int pos = -1) {
        return KexiDataAwareObjectInterface::insertEmptyRow(pos);
    }

    /*! Used when Return key is pressed on cell or "+" nav. button is clicked.
     Also used when we want to continue editing a cell after "invalid value" message
     was displayed (in this case, \a setText is usually not empty, what means
     that text will be set in the cell replacing previous value).
    */
    virtual void startEditCurrentCell(const QString& setText = QString()) {
        KexiDataAwareObjectInterface::startEditCurrentCell(setText);
    }

    /*! Deletes currently selected cell's contents, if allowed.
     In most cases delete is not accepted immediately but "record editing" mode is just started. */
    virtual void deleteAndStartEditCurrentCell() {
        KexiDataAwareObjectInterface::deleteAndStartEditCurrentCell();
    }

    /*! Cancels row editing All changes made to the editing
     row during this current session will be undone.
     \return true on success or false on failure (e.g. when editor does not exist) */
    virtual bool cancelRowEdit() {
        return KexiDataAwareObjectInterface::cancelRowEdit();
    }

    /*! Accepts row editing. All changes made to the editing
     row during this current session will be accepted (saved).
     \return true if accepting was successful, false otherwise
     (e.g. when current row contain data that does not meet given constraints). */
    virtual bool acceptRowEdit() {
        return KexiDataAwareObjectInterface::acceptRowEdit();
    }

    /*! Specifies, if this table view automatically accepts
     row editing (using acceptRowEdit()) on accepting any cell's edit
     (i.e. after acceptEditor()). \sa acceptsRowEditAfterCellAccepting() */
    virtual void setAcceptsRowEditAfterCellAccepting(bool set) {
        KexiDataAwareObjectInterface::setAcceptsRowEditAfterCellAccepting(set);
    }

    /*! Specifies, if this table accepts dropping data on the rows.
     If enabled:
     - dragging over row is indicated by drawing a line at bottom side of this row
     - dragOverRow() signal will be emitted on dragging,
      -droppedAtRow() will be emitted on dropping
     By default this flag is set to false. */
    virtual void setDropsAtRowEnabled(bool set) {
        KexiDataAwareObjectInterface::setDropsAtRowEnabled(set);
    }

    virtual bool cancelEditor() {
        return KexiDataAwareObjectInterface::cancelEditor();
    }
    virtual bool acceptEditor() {
        return KexiDataAwareObjectInterface::acceptEditor();
    }

signals:
    void dataSet(KexiTableViewData *data);

    void itemSelected(KexiDB::RecordData *);
    void cellSelected(int col, int row);

    void itemReturnPressed(KexiDB::RecordData *, int row, int col);
    void itemDblClicked(KexiDB::RecordData *, int row, int col);
    void itemMouseReleased(KexiDB::RecordData *, int row, int col);

    void dragOverRow(KexiDB::RecordData *record, int row, QDragMoveEvent* e);
    void droppedAtRow(KexiDB::RecordData *record, int row, QDropEvent *e, KexiDB::RecordData*& newRecord);

    /*! Data has been refreshed on-screen - emitted from initDataContents(). */
    void dataRefreshed();

    void itemChanged(KexiDB::RecordData *, int row, int col);
    void itemChanged(KexiDB::RecordData *, int row, int col, QVariant oldValue);
    void itemDeleteRequest(KexiDB::RecordData *, int row, int col);
    void currentItemDeleteRequest();
    //! Emitted for spreadsheet mode when an item was deleted and a new item has been appended
    void newItemAppendedForAfterDeletingInSpreadSheetMode();
// void addRecordRequest();
// void contextMenuRequested(KexiDB::RecordData *,  int row, int col, const QPoint &);
    void sortedColumnChanged(int col);

    //! emmited when row editing is started (for updating or inserting)
    void rowEditStarted(int row);

    //! emmited when row editing is terminated (for updating or inserting)
    //! no matter if accepted or not
    void rowEditTerminated(int row);

    //! emmited when state of 'save/cancel record changes' actions should be updated.
    void updateSaveCancelActions();
    
    //! Emitted in initActions() to force reload actions
    //! You should remove existing actions and add them again.
    void reloadActions();

protected slots:
    virtual void slotDataDestroying() {
        KexiDataAwareObjectInterface::slotDataDestroying();
    }

    virtual void slotRowsDeleted(const QList<int> &);

    //! updates display after many rows deletion
    void slotColumnWidthChanged(int col, int os, int ns);

    void slotSectionHandleDoubleClicked(int section);

    void slotUpdate();
    //! implemented because we needed this as slot
    virtual void sortColumnInternal(int col, int order = 0) {
        KexiDataAwareObjectInterface::sortColumnInternal(col, order);
    }

    void slotAutoScroll();

    //! internal, used when top header's size changed
    void slotTopHeaderSizeChange(int section, int oldSize, int newSize);

    //! receives a signal from cell editors
    void slotEditRequested();

    /*! Reloads data for this widget.
     Handles KexiTableViewData::reloadRequested() signal. */
    virtual void reloadData();

    //! Handles KexiTableViewData::rowRepaintRequested() signal
    virtual void slotRowRepaintRequested(KexiDB::RecordData& record);

    //! Handles KexiTableViewData::aboutToDeleteRow() signal. Prepares info for slotRowDeleted().
    virtual void slotAboutToDeleteRow(KexiDB::RecordData& record, KexiDB::ResultInfo* result, bool repaint) {
        KexiDataAwareObjectInterface::slotAboutToDeleteRow(record, result, repaint);
    }

    //! Handles KexiTableViewData::rowDeleted() signal to repaint when needed.
    virtual void slotRowDeleted() {
        KexiDataAwareObjectInterface::slotRowDeleted();
    }

    //! Handles KexiTableViewData::rowInserted() signal to repaint when needed.
    virtual void slotRowInserted(KexiDB::RecordData *record, bool repaint) {
        KexiDataAwareObjectInterface::slotRowInserted(record, repaint);
    }

    //! Like above, not db-aware version
    virtual void slotRowInserted(KexiDB::RecordData *record, uint row, bool repaint) {
        KexiDataAwareObjectInterface::slotRowInserted(record, row, repaint);
    }

    /*! Handles verticalScrollBar()'s valueChanged(int) signal.
     Called when vscrollbar's value has been changed. */
    virtual void vScrollBarValueChanged(int v) {
        KexiDataAwareObjectInterface::vScrollBarValueChanged(v);
    }

#if 0 // 2.0
    /*! Handles sliderReleased() signal of the verticalScrollBar(). Used to hide the "record number" tooltip. */
/*replaced by QToolTip    virtual void vScrollBarSliderReleased() {
        KexiDataAwareObjectInterface::vScrollBarSliderReleased();
    }*/

    /*! Handles timeout() signal of the m_scrollBarTipTimer. If the tooltip is visible,
     m_scrollBarTipTimerCnt is set to 0 and m_scrollBarTipTimerCnt is restarted;
     else the m_scrollBarTipTimerCnt is just set to 0.*/
    virtual void scrollBarTipTimeout() {
        KexiDataAwareObjectInterface::scrollBarTipTimeout();
    }
#endif
    //! for navigator
    virtual void moveToRecordRequested(uint r);
    virtual void moveToLastRecordRequested();
    virtual void moveToPreviousRecordRequested();
    virtual void moveToNextRecordRequested();
    virtual void moveToFirstRecordRequested();
    virtual void addNewRecordRequested() {
        KexiDataAwareObjectInterface::addNewRecordRequested();
    }
/* not needed after #2010-01-05 fix    void slotContentsMoving(int x, int y);*/
protected:
    /*! Reimplementation for KexiDataAwareObjectInterface
     Initializes data contents (resizes it, sets cursor at 1st row).
     Called on setData(). Also called once on show event after
     reloadRequested() signal was received from KexiTableViewData object. */
    virtual void initDataContents();

    /*! Implementation for KexiDataAwareObjectInterface.
     Updates widget's contents size using QScrollView::resizeContents()
     depending on tableSize(). */
    virtual void updateWidgetContentsSize();

    /*! Reimplementation for KexiDataAwareObjectInterface */
    virtual void clearVariables();

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual int currentLocalSortingOrder() const;

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual int currentLocalSortColumn() const;

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual void setLocalSortingOrder(int col, int order);

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual void updateGUIAfterSorting();

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual void updateWidgetScrollBars() {
        updateScrollBars();
    }

// /*! Implementation for KexiDataAwareObjectInterface */
// virtual void emitSortedColumnChanged(int col) { emit sortedColumnChanged(col); }

// /*! Implementation for KexiDataAwareObjectInterface */
// virtual void emitRowEditTerminated(int row) { emit rowEditTerminated(row); }

    /*! Implementation for KexiDataAwareObjectInterface.
     Adds another section within the horizontal header. */
    virtual void addHeaderColumn(const QString& caption, const QString& description,
                                 const QIcon& icon, int size);

    /*! @internal \return true if the row defined by \a record has default
     value at column \a col. If this is the case and \a value is not NULL,
     *value is set to the default value. */
    bool isDefaultValueDisplayed(KexiDB::RecordData *record, int col, QVariant* value = 0);

    //! painting and layout
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    void createBuffer(int width, int height);
    void paintCell(QPainter* p, KexiDB::RecordData *record, int col, int row, const QRect &cr, bool print = false);
    void paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch);
    void updateGeometries();

    QPoint contentsToViewport2(const QPoint &p);
    void contentsToViewport2(int x, int y, int& vx, int& vy);
    QPoint viewportToContents2(const QPoint& vp);

    // event handling
    virtual void contentsMousePressEvent(QMouseEvent* e);
    virtual void contentsMouseReleaseEvent(QMouseEvent* e);
    //! @internal called by contentsMouseOrEvent() contentsMouseReleaseEvent() to move cursor
    bool handleContentsMousePressOrRelease(QMouseEvent* e, bool release);
    virtual void contentsMouseMoveEvent(QMouseEvent* e);
    virtual void contentsMouseDoubleClickEvent(QMouseEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void contextMenuEvent(QContextMenuEvent* e);
    virtual void focusInEvent(QFocusEvent* e);
    virtual void focusOutEvent(QFocusEvent* e);
    virtual void resizeEvent(QResizeEvent* e);
    virtual void viewportResizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void contentsDragMoveEvent(QDragMoveEvent *e);
    virtual void contentsDropEvent(QDropEvent *e);
    virtual void viewportDragLeaveEvent(QDragLeaveEvent *e);
    virtual void paletteChange(const QPalette &oldPalette);

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual KexiDataItemInterface *editor(int col, bool ignoreMissingEditor = false);

    inline KexiTableEdit *tableEditorWidget(int col, bool ignoreMissingEditor = false) {
        return dynamic_cast<KexiTableEdit*>(editor(col, ignoreMissingEditor));
    }

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual void editorShowFocus(int row, int col);

    //! Creates editors and shows it, what usually means the beginning of a cell editing
    virtual void createEditor(int row, int col, const QString& addText = QString(),
                              bool removeOld = false);

    bool focusNextPrevChild(bool next);

    /*! Used in key event: \return true if event \a e should execute action \a action_name.
     Action shortcuts defined by shortCutPressed() are reused, if present, and if \a e matches
     given action's shortcut - false is returned (beause action is already performed at main
     window's level).
    */
    bool shortCutPressed(QKeyEvent *e, const QString &action_name);

#if 0 //we have now KexiActionProxy
    /*! Updates visibility/accesibility of popup menu items,
    returns false if no items are visible after update. */
    bool updateContextMenu();
#endif

    /*! Shows context menu at \a pos for selected cell
     if menu is configured,
     else: contextMenuRequested() signal is emmited.
     Method used in contentsMousePressEvent() (for right button)
     and keyPressEvent() for Qt::Key_Menu key.
     If \a pos is QPoint(-1,-1) (the default), menu is positioned below the current cell.
    */
    void showContextMenu(const QPoint& pos = QPoint(-1, -1));

    /*! internal */
    inline void paintRow(KexiDB::RecordData *record,
                         QPainter *pb, int r, int rowp, int cx, int cy,
                         int colfirst, int collast, int maxwc);

    virtual void setHBarGeometry(QScrollBar & hbar, int x, int y, int w, int h);

    //! Setups navigator widget
    void setupNavigator();

    //! internal, to determine valid row number when navigator text changed
    int validRowNumber(const QString& text);

    /*! Reimplementation for KexiDataAwareObjectInterface
     (viewport()->setFocus() is just added) */
    virtual void removeEditor();

    //! Internal: updated sched fonts for painting.
    void updateFonts(bool repaint = false);

    /*! @internal Changes bottom margin settings, in pixels.
     At this time, it's used by KexiComboBoxPopup to decrease margin for popup's table. */
    void setBottomMarginInternal(int pixels);

    virtual void updateWidgetContents() {
        update();
    }

    //! Copy current selection to a clipboard (e.g. cell)
    virtual void copySelection();

    //! Cut current selection to a clipboard (e.g. cell)
    virtual void cutSelection();

    //! Paste current clipboard contents (e.g. to a cell)
    virtual void paste();

    /*! Used in KexiDataAwareObjectInterface::slotRowDeleted()
     to repaint tow \a row and all visible below. */
    virtual void updateAllVisibleRowsBelow(int row);

    void updateAfterCancelRowEdit();
    void updateAfterAcceptRowEdit();

    /*! Sets \a cellValue if there is a lookup value for the cell \a record.
     Used in KexiTableView::paintCell() and KexiTableViewCellToolTip::maybeTip()
     \return true is \a cellValue has been found. */
    bool getVisibleLookupValue(QVariant& cellValue, KexiTableEdit *edit,
                               KexiDB::RecordData *record, KexiTableViewColumn *tvcol) const;

// //! Called to repaint contents after a row is deleted.
// void repaintAfterDelete();

    /*! Implementation for KexiDataItemChangesListener.
     Reaction for change of \a item. */
    virtual void valueChanged(KexiDataItemInterface* item);

    /*! Implementation for KexiDataItemChangesListener. */
    virtual bool cursorAtNewRow() const;

    QWidget* navPanelWidget() const;
    
    KexiTableViewPrivate * const d;

    class WhatsThis;
//Qt 4 friend class KexiTableItem;
    friend class WhatsThis;
    friend class KexiTableViewCellToolTip;
};

#endif
