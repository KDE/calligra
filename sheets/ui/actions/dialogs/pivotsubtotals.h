/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local

#ifndef PIVOTSUBTOTALS_H
#define PIVOTSUBTOTALS_H

#include <KoDialog.h>

namespace Calligra;
namespace Sheets
{

class Selection;

class PivotSubTotals : public KoDialog
{
    Q_OBJECT

public:
    explicit PivotSubTotals(QWidget *parent, Selection *selection);
    ~PivotSubTotals();

private:
    void selectLabels();
    class Private;
    Private *const d;
};

#endif // PIVOTSUBTOTALS_H
