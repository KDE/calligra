/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "BibliographyTypeEntryDelegate.h"

#include <KoOdfBibliographyConfiguration.h>

#include <QComboBox>

BibliographyTypeEntryDelegate::BibliographyTypeEntryDelegate()
{
}

QWidget* BibliographyTypeEntryDelegate::createEditor(QWidget *parent,
                                                     const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *comboBox = new QComboBox(parent);
    return comboBox;
}

void BibliographyTypeEntryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    comboBox->addItems(KoOdfBibliographyConfiguration::bibTypes);

    QString type = index.model()->data(index, Qt::EditRole).toString();
    comboBox->setCurrentIndex(comboBox->findText(type, Qt::MatchFixedString));
}

void BibliographyTypeEntryDelegate::updateEditorGeometry(QWidget *editor,
                                                         const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void BibliographyTypeEntryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}
