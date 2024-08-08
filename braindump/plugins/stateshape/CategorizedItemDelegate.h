/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KIS_CATEGORIZED_ITEM_DELEGATE_H_
#define _KIS_CATEGORIZED_ITEM_DELEGATE_H_

#include <QAbstractItemDelegate>

/**
 * This delegate draw categories using information from a KCategorizedSortFilterProxyModel .
 */
class CategorizedItemDelegate : public QAbstractItemDelegate
{
public:
    /**
     * The @p _fallback delegate is used to take care of drawing/editing of the items.
     */
    explicit CategorizedItemDelegate(QAbstractItemDelegate *_fallback, QObject *parent = nullptr);
    ~CategorizedItemDelegate();
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    struct Private;
    Private *const d;
};

#endif
