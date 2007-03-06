/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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
#include "KWPageLayout.h"
#include "KWPagePreview.h"

#include <klocale.h>

KWPageLayout::KWPageLayout(QWidget *parent, const KoPageLayout &layout, const KoColumns &columns)
    : QWidget(parent)
{
    widget.setupUi(this);

    m_orientationGroup = new QButtonGroup();
    m_orientationGroup->addButton(widget.portrait, KoPageFormat::Portrait);
    m_orientationGroup->addButton(widget.landscape, KoPageFormat::Landscape);

    QButtonGroup *group2 = new QButtonGroup(this);
    group2->addButton(widget.singleSided);
    group2->addButton(widget.facingPages);
    // the two sets of labels we use might have different lengths; make sure this does not create a 'jumping' ui
    widget.singleSided->setChecked(true);
    facingPagesChanged();
    int width = qMax(widget.leftLabel->width(), widget.rightLabel->width());
    widget.facingPages->setChecked(true);
    facingPagesChanged();
    width = qMax(width, qMax(widget.leftLabel->width(), widget.rightLabel->width()));
    widget.leftLabel->setMinimumSize(QSize(width, 5));

    widget.units->addItems( KoUnit::listOfUnitName() );
    widget.sizes->addItems(KoPageFormat::allFormats());

    QVBoxLayout *lay = new QVBoxLayout(widget.previewPane);
    widget.previewPane->setLayout(lay);
    lay->setMargin(0);
    KWPagePreview *prev = new KWPagePreview(widget.previewPane);
    lay->addWidget(prev);
    prev->setColumns(columns);

    connect(widget.sizes, SIGNAL(currentIndexChanged(int)), this, SLOT(sizeChanged(int)));
    connect(widget.units, SIGNAL(currentIndexChanged(int)), this, SLOT(unitChanged(int)));
    connect(group2, SIGNAL(buttonClicked (int)), this, SLOT(facingPagesChanged()));
    connect(m_orientationGroup, SIGNAL(buttonClicked (int)), this, SLOT(optionsChanged()));
    connect(widget.width, SIGNAL(valueChangedPt(double)), this, SLOT(optionsChanged()));
    connect(widget.height, SIGNAL(valueChangedPt(double)), this, SLOT(optionsChanged()));
    connect(widget.topMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));
    connect(widget.bottomMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));
    connect(widget.bindingEdgeMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));
    connect(widget.pageEdgeMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));

    connect(this, SIGNAL(layoutChanged(const KoPageLayout&)), prev, SLOT(setPageLayout(const KoPageLayout&)));

    setUnit(KoUnit(KoUnit::Millimeter));
    setPageLayout(layout);
}

void KWPageLayout::sizeChanged(int row) {
    m_pageLayout.format = static_cast<KoPageFormat::Format> (row);
    bool enable =  m_pageLayout.format == KoPageFormat::CustomSize;
    widget.width->setEnabled( enable );
    widget.height->setEnabled( enable );

    if ( m_pageLayout.format != KoPageFormat::CustomSize ) {
        m_pageLayout.width = MM_TO_POINT( KoPageFormat::width( m_pageLayout.format, m_pageLayout.orientation ) );
        m_pageLayout.height = MM_TO_POINT( KoPageFormat::height( m_pageLayout.format, m_pageLayout.orientation ) );
    }

    widget.width->changeValue( m_pageLayout.width );
    widget.height->changeValue( m_pageLayout.height );

    emit layoutChanged(m_pageLayout);
}

void KWPageLayout::unitChanged(int row) {
    setUnit(KoUnit(static_cast<KoUnit::Unit> (row)));
}

void KWPageLayout::setUnit(const KoUnit &unit) {
    m_unit = unit;

    widget.width->setUnit(m_unit);
    widget.height->setUnit(m_unit);
    widget.topMargin->setUnit(m_unit);
    widget.bottomMargin->setUnit(m_unit);
    widget.bindingEdgeMargin->setUnit(m_unit);
    widget.pageEdgeMargin->setUnit(m_unit);

    // TODO set combobox
}

void KWPageLayout::setPageLayout(const KoPageLayout &layout) {
    m_pageLayout = layout;
    widget.sizes->setCurrentIndex(layout.format);
    m_orientationGroup->button( layout.orientation )->setChecked( true );
    if(layout.left < 0 || layout.right < 0) {
        widget.facingPages->setChecked(true);
        widget.bindingEdgeMargin->changeValue(layout.bindingSide);
        widget.pageEdgeMargin->changeValue(layout.pageEdge);
    }
    else {
        widget.singleSided->setChecked(true);
        widget.bindingEdgeMargin->changeValue(layout.left);
        widget.pageEdgeMargin->changeValue(layout.right);
    }
    facingPagesChanged();

    widget.topMargin->changeValue(layout.top);
    widget.bottomMargin->changeValue(layout.bottom);
}

void KWPageLayout::facingPagesChanged() {
    if(widget.singleSided->isChecked()) {
        widget.leftLabel->setText(i18n("Left Edge:"));
        widget.rightLabel->setText(i18n("Right Edge:"));
    }
    else {
        widget.leftLabel->setText(i18n("Binding Edge:"));
        widget.rightLabel->setText(i18n("Page Edge:"));
    }
    marginsChanged();
}

void KWPageLayout::marginsChanged() {
    m_pageLayout.left = -1;
    m_pageLayout.right = -1;
    m_pageLayout.bindingSide = -1;
    m_pageLayout.pageEdge = -1;
    if(widget.singleSided->isChecked()) {
        m_pageLayout.left = m_marginsEnabled?widget.bindingEdgeMargin->value():0;
        m_pageLayout.right = m_marginsEnabled?widget.pageEdgeMargin->value():0;
    }
    else {
        m_pageLayout.bindingSide = m_marginsEnabled?widget.bindingEdgeMargin->value():0;
        m_pageLayout.pageEdge = m_marginsEnabled?widget.pageEdgeMargin->value():0;
    }
    m_pageLayout.top = m_marginsEnabled?widget.topMargin->value():0;
    m_pageLayout.bottom = m_marginsEnabled?widget.bottomMargin->value():0;
    emit layoutChanged(m_pageLayout);
}

void KWPageLayout::setTextAreaAvailable(bool available) {
    m_marginsEnabled = available;
    widget.margins->setEnabled(available);
    marginsChanged();
}

void KWPageLayout::optionsChanged() {
    m_pageLayout.orientation = widget.landscape->isChecked() ? KoPageFormat::Landscape : KoPageFormat::Portrait;

    emit layoutChanged(m_pageLayout);
}

#include <KWPageLayout.moc>
