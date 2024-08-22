/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormatErrorBarDialog.h"

using namespace KoChart;

FormatErrorBarDialog::FormatErrorBarDialog(QWidget *parent)
    : QDialog(parent)
{
    widget.setupUi(this);
    errorTypeChanged(0);

    connect(widget.posIndicator, &QAbstractButton::toggled, this, &FormatErrorBarDialog::errorIndicatorChanged);
    connect(widget.negIndicator, &QAbstractButton::toggled, this, &FormatErrorBarDialog::errorIndicatorChanged);
    connect(widget.posAndNegIndicator, &QAbstractButton::toggled, this, &FormatErrorBarDialog::errorIndicatorChanged);
    connect(widget.sameValueForBoth, &QCheckBox::toggled, this, QOverload<bool>::of(&FormatErrorBarDialog::setSameErrorValueForBoth));
    connect(widget.positiveValue, &QDoubleSpinBox::valueChanged, this, QOverload<double>::of(&FormatErrorBarDialog::setSameErrorValueForBoth));
    connect(widget.errorType, &QComboBox::currentIndexChanged, this, &FormatErrorBarDialog::errorTypeChanged);
}

FormatErrorBarDialog::~FormatErrorBarDialog() = default;

void FormatErrorBarDialog::errorIndicatorChanged()
{
    if (widget.posIndicator->isChecked()) {
        widget.positiveValue->setEnabled(true);
        widget.negativeValue->setEnabled(false);
        widget.sameValueForBoth->setEnabled(false);
    } else if (widget.negIndicator->isChecked()) {
        widget.negativeValue->setEnabled(true);
        widget.positiveValue->setEnabled(false);
        widget.sameValueForBoth->setEnabled(false);
    } else {
        widget.positiveValue->setEnabled(true);
        widget.negativeValue->setEnabled(true);
        widget.sameValueForBoth->setEnabled(true);
        setSameErrorValueForBoth(widget.sameValueForBoth->isChecked());
    }
}

void FormatErrorBarDialog::errorTypeChanged(int currIndex)
{
    switch (currIndex) {
    case 1:
        widget.constantError->show();
        widget.percentageError->hide();
        break;
    case 2:
    case 3:
        widget.constantError->hide();
        widget.percentageError->show();
        break;
    default:
        widget.constantError->hide();
        widget.percentageError->hide();
    }
}

void FormatErrorBarDialog::setSameErrorValueForBoth(bool isChecked)
{
    if (isChecked) {
        widget.negativeValue->setEnabled(false);
        widget.negativeValue->setValue(widget.positiveValue->value());
    } else {
        widget.negativeValue->setEnabled(true);
    }
}

void FormatErrorBarDialog::setSameErrorValueForBoth(double value)
{
    if (widget.sameValueForBoth->isEnabled() && widget.sameValueForBoth->isChecked()) {
        widget.negativeValue->setValue(value);
    }
}
