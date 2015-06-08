/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
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

#include "KexiRelationsTableContainer_p.h"
#include "KexiRelationsScrollArea.h"
#include <kexidragobjects.h>

#include <KDbTableSchema>
#include <KDbUtils>

#include <QLabel>
#include <QPushButton>
#include <QPoint>
#include <QStyle>
#include <QEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>

KexiRelationViewTableContainerHeader::KexiRelationViewTableContainerHeader(
    const QString& text, QWidget *parent)
        : QLabel(text, parent), m_dragging(false)
{
    setAutoFillBackground(true);
    setContentsMargins(2, 2, 2, 2);
    m_activeBG = KGlobalSettings::activeTitleColor();
    m_activeFG = KGlobalSettings::activeTextColor();
    m_inactiveBG = KGlobalSettings::inactiveTitleColor();
    m_inactiveFG = KGlobalSettings::inactiveTextColor();

    installEventFilter(this);
}

KexiRelationViewTableContainerHeader::~KexiRelationViewTableContainerHeader()
{
}

void KexiRelationViewTableContainerHeader::setFocus()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Window, m_activeBG);
    pal.setColor(QPalette::WindowText, m_activeFG);
    setPalette(pal);
}

void KexiRelationViewTableContainerHeader::unsetFocus()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Window, m_inactiveBG);
    pal.setColor(QPalette::WindowText, m_inactiveFG);
    setPalette(pal);
}

bool KexiRelationViewTableContainerHeader::eventFilter(QObject *, QEvent *ev)
{
    if (ev->type() == QEvent::MouseMove) {
        if (m_dragging) {// && static_cast<QMouseEvent*>(ev)->modifiers()==Qt::LeftButton) {
            int diffX = static_cast<QMouseEvent*>(ev)->globalPos().x() - m_grabX;
            int diffY = static_cast<QMouseEvent*>(ev)->globalPos().y() - m_grabY;
            if ((qAbs(diffX) > 2) || (qAbs(diffY) > 2)) {
                QPoint newPos = parentWidget()->pos() + QPoint(diffX, diffY);
                //correct the x position
                if (newPos.x() < 0) {
                    m_offsetX += newPos.x();
                    newPos.setX(0);
                } else if (m_offsetX < 0) {
                    m_offsetX += newPos.x();
                    if (m_offsetX > 0) {
                        newPos.setX(m_offsetX);
                        m_offsetX = 0;
                    } else newPos.setX(0);
                }
                //correct the y position
                if (newPos.y() < 0) {
                    m_offsetY += newPos.y();
                    newPos.setY(0);
                } else if (m_offsetY < 0) {
                    m_offsetY += newPos.y();
                    if (m_offsetY > 0) {
                        newPos.setY(m_offsetY);
                        m_offsetY = 0;
                    } else newPos.setY(0);
                }
                //move and update helpers
                parentWidget()->move(newPos);
                m_grabX = static_cast<QMouseEvent*>(ev)->globalPos().x();
                m_grabY = static_cast<QMouseEvent*>(ev)->globalPos().y();
                // qDebug()<<"HEADER:emitting moved";
                emit moved();
            }
            return true;
        }
    }
    return false;
}

void KexiRelationViewTableContainerHeader::mousePressEvent(QMouseEvent *ev)
{
    //qDebug();
    static_cast<KexiRelationsTableContainer*>(parentWidget())->setFocus();
    ev->accept();
    if (ev->button() == Qt::LeftButton) {
        m_dragging = true;
        m_grabX = ev->globalPos().x();
        m_grabY = ev->globalPos().y();
        m_offsetX = 0;
        m_offsetY = 0;
        setCursor(Qt::SizeAllCursor);
        return;
    }
    if (ev->button() == Qt::RightButton) {
        emit static_cast<KexiRelationsTableContainer*>(parentWidget())
        ->contextMenuRequest(ev->globalPos());
    }
}

void KexiRelationViewTableContainerHeader::mouseReleaseEvent(QMouseEvent *ev)
{
    //qDebug();
    if (m_dragging && ev->button() & Qt::LeftButton) {
        setCursor(Qt::ArrowCursor);
        m_dragging = false;
        emit endDrag();
    }
    ev->accept();
}

//=====================================================================================

KexiRelationsTableFieldList::KexiRelationsTableFieldList(
    KexiDB::TableOrQuerySchema* tableOrQuerySchema,
    KexiRelationsScrollArea *scrollArea, QWidget *parent)
        : KexiFieldListView(parent, ShowAsterisk)
        , m_scrollArea(scrollArea)
{
    setSchema(tableOrQuerySchema);
    setAcceptDrops(true);

    connect(this, SIGNAL(contentsMoving(int,int)),
            this, SLOT(slotContentsMoving(int,int)));

    horizontalScrollBar()->installEventFilter(this);
    verticalScrollBar()->installEventFilter(this);
}

