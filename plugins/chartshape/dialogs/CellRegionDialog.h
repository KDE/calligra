/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_CELL_REGION_DIALOG_H
#define KCHART_CELL_REGION_DIALOG_H

#include "ui_CellRegionDialog.h"

namespace KoChart
{

class CellRegionDialog : public QDialog, public Ui::CellRegionDialog
{
public:
    explicit CellRegionDialog(QWidget *parent = nullptr);
    ~CellRegionDialog();
};

} // Namespace KoChart

#endif // KCHART_CELL_REGION_DIALOG_H
