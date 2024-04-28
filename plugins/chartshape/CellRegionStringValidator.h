/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_CELLREGIONSTRINGVALIDATOR_H
#define KCHART_CELLREGIONSTRINGVALIDATOR_H

// Qt
#include <QValidator>

// Interface
#include <interfaces/KoChartModel.h>

namespace KoChart
{

class CellRegionStringValidator : public QValidator
{
public:
    explicit CellRegionStringValidator(KoChart::ChartModel *model);
    ~CellRegionStringValidator();

    State validate(QString &string, int &pos) const override;

private:
    class Private;
    Private *const d;
};

}

#endif // KCHART_CELLREGIONSTRINGVALIDATOR_H
