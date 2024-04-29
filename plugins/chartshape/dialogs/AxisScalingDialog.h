/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_AXIS_SCALING_DIALOG_H
#define KCHART_AXIS_SCALING_DIALOG_H

#include "ui_AxisScalingDialog.h"

namespace KoChart
{

class AxisScalingDialog : public QDialog, public Ui::AxisScalingDialog
{
public:
    explicit AxisScalingDialog(QWidget *parent = nullptr);
    ~AxisScalingDialog();
};

} // Namespace KoChart

#endif // KCHART_AXIS_SCALING_DIALOG_H