KexiRelationsTableFieldList::~KexiRelationsTableFieldList()
{
}

QSize KexiRelationsTableFieldList::sizeHint() const
{
    QFontMetrics fm(fontMetrics());

// qDebug() << schema()->name() << " cw=" << columnWidth(0) + fm.width("i")
//  << ", " << fm.width(schema()->name()+"  ");
//! @todo
    return KexiFieldListView::sizeHint();
}

int KexiRelationsTableFieldList::globalY(const QString &item)
{
    QAbstractItemModel *themodel = model();
    QModelIndex idx;
    
    for (int i = 0; i < themodel->rowCount(); ++i) {
        idx = themodel->index(i, 0);
        QVariant data = themodel->data(idx);
        if (data.toString() == item) {
            break;
        }
    }
    
    if (idx.isValid()) {
        QRect r = this->rectForIndex(idx);
        int y = r.y() + r.height()/2;
        
        //Not sure what this line is supposed to do...is it to check if the item is visible?
        if (visualRect(idx).y() > viewport()->height()){   
            y = 0;
        } else if (y == 0) {
            y = height();
        }
        return mapToGlobal(QPoint(0, y)).y();
    }
    return -1;
}

void KexiRelationsTableFieldList::dragEnterEvent(QDragEnterEvent* event)
{

    KexiFieldListView::dragEnterEvent(event);
}

void KexiRelationsTableFieldList::dragMoveEvent(QDragMoveEvent* event)
{
    QModelIndex receiver = indexAt(event->pos());
    if (!receiver.isValid() || !KexiFieldDrag::canDecode(event))
        return;
    QString sourceMimeType;
    QString srcTable;
    QStringList srcFields;
    QString srcField;
    
    if (!KexiFieldDrag::decode(event, &sourceMimeType, &srcTable, &srcFields)) {
        event->ignore();
        return;
    }
    
    if (sourceMimeType != "kexi/table" && sourceMimeType == "kexi/query"){
        event->ignore();
        return;
    }
    
    if (srcFields.count() != 1) {
        event->ignore();
        return;
    }
    
    srcField = srcFields[0];
    
    if (srcTable == schema()->name()) {
        event->ignore();
        return;
    }
        
    QString f = model()->data(receiver, Qt::DisplayRole).toString();
    
    //qDebug() << "Source:" << srcTable << "Dest:" << schema()->name();
    if (!srcField.trimmed().startsWith('*') && !f.startsWith('*'))
        event->acceptProposedAction();
}


void KexiRelationsTableFieldList::dropEvent(QDropEvent *event)
{
    //qDebug();
    QModelIndex idx = indexAt(event->pos());
    
    if (!idx.isValid() || !KexiFieldDrag::canDecode(event)) {
        event->ignore();
        return;
    }
    QString sourceMimeType;
    QString srcTable;
    QStringList srcFields;
    QString srcField;
    
    if (!KexiFieldDrag::decode(event, &sourceMimeType, &srcTable, &srcFields)) {
        return;
    }
    
    if (sourceMimeType != "kexi/table" && sourceMimeType == "kexi/query") {
        return;
    }
    
    if (srcFields.count() != 1) {
        return;
    }
    srcField = srcFields[0];
//  qDebug() << "srcfield:" << srcField;

    QString rcvField = model()->data(idx, Qt::DisplayRole).toString();

    SourceConnection s;
    s.masterTable = srcTable;
    s.detailsTable = schema()->name();
    s.masterField = srcField;
    s.detailsField = rcvField;

    m_scrollArea->addConnection(s);

    //qDebug() << srcTable << ":" << srcField << schema()->name() << ":" << rcvField;
    event->accept();
}

void KexiRelationsTableFieldList::slotContentsMoving(int, int)
{
    emit tableScrolling();
}

void KexiRelationsTableFieldList::contentsMousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    // set focus before showing context menu because contents of the menu depend on focused table
    static_cast<KexiRelationsTableContainer*>(parentWidget())->setFocus();
}

bool KexiRelationsTableFieldList::eventFilter(QObject *o, QEvent *ev)
{
    if (o == verticalScrollBar() || o == horizontalScrollBar()) {
        //qDebug() << ev->type();
    }
    return KexiFieldListView::eventFilter(o, ev);
}

