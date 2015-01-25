/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
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

#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QMatrix>
#include <QTimer>
#include <QMenu>
#include <QCursor>
#include <QStyle>
#include <QLayout>
#include <QLabel>
#include <QToolTip>
#include <QWhatsThis>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QShowEvent>
#include <QKeyEvent>
#include <QHelpEvent>
#include <QEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QPixmap>
#include <QDesktopWidget>
#include <QScrollArea>
#include <QApplication>
#include <QScrollBar>

#ifndef KEXI_NO_PRINT
#include <QPrinter>
#endif

#include <kcolorscheme.h>
#include <klocale.h>
#include <kdebug.h>
#include <kaction.h>
#include <kmenu.h>

#include <KoIcon.h>

#include "KexiTableScrollArea.h"
#include "KexiTableScrollArea_p.h"
#include "KexiTableScrollAreaWidget.h"
#include "KexiTableScrollAreaHeader.h"
#include "KexiTableScrollAreaHeaderModel.h"
#include "kexitableedit.h"
#include <kexi_global.h>
#include <kexiutils/utils.h>
#include "kexicelleditorfactory.h"
#include <widget/utils/kexidisplayutils.h>
#include <db/cursor.h>
#include <db/validator.h>

//#define KEXITABLEVIEW_DEBUG
//#define KEXITABLEVIEW_DEBUG_PAINT

const int MINIMUM_ROW_HEIGHT = 17;

KexiTableScrollArea::Appearance::Appearance(QWidget *widget)
{
    //set defaults
    if (qApp) {
        baseColor = KColorScheme(QPalette::Active, KColorScheme::View).background().color()/*QPalette::Base*/;
        textColor = KColorScheme(QPalette::Active, KColorScheme::View).foreground().color()/*QPalette::Base*/;
        QStyleOptionViewItemV4 option;
        option.init(widget);
        const int gridHint = widget->style()->styleHint(QStyle::SH_Table_GridLineColor, &option, widget);
        gridColor = static_cast<QRgb>(gridHint);
        emptyAreaColor = KColorScheme(QPalette::Active, KColorScheme::View).background().color()/*QPalette::Base*/;
        alternateBaseColor = widget->palette().color(QPalette::AlternateBase);
        rowHighlightingColor = KexiUtils::blendedColors(QPalette::Highlight, baseColor, 34, 66);
        rowMouseOverHighlightingColor = KexiUtils::blendedColors(QPalette::Highlight, baseColor, 10, 90);
        rowMouseOverAlternateHighlightingColor = KexiUtils::blendedColors(QPalette::Highlight, alternateBaseColor, 10, 90);
        rowHighlightingTextColor = textColor;
        rowMouseOverHighlightingTextColor = textColor;
    }
    backgroundAltering = true;
    rowMouseOverHighlightingEnabled = true;
    rowHighlightingEnabled = true;
    persistentSelections = true;
    navigatorEnabled = true;
    fullRowSelection = false;
    verticalGridEnabled = true;
    horizontalGridEnabled = !backgroundAltering || baseColor == alternateBaseColor;
}

//-----------------------------------------

#ifdef __GNUC__
#warning TODO KexiTableViewCellToolTip
#else
#pragma WARNING( TODO KexiTableViewCellToolTip )
#endif
/* TODO
KexiTableViewCellToolTip::KexiTableViewCellToolTip( KexiTableView * tableView )
 : QToolTip()
 , m_tableView(tableView)
{
}

KexiTableViewCellToolTip::~KexiTableViewCellToolTip()
{
}

void KexiTableViewCellToolTip::maybeTip( const QPoint & p )
{
  const QPoint cp( m_tableView->viewportToContents( p ) );
  const int row = m_tableView->rowAt( cp.y(), true );
  const int col = m_tableView->columnAt( cp.x() );

  //show tooltip if needed
  if (col>=0 && row>=0) {
    KexiTableEdit *editor = m_tableView->tableEditorWidget( col );
    const bool insertRowSelected = m_tableView->isInsertingEnabled() && row==m_tableView->rowCount();
    KexiDB::RecordData *record = insertRowSelected ? m_tableView->m_insertItem : m_tableView->itemAt( row );
    if (editor && record && (col < (int)record->count())) {
      int w = m_tableView->columnWidth( col );
      int h = m_tableView->rowHeight();
      int x = 0;
      int y_offset = 0;
      int align = SingleLine | AlignVCenter;
      QString txtValue;
      QVariant cellValue;
      KexiDB::TableViewColumn *tvcol = m_tableView->column(col);
      if (!m_tableView->getVisibleLookupValue(cellValue, editor, record, tvcol))
        cellValue = insertRowSelected ? editor->displayedField()->defaultValue() : record->at(col); //display default value if available
      const bool focused = m_tableView->selectedItem() == record && col == m_tableView->currentColumn();
      editor->setupContents( 0, focused, cellValue, txtValue, align, x, y_offset, w, h );
      QRect realRect(m_tableView->columnPos(col)-m_tableView->horizontalScrollBar()->value(),
        m_tableView->rowPos(row)-m_tableView->verticalScrollBar()->value(), w, h);
      if (editor->showToolTipIfNeeded(
        txtValue.isEmpty() ? record->at(col) : QVariant(txtValue),
        realRect, m_tableView->fontMetrics(), focused))
      {
        QString squeezedTxtValue;
        if (txtValue.length() > 50)
          squeezedTxtValue = txtValue.left(100) + "...";
        else
          squeezedTxtValue = txtValue;
        tip( realRect, squeezedTxtValue );
      }
    }
  }
}
*/ //TODO

//-----------------------------------------

KexiTableScrollArea::KexiTableScrollArea(KexiDB::TableViewData* data, QWidget* parent)
        : QScrollArea(parent)
        , KexiRecordNavigatorHandler()
        , KexiSharedActionClient()
        , KexiDataAwareObjectInterface()
        , d(new Private(this))
{
    setAttribute(Qt::WA_StaticContents, true);
    setAttribute(Qt::WA_CustomWhatsThis, true);

    d->scrollAreaWidget = new KexiTableScrollAreaWidget(this);
    setWidget(d->scrollAreaWidget);

    m_data = new KexiDB::TableViewData(); //to prevent crash because m_data==0
    m_owner = true;                   //-this will be deleted if needed

    viewport()->setFocusPolicy(Qt::WheelFocus);
    setFocusPolicy(Qt::WheelFocus); //<--- !!!!! important (was NoFocus),
                                    // otherwise QApplication::setActiveWindow() won't activate
                                    // this widget when needed!
    viewport()->installEventFilter(this);
    d->scrollAreaWidget->installEventFilter(this);

    d->diagonalGrayPattern = QBrush(d->appearance.gridColor, Qt::BDiagPattern);

    setLineWidth(1);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    horizontalScrollBar()->installEventFilter(this);

    //context menu
    m_contextMenu = new KMenu(this);
    m_contextMenu->setObjectName("m_contextMenu");

//! \todo replace lineedit with table_field icon
//setContextMenuTitle(koIcon("lineedit"), i18n("Record"));   // the default
    // cannot display anything here - most actions in the context menu
    // are related to a single cell (Cut, Copy..) and others to entire row (Delete Row):
    setContextMenuEnabled(false);

    d->pUpdateTimer = new QTimer(this);
    d->pUpdateTimer->setSingleShot(true);

    // Create headers
    d->headerModel = new KexiTableScrollAreaHeaderModel(this);

    d->horizontalHeader = new KexiTableScrollAreaHeader(Qt::Horizontal, this);
    d->horizontalHeader->setObjectName("horizontalHeader");
    d->horizontalHeader->setSelectionBackgroundColor(palette().color(QPalette::Highlight));

    d->verticalHeader = new KexiTableScrollAreaHeader(Qt::Vertical, this);
    d->verticalHeader->setObjectName("verticalHeader");
    d->verticalHeader->setSelectionBackgroundColor(palette().color(QPalette::Highlight));

    setupNavigator();

    if (data) {
        setData(data);
    }

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    // Connect header, table and scrollbars
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), d->horizontalHeader, SLOT(setOffset(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), d->verticalHeader, SLOT(setOffset(int)));
    connect(d->horizontalHeader, SIGNAL(sectionResized(int,int,int)), this, SLOT(slotColumnWidthChanged(int,int,int)));
    connect(d->horizontalHeader, SIGNAL(sectionHandleDoubleClicked(int)), this, SLOT(slotSectionHandleDoubleClicked(int)));
    connect(d->horizontalHeader, SIGNAL(sectionClicked(int)), this, SLOT(sortColumnInternal(int)));

    connect(d->pUpdateTimer, SIGNAL(timeout()), this, SLOT(slotUpdate()));

    setAppearance(d->appearance); //refresh

#ifdef __GNUC__
#warning TODO d->cellToolTip = new KexiTableViewCellToolTip(this);
#else
#pragma WARNING( TODO d->cellToolTip = new KexiTableViewCellToolTip(this); )
#endif
}

KexiTableScrollArea::~KexiTableScrollArea()
{
    cancelRowEdit();

    KexiDB::TableViewData *data = m_data;
    m_data = 0;
    if (m_owner) {
        if (data)
            data->deleteLater();
    }
    delete d;
}

void KexiTableScrollArea::clearVariables()
{
    KexiDataAwareObjectInterface::clearVariables();
    d->clearVariables();
}

void KexiTableScrollArea::setupNavigator()
{
    m_navPanel = new KexiRecordNavigator(*this, this);
    navPanelWidget()->setObjectName("navPanel");
    m_navPanel->setRecordHandler(this);
}

void KexiTableScrollArea::initDataContents()
{
    updateWidgetContentsSize();

    KexiDataAwareObjectInterface::initDataContents();

    m_navPanel->showEditingIndicator(false);
}

void KexiTableScrollArea::updateWidgetContentsSize()
{
    updateScrollAreaWidgetSize();
    d->horizontalHeader->setFixedSize(d->horizontalHeader->sizeHint());
    d->verticalHeader->setFixedSize(d->verticalHeader->sizeHint());
    //kDebug() << d->horizontalHeader->sizeHint() << d->verticalHeader->sizeHint();
    //kDebug() << d->horizontalHeader->geometry() << d->verticalHeader->geometry();
}

void KexiTableScrollArea::updateScrollAreaWidgetSize()
{
    QSize s(tableSize());
    s.setWidth(qMax(s.width(), viewport()->width()));
    s.setHeight(qMax(s.height(), viewport()->height()));
    d->scrollAreaWidget->resize(s);
}

