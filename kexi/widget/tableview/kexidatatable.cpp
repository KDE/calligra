/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2012 Jarosław Staniek <staniek@kde.org>

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


#include <QLayout>
#include <QLabel>

#include <klocale.h>
#include <kaction.h>
#include <kmenu.h>

#include <db/cursor.h>
#include <db/utils.h>

#include "kexidatatableview.h"
#include "kexidatatable.h"
#include <widget/dataviewcommon/kexitableviewdata.h>
#include <core/KexiWindow.h>
#include <core/kexiproject.h>
#include <core/KexiMainWindowIface.h>

class KexiDataTable::Private
{
public:
    bool storeUserDataBlock(int objectID, const QString& dataID, const QString &dataString,
                            KexiDB::TransactionGuard *tg)
    {
        if (transaction.isNull()) {
            transaction = KexiMainWindowIface::global()->project()->dbConnection()->beginTransaction();
            tg->setTransaction(transaction);
        }
        return KexiMainWindowIface::global()->project()->storeUserDataBlock(
            objectID, dataID, dataString);
    }

    KexiDB::Transaction transaction;
};

KexiDataTable::KexiDataTable(QWidget *parent, bool dbAware)
        : KexiDataAwareView(parent)
        , d(new Private)
{
    KexiTableView *view;
    if (dbAware)
        view = new KexiDataTableView(this);
    else
        view = new KexiTableView(0, this);
    view->setObjectName("datatableview");

    KexiDataAwareView::init(view, view, view);
}

KexiDataTable::KexiDataTable(QWidget *parent, KexiDB::Cursor *cursor)
        : KexiDataAwareView(parent)
        , d(new Private)
{
    KexiTableView *view = new KexiDataTableView(this, cursor);
    KexiDataAwareView::init(view, view, view);
}

KexiDataTable::~KexiDataTable()
{
    delete d;
}

bool KexiDataTable::loadTableViewSettings(KexiTableViewData* data)
{
    Q_ASSERT(data);
    const int id = window()->id();
    if (id > 0 && data->columnsCount() > 0) {
        QString columnWidthsString;
        tristate res = KexiMainWindowIface::global()->project()->loadUserDataBlock(
                id, "columnWidths", &columnWidthsString);
        if (false == res) {
            return false;
        }
        else if (true == res) {
            bool ok;
            const QList<int> columnWidths = KexiDB::deserializeIntList(columnWidthsString, &ok);
            if (!ok) {
                kWarning() << "Invalud format of 'columnWidths' value:" << columnWidthsString;
                return false;
            }
            KexiTableViewColumn::List* columns = data->columns();
            if (columnWidths.count() == columns->count()) {
                int i = 0;
                foreach (int width, columnWidths) {
                    // kDebug() << width;
                    columns->at(i)->setWidth(width);
                    ++i;
                }
            }
        }
    }
    return true;
}

void
KexiDataTable::setData(KexiDB::Cursor *c)
{
    if (!dynamic_cast<KexiDataTableView*>(mainWidget()))
        return;
    dynamic_cast<KexiDataTableView*>(mainWidget())->setData(c);
}

void KexiDataTable::filter()
{
}

KexiTableView* KexiDataTable::tableView() const
{
  return dynamic_cast<KexiTableView*>(internalView());
}

bool KexiDataTable::saveSettings()
{
#ifdef __GNUC__
#warning TODO save only if changed
#else
#pragma WARNING(TODO save only if changed)
#endif
    bool ok = true;
    KexiDB::TransactionGuard tg;
    if (dynamic_cast<KexiDataTableView*>(mainWidget())) { // db-aware
        KexiTableView* tv = tableView();
        const int id = window()->id();
        if (id > 0 && tv->data()->columnsCount() > 0) {
            QStringList widths;
            bool equal = true; // will be only saved if widths are not equal
            for (uint i = 0; i < tv->data()->columnsCount(); ++i) {
                if (equal) {
                    equal = tv->data()->column(i)->width() == uint(tv->columnWidth(i));
                }
                widths.append(QString::number(tv->columnWidth(i)));
            }
            if (   !equal
                && !d->storeUserDataBlock(id, "columnWidths", KexiDB::variantToString(widths), &tg))
            {
                return false;
            }
        }
        ok = tg.commit();
    }
    return ok;
}

#include "kexidatatable.moc"
