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

#include <KDebug>

#include <qstringlist.h>
#include <qlayout.h>
#include <qlabel.h>
#include <q3header.h>
#include <qevent.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qlineedit.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

#include <klocale.h>
#include <kaction.h>
#include <kmenu.h>
#include <kmessagebox.h>

#include <kexidb/tableschema.h>
#include <kexidb/indexschema.h>
#include <kexidb/utils.h>

#include "KexiRelationsScrollArea.h"
#include "KexiRelationsView.h"
#include "KexiRelationsConnection.h"
#include <kexi.h>

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
    ConnectionSet connectionViews;
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
    KexiRelationsScrollAreaWidget(KexiRelationsScrollArea* parent);
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
//Qt4 setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
//Qt4 setAttribute(Qt::WA_StaticContents, true);
// connect(relation, SIGNAL(relationListUpdated(QObject *)), this, SLOT(slotListUpdate(QObject *)));

    d->areaWidget = new KexiRelationsScrollAreaWidget(this);
    setWidget(d->areaWidget);
    setFocusPolicy(Qt::WheelFocus);
//Qt4 setResizePolicy(Manual);
// setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
#if 0
    d->removeSelectedTableQueryAction = new KAction(i18n("&Hide Selected Table/Query"), "edit-delete", "",
            this, SLOT(removeSelectedTableQuery()), parent->actionCollection(), "relationsview_removeSelectedTableQuery");
    d->removeSelectedConnectionAction = new KAction(i18n("&Remove Selected Relationship"), "dialog-cancel", "",
            this, SLOT(removeSelectedConnection()), parent->actionCollection(), "relationsview_removeSelectedConnection");
    d->openSelectedTableQueryAction = new KAction(i18n("&Open Selected Table/Query"), "", "",
            this, SLOT(openSelectedTableQuery()), 0/*parent->actionCollection()*/, "relationsview_openSelectedTableQuery");
#endif

// invalidateActions();

#if 0
    d->popup = new KMenu(this, "popup");
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
    delete d;
}

/*KexiRelationsTableContainer*
KexiRelationsScrollArea::containerForTable(KexiDB::TableSchema* tableSchema)
{
  if (!tableSchema)
    return 0;
  for (TablesDictIterator it(d->tables); it.current(); ++it) {
    if (it.current()->schema()->table()==tableSchema)
      return it.current();
  }
  return 0;
}*/

KexiRelationsTableContainer *
KexiRelationsScrollArea::tableContainer(KexiDB::TableSchema *t) const
{
    return t ? d->tables.value(t->name()) : 0;
}

KexiRelationsTableContainer*
KexiRelationsScrollArea::addTableContainer(KexiDB::TableSchema *t, const QRect &rect)
{
    if (!t)
        return 0;

    kDebug() << t->name(); // << ", " << viewport();

    KexiRelationsTableContainer* c = tableContainer(t);
    if (c) {
        kWarning() << "table already added";
        return c;
    }

    c = new KexiRelationsTableContainer(d->areaWidget, this,
                                        /*! @todo what about query? */
                                        new KexiDB::TableOrQuerySchema(t)
                                       );
    connect(c, SIGNAL(endDrag()), this, SLOT(slotTableViewEndDrag()));
    connect(c, SIGNAL(gotFocus()), this, SLOT(slotTableViewGotFocus()));
// connect(c, SIGNAL(headerContextMenuRequest(const QPoint&)),
//  this, SLOT(tableHeaderContextMenuRequest(const QPoint&)));
    connect(c, SIGNAL(contextMenuRequest(const QPoint&)),
            this, SIGNAL(tableContextMenuRequest(const QPoint&)));

    if (rect.isValid()) {//predefined size
        QSize finalSize = c->size().expandedTo(c->sizeHint());
        QRect r = rect;
        r.setSize(finalSize + QSize(0, 10));
        c->move(rect.left(), rect.top());
        //we're doing this instead of setGeometry(rect)
        //because the geomenty might be saved on other system with bigger fonts :)
        c->resize(c->sizeHint());
//  c->setGeometry(r);
//TODO

//  moveChild( c, rect.left(), rect.top() ); // setGeometry(rect);
//  c->resize( finalSize );
//  c->updateGeometry();
    } else
        c->move(100, 100);
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
//Qt 4 QPoint p = viewportToContents(QPoint(x, y));
//unused QPoint p(x, y);
//unused recalculateSize(p.x() + c->width(), p.y() + c->height());
    if (!rect.isValid()) {
        c->move(x, y);
    }

    d->tables.insert(t->name(), c);

    connect(c, SIGNAL(moved(KexiRelationsTableContainer *)), this,
            SLOT(containerMoved(KexiRelationsTableContainer *)));

    c->show();
    if (hasFocus()) //ok?
        c->setFocus();

    return c;
}