void KexiTableScrollArea::updateVerticalHeaderSection(int row)
{
    d->verticalHeader->updateSection(row);
}

void KexiTableScrollArea::slotRowsDeleted(const QList<int> &rows)
{
    viewport()->repaint();
    updateWidgetContentsSize();
    setCursorPosition(qMax(0, (int)m_curRow - (int)rows.count()), -1, ForceSetCursorPosition);
}

void KexiTableScrollArea::setFont(const QFont &font)
{
    QScrollArea::setFont(font);

#ifdef Q_WS_WIN
    d->rowHeight = fontMetrics().lineSpacing() + 4;
#else
    d->rowHeight = fontMetrics().lineSpacing() + 1;
#endif
    if (d->appearance.fullRowSelection) {
        d->rowHeight -= 1;
    }
    if (d->rowHeight < MINIMUM_ROW_HEIGHT) {
        d->rowHeight = MINIMUM_ROW_HEIGHT;
    }
    KexiDisplayUtils::initDisplayForAutonumberSign(d->autonumberSignDisplayParameters, this);
    KexiDisplayUtils::initDisplayForDefaultValue(d->defaultValueDisplayParameters, this);
    update();
}

void KexiTableScrollArea::updateAllVisibleRowsBelow(int row)
{
    //get last visible row
//    int r = rowAt(viewport()->height() + verticalScrollBar()->value());
//    if (r == -1) {
//        r = rowCount() + 1 + (isInsertingEnabled() ? 1 : 0);
//    }
    //update all visible rows below
    int leftcol = d->horizontalHeader->visualIndexAt(d->horizontalHeader->offset());
    d->scrollAreaWidget->update(columnPos(leftcol), rowPos(row),
                                viewport()->width(),
                                viewport()->height() - (rowPos(row) - verticalScrollBar()->value()));
}

void KexiTableScrollArea::clearColumnsInternal(bool /*repaint*/)
{
}

void KexiTableScrollArea::slotUpdate()
{
//    kDebug() << m_navPanel;
    updateScrollAreaWidgetSize();
    d->scrollAreaWidget->update();
    updateWidgetContentsSize();
}

Qt::SortOrder KexiTableScrollArea::currentLocalSortOrder() const
{
    return d->horizontalHeader->sortIndicatorOrder();
}

void KexiTableScrollArea::setLocalSortOrder(int column, Qt::SortOrder order)
{
    d->horizontalHeader->setSortIndicator(column, order);
}

int KexiTableScrollArea::currentLocalSortColumn() const
{
    return d->horizontalHeader->sortIndicatorSection();
}

void KexiTableScrollArea::updateGUIAfterSorting(int previousRow)
{
    int prevRowVisibleOffset = rowPos(previousRow) - verticalScrollBar()->value();
    verticalScrollBar()->setValue(rowPos(m_curRow) - prevRowVisibleOffset);
    d->scrollAreaWidget->update();
    selectCellInternal(m_curRow, m_curCol);
}

