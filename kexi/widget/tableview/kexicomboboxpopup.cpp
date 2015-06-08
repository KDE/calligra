/* This file is part of the KDE project
   Copyright (C) 2004-2015 Jarosław Staniek <staniek@kde.org>

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
 */

#include "kexicomboboxpopup.h"

#include "KexiDataTableScrollArea.h"
#include "KexiTableScrollArea_p.h"
#include "kexitableedit.h"

#include <kexi_global.h>
#include <KDbConnection>
#include <KDbLookupFieldSchema>
#include <KDbExpression>
#include <KDbCursor>

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QScrollBar>

/*! @internal
 Helper for KexiComboBoxPopup. */
class KexiComboBoxPopup_KexiTableView : public KexiDataTableScrollArea
{
public:
    KexiComboBoxPopup_KexiTableView(QWidget* parent = 0)
            : KexiDataTableScrollArea(parent) {
        init();
    }
    void init() {
        setObjectName("KexiComboBoxPopup_tv");
        setReadOnly(true);
        setLineWidth(0);
        d->moveCursorOnMouseRelease = true;
        KexiTableScrollArea::Appearance a(appearance());
        a.navigatorEnabled = false;
//! @todo add option for backgroundAltering??
        a.backgroundAltering = false;
        a.fullRowSelection = true;
        a.rowHighlightingEnabled = true;
        a.rowMouseOverHighlightingEnabled = true;
        a.persistentSelections = false;
        a.rowMouseOverHighlightingColor = palette().highlight().color();
        a.rowMouseOverHighlightingTextColor = palette().highlightedText().color();
        a.rowHighlightingTextColor = a.rowMouseOverHighlightingTextColor;
        a.horizontalGridEnabled = false;
        a.verticalGridEnabled = false;
        setAppearance(a);
        setInsertingEnabled(false);
        setSortingEnabled(false);
        setVerticalHeaderVisible(false);
        setHorizontalHeaderVisible(false);
        setContextMenuEnabled(false);
        setScrollbarToolTipsEnabled(false);
        installEventFilter(this);
        setBottomMarginInternal(0);
    }
    virtual void setData(KexiDB::TableViewData *data, bool owner = true) {
        KexiTableScrollArea::setData(data, owner);
    }
    bool setData(KexiDB::Cursor *cursor) {
        return KexiDataTableScrollArea::setData(cursor);
    }
};

//========================================

//! @internal
class KexiComboBoxPopupPrivate
{
public:
    KexiComboBoxPopupPrivate()
            : int_f(0)
            , privateQuery(0) {
        max_rows = KexiComboBoxPopup::defaultMaxRows;
    }
    ~KexiComboBoxPopupPrivate() {
        delete int_f;
        delete privateQuery;
    }

    KexiComboBoxPopup_KexiTableView *tv;
    KexiDB::Field *int_f; //!< @todo remove this -temporary
    KexiDB::QuerySchema* privateQuery;
    int max_rows;
    //! Columns that should be kept visible; the others should be hidden.
    //! Used when query is used as the row source type (KexiDB::LookupFieldSchema::RowSource::Query).
    //! We're doing this in this case because it's hard to alter the query to remove columns.
    QList<uint> visibleColumnsToShow;
};

//========================================

const int KexiComboBoxPopup::defaultMaxRows = 8;

KexiComboBoxPopup::KexiComboBoxPopup(QWidget* parent, KexiDB::TableViewColumn &column)
        : QFrame(parent, Qt::Popup)
        , d( new KexiComboBoxPopupPrivate )
{
    init();
    //setup tv data
    setData(&column, 0);
}

KexiComboBoxPopup::KexiComboBoxPopup(QWidget* parent, KexiDB::Field &field)
        : QFrame(parent, Qt::Popup)
        , d( new KexiComboBoxPopupPrivate )
{
    init();
    //setup tv data
    setData(0, &field);
}

KexiComboBoxPopup::~KexiComboBoxPopup()
{
    delete d;
}

