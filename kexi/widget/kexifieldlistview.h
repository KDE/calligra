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

#include <qpixmap.h>
#include <k3listview.h>
#include <kexi_export.h>

class QDrag;
class K3ListViewItem;

namespace KexiDB
{
class TableOrQuerySchema;
}

/*! This widget provides a list of fields from a table or query.
*/
class KEXIEXTWIDGETS_EXPORT KexiFieldListView : public K3ListView
{
    Q_OBJECT

public:
    //! Flags used to alter list's behaviour and appearance
    enum Options {
        ShowDataTypes = 1, //!< if set, 'data type' column is added
        ShowAsterisk = 2, //!< if set, asterisk ('*') item is prepended to the list
        AllowMultiSelection = 4 //!< if set, multiple selection is allowed
    };

    KexiFieldListView(QWidget *parent, int options = ShowDataTypes | AllowMultiSelection);
    virtual ~KexiFieldListView();

    /*! Sets table or query schema \a schema.
     The schema object will be owned by the KexiFieldListView object. */
    void setSchema(KexiDB::TableOrQuerySchema* schema);

    /*! \return table or query schema schema set for this widget. */
    KexiDB::TableOrQuerySchema* schema() const {
        return m_schema;
    }

    /*! \return list of selected field names. */
    QStringList selectedFieldNames() const;

//  void setReadOnly(bool);
//  virtual QSize sizeHint();

signals:
    /*! Emitted when a field is double clicked */
    void fieldDoubleClicked(const QString& sourcePartClass, const QString& sourceName,
                            const QString& fieldName);

protected slots:
    void slotDoubleClicked(Q3ListViewItem* item);

protected:
    //virtual QDrag *dragObject();

    KexiDB::TableOrQuerySchema* m_schema;
    QPixmap m_keyIcon; //!< a small "primary key" icon for 0-th column
    QPixmap m_noIcon; //!< blank icon of the same size as m_keyIcon
    int m_options;
    K3ListViewItem *m_allColumnsItem;
};

#endif