QSizePolicy KexiTableScrollArea::sizePolicy() const
{
    // this widget is expandable
    return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize KexiTableScrollArea::sizeHint() const
{
    QSize ts = tableSize();
    int w = qMax(ts.width() + leftMargin() + verticalScrollBar()->sizeHint().width() + 2 * 2,
                 (navPanelWidgetVisible() ? navPanelWidget()->width() : 0));
    int h = qMax(ts.height() + topMargin() + horizontalScrollBar()->sizeHint().height(),
                 minimumSizeHint().height());
    w = qMin(w, qApp->desktop()->availableGeometry(this).width() * 3 / 4); //stretch
    h = qMin(h, qApp->desktop()->availableGeometry(this).height() * 3 / 4); //stretch

#ifdef KEXITABLEVIEW_DEBUG
    kDebug() << w << h;
#endif

    return QSize(w, h);
}

QSize KexiTableScrollArea::minimumSizeHint() const
{
    return QSize(
               leftMargin() + ((columnCount() > 0) ? columnWidth(0) : KEXI_DEFAULT_DATA_COLUMN_WIDTH) + 2*2,
               d->rowHeight*5 / 2 + topMargin() + (navPanelWidgetVisible() ? navPanelWidget()->height() : 0)
           );
}

QRect KexiTableScrollArea::viewportGeometry() const
{
    return viewport()->geometry();
}

//internal
inline void KexiTableScrollArea::paintRow(KexiDB::RecordData *record,
                                    QPainter *pb, int r, int rowp, int cx, int cy,
                                    int colfirst, int collast, int maxwc)
{
    Q_UNUSED(cx);
    Q_UNUSED(cy);
    if (!record)
        return;

    //kDebug() << "r" << r << "rowp" << rowp << "cx" << cx << "cy" << cy
    //    << "colfirst" << colfirst << "collast" << collast << "maxwc" << maxwc;

    // Go through the columns in the row r
    // if we know from where to where, go through [colfirst, collast],
    // else go through all of them
    if (colfirst == -1)
        colfirst = 0;
    if (collast == -1)
        collast = columnCount() - 1;

    int transly = rowp;

    if (d->appearance.rowHighlightingEnabled && r == m_curRow && !d->appearance.fullRowSelection) {
        pb->fillRect(0, transly, maxwc, d->rowHeight, d->appearance.rowHighlightingColor);
    } else if (d->appearance.rowMouseOverHighlightingEnabled && r == d->highlightedRow) {
        if (d->appearance.backgroundAltering && (r % 2 != 0))
            pb->fillRect(0, transly, maxwc, d->rowHeight, d->appearance.rowMouseOverAlternateHighlightingColor);
        else
            pb->fillRect(0, transly, maxwc, d->rowHeight, d->appearance.rowMouseOverHighlightingColor);
    } else {
        if (d->appearance.backgroundAltering && (r % 2 != 0))
            pb->fillRect(0, transly, maxwc, d->rowHeight, d->appearance.alternateBaseColor);
        else
            pb->fillRect(0, transly, maxwc, d->rowHeight, d->appearance.baseColor);
    }

    for (int c = colfirst; c <= collast; c++) {
        // get position and width of column c
        int colp = columnPos(c);
        if (colp == -1)
            continue; //invisible column?
        int colw = columnWidth(c);
//        kDebug() << "c:" << c << "colp:" << colp << "cx:" << cx << "contentsX():" << horizontalScrollBar()->value() << "colw:" << colw;
//(js #2010-01-05) breaks rendering:       int translx = colp - cx + horizontalScrollBar()->value();
        int translx = colp;

        // Translate painter and draw the cell
        const QTransform oldTr( pb->worldTransform() );
        pb->translate(translx, transly);
        paintCell(pb, record, r, c, QRect(colp, rowp, colw, d->rowHeight));
        pb->setWorldTransform(oldTr);
    }

    if (m_dragIndicatorLine >= 0) {
        int y_line = -1;
        if (r == (rowCount() - 1) && m_dragIndicatorLine == rowCount()) {
            y_line = transly + d->rowHeight - 3; //draw at last line
        }
        if (m_dragIndicatorLine == r) {
            y_line = transly;
        }
        if (y_line >= 0) {
            if (!d->dragIndicatorRubberBand) {
                d->dragIndicatorRubberBand = new QRubberBand(QRubberBand::Line, viewport());
            }
            d->dragIndicatorRubberBand->setGeometry(0, y_line, maxwc, 3);
            d->dragIndicatorRubberBand->show();
        }
        else {
            if (d->dragIndicatorRubberBand) {
                d->dragIndicatorRubberBand->hide();
            }
        }
    }
    else {
        if (d->dragIndicatorRubberBand) {
            d->dragIndicatorRubberBand->hide();
        }
    }
}

void KexiTableScrollArea::drawContents(QPainter *p)
{
    int cx = p->clipBoundingRect().x();
    int cy = p->clipBoundingRect().y();
    int cw = p->clipBoundingRect().width();
    int ch = p->clipBoundingRect().height();
#ifdef KEXITABLEVIEW_DEBUG
    kDebug() << "disable" << d->disableDrawContents << "cx" << cx << "cy" << cy << "cw" << cw << "ch" << ch
             << "contentsRect" << contentsRect() << "geo" << geometry();
#endif
    if (d->disableDrawContents)
        return;

    bool paintOnlyInsertRow = false;
    bool inserting = isInsertingEnabled();
    bool plus1row = false; //true if we should show 'inserting' row at the end
    int colfirst = columnAt(cx);
    int rowfirst = rowAt(cy);
    int collast = columnAt(cx + cw - 1);
    int rowlast = rowAt(cy + ch - 1);
    if (rowfirst == -1 && (cy / d->rowHeight) == rowCount()) {
        // make the insert row paint too when requested
#ifdef KEXITABLEVIEW_DEBUG
        kDebug() << "rowfirst == -1 && (cy / d->rowHeight) == rowCount()";
#endif
        rowfirst = m_data->count();
        rowlast = rowfirst;
        paintOnlyInsertRow = true;
        plus1row = inserting;
    }
/*    kDebug() << "cx" << cx << "cy" << cy << "cw" << cw << "ch" << ch
        << "colfirst" << colfirst << "rowfirst" << rowfirst
        << "collast" << collast << "rowlast" << rowlast;*/

    if (rowlast == -1) {
        rowlast = rowCount() - 1;
        plus1row = inserting;
        if (rowfirst == -1) {
            if (rowAt(cy - d->rowHeight) != -1) {
                //paintOnlyInsertRow = true;
//    kDebug() << "-- paintOnlyInsertRow --";
            }
        }
    }
// kDebug() << "rowfirst="<<rowfirst<<" rowlast="<<rowlast<<" rowCount()="<<rowCount();
// kDebug()<<" plus1row=" << plus1row;

    if (collast == -1)
        collast = columnCount() - 1;

    if (colfirst > collast) {
        int tmp = colfirst;
        colfirst = collast;
        collast = tmp;
    }
    if (rowfirst > rowlast) {
        int tmp = rowfirst;
        rowfirst = rowlast;
        rowlast = tmp;
    }

//  qDebug("cx:%3d cy:%3d w:%3d h:%3d col:%2d..%2d row:%2d..%2d tsize:%4d,%4d",
// cx, cy, cw, ch, colfirst, collast, rowfirst, rowlast, tableSize().width(), tableSize().height());
    if (rowfirst == -1 || colfirst == -1) {
        if (!paintOnlyInsertRow && !plus1row) {
            paintEmptyArea(p, cx, cy, cw, ch);
            return;
        }
    }

    int maxwc = columnPos(columnCount() - 1) + columnWidth(columnCount() - 1);
// kDebug() << "maxwc:" << maxwc;

    p->fillRect(cx, cy, cw, ch, d->appearance.baseColor);

    int rowp = 0;
    int r = 0;
    if (paintOnlyInsertRow) {
        r = rowCount();
        rowp = rowPos(r); // 'insert' row's position
    } else {
        if (rowfirst >= 0) {
            QList<KexiDB::RecordData*>::ConstIterator it(m_data->constBegin());
            it += rowfirst;//move to 1st row
            rowp = rowPos(rowfirst); // row position
            for (r = rowfirst;r <= rowlast; r++, ++it, rowp += d->rowHeight) {
    //   (*it)->debug();
                paintRow(*it, p, r, rowp, cx, cy, colfirst, collast, maxwc);
            }
        }
    }

    if (plus1row && rowfirst >= 0) { //additional - 'insert' row
        paintRow(m_insertItem, p, r, rowp, cx, cy, colfirst, collast, maxwc);
    }
    paintEmptyArea(p, cx, cy, cw, ch);
}

bool KexiTableScrollArea::isDefaultValueDisplayed(KexiDB::RecordData *record, int col, QVariant* value)
{
    const bool cursorAtInsertRowOrEditingNewRow = (record == m_insertItem || (m_newRowEditing && m_currentItem == record));
    KexiDB::TableViewColumn *tvcol;
    if (cursorAtInsertRowOrEditingNewRow
            && (tvcol = m_data->column(col))
            && hasDefaultValueAt(*tvcol)
            && !tvcol->field()->isAutoIncrement())
    {
        if (value)
            *value = tvcol->field()->defaultValue();
        return true;
    }
    return false;
}

void KexiTableScrollArea::paintCell(QPainter* p, KexiDB::RecordData *record, int row, int col, const QRect &cr, bool print)
{
    Q_UNUSED(print);

    //kDebug() << "col/row:" << col << row << "rect:" << cr;
    p->save();
    int w = cr.width();
    int h = cr.height();
    int x2 = w - 1;
    int y2 = h - 1;

    // Draw our lines
    QPen pen(p->pen());

    if (d->appearance.horizontalGridEnabled) {
        p->setPen(d->appearance.gridColor);
        p->drawLine(0, y2, x2, y2);   // bottom
    }
    if (d->appearance.verticalGridEnabled) {
        p->setPen(d->appearance.gridColor);
        p->drawLine(x2, 0, x2, y2);   // right
    }
    p->setPen(pen);

    if (m_editor && row == m_curRow && col == m_curCol //don't paint contents of edited cell
            && m_editor->hasFocusableWidget() //..if it's visible
       ) {
        p->restore();
        return;
    }

    KexiTableEdit *edit = tableEditorWidget(col, /*ignoreMissingEditor=*/true);
    int x = edit ? edit->leftMargin() : 0;
    int y_offset = 0;
    int align = Qt::TextSingleLine | Qt::AlignVCenter;
    QString txt; //text to draw

    if (record == m_insertItem) {
        //kDebug() << "we're at INSERT row...";
    }

    KexiDB::TableViewColumn *tvcol = m_data->column(col);

    QVariant cellValue;
    if (col < (int)record->count()) {
        if (m_currentItem == record) {
            if (m_editor && row == m_curRow && col == m_curCol
                    && !m_editor->hasFocusableWidget()) {
                //we're over editing cell and the editor has no widget
                // - we're displaying internal values, not buffered
                cellValue = m_editor->value();
            } else {
                //we're displaying values from edit buffer, if available
                // this assignment will also get default value if there's no actual value set
                cellValue = *bufferedValueAt(col);
            }
        } else {
            cellValue = record->at(col);
        }
    }

    bool defaultValueDisplayed = isDefaultValueDisplayed(record, col);

    if ((record == m_insertItem /*|| m_newRowEditing*/) && cellValue.isNull()) {
        if (!tvcol->field()->isAutoIncrement() && !tvcol->field()->defaultValue().isNull()) {
            //display default value in the "insert record", if available
            //(but not if there is autoincrement flag set)
            cellValue = tvcol->field()->defaultValue();
            defaultValueDisplayed = true;
        }
    }

    const bool columnReadOnly = tvcol->isReadOnly();
    const bool dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
        =    d->appearance.rowHighlightingEnabled && !d->appearance.persistentSelections
          && m_curRow >= 0 && row != m_curRow;

    // setup default pen
    QPen defaultPen;
    const bool usesSelectedTextColor = edit && edit->usesSelectedTextColor();
    if (defaultValueDisplayed){
        if (col == m_curCol && row == m_curRow && usesSelectedTextColor)
            defaultPen = d->defaultValueDisplayParameters.selectedTextColor;
        else
            defaultPen = d->defaultValueDisplayParameters.textColor;
    } else if (d->appearance.fullRowSelection
               && (row == d->highlightedRow || (row == m_curRow && d->highlightedRow == -1))
               && usesSelectedTextColor)
    {
        defaultPen = d->appearance.rowHighlightingTextColor; //special case: highlighted record
    }
    else if (d->appearance.fullRowSelection && row == m_curRow && usesSelectedTextColor) {
        defaultPen = d->appearance.textColor; //special case for full record selection
    }
    else if (   m_currentItem == record && col == m_curCol && !columnReadOnly
             && !dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
             && usesSelectedTextColor)
    {
        defaultPen = palette().color(QPalette::HighlightedText); //selected text
    } else if (   d->appearance.rowHighlightingEnabled && row == m_curRow
               && !dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
               && usesSelectedTextColor)
    {
        defaultPen = d->appearance.rowHighlightingTextColor;
    } else if (   d->appearance.rowMouseOverHighlightingEnabled && row == d->highlightedRow
               && !dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
               && usesSelectedTextColor)
    {
        defaultPen = d->appearance.rowMouseOverHighlightingTextColor;
    } else {
        defaultPen = d->appearance.textColor;
    }

    if (edit) {
        if (defaultValueDisplayed)
            p->setFont(d->defaultValueDisplayParameters.font);
        p->setPen(defaultPen);

        //get visible lookup value if available
        getVisibleLookupValue(cellValue, edit, record, tvcol);

/*kDebug() << "edit->setupContents()" << (m_currentItem == record && col == m_curCol)
        << cellValue << txt << align << x << y_offset << w << h;*/
        edit->setupContents(p, m_currentItem == record && col == m_curCol,
                            cellValue, txt, align, x, y_offset, w, h);
    }
    if (!d->appearance.horizontalGridEnabled)
        y_offset++; //correction because we're not drawing cell borders

    if (d->appearance.fullRowSelection && d->appearance.fullRowSelection) {
    }
    if (m_currentItem == record && (col == m_curCol || d->appearance.fullRowSelection)) {
        if (edit && (   (d->appearance.rowHighlightingEnabled && !d->appearance.fullRowSelection)
                     || (row == m_curRow && d->highlightedRow == -1 && d->appearance.fullRowSelection))
           )
        {
            edit->paintSelectionBackground(p, isEnabled(), txt, align, x, y_offset, w, h,
                                           isEnabled() ? palette().color(QPalette::Highlight) : QColor(200, 200, 200),//d->grayColor,
                                           p->fontMetrics(), columnReadOnly, d->appearance.fullRowSelection);
        }
    }

    if (!edit) {
        p->fillRect(0, 0, x2, y2, d->diagonalGrayPattern);
    }

// If we are in the focus cell, draw indication
    if (   m_currentItem == record && col == m_curCol //js: && !d->recordIndicator)
        && !d->appearance.fullRowSelection)
    {
//  kDebug() << ">>> CURRENT CELL ("<<m_curCol<<"," << m_curRow<<") focus="<<has_focus;
        if (isEnabled()) {
            p->setPen(d->appearance.textColor);
        } else {
            QPen gray_pen(p->pen());
            gray_pen.setColor(d->appearance.gridColor);
            p->setPen(gray_pen);
        }
        if (edit)
            edit->paintFocusBorders(p, cellValue, 0, 0, x2, y2);
        else
            p->drawRect(0, 0, x2, y2);
    }

    if (   (!m_newRowEditing && record == m_insertItem)
        || (m_newRowEditing && record == m_currentItem && cellValue.isNull()))
    {
        //we're in "insert record"
        if (tvcol->field()->isAutoIncrement()) { // "autonumber" column
            KexiDisplayUtils::paintAutonumberSign(d->autonumberSignDisplayParameters, p,
                                                  x, y_offset, w - x - x - ((align & Qt::AlignLeft) ? 2 : 0),
                                                  h, (Qt::Alignment)align);
        }
    }

    // draw text
    if (!txt.isEmpty()) {
        if (defaultValueDisplayed)
            p->setFont(d->defaultValueDisplayParameters.font);
        p->setPen(defaultPen);
        p->drawText(x, y_offset,
                    w - (x + x) - ((align & Qt::AlignLeft) ? 2 : 0)/*right space*/, h,
                    align, txt);
    }
#ifdef KEXITABLEVIEW_DEBUG_PAINT
    p->setPen(QPen(QColor(255, 0, 0, 150), 1, Qt::DashLine));
    p->drawRect(x, y_offset, w - 1, h - 1);
    kDebug() << cellValue << "x:" << x << "y:" <<  y_offset << "w:" << w << "h:" << h;
#endif
    p->restore();
}

QPoint KexiTableScrollArea::contentsToViewport2(const QPoint &p)
{
    return QPoint(p.x() - horizontalScrollBar()->value(), p.y() - verticalScrollBar()->value());
}

void KexiTableScrollArea::contentsToViewport2(int x, int y, int& vx, int& vy)
{
    const QPoint v = contentsToViewport2(QPoint(x, y));
    vx = v.x();
    vy = v.y();
}

QPoint KexiTableScrollArea::viewportToContents2(const QPoint& vp)
{
    return QPoint(vp.x() + horizontalScrollBar()->value(),
                  vp.y() + verticalScrollBar()->value());
}

void KexiTableScrollArea::paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch)
{
    //kDebug() << cx << cy << cw << ch;

    // Regions work with shorts, so avoid an overflow and adjust the
    // table size to the visible size
    QSize ts(tableSize());
    //kDebug() << ts;
    /* kDebug() << QString(" (cx:%1 cy:%2 cw:%3 ch:%4)")
          .arg(cx).arg(cy).arg(cw).arg(ch);
      kDebug() << QString(" (w:%3 h:%4)")
          .arg(ts.width()).arg(ts.height());*/

    // Region of the rect we should draw, calculated in viewport
    // coordinates, as a region can't handle bigger coordinates
    contentsToViewport2(cx, cy, cx, cy);
    QRegion reg(QRect(cx, cy, cw, ch));
    //kDebug() << "---cy-- " << verticalScrollBar()->value();
    // Subtract the table from it
    reg = reg.subtract(QRect(QPoint(0, 0), ts - QSize(0, + verticalScrollBar()->value())));

    // And draw the rectangles (transformed inc contents coordinates as needed)
    const QVector<QRect> rects(reg.rects());
    foreach(const QRect& rect, rects) {
        QRect realRect(viewportToContents2(rect.topLeft()), rect.size());
//        kDebug() << QString("- pEA: p->fillRect(x:%1 y:%2 w:%3 h:%4)")
//              .arg(rect.x()).arg(rect.y())
//              .arg(rect.width()).arg(rect.height())
//                   << viewportGeometry();
        p->fillRect(realRect, d->appearance.emptyAreaColor);
    }
}

