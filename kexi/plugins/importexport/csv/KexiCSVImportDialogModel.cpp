/* This file is part of the KDE project
   Copyright (C) 2005-2015 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Oleg Kukharchuk <oleg.kuh@gmail.com>

   This work is based on kspread/dialogs/kspread_dlg_csv.cc
   and will be merged back with Calligra Libraries.

   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2002 Laurent Montel <montel@kde.org>
   Copyright (C) 1999 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiCSVImportDialogModel.h"

#include <KLocale>

#include <QVector>

class KexiCSVImportDialogModel::Private
{
public:
    Private()
     : firstRowForFieldNames(true)
    {
    }
    bool firstRowForFieldNames;
    QVector<QString> columnNames;
};

KexiCSVImportDialogModel::KexiCSVImportDialogModel(QObject *parent)
    : QStandardItemModel(parent)
    , d(new Private)
{
}

KexiCSVImportDialogModel::~KexiCSVImportDialogModel()
{
    delete d;
}

QVariant KexiCSVImportDialogModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QStandardItemModel::data(index, role);

    switch (role) {
    case Qt::FontRole:
        if (index.row() == 0) {
            QFont f(value.value<QFont>());
            f.setBold(true);
            return qVariantFromValue(f);
        }
    }
    return value;
}

QVariant KexiCSVImportDialogModel::headerData(int section, Qt::Orientation orientation,
                                              int role) const
{
    QVariant value = QStandardItemModel::headerData(section, orientation, role);
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        if (section == 0) {
            return xi18nc("@title:row (with two spaces at the end)", "Column name  ");
        } else {
            return QString::number(section);
        }
    }
    return value;
}

Qt::ItemFlags KexiCSVImportDialogModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QStandardItemModel::flags(index);
    f |= Qt::ItemIsEditable;
    if (index.row() > 0) {
        f ^= Qt::ItemIsEditable;
    }
    return f;
}

void KexiCSVImportDialogModel::setColumnCount(int col)
{
    if (d->columnNames.size()<col) {
        d->columnNames.resize(col);
    }
    QStandardItemModel::setColumnCount(col);
}

bool KexiCSVImportDialogModel::firstRowForFieldNames() const
{
    return d->firstRowForFieldNames;
}

void KexiCSVImportDialogModel::setFirstRowForFieldNames(bool flag)
{
    if (!flag) {
        d->firstRowForFieldNames = flag;
        for (int i = 0; i < columnCount(); ++i) {
            setData(index(0, i),
                    xi18nc("@title:column Column 1, Column 2, etc.", "Column %1", i + 1),
                    Qt::EditRole);
        }
    }
}