void
KexiRelationsScrollArea::addConnection(const SourceConnection& _conn)
{
    SourceConnection conn = _conn;
    kDebug();

    KexiRelationsTableContainer *master = d->tables[conn.masterTable];
    KexiRelationsTableContainer *details = d->tables[conn.detailsTable];
    if (!master || !details)
        return;

    /*! @todo what about query? */
    KexiDB::TableSchema *masterTable = master->schema()->table();
    /*! @todo what about query? */
    KexiDB::TableSchema *detailsTable = details->schema()->table();
    if (!masterTable || !detailsTable)
        return;

    // ok, but we need to know where is the 'master' and where is the 'details' side:
    KexiDB::Field *masterFld = masterTable->field(conn.masterField);
    KexiDB::Field *detailsFld = detailsTable->field(conn.detailsField);
    if (!masterFld || !detailsFld)
        return;

    if (!masterFld->isUniqueKey()) {
        if (detailsFld->isUniqueKey()) {
            //SWAP:
            KexiDB::Field *tmpFld = masterFld;
            masterFld = detailsFld;
            detailsFld = tmpFld;
            KexiDB::TableSchema *tmpTable = masterTable;
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

// kDebug() << "finalSRC =" << d->tables[conn.srcTable];

    KexiRelationsConnection *connView = new KexiRelationsConnection(master, details, conn, this);
    d->connectionViews.insert(connView);
//Qt4 updateContents(connView->connectionRect());
    kDebug() << "connView->connectionRect() " << connView->connectionRect();
    d->areaWidget->update();// connView->connectionRect() );


    /*js: will be moved up to relation/query part as this is only visual class
      KexiDB::TableSchema *mtable = d->conn->tableSchema(conn.srcTable);
      KexiDB::TableSchema *ftable = d->conn->tableSchema(conn.rcvTable);
      KexiDB::IndexSchema *forign = new KexiDB::IndexSchema(ftable);

      forign->addField(mtable->field(conn.srcField));
      new KexiDB::Reference(forign, mtable->primaryKey());
    */
#if 0
    if (!interactive) {
        kDebug() << "adding self";
        RelationList l = d->relation->projectRelations();
        l.append(conn);
        d->relation->updateRelationList(this, l);
    }
#endif
}

/* unused
void
KexiRelationsScrollArea::slotTableScrolling(const QString& table)
{
  KexiRelationsTableContainer *c = d->tables[table];

  if(c)
    containerMoved(c);
}*/

void
KexiRelationsScrollArea::containerMoved(KexiRelationsTableContainer *c)
{
    d->movedTableContainer = c;
    QRect r;
    foreach(KexiRelationsConnection* cview, d->connectionViews) {
//! @todo optimize
        if (cview->masterTable() == c || cview->detailsTable() == c
                || cview->connectionRect().intersects(r)) {
            r |= cview->oldRect();
            kDebug() << r;
            r |= cview->connectionRect();
            kDebug() << r;
        }
//   updateContents(cview->oldRect());
//   updateContents(cview->connectionRect());
//  }
    }
//! @todo optimize!
//didn't work well: updateContents(r);
//Qt 4 updateContents();

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

// QRect w(c->x() - 5, c->y() - 5, c->width() + 5, c->height() + 5);
// updateContents(w);

//Qt 4 QPoint p = viewportToContents(QPoint(c->x(), c->y()));
//unused QPoint p(c->x(), c->y());
//unused recalculateSize(p.x() + c->width(), p.y() + c->height());

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
        kDebug() << delay;
//  setUpdatesEnabled(false);
//  d->movedTableContainer->setUpdatesEnabled(false);
//  d->areaWidget->setUpdatesEnabled(false);
        int add = 16;
        if (horizontalScrollBar()->maximum() > (d->movedTableContainer->geometry().right() + add)) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + add);
            d->movedTableContainer->move(d->movedTableContainer->x() + add, d->movedTableContainer->y());
        }
//  d->areaWidget->setUpdatesEnabled(true);
//  d->movedTableContainer->setUpdatesEnabled(true);
//  setUpdatesEnabled(true);
    }
    d->areaWidget->update();
    if (d->movedTableContainer) {
//  if (d->autoScrollTimer.interval() > 100)
//   d->autoScrollTimer.setInterval( d->autoScrollTimer.interval() - 1 );
        d->autoScrollTimer.setInterval(delay);
        d->autoScrollTimer.start();
    }
}