void KexiTableScrollArea::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
// kDebug();
    m_contentsMousePressEvent_dblClick = true;
    contentsMousePressEvent(e);
    m_contentsMousePressEvent_dblClick = false;

    if (m_currentItem) {
        if (d->editOnDoubleClick && columnEditable(m_curCol) && columnType(m_curCol) != KexiDB::Field::Boolean) {
            KexiTableEdit *edit = tableEditorWidget(m_curCol, /*ignoreMissingEditor=*/true);
            if (edit && edit->handleDoubleClick()) {
                //nothing to do: editors like BLOB editor has custom handling of double clicking
            } else {
                startEditCurrentCell();
                //   createEditor(m_curRow, m_curCol, QString());
            }
        }

        emit itemDblClicked(m_currentItem, m_curRow, m_curCol);
    }
}

void KexiTableScrollArea::contentsMousePressEvent(QMouseEvent* e)
{
    setFocus();
    if (m_data->isEmpty() && !isInsertingEnabled()) {
        return;
    }
    //kDebug() << e->pos();
    const int x = e->pos().x();
    if (columnAt(x) == -1) { //outside a column
        return;
    }
    if (!d->moveCursorOnMouseRelease) {
        if (!handleContentsMousePressOrRelease(e, false))
            return;
    }

// kDebug()<< "by now the current items should be set, if not -> error + crash";
    if (e->button() == Qt::RightButton) {
        showContextMenu(e->globalPos());
    } else if (e->button() == Qt::LeftButton) {
        if (columnType(m_curCol) == KexiDB::Field::Boolean && columnEditable(m_curCol)) {
            //only accept clicking on the [x] rect (copied from KexiBoolTableEdit::setupContents())
            int s = qMax(d->rowHeight - 5, 12);
            s = qMin(d->rowHeight - 3, s);
            s = qMin(columnWidth(m_curCol) - 3, s); //avoid too large box
            const QRect r(
                columnPos(m_curCol) + qMax(columnWidth(m_curCol) / 2 - s / 2, 0),
                rowPos(m_curRow) + d->rowHeight / 2 - s / 2 /*- 1*/,
                s, s);
            //kDebug() << r;
            if (r.contains(e->pos())) {
//    kDebug() << "e->x:" << e->x() << " e->y:" << e->y() << " " << rowPos(m_curRow) <<
//     " " << columnPos(m_curCol);
                boolToggled();
            }
        }
//! @todo
#if 0
        else if (columnType(m_curCol) == QVariant::StringList && columnEditable(m_curCol)) {
            createEditor(m_curRow, m_curCol);
        }
#endif
    }
}

void KexiTableScrollArea::contentsMouseReleaseEvent(QMouseEvent* e)
{
    if (m_data->count() == 0 && !isInsertingEnabled())
        return;

    if (d->moveCursorOnMouseRelease)
        handleContentsMousePressOrRelease(e, true);

    int col = columnAt(e->pos().x());
    int row = rowAt(e->pos().y());

    if (!m_currentItem || col == -1 || row == -1 || col != m_curCol || row != m_curRow)//outside a current cell
        return;

    emit itemMouseReleased(m_currentItem, m_curRow, m_curCol);
}

bool KexiTableScrollArea::handleContentsMousePressOrRelease(QMouseEvent* e, bool release)
{
    Q_UNUSED(release);
    //kDebug() << "oldRow=" << m_curRow << " oldCol=" << m_curCol;

    int newrow, newcol;
    //compute clicked row nr
    const int x = e->pos().x();
    if (isInsertingEnabled()) {
        if (rowAt(e->pos().y()) == -1) {
            newrow = rowAt(e->pos().y() - d->rowHeight);
            if (newrow == -1 && m_data->count() > 0) {
                return false;
            }
            newrow++;
            kDebug() << "Clicked just on 'insert' record.";
        } else {
            // get new focus cell
            newrow = rowAt(e->pos().y());
        }
    } else {
        if (rowAt(e->pos().y()) == -1 || columnAt(x) == -1) {
            return false; //clicked outside a grid
        }
        // get new focus cell
        newrow = rowAt(e->pos().y());
    }
    newcol = columnAt(x);

    if (e->button() != Qt::NoButton) {
        setCursorPosition(newrow, newcol);
    }
    return true;
}

void KexiTableScrollArea::showContextMenu(const QPoint& _pos)
{
    if (!d->contextMenuEnabled || m_contextMenu->isEmpty())
        return;
    QPoint pos(_pos);
    if (pos == QPoint(-1, -1)) {
        pos = viewport()->mapToGlobal(QPoint(columnPos(m_curCol), rowPos(m_curRow) + d->rowHeight));
    }
    //show own context menu if configured
    selectRow(m_curRow);
    m_contextMenu->exec(pos);
}

void KexiTableScrollArea::contentsMouseMoveEvent(QMouseEvent *e)
{
    int row;
    const int col = columnAt(e->x());
    if (col < 0) {
        row = -1;
    } else {
        row = rowAt(e->y(), true /*ignoreEnd*/);
        if (row > (rowCount() - 1 + (isInsertingEnabled() ? 1 : 0)))
            row = -1; //no row to paint
    }
// kDebug() << " row="<<row<< " col="<<col;
    //update row highlight if needed
    if (d->appearance.rowMouseOverHighlightingEnabled) {
        if (row != d->highlightedRow) {
            const int oldRow = d->highlightedRow;
            d->highlightedRow = row;
            updateRow(oldRow);
            updateRow(d->highlightedRow);
            //currently selected (not necessary highlighted) record needs to be repainted
            updateRow(m_curRow);
            if (oldRow != d->highlightedRow) {
                d->verticalHeader->updateSection(oldRow);
            }
            d->verticalHeader->updateSection(d->highlightedRow);
        }
    }
}

static bool overrideEditorShortcutNeeded(QKeyEvent *e)
{
    //perhaps more to come...
    return e->key() == Qt::Key_Delete && e->modifiers() == Qt::ControlModifier;
}

bool KexiTableScrollArea::shortCutPressed(QKeyEvent *e, const QString &action_name)
{
    const int k = e->key();
    KAction *action = dynamic_cast<KAction*>(m_sharedActions.value(action_name));
    if (action) {
        if (!action->isEnabled())//this action is disabled - don't process it!
            return false;
#ifdef __GNUC__
#warning OK? (action->shortcut().primary() == QKeySequence( e->key()|e->modifiers() )
#else
#pragma WARNING( OK? (action->shortcut().primary() == QKeySequence( e->key()|e->modifiers() ) )
#endif
        if (action->shortcut().primary() == QKeySequence(e->key() | e->modifiers())
                || (action->shortcut().alternate() == QKeySequence(e->key() | e->modifiers())))
        {
            //special cases when we need to override editor's shortcut
            if (overrideEditorShortcutNeeded(e)) {
                return true;
            }
            return false;//this shortcut is owned by shared action - don't process it!
        }
    }

    //check default shortcut (when user app has no action shortcuts defined
    // but we want these shortcuts to still work)
    if (action_name == "data_save_row")
        return (k == Qt::Key_Return || k == Qt::Key_Enter) && e->modifiers() == Qt::ShiftModifier;
    if (action_name == "edit_delete_row")
        return k == Qt::Key_Delete && e->modifiers() == Qt::ControlModifier;
    if (action_name == "edit_delete")
        return k == Qt::Key_Delete && e->modifiers() == Qt::NoModifier;
    if (action_name == "edit_edititem")
        return k == Qt::Key_F2 && e->modifiers() == Qt::NoModifier;
    if (action_name == "edit_insert_empty_row")
        return k == Qt::Key_Insert && e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier);

    return false;
}

void KexiTableScrollArea::contentsContextMenuEvent(QContextMenuEvent* e)
{
    const bool nobtn = e->modifiers() == Qt::NoModifier;
    if (nobtn && e->reason() == QContextMenuEvent::Keyboard) {
        showContextMenu();
    }
}

