/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CellRegionDialog.h"

using namespace KoChart;

CellRegionDialog::CellRegionDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

CellRegionDialog::~CellRegionDialog() = default;
