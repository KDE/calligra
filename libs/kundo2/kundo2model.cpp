/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Matus Talcik <matus.talcik@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
/****************************************************************************
**
** SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "kundo2model.h"
#include <KLocalizedString>
KUndo2Model::KUndo2Model(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_stack = nullptr;
    m_sel_model = new QItemSelectionModel(this, this);
    connect(m_sel_model, &QItemSelectionModel::currentChanged, this, &KUndo2Model::setStackCurrentIndex);
    m_emty_label = i18n("<empty>");
}

QItemSelectionModel *KUndo2Model::selectionModel() const
{
    return m_sel_model;
}

KUndo2QStack *KUndo2Model::stack() const
{
    return m_stack;
}

void KUndo2Model::setStack(KUndo2QStack *stack)
{
    if (m_stack == stack)
        return;

    if (m_stack != nullptr) {
        disconnect(m_stack, &KUndo2QStack::cleanChanged, this, &KUndo2Model::stackChanged);
        disconnect(m_stack, &KUndo2QStack::indexChanged, this, &KUndo2Model::stackChanged);
        disconnect(m_stack, &QObject::destroyed, this, &KUndo2Model::stackDestroyed);
    }
    m_stack = stack;
    if (m_stack != nullptr) {
        connect(m_stack, &KUndo2QStack::cleanChanged, this, &KUndo2Model::stackChanged);
        connect(m_stack, &KUndo2QStack::indexChanged, this, &KUndo2Model::stackChanged);
        connect(m_stack, &QObject::destroyed, this, &KUndo2Model::stackDestroyed);
    }

    stackChanged();
}

void KUndo2Model::stackDestroyed(QObject *obj)
{
    if (obj != m_stack)
        return;
    m_stack = nullptr;

    stackChanged();
}

void KUndo2Model::stackChanged()
{
    beginResetModel();
    endResetModel(); // @todo: this call may be moved after the setCurrentIndex()
    m_sel_model->setCurrentIndex(selectedIndex(), QItemSelectionModel::ClearAndSelect);
}

void KUndo2Model::setStackCurrentIndex(const QModelIndex &index)
{
    if (m_stack == nullptr)
        return;

    if (index == selectedIndex())
        return;

    if (index.column() != 0)
        return;

    m_stack->setIndex(index.row());
}

QModelIndex KUndo2Model::selectedIndex() const
{
    return m_stack == nullptr ? QModelIndex() : createIndex(m_stack->index(), 0);
}

QModelIndex KUndo2Model::index(int row, int column, const QModelIndex &parent) const
{
    if (m_stack == nullptr)
        return QModelIndex();

    if (parent.isValid())
        return QModelIndex();

    if (column != 0)
        return QModelIndex();

    if (row < 0 || row > m_stack->count())
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex KUndo2Model::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int KUndo2Model::rowCount(const QModelIndex &parent) const
{
    if (m_stack == nullptr)
        return 0;

    if (parent.isValid())
        return 0;

    return m_stack->count() + 1;
}

int KUndo2Model::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant KUndo2Model::data(const QModelIndex &index, int role) const
{
    if (m_stack == nullptr)
        return QVariant();

    if (index.column() != 0)
        return QVariant();

    if (index.row() < 0 || index.row() > m_stack->count())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.row() == 0)
            return m_emty_label;
        return m_stack->text(index.row() - 1);
    } else if (role == Qt::DecorationRole) {
        if (index.row() == m_stack->cleanIndex() && !m_clean_icon.isNull())
            return m_clean_icon;
    }

    return QVariant();
}

QString KUndo2Model::emptyLabel() const
{
    return m_emty_label;
}

void KUndo2Model::setEmptyLabel(const QString &label)
{
    m_emty_label = label;
    stackChanged();
}

void KUndo2Model::setCleanIcon(const QIcon &icon)
{
    m_clean_icon = icon;
    stackChanged();
}

QIcon KUndo2Model::cleanIcon() const
{
    return m_clean_icon;
}
