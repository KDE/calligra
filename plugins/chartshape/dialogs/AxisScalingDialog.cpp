/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AxisScalingDialog.h"

using namespace KoChart;

AxisScalingDialog::AxisScalingDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

AxisScalingDialog::~AxisScalingDialog() = default;
