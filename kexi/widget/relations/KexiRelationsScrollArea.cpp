/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#include "KexiRelationsScrollArea.h"
#include "KexiRelationsView.h"
#include "KexiRelationsConnection.h"
#include <kexi.h>

#include <KDbTableSchema>
#include <KDbIndexSchema>
#include <KDbTableOrQuerySchema>
#include <KDbUtils>

#include <KLocalizedString>

#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QDebug>

//! @internal
class KexiRelationsScrollArea::Private
{
public:
    Private()
            : readOnly(false)
            , selectedConnection(0)
            , focusedTableContainer(0) {
        autoScrollTimer.setSingleShot(true);
    }

    QWidget *areaWidget;
    TablesHash tables;
    bool readOnly;
    QSet<KexiRelationsConnection*> relationsConnections;
    KexiRelationsConnection* selectedConnection;
    QPointer<KexiRelationsTableContainer> focusedTableContainer;
    QPointer<KexiRelationsTableContainer> movedTableContainer;
    QTimer autoScrollTimer;
};

//-------------

//! @internal scroll area widget that draws connections
class KexiRelationsScrollAreaWidget : public QWidget
{
public:
    explicit KexiRelationsScrollAreaWidget(KexiRelationsScrollArea* parent);
    ~KexiRelationsScrollAreaWidget();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *ev);

private:
    KexiRelationsScrollArea *scrollArea() {
        return static_cast<KexiRelationsScrollArea*>(parentWidget()->parentWidget());
    }
};

KexiRelationsScrollAreaWidget::KexiRelationsScrollAreaWidget(
    KexiRelationsScrollArea* parent)
        : QWidget(parent)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Mid);
    resize(10240, 7680);
}

KexiRelationsScrollAreaWidget::~KexiRelationsScrollAreaWidget()
{
}

void KexiRelationsScrollAreaWidget::paintEvent(QPaintEvent *event)
{
    scrollArea()->handlePaintEvent(event);
}

void KexiRelationsScrollAreaWidget::mousePressEvent(QMouseEvent *ev)
{
    scrollArea()->handleMousePressEvent(ev);
    QWidget::mousePressEvent(ev);
}

//-------------

