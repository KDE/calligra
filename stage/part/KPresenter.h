/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRESENTER_H
#define KPRESENTER_H

#define TextShape_SHAPEID "TextShapeID"

#include <KoDocumentResourceManager.h>

/// The stage-global namespace for all Stage related things.
namespace KPresenter
{
/// Specifies the Stage specific resources that you can put and get from the KoDocumentResourceManager
enum KPresenterDocumentResources {
    SoundCollection = KoDocumentResourceManager::StageStart + 1,
    PageLayouts ///< of type KPrPageLayouts
};
}

#endif // KPRESENTER_H
