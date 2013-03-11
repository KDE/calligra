/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIFIELDLISTVIEW_H
#define KEXIFIELDLISTVIEW_H

#include <QPixmap>
#include <QListView>
#include <kexi_export.h>
#include <kexi.h>
#include "KexiFieldListModel.h"

namespace KexiDB
{
class TableOrQuerySchema;
}

/*! This widget provides a list of fields from a table or query.
*/
class KEXIEXTWIDGETS_EXPORT KexiFieldListView : public QListView
{
    Q_OBJECT

public:

    KexiFieldListView(QWidget *parent, KexiFieldListOptions options);
    virtual ~KexiFieldListView();

    /*! Sets table or query schema \a schema.
     The schema object will be owned by the KexiFieldListView object. */
    void setSchema(KexiDB::TableOrQuerySchema* schema);

    /*! \return table or query schema schema set for this widget. */
    KexiDB::TableOrQuerySchema* schema() const;

    /*! \return list of selected field names. */
    QStringList selectedFieldNames() const;

signals:
    /*! Emitted when a field is double clicked */
    void fieldDoubleClicked(const QString& sourcePartClass, const QString& sourceName,
                            const QString& fieldName);

protected slots:
    void slotDoubleClicked(const QModelIndex &idx);

protected:

private:
    class Private;
    Private * const d;
};

#endif