KexiRelationsScrollArea::KexiRelationsScrollArea(QWidget *parent)
        : QScrollArea(parent)
        , d(new Private)
{
    d->areaWidget = new KexiRelationsScrollAreaWidget(this);
    setWidget(d->areaWidget);
    setFocusPolicy(Qt::WheelFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
#if 0
    d->removeSelectedTableQueryAction = new QAction(xi18n("&Hide Selected Table/Query"), "edit-delete", "",
            this, SLOT(removeSelectedTableQuery()), parent->actionCollection(), "relationsview_removeSelectedTableQuery");
    d->removeSelectedConnectionAction = new QAction(xi18n("&Remove Selected Relationship"), "dialog-cancel", "",
            this, SLOT(removeSelectedConnection()), parent->actionCollection(), "relationsview_removeSelectedConnection");
    d->openSelectedTableQueryAction = new QAction(xi18n("&Open Selected Table/Query"), "", "",
            this, SLOT(openSelectedTableQuery()), 0/*parent->actionCollection()*/, "relationsview_openSelectedTableQuery");
#endif

#if 0
    d->popup = new QMenu(this, "popup");
    d->openSelectedTableQueryAction->plug(d->popup);
    d->removeSelectedTableQueryAction->plug(d->popup);
    d->removeSelectedConnectionAction->plug(d->popup);

    invalidateActions();
#endif

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //true);
    connect(&d->autoScrollTimer, SIGNAL(timeout()), this, SLOT(slotAutoScrollTimeout()));
}

KexiRelationsScrollArea::~KexiRelationsScrollArea()
{
    clearSelection(); //sanity
    qDeleteAll(d->relationsConnections);
    d->relationsConnections.clear();
    delete d;
}

KexiRelationsTableContainer *
KexiRelationsScrollArea::tableContainer(KDbTableSchema *t) const
{
    return t ? d->tables.value(t->name()) : 0;
}

KexiRelationsTableContainer*
KexiRelationsScrollArea::addTableContainer(KDbTableSchema *t, const QRect &rect)
{
    if (!t)
        return 0;

    qDebug() << t->name();

    KexiRelationsTableContainer* c = tableContainer(t);
    if (c) {
        qWarning() << "table already added";
        return c;
    }

    c = new KexiRelationsTableContainer(d->areaWidget, this,
                                        /*! @todo what about query? */
                                        new KDbTableOrQuerySchema(t)
                                       );
    connect(c, SIGNAL(endDrag()), this, SLOT(slotTableViewEndDrag()));
    connect(c, SIGNAL(gotFocus()), this, SLOT(slotTableViewGotFocus()));
    connect(c, SIGNAL(contextMenuRequest(QPoint)),
            this, SIGNAL(tableContextMenuRequest(QPoint)));

    if (rect.isValid()) {//predefined size
        QSize finalSize = c->size().expandedTo(c->sizeHint());
        QRect r = rect;
        r.setSize(finalSize + QSize(0, 10));
        c->move(rect.left(), rect.top());
        //we're doing this instead of setGeometry(rect)
        //because the geomenty might be saved on other system with bigger fonts :)
        c->resize(c->sizeHint());
    }
    else {
        c->move(100, 100);
    }
    updateGeometry();
    if (!rect.isValid()) {
        c->updateGeometry();
        c->resize(c->sizeHint());
    }
    int x, y;

    if (d->tables.count() > 0) {
        int place = -10;
        foreach(KexiRelationsTableContainer* container, d->tables) {
            int right = container->x() + container->width();
            if (right > place)
                place = right;
        }

        x = place + 30;
    } else {
        x = 5;
    }

    y = 5;
    if (!rect.isValid()) {
        c->move(x, y);
    }

    d->tables.insert(t->name(), c);

    connect(c, SIGNAL(moved(KexiRelationsTableContainer*)), this,
            SLOT(containerMoved(KexiRelationsTableContainer*)));

    c->show();
    if (hasFocus()) //ok?
        c->setFocus();

    return c;
}

void
KexiRelationsScrollArea::addConnection(const SourceConnection& _conn)
{
    SourceConnection conn = _conn;
    qDebug();

    KexiRelationsTableContainer *master = d->tables[conn.masterTable];
    KexiRelationsTableContainer *details = d->tables[conn.detailsTable];
    if (!master || !details)
        return;

    /*! @todo what about query? */
    KDbTableSchema *masterTable = master->schema()->table();
    /*! @todo what about query? */
    KDbTableSchema *detailsTable = details->schema()->table();
    if (!masterTable || !detailsTable)
        return;

    // ok, but we need to know where is the 'master' and where is the 'details' side:
    KDbField *masterFld = masterTable->field(conn.masterField);
    KDbField *detailsFld = detailsTable->field(conn.detailsField);
    if (!masterFld || !detailsFld)
        return;

    if (!masterFld->isUniqueKey()) {
        if (detailsFld->isUniqueKey()) {
            //SWAP:
            KDbField *tmpFld = masterFld;
            masterFld = detailsFld;
            detailsFld = tmpFld;
            KDbTableSchema *tmpTable = masterTable;
            masterTable = detailsTable;
            detailsTable = tmpTable;
            KexiRelationsTableContainer *tmp = master;
            master = details;
            details = tmp;
            QString tmp_masterTable = conn.masterTable;
            conn.masterTable = conn.detailsTable;
            conn.detailsTable = tmp_masterTable;
            QString tmp_masterField = conn.masterField;
            conn.masterField = conn.detailsField;
            conn.detailsField = tmp_masterField;
        }
    }

// qDebug() << "finalSRC =" << d->tables[conn.srcTable];

    KexiRelationsConnection *connView = new KexiRelationsConnection(master, details, conn, this);
    d->relationsConnections.insert(connView);
    qDebug() << "connView->connectionRect() " << connView->connectionRect();
    d->areaWidget->update();

    /*! @todo will be moved up to relation/query part as this is only visual class
      KDbTableSchema *mtable = d->conn->tableSchema(conn.srcTable);
      KDbTableSchema *ftable = d->conn->tableSchema(conn.rcvTable);
      KDbIndexSchema *forign = new KDbIndexSchema(ftable);

      forign->addField(mtable->field(conn.srcField));
      new KDbReference(forign, mtable->primaryKey());
    */
#if 0
    if (!interactive) {
        qDebug() << "adding self";
        RelationList l = d->relation->projectRelations();
        l.append(conn);
        d->relation->updateRelationList(this, l);
    }
#endif
}

void
KexiRelationsScrollArea::containerMoved(KexiRelationsTableContainer *c)
{
    d->movedTableContainer = c;
    QRect r;
    foreach(KexiRelationsConnection* cview, d->relationsConnections) {
//! @todo optimize
        if (cview->masterTable() == c || cview->detailsTable() == c
                || cview->connectionRect().intersects(r)) {
            r |= cview->oldRect();
            //qDebug() << r;
            r |= cview->connectionRect();
            //qDebug() << r;
        }
    }
//! @todo optimize!

    //scroll if needed:
    if (horizontalScrollBar()->maximum() > c->geometry().right()) {
        bool beyondBorder = c->geometry().left() > (horizontalScrollBar()->value() + width() - verticalScrollBar()->width());
        if (!d->autoScrollTimer.isActive()) {
            if (beyondBorder) {
                d->autoScrollTimer.setInterval(200);
                d->autoScrollTimer.start();
                //  horizontalScrollBar()->setValue( horizontalScrollBar()->value() + 50 );
            }
        } else {
            if (!beyondBorder) {
                d->autoScrollTimer.stop();
            }
        }
    } else
        d->autoScrollTimer.stop();

    d->areaWidget->update();
    emit tablePositionChanged(c);
}

void
KexiRelationsScrollArea::slotAutoScrollTimeout()
{
    int delay = 100;
    if (d->movedTableContainer) {
        delay = qMin(
                    100,
                    100 - (d->movedTableContainer->geometry().left()
                           - (horizontalScrollBar()->value() + width() - verticalScrollBar()->width()))
                );
        if (delay < 0)
            delay = 0;
        delay = delay * delay / 100;
        qDebug() << delay;
        int add = 16;
        if (horizontalScrollBar()->maximum() > (d->movedTableContainer->geometry().right() + add)) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + add);
            d->movedTableContainer->move(d->movedTableContainer->x() + add, d->movedTableContainer->y());
        }
    }
    d->areaWidget->update();
    if (d->movedTableContainer) {
        d->autoScrollTimer.setInterval(delay);
        d->autoScrollTimer.start();
    }
}

