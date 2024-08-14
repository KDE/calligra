/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2005, 2007-2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWStartupWidget.h"

#include "../KWDocument.h"
#include "../KWPage.h"
#include "KWDocumentColumns.h"

#include <KoPageLayoutWidget.h>
#include <KoPagePreviewWidget.h>
#include <QTabBar>

KWStartupWidget::KWStartupWidget(QWidget *parent, KWDocument *doc, const KoColumns &columns)
    : QWidget(parent)
    , m_unit(doc->unit())
{
    widget.setupUi(this);
    widget.createButtonLayout->insertStretch(0);
    widget.tabs->tabBar()->setExpanding(true);

    m_columns = columns;
    m_layout.leftMargin = MM_TO_POINT(30);
    m_layout.rightMargin = MM_TO_POINT(30);
    m_layout.topMargin = MM_TO_POINT(25);
    m_layout.bottomMargin = MM_TO_POINT(25);
    m_doc = doc;

    setFocusProxy(widget.createButton);

    QVBoxLayout *lay = new QVBoxLayout(widget.sizeTab);
    m_sizeWidget = new KoPageLayoutWidget(widget.sizeTab, m_layout);
    m_sizeWidget->showPageSpread(true);
    m_sizeWidget->setUnit(m_unit);
    lay->addWidget(m_sizeWidget);

    lay = new QVBoxLayout(widget.columnsTab);
    m_columnsWidget = new KWDocumentColumns(widget.columnsTab, m_columns);
    m_columnsWidget->setUnit(m_unit);
    m_columnsWidget->setShowPreview(false);
    lay->addWidget(m_columnsWidget);

    lay = new QVBoxLayout(widget.previewPane);
    widget.previewPane->setLayout(lay);
    KoPagePreviewWidget *prev = new KoPagePreviewWidget(widget.previewPane);
    lay->addWidget(prev);
    prev->setColumns(columns);
    prev->setPageLayout(m_layout);

    connect(m_sizeWidget, &KoPageLayoutWidget::layoutChanged, this, &KWStartupWidget::sizeUpdated);
    connect(widget.createButton, &QAbstractButton::clicked, this, &KWStartupWidget::buttonClicked);
    connect(m_sizeWidget, &KoPageLayoutWidget::unitChanged, this, &KWStartupWidget::unitChanged);
    connect(m_columnsWidget, &KWDocumentColumns::columnsChanged, prev, &KoPagePreviewWidget::setColumns);
    connect(m_columnsWidget, &KWDocumentColumns::columnsChanged, this, &KWStartupWidget::columnsUpdated);
    connect(m_sizeWidget, &KoPageLayoutWidget::layoutChanged, prev, &KoPagePreviewWidget::setPageLayout);
}

void KWStartupWidget::unitChanged(const KoUnit &unit)
{
    m_unit = unit;
    m_columnsWidget->setUnit(unit);
}

void KWStartupWidget::sizeUpdated(const KoPageLayout &layout)
{
    m_layout = layout;
}

void KWStartupWidget::columnsUpdated(const KoColumns &columns)
{
    m_columns = columns;
}

void KWStartupWidget::buttonClicked()
{
    m_doc->initEmpty();

    KWPageStyle style = m_doc->pageManager()->defaultPageStyle();
    Q_ASSERT(style.isValid());
    style.setColumns(m_columns);
    style.setPageLayout(m_layout);
    m_doc->setUnit(m_unit);

    m_doc->relayout();

    Q_EMIT documentSelected();
}
