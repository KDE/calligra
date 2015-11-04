/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXITABLESCROLLAREA_H
#define KEXITABLESCROLLAREA_H

#include "kexidatatable_export.h"
#include <widget/utils/kexirecordnavigator.h>
#include <widget/utils/kexisharedactionclient.h>
#include <widget/dataviewcommon/kexidataawareobjectiface.h>
#include <core/KexiRecordNavigatorHandler.h>
#include <config-kexi.h>

#include <KDbTristate>
#include <KDbTableViewData>

#include <QScrollArea>
#include <QVariant>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

class QPrinter;
class QPrintDialog;
class QAbstractItemModel;
class KexiTableEdit;

//! minimum column width in pixels
#define KEXITV_MINIMUM_COLUMN_WIDTH 10

//! @short KexiTableScrollArea class provides a widget for displaying data in a tabular view.
/*! @see KexiFormScrollView */
class KEXIDATATABLE_EXPORT KexiTableScrollArea :
            public QScrollArea,
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
        explicit Appearance(QWidget *widget = 0);

        /*! Base color for cells, default is "Base" color for application's
         current palette */
        QColor baseColor;

        /*! Text color for cells, default is "Text" color for application's
         current palette */
        QColor textColor;

        /*! Grid color for cells, default is the same as default grid color of QTableView
         for application's current palette. */
        QColor gridColor;

        /*! empty area color, default is "Base" color for application's
         current palette. */
        QColor emptyAreaColor;

        /*! Alternate base color for cells, default is "AlternateBase" color for application's
         current palette. */
        QColor alternateBaseColor;

        /*! True if background alternate color should be used, true by default. */
        bool backgroundAltering;

        /*! True if full-record-selection mode is set,
         what means that all cells of the current record are always selected, instead of single cell.
         This mode is usable for read-only table views, when we're interested only in navigating
         by records. False by default, even for read-only table views.
        */
        bool fullRecordSelection;

        /*! True if full grid is enabled. By default true if backgroundAltering is false
         or if baseColor == alternateBaseColor.
         It is set to false for comboboxpopup table, to mimic original
         combobox look and feel. */
        bool horizontalGridEnabled;

        /*! True if full grid is enabled. True by default.
         It is set to false for comboboxpopup table, to mimic original
         combobox look and feel. */
        bool verticalGridEnabled;

        /*! \if the navigation panel is enabled (visible) for the view.
         True by default. */
        bool navigatorEnabled;

        /*! True if "record highlight" behaviour is enabled. False by default. */
        bool recordHighlightingEnabled;

        /*! True if "record highlight over" behaviour is enabled. False by default. */
        bool recordMouseOverHighlightingEnabled;

        /*! True if selection of a record should be kept when a user moved mouse
         pointer over other records. Makes only sense when recordMouseOverHighlightingEnabled is true.
         True by default. It is set to false for comboboxpopup table, to mimic original
         combobox look and feel. */
        bool persistentSelections;

        /*! Color for record highlight, default is intermediate (33%/60%) between
         active highlight and base color. */
        QColor recordHighlightingColor;

        /*! Color for text under record highlight, default is the same as textColor.
         Used when recordHighlightingEnabled is true. */
        QColor recordHighlightingTextColor;

        /*! Color for record highlight for mouseover, default is intermediate (20%/80%) between
         active highlight and base color. Used when recordMouseOverHighlightingEnabled is true. */
        QColor recordMouseOverHighlightingColor;

        /*! Color for text under record highlight for mouseover, default is the same as textColor.
         Used when recordMouseOverHighlightingEnabled is true. */
        QColor recordMouseOverHighlightingTextColor;

        /*! Like recordMouseOverHighlightingColor but for areas painted with alternate color.
         This is computed using active highlight color and alternateBaseColor. */
        QColor recordMouseOverAlternateHighlightingColor;
    };

    explicit KexiTableScrollArea(KDbTableViewData* data = 0, QWidget* parent = 0);

    virtual ~KexiTableScrollArea();

    //! redeclared to avoid conflict with private QWidget::data
    inline KDbTableViewData *data() const {
        return KexiDataAwareObjectInterface::data();
    }

    /*! \return current appearance settings */
    const Appearance& appearance() const;

    /*! Sets appearance settings. Table view is updated automatically. */
    void setAppearance(const Appearance& a);

    /*! Convenience function.
     \return field object that define column \a column or NULL if there is no such column */
    KDbField* field(int column) const;

    /*! \return maximum number of records that can be displayed per one "page"
     for current table view's size. */
    virtual int recordsPerPage() const;

    /*! \return number of records in this view. */
    virtual int recordCount() const;

    /*! \return number of the currently selected record number or -1. */
    virtual int currentRecord() const;

    QRect cellGeometry(int record, int column) const;
    int columnWidth(int col) const;
    int recordHeight() const;
    int columnPos(int col) const;
    int recordPos(int record) const;
    int columnNumberAt(int pos) const;
    int recordNumberAt(int pos, bool ignoreEnd = false) const;

    /*! \return true if the last visible column takes up all the available space.
     @see setStretchLastColumn(bool). */
    bool stretchLastColumn() const;

    /*! \return last record visible on the screen (counting from 0).
     The returned value is guaranteed to be smaller or equal to currentRecord() or -1
     if there are no records. */
    virtual int lastVisibleRecord() const;

    /*! Redraws specified cell. */
    virtual void updateCell(int record, int column);

    /*! Redraws the current cell. Implemented after KexiDataAwareObjectInterface. */
    virtual void updateCurrentCell();

    /*! Redraws all cells of specified record. */
    virtual void updateRecord(int record);

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

    void updateViewportMargins();