void
KexiRelationsScrollArea::setReadOnly(bool b)
{
    d->readOnly = b;
//! @todo
}

void
KexiRelationsScrollArea::slotListUpdate(QObject *)
{
#if 0
    if (s != this) {
        d->connectionViews.clear();
        RelationList rl = d->relation->projectRelations();
        if (!rl.isEmpty()) {
            RelationList::ConstIterator it, end(rl.constEnd());
            for (it = rl.begin(); it != end; ++it) {
                addConnection((*it), true);
            }
        }
    }

    updateContents();
#endif
}

void
KexiRelationsScrollArea::handleMousePressEvent(QMouseEvent *ev)
{
    foreach(KexiRelationsConnection* cview, d->relationsConnections) {
        if (!cview->matchesPoint(ev->pos(), 3))
            continue;
        clearSelection();
        setFocus();
        cview->setSelected(true);
        d->areaWidget->update(cview->connectionRect());
        d->selectedConnection = cview;
        emit connectionViewGotFocus();

        if (ev->button() == Qt::RightButton) {//show popup
            qDebug() << "context";
            emit connectionContextMenuRequest(ev->globalPos());
        }
        return;
    }
    //connection not found
    clearSelection();
    if (ev->button() == Qt::RightButton) {//show popup on view background area
        emit emptyAreaContextMenuRequest(ev->globalPos());
    } else {
        emit emptyAreaGotFocus();
    }
    setFocus();
}

void KexiRelationsScrollArea::handlePaintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(d->areaWidget);
    p.setWindow(
        horizontalScrollBar() ? horizontalScrollBar()->value() : 0,
        verticalScrollBar() ? verticalScrollBar()->value() : 0,
        d->areaWidget->width(), d->areaWidget->height());
    QRect clipping(
        horizontalScrollBar() ? horizontalScrollBar()->value() : 0,
        verticalScrollBar() ? verticalScrollBar()->value() : 0,
        width(), height());
    foreach(KexiRelationsConnection *cview, d->relationsConnections) {
        cview->drawConnection(&p);
    }
}

void KexiRelationsScrollArea::clearSelection()
{
    if (d->focusedTableContainer) {
        d->focusedTableContainer->unsetFocus();
        d->focusedTableContainer = 0;
    }
    if (d->selectedConnection) {
        d->selectedConnection->setSelected(false);
        d->areaWidget->update(d->selectedConnection->connectionRect());
        d->selectedConnection = 0;
    }
}

void
KexiRelationsScrollArea::contextMenuEvent(QContextMenuEvent* event)
{
    Q_UNUSED(event);
    if (d->selectedConnection) {
        emit connectionContextMenuRequest(
            mapToGlobal(d->selectedConnection->connectionRect().center()));
    }
}

void
KexiRelationsScrollArea::keyPressEvent(QKeyEvent *ev)
{
    qDebug();

    if (ev->key() == Qt::Key_Delete) {
        removeSelectedObject();
    }
}

