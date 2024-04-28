/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCANVASRESOURCEIDENTITIES_H
#define KOCANVASRESOURCEIDENTITIES_H

#include "KoCanvasResourceManager.h"

/**
 * Register the applivations canvas resources here
 * to make them accessible by plugins.
 *
 * @see KoCanvasResourceManager::CanvasResource
 */

namespace Sheets
{
namespace CanvasResource
{
enum Identities {
    Selection = KoCanvasResourceManager::SheetsStart, ///< the cell selection
    MapResource, ///< the document data
    AccessModel ///< the data table access model
};
}
}

#endif
