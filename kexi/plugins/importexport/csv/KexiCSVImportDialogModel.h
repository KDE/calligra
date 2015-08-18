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

#ifndef KEXICSVIMPORTDIALOGMODEL_H
#define KEXICSVIMPORTDIALOGMODEL_H

#include <QStandardItemModel>

//! Model for displaying data of CSV import dialog.
//! @see KexiCSVImportDialog
class KexiCSVImportDialogModel : public QStandardItemModel
{
public:
    explicit KexiCSVImportDialogModel(QObject *parent);

    virtual ~KexiCSVImportDialogModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setColumnCount(int col);

    bool firstRowForFieldNames() const;

    void setFirstRowForFieldNames(bool flag);

private:
    class Private;
    Private * const d;
};

#endif
