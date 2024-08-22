/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "NewAxisDialog.h"

using namespace KoChart;

NewAxisDialog::NewAxisDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

NewAxisDialog::~NewAxisDialog() = default;