void KexiComboBoxPopup::init()
{
    setObjectName("KexiComboBoxPopup");
    setAttribute(Qt::WA_WindowPropagation);
    setAttribute(Qt::WA_X11NetWmWindowTypeCombo);

    QPalette pal(palette());
    pal.setBrush(backgroundRole(), pal.brush(QPalette::Base));
    setPalette(pal);
    setLineWidth(1);
    setFrameStyle(Box | Plain);

    d->tv = new KexiComboBoxPopup_KexiTableView(this);
    d->tv->setFrameShape(QFrame::NoFrame);
    d->tv->setLineWidth(0);
    installEventFilter(this);

    connect(d->tv, SIGNAL(itemReturnPressed(KexiDB::RecordData*,int,int)),
            this, SLOT(slotTVItemAccepted(KexiDB::RecordData*,int,int)));

    connect(d->tv, SIGNAL(itemMouseReleased(KexiDB::RecordData*,int,int)),
            this, SLOT(slotTVItemAccepted(KexiDB::RecordData*,int,int)));

    connect(d->tv, SIGNAL(itemDblClicked(KexiDB::RecordData*,int,int)),
            this, SLOT(slotTVItemAccepted(KexiDB::RecordData*,int,int)));
}

void KexiComboBoxPopup::setData(KexiDB::TableViewColumn *column, KexiDB::Field *field)
{
    d->visibleColumnsToShow.clear();
    if (column && !field)
        field = column->field();
    if (!field) {
        qWarning() << "!field";
        return;
    }

    // case 1: simple related data
    if (column && column->relatedData()) {
        d->tv->setColumnsResizeEnabled(true); //only needed when using single column
        setDataInternal(column->relatedData(), false /*!owner*/);
        return;
    }
    // case 2: lookup field
    KexiDB::LookupFieldSchema *lookupFieldSchema = 0;
    if (field->table())
        lookupFieldSchema = field->table()->lookupFieldSchema(*field);
    delete d->privateQuery;
    d->privateQuery = 0;
    const QList<uint> visibleColumns(lookupFieldSchema ? lookupFieldSchema->visibleColumns() : QList<uint>());
    if (!visibleColumns.isEmpty() && lookupFieldSchema->boundColumn() >= 0) {
        const bool multipleLookupColumnJoined = visibleColumns.count() > 1;
//! @todo support more RowSourceType's, not only table and query
        KexiDB::Cursor *cursor = 0;
        switch (lookupFieldSchema->rowSource().type()) {
        case KexiDB::LookupFieldSchema::RowSource::Table: {
            KexiDB::TableSchema *lookupTable
            = field->table()->connection()->tableSchema(lookupFieldSchema->rowSource().name());
            if (!lookupTable)
//! @todo errmsg
                return;
            if (multipleLookupColumnJoined) {
                /*qDebug() << "--- Orig query: ";
                lookupTable->query()->debug();
                qDebug() << field->table()->connection()->selectStatement(*lookupTable->query());*/
                d->privateQuery = new KexiDB::QuerySchema(*lookupTable->query());
            } else {
                // Create a simple SELECT query that contains only needed columns,
                // that is visible and bound ones. The bound columns are placed on the end.
                // Don't do this if one or more visible or bound columns cannot be found.
                const KexiDB::QueryColumnInfo::Vector fieldsExpanded(lookupTable->query()->fieldsExpanded());
                d->privateQuery = new KexiDB::QuerySchema;
                bool columnsFound = true;
                QList<uint> visibleAndBoundColumns = visibleColumns;
                visibleAndBoundColumns.append(lookupFieldSchema->boundColumn());
                qDebug() << visibleAndBoundColumns;
                foreach (uint index, visibleAndBoundColumns) {
                    KexiDB::QueryColumnInfo *columnInfo = fieldsExpanded.value(index);
                    if (columnInfo && columnInfo->field) {
                        d->privateQuery->addField(columnInfo->field);
                    }
                    else {
                        columnsFound = false;
                        break;
                    }
                }
                if (columnsFound) {
                    // proper data source: bound + visible columns
                    cursor = field->table()->connection()->prepareQuery(*d->privateQuery);
                    /*qDebug() << "--- Composed query:";
                    d->privateQuery->debug();
                    qDebug() << field->table()->connection()->selectStatement(*d->privateQuery);*/
                } else {
                    // for sanity
                    delete d->privateQuery;
                    d->privateQuery = 0;
                    cursor = field->table()->connection()->prepareQuery(*lookupTable);
                }
            }
            break;
        }
        case KexiDB::LookupFieldSchema::RowSource::Query: {
            KexiDB::QuerySchema *lookupQuery
            = field->table()->connection()->querySchema(lookupFieldSchema->rowSource().name());
            if (!lookupQuery)
//! @todo errmsg
                return;
            if (multipleLookupColumnJoined) {
                /*qDebug() << "--- Orig query: ";
                lookupQuery->debug();
                qDebug() << field->table()->connection()->selectStatement(*lookupQuery);*/
                d->privateQuery = new KexiDB::QuerySchema(*lookupQuery);
            } else {
                d->visibleColumnsToShow = visibleColumns;
                qSort(d->visibleColumnsToShow); // because we will depend on a sorted list
                cursor = field->table()->connection()->prepareQuery(*lookupQuery);
            }
            break;
        }
        default:;
        }
        if (multipleLookupColumnJoined && d->privateQuery) {
            // append column computed using multiple columns
            const KexiDB::QueryColumnInfo::Vector fieldsExpanded(d->privateQuery->fieldsExpanded());
            uint fieldsExpandedSize(fieldsExpanded.size());
            KexiDB::BaseExpr *expr = 0;
            QList<uint>::ConstIterator it(visibleColumns.constBegin());
            for (it += visibleColumns.count() - 1; it != visibleColumns.constEnd(); --it) {
                KexiDB::QueryColumnInfo *ci = ((*it) < fieldsExpandedSize) ? fieldsExpanded.at(*it) : 0;
                if (!ci) {
                    qWarning() << *it << ">= fieldsExpandedSize";
                    continue;
                }
                KexiDB::VariableExpr *fieldExpr
                = new KexiDB::VariableExpr(ci->field->table()->name() + "." + ci->field->name());
                fieldExpr->field = ci->field;
                fieldExpr->tablePositionForField = d->privateQuery->tableBoundToColumn(*it);
                if (expr) {
//! @todo " " separator hardcoded...
//! @todo use SQL sub-parser here...
                    KexiDB::ConstExpr *constExpr = new KexiDB::ConstExpr(CHARACTER_STRING_LITERAL, " ");
                    expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, constExpr, CONCATENATION, expr);
                    expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, fieldExpr, CONCATENATION, expr);
                } else
                    expr = fieldExpr;
            }
            expr->debug();
            kDebug() << expr->toString(0);

            KexiDB::Field *f = new KexiDB::Field();
            f->setExpression(expr);
            d->privateQuery->addField(f);
#if 0 //does not work yet
// <remove later>
//! @todo temp: improved display by hiding all columns except the computed one
            const int numColumntoHide = d->privateQuery->fieldsExpanded().count() - 1;
            for (int i = 0; i < numColumntoHide; i++)
                d->privateQuery->setColumnVisible(i, false);
// </remove later>
#endif
//! @todo ...
            qDebug() << "--- Private query: ";
            d->privateQuery->debug();
            cursor = field->table()->connection()->prepareQuery(*d->privateQuery);
        }
        if (!cursor)
