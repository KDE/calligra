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

// KDE includes.
#include <kdebug.h>

// KOffice includes.
#include <KoUnitWidgets.h>

// KWord includes.
#include "KWPagePreview.h"
#include "KWDocumentColumns.h"

KWDocumentColumns::KWDocumentColumns(QWidget *parent, const KoColumns &columns, 
    KoUnit unit, bool enableUnitChooser, const KoPageLayout &layout)
    : QWidget(parent), m_columns(columns), m_unit(unit), m_layout(layout)
{
    setupUi(this);

    // -------------- page preview --------------
    m_pagePreview = new KWPagePreview(previewPane, layout);

    // -------------- unit ----------------------
    unitCombo->addItems(KoUnit::listOfUnitName());
    unitCombo->setCurrentIndex(unit.indexInList());

    // Display layout settings in GUI.
    setGuiValues();

    // Connect signals.
    connect(columnsSB, SIGNAL(valueChanged(int)), this, SLOT(columnsChanged(int)));
    connect(spacingUSB, SIGNAL(valueChangedPt(double)), this, SLOT(spacingChanged(double)));

    updatePreview();

    unitBox->setEnabled(enableUnitChooser);
}

void KWDocumentColumns::updatePreview() {
    m_pagePreview->setPageLayout(m_layout);
    m_pagePreview->setPageColumns(m_columns);
}

void KWDocumentColumns::setGuiValues() {
    // Columns.
    columnsSB->setValue(m_columns.columns);
    // Units and spacing.
    setUnit(m_unit);
    updatePreview();
}

void KWDocumentColumns::columnsChanged(int columns) {
    m_columns.columns = columns;
    updatePreview();
    emit propertyChange(m_columns);
}

void KWDocumentColumns::spacingChanged(double spacing) {
    m_columns.ptColumnSpacing = spacing;
    updatePreview();
    emit propertyChange(m_columns);
}

void KWDocumentColumns::setUnit(KoUnit unit) {
    m_unit = unit;
    spacingUSB->setUnit(m_unit);
    double dStep = KoUnit::fromUserValue(0.2, unit);
    spacingUSB->setMinMaxStep(0, m_layout.ptWidth/2, dStep);
}

void KWDocumentColumns::setUnitInt(int unit) {
    setUnit(KoUnit((KoUnit::Unit)unit));
}

bool KWDocumentColumns::queryClose() {
    return true;
}

void KWDocumentColumns::setLayout(const KoPageLayout& layout) {
    m_layout = layout;
    updatePreview();
}

#include <KWDocumentColumns.moc>
