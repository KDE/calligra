/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSHAPEANIMATIONREGISTRY_H
#define KPRSHAPEANIMATIONREGISTRY_H

#include <KoGenericRegistry.h>

#include "KoXmlReaderForward.h"
class KPrShapeAnimationOld;
class KPrShapeAnimationFactory;

class KPrShapeAnimationRegistry : public KoGenericRegistry<KPrShapeAnimationFactory *>
{
public:
    class Singleton;
    static KPrShapeAnimationRegistry * instance();

    KPrShapeAnimationOld * createShapeAnimation( const KoXmlElement & element );

private:
    friend class Singleton;
    KPrShapeAnimationRegistry();
    ~KPrShapeAnimationRegistry();
};

#endif /* KPRSHAPEANIMATIONREGISTRY_H */
