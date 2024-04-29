/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_FORMAT_ERRORBAR_DIALOG_DIALOG_H
#define KCHART_FORMAT_ERRORBAR_DIALOG_DIALOG_H

#include "ui_FormatErrorBarDialog.h"

namespace KoChart
{

class FormatErrorBarDialog : public QDialog, public Ui::FormatErrorBarDialog
{
    Q_OBJECT
public:
    explicit FormatErrorBarDialog(QWidget *parent = nullptr);
    ~FormatErrorBarDialog();
    Ui::FormatErrorBarDialog widget;

private Q_SLOTS:
    void errorIndicatorChanged();
    void errorTypeChanged(int currIndex);
    void setSameErrorValueForBoth(bool isChecked);
    void setSameErrorValueForBoth(double value);
};

} // Namespace KoChart

#endif // KCHART_FORMAT_ERRORBAR_DIALOG_H
