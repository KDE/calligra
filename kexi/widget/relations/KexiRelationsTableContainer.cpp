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

#include <stdlib.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qstyle.h>
#include <QEvent>
#include <QDropEvent>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QStyleOptionFocusRect>

#include <KDebug>
#include <KIconLoader>

#include <kconfig.h>
#include <kglobalsettings.h>

#include <kexiutils/utils.h>
#include <kexidb/tableschema.h>
#include <kexidb/utils.h>
#include <kexidragobjects.h>
#include "KexiRelationsTableContainer.h"
#include "KexiRelationsTableContainer_p.h"
#include "KexiRelationsScrollArea.h"

class KexiRelationsTableContainer::Private
{
public:
    Private() {
    }
    KexiRelationViewTableContainerHeader *tableHeader;
    KexiRelationsTableFieldList *fieldList;
    KexiRelationsScrollArea *scrollArea;
};

//----------------------

KexiRelationsTableContainer::KexiRelationsTableContainer(
    QWidget* parent,
    KexiRelationsScrollArea *scrollArea,
    KexiDB::TableOrQuerySchema *schema)
        : QFrame(parent)
        , d(new Private)
{
    d->scrollArea = scrollArea;
    setObjectName("KexiRelationsTableContainer");
    setVisible(false); // scroll area will show it later
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Window);

// setFixedSize(100, 150);
//js: resize(100, 150);
    //setMouseTracking(true);

    setFrameStyle(QFrame::WinPanel | QFrame::Raised);

    QVBoxLayout *lyr = new QVBoxLayout(this);
    lyr->setContentsMargins(2, 2, 2, 2);
    lyr->setSpacing(1);

    d->tableHeader = new KexiRelationViewTableContainerHeader(schema->name(), this);

    d->tableHeader->unsetFocus();
    d->tableHeader->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    lyr->addWidget(d->tableHeader);
    connect(d->tableHeader, SIGNAL(moved()), this, SLOT(moved()));
    connect(d->tableHeader, SIGNAL(endDrag()), this, SIGNAL(endDrag()));

    d->fieldList = new KexiRelationsTableFieldList(schema, d->scrollArea, this);
    d->fieldList->setObjectName("KexiRelationsTableFieldList");
    //d->tableHeader->setFocusProxy( d->fieldList );
    d->fieldList->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
 //!TODO   d->fieldList->setHScrollBarMode(Q3ScrollView::AlwaysOff);

    d->fieldList->setMaximumSize(d->fieldList->sizeHint());

// d->fieldList->resize( d->fieldList->sizeHint() );
    lyr->addWidget(d->fieldList);
    connect(d->fieldList, SIGNAL(tableScrolling()), this, SLOT(moved()));
    connect(d->fieldList, SIGNAL(contextMenu(K3ListView*, Q3ListViewItem*, const QPoint&)),
            this, SLOT(slotContextMenu(K3ListView*, Q3ListViewItem*, const QPoint&)));
    connect(d->fieldList, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(slotFieldsDoubleClicked(const QModelIndex &)));
}

KexiRelationsTableContainer::~KexiRelationsTableContainer()
{
    delete d;
}

KexiDB::TableOrQuerySchema* KexiRelationsTableContainer::schema() const
{
    return d->fieldList->schema();
}

void KexiRelationsTableContainer::slotContextMenu(K3ListView *, Q3ListViewItem *,
        const QPoint &p)
{
// d->parent->executePopup(p);
    emit contextMenuRequest(p);
}

void KexiRelationsTableContainer::moved()
{
// kDebug()<<"finally emitting moved";
    emit moved(this);
}

int KexiRelationsTableContainer::globalY(const QString &field)
{
// kDebug();
// QPoint o = mapFromGlobal(QPoint(0, (d->fieldList->globalY(field))/*+d->parent->contentsY()*/));

    QPoint o(0, d->fieldList->globalY(field) + d->scrollArea->verticalScrollBar()->value()); //d->scrollArea->contentsY());
// kDebug() << "db2";
//Qt 4 return d->scrollArea->viewport()->mapFromGlobal(o).y();
    return d->scrollArea->widget()->mapFromGlobal(o).y();
}

#if 0//js
QSize KexiRelationsTableContainer::sizeHint()
{
#ifdef Q_WS_WIN
    QSize s = d->fieldList->sizeHint()
              + QSize(2 * 5 , d->tableHeader->height() + 2 * 5);
#else
    QSize s = d->fieldList->sizeHint();
    s.setWidth(s.width() + 4);
    s.setHeight(d->tableHeader->height() + s.height());
#endif
    return s;
}
#endif

void KexiRelationsTableContainer::focusInEvent(QFocusEvent* event)
{
    QFrame::focusInEvent(event);
    setFocus();
}

void KexiRelationsTableContainer::setFocus()
{
    kDebug() << "SET FOCUS";
    //select 1st:
//!TODO
#if 0
    if (d->fieldList->firstChild()) {
        if (d->fieldList->selectedItems().isEmpty())
            d->fieldList->setSelected(d->fieldList->firstChild(), true);
    }
    d->tableHeader->setFocus();
    d->fieldList->setFocus();
#endif
    raise();
    repaint();
    emit gotFocus();
}

void KexiRelationsTableContainer::focusOutEvent(QFocusEvent* event)
{
    QFrame::focusOutEvent(event);
    if (!d->fieldList->hasFocus())
        unsetFocus();
}

void KexiRelationsTableContainer::unsetFocus()
{
    kDebug() << "UNSET FOCUS";
    d->tableHeader->unsetFocus();
    d->fieldList->clearSelection();

    clearFocus();
    repaint();
}

void KexiRelationsTableContainer::slotFieldsDoubleClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);
    if (!KexiUtils::objectIsA(sender(), "KexiRelationsTableFieldList"))
        return;
    const KexiRelationsTableFieldList* t = static_cast<const KexiRelationsTableFieldList*>(sender());
    kDebug();
    emit fieldsDoubleClicked(*t->schema(), t->selectedFieldNames());
}

QStringList KexiRelationsTableContainer::selectedFieldNames() const
{
    return d->fieldList->selectedFieldNames();
}

#include "KexiRelationsTableContainer.moc"
