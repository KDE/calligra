/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWDocumentColumns.h"
#include <KoPagePreviewWidget.h>

#include <KoPageLayout.h>
#include <KoUnit.h>

KWDocumentColumns::KWDocumentColumns(QWidget *parent, const KoColumns &columns)
    : QWidget(parent)
{
    widget.setupUi(this);

    setColumns(columns);
    setUnit(KoUnit(KoUnit::Millimeter));

    QGridLayout *layout = new QGridLayout(widget.previewPane);
    layout->setContentsMargins({});
    widget.previewPane->setLayout(layout);
    m_preview = new KoPagePreviewWidget(this);
    layout->addWidget(m_preview);
    m_preview->setColumns(columns);

    connect(widget.columns, QOverload<int>::of(&QSpinBox::valueChanged), this, &KWDocumentColumns::optionsChanged);
    connect(widget.spacing, &KoUnitDoubleSpinBox::valueChangedPt, this, &KWDocumentColumns::optionsChanged);
    connect(this, &KWDocumentColumns::columnsChanged, m_preview, &KoPagePreviewWidget::setColumns);
}

void KWDocumentColumns::setColumns(const KoColumns &columns)
{
    m_columns = columns;
    widget.columns->setValue(columns.count);
    widget.spacing->changeValue(columns.gapWidth);
}

void KWDocumentColumns::setTextAreaAvailable(bool available)
{
    widget.columns->setEnabled(available);
    widget.spacing->setEnabled(available);
    if (available)
        optionsChanged();
    else {
        m_columns.count = 1;
        emit columnsChanged(m_columns);
    }
}

void KWDocumentColumns::setUnit(const KoUnit &unit)
{
    widget.spacing->setUnit(unit);
}

void KWDocumentColumns::optionsChanged()
{
    m_columns.count = widget.columns->value();
    m_columns.gapWidth = widget.spacing->value();
    // Workaround for currently incomplete support of column data:
    // once some editing is done, drop any individual column data
    // TODO: complete UI to set individual column data
    m_columns.columnData.clear();
    emit columnsChanged(m_columns);
}

void KWDocumentColumns::setShowPreview(bool on)
{
    widget.previewPane->setVisible(on);
}
