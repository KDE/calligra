/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLEOFCONTENTSENTRYDELEGATE_H
#define TABLEOFCONTENTSENTRYDELEGATE_H

#include <QStyledItemDelegate>

class KoStyleManager;

class TableOfContentsEntryDelegate : public QStyledItemDelegate
{
public:
    explicit TableOfContentsEntryDelegate(KoStyleManager *manager);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    KoStyleManager *m_styleManager;
};

#endif // TABLEOFCONTENTSENTRYDELEGATE_H
