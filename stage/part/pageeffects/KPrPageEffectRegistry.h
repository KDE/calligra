/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPAGEEFFECTREGISTRY_H
#define KPRPAGEEFFECTREGISTRY_H

#include <KoGenericRegistry.h>

#include "KPrPageEffectFactory.h"

#include "KoXmlReaderForward.h"
class KPrPageEffect;

class STAGE_EXPORT KPrPageEffectRegistry : public KoGenericRegistry<KPrPageEffectFactory *>
{
public:
    class Singleton;
    static KPrPageEffectRegistry *instance();

    KPrPageEffect *createPageEffect(const KoXmlElement &element);

private:
    void init();

    KPrPageEffectRegistry();
    ~KPrPageEffectRegistry() override;

    friend class Singleton;

    struct Private;
    Private *const d;
};

#endif /* KPRPAGEEFFECTREGISTRY_H */