//! @todo errmsg
            return;

        if (d->tv->data())
            d->tv->data()->disconnect(this);
        d->tv->setData(cursor);

        connect(d->tv, SIGNAL(dataRefreshed()), this, SLOT(slotDataReloadRequested()));
        updateSize();
        return;
    }

    qWarning() << "no column relatedData \n - moving to setData(KexiDB::Field &)";

    // case 3: enum hints
    d->tv->setColumnsResizeEnabled(true);   //only needed when using single column

//! @todo THIS IS PRIMITIVE: we'd need to employ KexiDB::Reference here!
    d->int_f = new KexiDB::Field(field->name(), KexiDB::Field::Text);
    KexiDB::TableViewData *data = new KexiDB::TableViewData();
    data->addColumn(new KexiDB::TableViewColumn(*d->int_f));
    const QVector<QString> hints(field->enumHints());
    for (int i = 0; i < hints.size(); i++) {
        KexiDB::RecordData *record = data->createItem();
        (*record)[0] = QVariant(hints[i]);
        qDebug() << "added: '" << hints[i] << "'";
        data->append(record);
    }
    setDataInternal(data, true);
    updateSize();
}

void KexiComboBoxPopup::setDataInternal(KexiDB::TableViewData *data, bool owner)
{
    if (d->tv->data())
        d->tv->data()->disconnect(this);
    d->tv->setData(data, owner);
    connect(d->tv, SIGNAL(dataRefreshed()), this, SLOT(slotDataReloadRequested()));

    updateSize();
}

