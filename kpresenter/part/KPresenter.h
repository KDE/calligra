/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRESENTER_H
#define KPRESENTER_H

#define TextShape_SHAPEID "TextShapeID"

#include <KoResourceManager.h>

/// The kpresenter-global namespace for all KPresenter related things.
namespace KPresenter
{
    /// Specifies the KPresenter specific resources that you can put and get from the KoResourceManager
    enum KPresenterDocumentResources {
        SoundCollection = KoDocumentResource::KPresenterStart + 1,
        PageLayouts ///< of type KPrPageLayouts
    };
}

#endif // KPRESENTER_H
