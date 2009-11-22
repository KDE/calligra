/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#include "kexidatatableview.h"
#include "kexitableview_p.h"
#include "kexitableedit.h"

#include <kexi_global.h>
#include <kexidb/connection.h>
#include <kexidb/lookupfieldschema.h>
#include <kexidb/expression.h>
#include <kexidb/parser/sqlparser.h>

#include <kdebug.h>

#include <qlayout.h>
#include <qevent.h>
#include <QKeyEvent>

/*! @internal
 Helper for KexiComboBoxPopup. */
class KexiComboBoxPopup_KexiTableView : public KexiDataTableView
{
public:
    KexiComboBoxPopup_KexiTableView(QWidget* parent = 0)
            : KexiDataTableView(parent) {
        init();
    }
    void init() {
        setObjectName("KexiComboBoxPopup_tv");
        setReadOnly(true);
        setLineWidth(0);
        d->moveCursorOnMouseRelease = true;
        KexiTableView::Appearance a(appearance());
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
        a.gridEnabled = false;
        setAppearance(a);
        setInsertingEnabled(false);
        setSortingEnabled(false);
        setVerticalHeaderVisible(false);
        setHorizontalHeaderVisible(false);
        setContextMenuEnabled(false);
        setScrollbarToolTipsEnabled(false);
        installEventFilter(this);
        setBottomMarginInternal(- horizontalScrollBar()->sizeHint().height());
    }
    virtual void setData(KexiTableViewData *data, bool owner = true) {
        KexiTableView::setData(data, owner);
    }
    bool setData(KexiDB::Cursor *cursor) {
        return KexiDataTableView::setData(cursor);
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
    KexiDB::Field *int_f; //TODO: remove this -temporary
    KexiDB::QuerySchema* privateQuery;
    int max_rows;
};

//========================================

const int KexiComboBoxPopup::defaultMaxRows = 8;

KexiComboBoxPopup::KexiComboBoxPopup(QWidget* parent, KexiTableViewColumn &column)
        : QFrame(parent, Qt::Popup)
        , d( new KexiComboBoxPopupPrivate )
{
    init();
    //setup tv data
    setData(&column, 0);
}

KexiComboBoxPopup::KexiComboBoxPopup(QWidget* parent, KexiDB::Field &field)
        : QFrame(parent, Qt::WType_Popup)
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
//    setPaletteBackgroundColor(palette().color(QPalette::Active, QColorGroup::Base));
    QPalette pal(palette());
    pal.setBrush(backgroundRole(), pal.brush(QPalette::Base));
    setPalette(pal);
    setLineWidth(1);
    setFrameStyle(Box | Plain);

    d->tv = new KexiComboBoxPopup_KexiTableView(this);
    d->tv->setFrameShape(QFrame::NoFrame);
    d->tv->setLineWidth(0);
    installEventFilter(this);

    connect(d->tv, SIGNAL(itemReturnPressed(KexiDB::RecordData*, int, int)),
            this, SLOT(slotTVItemAccepted(KexiDB::RecordData*, int, int)));

    connect(d->tv, SIGNAL(itemMouseReleased(KexiDB::RecordData*, int, int)),
            this, SLOT(slotTVItemAccepted(KexiDB::RecordData*, int, int)));

    connect(d->tv, SIGNAL(itemDblClicked(KexiDB::RecordData*, int, int)),
            this, SLOT(slotTVItemAccepted(KexiDB::RecordData*, int, int)));
}

