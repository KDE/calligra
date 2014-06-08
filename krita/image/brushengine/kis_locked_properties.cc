#include "kis_locked_properties.h"


KisLockedProperties::KisLockedProperties()
{
    m_lockedProperties = new KisPropertiesConfiguration();
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
    return m_lockedProperties;
}