#ifdef KEXI_TABLE_PRINT_SUPPORT
    // printing
    // void  setupPrinter(KPrinter &printer, QPrintDialog &printDialog);
    void  print(QPrinter &printer, QPrintDialog &printDialog);
#endif

    // reimplemented for internal reasons
    virtual QSizePolicy sizePolicy() const;
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    /*! @return geometry of the viewport, i.e. the scrollable area, minus any scrollbars, etc.
     Implementation for KexiDataAwareObjectInterface. */
    virtual QRect viewportGeometry() const;

    /*! Reimplemented to update cached fonts and record sizes for the painter. */
    void setFont(const QFont &f);

    virtual QSize tableSize() const;

    void emitSelected();

    //! single shot after 1ms for contents updating
    void triggerUpdate();

    //! Initializes standard editor cell editor factories. This is called internally, once.
    static void initCellEditorFactories();

    /*! \return highlighted record number or -1 if no record is highlighted.
     Makes sense if record highlighting is enabled.
     @see Appearance::recordHighlightingEnabled setHighlightedRecord() */
    int highlightedRecordNumber() const;

    KDbRecordData *highlightedRecord() const;

    /*! \return vertical scrollbar. Implemented for KexiDataAwareObjectInterface. */
    virtual QScrollBar* verticalScrollBar() const;

    virtual bool eventFilter(QObject *o, QEvent *e);

