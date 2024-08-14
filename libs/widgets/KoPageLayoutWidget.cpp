/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPageLayoutWidget.h"

#include <ui_KoPageLayoutWidget.h>

#include <KoUnit.h>

#include <QButtonGroup>

class Q_DECL_HIDDEN KoPageLayoutWidget::Private
{
public:
    Ui::KoPageLayoutWidget widget;
    KoPageLayout pageLayout;
    KoUnit unit;

    QButtonGroup *orientationGroup;
    bool marginsEnabled;
    bool allowSignals;
};

KoPageLayoutWidget::KoPageLayoutWidget(QWidget *parent, const KoPageLayout &layout)
    : QWidget(parent)
    , d(new Private)
{
    d->widget.setupUi(this);
    d->widget.mainLayout->insertStretch(0);
    d->widget.mainLayout->insertStretch(2);
    d->widget.gridLayout->setRowStretch(d->widget.gridLayout->rowCount(), 1);

    d->pageLayout = layout;
    d->marginsEnabled = true;
    d->allowSignals = true;
    d->orientationGroup = new QButtonGroup(this);
    d->orientationGroup->addButton(d->widget.portrait, KoPageFormat::Portrait);
    d->orientationGroup->addButton(d->widget.landscape, KoPageFormat::Landscape);

    QButtonGroup *group2 = new QButtonGroup(this);
    group2->addButton(d->widget.singleSided);
    group2->addButton(d->widget.facingPages);
    // the two sets of labels we use might have different lengths; make sure this does not create a 'jumping' ui
    d->widget.facingPages->setChecked(true);
    facingPagesChanged();
    int width = qMax(d->widget.leftLabel->width(), d->widget.rightLabel->width());
    d->widget.singleSided->setChecked(true);
    facingPagesChanged();
    width = qMax(width, qMax(d->widget.leftLabel->width(), d->widget.rightLabel->width()));
    d->widget.leftLabel->setMinimumSize(QSize(width, 5));

    d->widget.units->addItems(KoUnit::listOfUnitNameForUi(KoUnit::HidePixel));
    d->widget.sizes->addItems(KoPageFormat::localizedPageFormatNames());
    setPageSpread(false);

    connect(d->widget.sizes, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoPageLayoutWidget::sizeChanged);
    connect(d->widget.units, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoPageLayoutWidget::slotUnitChanged);
    connect(group2, &QButtonGroup::buttonClicked, this, &KoPageLayoutWidget::facingPagesChanged);
    connect(d->orientationGroup, &QButtonGroup::buttonClicked, this, &KoPageLayoutWidget::orientationChanged);
    connect(d->widget.width, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::optionsChanged);
    connect(d->widget.height, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::optionsChanged);
    connect(d->widget.topMargin, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::marginsChanged);
    connect(d->widget.bottomMargin, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::marginsChanged);
    connect(d->widget.bindingEdgeMargin, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::marginsChanged);
    connect(d->widget.pageEdgeMargin, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::marginsChanged);
    connect(d->widget.width, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::optionsChanged);
    connect(d->widget.height, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoPageLayoutWidget::optionsChanged);

    setUnit(KoUnit(KoUnit::Millimeter));
    setPageLayout(layout);
    if (layout.format == 0) // make sure we always call this during startup, even if the A3 (index=0) was chosen
        sizeChanged(layout.format);

    showTextDirection(false);
    /* disable advanced page layout features by default */
    d->widget.facingPageLabel->setVisible(false);
    d->widget.facingPages->setVisible(false);
    d->widget.singleSided->setVisible(false);
    d->widget.stylesLabel->setVisible(false);
    d->widget.pageStyle->setVisible(false);
}

KoPageLayoutWidget::~KoPageLayoutWidget()
{
    delete d;
}

KoPageLayout KoPageLayoutWidget::pageLayout() const
{
    return d->pageLayout;
}

void KoPageLayoutWidget::sizeChanged(int row)
{
    if (row < 0)
        return;
    if (!d->allowSignals)
        return;
    d->allowSignals = false;
    d->pageLayout.format = static_cast<KoPageFormat::Format>(row);
    bool custom = d->pageLayout.format == KoPageFormat::CustomSize;
    d->widget.width->setEnabled(custom);
    d->widget.height->setEnabled(custom);

    if (!custom) {
        d->pageLayout.width = MM_TO_POINT(KoPageFormat::width(d->pageLayout.format, d->pageLayout.orientation));
        d->pageLayout.height = MM_TO_POINT(KoPageFormat::height(d->pageLayout.format, d->pageLayout.orientation));
        if (d->widget.facingPages->isChecked()) // is pagespread
            d->pageLayout.width *= 2;
    }

    d->widget.width->changeValue(d->pageLayout.width);
    d->widget.height->changeValue(d->pageLayout.height);

    Q_EMIT layoutChanged(d->pageLayout);
    d->allowSignals = true;
}