void KexiComboBoxPopup::updateSize(int minWidth)
{
    const int rows = qMin(d->max_rows, d->tv->rowCount());

    KexiTableEdit *te = dynamic_cast<KexiTableEdit*>(parentWidget());
    int width = qMax(d->tv->tableSize().width(),
                           (te ? te->totalSize().width() : (parentWidget() ? parentWidget()->width() : 0/*sanity*/)));
    //qDebug() << "size=" << size();
    resize(qMax(minWidth, width)/*+(d->tv->columnCount()>1?2:0)*/ /*(d->updateSizeCalled?0:1)*/, d->tv->rowHeight() * rows + 2);
    //qDebug() << "size after=" << size();
    if (d->visibleColumnsToShow.isEmpty()) {
        // row source type is not Query
        d->tv->setColumnResizeEnabled(0, true);
        d->tv->setColumnResizeEnabled(d->tv->columnCount() - 1, false);
        d->tv->setColumnWidth(1, 0); //!< @todo A temp. hack to hide the bound column
        d->tv->setColumnWidth(0, d->tv->width() - 1);
    }
    else {
        // row source type is Query
        // Set width to 0 and disable resizing of columns that shouldn't be visible
        const KexiDB::QueryColumnInfo::Vector fieldsExpanded(d->tv->cursor()->query()->fieldsExpanded());
        QList<uint>::ConstIterator visibleColumnsToShowIt = d->visibleColumnsToShow.constBegin();
        for (uint i = 0; i < uint(fieldsExpanded.count()); ++i) {
            bool show = visibleColumnsToShowIt != d->visibleColumnsToShow.constEnd() && i == *visibleColumnsToShowIt;
            d->tv->setColumnResizeEnabled(i, show);
            if (show) {
                if (d->visibleColumnsToShow.count() == 1) {
                    d->tv->setColumnWidth(i, d->tv->width() - 1);
                }
                ++visibleColumnsToShowIt;
            }
            else {
                d->tv->setColumnWidth(i, 0);
            }
            //qDebug() << i << show;
        }
    }
}

KexiTableScrollArea* KexiComboBoxPopup::tableView()
{
    return d->tv;
}

void KexiComboBoxPopup::resize(int w, int h)
{
    //d->tv->horizontalScrollBar()->hide();
    //d->tv->verticalScrollBar()->hide();
    d->tv->move(0, 0);
    d->tv->resize(w + 1, h - 1);
    QFrame::resize(d->tv->size() + QSize(1, 1));
    update();
    updateGeometry();
}

void KexiComboBoxPopup::setMaxRows(int r)
{
    d->max_rows = r;
}

int KexiComboBoxPopup::maxRows() const
{
    return d->max_rows;
}

void KexiComboBoxPopup::slotTVItemAccepted(KexiDB::RecordData *record, int row, int)
{
    hide();
    emit rowAccepted(record, row);
}

bool KexiComboBoxPopup::eventFilter(QObject *o, QEvent *e)
{
#if 0
    if (e->type() == QEvent::Resize) {
        qDebug() << "QResizeEvent"
                 << dynamic_cast<QResizeEvent*>(e)->size()
                 << "old=" << dynamic_cast<QResizeEvent*>(e)->oldSize()
                 << o << qobject_cast<QWidget*>(o)->geometry()
                 << "visible=" << qobject_cast<QWidget*>(o)->isVisible();
    }
#endif
    if (o == this && (e->type() == QEvent::Hide || e->type() == QEvent::FocusOut)) {
        qDebug() << "HIDE!!!";
        emit hidden();
    } else if (e->type() == QEvent::MouseButtonPress) {
        qDebug() << "QEvent::MousePress";
    } else if (o == d->tv) {
        qDebug() << "QEvent::KeyPress TV";
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            const int k = ke->key();
            if ((ke->modifiers() == Qt::NoButton && (k == Qt::Key_Escape || k == Qt::Key_F4))
                    || (ke->modifiers() == Qt::AltModifier && k == Qt::Key_Up)) {
                hide();
                emit cancelled();
                emit hidden();
                return true;
            }
        }
    }
    return QFrame::eventFilter(o, e);
}

void KexiComboBoxPopup::slotDataReloadRequested()
{
    updateSize();
}