void KexiTableScrollArea::keyPressEvent(QKeyEvent* e)
{
#ifdef KEXITABLEVIEW_DEBUG
    kDebug() << e;
#endif
    if (!hasData())
        return;
// kDebug() << "key=" <<e->key() << " txt=" <<e->text();

    const int k = e->key();
    const bool ro = isReadOnly();
    QWidget *w = focusWidget();
    if (!w || (w != viewport() && w != this && (!m_editor || !KexiUtils::hasParent(dynamic_cast<QObject*>(m_editor), w)))) {
        //don't process stranger's events
        e->ignore();
        return;
    }
    if (d->skipKeyPress) {
        d->skipKeyPress = false;
        e->ignore();
        return;
    }

    if (m_currentItem == 0 && (m_data->count() > 0 || isInsertingEnabled())) {
        setCursorPosition(0, 0);
    } else if (m_data->count() == 0 && !isInsertingEnabled()) {
        e->accept();
        return;
    }

    if (m_editor) {// if a cell is edited, do some special stuff
        if (k == Qt::Key_Escape) {
            cancelEditor();
            emit updateSaveCancelActions();
            e->accept();
            return;
        } else if (k == Qt::Key_Return || k == Qt::Key_Enter) {
            if (columnType(m_curCol) == KexiDB::Field::Boolean) {
                boolToggled();
            } else {
                acceptEditor();
            }
            e->accept();
            return;
        }
    } else if (rowEditing()) {// if a row is in edit mode, do some special stuff
        if (shortCutPressed(e, "data_save_row")) {
            kDebug() << "shortCutPressed!!!";
            acceptRowEdit();
            return;
        }
    }

    if (k == Qt::Key_Return || k == Qt::Key_Enter) {
        emit itemReturnPressed(m_currentItem, m_curRow, m_curCol);
    }

    int curRow = m_curRow;
    int curCol = m_curCol;

    const bool nobtn = e->modifiers() == Qt::NoModifier;
    bool printable = false;

    //check shared shortcuts
    if (!ro) {
        if (shortCutPressed(e, "edit_delete_row")) {
            deleteCurrentRow();
            e->accept();
            return;
        } else if (shortCutPressed(e, "edit_delete")) {
            deleteAndStartEditCurrentCell();
            e->accept();
            return;
        } else if (shortCutPressed(e, "edit_insert_empty_row")) {
            insertEmptyRow();
            e->accept();
            return;
        }
    }

    if (k == Qt::Key_Shift || k == Qt::Key_Alt || k == Qt::Key_Control || k == Qt::Key_Meta) {
        e->ignore();
    } else if (KexiDataAwareObjectInterface::handleKeyPress(
                   e, curRow, curCol, d->appearance.fullRowSelection))
    {
        if (e->isAccepted())
            return;
    } else if (k == Qt::Key_Backspace && nobtn) {
        if (!ro && columnType(curCol) != KexiDB::Field::Boolean && columnEditable(curCol))
            createEditor(curRow, curCol, QString(), true);
    } else if (k == Qt::Key_Space) {
        if (nobtn && !ro && columnEditable(curCol)) {
            if (columnType(curCol) == KexiDB::Field::Boolean) {
                boolToggled();
            } else
                printable = true; //just space key
        }
    } else if (k == Qt::Key_Escape) {
        if (nobtn && rowEditing()) {
            cancelRowEdit();
            return;
        }
    } else {
        //others:
        if ((nobtn && k == Qt::Key_Tab) || k == Qt::Key_Right) {
//! \todo add option for stopping at 1st column for Qt::Key_left
            //tab
            if (acceptEditor()) {
                if (curCol == (columnCount() - 1)) {
                    if (curRow < (rowCount() - 1 + (isInsertingEnabled() ? 1 : 0))) {//skip to next row
                        curRow++;
                        curCol = 0;
                    }
                } else
                    curCol++;
            }
        } else if ((e->modifiers() == Qt::ShiftModifier && k == Qt::Key_Tab)
                   || (nobtn && k == Qt::Key_Backtab)
                   || (e->modifiers() == Qt::ShiftModifier && k == Qt::Key_Backtab)
                   || k == Qt::Key_Left
                  ) {
//! \todo add option for stopping at last column
            //backward tab
            if (acceptEditor()) {
                if (curCol == 0) {
                    if (curRow > 0) {//skip to previous row
                        curRow--;
                        curCol = columnCount() - 1;
                    }
                } else
                    curCol--;
            }
        } else {
            KexiTableEdit *edit = tableEditorWidget(m_curCol);
            if (edit && edit->handleKeyPress(e, m_editor == edit)) {
                //try to handle the event @ editor's level
                e->accept();
                return;
            } else if (nobtn && (k == Qt::Key_Enter || k == Qt::Key_Return || shortCutPressed(e, "edit_edititem"))) {
                //this condition is moved after handleKeyPress() to allow to everride enter key as well
                startEditOrToggleValue();
            } else {
                kDebug() << "default";
                if (e->text().isEmpty() || !e->text()[0].isPrint()) {
                    kDebug() << "NOT PRINTABLE: 0x0" << QString("%1").arg(k, 0, 16);
                    //    e->ignore();
                    QScrollArea::keyPressEvent(e);
                    return;
                }
                printable = true;
            }
        }
    }
    //finally: we've printable char:
    if (printable && !ro) {
        KexiDB::TableViewColumn *tvcol = m_data->column(curCol);
        if (tvcol->acceptsFirstChar(e->text()[0])) {
            kDebug() << "ev pressed: acceptsFirstChar()==true";
            //   if (e->text()[0].isPrint())
            createEditor(curRow, curCol, e->text(), true);
        } else {
//! @todo show message "key not allowed eg. on a statusbar"
            kDebug() << "ev pressed: acceptsFirstChar()==false";
        }
    }

    m_vScrollBarValueChanged_enabled = false;

    // if focus cell changes, repaint
    setCursorPosition(curRow, curCol, DontEnsureCursorVisibleIfPositionUnchanged);

    m_vScrollBarValueChanged_enabled = true;

    e->accept();
}

void KexiTableScrollArea::emitSelected()
{
    if (m_currentItem)
        emit itemSelected(m_currentItem);
}

int KexiTableScrollArea::rowsPerPage() const
{
    return viewport()->height() / d->rowHeight;
}

KexiDataItemInterface *KexiTableScrollArea::editor(int col, bool ignoreMissingEditor)
{
    if (!m_data || col < 0 || col >= columnCount())
        return 0;
    KexiDB::TableViewColumn *tvcol = m_data->column(col);

    //find the editor for this column
    KexiTableEdit *editor = d->editors.value(tvcol);
    if (editor)
        return editor;

    //not found: create
    editor = KexiCellEditorFactory::createEditor(*tvcol, d->scrollAreaWidget);
    if (!editor) {//create error!
        if (!ignoreMissingEditor) {
            //! @todo show error???
            cancelRowEdit();
        }
        return 0;
    }
    editor->hide();
    if (m_data->cursor() && m_data->cursor()->query())
        editor->createInternalEditor(*m_data->cursor()->query());

    connect(editor, SIGNAL(editRequested()), this, SLOT(slotEditRequested()));
    connect(editor, SIGNAL(cancelRequested()), this, SLOT(cancelEditor()));
    connect(editor, SIGNAL(acceptRequested()), this, SLOT(acceptEditor()));

    editor->resize(columnWidth(col), rowHeight());
    editor->installEventFilter(this);
    if (editor->widget())
        editor->widget()->installEventFilter(this);
    //store
    d->editors.insert(tvcol, editor);
    return editor;
}

KexiTableEdit* KexiTableScrollArea::tableEditorWidget(int col, bool ignoreMissingEditor)
{
    return dynamic_cast<KexiTableEdit*>(editor(col, ignoreMissingEditor));
}

void KexiTableScrollArea::editorShowFocus(int row, int col)
{
    Q_UNUSED(row);
    KexiDataItemInterface *edit = editor(col);
    if (edit) {
        //kDebug() << "IN";
        QRect rect = cellGeometry(m_curRow, m_curCol);
        edit->showFocus(rect, isReadOnly() || m_data->column(col)->isReadOnly());
    }
}

void KexiTableScrollArea::slotEditRequested()
{
    createEditor(m_curRow, m_curCol);
}

void KexiTableScrollArea::reloadData()
{
    KexiDataAwareObjectInterface::reloadData();
    d->scrollAreaWidget->update();
}

void KexiTableScrollArea::createEditor(int row, int col, const QString& addText, bool removeOld)
{
    //kDebug() << "addText:" << addText << "removeOld:" << removeOld;
    if (isReadOnly()) {
        kDebug() << "DATA IS READ ONLY!";
        return;
    }

    if (m_data->column(col)->isReadOnly()) {//d->pColumnModes.at(d->numCols-1) & ColumnReadOnly)
        kDebug() << "COL IS READ ONLY!";
        return;
    }

    const bool startRowEdit = !rowEditing(); //remember if we're starting row edit

    if (!rowEditing()) {
        //we're starting row editing session
        m_data->clearRowEditBuffer();

        setRowEditing(true);
        //indicate on the vheader that we are editing:
        if (isInsertingEnabled() && m_currentItem == m_insertItem) {
            //we should know that we are in state "new record editing"
            m_newRowEditing = true;
            //'insert' row editing: show another row after that:
            m_data->append(m_insertItem);
            //new empty 'inserting' item
            m_insertItem = m_data->createItem();
            //TODO d->verticalHeader->addLabel();
            d->verticalHeaderAlreadyAdded = true;
            updateWidgetContentsSize();
            //refr. current and next row
            d->scrollAreaWidget->update(columnPos(0), rowPos(row),
                                        viewport()->width(), d->rowHeight*2);
//! @todo this breaks behaviour (cursor is skipping, etc.): qApp->processEvents(500);
            ensureVisible(columnPos(m_curCol), rowPos(row + 1) + d->rowHeight - 1, columnWidth(m_curCol), d->rowHeight);

            d->verticalHeader->setOffset(verticalScrollBar()->value());
        }
        d->verticalHeader->updateSection(m_curRow);
    }

    KexiTableEdit *editorWidget = tableEditorWidget(col);
    m_editor = editorWidget;
    if (!editorWidget)
        return;

    m_editor->setValue(*bufferedValueAt(col, !removeOld/*useDefaultValueIfPossible*/), addText, removeOld);
    if (m_editor->hasFocusableWidget()) {
        editorWidget->move(columnPos(m_curCol), rowPos(m_curRow));
        editorWidget->resize(columnWidth(m_curCol), rowHeight());
        editorWidget->show();

        m_editor->setFocus();
    }

    if (startRowEdit) {
        m_navPanel->showEditingIndicator(true); //this will allow to enable 'next' btn
        emit rowEditStarted(m_curRow);
    }
    m_editor->installListener(this);
}

void KexiTableScrollArea::focusOutEvent(QFocusEvent* e)
{
    KexiDataAwareObjectInterface::focusOutEvent(e);
}

bool KexiTableScrollArea::focusNextPrevChild(bool /*next*/)
{
    return false; //special Tab/BackTab meaning
}

void KexiTableScrollArea::resizeEvent(QResizeEvent *e)
{
    if (d->insideResizeEvent)
        return;
    d->insideResizeEvent = true;
    QScrollArea::resizeEvent(e);

    if ((viewport()->height() - e->size().height()) <= d->rowHeight) {
        slotUpdate();
        triggerUpdate();
    }
    d->insideResizeEvent = false;
}

void KexiTableScrollArea::showEvent(QShowEvent *e)
{
    QScrollArea::showEvent(e);
    if (!d->maximizeColumnsWidthOnShow.isEmpty()) {
        maximizeColumnsWidth(d->maximizeColumnsWidthOnShow);
        d->maximizeColumnsWidthOnShow.clear();
    }

    if (m_initDataContentsOnShow) {
        //full init
        m_initDataContentsOnShow = false;
        initDataContents();
    } else {
        //just update size
        updateScrollAreaWidgetSize();
    }
    updateGeometries();

    //now we can ensure cell's visibility ( if there was such a call before show() )
    if (d->ensureCellVisibleOnShow != QPoint(-1, -1)) {
        ensureCellVisible(d->ensureCellVisibleOnShow.y(), d->ensureCellVisibleOnShow.x());
        d->ensureCellVisibleOnShow = QPoint(-1, -1); //reset the flag
    }
    if (d->firstShowEvent) {
        ensureVisible(0, 0, 0, 0); // needed because for small geometries contents were moved 1/2 of row height up
        d->firstShowEvent = false;
    }
    updateViewportMargins();
}

