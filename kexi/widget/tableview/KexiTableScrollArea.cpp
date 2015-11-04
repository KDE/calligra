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
#include <KexiIcon.h>
#include <config-kexi.h>

#include <KDbCursor>
#include <KDbValidator>

#include <KColorScheme>
#include <KLocalizedString>

#include <QPainter>
#include <QTimer>
#include <QMenu>
#include <QStyle>
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
#include <QDesktopWidget>
#include <QScrollArea>
#include <QApplication>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QDebug>
#ifdef KEXI_TABLE_PRINT_SUPPORT
#include <QPrinter>
#endif

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
        option.initFrom(widget);
        const int gridHint = widget->style()->styleHint(QStyle::SH_Table_GridLineColor, &option, widget);
        gridColor = static_cast<QRgb>(gridHint);
        emptyAreaColor = KColorScheme(QPalette::Active, KColorScheme::View).background().color()/*QPalette::Base*/;
        alternateBaseColor = widget->palette().color(QPalette::AlternateBase);
        recordHighlightingColor = KexiUtils::blendedColors(QPalette::Highlight, baseColor, 34, 66);
        recordMouseOverHighlightingColor = KexiUtils::blendedColors(QPalette::Highlight, baseColor, 10, 90);
        recordMouseOverAlternateHighlightingColor = KexiUtils::blendedColors(QPalette::Highlight, alternateBaseColor, 10, 90);
        recordHighlightingTextColor = textColor;
        recordMouseOverHighlightingTextColor = textColor;
    }
    backgroundAltering = true;
    recordMouseOverHighlightingEnabled = true;
    recordHighlightingEnabled = true;
    persistentSelections = true;
    navigatorEnabled = true;
    fullRecordSelection = false;
    verticalGridEnabled = true;
    horizontalGridEnabled = !backgroundAltering || baseColor == alternateBaseColor;
}

//-----------------------------------------

