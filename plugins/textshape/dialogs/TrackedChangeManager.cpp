/* This file is part of the KDE project
* SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TrackedChangeManager.h"

#include "TrackedChangeModel.h"

#include <klocalizedstring.h>

#include <QModelIndex>
#include <QTreeView>

#include <QDebug>

TrackedChangeManager::TrackedChangeManager(QWidget* parent): QWidget(parent),
    m_model(0)
{
    widget.setupUi(this);
}

TrackedChangeManager::~TrackedChangeManager()
{
}

void TrackedChangeManager::setModel(TrackedChangeModel* model)
{
    m_model = model;
    widget.treeView->setModel(m_model);
    widget.treeView->reset();
    connect(widget.treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, QOverload<const QModelIndex&, const QModelIndex&>::of(&TrackedChangeManager::slotCurrentChanged));
}

void TrackedChangeManager::slotCurrentChanged(const QModelIndex &newIndex, const QModelIndex &previousIndex)
{
    Q_UNUSED(previousIndex);
    emit currentChanged(newIndex);
}

void TrackedChangeManager::selectItem(const QModelIndex &newIndex)
{
    QModelIndex currentIndex = widget.treeView->currentIndex();
    widget.treeView->setCurrentIndex(newIndex);
    slotCurrentChanged(newIndex, currentIndex);
}