void KoPageLayoutWidget::slotUnitChanged(int row)
{
    setUnit(KoUnit::fromListForUi(row, KoUnit::HidePixel));
}

void KoPageLayoutWidget::setUnit(const KoUnit &unit)
{
    if (d->unit == unit)
        return;
    d->unit = unit;

    d->widget.width->setUnit(unit);
    d->widget.height->setUnit(unit);
    d->widget.topMargin->setUnit(unit);
    d->widget.bottomMargin->setUnit(unit);
    d->widget.bindingEdgeMargin->setUnit(unit);
    d->widget.pageEdgeMargin->setUnit(unit);
    d->widget.units->setCurrentIndex(unit.indexInListForUi(KoUnit::HidePixel));

    Q_EMIT unitChanged(d->unit);
}

void KoPageLayoutWidget::setPageLayout(const KoPageLayout &layout)
{
    if (!d->allowSignals)
        return;
    d->allowSignals = false;
    d->pageLayout = layout;

    Q_ASSERT(d->orientationGroup->button(layout.orientation));
    d->orientationGroup->button(layout.orientation)->setChecked(true);
    if (layout.bindingSide >= 0 && layout.pageEdge >= 0) {
        d->widget.facingPages->setChecked(true);
        d->widget.bindingEdgeMargin->changeValue(layout.bindingSide);
        d->widget.pageEdgeMargin->changeValue(layout.pageEdge);
        d->pageLayout.leftMargin = -1;
        d->pageLayout.rightMargin = -1;
    } else {
        d->widget.singleSided->setChecked(true);
        d->widget.bindingEdgeMargin->changeValue(layout.leftMargin);
        d->widget.pageEdgeMargin->changeValue(layout.rightMargin);
        d->pageLayout.pageEdge = -1;
        d->pageLayout.bindingSide = -1;
    }
    facingPagesChanged();

    d->widget.topMargin->changeValue(layout.topMargin);
    d->widget.bottomMargin->changeValue(layout.bottomMargin);
    d->allowSignals = true;
    d->widget.sizes->setCurrentIndex(layout.format); // calls sizeChanged()
}

void KoPageLayoutWidget::facingPagesChanged()
{
    if (!d->allowSignals)
        return;
    d->allowSignals = false;
    if (d->widget.singleSided->isChecked()) {
        d->widget.leftLabel->setText(i18n("Left Edge Margin:"));
        d->widget.rightLabel->setText(i18n("Right Edge Margin:"));
    } else {
        d->widget.leftLabel->setText(i18n("Binding Edge Margin:"));
        d->widget.rightLabel->setText(i18n("Page Edge Margin:"));
    }
    d->allowSignals = true;
    marginsChanged();
    sizeChanged(d->widget.sizes->currentIndex());
}

void KoPageLayoutWidget::marginsChanged()
{
    if (!d->allowSignals)
        return;
    d->allowSignals = false;
    d->pageLayout.leftMargin = -1;
    d->pageLayout.rightMargin = -1;
    d->pageLayout.bindingSide = -1;
    d->pageLayout.pageEdge = -1;
    d->pageLayout.topMargin = d->marginsEnabled ? d->widget.topMargin->value() : 0;
    d->pageLayout.bottomMargin = d->marginsEnabled ? d->widget.bottomMargin->value() : 0;
    qreal left = d->marginsEnabled ? d->widget.bindingEdgeMargin->value() : 0;
    qreal right = d->marginsEnabled ? d->widget.pageEdgeMargin->value() : 0;
    if (left + right > d->pageLayout.width - 10) {
        // make sure the actual text area is never smaller than 10 points.
        qreal diff = d->pageLayout.width - 10 - left - right;
        left = qMin(d->pageLayout.width - 10, qMax(qreal(0.0), left - diff / qreal(2.0)));
        right = qMax(qreal(0.0), right - d->pageLayout.width - 10 - left);
    }

    if (d->widget.singleSided->isChecked()) {
        d->pageLayout.leftMargin = left;
        d->pageLayout.rightMargin = right;
    } else {
        d->pageLayout.bindingSide = left;
        d->pageLayout.pageEdge = right;
    }
    // debugWidgets << "  " << d->pageLayout.left <<"|"<< d->pageLayout.bindingSide << "," <<
    //    d->pageLayout.right << "|"<< d->pageLayout.pageEdge;
    Q_EMIT layoutChanged(d->pageLayout);
    d->allowSignals = true;
}

