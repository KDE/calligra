/* This file is part of the KDE project
 * Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Gary Cramblitt <garycramblitt@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.
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

// Qt includes.
#include <QPixmap>

// KDE includes.
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdebug.h>

// KOffice includes.
#include <KoUnitWidgets.h>

// KWord includes.
#include "KWPagePreview.h"
#include "KWPageLayout.h"

KWPageLayout::KWPageLayout(QWidget *parent, const KoPageLayout& layout, KoUnit unit,const KoColumns& columns, bool enableUnitChooser, bool enableBorders)
    : QWidget(parent), m_layout(layout), m_unit(unit), m_blockSignals(false), m_haveBorders(enableBorders)
{
    setupUi(this);

    // -------------- page preview --------------
    m_pagePreview = new KWPagePreview(previewPane, layout);

    // -------------- unit ----------------------
    unitCombo->addItems(KoUnit::listOfUnitName());
    unitCombo->setCurrentIndex(unit.indexInList());

    // -------------- page size -----------------
    sizeCombo->addItems(KoPageFormat::allFormats());

    widthUSB->setEnabled(m_layout.format != PG_CUSTOM);

    heightUSB->setEnabled(m_layout.format != PG_CUSTOM);

    // --------------- orientation ---------------
    QPixmap portraitIcon = QPixmap(UserIcon("koPortrait"));
    portraitIconLabel->setPixmap(portraitIcon);
    portraitIconLabel->setMaximumWidth(portraitIcon.width());

    QPixmap landscapeIcon = QPixmap(UserIcon("koLandscape"));
    landscapeIconLabel->setPixmap(landscapeIcon);
    landscapeIconLabel->setMaximumWidth(landscapeIcon.width());

    // Display layout settings in GUI.
    setGuiValues();

    // Connect signals.
    connect(unitCombo, SIGNAL(activated(int)), this, SLOT(setUnitInt(int)));
    connect(sizeCombo, SIGNAL(activated(int)), this, SLOT(formatChanged(int)));
    connect(widthUSB, SIGNAL(valueChangedPt(double)), this,  SLOT(widthChanged(double)));
    connect(heightUSB, SIGNAL(valueChangedPt(double)), this, SLOT(heightChanged(double)));
    connect(portraitRB, SIGNAL(toggled(bool)), this, SLOT(orientationChanged()));
    connect(landscapeRB, SIGNAL(toggled(bool)), this, SLOT(orientationChanged()));
    connect(leftUSB, SIGNAL(valueChangedPt(double)), this, SLOT(leftChanged(double)));
    connect(rightUSB, SIGNAL(valueChangedPt(double)), this, SLOT(rightChanged(double)));
    connect(topUSB, SIGNAL(valueChangedPt(double)), this, SLOT(topChanged(double)));
    connect(bottomUSB, SIGNAL(valueChangedPt(double)), this, SLOT(bottomChanged(double)));

    updatePreview();
    m_pagePreview->setPageColumns(columns);

    unitBox->setEnabled(enableUnitChooser);
    marginGB->setEnabled(enableBorders);
}

void KWPageLayout::setEnableBorders(bool on) {
    m_haveBorders = on;
    marginGB->setEnabled(on);

    // update m_layout
    m_layout.ptLeft = on?leftUSB->value():0;
    m_layout.ptRight = on?rightUSB->value():0;
    m_layout.ptTop = on?topUSB->value():0;
    m_layout.ptBottom = on?bottomUSB->value():0;

    // use updated m_layout
    updatePreview();
    emit propertyChange(m_layout);
}

void KWPageLayout::updatePreview() {
    m_pagePreview->setPageLayout( m_layout );
}

void KWPageLayout::setGuiValues() {
    // page size
    sizeCombo->setCurrentIndex(m_layout.format);
    // orientation
    if (m_layout.orientation == PG_PORTRAIT)
        portraitRB->setChecked(true);
    else
        landscapeRB->setChecked(true);
    // Units and margins.
    setUnit(m_unit);
    updatePreview();
}

void KWPageLayout::setUnit(KoUnit unit) {
    m_unit = unit;
    m_blockSignals = true; // due to non-atomic changes the propertyChange emits should be blocked

    widthUSB->setUnit(m_unit);
    widthUSB->setMinMaxStep(0, KoUnit::fromUserValue(9999, m_unit), KoUnit::fromUserValue(0.01, m_unit));
    widthUSB->changeValue(m_layout.ptWidth);

    heightUSB->setUnit(m_unit);
    heightUSB->setMinMaxStep(0, KoUnit::fromUserValue(9999, m_unit), KoUnit::fromUserValue(0.01, m_unit));
    heightUSB->changeValue(m_layout.ptHeight);

    double dStep = KoUnit::fromUserValue(0.2, m_unit);

    leftUSB->setUnit(m_unit);
    leftUSB->changeValue(m_layout.ptLeft);
    leftUSB->setMinMaxStep(0, m_layout.ptWidth, dStep);

    rightUSB->setUnit(m_unit);
    rightUSB->changeValue(m_layout.ptRight);
    rightUSB->setMinMaxStep(0, m_layout.ptWidth, dStep);

    topUSB->setUnit(m_unit);
    topUSB->changeValue(m_layout.ptTop);
    topUSB->setMinMaxStep(0, m_layout.ptHeight, dStep);

    bottomUSB->setUnit(m_unit);
    bottomUSB->changeValue(m_layout.ptBottom);
    bottomUSB->setMinMaxStep(0, m_layout.ptHeight, dStep);

    m_blockSignals = false;
}

void KWPageLayout::setUnitInt(int unit) {
    setUnit(KoUnit((KoUnit::Unit)unit));
}

void KWPageLayout::formatChanged(int format) {
    if ((KoFormat)format == m_layout.format)
        return;
    m_layout.format = (KoFormat)format;
    bool enable = ((KoFormat)format == PG_CUSTOM);
    widthUSB->setEnabled(enable);
    heightUSB->setEnabled(enable);

    if (m_layout.format != PG_CUSTOM) {
        m_layout.ptWidth = MM_TO_POINT(KoPageFormat::width(
            m_layout.format, m_layout.orientation));
        m_layout.ptHeight = MM_TO_POINT(KoPageFormat::height(
            m_layout.format, m_layout.orientation));
    }

    widthUSB->changeValue(m_layout.ptWidth);
    heightUSB->changeValue(m_layout.ptHeight);

    updatePreview();
    emit propertyChange(m_layout);
}

void KWPageLayout::orientationChanged() {
    m_layout.orientation = (portraitRB->isChecked() ? PG_PORTRAIT : PG_LANDSCAPE);

    // swap dimension
    double val = widthUSB->value();
    widthUSB->changeValue(heightUSB->value());
    heightUSB->changeValue(val);
    // and adjust margins
    m_blockSignals = true;
    val = topUSB->value();
    if(m_layout.orientation == PG_PORTRAIT) { // clockwise
        topUSB->changeValue(rightUSB->value());
        rightUSB->changeValue(bottomUSB->value());
        bottomUSB->changeValue(leftUSB->value());
        leftUSB->changeValue(val);
    } else { // counter clockwise
        topUSB->changeValue(leftUSB->value());
        leftUSB->changeValue(bottomUSB->value());
        bottomUSB->changeValue(rightUSB->value());
        rightUSB->changeValue(val);
    }
    m_blockSignals = false;

    setEnableBorders(m_haveBorders); // will update preview+emit
}

void KWPageLayout::widthChanged(double width) {
    if(m_blockSignals) return;
    m_layout.ptWidth = width;
    updatePreview();
    emit propertyChange(m_layout);
}
void KWPageLayout::heightChanged(double height) {
    if(m_blockSignals) return;
    m_layout.ptHeight = height;
    updatePreview();
    emit propertyChange(m_layout);
}
void KWPageLayout::leftChanged( double left ) {
    if(m_blockSignals) return;
    m_layout.ptLeft = left;
    updatePreview();
    emit propertyChange(m_layout);
}
void KWPageLayout::rightChanged(double right) {
    if(m_blockSignals) return;
    m_layout.ptRight = right;
    updatePreview();
    emit propertyChange(m_layout);
}
void KWPageLayout::topChanged(double top) {
    if(m_blockSignals) return;
    m_layout.ptTop = top;
    updatePreview();
    emit propertyChange(m_layout);
}
void KWPageLayout::bottomChanged(double bottom) {
    if(m_blockSignals) return;
    m_layout.ptBottom = bottom;
    updatePreview();
    emit propertyChange(m_layout);
}

bool KWPageLayout::queryClose() {
    if (m_layout.ptLeft + m_layout.ptRight > m_layout.ptWidth) {
        KMessageBox::error(this,
            i18n("The page width is smaller than the left and right margins."),
            i18n("Page Layout Problem"));
        return false;
    }
    if (m_layout.ptTop + m_layout.ptBottom > m_layout.ptHeight) {
        KMessageBox::error(this,
            i18n("The page height is smaller than the top and bottom margins."),
            i18n("Page Layout Problem") );
        return false;
    }
    return true;
}

void KWPageLayout::setColumns(KoColumns &columns) {
    m_pagePreview->setPageColumns(columns);
}

#include <KWPageLayout.moc>
