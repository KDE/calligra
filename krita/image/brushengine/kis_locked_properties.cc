#include "kis_locked_properties.h"


KisLockedProperties::KisLockedProperties()
{
    m_lockedProperties = new KisPropertiesConfiguration();
    m_lockedProperties->setProperty("OpacityValue",0.34);
    m_lockedProperties->setProperty("FlowValue",0.56);
}

KisLockedProperties::KisLockedProperties(KisPropertiesConfiguration *p)
{
    m_lockedProperties = new KisPropertiesConfiguration();
    QMap<QString,QVariant>::Iterator i;
    for(i = p->getProperties().begin();i!=p->getProperties().end();i++)
    {
        m_lockedProperties->setProperty(i.key(),i.value());
    }
}
void KisLockedProperties::addToLockedProperties(KisPropertiesConfiguration *p)
{
    QMap<QString,QVariant>::Iterator i;
    for(i = p->getProperties().begin();i!=p->getProperties().end();i++)
    {
        m_lockedProperties->setProperty(i.key(),i.value());
    }
}
void KisLockedProperties::removeFromLockedProperties(KisPropertiesConfiguration *p)
{
    KisPropertiesConfiguration *temp = new KisPropertiesConfiguration();
    QMap<QString,QVariant>::Iterator i;
    for(i = m_lockedProperties->getProperties().begin();i!=m_lockedProperties->getProperties().end();i++)
    {
        temp->setProperty(i.key(),i.value());
    }
    m_lockedProperties->clearProperties();
    for(i = temp->getProperties().begin();i!=temp->getProperties().end();i++)
    {
        if(!p->hasProperty(i.key()))
        {
            m_lockedProperties->setProperty(i.key(),i.value());
        }
    }
}
KisPropertiesConfiguration *KisLockedProperties::lockedProperties()
{

    if(!m_lockedProperties)
    {
        m_lockedProperties = new KisPropertiesConfiguration();
    }

    return m_lockedProperties;
}
