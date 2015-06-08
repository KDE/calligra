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

#include <kexiutils/utils.h>
#include <kexidragobjects.h>
#include "KexiRelationsTableContainer.h"
#include "KexiRelationsTableContainer_p.h"
#include "KexiRelationsScrollArea.h"

#include <KDbTableSchema>
#include <KDbUtils>

#include <QPushButton>
#include <QPoint>
#include <QStyle>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QDebug>

#include <stdlib.h>

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
    d->fieldList->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    d->fieldList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->fieldList->setMaximumSize(d->fieldList->sizeHint());
    d->fieldList->setContextMenuPolicy(Qt::CustomContextMenu);

    lyr->addWidget(d->fieldList);
    connect(d->fieldList, SIGNAL(tableScrolling()), this, SLOT(moved()));
    
    connect(d->fieldList, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(slotContextMenu(QPoint)));

    connect(d->fieldList, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(slotFieldsDoubleClicked(QModelIndex)));
}

KexiRelationsTableContainer::~KexiRelationsTableContainer()
{
    delete d;
}

KexiDB::TableOrQuerySchema* KexiRelationsTableContainer::schema() const
{
    return d->fieldList->schema();
}

void KexiRelationsTableContainer::slotContextMenu(const QPoint &p)
{
  emit gotFocus();  
  emit contextMenuRequest(d->fieldList->mapToGlobal(p));
}

void KexiRelationsTableContainer::moved()
{
// qDebug()<<"finally emitting moved";
    emit moved(this);
}

int KexiRelationsTableContainer::globalY(const QString &field)
{
// qDebug();
    QPoint o(0, d->fieldList->globalY(field) + d->scrollArea->verticalScrollBar()->value()); //d->scrollArea->contentsY());
// qDebug() << "db2";
    return d->scrollArea->widget()->mapFromGlobal(o).y();
}

void KexiRelationsTableContainer::focusInEvent(QFocusEvent* event)
{
    QFrame::focusInEvent(event);
    setFocus();
}

void KexiRelationsTableContainer::setFocus()
{
    //qDebug() << "SET FOCUS";
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
    //qDebug() << "UNSET FOCUS";
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
    qDebug();
    emit fieldsDoubleClicked(*t->schema(), t->selectedFieldNames());
}

QStringList KexiRelationsTableContainer::selectedFieldNames() const
{
    return d->fieldList->selectedFieldNames();
}

