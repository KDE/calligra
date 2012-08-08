/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexidatatableview.h"
#include "kexidatatable.h"

KexiDataTable::KexiDataTable(QWidget *parent, bool dbAware)
        : KexiDataAwareView(parent)
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
{
    KexiTableView *view = new KexiDataTableView(this, cursor);
    KexiDataAwareView::init(view, view, view);
}

KexiDataTable::~KexiDataTable()
{
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
    return dynamic_cast<KexiTableView*>(m_internalView);
}

#include "kexidatatable.moc"
