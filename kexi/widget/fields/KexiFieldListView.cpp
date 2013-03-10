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

#include "KexiFieldListView.h"

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QCursor>
#include <QPoint>
#include <QApplication>
#include <QBitmap>
#include <QStyle>
//Added by qt3to4:
#include <QPixmap>

#include <kdebug.h>

#include <kconfig.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include <db/tableschema.h>
#include <db/queryschema.h>
#include <db/utils.h>
#include <kexidragobjects.h>
#include <kexiutils/utils.h>

class KexiFieldListView::Private
{
public:
    Private(KexiFieldListOptions options_)
      : schema(0)
      , model(0)
      , options(options_)
    {
    }

    ~Private()
    {
        delete schema;
    }


    KexiDB::TableOrQuerySchema* schema;
    KexiFieldListModel *model;
    KexiFieldListOptions options;
};

KexiFieldListView::KexiFieldListView(QWidget *parent, KexiFieldListOptions options)
        : QListView(parent)
        , d(new Private(options))
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAlternatingRowColors(true);

/*    setDropVisualizer(false);
    setDropHighlighter(true);
    setAllColumnsShowFocus(true);
    addColumn(i18n("Field Name"));
    if (d->options & ShowDataTypes)
        addColumn(i18n("Data Type"));
    if (d->options & AllowMultiSelection)
        setSelectionMode(Q3ListView::Extended);
    setResizeMode(Q3ListView::LastColumn);
// header()->hide();
    setSorting(-1, true); // disable sorting
*/
    connect(this, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(slotDoubleClicked(QModelIndex)));
}

KexiFieldListView::~KexiFieldListView()
{
    delete d;
}

void KexiFieldListView::setSchema(KexiDB::TableOrQuerySchema* schema)
{
    if (schema && d->schema == schema)
        return;

    delete d->schema;
    d->schema = schema;
    if (!d->schema)
        return;

    if (!schema->table() && !schema->query())
        return;

    delete d->model;

    d->model = new KexiFieldListModel(this, d->options);

    d->model->setSchema(schema);
    setModel(d->model);
}

KexiDB::TableOrQuerySchema* KexiFieldListView::schema() const {
    return d->schema;
}

QStringList KexiFieldListView::selectedFieldNames() const
{
    if (!schema())
        return QStringList();

    QStringList selectedFields;
    QModelIndexList idxlist = selectedIndexes();

    foreach (const QModelIndex &idx, idxlist) {
        QString field = model()->data(idx).toString();
        if (field.startsWith("*")) {
            selectedFields.append("*");
        }
        else {
            selectedFields.append(field);
        }
    }
    
    return selectedFields;

}

void KexiFieldListView::slotDoubleClicked(const QModelIndex &idx)
{
    kDebug();
    if (schema() && idx.isValid()) {
        //! @todo what about query fields/aliases? it.current()->text(0) can be not enough
        emit fieldDoubleClicked(schema()->table() ? "kexi/table" : "kexi/query",
                                schema()->name(), model()->data(idx).toString());
    }
}

#include "KexiFieldListView.moc"
