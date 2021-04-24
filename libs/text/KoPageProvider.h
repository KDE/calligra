/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAGEPROVIDER_H
#define KOPAGEPROVIDER_H

#include "kotext_export.h"

class KoShape;
class KoTextPage;

/// \internal  this is a hack for Stage
class KOTEXT_EXPORT KoPageProvider
{
public:
    KoPageProvider();
    virtual ~KoPageProvider();

    /**
     * Get the page number for the given shape
     */
    virtual KoTextPage *page(KoShape *shape) = 0;
};
#endif // KOPAGEPROVIDER_H
