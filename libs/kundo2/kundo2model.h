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
#ifndef K_UNDO_2_MODEL
#define K_UNDO_2_MODEL
#include <QAbstractItemModel>

#include "kundo2stack.h"
#include <QIcon>
#include <QItemSelectionModel>

class KUndo2Model : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit KUndo2Model(QObject *parent = nullptr);

    KUndo2QStack *stack() const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex selectedIndex() const;
    QItemSelectionModel *selectionModel() const;

    QString emptyLabel() const;
    void setEmptyLabel(const QString &label);

    void setCleanIcon(const QIcon &icon);
    QIcon cleanIcon() const;

public Q_SLOTS:
    void setStack(KUndo2QStack *stack);

private Q_SLOTS:
    void stackChanged();
    void stackDestroyed(QObject *obj);
    void setStackCurrentIndex(const QModelIndex &index);

private:
    KUndo2QStack *m_stack;
    QItemSelectionModel *m_sel_model;
    QString m_emty_label;
    QIcon m_clean_icon;
};
#endif
