/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoSopranoTableModelDelegate.h"

// lib
#include "KoDocumentRdf.h"
#include "KoSopranoTableModel.h"
// main
#include <KoDocument.h>
// KF5
#include <KLocalizedString>
// Qt
#include <QComboBox>

KoSopranoTableModelDelegate::KoSopranoTableModelDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *KoSopranoTableModelDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *comboBox = new QComboBox(parent);
    if (index.column() == KoSopranoTableModel::ColObjType) {
        comboBox->addItem(i18n("URI"));
        comboBox->addItem(i18n("Literal"));
        comboBox->addItem(i18n("Blank"));
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
    return comboBox;
}

void KoSopranoTableModelDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) {
        return QStyledItemDelegate::setEditorData(editor, index);
    }
    int pos = comboBox->findText(index.model()->data(index).toString(), Qt::MatchExactly);
    comboBox->setCurrentIndex(pos);
}

void KoSopranoTableModelDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) {
        return QStyledItemDelegate::setModelData(editor, model, index);
    }
    model->setData(index, comboBox->currentText());
}

void KoSopranoTableModelDelegate::emitCommitData()
{
    Q_EMIT commitData(qobject_cast<QWidget *>(sender()));
}
