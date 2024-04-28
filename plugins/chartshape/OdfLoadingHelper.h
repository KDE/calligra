/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCHART_ODF_LOADING_HELPER_H
#define KCHART_ODF_LOADING_HELPER_H

// Qt
#include <QString>

// Calligra
#include "KoSharedLoadingData.h"
#include "KoXmlReader.h"

// KoChart
#include "TableSource.h"

class KoodfStylesReader;

namespace KoChart
{

class OdfLoadingHelper : public KoSharedLoadingData
{
public:
    OdfLoadingHelper();

    TableSource *tableSource;
    bool chartUsesInternalModelOnly;
    // In ODF the x axis determines the cell-range-address for categories in
    // the chart, we however let ChartProxyModel determine it, thus we need
    // a way to tell it that it will find it in the axis.
    bool categoryRegionSpecifiedInXAxis;
};

} // namespace KoChart

#endif // KCHART_ODF_LOADING_HELPER_H
