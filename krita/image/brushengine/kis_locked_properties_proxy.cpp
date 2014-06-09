#include "kis_locked_properties_proxy.h"

KisLockedPropertiesProxy ::KisLockedPropertiesProxy ()
{
    m_lockedProperties = NULL;
    m_parent = NULL;
}
KisLockedPropertiesProxy::KisLockedPropertiesProxy(KisLockedProperties* p)
{
    m_lockedProperties = p;
}
KisLockedPropertiesProxy::KisLockedPropertiesProxy(const KisPropertiesConfiguration *p, KisLockedProperties *l)
{
    m_lockedProperties = l;
    m_parent = p;
}
QVariant KisLockedPropertiesProxy::getProperty(const QString &name) const
{

   if(m_lockedProperties->lockedProperties())
    {
       if(m_lockedProperties->lockedProperties()->hasProperty(name))
        {
           KisPropertiesConfiguration* temp = const_cast<KisPropertiesConfiguration*>(m_parent);
           temp->setProperty(name,m_lockedProperties->lockedProperties()->getProperty(name));
           return m_lockedProperties->lockedProperties()->getProperty(name);

        }
    }
    return m_parent->getProperty(name);
}
void KisLockedPropertiesProxy::setProperty(const QString & name, const QVariant & value)
{
    if(m_lockedProperties->lockedProperties())
     {
        if(m_lockedProperties->lockedProperties()->hasProperty(name))
         {

             m_lockedProperties->lockedProperties()->setProperty(name,value);
         }
     }
    KisPropertiesConfiguration* temp = const_cast<KisPropertiesConfiguration*>(m_parent);
    temp->setProperty(name,value);
}


