/*
 * This file is part of KSpread
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ScriptingWidgets.h"
#include "ScriptingModule.h"

#include <QVBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <klocale.h>
#include <kdebug.h>

#include <Doc.h>
#include <Map.h>
#include <Sheet.h>
#include <Cell.h>
#include <Region.h>

ScriptingSheetsListView::ScriptingSheetsListView(ScriptingModule* module, QWidget* parent)
    : QWidget(parent), m_module(module), m_initialized(false)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
    m_view = new QTreeView(this);
    m_view->setAlternatingRowColors(true);
    m_view->setRootIsDecorated(false);
    m_view->setSortingEnabled(false);
    m_view->setItemsExpandable(false);
    m_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    m_view->setModel( new QStandardItemModel(this) );
    layout->addWidget(m_view);
}

ScriptingSheetsListView::~ScriptingSheetsListView()
{
}

void ScriptingSheetsListView::initialize()
{
    if( m_initialized )
        finalize();

    kDebug()<<"ScriptingSheetsListView::initialize()"<<endl;

    QStandardItemModel* model = static_cast< QStandardItemModel* >(m_view->model());
    model->setHorizontalHeaderLabels( QStringList() << i18n("Sheet") << i18n("Range") );
    KSpread::Doc* doc = m_module->doc();
    if( doc && doc->map() ) {
        foreach(KSpread::Sheet* sheet, doc->map()->sheetList()) {
            KSpread::Cell* cell = sheet->firstCell();

            bool enabled = cell && ! sheet->isHidden();
            QString range;
            foreach(QVariant v, m_prevlist) {
                QVariantList l = v.toList();
                if( l.count() < 1 || l[0].toString() != sheet->sheetName() )
                    continue;
                if( l.count() >= 2 )
                    enabled = l[1].toBool();
                if( l.count() >= 3 ) {
                    QStringList rangelist;
                    for(int i = 2; i < l.count(); ++i) {
                        const QRect rect = l[i].toRect();
                        if( rect.isNull() )
                            continue;
                        KSpread::Region region(rect, sheet);
                        for(KSpread::Region::ConstIterator it = region.constBegin(); it != region.constEnd(); ++it) {
                            const QString n = (*it)->name(sheet);
                            if( ! n.isEmpty() )
                                rangelist.append(n);
                        }
                    }
                    range = rangelist.join(";");
                }
                break;
            }

            QStandardItem* nameitem = new QStandardItem( sheet->sheetName() );
            nameitem->setCheckable(true);
            nameitem->setEditable(false);
            nameitem->setCheckState( enabled ? Qt::Checked : Qt::Unchecked );

            if( range.isEmpty() && cell )
                range = QString("A1:%1").arg(cell->name());

            model->appendRow( QList< QStandardItem* >() << nameitem << new QStandardItem(range) );
        }
    }

    m_initialized = true;
}

void ScriptingSheetsListView::finalize()
{
    if( m_initialized ) {
        kDebug()<<"ScriptingSheetsListView::finalize()"<<endl;

        m_prevlist = sheets();
        QStandardItemModel* model = static_cast< QStandardItemModel* >(m_view->model());
        model->clear();
        m_initialized = false;
    }
}

void ScriptingSheetsListView::showEvent(QShowEvent* event)
{
    finalize();
    QWidget::showEvent(event);
    initialize();
}

QVariantList ScriptingSheetsListView::sheets()
{
    if( ! m_initialized )
        initialize();

    QVariantList list;
    QStandardItemModel* model = static_cast< QStandardItemModel* >(m_view->model());
    const int rowcount = model->rowCount();
    for(int row = 0; row < rowcount; ++row) {
        QStandardItem* nameitem = model->item(row,0);
        if( ! nameitem )
            continue;

        bool enabled = nameitem->checkState() == Qt::Checked;

        const QString sheetname = nameitem->text();
        KSpread::Sheet* sheet = m_module->doc()->map()->findSheet(sheetname);
        if( ! sheet )
            continue;

        QVariantList l;
        l << sheetname << enabled;

        QStandardItem* rangeitem = model->item(row,1);
        Q_ASSERT(rangeitem);
        const QString range = rangeitem->text();
        KSpread::Region region(m_module->doc()->map(), range, sheet);
        for(KSpread::Region::ConstIterator it = region.constBegin(); it != region.constEnd(); ++it) {
            const QRect rect = (*it)->rect();
            if( ! rect.isNull() )
                l << rect;
        }

        list.append(l);
    }
    return list;
}

#include "ScriptingWidgets.moc"