//! @todo KEXI3 KexiTableViewCellToolTip
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
  const int row = m_tableView->recordAt( cp.y(), true );
  const int col = m_tableView->columnAt( cp.x() );

  //show tooltip if needed
  if (col>=0 && row>=0) {
    KexiTableEdit *editor = m_tableView->tableEditorWidget( col );
    const bool insertRowSelected = m_tableView->isInsertingEnabled() && row==m_tableView->rowCount();
    KDbRecordData *data = insertRowSelected ? m_tableView->m_insertItem : m_tableView->itemAt( row );
    if (editor && record && (col < (int)record->count())) {
      int w = m_tableView->columnWidth( col );
      int h = m_tableView->rowHeight();
      int x = 0;
      int y_offset = 0;
      int align = SingleLine | AlignVCenter;
      QString txtValue;
      QVariant cellValue;
      KDbTableViewColumn *tvcol = m_tableView->column(col);
      if (!m_tableView->getVisibleLookupValue(cellValue, editor, record, tvcol))
        cellValue = insertRowSelected ? editor->displayedField()->defaultValue() : record->at(col); //display default value if available
      const bool focused = m_tableView->selectedRecord() == record && col == m_tableView->currentColumn();
      editor->setupContents( 0, focused, cellValue, txtValue, align, x, y_offset, w, h );
      QRect realRect(m_tableView->columnPos(col)-m_tableView->horizontalScrollBar()->value(),
        m_tableView->recordPos(row)-m_tableView->verticalScrollBar()->value(), w, h);
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

KexiTableScrollArea::KexiTableScrollArea(KDbTableViewData* data, QWidget* parent)
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

    m_data = new KDbTableViewData(); //to prevent crash because m_data==0
    m_owner = true;                   //-this will be deleted if needed

    viewport()->setFocusPolicy(Qt::WheelFocus);
    setFocusPolicy(Qt::WheelFocus); //<--- !!!!! important (was NoFocus),
                                    // otherwise QApplication::setActiveWindow() won't activate
                                    // this widget when needed!
    viewport()->installEventFilter(this);
    d->scrollAreaWidget->installEventFilter(this);

    d->diagonalGrayPattern = QBrush(d->appearance.gridColor, Qt::BDiagPattern);

    setLineWidth(1);
    horizontalScrollBar()->installEventFilter(this);

    //context menu
    m_contextMenu = new QMenu(this);
    m_contextMenu->setObjectName("m_contextMenu");

//! \todo replace lineedit with table_field icon
//setContextMenuTitle(koIcon("lineedit"), xi18n("Record"));   // the default
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
    d->setSpreadSheetMode(false);

//! @todo KEXI3 d->cellToolTip = new KexiTableViewCellToolTip(this);
}

KexiTableScrollArea::~KexiTableScrollArea()
{
    cancelRecordEditing();

    KDbTableViewData *data = m_data;
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
    //qDebug() << d->horizontalHeader->sizeHint() << d->verticalHeader->sizeHint();
    //qDebug() << d->horizontalHeader->geometry() << d->verticalHeader->geometry();
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

void KexiTableScrollArea::slotRecordsDeleted(const QList<int> &records)
{
    viewport()->repaint();
    updateWidgetContentsSize();
    setCursorPosition(qMax(0, (int)m_curRecord - (int)records.count()), -1, ForceSetCursorPosition);
}

void KexiTableScrollArea::setFont(const QFont &font)
{
    QScrollArea::setFont(font);

#ifdef Q_OS_WIN
    //! @todo KEXI3 WIN32 test this
    d->recordHeight = fontMetrics().lineSpacing() + 4;
#else
    d->recordHeight = fontMetrics().lineSpacing() + 1;
#endif
    if (d->appearance.fullRecordSelection) {
        d->recordHeight -= 1;
    }
    if (d->recordHeight < MINIMUM_ROW_HEIGHT) {
        d->recordHeight = MINIMUM_ROW_HEIGHT;
    }
    KexiDisplayUtils::initDisplayForAutonumberSign(d->autonumberSignDisplayParameters, this);
    KexiDisplayUtils::initDisplayForDefaultValue(d->defaultValueDisplayParameters, this);
    update();
}

void KexiTableScrollArea::updateAllVisibleRecordsBelow(int record)
{
    //get last visible row
//    int r = recordAt(viewport()->height() + verticalScrollBar()->value());
//    if (r == -1) {
//        r = rowCount() + 1 + (isInsertingEnabled() ? 1 : 0);
//    }
    //update all visible rows below
    int leftcol = d->horizontalHeader->visualIndexAt(d->horizontalHeader->offset());
    d->scrollAreaWidget->update(columnPos(leftcol), recordPos(record),
                                viewport()->width(),
                                viewport()->height() - (recordPos(record) - verticalScrollBar()->value()));
}

void KexiTableScrollArea::clearColumnsInternal(bool /*repaint*/)
{
}

void KexiTableScrollArea::slotUpdate()
{
//    qDebug() << m_navPanel;
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
    int prevRowVisibleOffset = recordPos(previousRow) - verticalScrollBar()->value();
    verticalScrollBar()->setValue(recordPos(m_curRecord) - prevRowVisibleOffset);
    d->scrollAreaWidget->update();
    selectCellInternal(m_curRecord, m_curColumn);
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
    qDebug() << w << h;
#endif

    return QSize(w, h);
}

QSize KexiTableScrollArea::minimumSizeHint() const
{
    return QSize(
               leftMargin() + ((columnCount() > 0) ? columnWidth(0) : KEXI_DEFAULT_DATA_COLUMN_WIDTH) + 2*2,
               d->recordHeight*5 / 2 + topMargin() + (navPanelWidgetVisible() ? navPanelWidget()->height() : 0)
           );
}

QRect KexiTableScrollArea::viewportGeometry() const
{
    return viewport()->geometry();
}

//internal
inline void KexiTableScrollArea::paintRow(KDbRecordData *data,
                                    QPainter *pb, int r, int rowp, int cx, int cy,
                                    int colfirst, int collast, int maxwc)
{
    Q_UNUSED(cx);
    Q_UNUSED(cy);
    if (!data)
        return;

    //qDebug() << "r" << r << "rowp" << rowp << "cx" << cx << "cy" << cy
    //    << "colfirst" << colfirst << "collast" << collast << "maxwc" << maxwc;

    // Go through the columns in the row r
    // if we know from where to where, go through [colfirst, collast],
    // else go through all of them
    if (colfirst == -1)
        colfirst = 0;
    if (collast == -1)
        collast = columnCount() - 1;

    int transly = rowp;

    if (d->appearance.recordHighlightingEnabled && r == m_curRecord && !d->appearance.fullRecordSelection) {
        pb->fillRect(0, transly, maxwc, d->recordHeight, d->appearance.recordHighlightingColor);
    } else if (d->appearance.recordMouseOverHighlightingEnabled && r == d->highlightedRecord) {
        if (d->appearance.backgroundAltering && (r % 2 != 0))
            pb->fillRect(0, transly, maxwc, d->recordHeight, d->appearance.recordMouseOverAlternateHighlightingColor);
        else
            pb->fillRect(0, transly, maxwc, d->recordHeight, d->appearance.recordMouseOverHighlightingColor);
    } else {
        if (d->appearance.backgroundAltering && (r % 2 != 0))
            pb->fillRect(0, transly, maxwc, d->recordHeight, d->appearance.alternateBaseColor);
        else
            pb->fillRect(0, transly, maxwc, d->recordHeight, d->appearance.baseColor);
    }

    for (int c = colfirst; c <= collast; c++) {
        // get position and width of column c
        int colp = columnPos(c);
        if (colp == -1)
            continue; //invisible column?
        int colw = columnWidth(c);
//        qDebug() << "c:" << c << "colp:" << colp << "cx:" << cx << "contentsX():" << horizontalScrollBar()->value() << "colw:" << colw;
//(js #2010-01-05) breaks rendering:       int translx = colp - cx + horizontalScrollBar()->value();
        int translx = colp;

        // Translate painter and draw the cell
        const QTransform oldTr( pb->worldTransform() );
        pb->translate(translx, transly);
        paintCell(pb, data, r, c, QRect(colp, rowp, colw, d->recordHeight));
        pb->setWorldTransform(oldTr);
    }

    if (m_dragIndicatorLine >= 0) {
        int y_line = -1;
        if (r == (recordCount() - 1) && m_dragIndicatorLine == recordCount()) {
            y_line = transly + d->recordHeight - 3; //draw at last line
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
    qDebug() << "disable" << d->disableDrawContents << "cx" << cx << "cy" << cy << "cw" << cw << "ch" << ch
             << "contentsRect" << contentsRect() << "geo" << geometry();
#endif
    if (d->disableDrawContents)
        return;

    bool paintOnlyInsertRow = false;
    bool inserting = isInsertingEnabled();
    bool plus1row = false; //true if we should show 'inserting' row at the end
    int colfirst = columnNumberAt(cx);
    int rowfirst = recordNumberAt(cy);
    int collast = columnNumberAt(cx + cw - 1);
    int rowlast = recordNumberAt(cy + ch - 1);
    if (rowfirst == -1 && (cy / d->recordHeight) == recordCount()) {
        // make the insert row paint too when requested
#ifdef KEXITABLEVIEW_DEBUG
        qDebug() << "rowfirst == -1 && (cy / d->rowHeight) == rowCount()";
#endif
        rowfirst = m_data->count();
        rowlast = rowfirst;
        paintOnlyInsertRow = true;
        plus1row = inserting;
    }
/*    qDebug() << "cx" << cx << "cy" << cy << "cw" << cw << "ch" << ch
        << "colfirst" << colfirst << "rowfirst" << rowfirst
        << "collast" << collast << "rowlast" << rowlast;*/

    if (rowlast == -1) {
        rowlast = recordCount() - 1;
        plus1row = inserting;
        if (rowfirst == -1) {
            if (recordNumberAt(cy - d->recordHeight) != -1) {
                //paintOnlyInsertRow = true;
//    qDebug() << "-- paintOnlyInsertRow --";
            }
        }
    }
// qDebug() << "rowfirst="<<rowfirst<<" rowlast="<<rowlast<<" rowCount()="<<rowCount();
// qDebug()<<" plus1row=" << plus1row;

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
// qDebug() << "maxwc:" << maxwc;

    p->fillRect(cx, cy, cw, ch, d->appearance.baseColor);

    int rowp = 0;
    int r = 0;
    if (paintOnlyInsertRow) {
        r = recordCount();
        rowp = recordPos(r); // 'insert' row's position
    } else {
        if (rowfirst >= 0) {
            QList<KDbRecordData*>::ConstIterator it(m_data->constBegin());
            it += rowfirst;//move to 1st row
            rowp = recordPos(rowfirst); // row position
            for (r = rowfirst;r <= rowlast; r++, ++it, rowp += d->recordHeight) {
    //   qDebug() << *it;
                paintRow(*it, p, r, rowp, cx, cy, colfirst, collast, maxwc);
            }
        }
    }

    if (plus1row && rowfirst >= 0) { //additional - 'insert' row
        paintRow(m_insertRecord, p, r, rowp, cx, cy, colfirst, collast, maxwc);
    }
    paintEmptyArea(p, cx, cy, cw, ch);
}

bool KexiTableScrollArea::isDefaultValueDisplayed(KDbRecordData *data, int col, QVariant* value)
{
    const bool cursorAtInsertRowOrEditingNewRow = (data == m_insertRecord || (m_newRecordEditing && m_currentRecord == data));
    KDbTableViewColumn *tvcol;
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

void KexiTableScrollArea::paintCell(QPainter* p, KDbRecordData *data, int record, int column, const QRect &cr, bool print)
{
    Q_UNUSED(print);

    //qDebug() << "col/row:" << col << row << "rect:" << cr;
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

    if (m_editor && record == m_curRecord && column == m_curColumn //don't paint contents of edited cell
            && m_editor->hasFocusableWidget() //..if it's visible
       ) {
        p->restore();
        return;
    }

    KexiTableEdit *edit = tableEditorWidget(column, /*ignoreMissingEditor=*/true);
    int x = edit ? edit->leftMargin() : 0;
    int y_offset = 0;
    int align = Qt::TextSingleLine | Qt::AlignVCenter;
    QString txt; //text to draw

    if (data == m_insertRecord) {
        //qDebug() << "we're at INSERT row...";
    }

    KDbTableViewColumn *tvcol = m_data->column(column);

    QVariant cellValue;
    if (column < (int)data->count()) {
        if (m_currentRecord == data) {
            if (m_editor && record == m_curRecord && column == m_curColumn
                    && !m_editor->hasFocusableWidget()) {
                //we're over editing cell and the editor has no widget
                // - we're displaying internal values, not buffered
                cellValue = m_editor->value();
            } else {
                //we're displaying values from edit buffer, if available
                // this assignment will also get default value if there's no actual value set
                cellValue = *bufferedValueAt(record, column);
            }
        } else {
            cellValue = data->at(column);
        }
    }

    bool defaultValueDisplayed = isDefaultValueDisplayed(data, column);

    if (data == m_insertRecord && cellValue.isNull()) {
        if (!tvcol->field()->isAutoIncrement() && !tvcol->field()->defaultValue().isNull()) {
            //display default value in the "insert record", if available
            //(but not if there is autoincrement flag set)
            cellValue = tvcol->field()->defaultValue();
            defaultValueDisplayed = true;
        }
    }

    const bool columnReadOnly = tvcol->isReadOnly();
    const bool dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
        =    d->appearance.recordHighlightingEnabled && !d->appearance.persistentSelections
          && m_curRecord >= 0 && record != m_curRecord;

    // setup default pen
    QPen defaultPen;
    const bool usesSelectedTextColor = edit && edit->usesSelectedTextColor();
    if (defaultValueDisplayed){
        if (column == m_curColumn && record == m_curRecord && usesSelectedTextColor)
            defaultPen = d->defaultValueDisplayParameters.selectedTextColor;
        else
            defaultPen = d->defaultValueDisplayParameters.textColor;
    } else if (d->appearance.fullRecordSelection
               && (record == d->highlightedRecord || (record == m_curRecord && d->highlightedRecord == -1))
               && usesSelectedTextColor)
    {
        defaultPen = d->appearance.recordHighlightingTextColor; //special case: highlighted record
    }
    else if (d->appearance.fullRecordSelection && record == m_curRecord && usesSelectedTextColor) {
        defaultPen = d->appearance.textColor; //special case for full record selection
    }
    else if (   m_currentRecord == data && column == m_curColumn && !columnReadOnly
             && !dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
             && usesSelectedTextColor)
    {
        defaultPen = palette().color(QPalette::HighlightedText); //selected text
    } else if (   d->appearance.recordHighlightingEnabled && record == m_curRecord
               && !dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
               && usesSelectedTextColor)
    {
        defaultPen = d->appearance.recordHighlightingTextColor;
    } else if (   d->appearance.recordMouseOverHighlightingEnabled && record == d->highlightedRecord
               && !dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
               && usesSelectedTextColor)
    {
        defaultPen = d->appearance.recordMouseOverHighlightingTextColor;
    } else {
        defaultPen = d->appearance.textColor;
    }

    if (edit) {
        if (defaultValueDisplayed)
            p->setFont(d->defaultValueDisplayParameters.font);
        p->setPen(defaultPen);

        //get visible lookup value if available
        getVisibleLookupValue(cellValue, edit, data, tvcol);

/*qDebug() << "edit->setupContents()" << (m_currentRecord == record && col == m_curColumn)
        << cellValue << txt << align << x << y_offset << w << h;*/
        edit->setupContents(p, m_currentRecord == data && column == m_curColumn,
                            cellValue, txt, align, x, y_offset, w, h);
    }
    if (!d->appearance.horizontalGridEnabled)
        y_offset++; //correction because we're not drawing cell borders

    if (d->appearance.fullRecordSelection && d->appearance.fullRecordSelection) {
    }
    if (m_currentRecord == data && (column == m_curColumn || d->appearance.fullRecordSelection)) {
        if (edit && (   (d->appearance.recordHighlightingEnabled && !d->appearance.fullRecordSelection)
                     || (record == m_curRecord && d->highlightedRecord == -1 && d->appearance.fullRecordSelection))
           )
        {
            edit->paintSelectionBackground(p, isEnabled(), txt, align, x, y_offset, w, h,
                                           isEnabled() ? palette().color(QPalette::Highlight) : QColor(200, 200, 200),//d->grayColor,
                                           p->fontMetrics(), columnReadOnly, d->appearance.fullRecordSelection);
        }
    }

    if (!edit) {
        p->fillRect(0, 0, x2, y2, d->diagonalGrayPattern);
    }

// If we are in the focus cell, draw indication
    if (   m_currentRecord == data && column == m_curColumn //js: && !d->recordIndicator)
        && !d->appearance.fullRecordSelection)
    {
//  qDebug() << ">>> CURRENT CELL ("<<m_curColumn<<"," << m_curRecord<<") focus="<<has_focus;
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

    if (   (data == m_insertRecord)
        || (m_newRecordEditing && data == m_currentRecord && cellValue.isNull()))
    {
        // this is an "insert record" or a newly edited record without altered value
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
    qDebug() << cellValue << "x:" << x << "y:" <<  y_offset << "w:" << w << "h:" << h;
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
    //qDebug() << cx << cy << cw << ch;

    // Regions work with shorts, so avoid an overflow and adjust the
    // table size to the visible size
    QSize ts(tableSize());
    //qDebug() << ts;
    /* qDebug() << QString(" (cx:%1 cy:%2 cw:%3 ch:%4)")
          .arg(cx).arg(cy).arg(cw).arg(ch);
      qDebug() << QString(" (w:%3 h:%4)")
          .arg(ts.width()).arg(ts.height());*/

    // Region of the rect we should draw, calculated in viewport
    // coordinates, as a region can't handle bigger coordinates
    contentsToViewport2(cx, cy, cx, cy);
    QRegion reg(QRect(cx, cy, cw, ch));
    //qDebug() << "---cy-- " << verticalScrollBar()->value();
    // Subtract the table from it
    reg = reg.subtract(QRect(QPoint(0, 0), ts - QSize(0, + verticalScrollBar()->value())));

    // And draw the rectangles (transformed inc contents coordinates as needed)
    const QVector<QRect> rects(reg.rects());
    foreach(const QRect& rect, rects) {
        QRect realRect(viewportToContents2(rect.topLeft()), rect.size());
//        qDebug() << QString("- pEA: p->fillRect(x:%1 y:%2 w:%3 h:%4)")
//              .arg(rect.x()).arg(rect.y())
//              .arg(rect.width()).arg(rect.height())
//                   << viewportGeometry();
        p->fillRect(realRect, d->appearance.emptyAreaColor);
    }
}

void KexiTableScrollArea::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
// qDebug();
    m_contentsMousePressEvent_dblClick = true;
    contentsMousePressEvent(e);
    m_contentsMousePressEvent_dblClick = false;

    if (m_currentRecord) {
        if (d->editOnDoubleClick && columnEditable(m_curColumn) && columnType(m_curColumn) != KDbField::Boolean) {
            KexiTableEdit *edit = tableEditorWidget(m_curColumn, /*ignoreMissingEditor=*/true);
            if (edit && edit->handleDoubleClick()) {
                //nothing to do: editors like BLOB editor has custom handling of double clicking
            } else {
                startEditCurrentCell();
                //   createEditor(m_curRecord, m_curColumn, QString());
            }
        }

        emit itemDblClicked(m_currentRecord, m_curRecord, m_curColumn);
    }
}

void KexiTableScrollArea::contentsMousePressEvent(QMouseEvent* e)
{
    setFocus();
    if (m_data->isEmpty() && !isInsertingEnabled()) {
        return;
    }
    //qDebug() << e->pos();
    const int x = e->pos().x();
    if (columnNumberAt(x) == -1) { //outside a column
        return;
    }
    if (!d->moveCursorOnMouseRelease) {
        if (!handleContentsMousePressOrRelease(e, false))
            return;
    }

// qDebug()<< "by now the current items should be set, if not -> error + crash";
    if (e->button() == Qt::RightButton) {
        showContextMenu(e->globalPos());
    } else if (e->button() == Qt::LeftButton) {
        if (columnType(m_curColumn) == KDbField::Boolean && columnEditable(m_curColumn)) {
            //only accept clicking on the [x] rect (copied from KexiBoolTableEdit::setupContents())
            int s = qMax(d->recordHeight - 5, 12);
            s = qMin(d->recordHeight - 3, s);
            s = qMin(columnWidth(m_curColumn) - 3, s); //avoid too large box
            const QRect r(
                columnPos(m_curColumn) + qMax(columnWidth(m_curColumn) / 2 - s / 2, 0),
                recordPos(m_curRecord) + d->recordHeight / 2 - s / 2 /*- 1*/,
                s, s);
            //qDebug() << r;
            if (r.contains(e->pos())) {
//    qDebug() << "e->x:" << e->x() << " e->y:" << e->y() << " " << recordPos(m_curRecord) <<
//     " " << columnPos(m_curColumn);
                boolToggled();
            }
        }
//! @todo
#if 0
        else if (columnType(m_curColumn) == QVariant::StringList && columnEditable(m_curColumn)) {
            createEditor(m_curRecord, m_curColumn);
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

    int col = columnNumberAt(e->pos().x());
    int row = recordNumberAt(e->pos().y());

    if (!m_currentRecord || col == -1 || row == -1 || col != m_curColumn || row != m_curRecord)//outside a current cell
        return;

    emit itemMouseReleased(m_currentRecord, m_curRecord, m_curColumn);
}

bool KexiTableScrollArea::handleContentsMousePressOrRelease(QMouseEvent* e, bool release)
{
    Q_UNUSED(release);
    //qDebug() << "oldRow=" << m_curRecord << " oldCol=" << m_curColumn;

    int newrow, newcol;
    //compute clicked row nr
    const int x = e->pos().x();
    if (isInsertingEnabled()) {
        if (recordNumberAt(e->pos().y()) == -1) {
            newrow = recordNumberAt(e->pos().y() - d->recordHeight);
            if (newrow == -1 && m_data->count() > 0) {
                return false;
            }
            newrow++;
            qDebug() << "Clicked just on 'insert' record.";
        } else {
            // get new focus cell
            newrow = recordNumberAt(e->pos().y());
        }
    } else {
        if (recordNumberAt(e->pos().y()) == -1 || columnNumberAt(x) == -1) {
            return false; //clicked outside a grid
        }
        // get new focus cell
        newrow = recordNumberAt(e->pos().y());
    }
    newcol = columnNumberAt(x);

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
        pos = viewport()->mapToGlobal(QPoint(columnPos(m_curColumn), recordPos(m_curRecord) + d->recordHeight));
    }
    //show own context menu if configured
    selectRecord(m_curRecord);
    m_contextMenu->exec(pos);
}

void KexiTableScrollArea::contentsMouseMoveEvent(QMouseEvent *e)
{
    int row;
    const int col = columnNumberAt(e->x());
    if (col < 0) {
        row = -1;
    } else {
        row = recordNumberAt(e->y(), true /*ignoreEnd*/);
        if (row > (recordCount() - 1 + (isInsertingEnabled() ? 1 : 0)))
            row = -1; //no row to paint
    }
// qDebug() << " row="<<row<< " col="<<col;
    //update row highlight if needed
    if (d->appearance.recordMouseOverHighlightingEnabled) {
        if (row != d->highlightedRecord) {
            const int oldRow = d->highlightedRecord;
            d->highlightedRecord = row;
            updateRecord(oldRow);
            updateRecord(d->highlightedRecord);
            //currently selected (not necessary highlighted) record needs to be repainted
            updateRecord(m_curRecord);
            if (oldRow != d->highlightedRecord) {
                d->verticalHeader->updateSection(oldRow);
            }
            d->verticalHeader->updateSection(d->highlightedRecord);
        }
    }
}

static bool overrideEditorShortcutNeeded(QKeyEvent *e)
{
    //perhaps more to come...
    return (e->key() == Qt::Key_Delete && e->modifiers() == Qt::ControlModifier)
            || (e->key() == Qt::Key_Insert && e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier));
}

bool KexiTableScrollArea::shortCutPressed(QKeyEvent *e, const QString &action_name)
{
    const int k = e->key();
    QAction *action = dynamic_cast<QAction*>(m_sharedActions.value(action_name));
    if (action) {
        if (!action->isEnabled())//this action is disabled - don't process it!
            return false;
//! @todo KEXI3 (action->shortcut().primary() == QKeySequence( e->key()|e->modifiers() )
        if (action->shortcut() == QKeySequence(e->key() | e->modifiers())) {
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
    qDebug() << e;
#endif
    if (!hasData())
        return;
// qDebug() << "key=" <<e->key() << " txt=" <<e->text();

    const int k = e->key();
    const bool ro = isReadOnly();
    QWidget *w = focusWidget();
    if (!w || (w != viewport() && w != this && (!m_editor || !KDbUtils::hasParent(dynamic_cast<QObject*>(m_editor), w)))) {
        //don't process stranger's events
        e->ignore();
        return;
    }
    if (d->skipKeyPress) {
        d->skipKeyPress = false;
        e->ignore();
        return;
    }

    if (m_currentRecord == 0 && (m_data->count() > 0 || isInsertingEnabled())) {
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
            if (columnType(m_curColumn) == KDbField::Boolean) {
                boolToggled();
            } else {
                acceptEditor();
            }
            e->accept();
            return;
        }
    } else if (recordEditing() >= 0) {// if a row is in edit mode, do some special stuff
        if (shortCutPressed(e, "data_save_row")) {
            qDebug() << "shortCutPressed!!!";
            acceptRecordEditing();
            return;
        }
    }

    if (k == Qt::Key_Return || k == Qt::Key_Enter) {
        emit itemReturnPressed(m_currentRecord, m_curRecord, m_curColumn);
    }

    int curRow = m_curRecord;
    int curCol = m_curColumn;

    const bool nobtn = e->modifiers() == Qt::NoModifier;
    bool printable = false;

    //check shared shortcuts
    if (!ro) {
        if (shortCutPressed(e, "edit_delete_row")) {
            deleteCurrentRecord();
            e->accept();
            return;
        } else if (shortCutPressed(e, "edit_delete")) {
            deleteAndStartEditCurrentCell();
            e->accept();
            return;
        } else if (shortCutPressed(e, "edit_insert_empty_row")) {
            insertEmptyRecord();
            e->accept();
            return;
        }
    }

    if (k == Qt::Key_Shift || k == Qt::Key_Alt || k == Qt::Key_Control || k == Qt::Key_Meta) {
        e->ignore();
    } else if (KexiDataAwareObjectInterface::handleKeyPress(
                   e, &curRow, &curCol, d->appearance.fullRecordSelection))
    {
        if (e->isAccepted())
            return;
    } else if (k == Qt::Key_Backspace && nobtn) {
        if (!ro && columnType(curCol) != KDbField::Boolean && columnEditable(curCol)) {
            const CreateEditorFlags flags = DefaultCreateEditorFlags | ReplaceOldValue;
            createEditor(curRow, curCol, QString(), flags);
        }
    } else if (k == Qt::Key_Space) {
        if (nobtn && !ro && columnEditable(curCol)) {
            if (columnType(curCol) == KDbField::Boolean) {
                boolToggled();
            } else
                printable = true; //just space key
        }
    } else if (k == Qt::Key_Escape) {
        if (nobtn && recordEditing() >= 0) {
            cancelRecordEditing();
            return;
        }
    } else {
        //others:
        if ((nobtn && k == Qt::Key_Tab) || k == Qt::Key_Right) {
//! \todo add option for stopping at 1st column for Qt::Key_left
            //tab
            if (acceptEditor()) {
                if (curCol == (columnCount() - 1)) {
                    if (curRow < (recordCount() - 1 + (isInsertingEnabled() ? 1 : 0))) {//skip to next row
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
            KexiTableEdit *edit = tableEditorWidget(m_curColumn);
            if (edit && edit->handleKeyPress(e, m_editor == edit)) {
                //try to handle the event @ editor's level
                e->accept();
                return;
            } else if (nobtn && (k == Qt::Key_Enter || k == Qt::Key_Return || shortCutPressed(e, "edit_edititem"))) {
                //this condition is moved after handleKeyPress() to allow to everride enter key as well
                startEditOrToggleValue();
            } else {
                qDebug() << "default";
                if (e->text().isEmpty() || !e->text()[0].isPrint()) {
                    qDebug() << "NOT PRINTABLE: 0x0" << QString("%1").arg(k, 0, 16);
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
        KDbTableViewColumn *tvcol = m_data->column(curCol);
        if (tvcol->acceptsFirstChar(e->text()[0])) {
            qDebug() << "ev pressed: acceptsFirstChar()==true";
            const CreateEditorFlags flags = DefaultCreateEditorFlags | ReplaceOldValue;
            createEditor(curRow, curCol, e->text(), flags);
        } else {
//! @todo show message "key not allowed eg. on a statusbar"
            qDebug() << "ev pressed: acceptsFirstChar()==false";
        }
    }

    m_verticalScrollBarValueChanged_enabled = false;

    // if focus cell changes, repaint
    setCursorPosition(curRow, curCol, DontEnsureCursorVisibleIfPositionUnchanged);

    m_verticalScrollBarValueChanged_enabled = true;

    e->accept();
}

void KexiTableScrollArea::emitSelected()
{
    if (m_currentRecord)
        emit itemSelected(m_currentRecord);
}

int KexiTableScrollArea::recordsPerPage() const
{
    return viewport()->height() / d->recordHeight;
}

KexiDataItemInterface *KexiTableScrollArea::editor(int col, bool ignoreMissingEditor)
{
    if (!m_data || col < 0 || col >= columnCount())
        return 0;
    KDbTableViewColumn *tvcol = m_data->column(col);

    //find the editor for this column
    KexiTableEdit *editor = d->editors.value(tvcol);
    if (editor)
        return editor;

    //not found: create
    editor = KexiCellEditorFactory::createEditor(*tvcol, d->scrollAreaWidget);
    if (!editor) {//create error!
        if (!ignoreMissingEditor) {
            //! @todo show error???
            cancelRecordEditing();
        }
        return 0;
    }
    editor->hide();
    if (m_data->cursor() && m_data->cursor()->query())
        editor->createInternalEditor(*m_data->cursor()->query());

    connect(editor, SIGNAL(editRequested()), this, SLOT(slotEditRequested()));
    connect(editor, SIGNAL(cancelRequested()), this, SLOT(cancelEditor()));
    connect(editor, SIGNAL(acceptRequested()), this, SLOT(acceptEditor()));

    editor->resize(columnWidth(col), recordHeight());
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
        //qDebug() << "IN";
        QRect rect = cellGeometry(m_curRecord, m_curColumn);
        edit->showFocus(rect, isReadOnly() || m_data->column(col)->isReadOnly());
    }
}

void KexiTableScrollArea::slotEditRequested()
{
    createEditor(m_curRecord, m_curColumn);
}

void KexiTableScrollArea::reloadData()
{
    KexiDataAwareObjectInterface::reloadData();
    d->scrollAreaWidget->update();
}

void KexiTableScrollArea::createEditor(int row, int col, const QString& addText,
                                       CreateEditorFlags flags)
{
    //qDebug() << "addText:" << addText << "removeOld:" << removeOld;
    if (row < 0) {
        qWarning() << "ROW NOT SPECIFIED!" << row;
        return;
    }
    if (isReadOnly()) {
        qDebug() << "DATA IS READ ONLY!";
        return;
    }
    if (m_data->column(col)->isReadOnly()) {//d->pColumnModes.at(d->numCols-1) & ColumnReadOnly)
        qDebug() << "COL IS READ ONLY!";
        return;
    }
    if (recordEditing() >= 0 && row != recordEditing()) {
        if (!acceptRecordEditing()) {
            return;
        }
    }
    const bool startRecordEditing = recordEditing() == -1; //remember if we're starting row edit
    if (startRecordEditing) {
        //we're starting row editing session
        m_data->clearRecordEditBuffer();
        setRecordEditing(row);
        //indicate on the vheader that we are editing:
        if (isInsertingEnabled() && row == recordCount()) {
            //we should know that we are in state "new record editing"
            m_newRecordEditing = true;
            KDbRecordData *insertItem = m_insertRecord;
            beginInsertItem(insertItem, row);
            //'insert' row editing: show another row after that:
            m_data->append(insertItem);
            //new empty 'inserting' item
            m_insertRecord = m_data->createItem();
            endInsertItem(insertItem, row);
            updateWidgetContentsSize();
            //refr. current and next row
            d->scrollAreaWidget->update(columnPos(col), recordPos(row),
                                        viewport()->width(), d->recordHeight*2);
            if (flags & EnsureCellVisible) {
                ensureVisible(columnPos(col), recordPos(row + 1) + d->recordHeight - 1,
                              columnWidth(col), d->recordHeight);
            }
            d->verticalHeader->setOffset(verticalScrollBar()->value());
        }
        d->verticalHeader->updateSection(row);
    }

    KexiTableEdit *editorWidget = tableEditorWidget(col);
    m_editor = editorWidget;
    if (!editorWidget)
        return;

    m_editor->setValue(*bufferedValueAt(row, col, !(flags & ReplaceOldValue)/*useDefaultValueIfPossible*/),
                       addText, flags & ReplaceOldValue);
    if (m_editor->hasFocusableWidget()) {
        editorWidget->move(columnPos(col), recordPos(row));
        editorWidget->resize(columnWidth(col), recordHeight());
        editorWidget->show();

        m_editor->setFocus();
    }

    if (startRecordEditing) {
        m_navPanel->showEditingIndicator(true); //this will allow to enable 'next' btn
        //emit recordEditingStarted(row);
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

    if ((viewport()->height() - e->size().height()) <= d->recordHeight) {
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
    if (d->ensureCellVisibleOnShow != QPoint(-17, -17)) { // because (-1, -1) means "current cell"
        ensureCellVisible(d->ensureCellVisibleOnShow.y(), d->ensureCellVisibleOnShow.x());
        d->ensureCellVisibleOnShow = QPoint(-17, -17); //reset the flag
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
    if (m_dropsAtRecordEnabled) {
        QPoint p = e->pos();
        int row = recordNumberAt(p.y());
        if ((p.y() % d->recordHeight) > (d->recordHeight*2 / 3)) {
            row++;
        }
        KDbRecordData *data = m_data->at(row);
        emit dragOverRecord(data, row, e);
        if (e->isAccepted()) {
            if (m_dragIndicatorLine >= 0 && m_dragIndicatorLine != row) {
                //erase old indicator
                updateRecord(m_dragIndicatorLine);
            }
            if (m_dragIndicatorLine != row) {
                m_dragIndicatorLine = row;
                updateRecord(m_dragIndicatorLine);
            }
        } else {
            if (m_dragIndicatorLine >= 0) {
                //erase old indicator
                updateRecord(m_dragIndicatorLine);
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
    if (m_dropsAtRecordEnabled) {
        //we're no longer dragging over the table
        if (m_dragIndicatorLine >= 0) {
            int row2update = m_dragIndicatorLine;
            m_dragIndicatorLine = -1;
            updateRecord(row2update);
        }
        QPoint p = e->pos();
        int row = recordNumberAt(p.y());
        if ((p.y() % d->recordHeight) > (d->recordHeight*2 / 3)) {
            row++;
        }
        KDbRecordData *data = m_data->at(row);
        KDbRecordData *newData = 0;
        emit droppedAtRecord(data, row, e, newData );
        if (newData ) {
            const int realRow = (row == m_curRecord ? -1 : row);
            insertItem(newData , realRow);
            setCursorPosition(row, 0);
        }
    }
}

void KexiTableScrollArea::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_UNUSED(e);
    if (!hasData())
        return;
    if (m_dropsAtRecordEnabled) {
        //we're no longer dragging over the table
        if (m_dragIndicatorLine >= 0) {
            int row2update = m_dragIndicatorLine;
            m_dragIndicatorLine = -1;
            updateRecord(row2update);
        }
    }
}

void KexiTableScrollArea::updateCell(int record, int column)
{
//    qDebug() << record << column;
    d->scrollAreaWidget->update(cellGeometry(record, column));
}

void KexiTableScrollArea::updateCurrentCell()
{
    updateCell(m_curRecord, m_curColumn);
}

void KexiTableScrollArea::updateRecord(int record)
{
//    qDebug()<<record << horizontalScrollBar()->value() << recordPos(row) << viewport()->width() << rowHeight();
    if (record < 0 || record >= (recordCount() + 2/* sometimes we want to refresh the row after last*/))
        return;
    //qDebug() << horizontalScrollBar()->value() << " " << verticalScrollBar()->value();
    //qDebug() << QRect( columnPos( leftcol ), recordPos(row), viewport()->width(), rowHeight() );
    d->scrollAreaWidget->update(horizontalScrollBar()->value(), recordPos(record),
                                viewport()->width(), recordHeight());
}

void KexiTableScrollArea::slotColumnWidthChanged(int column, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);
    updateScrollAreaWidgetSize();
    d->scrollAreaWidget->update(d->horizontalHeader->offset() + columnPos(column), d->verticalHeader->offset(),
                                viewport()->width() - columnPos(column), viewport()->height());
    //qDebug() << QRect(columnPos(column), 0, viewport()->width() - columnPos(column), viewport()->height());

    QWidget *editorWidget = dynamic_cast<QWidget*>(m_editor);
    if (editorWidget && editorWidget->isVisible()) {
        editorWidget->move(columnPos(m_curColumn), recordPos(m_curRecord));
        editorWidget->resize(columnWidth(m_curColumn), recordHeight());
    }
    updateGeometries();
    editorShowFocus(m_curRecord, m_curColumn);
    if (editorWidget && editorWidget->isVisible()) {
        m_editor->setFocus();
    }
}

void KexiTableScrollArea::slotSectionHandleDoubleClicked(int section)
{
    adjustColumnWidthToContents(section);
    slotColumnWidthChanged(0, 0, 0); //to update contents and redraw
    ensureColumnVisible(section);
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
    //qDebug() << d->horizontalHeader->height();
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
    d->verticalHeader->move(frameLeftMargin,
                            d->horizontalHeader->geometry().bottom() + 1);
}

int KexiTableScrollArea::columnWidth(int col) const
{
    if (!hasData())
        return 0;
    int vcID = m_data->visibleColumnIndex(col);
    //qDebug() << vcID << d->horizontalHeader->sectionSize(vcID);
    return (vcID == -1) ? 0 : d->horizontalHeader->sectionSize(vcID);
}

int KexiTableScrollArea::recordHeight() const
{
    return d->recordHeight;
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

int KexiTableScrollArea::recordPos(int record) const
{
    return d->recordHeight*record;
}

int KexiTableScrollArea::columnNumberAt(int pos) const
{
    if (!hasData())
        return -1;
    const int realPos = pos - d->horizontalHeader->offset();
    const int c = d->horizontalHeader->logicalIndexAt(realPos);
    if (c < 0)
        return c;
    return m_data->globalIndexOfVisibleColumn(c);
}

int KexiTableScrollArea::recordNumberAt(int pos, bool ignoreEnd) const
{
    if (!hasData())
        return -1;
    pos /= d->recordHeight;
    if (pos < 0)
        return 0;
    if ((pos >= (int)m_data->count()) && !ignoreEnd)
        return -1;
    return pos;
}

QRect KexiTableScrollArea::cellGeometry(int record, int column) const
{
    return QRect(columnPos(column), recordPos(record),
                 columnWidth(column), recordHeight());
}

//#define KEXITABLEVIEW_COMBO_DEBUG

QSize KexiTableScrollArea::tableSize() const
{
#ifdef KEXITABLEVIEW_COMBO_DEBUG
    if (objectName() == "KexiComboBoxPopup_tv") {
        qDebug() << "rowCount" << rowCount() << "\nisInsertingEnabled" << isInsertingEnabled()
                 << "columnCount" << columnCount();
    }
#endif
    if ((recordCount() + (isInsertingEnabled() ? 1 : 0)) > 0 && columnCount() > 0) {
        /*  qDebug() << columnPos( columnCount() - 1 ) + columnWidth( columnCount() - 1 )
              << ", " << recordPos( rowCount()-1+(isInsertingEnabled()?1:0)) + d->rowHeight */
//  qDebug() << m_navPanel->isVisible() <<" "<<m_navPanel->height()<<" "
//           << horizontalScrollBar()->sizeHint().height()<<" "<<recordPos( rowCount()-1+(isInsertingEnabled()?1:0));
        QSize s(
            columnPos(columnCount() - 1) + columnWidth(columnCount() - 1),
            recordPos(recordCount() - 1 + (isInsertingEnabled() ? 1 : 0)) + d->recordHeight
            + d->internal_bottomMargin
        );
#ifdef KEXITABLEVIEW_COMBO_DEBUG
        if (objectName() == "KexiComboBoxPopup_tv") {
            qDebug() << "size" << s
                     << "\ncolumnPos(columnCount()-1)" << columnPos(columnCount() - 1)
                     << "\ncolumnWidth(columnCount()-1)" << columnWidth(columnCount() - 1)
                     << "\nrecordPos(rowCount()-1+(isInsertingEnabled()?1:0))" << recordPos(rowCount()-1+(isInsertingEnabled()?1:0))
                     << "\nd->rowHeight" << d->rowHeight
                     << "\nd->internal_bottomMargin" << d->internal_bottomMargin;
        }
#endif

//  qDebug() << rowCount()-1 <<" "<< (isInsertingEnabled()?1:0) <<" "<< rowEditing() << " " <<  s;
#ifdef KEXITABLEVIEW_DEBUG
qDebug() << s << "cw(last):" << columnWidth(columnCount() - 1);
#endif
        return s;
    }
    return QSize(0, 0);
}

void KexiTableScrollArea::ensureCellVisible(int record, int column)
{
    if (!isVisible()) {
        //the table is invisible: we can't ensure visibility now
        d->ensureCellVisibleOnShow = QPoint(record, column);
        return;
    }
    if (column == -1) {
        column = m_curColumn;
    }
    if (record == -1) {
        record = m_curRecord;
    }
    if (column < 0 || record < 0) {
        return;
    }

    //quite clever: ensure the cell is visible:
    QRect r(columnPos(column) - 1, recordPos(record) + (d->appearance.fullRecordSelection ? 1 : 0) - 1,
            columnWidth(column)  + 2, recordHeight() + 2);

    if (navPanelWidgetVisible() && horizontalScrollBar()->isHidden()) {
        //a hack: for visible navigator: increase height of the visible rect 'r'
        r.setBottom(r.bottom() + navPanelWidget()->height());
    }

    QSize tableSize(this->tableSize());
    const int bottomBorder = r.bottom() + (isInsertingEnabled() ? recordHeight() : 0);
    if (!spreadSheetMode() && (tableSize.height() - bottomBorder) < recordHeight()) {
        // ensure the very bottom of scroll area is displayed to help the user see what's there
        r.moveTop(tableSize.height() - r.height() + 1);
    }
    QPoint pcenter = r.center();
#ifdef KEXITABLEVIEW_DEBUG
    qDebug() << pcenter.x() << pcenter.y() << (r.width() / 2)  << (r.height() / 2);
#endif
    ensureVisible(pcenter.x(), pcenter.y(), r.width() / 2, r.height() / 2);
}

void KexiTableScrollArea::ensureColumnVisible(int col)
{
    if (!isVisible()) {
        return;
    }

    //quite clever: ensure the cell is visible:
    QRect r(columnPos(col == -1 ? m_curColumn : col) - 1, d->verticalHeader->offset(),
            columnWidth(col == -1 ? m_curColumn : col)  + 2, 0);

    QPoint pcenter = r.center();
#ifdef KEXITABLEVIEW_DEBUG
    qDebug() << pcenter.x() << pcenter.y() << (r.width() / 2)  << (r.height() / 2);
#endif
    ensureVisible(pcenter.x(), pcenter.y(), r.width() / 2, r.height() / 2);
}

void KexiTableScrollArea::deleteCurrentRecord()
{
    KexiDataAwareObjectInterface::deleteCurrentRecord();
    ensureCellVisible(m_curRecord, -1);
}

KDbRecordData* KexiTableScrollArea::insertEmptyRecord(int pos)
{
    const int previousRow = m_curRecord;
    KDbRecordData* data = KexiDataAwareObjectInterface::insertEmptyRecord(pos);
    // update header selection
    d->verticalHeader->setCurrentIndex(
                d->verticalHeader->selectionModel()->model()->index(m_curRecord, m_curColumn));
    d->verticalHeader->updateSection(previousRow);
    d->verticalHeader->updateSection(m_curRecord);
    return data;
}

void KexiTableScrollArea::updateAfterCancelRecordEditing()
{
    KexiDataAwareObjectInterface::updateAfterCancelRecordEditing();
    m_navPanel->showEditingIndicator(false);
}

void KexiTableScrollArea::updateAfterAcceptRecordEditing()
{
    KexiDataAwareObjectInterface::updateAfterAcceptRecordEditing();
    m_navPanel->showEditingIndicator(false);
}

bool KexiTableScrollArea::getVisibleLookupValue(QVariant& cellValue, KexiTableEdit *edit,
        KDbRecordData *data, KDbTableViewColumn *tvcol) const
{
    if (edit->columnInfo() && edit->columnInfo()->indexForVisibleLookupValue() != -1
            && edit->columnInfo()->indexForVisibleLookupValue() < (int)data->count()) {
        const QVariant *visibleFieldValue = 0;
        if (m_currentRecord == data && m_data->recordEditBuffer()) {
            visibleFieldValue = m_data->recordEditBuffer()->at(
                                    tvcol->visibleLookupColumnInfo(), false/*!useDefaultValueIfPossible*/);
        }

        if (visibleFieldValue)
            //(use bufferedValueAt() - try to get buffered visible value for lookup field)
            cellValue = *visibleFieldValue;
        else
            cellValue /*txt*/ = data->at(edit->columnInfo()->indexForVisibleLookupValue());
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

void KexiTableScrollArea::slotRecordRepaintRequested(KDbRecordData* data)
{
    updateRecord(m_data->indexOf(data));
}

#ifdef KEXI_TABLE_PRINT_SUPPORT
void
KexiTableScrollArea::print(QPrinter & /*printer*/ , QPrintDialog & /*printDialog*/)
{
    int leftMargin = printer.margins().width() + 2 + d->rowHeight;
    int topMargin = printer.margins().height() + 2;
// int bottomMargin = topMargin + ( printer.realPageSize()->height() * printer.resolution() + 36 ) / 72;
    int bottomMargin = 0;
    qDebug() << "bottom:" << bottomMargin;

    QPainter p(&printer);

    KDbRecordData *i;
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
            qDebug() << "row=" << row << "y=" << yOffset;
            int xOffset = leftMargin;
            for (int col = 0; col < columnCount(); col++) {
                qDebug() << "col=" << col << "x=" << xOffset;
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
}
#endif

KDbField* KexiTableScrollArea::field(int column) const
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

    QList<KDbRecordData*>::ConstIterator it(m_data->constBegin());
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
    const QFontMetrics fm(fontMetrics());
    if (ed) {
        for (it = m_data->constBegin(); it != m_data->constEnd(); ++it) {
            const int wfw = ed->widthForValue((*it)->at(indexOfVisibleColumn), fm);
            maxw = qMax(maxw, wfw);
        }
        const bool focused = currentColumn() == column;
        maxw += (fm.width("  ") + ed->leftMargin() + ed->rightMargin(focused) + 2);
    }
    if (maxw < KEXITV_MINIMUM_COLUMN_WIDTH)
        maxw = KEXITV_MINIMUM_COLUMN_WIDTH; //not too small
    //qDebug() << "setColumnWidth(column=" << column
    //    << ", indexOfVisibleColumn=" << indexOfVisibleColumn << ", width=" << maxw << " )";
    setColumnWidth(column/* not indexOfVisibleColumn*/, maxw);
}

void KexiTableScrollArea::setColumnWidth(int column, int width)
{
    if (columnCount() <= column || column < 0)
        return;
    d->horizontalHeader->resizeSection(column, width);
    editorShowFocus(m_curRecord, m_curColumn);
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
    editorShowFocus(m_curRecord, m_curColumn);
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

bool KexiTableScrollArea::stretchLastColumn() const
{
    return d->horizontalHeader->stretchLastSection();
}

void KexiTableScrollArea::setStretchLastColumn(bool set)
{
    if (columnCount() > 0) {
        setColumnResizeEnabled(columnCount() - 1, !set);
    }
    d->horizontalHeader->setStretchLastSection(set);
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
    qDebug() << d->viewportMargins;
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
// qDebug();
    d->pUpdateTimer->start(20);
}

void KexiTableScrollArea::setHBarGeometry(QScrollBar & hbar, int x, int y, int w, int h)
{
#ifdef KEXITABLEVIEW_DEBUG
    /*todo*/
    qDebug();
#endif
    if (d->appearance.navigatorEnabled) {
        m_navPanel->setHBarGeometry(hbar, x, y, w, h);
    } else {
        hbar.setGeometry(x , y, w, h);
    }
}

void KexiTableScrollArea::setSpreadSheetMode(bool set)
{
    KexiDataAwareObjectInterface::setSpreadSheetMode(set);
    d->setSpreadSheetMode(set);
}

int KexiTableScrollArea::validRowNumber(const QString& text)
{
    bool ok = true;
    int r = text.toInt(&ok);
    if (!ok || r < 1)
        r = 1;
    else if (r > (recordCount() + (isInsertingEnabled() ? 1 : 0)))
        r = recordCount() + (isInsertingEnabled() ? 1 : 0);
    return r -1;
}

void KexiTableScrollArea::moveToRecordRequested(int record)
{
    setFocus();
    selectRecord(record);
}

void KexiTableScrollArea::moveToLastRecordRequested()
{
    setFocus();
    selectLastRecord();
}

void KexiTableScrollArea::moveToPreviousRecordRequested()
{
    setFocus();
    selectPreviousRecord();
}

void KexiTableScrollArea::moveToNextRecordRequested()
{
    setFocus();
    selectNextRecord();
}

void KexiTableScrollArea::moveToFirstRecordRequested()
{
    setFocus();
    selectFirstRecord();
}

void KexiTableScrollArea::copySelection()
{
    if (m_currentRecord && m_curColumn != -1) {
        KexiTableEdit *edit = tableEditorWidget(m_curColumn);
        QVariant defaultValue;
        const bool defaultValueDisplayed = isDefaultValueDisplayed(m_currentRecord, m_curColumn, &defaultValue);
        if (edit) {
            QVariant visibleValue;
            getVisibleLookupValue(visibleValue, edit, m_currentRecord, m_data->column(m_curColumn));
            edit->handleCopyAction(
                defaultValueDisplayed ? defaultValue : m_currentRecord->at(m_curColumn),
                visibleValue);
        }
    }
}

void KexiTableScrollArea::cutSelection()
{
    //try to handle @ editor's level
    KexiTableEdit *edit = tableEditorWidget(m_curColumn);
    if (edit)
        edit->handleAction("edit_cut");
}

void KexiTableScrollArea::paste()
{
    //try to handle @ editor's level
    KexiTableEdit *edit = tableEditorWidget(m_curColumn);
    if (edit)
        edit->handleAction("edit_paste");
}

bool KexiTableScrollArea::eventFilter(QObject *o, QEvent *e)
{
    //don't allow to stole key my events by others:
// qDebug() << "spontaneous " << e->spontaneous() << " type=" << e->type();
#ifdef KEXITABLEVIEW_DEBUG
    if (e->type() != QEvent::Paint
        && e->type() != QEvent::Leave
        && e->type() != QEvent::MouseMove
        && e->type() != QEvent::HoverMove
        && e->type() != QEvent::HoverEnter
        && e->type() != QEvent::HoverLeave)
    {
        qDebug() << e << o;
    }
    if (e->type() == QEvent::Paint) {
        qDebug() << "PAINT!" << static_cast<QPaintEvent*>(e) << static_cast<QPaintEvent*>(e)->rect();
    }
#endif
    if (e->type() == QEvent::KeyPress) {
        if (e->spontaneous()) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            const int k = ke->key();
            int mods = ke->modifiers();
            //cell editor's events:
            //try to handle the event @ editor's level
            KexiTableEdit *edit = tableEditorWidget(m_curColumn);
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
            && d->appearance.recordMouseOverHighlightingEnabled
            && d->appearance.persistentSelections)
        {
            if (d->highlightedRecord != -1) {
                int oldRow = d->highlightedRecord;
                d->highlightedRecord = -1;
                updateRecord(oldRow);
                d->verticalHeader->updateSection(oldRow);
                const bool dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted
                    = d->appearance.recordHighlightingEnabled && !d->appearance.persistentSelections;
                if (oldRow != m_curRecord && m_curRecord >= 0) {
                    if (!dontPaintNonpersistentSelectionBecauseDifferentRowHasBeenHighlighted) {
                        //no highlight for now: show selection again
                        updateRecord(m_curRecord);
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
    updateWidgetContentsSize();
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
    if (a.fullRecordSelection) {
        d->recordHeight -= 1;
    } else {
        d->recordHeight += 1;
    }
    if (d->verticalHeader) {
        d->verticalHeader->setDefaultSectionSize(d->recordHeight);
    }
    if (a.recordHighlightingEnabled) {
        m_updateEntireRecordWhenMovingToOtherRecord = true;
    }
    navPanelWidget()->setVisible(a.navigatorEnabled);
    setHorizontalScrollBarPolicy(a.navigatorEnabled ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAsNeeded);
    d->highlightedRecord = -1;
//! @todo is setMouseTracking useful for other purposes?
    viewport()->setMouseTracking(a.recordMouseOverHighlightingEnabled);
    d->appearance = a;
    updateViewportMargins();
}

int KexiTableScrollArea::highlightedRecordNumber() const
{
    return d->highlightedRecord;
}

void KexiTableScrollArea::setHighlightedRecordNumber(int record)
{
    if (record != -1) {
        record = qMin(recordCount() - 1 + (isInsertingEnabled() ? 1 : 0), record);
        record = qMax(0, record);
    }
    const int previouslyHighlightedRow = d->highlightedRecord;
    if (previouslyHighlightedRow == record) {
        if (previouslyHighlightedRow != -1)
            updateRecord(previouslyHighlightedRow);
        return;
    }
    d->highlightedRecord = record;
    if (d->highlightedRecord != -1)
        updateRecord(d->highlightedRecord);

    if (previouslyHighlightedRow != -1)
        updateRecord(previouslyHighlightedRow);

    if (m_curRecord >= 0 && (previouslyHighlightedRow == -1 || previouslyHighlightedRow == m_curRecord)
            && d->highlightedRecord != m_curRecord && !d->appearance.persistentSelections)
    {
        //currently selected row needs to be repainted
        updateRecord(m_curRecord);
    }
}

KDbRecordData *KexiTableScrollArea::highlightedRecord() const
{
    return d->highlightedRecord == -1 ? 0 : m_data->at(d->highlightedRecord);
}

QScrollBar* KexiTableScrollArea::verticalScrollBar() const
{
    return QScrollArea::verticalScrollBar();
}

int KexiTableScrollArea::lastVisibleRecord() const
{
    return recordNumberAt(verticalScrollBar()->value());
}

void KexiTableScrollArea::valueChanged(KexiDataItemInterface* item)
{
#ifdef KEXITABLEVIEW_DEBUG
    qDebug() << item->field()->name() << item->value();
#else
    Q_UNUSED(item);
#endif
    // force reload editing-related actions
    emit updateSaveCancelActions();
}

bool KexiTableScrollArea::cursorAtNewRecord() const
{
    return m_newRecordEditing;
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
    if (KDbUtils::hasParent(d->verticalHeader, childAt(pos))) {
        return xi18nc("@info:whatsthis", "Contains a pointer to the currently selected record.");
    }
    else if (KDbUtils::hasParent(navPanelWidget(), childAt(pos))) {
        return xi18nc("@info:whatsthis", "Record navigator.");
    }
    const int col = columnNumberAt(pos.x() - leftMargin);
    KDbField *f = col == -1 ? 0 : field(col);
    if (!f) {
        return QString();
    }
    return xi18nc("@info:whatsthis", "Column <resource>%1</resource>.",
                 f->description().isEmpty() ? f->captionOrName() : f->description());
}

void KexiTableScrollArea::selectCellInternal(int previousRow, int previousColumn)
{
    // let the current style draw selection
    d->horizontalHeader->setCurrentIndex(
                d->horizontalHeader->selectionModel()->model()->index(m_curRecord, m_curColumn));
    d->verticalHeader->setCurrentIndex(
                d->verticalHeader->selectionModel()->model()->index(m_curRecord, m_curColumn));
    if (previousColumn != m_curColumn) {
        d->horizontalHeader->updateSection(previousColumn);
    }
    d->horizontalHeader->updateSection(m_curColumn);
    if (previousRow != m_curRecord) {
        d->verticalHeader->updateSection(previousRow);
    }
    d->verticalHeader->updateSection(m_curRecord);
}

QAbstractItemModel* KexiTableScrollArea::headerModel() const
{
    return d->headerModel;
}

void KexiTableScrollArea::beginInsertItem(KDbRecordData *data, int pos)
{
    Q_UNUSED(data);
    KexiTableScrollAreaHeaderModel* headerModel
            = static_cast<KexiTableScrollAreaHeaderModel*>(d->headerModel);
    headerModel->beginInsertRows(headerModel->index(pos, 0).parent(), pos, pos);
}

void KexiTableScrollArea::endInsertItem(KDbRecordData *data, int pos)
{
    Q_UNUSED(data);
    Q_UNUSED(pos);
    KexiTableScrollAreaHeaderModel* headerModel
            = static_cast<KexiTableScrollAreaHeaderModel*>(d->headerModel);
    headerModel->endInsertRows();
}

void KexiTableScrollArea::beginRemoveItem(KDbRecordData *data, int pos)
{
    Q_UNUSED(data);
    KexiTableScrollAreaHeaderModel* headerModel
            = static_cast<KexiTableScrollAreaHeaderModel*>(d->headerModel);
    headerModel->beginRemoveRows(headerModel->index(pos, 0).parent(), pos, pos);
}

void KexiTableScrollArea::endRemoveItem(int pos)
{
    Q_UNUSED(pos);
    KexiTableScrollAreaHeaderModel* headerModel
            = static_cast<KexiTableScrollAreaHeaderModel*>(d->headerModel);
    headerModel->endRemoveRows();
    updateWidgetContentsSize();
}

int KexiTableScrollArea::recordCount() const
{
    return KexiDataAwareObjectInterface::recordCount();
}

int KexiTableScrollArea::currentRecord() const
{
    return KexiDataAwareObjectInterface::currentRecord();
}
