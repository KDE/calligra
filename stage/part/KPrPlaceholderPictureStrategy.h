/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPLACEHOLDERPICTURESTRATEGY_H
#define KPRPLACEHOLDERPICTURESTRATEGY_H

#include "KPrPlaceholderStrategy.h"

class KPrPlaceholderPictureStrategy : public KPrPlaceholderStrategy
{
public:
    ~KPrPlaceholderPictureStrategy() override;

    KoShape *createShape(KoDocumentResourceManager *documenResources) override;

protected:
    KPrPlaceholderPictureStrategy();

    friend class KPrPlaceholderStrategy;
};

#endif // KPRPLACEHOLDERPICTURESTRATEGY_H