void KexiTableScrollArea::dragMoveEvent(QDragMoveEvent *e)
{
    if (!hasData())
        return;
    if (m_dropsAtRowEnabled) {
        QPoint p = e->pos();
        int row = rowAt(p.y());
        if ((p.y() % d->rowHeight) > (d->rowHeight*2 / 3)) {
            row++;
        }
        KexiDB::RecordData *record = m_data->at(row);
        emit dragOverRow(record, row, e);
        if (e->isAccepted()) {
            if (m_dragIndicatorLine >= 0 && m_dragIndicatorLine != row) {
                //erase old indicator
                updateRow(m_dragIndicatorLine);
            }
            if (m_dragIndicatorLine != row) {
                m_dragIndicatorLine = row;
                updateRow(m_dragIndicatorLine);
            }
        } else {
            if (m_dragIndicatorLine >= 0) {
                //erase old indicator
                updateRow(m_dragIndicatorLine);
            }
            m_dragIndicatorLine = -1;
        }
    }
    else {
        e->accept();
    }
}

void KexiTableScrollArea::dropEvent(QDropEvent *e)
{
    if (!hasData())
        return;
    if (m_dropsAtRowEnabled) {
        //we're no longer dragging over the table
        if (m_dragIndicatorLine >= 0) {
            int row2update = m_dragIndicatorLine;
            m_dragIndicatorLine = -1;
            updateRow(row2update);
        }
        QPoint p = e->pos();
        int row = rowAt(p.y());
        if ((p.y() % d->rowHeight) > (d->rowHeight*2 / 3)) {
            row++;
        }
        KexiDB::RecordData *record = m_data->at(row);
        KexiDB::RecordData *newRecord = 0;
        emit droppedAtRow(record, row, e, newRecord);
        if (newRecord) {
            const int realRow = (row == m_curRow ? -1 : row);
            insertItem(newRecord, realRow);
            setCursorPosition(row, 0);
        }
    }
}

void KexiTableScrollArea::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_UNUSED(e);
    if (!hasData())
        return;
    if (m_dropsAtRowEnabled) {
        //we're no longer dragging over the table
        if (m_dragIndicatorLine >= 0) {
            int row2update = m_dragIndicatorLine;
            m_dragIndicatorLine = -1;
            updateRow(row2update);
        }
    }
}

void KexiTableScrollArea::updateCell(int row, int col)
{
//    kDebug() << row << col;
    d->scrollAreaWidget->update(cellGeometry(row, col));
}

void KexiTableScrollArea::updateCurrentCell()
{
    updateCell(m_curRow, m_curCol);
}

void KexiTableScrollArea::updateRow(int row)
{
//    kDebug()<<row << horizontalScrollBar()->value() << rowPos(row) << viewport()->width() << rowHeight();
    if (row < 0 || row >= (rowCount() + 2/* sometimes we want to refresh the row after last*/))
        return;
    //kDebug() << horizontalScrollBar()->value() << " " << verticalScrollBar()->value();
    //kDebug() << QRect( columnPos( leftcol ), rowPos(row), viewport()->width(), rowHeight() );
    d->scrollAreaWidget->update(horizontalScrollBar()->value(), rowPos(row),
                                viewport()->width(), rowHeight());
}

void KexiTableScrollArea::slotColumnWidthChanged(int column, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);
    updateScrollAreaWidgetSize();
    d->scrollAreaWidget->update(d->horizontalHeader->offset() + columnPos(column), d->verticalHeader->offset(),
                                viewport()->width() - columnPos(column), viewport()->height());
    //kDebug() << QRect(columnPos(column), 0, viewport()->width() - columnPos(column), viewport()->height());

    QWidget *editorWidget = dynamic_cast<QWidget*>(m_editor);
    if (editorWidget) {
        editorWidget->move(columnPos(m_curCol), rowPos(m_curRow));
        editorWidget->resize(columnWidth(m_curCol), rowHeight());
    }
    updateGeometries();
    if (d->firstTimeEnsureCellVisible) {
        d->firstTimeEnsureCellVisible = false;
        ensureCellVisible( currentRow(), currentColumn() );
    }
    editorShowFocus(m_curRow, m_curCol);
}

void KexiTableScrollArea::slotSectionHandleDoubleClicked(int section)
{
    adjustColumnWidthToContents(section);
    slotColumnWidthChanged(0, 0, 0); //to update contents and redraw
}

void KexiTableScrollArea::setSortingEnabled(bool set)
{
    KexiDataAwareObjectInterface::setSortingEnabled(set);
    d->horizontalHeader->setSortingEnabled(set);
}

void KexiTableScrollArea::sortColumnInternal(int col, int order)
{
    KexiDataAwareObjectInterface::sortColumnInternal(col, order);
}

int KexiTableScrollArea::leftMargin() const
{
    return verticalHeaderVisible() ? d->verticalHeader->width() : 0;
}

int KexiTableScrollArea::topMargin() const
{
    //kDebug() << d->horizontalHeader->height();
    return horizontalHeaderVisible() ? d->horizontalHeader->height() : 0;
}

void KexiTableScrollArea::updateGeometries()
{
    const QSize ts(tableSize());
    if (d->horizontalHeader->offset() && ts.width() < (d->horizontalHeader->offset() + d->horizontalHeader->width())) {
        horizontalScrollBar()->setValue(ts.width() - d->horizontalHeader->width());
    }
    int frameLeftMargin = style()->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, this) + 2;
    int frameTopMargin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, this) + 2;
    d->horizontalHeader->move(leftMargin() + frameLeftMargin, frameTopMargin);
    d->verticalHeader->move(frameLeftMargin, ///*topMargin() + */frameTopMargin);
                            d->horizontalHeader->geometry().bottom() + 1);
}

int KexiTableScrollArea::columnWidth(int col) const
{
    if (!hasData())
        return 0;
    int vcID = m_data->visibleColumnIndex(col);
    //kDebug () << vcID << d->horizontalHeader->sectionSize(vcID);
    return (vcID == -1) ? 0 : d->horizontalHeader->sectionSize(vcID);
}

int KexiTableScrollArea::rowHeight() const
{
    return d->rowHeight;
}

int KexiTableScrollArea::columnPos(int col) const
{
    if (!hasData())
        return 0;
    //if this column is hidden, find first column before that is visible
    int c = qMin(col, (int)m_data->columnCount() - 1), vcID = 0;
    while (c >= 0 && (vcID = m_data->visibleColumnIndex(c)) == -1)
        c--;
    if (c < 0)
        return 0;
    if (c == col)
        return d->horizontalHeader->sectionPosition(vcID);
    return d->horizontalHeader->sectionPosition(vcID) + d->horizontalHeader->sectionSize(vcID);
}

int KexiTableScrollArea::rowPos(int row) const
{
    return d->rowHeight*row;
}

int KexiTableScrollArea::columnAt(int pos) const
{
    if (!hasData())
        return -1;
    const int realPos = pos - d->horizontalHeader->offset();
    const int c = d->horizontalHeader->logicalIndexAt(realPos);
    if (c < 0)
        return c;
    return m_data->globalIndexOfVisibleColumn(c);
}

int KexiTableScrollArea::rowAt(int pos, bool ignoreEnd) const
{
    if (!hasData())
        return -1;
    pos /= d->rowHeight;
    if (pos < 0)
        return 0;
    if ((pos >= (int)m_data->count()) && !ignoreEnd)
        return -1;
    return pos;
}

QRect KexiTableScrollArea::cellGeometry(int row, int col) const
{
    return QRect(columnPos(col), rowPos(row),
                 columnWidth(col), rowHeight());
}

QSize KexiTableScrollArea::tableSize() const
{
    if ((rowCount() + (isInsertingEnabled() ? 1 : 0)) > 0 && columnCount() > 0) {
        /*  kDebug() << columnPos( columnCount() - 1 ) + columnWidth( columnCount() - 1 )
              << ", " << rowPos( rowCount()-1+(isInsertingEnabled()?1:0)) + d->rowHeight */
//  kDebug() << m_navPanel->isVisible() <<" "<<m_navPanel->height()<<" "
//           << horizontalScrollBar()->sizeHint().height()<<" "<<rowPos( rowCount()-1+(isInsertingEnabled()?1:0));
        QSize s(
            columnPos(columnCount() - 1) + columnWidth(columnCount() - 1),
            rowPos(rowCount() - 1 + (isInsertingEnabled() ? 1 : 0)) + d->rowHeight
            + d->internal_bottomMargin
        );

//  kDebug() << rowCount()-1 <<" "<< (isInsertingEnabled()?1:0) <<" "<< (rowEditing()?1:0) << " " <<  s;
#ifdef KEXITABLEVIEW_DEBUG
kDebug() << s << "cw(last):" << columnWidth(columnCount() - 1);
#endif
        return s;
    }
    return QSize(0, 0);
}

void KexiTableScrollArea::ensureCellVisible(int row, int col)
{
    if (!isVisible()) {
        //the table is invisible: we can't ensure visibility now
        d->ensureCellVisibleOnShow = QPoint(row, col);
        return;
    }

    //quite clever: ensure the cell is visible:
    QRect r(columnPos(col == -1 ? m_curCol : col) - 1, rowPos(row) + (d->appearance.fullRowSelection ? 1 : 0) - 1,
            columnWidth(col == -1 ? m_curCol : col)  + 2, rowHeight() + 2);

    if (navPanelWidgetVisible() && horizontalScrollBar()->isHidden()) {
        //a hack: for visible navigator: increase height of the visible rect 'r'
        r.setBottom(r.bottom() + navPanelWidget()->height());
    }

    QPoint pcenter = r.center();
#ifdef KEXITABLEVIEW_DEBUG
    kDebug() << pcenter.x() << pcenter.y() << (r.width() / 2)  << (r.height() / 2);
#endif
    ensureVisible(pcenter.x(), pcenter.y(), r.width() / 2, r.height() / 2);
}

void KexiTableScrollArea::updateAfterCancelRowEdit()
{
    KexiDataAwareObjectInterface::updateAfterCancelRowEdit();
    m_navPanel->showEditingIndicator(false);
}

void KexiTableScrollArea::updateAfterAcceptRowEdit()
{
    KexiDataAwareObjectInterface::updateAfterAcceptRowEdit();
    m_navPanel->showEditingIndicator(false);
}

