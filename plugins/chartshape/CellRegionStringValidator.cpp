/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "CellRegionStringValidator.h"

// KoChart
#include "CellRegion.h"

using namespace KoChart;

class CellRegionStringValidator::Private
{
public:
    Private();
    ~Private();

    KoChart::ChartModel *model;
    // A regular expression that characterizes any possible
    // intermediate state of a valid cell region string
    QRegularExpression intermediatelyCorrectRegion;
};

CellRegionStringValidator::Private::Private()
    : intermediatelyCorrectRegion("[a-zA-Z0-9$.:;]*")
{
}

CellRegionStringValidator::Private::~Private() = default;

CellRegionStringValidator::CellRegionStringValidator(KoChart::ChartModel *model)
    : QValidator(nullptr)
    , d(new Private)
{
    d->model = model;
}

CellRegionStringValidator::~CellRegionStringValidator()
{
    delete d;
}

QValidator::State CellRegionStringValidator::validate(QString &string, int &pos) const
{
    Q_UNUSED(pos);

    if (!d->intermediatelyCorrectRegion.match(string).hasMatch())
        return QValidator::Invalid;

    if (d->model->isCellRegionValid(string))
        return QValidator::Acceptable;

    return QValidator::Intermediate;
}
