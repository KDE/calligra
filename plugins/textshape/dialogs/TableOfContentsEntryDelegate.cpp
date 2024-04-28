/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TableOfContentsEntryDelegate.h"

#include <KLocalizedString>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <QComboBox>
#include <QPainter>

TableOfContentsEntryDelegate::TableOfContentsEntryDelegate(KoStyleManager *manager)
    : QStyledItemDelegate()
    , m_styleManager(manager)
{
    Q_ASSERT(manager);
}

QSize TableOfContentsEntryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(250, 48);
}

QWidget *TableOfContentsEntryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex & /* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    return editor;
}

void TableOfContentsEntryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QComboBox *comboBox = static_cast<QComboBox *>(editor);

    QList<KoParagraphStyle *> paragraphStyles = m_styleManager->paragraphStyles();
    int count = 0;
    int indexCount = 0;
    foreach (const KoParagraphStyle *style, paragraphStyles) {
        comboBox->addItem(style->name());
        comboBox->setItemData(count, style->styleId());

        if (style->styleId() == value) {
            indexCount = count;
        }

        count++;
    }

    comboBox->setCurrentIndex(indexCount);
}

void TableOfContentsEntryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    int value = comboBox->itemData(comboBox->currentIndex()).toInt();

    model->setData(index, value, Qt::EditRole);
}

void TableOfContentsEntryDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & /* index */) const
{
    editor->setGeometry(option.rect);
}
