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
#include "KWPage.h"

#include <KDebug>

KWPageLayout::KWPageLayout(QWidget *parent, const KoPageLayout &layout)
    : QWidget(parent),
    m_pageLayout(layout),
    m_marginsEnabled(true),
    m_allowSignals(false)
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
    forSinglePage(0);

    connect(widget.sizes, SIGNAL(currentIndexChanged(int)), this, SLOT(sizeChanged(int)));
    connect(widget.units, SIGNAL(currentIndexChanged(int)), this, SLOT(unitChanged(int)));
    connect(group2, SIGNAL(buttonClicked (int)), this, SLOT(facingPagesChanged()));
    connect(m_orientationGroup, SIGNAL(buttonClicked (int)), this, SLOT(orientationChanged()));
    connect(widget.width, SIGNAL(valueChangedPt(double)), this, SLOT(optionsChanged()));
    connect(widget.height, SIGNAL(valueChangedPt(double)), this, SLOT(optionsChanged()));
    connect(widget.topMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));
    connect(widget.bottomMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));
    connect(widget.bindingEdgeMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));
    connect(widget.pageEdgeMargin, SIGNAL(valueChangedPt(double)), this, SLOT(marginsChanged()));
    connect(widget.width, SIGNAL(valueChangedPt(double)), this, SLOT(optionsChanged()));
    connect(widget.height, SIGNAL(valueChangedPt(double)), this, SLOT(optionsChanged()));

    setUnit(KoUnit(KoUnit::Millimeter));
    m_allowSignals = true;
    setPageLayout(layout);
}

void KWPageLayout::sizeChanged(int row) {
    m_pageLayout.format = static_cast<KoPageFormat::Format> (row);
    bool custom =  m_pageLayout.format == KoPageFormat::CustomSize;
    bool pageSpread = widget.facingPages->isChecked();
    widget.width->setEnabled( custom );
    widget.height->setEnabled( custom );

    if ( !custom ) {
        m_pageLayout.width = MM_TO_POINT( KoPageFormat::width( m_pageLayout.format, m_pageLayout.orientation ) );
        m_pageLayout.height = MM_TO_POINT( KoPageFormat::height( m_pageLayout.format, m_pageLayout.orientation ) );
        if(pageSpread)
            m_pageLayout.width *= 2;
    }

    widget.height->changeValue( m_pageLayout.height );
    widget.width->changeValue( m_pageLayout.width / (pageSpread?2:1) );

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

    emit unitChanged(m_unit);
    // TODO set combobox
}

void KWPageLayout::setPageLayout(const KoPageLayout &layout) {
    if(! m_allowSignals) return;
    m_allowSignals = false;
    m_pageLayout = layout;
    widget.sizes->setCurrentIndex(layout.format);
    widget.width->changeValue( layout.width );
    widget.height->changeValue( layout.height );

    m_orientationGroup->button( layout.orientation )->setChecked( true );
    if(layout.bindingSide >= 0 && layout.pageEdge >= 0) {
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
    m_allowSignals = true;
}

void KWPageLayout::facingPagesChanged() {
    if(! m_allowSignals) return;
    m_allowSignals = false;
    if(widget.singleSided->isChecked()) {
        widget.leftLabel->setText(i18n("Left Edge:"));
        widget.rightLabel->setText(i18n("Right Edge:"));
    }
    else {
        widget.leftLabel->setText(i18n("Binding Edge:"));
        widget.rightLabel->setText(i18n("Page Edge:"));
    }
    m_allowSignals = true;
    marginsChanged();
    sizeChanged(widget.sizes->currentIndex());
}

void KWPageLayout::marginsChanged() {
    if(! m_allowSignals) return;
    m_allowSignals = false;
    m_pageLayout.left = -1;
    m_pageLayout.right = -1;
    m_pageLayout.bindingSide = -1;
    m_pageLayout.pageEdge = -1;
    m_pageLayout.top = m_marginsEnabled?widget.topMargin->value():0;
    m_pageLayout.bottom = m_marginsEnabled?widget.bottomMargin->value():0;
    double left = m_marginsEnabled?widget.bindingEdgeMargin->value():0;
    double right = m_marginsEnabled?widget.pageEdgeMargin->value():0;
    if(left + right > m_pageLayout.width - 10) {
        // make sure the actual text area is never smaller than 10 points.
        double diff = m_pageLayout.width - 10 - left - right;
        left = qMin(m_pageLayout.width - 10, qMax(0.0, left - diff / 2.0));
        right = qMax(0.0, right - m_pageLayout.width - 10 - left);
    }

    if(widget.singleSided->isChecked()) {
        m_pageLayout.left = left;
        m_pageLayout.right = right;
    }
    else {
        m_pageLayout.bindingSide = left;
        m_pageLayout.pageEdge = right;
    }
    // kDebug() << "  " << m_pageLayout.left << "|"<< m_pageLayout.bindingSide << ", " <<
    //    m_pageLayout.right << "|"<< m_pageLayout.pageEdge << endl;
    emit layoutChanged(m_pageLayout);
    m_allowSignals = true;
}

void KWPageLayout::setTextAreaAvailable(bool available) {
    m_marginsEnabled = available;
    widget.margins->setEnabled(available);
    marginsChanged();
}

void KWPageLayout::optionsChanged() {
    if(! m_allowSignals) return;
    m_allowSignals = false;
    m_pageLayout.orientation = widget.landscape->isChecked() ? KoPageFormat::Landscape : KoPageFormat::Portrait;
    if(widget.sizes->currentIndex() == KoPageFormat::CustomSize) {
        m_pageLayout.width = widget.width->value();
        m_pageLayout.height = widget.height->value();
    }

    m_allowSignals = true;
    marginsChanged();
}

void KWPageLayout::orientationChanged() {
    if(! m_allowSignals) return;
    m_allowSignals = false;
    qSwap(m_pageLayout.width, m_pageLayout.height);
    widget.width->changeValue( m_pageLayout.width );
    widget.height->changeValue( m_pageLayout.height );
    m_allowSignals = true;
    optionsChanged();
}

void KWPageLayout::showUnitchooser(bool on) {
    widget.units->setVisible(on);
    widget.unitsLabel->setVisible(on);
}

void KWPageLayout::forSinglePage(KWPage *page) {
    if(page) {
        widget.facingPageLabel->setText(i18n("Page Layout:"));
        widget.facingPages->setText(i18n("Page spread"));
        widget.wholeDocument->setCheckState(Qt::Unchecked);
        if(page->pageSide() == KWPage::PageSpread) {
            widget.facingPages->setChecked(true);
            widget.width->changeValue( m_pageLayout.width / 2.0);
        }
        else
            widget.singleSided->setChecked(true);
    }
    else {
        widget.facingPageLabel->setText(i18n("Facing Pages:"));
        widget.facingPages->setText(i18n("Facing pages"));
        widget.wholeDocument->setCheckState(Qt::Checked);
    }
    widget.wholeDocument->setVisible(page);
    widget.wholeDocumentLabel->setVisible(page);
}

void KWPageLayout::setStartPageNumber(int pageNumber) {
    widget.firstPage->setValue(pageNumber);
}

int KWPageLayout::startPageNumber() const {
    return widget.firstPage->value();
}

bool KWPageLayout::marginsForDocument() const {
    return widget.wholeDocument->checkState() == Qt::Checked;
}

#include <KWPageLayout.moc>
