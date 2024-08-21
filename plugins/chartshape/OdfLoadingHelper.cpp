/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "OdfLoadingHelper.h"

// Calligra
#include "KoOdfStylesReader.h"

using namespace KoChart;

OdfLoadingHelper::OdfLoadingHelper()
    : tableSource(nullptr)
    , chartUsesInternalModelOnly(true)
    , categoryRegionSpecifiedInXAxis(false)
{
}