bool KexiTableScrollArea::getVisibleLookupValue(QVariant& cellValue, KexiTableEdit *edit,
        KexiDB::RecordData *record, KexiDB::TableViewColumn *tvcol) const
{
    if (edit->columnInfo() && edit->columnInfo()->indexForVisibleLookupValue() != -1
            && edit->columnInfo()->indexForVisibleLookupValue() < (int)record->count()) {
        const QVariant *visibleFieldValue = 0;
        if (m_currentItem == record && m_data->rowEditBuffer()) {
            visibleFieldValue = m_data->rowEditBuffer()->at(
                                    *tvcol->visibleLookupColumnInfo(), false/*!useDefaultValueIfPossible*/);
        }

        if (visibleFieldValue)
            //(use bufferedValueAt() - try to get buffered visible value for lookup field)
            cellValue = *visibleFieldValue;
        else
            cellValue /*txt*/ = record->at(edit->columnInfo()->indexForVisibleLookupValue());
        return true;
    }
    return false;
}

//reimpl.
void KexiTableScrollArea::removeEditor()
{
    if (!m_editor)
        return;
    KexiDataAwareObjectInterface::removeEditor();
    viewport()->setFocus();
}

void KexiTableScrollArea::slotRowRepaintRequested(KexiDB::RecordData& record)
{
    updateRow(m_data->indexOf(&record));
}

#ifndef KEXI_NO_PRINT
void
KexiTableScrollArea::print(QPrinter & /*printer*/ , QPrintDialog & /*printDialog*/)
{
#if 0
    int leftMargin = printer.margins().width() + 2 + d->rowHeight;
    int topMargin = printer.margins().height() + 2;
// int bottomMargin = topMargin + ( printer.realPageSize()->height() * printer.resolution() + 36 ) / 72;
    int bottomMargin = 0;
    kDebug() << "bottom:" << bottomMargin;

    QPainter p(&printer);

    KexiDB::RecordData *i;
    int width = leftMargin;
    for (int col = 0; col < columnCount(); col++) {
        p.fillRect(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight, QBrush(Qt::gray));
        p.drawRect(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight);
        p.drawText(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight, Qt::AlignLeft | Qt::AlignVCenter,
                   d->horizontalHeader->label(col));
        width = width + columnWidth(col);
    }

    int yOffset = topMargin;
    int row = 0;
    int right = 0;
    for (i = m_data->first(); i; i = m_data->next()) {
        if (!i->isInsertItem()) {
            kDebug() << "row=" << row << "y=" << yOffset;
            int xOffset = leftMargin;
            for (int col = 0; col < columnCount(); col++) {
                kDebug() << "col=" << col << "x=" << xOffset;
                p.saveWorldMatrix();
                p.translate(xOffset, yOffset);
                paintCell(&p, i, row, col, QRect(0, 0, columnWidth(col) + 1, d->rowHeight), true);
                p.restoreWorldMatrix();
                xOffset = xOffset + columnWidth(col);
                right = xOffset;
            }

            row++;
            yOffset = topMargin  + row * d->rowHeight;
        }

        if (yOffset > 900) {
            p.drawLine(leftMargin, topMargin, leftMargin, yOffset);
            p.drawLine(leftMargin, topMargin, right - 1, topMargin);
            printer.newPage();
            yOffset = topMargin;
            row = 0;
        }
    }
    p.drawLine(leftMargin, topMargin, leftMargin, yOffset);
    p.drawLine(leftMargin, topMargin, right - 1, topMargin);
    p.end();
#endif
}
#endif

KexiDB::Field* KexiTableScrollArea::field(int column) const
{
    if (!m_data || !m_data->column(column))
        return 0;
    return m_data->column(column)->field();
}

void KexiTableScrollArea::adjustColumnWidthToContents(int column)
{
    if (!hasData())
        return;
    if (column == -1) {
        const int cols = columnCount();
        for (int i = 0; i < cols; i++)
            adjustColumnWidthToContents(i);
        return;
    }

    int indexOfVisibleColumn = (m_data->column(column) && m_data->column(column)->columnInfo())
                               ? m_data->column(column)->columnInfo()->indexForVisibleLookupValue() : -1;
    if (-1 == indexOfVisibleColumn)
        indexOfVisibleColumn = column;

    if (indexOfVisibleColumn < 0)
        return;

    QList<KexiDB::RecordData*>::ConstIterator it(m_data->constBegin());
    if (it != m_data->constEnd() && (*it)->count() <= indexOfVisibleColumn)
        return;

    KexiCellEditorFactoryItem *item = KexiCellEditorFactory::item(columnType(indexOfVisibleColumn));
    if (!item)
        return;
    int maxw = horizontalHeaderVisible() ? d->horizontalHeader->preferredSectionSize(column) : 0;
    if (maxw == 0 && m_data->isEmpty())
        return; //nothing to adjust

//! \todo js: this is NOT EFFECTIVE for big data sets!!!!

    KexiTableEdit *ed = tableEditorWidget(column/* not indexOfVisibleColumn*/);
    if (ed) {
        for (it = m_data->constBegin(); it != m_data->constEnd(); ++it) {
            const int wfw = ed->widthForValue((*it)->at(indexOfVisibleColumn), fontMetrics());
            maxw = qMax(maxw, wfw);
        }
        const bool focused = currentColumn() == column;
        maxw += (fontMetrics().width("  ") + ed->leftMargin() + ed->rightMargin(focused) + 2);
    }
    if (maxw < KEXITV_MINIMUM_COLUMN_WIDTH)
        maxw = KEXITV_MINIMUM_COLUMN_WIDTH; //not too small
    //kDebug() << "setColumnWidth(column=" << column
    //    << ", indexOfVisibleColumn=" << indexOfVisibleColumn << ", width=" << maxw << " )";
    setColumnWidth(column/* not indexOfVisibleColumn*/, maxw);
}

void KexiTableScrollArea::setColumnWidth(int column, int width)
{
    if (columnCount() <= column || column < 0)
        return;
    d->horizontalHeader->resizeSection(column, width);
    editorShowFocus(m_curRow, m_curCol);
}

void KexiTableScrollArea::maximizeColumnsWidth(const QList<int> &columnList)
{
    if (!isVisible()) {
        d->maximizeColumnsWidthOnShow += columnList;
        return;
    }
    if (width() <= d->horizontalHeader->headerWidth())
        return;
    //sort the list and make it unique
    QList<int> cl, sortedList(columnList);
    qSort(sortedList);
    int i = -999;
    QList<int>::ConstIterator it(sortedList.constBegin()), end(sortedList.constEnd());
    for (; it != end; ++it) {
        if (i != (*it)) {
            cl += (*it);
            i = (*it);
        }
    }
    //resize
    int sizeToAdd = (width() - d->horizontalHeader->headerWidth()) / cl.count()
                    - d->verticalHeader->width();
    if (sizeToAdd <= 0)
        return;
    end = cl.constEnd();
    for (it = cl.constBegin(); it != end; ++it) {
        int w = d->horizontalHeader->sectionSize(*it);
        if (w > 0) {
            d->horizontalHeader->resizeSection(*it, w + sizeToAdd);
        }
    }
    d->scrollAreaWidget->update();
    editorShowFocus(m_curRow, m_curCol);
}

void KexiTableScrollArea::adjustHorizontalHeaderSize()
{
    d->horizontalHeader->resizeSections(QHeaderView::Stretch);
}

void KexiTableScrollArea::setColumnResizeEnabled(int column, bool set)
{
    if (column < 0 || column >= columnCount()) {
        return;
    }
    d->horizontalHeader->setResizeMode(column, set ? QHeaderView::Interactive : QHeaderView::Fixed);
}

void KexiTableScrollArea::setColumnsResizeEnabled(bool set)
{
    d->horizontalHeader->setResizeMode(set ? QHeaderView::Interactive : QHeaderView::Fixed);
}

void KexiTableScrollArea::setEditableOnDoubleClick(bool set)
{
    d->editOnDoubleClick = set;
}
bool KexiTableScrollArea::editableOnDoubleClick() const
{
    return d->editOnDoubleClick;
}

bool KexiTableScrollArea::verticalHeaderVisible() const
{
    return d->verticalHeader->isVisible();
}

void KexiTableScrollArea::setVerticalHeaderVisible(bool set)
{
    d->verticalHeader->setVisible(set);
    updateViewportMargins();
}

void KexiTableScrollArea::updateViewportMargins()
{
    d->viewportMargins = QMargins(
        leftMargin() + 1,
        topMargin() + 1,
        0, // right
        0 // bottom
    );
    setViewportMargins(d->viewportMargins);
    kDebug () << d->viewportMargins;
}

bool KexiTableScrollArea::horizontalHeaderVisible() const
{
    return d->horizontalHeaderVisible;
}

void KexiTableScrollArea::setHorizontalHeaderVisible(bool set)
{
    d->horizontalHeaderVisible = set; //needed because isVisible() is not always accurate
    d->horizontalHeader->setVisible(set);
    updateViewportMargins();
}

void KexiTableScrollArea::triggerUpdate()
{
// kDebug();
    d->pUpdateTimer->start(20);
}

void KexiTableScrollArea::setHBarGeometry(QScrollBar & hbar, int x, int y, int w, int h)
{
#ifdef KEXITABLEVIEW_DEBUG
    /*todo*/
    kDebug();
#endif
    if (d->appearance.navigatorEnabled) {
        m_navPanel->setHBarGeometry(hbar, x, y, w, h);
    } else {
        hbar.setGeometry(x , y, w, h);
    }
}

void KexiTableScrollArea::setSpreadSheetMode()
{
    KexiDataAwareObjectInterface::setSpreadSheetMode();
    //copy m_navPanelEnabled flag
    Appearance a = d->appearance;
    a.navigatorEnabled = m_navPanelEnabled;
    setAppearance(a);
}

int KexiTableScrollArea::validRowNumber(const QString& text)
{
    bool ok = true;
    int r = text.toInt(&ok);
    if (!ok || r < 1)
        r = 1;
    else if (r > (rowCount() + (isInsertingEnabled() ? 1 : 0)))
        r = rowCount() + (isInsertingEnabled() ? 1 : 0);
    return r -1;
}

void KexiTableScrollArea::moveToRecordRequested(uint r)
{
    if (r > uint(rowCount() + (isInsertingEnabled() ? 1 : 0)))
        r = rowCount() + (isInsertingEnabled() ? 1 : 0);
    setFocus();
    selectRow(r);
}

void KexiTableScrollArea::moveToLastRecordRequested()
{
    setFocus();
    selectRow(rowCount() > 0 ? (rowCount() - 1) : 0);
}