public Q_SLOTS:
    virtual void setData(KDbTableViewData *data, bool owner = true) {
        KexiDataAwareObjectInterface::setData(data, owner);
    }

    virtual void clearColumnsInternal(bool repaint);

    /*! Reimplementation for KexiDataAwareObjectInterface */
    virtual void setSpreadSheetMode(bool set);

    /*! Adjusts \a column column's width to its (current) contents.
     If \a column == -1, all columns' width is adjusted. */
    void adjustColumnWidthToContents(int column = -1);

    //! Sets column width to \a width.
    void setColumnWidth(int column, int width);

    /*! If \a set is true, \a column column can be interactively resized by user.
     This is equivalent of QHeaderView::setResizeMode(column, QHeaderView::Interactive).
     If \a set is false, user cannot resize the column.
     This is equivalent of QHeaderView::setResizeMode(column, QHeaderView::Fixed).
     In both cases the column is initially resized to its default size and can be resized
     programatically afterwards using setColumnWidth(int, int).
      */
    void setColumnResizeEnabled(int column, bool set);

    /*! If \a set is true, all columns in this table view can be interactively
     resized by user.
     This is equivalent of QHeaderView::setResizeMode(QHeaderView::Interactive).
     If \a set is false, user cannot resize the columns.
     This is equivalent of QHeaderView::setResizeMode(QHeaderView::Fixed).
     In both cases columns are initially resized to their default sizes and can be resized
     programatically afterwards using setColumnWidth(int, int). */
    void setColumnsResizeEnabled(bool set);

    /*! Maximizes widths of columns selected by \a columnList, so the horizontal
     header has maximum overall width. Each selected column's width will be increased
     by the same value. Does nothing if \a columnList is empty or there is no free space
     to resize columns. If this table view is not visible, resizing will be performed on showing. */
    void maximizeColumnsWidth(const QList<int> &columnList);

    /*! If \a set is true the last visible column takes up all the available space
     ensuring that the available area is not wasted. Then it can't be resized by the user
     (setColumnResizeEnabled(columnCount()-1, !set) is used for that).
     The default value is false.
     @note Calling setColumnResizeEnabled(columnCount()-1, bool) can override this property. */
    void setStretchLastColumn(bool set);

    /*! Sets highlighted record number or -1 if no record has to be highlighted.
     Makes sense if record highlighting is enabled.
     @see Appearance::recordHighlightingEnabled */
    void setHighlightedRecordNumber(int record);

    /*! Ensures that cell at \a record and \a column is visible.
     If \a column is -1, current column number is used.
     If \a record is -1, current record number is used.
     \a record and \a column, if not -1, must be between 0 and recordCount()-1 (or columnCount()-1 accordingly). */
    virtual void ensureCellVisible(int record, int column);

    /*! Ensures that column \a col is visible.
     If \a col is -1, current column number is used. \a col, if not -1, must be between
     0 and columnCount()-1. */
    virtual void ensureColumnVisible(int col);

    /*! Deletes currently selected record; does nothing if no record
     is currently selected. If record is in edit mode, editing
     is cancelled before deleting.  */
    virtual void deleteCurrentRecord();

    /*! Inserts one empty record above record \a record. If \a record is -1 (the default),
     new record is inserted above the current record (or above 1st record if there is no current).
     A new item becomes current if record is -1 or if record is equal currentRecord().
     This method does nothing if:
     -inserting flag is disabled (see isInsertingEnabled())
     -read-only flag is set (see isReadOnly())
     \ return inserted record's data
    */
    virtual KDbRecordData *insertEmptyRecord(int pos = -1);

    /*! Used when Return key is pressed on cell or "+" nav. button is clicked.
     Also used when we want to continue editing a cell after "invalid value" message
     was displayed (in this case, \a setText is usually not empty, what means
     that text will be set in the cell replacing previous value).
    */
    virtual void startEditCurrentCell(const QString& setText = QString(),
                                      CreateEditorFlags flags = DefaultCreateEditorFlags) {
        KexiDataAwareObjectInterface::startEditCurrentCell(setText, flags);
    }

    /*! Deletes currently selected cell's contents, if allowed.
     In most cases delete is not accepted immediately but "record editing" mode is just started. */
    virtual void deleteAndStartEditCurrentCell() {
        KexiDataAwareObjectInterface::deleteAndStartEditCurrentCell();
    }

    /*! Cancels record editing All changes made to the editing
     record during this current session will be undone.
     \return true on success or false on failure (e.g. when editor does not exist) */
    virtual bool cancelRecordEditing() {
        return KexiDataAwareObjectInterface::cancelRecordEditing();
    }

    /*! Accepts record editing. All changes made to the editing
     record during this current session will be accepted (saved).
     \return true if accepting was successful, false otherwise
     (e.g. when current record contain data that does not meet given constraints). */
    virtual bool acceptRecordEditing() {
        return KexiDataAwareObjectInterface::acceptRecordEditing();
    }

    /*! Specifies, if this table view automatically accepts
     record editing (using acceptRecordEdit()) on accepting any cell's edit
     (i.e. after acceptEditor()). \sa acceptsRecordEditingAfterCellAccepting() */
    virtual void setAcceptsRecordEditAfterCellAccepting(bool set) {
        KexiDataAwareObjectInterface::setAcceptsRecordEditAfterCellAccepting(set);
    }

    /*! Specifies, if this table accepts dropping data on the records.
     If enabled:
     - dragging over record is indicated by drawing a line at bottom side of this record
     - dragOverRecord() signal will be emitted on dragging,
      -droppedAtRecord() will be emitted on dropping
     By default this flag is set to false. */
    virtual void setDropsAtRecordEnabled(bool set) {
        KexiDataAwareObjectInterface::setDropsAtRecordEnabled(set);
    }

    virtual bool cancelEditor() {
        return KexiDataAwareObjectInterface::cancelEditor();
    }
    virtual bool acceptEditor() {
        return KexiDataAwareObjectInterface::acceptEditor();
    }

    /*! Reimplementation for KexiDataAwareObjectInterface, to react on changes
     of the sorting flag. */
    virtual void setSortingEnabled(bool set);

