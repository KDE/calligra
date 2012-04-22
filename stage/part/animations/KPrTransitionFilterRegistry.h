#ifndef KPRTRANSITIONFILTERREGISTRY_H
#define KPRTRANSITIONFILTERREGISTRY_H

#include <KoGenericRegistry.h>
#include "KPrAnimTransitionFilterFactory.h"
#include "KoXmlReaderForward.h"

class KPrAnimTransitionFilterEffect;

class STAGE_EXPORT KPrTransitionFilterRegistry : public KoGenericRegistry<KPrAnimTransitionFilterFactory *>
{
public:
    class Singleton;
    static KPrTransitionFilterRegistry *instance();

   KPrAnimTransitionFilterEffect *createTransitionFilterEffect(const KoXmlElement &element);

private:
    void init();
    KPrTransitionFilterRegistry();
    ~KPrTransitionFilterRegistry();

    friend class Singleton;

    struct Private;
    Private * const d;
};

#endif // KPRTRANSITIONFILTERREGISTRY_H