void KoPageLayoutWidget::setTextAreaAvailable(bool available)
{
    d->marginsEnabled = available;
    d->widget.topMargin->setEnabled(available);
    d->widget.bottomMargin->setEnabled(available);
    d->widget.bindingEdgeMargin->setEnabled(available);
    d->widget.pageEdgeMargin->setEnabled(available);
    marginsChanged();
}

void KoPageLayoutWidget::optionsChanged()
{
    if (!d->allowSignals)
        return;
    if (d->widget.sizes->currentIndex() == KoPageFormat::CustomSize) {
        d->pageLayout.width = d->widget.width->value();
        d->pageLayout.height = d->widget.height->value();
    } else
        sizeChanged(d->widget.sizes->currentIndex());

    marginsChanged();
}

void KoPageLayoutWidget::orientationChanged()
{
    if (!d->allowSignals)
        return;
    d->allowSignals = false;
    d->pageLayout.orientation = d->widget.landscape->isChecked() ? KoPageFormat::Landscape : KoPageFormat::Portrait;

    qreal x = d->widget.height->value();
    d->widget.height->changeValue(d->widget.width->value());
    d->widget.width->changeValue(x);

    d->allowSignals = true;
    optionsChanged();
}

void KoPageLayoutWidget::showUnitchooser(bool on)
{
    d->widget.units->setVisible(on);
    d->widget.unitsLabel->setVisible(on);
}

void KoPageLayoutWidget::showPageSpread(bool on)
{
    d->widget.facingPageLabel->setVisible(on);
    d->widget.singleSided->setVisible(on);
    d->widget.facingPages->setVisible(on);
}

void KoPageLayoutWidget::setPageSpread(bool pageSpread)
{
    if (pageSpread)
        d->widget.facingPages->setChecked(true);
    else
        d->widget.singleSided->setChecked(true);
}

void KoPageLayoutWidget::setApplyToDocument(bool apply)
{
    if (apply) {
        d->widget.facingPageLabel->setText(i18n("Facing Pages:"));
        d->widget.facingPages->setText(i18n("Facing pages"));
    } else {
        d->widget.facingPageLabel->setText(i18n("Page Layout:"));
        d->widget.facingPages->setText(i18n("Page spread"));
    }
}

void KoPageLayoutWidget::showTextDirection(bool on)
{
    d->widget.directionLabel->setVisible(on);
    d->widget.textDirection->setVisible(on);
}

void KoPageLayoutWidget::setTextDirection(KoText::Direction direction)
{
    int index = 0;
    switch (direction) {
    case KoText::LeftRightTopBottom:
        index = 1;
        break;
    case KoText::RightLeftTopBottom:
        index = 2;
        break;
    case KoText::TopBottomRightLeft: // unused for now.
    case KoText::InheritDirection:
    case KoText::AutoDirection:
        index = 0;
    case KoText::TopBottomLeftRight:; // unhandled, because it actually doesn't exist in real-world writing systems.
                                      // Boustrophedon would be interesting to implement, though
    }
    d->widget.textDirection->setCurrentIndex(index);
}

KoText::Direction KoPageLayoutWidget::textDirection() const
{
    switch (d->widget.textDirection->currentIndex()) {
    case 1:
        return KoText::LeftRightTopBottom;
    case 2:
        return KoText::RightLeftTopBottom;
    default:
    case 0:
        return KoText::AutoDirection;
    }
}

void KoPageLayoutWidget::showPageStyles(bool on)
{
    d->widget.stylesLabel->setVisible(on);
    d->widget.pageStyle->setVisible(on);
}

void KoPageLayoutWidget::setPageStyles(const QStringList &styles)
{
    d->widget.pageStyle->clear();
    d->widget.pageStyle->addItems(styles);
}

QString KoPageLayoutWidget::currentPageStyle() const
{
    return d->widget.pageStyle->currentText();
}