Q_SIGNALS:
    void dataSet(KDbTableViewData *data);

    void itemSelected(KDbRecordData *data);
    void cellSelected(int record, int column);

    void itemReturnPressed(KDbRecordData *data, int record, int column);
    void itemDblClicked(KDbRecordData *data, int record, int column);
    void itemMouseReleased(KDbRecordData *data, int record, int column);

    void dragOverRecord(KDbRecordData *data, int record, QDragMoveEvent* e);
    void droppedAtRecord(KDbRecordData *data, int record, QDropEvent *e, KDbRecordData*& newData);

    /*! Data has been refreshed on-screen - emitted from initDataContents(). */
    void dataRefreshed();

    void itemChanged(KDbRecordData *data, int record, int column);
    void itemChanged(KDbRecordData *data, int record, int column, const QVariant &oldValue);
    void itemDeleteRequest(KDbRecordData *data, int record, int column);
    void currentItemDeleteRequest();
    //! Emitted for spreadsheet mode when an item was deleted and a new item has been appended
    void newItemAppendedForAfterDeletingInSpreadSheetMode();
    void sortedColumnChanged(int col);

    //! emitted when record editing is started (for updating or inserting)
    void recordEditingStarted(int record);

    //! emitted when record editing is terminated (for updating or inserting)
    //! no matter if accepted or not
    void recordEditingTerminated(int record);

    //! emitted when state of 'save/cancel record changes' actions should be updated.
    void updateSaveCancelActions();

    //! Emitted in initActions() to force reload actions
    //! You should remove existing actions and add them again.
    void reloadActions();

