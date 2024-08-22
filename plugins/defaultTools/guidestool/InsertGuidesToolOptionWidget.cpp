/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Carlos Licea <carlos.licea@kdemail.net>
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InsertGuidesToolOptionWidget.h"

#include <KoIcon.h>

InsertGuidesToolOptionWidget::InsertGuidesToolOptionWidget(QWidget *parent)
    : QWidget(parent)
{
    m_widget.setupUi(this);

    m_widget.m_horizontalEdgesCheckBox->setIcon(koIconNeeded("add guides at top & bottom side of page", "add-horizontal-edges"));
    m_widget.m_verticalEdgesCheckBox->setIcon(koIconNeeded("add guides at left & right side of page", "add-vertical-edges"));

    connect(m_widget.m_createButton, &QAbstractButton::clicked, this, &InsertGuidesToolOptionWidget::onCreateButtonClicked);
}

InsertGuidesToolOptionWidget::~InsertGuidesToolOptionWidget() = default;

void InsertGuidesToolOptionWidget::onCreateButtonClicked(bool checked)
{
    Q_UNUSED(checked);

    GuidesTransaction *transaction = new GuidesTransaction;
    transaction->erasePreviousGuides = m_widget.m_erasePreviousCheckBox->isChecked();
    transaction->verticalGuides = m_widget.m_verticalSpinBox->value();
    transaction->insertVerticalEdgesGuides = m_widget.m_verticalEdgesCheckBox->isChecked();
    transaction->horizontalGuides = m_widget.m_horizontalSpinBox->value();
    transaction->insertHorizontalEdgesGuides = m_widget.m_horizontalEdgesCheckBox->isChecked();

    Q_EMIT createGuides(transaction);
}
