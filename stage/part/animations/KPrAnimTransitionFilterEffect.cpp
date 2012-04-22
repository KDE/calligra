#include "KPrAnimTransitionFilterEffect.h"

#include "KPrAnimTransitionFilterStrategy.h"

KPrAnimTransitionFilterEffect::KPrAnimTransitionFilterEffect(int duration, const QString &id, KPrAnimTransitionFilterStrategy *strategy)
    : m_duration( duration )
    , m_id( id )
    , m_strategy( strategy )
{
}
