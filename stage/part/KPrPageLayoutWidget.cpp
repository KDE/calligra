/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrPageLayoutWidget.h"

#include <QGridLayout>
#include <QListWidget>
#include <QSize>

#include <KLocalizedString>

#include <KoPADocument.h>

#include "KPrPage.h"
#include "KPrView.h"
#include "KPresenter.h"
#include "StageDebug.h"
#include "pagelayout/KPrPageLayout.h"
#include "pagelayout/KPrPageLayouts.h"

// this is needed so it can be used in a QVariant
Q_DECLARE_METATYPE(KPrPageLayout *)

KPrPageLayoutWidget::KPrPageLayoutWidget(QWidget *parent)
    : QWidget(parent)
    , m_view(nullptr)
    , m_previousItem(nullptr)
{
    setWindowTitle(i18n("Layout"));
    setObjectName("Slide Layouts");

    m_layoutsView = new QListWidget();
    m_layoutsView->setIconSize(QSize(80, 60));
    m_layoutsView->setGridSize(QSize(80, 60));
    m_layoutsView->setViewMode(QListView::IconMode);
    m_layoutsView->setResizeMode(QListView::Adjust);
    m_layoutsView->setMovement(QListView::Static);
    m_layoutsView->setSelectionRectVisible(false);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_layoutsView);
    layout->setContentsMargins({});

    setLayout(layout);
}

void KPrPageLayoutWidget::setView(KPrView *view)
{
    Q_ASSERT(view);
    if (m_view) {
        // don't disconnect the m_view->proxyObject as the object is already deleted
        disconnect(m_layoutsView, nullptr, this, nullptr);
    }
    m_view = view;
    connect(m_view->proxyObject, &KoPAViewProxyObject::activePageChanged, this, &KPrPageLayoutWidget::slotActivePageChanged);

    // remove the layouts from the last view
    m_layoutsView->clear();

    KPrPageLayouts *layouts = view->kopaDocument()->resourceManager()->resource(KPresenter::PageLayouts).value<KPrPageLayouts *>();

    Q_ASSERT(layouts);

    const QList<KPrPageLayout *> layoutMap = layouts->layouts();

    // TODO add empty layout

    foreach (KPrPageLayout *layout, layoutMap) {
        if (layout->type() == KPrPageLayout::Page) {
            addLayout(layout);
        }
    }

    slotActivePageChanged();

    connect(m_layoutsView, &QListWidget::itemPressed, this, &KPrPageLayoutWidget::slotItemPressed);
    connect(m_layoutsView, &QListWidget::currentItemChanged, this, &KPrPageLayoutWidget::slotCurrentItemChanged);
}

void KPrPageLayoutWidget::slotActivePageChanged()
{
    Q_ASSERT(m_view);

    KPrPage *page = dynamic_cast<KPrPage *>(m_view->activePage());
    if (page) {
        KPrPageLayout *layout = page->layout();
        QListWidgetItem *item = m_layout2item.value(layout, 0);
        if (item == nullptr && layout != nullptr && layout->type() == KPrPageLayout::Page) {
            item = addLayout(layout);
        }

        if (item) {
            m_layoutsView->blockSignals(true);
            item->setSelected(true);
            m_layoutsView->blockSignals(false);
            m_layoutsView->scrollToItem(item);
        } else {
            QList<QListWidgetItem *> items = m_layoutsView->selectedItems();
            foreach (QListWidgetItem *i, items) {
                m_layoutsView->blockSignals(true);
                i->setSelected(false);
                m_layoutsView->blockSignals(false);
            }
        }
    }
}

void KPrPageLayoutWidget::slotItemPressed(QListWidgetItem *item)
{
    if (item == m_previousItem) {
        applyLayout(item);
    } else {
        m_previousItem = item;
    }
}

void KPrPageLayoutWidget::slotCurrentItemChanged(QListWidgetItem *item, QListWidgetItem *previous)
{
    applyLayout(item);
    m_previousItem = previous;
}

QListWidgetItem *KPrPageLayoutWidget::addLayout(KPrPageLayout *layout)
{
    QListWidgetItem *item = new QListWidgetItem(QIcon(layout->thumbnail()), "", m_layoutsView);
    item->setData(Qt::UserRole, QVariant::fromValue(layout));
    m_layout2item.insert(layout, item);
    return item;
}

void KPrPageLayoutWidget::applyLayout(QListWidgetItem *item)
{
    // don't crash when all items are replaced
    if (item) {
        Q_ASSERT(m_view);
        KPrPage *page = dynamic_cast<KPrPage *>(m_view->activePage());
        if (page) {
            page->setLayout(item->data(Qt::UserRole).value<KPrPageLayout *>(), m_view->kopaDocument());
        }
    }
}
