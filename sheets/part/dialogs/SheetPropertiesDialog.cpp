/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetPropertiesDialog.h"

#include <KoVBox.h>

using namespace Calligra::Sheets;

SheetPropertiesDialog::SheetPropertiesDialog(QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Sheet Properties"));
    setObjectName(QLatin1String("sheetPropertiesDialog"));
    setModal(true);
    setButtons(Ok | Cancel | Default);

    KoVBox *mainWidget = new KoVBox(); // makeVBoxMainWidget();
    setMainWidget(mainWidget);
    m_widget = new SheetPropertiesWidget(mainWidget);
    QWidget *spacer = new QWidget(mainWidget);
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    showButtonSeparator(true);
    connect(this, &KoDialog::defaultClicked, this, &SheetPropertiesDialog::slotDefault);
}

SheetPropertiesDialog::~SheetPropertiesDialog()
{
    delete m_widget;
}

void SheetPropertiesDialog::slotDefault()
{
    setLayoutDirection(Qt::LeftToRight);
    setAutoCalculationEnabled(true);
    setShowGrid(true);
    setShowFormula(false);
    setHideZero(false);
    setShowFormulaIndicator(true);
    setShowCommentIndicator(true);
    setShowPageOutline(false);
    setColumnAsNumber(false);
    setLcMode(false);
    setCapitalizeFirstLetter(false);
}

Qt::LayoutDirection SheetPropertiesDialog::layoutDirection() const
{
    if (m_widget->directionComboBox->currentText() == i18n("Left to Right"))
        return Qt::LeftToRight;

    if (m_widget->directionComboBox->currentText() == i18n("Right to Left"))
        return Qt::RightToLeft;

    // fallback
    return Qt::LeftToRight;
}

void SheetPropertiesDialog::setLayoutDirection(Qt::LayoutDirection dir)
{
    switch (dir) {
    case Qt::LeftToRight:
        m_widget->directionComboBox->setCurrentIndex(0);
        break;
    case Qt::RightToLeft:
        m_widget->directionComboBox->setCurrentIndex(1);
        break;
    default:
        break;
    };
}

bool SheetPropertiesDialog::autoCalc() const
{
    return m_widget->autoCalcCheckBox->isChecked();
}

void SheetPropertiesDialog::setAutoCalculationEnabled(bool b)
{
    m_widget->autoCalcCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::showGrid() const
{
    return m_widget->showGridCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowGrid(bool b)
{
    m_widget->showGridCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::showPageOutline() const
{
    return m_widget->showPageOutlineCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowPageOutline(bool b)
{
    m_widget->showPageOutlineCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::showFormula() const
{
    return m_widget->showFormulaCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowFormula(bool b)
{
    m_widget->showFormulaCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::hideZero() const
{
    return m_widget->hideZeroCheckBox->isChecked();
}

void SheetPropertiesDialog::setHideZero(bool b)
{
    m_widget->hideZeroCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::showFormulaIndicator() const
{
    return m_widget->showFormulaIndicatorCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowFormulaIndicator(bool b)
{
    m_widget->showFormulaIndicatorCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::showCommentIndicator() const
{
    return m_widget->showCommentIndicatorCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowCommentIndicator(bool b)
{
    m_widget->showCommentIndicatorCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::columnAsNumber() const
{
    return m_widget->showColumnAsNumbersCheckBox->isChecked();
}

void SheetPropertiesDialog::setColumnAsNumber(bool b)
{
    m_widget->showColumnAsNumbersCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::lcMode() const
{
    return m_widget->useLCModeCheckBox->isChecked();
}

void SheetPropertiesDialog::setLcMode(bool b)
{
    m_widget->useLCModeCheckBox->setChecked(b);
}

bool SheetPropertiesDialog::capitalizeFirstLetter() const
{
    return m_widget->capitalizeFirstLetterCheckBox->isChecked();
}

void SheetPropertiesDialog::setCapitalizeFirstLetter(bool b)
{
    m_widget->capitalizeFirstLetterCheckBox->setChecked(b);
}
