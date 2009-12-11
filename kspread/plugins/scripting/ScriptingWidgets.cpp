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

#include <QMetaEnum>
#include <QVBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <klocale.h>
#include <kdebug.h>

#include <part/View.h>
#include <part/Doc.h>
#include <Map.h>
#include <Sheet.h>
#include <Cell.h>
#include <Region.h>

ScriptingSheetsListView::ScriptingSheetsListView(ScriptingModule* module, QWidget* parent)
        : QWidget(parent), m_module(module), m_initialized(false), m_selectiontype(SingleSelect), m_editortype(Disabled)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
    m_view = new QTreeView(this);
    //m_view->setAlternatingRowColors(true);
    m_view->setRootIsDecorated(false);
    m_view->setSortingEnabled(false);
    m_view->setItemsExpandable(false);
    m_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    m_view->setModel(new QStandardItemModel(this));
    layout->addWidget(m_view);
}

ScriptingSheetsListView::~ScriptingSheetsListView()
{
}

void ScriptingSheetsListView::setSelectionType(const QString& selectiontype)
{
    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("SelectionType"));
    int v = e.keysToValue(selectiontype.toUtf8());
    if (v >= 0) m_selectiontype = (SelectionType) v;
}

void ScriptingSheetsListView::setEditorType(const QString& editortype)
{
    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("EditorType"));
    int v = e.keysToValue(editortype.toUtf8());
    if (v >= 0) m_editortype = (EditorType) v;
}

void ScriptingSheetsListView::initialize()
{
    if (m_initialized)
        finalize();

    kDebug() << "ScriptingSheetsListView::initialize()";

    QStringList headers;
    headers << i18n("Sheet");
    switch (m_editortype) {
    case Disabled: break;
    case Cell: headers << i18n("Cell"); break;
    case Range: headers << i18n("Range"); break;
    }

    QStandardItemModel* model = static_cast< QStandardItemModel* >(m_view->model());
    model->setHorizontalHeaderLabels(headers);
    KSpread::Doc* doc = m_module->kspreadDoc();
    KSpread::View* view = m_module->kspreadView();
    KSpread::Sheet* activeSheet = view ? view->activeSheet() : 0;
    if (doc && doc->map()) {
        foreach(KSpread::Sheet* sheet, doc->map()->sheetList()) {
            if (! sheet || sheet->isHidden())
                continue;
            QRect area = sheet->usedArea();
            bool enabled = area.isValid();
            Q_UNUSED(enabled);
            QList< QStandardItem* > items;

            QStandardItem* nameitem = new QStandardItem(sheet->sheetName());
            nameitem->setEditable(false);
            if (m_selectiontype == MultiSelect) {
                nameitem->setCheckable(true);
                nameitem->setCheckState((activeSheet == sheet) ? Qt::Checked : Qt::Unchecked);
            }
            items << nameitem;

            if (m_editortype != Disabled) {
                QString range;
                foreach(QVariant v, m_prevlist) {
                    QVariantList l = v.toList();
                    if (l.count() < 1 || l[0].toString() != sheet->sheetName())
                        continue;
                    if (l.count() >= 2)
                        if (m_selectiontype == MultiSelect)
                            nameitem->setCheckState(l[1].toBool() ? Qt::Checked : Qt::Unchecked);
                    if (l.count() >= 3) {
                        QStringList rangelist;
                        for (int i = 2; i < l.count(); ++i) {
                            const QRect rect = l[i].toRect();
                            if (rect.isNull())
                                continue;
                            KSpread::Region region(rect, sheet);
                            for (KSpread::Region::ConstIterator it = region.constBegin(); it != region.constEnd(); ++it) {
                                const QString n = (*it)->name(sheet);
                                if (! n.isEmpty())
                                    rangelist.append(n);
                            }
                        }
                        range = rangelist.join(";");
                    }
                    break;
                }
                if (range.isEmpty() && area.isValid())
                    range = KSpread::Region(area, sheet).name(sheet);
                if (m_editortype == Cell) {
                    int p = range.indexOf(':');
                    range = p > 0 ? range.left(p) : "A1";
                }
                items << new QStandardItem(range);
            }

            model->appendRow(items);
            if (activeSheet == sheet)
                m_view->setCurrentIndex(nameitem->index());
        }
    }

    m_initialized = true;
}

void ScriptingSheetsListView::finalize()
{
    if (m_initialized) {
        kDebug() << "ScriptingSheetsListView::finalize()";

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

QString ScriptingSheetsListView::sheet()
{
    if (! m_initialized)
        initialize();
    QStandardItemModel* model = static_cast< QStandardItemModel* >(m_view->model());
    QStandardItem* current = model->itemFromIndex(m_view->currentIndex());
    QStandardItem* nameitem = current ? model->item(current->row(), 0) : 0;
    return nameitem ? nameitem->text() : QString();
}

QString ScriptingSheetsListView::editor()
{
    if (! m_initialized)
        initialize();
    QStandardItemModel* model = static_cast< QStandardItemModel* >(m_view->model());
    QStandardItem* current = model->itemFromIndex(m_view->currentIndex());
    QStandardItem* rangeitem = current ? model->item(current->row(), 1) : 0;
    return rangeitem ? rangeitem->text() : QString();
}

QVariantList ScriptingSheetsListView::sheets()
{
    if (! m_initialized)
        initialize();

    QVariantList list;
    QStandardItemModel* model = static_cast< QStandardItemModel* >(m_view->model());
    const int rowcount = model->rowCount();
    for (int row = 0; row < rowcount; ++row) {
        QStandardItem* nameitem = model->item(row, 0);
        if (! nameitem)
            continue;

        bool enabled = nameitem->checkState() == Qt::Checked;

        const QString sheetname = nameitem->text();
        KSpread::Sheet* sheet = m_module->kspreadDoc()->map()->findSheet(sheetname);
        if (! sheet)
            continue;

        QVariantList l;
        l << sheetname << enabled;

        QStandardItem* rangeitem = model->item(row, 1);
        if (rangeitem) {
            const QString range = rangeitem->text();
            KSpread::Region region(range, m_module->kspreadDoc()->map(), sheet);
            for (KSpread::Region::ConstIterator it = region.constBegin(); it != region.constEnd(); ++it) {
                const QRect rect = (*it)->rect();
                if (! rect.isNull())
                    l << rect;
            }
        }

        list.append(l);
    }
    return list;
}

#include "ScriptingWidgets.moc"