void KexiRelationsScrollArea::slotTableViewEndDrag()
{
    qDebug() << "END DRAG!";
    d->autoScrollTimer.stop();
}

void
KexiRelationsScrollArea::removeSelectedObject()
{
    if (d->selectedConnection) {
        KexiRelationsConnection* tmp = d->selectedConnection;
        d->selectedConnection = 0;
        removeConnection(tmp);

#if 0
        RelationList l = d->relation->projectRelations();
        RelationList nl;
        for (RelationList::Iterator it = l.begin(); it != l.end(); ++it) {
            if ((*it).srcTable == d->selectedConnection->connection().srcTable
                    && (*it).rcvTable == d->selectedConnection->connection().rcvTable
                    && (*it).srcField == d->selectedConnection->connection().srcField
                    && (*it).rcvField == d->selectedConnection->connection().rcvField) {
                qDebug() << "matching found!";
            } else {
                nl.append(*it);
            }
        }

        d->relation->updateRelationList(this, nl);
#endif
    }
    else if (d->focusedTableContainer) {
        KexiRelationsTableContainer *tmp = d->focusedTableContainer;
        d->focusedTableContainer = 0;
        hideTable(tmp);
    }
}

void
KexiRelationsScrollArea::hideTable(KexiRelationsTableContainer* container)
{
    Q_ASSERT(container);
    /*! @todo what about query? */
    TablesHashMutableIterator it(d->tables);
    if (!it.findNext(container))
        return;
    hideTableInternal(&it);
}

void KexiRelationsScrollArea::hideTableInternal(TablesHashMutableIterator* it)
{
    Q_ASSERT(it);
    KexiRelationsTableContainer* container = it->value();
    KDbTableSchema *ts = container->schema()->table();
    //for all connections: find and remove all connected with this table
    for (ConnectionSetMutableIterator itConn(d->relationsConnections);itConn.hasNext();) {
        KexiRelationsConnection* conn = itConn.next();
        if (conn->masterTable() == container
                || conn->detailsTable() == container) {
            //remove this
            removeConnectionInternal(&itConn);
        }
    }
    it->remove();
    delete container;
    emit tableHidden(ts);
}

void
KexiRelationsScrollArea::hideAllTablesExcept(QList<KDbTableSchema*>* tables)
{
    Q_ASSERT(tables);
//! @todo what about queries?
    for (TablesHashMutableIterator it(d->tables); it.hasNext();) {
        it.next();
        KDbTableSchema *table = it.value()->schema()->table();
        if (!table || tables->contains(table))
            continue;
        hideTableInternal(&it);
    }
}

void
KexiRelationsScrollArea::removeConnection(KexiRelationsConnection *conn)
{
    Q_ASSERT(conn);
    ConnectionSetMutableIterator it(d->relationsConnections);
    if (!it.findNext(conn))
        return;
    removeConnectionInternal(&it);
}

void KexiRelationsScrollArea::removeConnectionInternal(ConnectionSetMutableIterator* it)
{
    Q_ASSERT(it);
    KexiRelationsConnection *conn = it->value();
    emit aboutConnectionRemove(conn);
    it->remove();
    d->areaWidget->update(conn->connectionRect());
    delete conn;
}

void KexiRelationsScrollArea::slotTableViewGotFocus()
{
    if (d->focusedTableContainer == sender())
        return;
    qDebug() << "GOT FOCUS!";
    clearSelection();
    d->focusedTableContainer = (KexiRelationsTableContainer*)sender();
    emit tableViewGotFocus();
}

QSize KexiRelationsScrollArea::sizeHint() const
{
    return QSize(QScrollArea::sizeHint());
}

void KexiRelationsScrollArea::clear()
{
    removeAllConnections();
    qDeleteAll(d->tables);
    d->tables.clear();
    d->areaWidget->update();
}

void KexiRelationsScrollArea::removeAllConnections()
{
    clearSelection(); //sanity
    qDeleteAll(d->relationsConnections);
    d->relationsConnections.clear();
    d->areaWidget->update();
}

TablesHash* KexiRelationsScrollArea::tables() const
{
    return &d->tables;
}

KexiRelationsConnection* KexiRelationsScrollArea::selectedConnection() const
{
    return d->selectedConnection;
}

KexiRelationsTableContainer* KexiRelationsScrollArea::focusedTableContainer() const
{
    return d->focusedTableContainer;
}

const QSet<KexiRelationsConnection*>* KexiRelationsScrollArea::relationsConnections() const
{
    return &d->relationsConnections;
}

