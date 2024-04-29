/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_NEW_AXIS_DIALOG_H
#define KCHART_NEW_AXIS_DIALOG_H

#include "ui_NewAxisDialog.h"

namespace KoChart
{

class NewAxisDialog : public QDialog, public Ui::NewAxisDialog
{
public:
    explicit NewAxisDialog(QWidget *parent = nullptr);
    ~NewAxisDialog();
};

} // Namespace KoChart

#endif // KCHART_NEW_AXIS_DIALOG