protected Q_SLOTS:
    virtual void slotDataDestroying() {
        KexiDataAwareObjectInterface::slotDataDestroying();
    }

    virtual void slotRecordsDeleted(const QList<int> &records);

    //! updates display after deletion of many records
    void slotColumnWidthChanged(int column, int oldSize, int newSize);

    void slotSectionHandleDoubleClicked(int section);

    void slotUpdate();

    //! implemented because we needed this as slot
    virtual void sortColumnInternal(int col, int order = 0);

    //! receives a signal from cell editors
    void slotEditRequested();

    /*! Reloads data for this widget.
     Handles KDbTableViewData::reloadRequested() signal. */
    virtual void reloadData();

    //! Handles KDbTableViewData::recordRepaintRequested() signal
    virtual void slotRecordRepaintRequested(KDbRecordData* data);

    //! Handles KDbTableViewData::aboutToDeleteRecord() signal. Prepares info for slotRecordDeleted().
    virtual void slotAboutToDeleteRecord(KDbRecordData* data, KDbResultInfo* result, bool repaint) {
        KexiDataAwareObjectInterface::slotAboutToDeleteRecord(data, result, repaint);
    }

    //! Handles KDbTableViewData::recordDeleted() signal to repaint when needed.
    virtual void slotRecordDeleted() {
        KexiDataAwareObjectInterface::slotRecordDeleted();
    }

    //! Handles KDbTableViewData::recordInserted() signal to repaint when needed.
    virtual void slotRecordInserted(KDbRecordData *data, bool repaint) {
        KexiDataAwareObjectInterface::slotRecordInserted(data, repaint);
    }

    //! Like above, not db-aware version
    virtual void slotRecordInserted(KDbRecordData *data, int record, bool repaint) {
        KexiDataAwareObjectInterface::slotRecordInserted(data, record, repaint);
    }

    /*! Handles verticalScrollBar()'s valueChanged(int) signal.
     Called when vscrollbar's value has been changed. */
    virtual void verticalScrollBarValueChanged(int v);

    //! for navigator
    virtual void moveToRecordRequested(int record);
    virtual void moveToLastRecordRequested();
    virtual void moveToPreviousRecordRequested();
    virtual void moveToNextRecordRequested();
    virtual void moveToFirstRecordRequested();
    virtual void addNewRecordRequested() {
        KexiDataAwareObjectInterface::addNewRecordRequested();
    }

