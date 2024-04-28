/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoSopranoTableModelDelegate_h__
#define __rdf_KoSopranoTableModelDelegate_h__

#include <QItemDelegate>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QWidget>

/**
 * @short The Object-Type column really needs to be restricted to only URI, Literal etc.
 * @author Ben Martin <ben.martin@kogmbh.com>
 */
class KoSopranoTableModelDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit KoSopranoTableModelDelegate(QObject *parent);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private Q_SLOTS:
    void emitCommitData();
};

#endif
