#include "KPrAnimTransitionFilterStrategy.h"

#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoGenStyle.h>

KPrAnimTransitionFilterStrategy::KPrAnimTransitionFilterStrategy(int subType, const char * smilType, const char *smilSubType, bool reverse, bool graphicsView)
    : m_subType( subType )
    , m_smilData( smilType, smilSubType, reverse )
{
}

KPrAnimTransitionFilterStrategy::~KPrAnimTransitionFilterStrategy()
{
}

void KPrAnimTransitionFilterStrategy::finish( const KPrAnimTransitionFilterEffect::Data &data )
{

}

void KPrAnimTransitionFilterStrategy::saveOdfSmilAttributes( KoXmlWriter & xmlWriter ) const
{

}

void KPrAnimTransitionFilterStrategy::saveOdfSmilAttributes( KoGenStyle & style ) const
{

}

void KPrAnimTransitionFilterStrategy::loadOdfSmilAttributes( const KoXmlElement & element )
{
    Q_UNUSED( element );
}

int KPrAnimTransitionFilterStrategy::subType() const
{
    return m_subType;
}

const QString & KPrAnimTransitionFilterStrategy::smilType() const
{
    return m_smilData.type;
}

const QString & KPrAnimTransitionFilterStrategy::smilSubType() const
{
    return m_smilData.subType;
}


bool KPrAnimTransitionFilterStrategy::reverse() const
{
    return m_smilData.reverse;
}