void
KexiRelationsScrollArea::setReadOnly(bool b)
{
    d->readOnly = b;
//TODO
// invalidateActions();
    /* TableList::Iterator it, end( d->tables.end() );
      for ( it=d->tables.begin(); it != end; ++it)
      {
    //  (*it)->setReadOnly(b);
    #ifndef Q_WS_WIN
    #ifdef __GNUC__
        #warning readonly needed
    #endif
    #endif
      }*/
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
    foreach(KexiRelationsConnection* cview, d->connectionViews) {
        if (!cview->matchesPoint(ev->pos(), 3))
            continue;
        clearSelection();
        setFocus();
        cview->setSelected(true);
//Qt 4  updateContents(cview->connectionRect());
        d->areaWidget->update(cview->connectionRect());
        d->selectedConnection = cview;
        emit connectionViewGotFocus();
//  invalidateActions();

        if (ev->button() == Qt::RightButton) {//show popup
            kDebug() << "context";
            emit connectionContextMenuRequest(ev->globalPos());
        }
        return;
    }
    //connection not found
    clearSelection();
// invalidateActions();
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
//Qt 4 QRect clipping(cx, cy, cw, ch);
    QPainter p(d->areaWidget);
    p.setWindow(
        horizontalScrollBar() ? horizontalScrollBar()->value() : 0,
        verticalScrollBar() ? verticalScrollBar()->value() : 0,
        d->areaWidget->width(), d->areaWidget->height());
    QRect clipping(
        horizontalScrollBar() ? horizontalScrollBar()->value() : 0,
        verticalScrollBar() ? verticalScrollBar()->value() : 0,
        width(), height());
//  viewport()->width(), viewport()->height());
    foreach(KexiRelationsConnection *cview, d->connectionViews) {
//TODO  if (clipping.intersects(cview->oldRect() | cview->connectionRect()))
        cview->drawConnection(&p);
    }
}

void KexiRelationsScrollArea::clearSelection()
{
    if (d->focusedTableContainer) {
        d->focusedTableContainer->unsetFocus();
        d->focusedTableContainer = 0;
//  setFocus();
//  invalidateActions();
    }
    if (d->selectedConnection) {
        d->selectedConnection->setSelected(false);
//Qt 4  updateContents(d->selectedConnection->connectionRect());
        d->areaWidget->update(d->selectedConnection->connectionRect());
        d->selectedConnection = 0;
//  invalidateActions();
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
//  d->popup->exec( mapToGlobal( d->focusedTableContainer ? d->focusedTableContainer->pos() + d->focusedTableContainer->rect().center() : rect().center() ) );
//  executePopup();
}

void
KexiRelationsScrollArea::keyPressEvent(QKeyEvent *ev)
{
    kDebug();

    if (ev->key() == Qt::Key_Delete) {
        removeSelectedObject();
    }
}

/* unused, we have now one large area
void
KexiRelationsScrollArea::recalculateSize(int width, int height)
{
  kDebug() << "recalculateSize(" << width << ", " << height << ")";
  int newW = d->areaWidget->width();
  int newH = d->areaWidget->height();
  kDebug() << "contentsSize(" << newW << ", " << newH << ")";

  if (newW < width)
    newW = width;

  if (newH < height)
    newH = height;

  d->areaWidget->resize(newW, newH);
}*/

/* Resizes contents to size exactly enough to fit tableViews.
  Executed on every tableView's drop event.
*/
/*unused
void
KexiRelationsScrollArea::stretchExpandSize()
{
  int max_x=-1, max_y=-1;
  foreach (KexiRelationsTableContainer* container, d->tables) {
    if (container->right() > max_x)
      max_x = container->right();
    if (container->bottom() > max_y)
      max_y = container->bottom();
  }
  QPoint p(max_x + 3, max_y + 3);
//Qt 4 QPoint p = viewportToContents(QPoint(max_x, max_y) + QPoint(3,3)); //3 pixels margin
  d->areaWidget->resize(p.x(), p.y());
}*/

void KexiRelationsScrollArea::slotTableViewEndDrag()
{
    kDebug() << "END DRAG!";
    d->autoScrollTimer.stop();
// stretchExpandSize();
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
                kDebug() << "matching found!";
//   l.remove(it);
            } else {
                nl.append(*it);
            }
        }

        d->relation->updateRelationList(this, nl);
