/* This file is part of the KDE project
   Copyright (C) 2005-2012 Jarosław Staniek <staniek@kde.org>
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
    case Qt::EditRole:
    case Qt::DisplayRole:
        if (index.row() == 0) {
            if (!d->columnNames[index.column()].isEmpty() && d->firstRowForFieldNames) {
                return d->columnNames[index.column()];
            } else {
                return i18n("Column %1", index.column() + 1);
            }
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
            return QString(i18nc("@title:row", "Column name") + "  ");
        } else {
            return QString::number(section);
        }
    }
    return value;
}

bool KexiCSVImportDialogModel::setData(const QModelIndex &index, const QVariant &value,
                                       int role)
{
    if (index.row() == 0 && d->firstRowForFieldNames && role == Qt::EditRole) {
        d->columnNames[index.column()] = value.toString();
        return true;
    }
    return QStandardItemModel::setData(index, value, role);
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
    d->firstRowForFieldNames = flag;
}
