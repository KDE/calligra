/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiDataTableView.h"
#include "KexiDataTableScrollArea.h"
#include <core/KexiWindow.h>
#include <core/kexiproject.h>
#include <core/KexiMainWindowIface.h>

#include <KDb>
#include <KDbCursor>
#include <KDbTableViewData>

#include <QDebug>

class KexiDataTableView::Private
{
public:
    bool storeUserDataBlock(int objectID, const QString& dataID, const QString &dataString,
                            KDbTransactionGuard *tg)
    {
        if (transaction.isNull()) {
            transaction = KexiMainWindowIface::global()->project()->dbConnection()->beginTransaction();
            tg->setTransaction(transaction);
        }
        return KexiMainWindowIface::global()->project()->storeUserDataBlock(
            objectID, dataID, dataString);
    }

    KDbTransaction transaction;
};

KexiDataTableView::KexiDataTableView(QWidget *parent, bool dbAware)
        : KexiDataAwareView(parent)
        , d(new Private)
{
    KexiTableScrollArea *view;
    if (dbAware)
        view = new KexiDataTableScrollArea(this);
    else
        view = new KexiTableScrollArea(0, this);
    view->setObjectName("datatableview");

    KexiDataAwareView::init(view, view, view);
}

KexiDataTableView::KexiDataTableView(QWidget *parent, KDbCursor *cursor)
        : KexiDataAwareView(parent)
        , d(new Private)
{
    KexiTableScrollArea *view = new KexiDataTableScrollArea(this, cursor);
    KexiDataAwareView::init(view, view, view);
}

KexiDataTableView::~KexiDataTableView()
{
    delete d;
}

bool KexiDataTableView::loadTableViewSettings(KDbTableViewData* data)
{
    Q_ASSERT(data);
    const int id = window()->id();
    if (id > 0 && data->columnCount() > 0) {
        QString columnWidthsString;
        tristate res = KexiMainWindowIface::global()->project()->loadUserDataBlock(
                id, "columnWidths", &columnWidthsString);
        if (false == res) {
            return false;
        }
        else if (true == res) {
            bool ok;
            const QList<int> columnWidths = KDb::deserializeIntList(columnWidthsString, &ok);
            if (!ok) {
                qWarning() << "Invalud format of 'columnWidths' value:" << columnWidthsString;
                return false;
            }
            QList<KDbTableViewColumn*>* columns = data->columns();
            if (columnWidths.count() == columns->count()) {
                int i = 0;
                foreach (int width, columnWidths) {
                    // qDebug() << width;
                    columns->at(i)->setWidth(width);
                    ++i;
                }
            }
        }
    }
    return true;
}

void
KexiDataTableView::setData(KDbCursor *c)
{
    if (!dynamic_cast<KexiDataTableScrollArea*>(mainWidget()))
        return;
    dynamic_cast<KexiDataTableScrollArea*>(mainWidget())->setData(c);
}

void KexiDataTableView::filter()
{
}

KexiTableScrollArea* KexiDataTableView::tableView() const
{
  return dynamic_cast<KexiTableScrollArea*>(internalView());
}

bool KexiDataTableView::saveSettings()
{
//! @todo KEXI3 save only if changed
    bool ok = true;
    KDbTransactionGuard tg;
    if (dynamic_cast<KexiDataTableScrollArea*>(mainWidget())) { // db-aware
        KexiTableScrollArea* tv = tableView();
        const int id = window()->id();
        if (id > 0 && tv->data()->columnCount() > 0) {
            QStringList widths;
            bool equal = true; // will be only saved if widths are not equal
            for (int i = 0; i < tv->data()->columnCount(); ++i) {
                if (equal) {
                    equal = tv->data()->column(i)->width() == tv->columnWidth(i);
                }
                widths.append(QString::number(tv->columnWidth(i)));
            }
            if (   !equal
                && !d->storeUserDataBlock(id, "columnWidths", KDb::variantToString(widths), &tg))
            {
                return false;
            }
        }
        ok = tg.commit();
    }
    return ok;
}