#endif
//  invalidateActions();
    } else if (d->focusedTableContainer) {
        KexiRelationsTableContainer *tmp = d->focusedTableContainer;
        d->focusedTableContainer = 0;
        hideTable(tmp);
    }
}

void
KexiRelationsScrollArea::hideTable(KexiRelationsTableContainer* container)
{
    /*! @todo what about query? */
    TablesHashMutableIterator it(d->tables);
    if (!it.findNext(container))
        return;
    hideTableInternal(it);
}

void
KexiRelationsScrollArea::hideTableInternal(TablesHashMutableIterator& it)
{
    KexiRelationsTableContainer* container = it.value();
    KexiDB::TableSchema *ts = container->schema()->table();
    //for all connections: find and remove all connected with this table
    for (ConnectionSetMutableIterator itConn(d->connectionViews);itConn.hasNext();) {
        KexiRelationsConnection* conn = itConn.next();
        if (conn->masterTable() == container
                || conn->detailsTable() == container) {
            //remove this
            removeConnectionInternal(itConn);
        }
    }
    it.remove();
// d->tables.take(container->schema()->name());
    delete container;
    emit tableHidden(*ts);
}

void
KexiRelationsScrollArea::hideAllTablesExcept(KexiDB::TableSchema::List* tables)
{
//! @todo what about queries?
    for (TablesHashMutableIterator it(d->tables); it.hasNext();) {
        it.next();
        KexiDB::TableSchema *table = it.value()->schema()->table();
        if (!table || tables->contains(table))
            continue;
        hideTableInternal(it);
    }
}

void
KexiRelationsScrollArea::removeConnection(KexiRelationsConnection *conn)
{
    ConnectionSetMutableIterator it(d->connectionViews);
    if (!it.findNext(conn))
        return;
    removeConnectionInternal(it);
}

void
KexiRelationsScrollArea::removeConnectionInternal(ConnectionSetMutableIterator& it)
{
    KexiRelationsConnection *conn = it.value();
    emit aboutConnectionRemove(conn);
    it.remove();
//Qt 4 updateContents(conn->connectionRect());
    d->areaWidget->update(conn->connectionRect());
    delete conn;
    kDebug();
}

void KexiRelationsScrollArea::slotTableViewGotFocus()
{
    if (d->focusedTableContainer == sender())
        return;
    kDebug() << "GOT FOCUS!";
    clearSelection();
// if (d->focusedTableContainer)
//  d->focusedTableContainer->unsetFocus();
    d->focusedTableContainer = (KexiRelationsTableContainer*)sender();
// invalidateActions();
    emit tableViewGotFocus();
}

QSize KexiRelationsScrollArea::sizeHint() const
{
    return QSize(QScrollArea::sizeHint());//.width(), 600);
}

void KexiRelationsScrollArea::clear()
{
    removeAllConnections();
    foreach(KexiRelationsTableContainer* container, d->tables)
    delete container;
    d->tables.clear();
//Qt 4 updateContents();
    d->areaWidget->update();
}

void KexiRelationsScrollArea::removeAllConnections()
{
    clearSelection(); //sanity
    qDeleteAll(d->connectionViews);
    d->connectionViews.clear();
//Qt 4 updateContents();
    d->areaWidget->update();
}

/*

void KexiRelationsScrollArea::tableHeaderContextMenuRequest(const QPoint& pos)
{
  if (d->focusedTableContainer != sender())
    return;
  kDebug() << "HEADER CTXT MENU!";
  invalidateActions();
  d->tableQueryPopup->exec(pos);
}

//! Invalidates all actions availability
void KexiRelationsScrollArea::invalidateActions()
{
  setAvailable("edit_delete", d->selectedConnection || d->focusedTableContainer);
}

void KexiRelationsScrollArea::executePopup( QPoint pos )
{
  if (pos==QPoint(-1,-1)) {
    pos = mapToGlobal( d->focusedTableContainer ? d->focusedTableContainer->pos() + d->focusedTableContainer->rect().center() : rect().center() );
  }
  if (d->focusedTableContainer)
    d->tableQueryPopup->exec(pos);
  else if (d->selectedConnection)
    d->connectionPopup->exec(pos);
}
*/

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

const ConnectionSet* KexiRelationsScrollArea::connections() const
{
    return &d->connectionViews;
}

#include "KexiRelationsScrollArea.moc"