void KexiTableScrollArea::moveToPreviousRecordRequested()
{
    setFocus();
    selectPrevRow();
}

void KexiTableScrollArea::moveToNextRecordRequested()
{
    setFocus();
    selectNextRow();
}

void KexiTableScrollArea::moveToFirstRecordRequested()
{
    setFocus();
    selectFirstRow();
}

void KexiTableScrollArea::copySelection()
{
    if (m_currentItem && m_curCol != -1) {
        KexiTableEdit *edit = tableEditorWidget(m_curCol);
        QVariant defaultValue;
        const bool defaultValueDisplayed = isDefaultValueDisplayed(m_currentItem, m_curCol, &defaultValue);
        if (edit) {
            QVariant visibleValue;
            getVisibleLookupValue(visibleValue, edit, m_currentItem, m_data->column(m_curCol));
            edit->handleCopyAction(
                defaultValueDisplayed ? defaultValue : m_currentItem->at(m_curCol),
                visibleValue);
        }
    }
}

void KexiTableScrollArea::cutSelection()
{
    //try to handle @ editor's level
    KexiTableEdit *edit = tableEditorWidget(m_curCol);
    if (edit)
        edit->handleAction("edit_cut");
}

void KexiTableScrollArea::paste()
{
    //try to handle @ editor's level
    KexiTableEdit *edit = tableEditorWidget(m_curCol);
    if (edit)
        edit->handleAction("edit_paste");
}

bool KexiTableScrollArea::eventFilter(QObject *o, QEvent *e)
{
    //don't allow to stole key my events by others:
// kDebug() << "spontaneous " << e->spontaneous() << " type=" << e->type();
#ifdef KEXITABLEVIEW_DEBUG
    if (e->type() != QEvent::Paint
        && e->type() != QEvent::Leave
        && e->type() != QEvent::MouseMove
        && e->type() != QEvent::HoverMove
        && e->type() != QEvent::HoverEnter
        && e->type() != QEvent::HoverLeave)
    {
        kDebug() << e << o;
    }
    if (e->type() == QEvent::Paint) {
        kDebug() << "PAINT!" << static_cast<QPaintEvent*>(e) << static_cast<QPaintEvent*>(e)->rect();
    }
#endif
    if (e->type() == QEvent::KeyPress) {
        if (e->spontaneous()) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            const int k = ke->key();
            int mods = ke->modifiers();
            //cell editor's events:
            //try to handle the event @ editor's level
            KexiTableEdit *edit = tableEditorWidget(m_curCol);
            if (edit && edit->handleKeyPress(ke, m_editor == edit)) {
                ke->accept();
                return true;
            }
            else if (m_editor && (o == dynamic_cast<QObject*>(m_editor) || o == m_editor->widget()))
            {
                if ((k == Qt::Key_Tab && (mods == Qt::NoModifier || mods == Qt::ShiftModifier))
                        || (overrideEditorShortcutNeeded(ke))
                        || (k == Qt::Key_Enter || k == Qt::Key_Return || k == Qt::Key_Up || k == Qt::Key_Down)
                        || (k == Qt::Key_Left && m_editor->cursorAtStart())
                        || (k == Qt::Key_Right && m_editor->cursorAtEnd())
                   )
                {
                    //try to steal the key press from editor or it's internal widget...
                    keyPressEvent(ke);
                    if (ke->isAccepted())
                        return true;
                }
            }
        }
    } else if (e->type() == QEvent::Leave) {
        if (   o == d->scrollAreaWidget
            && d->appearance.rowMouseOverHighlightingEnabled
            && d->appearance.persistentSelections)
        {
            if (d->highlightedRow != -1) {
                int oldRow = d->highlightedRow;
                d->highlightedRow = -1;
                updateRow(oldRow);
                d->verticalHeader->updateSection(oldRow);
                const bool dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
                    = d->appearance.rowHighlightingEnabled && !d->appearance.persistentSelections;
                if (oldRow != m_curRow && m_curRow >= 0) {
                    if (!dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted) {
                        //no highlight for now: show selection again
                        updateRow(m_curRow);
                    }
                }
            }
        }
        d->recentCellWithToolTip = QPoint(-1, -1);
    }
    else if (o == viewport() && e->type() == QEvent::DragEnter) {
      e->accept();
    }
    return QScrollArea::eventFilter(o, e);
}

void KexiTableScrollArea::setBottomMarginInternal(int pixels)
{
    d->internal_bottomMargin = pixels;
}

void KexiTableScrollArea::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::PaletteChange: {
        d->verticalHeader->setSelectionBackgroundColor(palette().color(QPalette::Highlight));
        d->horizontalHeader->setSelectionBackgroundColor(palette().color(QPalette::Highlight));
        break;
    }
    default:;
    }
    QScrollArea::changeEvent(e);
}

const KexiTableScrollArea::Appearance& KexiTableScrollArea::appearance() const
{
    return d->appearance;
}

void KexiTableScrollArea::setAppearance(const Appearance& a)
{
    setFont(font()); //this also updates contents
    if (a.fullRowSelection) {
        d->rowHeight -= 1;
    } else {
        d->rowHeight += 1;
    }
    if (d->verticalHeader) {
        d->verticalHeader->setDefaultSectionSize(d->rowHeight);
    }
    if (a.rowHighlightingEnabled) {
        m_updateEntireRowWhenMovingToOtherRow = true;
    }
    navPanelWidget()->setVisible(a.navigatorEnabled);
    d->highlightedRow = -1;
//! @todo is setMouseTracking useful for other purposes?
    viewport()->setMouseTracking(a.rowMouseOverHighlightingEnabled);
    d->appearance = a;
    updateViewportMargins();
}

int KexiTableScrollArea::highlightedRow() const
{
    return d->highlightedRow;
}

void KexiTableScrollArea::setHighlightedRow(int row)
{
    if (row != -1) {
        row = qMin(rowCount() - 1 + (isInsertingEnabled() ? 1 : 0), row);
        row = qMax(0, row);
    }
    const int previouslyHighlightedRow = d->highlightedRow;
    if (previouslyHighlightedRow == row) {
        if (previouslyHighlightedRow != -1)
            updateRow(previouslyHighlightedRow);
        return;
    }
    d->highlightedRow = row;
    if (d->highlightedRow != -1)
        updateRow(d->highlightedRow);

    if (previouslyHighlightedRow != -1)
        updateRow(previouslyHighlightedRow);

    if (m_curRow >= 0 && (previouslyHighlightedRow == -1 || previouslyHighlightedRow == m_curRow)
            && d->highlightedRow != m_curRow && !d->appearance.persistentSelections)
    {
        //currently selected row needs to be repainted
        updateRow(m_curRow);
    }
}

KexiDB::RecordData *KexiTableScrollArea::highlightedItem() const
{
    return d->highlightedRow == -1 ? 0 : m_data->at(d->highlightedRow);
}

QScrollBar* KexiTableScrollArea::verticalScrollBar() const
{
    return QScrollArea::verticalScrollBar();
}

int KexiTableScrollArea::lastVisibleRow() const
{
    return rowAt(verticalScrollBar()->value());
}

void KexiTableScrollArea::valueChanged(KexiDataItemInterface* item)
{
#ifdef KEXITABLEVIEW_DEBUG
    kDebug() << item->field()->name() << item->value();
#else
    Q_UNUSED(item);
#endif
    // force reload editing-related actions
    emit updateSaveCancelActions();
}

bool KexiTableScrollArea::cursorAtNewRow() const
{
    return m_newRowEditing;
}

void KexiTableScrollArea::lengthExceeded(KexiDataItemInterface *item, bool lengthExceeded)
{
    showLengthExceededMessage(item, lengthExceeded);
}

void KexiTableScrollArea::updateLengthExceededMessage(KexiDataItemInterface *item)
{
    showUpdateForLengthExceededMessage(item);
}

QHeaderView* KexiTableScrollArea::horizontalHeader() const
{
    return d->horizontalHeader;
}

QHeaderView* KexiTableScrollArea::verticalHeader() const
{
    return d->verticalHeader;
}

int KexiTableScrollArea::horizontalHeaderHeight() const
{
    return d->horizontalHeader->height();
}

QWidget* KexiTableScrollArea::navPanelWidget() const
{
    return dynamic_cast<QWidget*>(m_navPanel);
}

bool KexiTableScrollArea::navPanelWidgetVisible() const
{
    return navPanelWidget() && d->appearance.navigatorEnabled;
}

bool KexiTableScrollArea::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::QueryWhatsThis:
    case QEvent::WhatsThis: {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        QString text = whatsThisText(he->pos());
        if (!text.isEmpty()) {
            if (e->type() == QEvent::WhatsThis) {
                QWhatsThis::showText(mapToGlobal(he->pos()), text, this);
            }
            return true;
        }
        return false;
    }
    default:
        break;
    }
    return QScrollArea::event(e);
}

QString KexiTableScrollArea::whatsThisText(const QPoint &pos) const
{
    const int leftMargin = verticalHeaderVisible() ? d->verticalHeader->width() : 0;
    if (KexiUtils::hasParent(d->verticalHeader, childAt(pos))) {
        return i18nc("@info:whatsthis", "Contains a pointer to the currently selected record.");
    }
    else if (KexiUtils::hasParent(navPanelWidget(), childAt(pos))) {
        return i18nc("@info:whatsthis", "Record navigator.");
    }
    const int col = columnAt(pos.x() - leftMargin);
    KexiDB::Field *f = col == -1 ? 0 : field(col);
    if (!f) {
        return QString();
    }
    return i18nc("@info:whatsthis", "Column <resource>%1</resource>.",
                 f->description().isEmpty() ? f->captionOrName() : f->description());
}

void KexiTableScrollArea::selectCellInternal(int previousRow, int previousColumn)
{
    // let the current style draw selection
    d->horizontalHeader->setCurrentIndex(
                d->horizontalHeader->selectionModel()->model()->index(m_curRow, m_curCol));
    d->verticalHeader->setCurrentIndex(
                d->verticalHeader->selectionModel()->model()->index(m_curRow, m_curCol));
    if (previousColumn != m_curCol) {
        d->horizontalHeader->updateSection(previousColumn);
    }
    d->horizontalHeader->updateSection(m_curCol);
    if (previousRow != m_curRow) {
        d->verticalHeader->updateSection(previousRow);
    }
    d->verticalHeader->updateSection(m_curRow);
}

QAbstractItemModel* KexiTableScrollArea::headerModel() const
{
    return d->headerModel;
}

#include "KexiTableScrollArea.moc"
