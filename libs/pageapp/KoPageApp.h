/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPAGEAPP_H
#define KOPAGEAPP_H

#include <KoCanvasResourceManager.h>

/// add docs please
namespace KoPageApp
{
/// add docs please
enum PageNavigation { PageFirst, PagePrevious, PageNext, PageLast };

/**
 * This enum holds identifiers to the resources that can be stored in the KoCanvasResourceManager.
 */
enum CanvasResource {
    CurrentPage = KoCanvasResourceManager::KoPageAppStart + 1 ///< The current page as a KoShape
};

/**
 * This enum defines if we should talk about pages or slides in the document
 */
enum PageType { Page, Slide };
}

#endif // KOPAGEAPP_H