void KexiComboBoxPopup::setData(KexiTableViewColumn *column, KexiDB::Field *field)
{
    if (column && !field)
        field = column->field();
    if (!field) {
        kWarning() << "!field";
        return;
    }

    // case 1: simple related data
    if (column && column->relatedData()) {
        d->tv->setColumnStretchEnabled(true, -1);   //only needed when using single column
        setDataInternal(column->relatedData(), false /*!owner*/);
        return;
    }
    // case 2: lookup field
    KexiDB::LookupFieldSchema *lookupFieldSchema = 0;
    if (field->table())
        lookupFieldSchema = field->table()->lookupFieldSchema(*field);
    delete d->privateQuery;
    d->privateQuery = 0;
    if (lookupFieldSchema) {
        const QList<uint> visibleColumns(lookupFieldSchema->visibleColumns());
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
                kDebug() << "--- Orig query: ";
                lookupTable->query()->debug();
                d->privateQuery = new KexiDB::QuerySchema(*lookupTable->query());
            } else {
                cursor = field->table()->connection()->prepareQuery(*lookupTable);
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
                kDebug() << "--- Orig query: ";
                lookupQuery->debug();
                d->privateQuery = new KexiDB::QuerySchema(*lookupQuery);
            } else {
                cursor = field->table()->connection()->prepareQuery(*lookupQuery);
            }
            break;
        }
        default:;
        }
        if (d->privateQuery) {
            // append column computed using multiple columns
            const KexiDB::QueryColumnInfo::Vector fieldsExpanded(d->privateQuery->fieldsExpanded());
            uint fieldsExpandedSize(fieldsExpanded.size());
            KexiDB::BaseExpr *expr = 0;
            QList<uint>::ConstIterator it(visibleColumns.constBegin());
            for (it += visibleColumns.count() - 1; it != visibleColumns.constEnd(); --it) {
                KexiDB::QueryColumnInfo *ci = ((*it) < fieldsExpandedSize) ? fieldsExpanded.at(*it) : 0;
                if (!ci) {
                    kWarning() << *it << ">= fieldsExpandedSize";
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
            kDebug() << expr->toString();

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
//todo...
            kDebug() << "--- Private query: ";
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

    kWarning() << "no column relatedData \n - moving to setData(KexiDB::Field &)";

    // case 3: enum hints
    d->tv->setColumnStretchEnabled(true, -1);   //only needed when using single column

//! @todo THIS IS PRIMITIVE: we'd need to employ KexiDB::Reference here!
    d->int_f = new KexiDB::Field(field->name(), KexiDB::Field::Text);
    KexiTableViewData *data = new KexiTableViewData();
    data->addColumn(new KexiTableViewColumn(*d->int_f));
    const QVector<QString> hints(field->enumHints());
    for (int i = 0; i < hints.size(); i++) {
        KexiDB::RecordData *record = data->createItem();
        (*record)[0] = QVariant(hints[i]);
        kDebug() << "added: '" << hints[i] << "'";
        data->append(record);
    }
    setDataInternal(data, true);
}

void KexiComboBoxPopup::setDataInternal(KexiTableViewData *data, bool owner)
{
    if (d->tv->data())
        d->tv->data()->disconnect(this);
    d->tv->setData(data, owner);
    connect(d->tv, SIGNAL(dataRefreshed()), this, SLOT(slotDataReloadRequested()));

    updateSize();
}

void KexiComboBoxPopup::updateSize(int minWidth)
{
    const int rows = qMin(d->max_rows, d->tv->rows());

    d->tv->adjustColumnWidthToContents(-1);

    KexiTableEdit *te = dynamic_cast<KexiTableEdit*>(parentWidget());
    const int width = qMax(d->tv->tableSize().width(),
                           (te ? te->totalSize().width() : (parentWidget() ? parentWidget()->width() : 0/*sanity*/)));
    kDebug() << "size=" << size();
    resize(qMax(minWidth, width)/*+(d->tv->columns()>1?2:0)*/ /*(d->updateSizeCalled?0:1)*/, d->tv->rowHeight() * rows + 2);
    kDebug() << "size after=" << size();

    //stretch the last column
    d->tv->setColumnStretchEnabled(true, d->tv->columns() - 1);
}

KexiTableView* KexiComboBoxPopup::tableView()
{
    return d->tv;
}

void KexiComboBoxPopup::resize(int w, int h)
{
    d->tv->horizontalScrollBar()->hide();
    d->tv->verticalScrollBar()->hide();
    d->tv->move(1, 1);
    d->tv->resize(w - 2, h - 2);
    QFrame::resize(w, h);
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
    if (o == this && e->type() == QEvent::Hide) {
        emit hidden();
    } else if (e->type() == QEvent::MouseButtonPress) {
        kDebug() << "QEvent::MousePress";
    } else if (o == d->tv) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            const int k = ke->key();
            if ((ke->modifiers() == Qt::NoButton && (k == Qt::Key_Escape || k == Qt::Key_F4))
                    || (ke->modifiers() == Qt::AltModifier && k == Qt::Key_Up)) {
                hide();
                emit cancelled();
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

#include "kexicomboboxpopup.moc"