protected:
    /*! Reimplementation for KexiDataAwareObjectInterface
     Initializes data contents (resizes it, sets cursor at 1st row).
     Called on setData(). Also called once on show event after
     reloadRequested() signal was received from KDbTableViewData object. */
    virtual void initDataContents();

    /*! Implementation for KexiDataAwareObjectInterface.
     Updates widget's contents size using QScrollView::resizeContents()
     depending on tableSize(). */
    virtual void updateWidgetContentsSize();

    /*! Reimplementation for KexiDataAwareObjectInterface */
    virtual void clearVariables();

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual Qt::SortOrder currentLocalSortOrder() const;

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual int currentLocalSortColumn() const;

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual void setLocalSortOrder(int column, Qt::SortOrder order);

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual void updateGUIAfterSorting(int previousRow);

    int leftMargin() const;

    int topMargin() const;

    //! temporary
    int bottomMargin() const { return 0; }

    /*! @internal \return true if the row defined by \a data has default
     value at column \a col. If this is the case and \a value is not NULL,
     *value is set to the default value. */
    bool isDefaultValueDisplayed(KDbRecordData *data, int col, QVariant* value = 0);

    //! painting and layout
    void drawContents(QPainter *p);
    void paintCell(QPainter* p, KDbRecordData *data, int record, int column, const QRect &cr, bool print = false);
    void paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch);
    void updateGeometries();

    QPoint contentsToViewport2(const QPoint &p);
    void contentsToViewport2(int x, int y, int& vx, int& vy);
    QPoint viewportToContents2(const QPoint& vp);

    // event handling
    void contentsMousePressEvent(QMouseEvent* e);
    void contentsMouseReleaseEvent(QMouseEvent* e);
    //! @internal called by contentsMouseOrEvent() contentsMouseReleaseEvent() to move cursor
    bool handleContentsMousePressOrRelease(QMouseEvent* e, bool release);
    void contentsMouseMoveEvent(QMouseEvent* e);
    void contentsMouseDoubleClickEvent(QMouseEvent* e);
    void contentsContextMenuEvent(QContextMenuEvent* e);
    virtual void keyPressEvent(QKeyEvent *e);
    //virtual void focusInEvent(QFocusEvent* e);
    virtual void focusOutEvent(QFocusEvent* e);
    virtual void resizeEvent(QResizeEvent* e);
    //virtual void viewportResizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void dragMoveEvent(QDragMoveEvent *e);
    virtual void dropEvent(QDropEvent *e);
    virtual void dragLeaveEvent(QDragLeaveEvent *e);
    //! For handling changes of palette
    virtual void changeEvent(QEvent *e);

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual KexiDataItemInterface *editor(int col, bool ignoreMissingEditor = false);

    KexiTableEdit *tableEditorWidget(int col, bool ignoreMissingEditor = false);

    /*! Implementation for KexiDataAwareObjectInterface */
    virtual void editorShowFocus(int row, int col);

    //! Creates editors and shows it, what usually means the beginning of a cell editing
    virtual void createEditor(int row, int col, const QString& addText = QString(),
                              CreateEditorFlags flags = DefaultCreateEditorFlags);

    bool focusNextPrevChild(bool next);

    /*! Used in key event: \return true if event \a e should execute action \a action_name.
     Action shortcuts defined by shortCutPressed() are reused, if present, and if \a e matches
     given action's shortcut - false is returned (beause action is already performed at main
     window's level).
    */
    bool shortCutPressed(QKeyEvent *e, const QString &action_name);

    /*! Shows context menu at \a pos for selected cell
     if menu is configured,
     else: contextMenuRequested() signal is emitted.
     Method used in contentsMousePressEvent() (for right button)
     and keyPressEvent() for Qt::Key_Menu key.
     If \a pos is QPoint(-1,-1) (the default), menu is positioned below the current cell.
    */
    void showContextMenu(const QPoint& pos = QPoint(-1, -1));

    /*! internal */
    inline void paintRow(KDbRecordData *data,
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

    /*! Used in KexiDataAwareObjectInterface::slotRecordDeleted()
     to repaint tow \a row and all visible below. */
    virtual void updateAllVisibleRecordsBelow(int row);

    void updateAfterCancelRecordEditing();
    void updateAfterAcceptRecordEditing();

    /*! Sets \a cellValue if there is a lookup value for the cell \a data.
     Used in paintCell() and KexiTableCellToolTip::maybeTip()
     \return true is \a cellValue has been found. */
    bool getVisibleLookupValue(QVariant& cellValue, KexiTableEdit *edit,
                               KDbRecordData *data, KDbTableViewColumn *tvcol) const;

    /*! Implementation for KexiDataItemChangesListener.
     Reaction for change of \a item. */
    virtual void valueChanged(KexiDataItemInterface* item);

    /*! Implementation for KexiDataItemChangesListener. */
    virtual bool cursorAtNewRecord() const;

    /*! Implementation for KexiDataItemChangesListener. */
    virtual void lengthExceeded(KexiDataItemInterface *item, bool lengthExceeded);

    /*! Implementation for KexiDataItemChangesListener. */
    virtual void updateLengthExceededMessage(KexiDataItemInterface *item);

    virtual int horizontalHeaderHeight() const;

    //! @return record navigator pane
    QWidget* navPanelWidget() const;

    //! @return true if record navigator pane exists and is has "visible" appearance set to ON
    bool navPanelWidgetVisible() const;

    virtual bool event(QEvent *e);

    //! @internal @return text information about a given column or other specific areas of the table view.
    QString whatsThisText(const QPoint &pos) const;

    /*! Called by KexiDataAwareObjectInterface::setCursorPosition()
      if cursor's position is really changed. */
    virtual void selectCellInternal(int previousRow, int previousColumn);

    //! @return horizontal header
    virtual QHeaderView* horizontalHeader() const;

    //! @return vertical header
    virtual QHeaderView* verticalHeader() const;

    //! @return common model for header views of this area. @see KexiTableScrollAreaHeader
    QAbstractItemModel* headerModel() const;

    void updateScrollAreaWidgetSize();

    //! Update section of vertical header
    virtual void updateVerticalHeaderSection(int section);

    virtual void beginInsertItem(KDbRecordData *data, int pos);

    virtual void endInsertItem(KDbRecordData *data, int pos);

    virtual void beginRemoveItem(KDbRecordData *data, int pos);

    virtual void endRemoveItem(int pos);

    class Private;
    Private * const d;

    friend class KexiTableScrollAreaWidget;
    friend class KexiTableCellToolTip;
    friend class KexiTableScrollAreaHeader;
};

#endif
