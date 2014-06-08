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
    qDebug("In Inherited property");
    if(m_lockedProperties->lockedProperties()->hasProperty(name))
    {
        return m_lockedProperties->lockedProperties()->getProperty(name);
    }
    else
    {
        return m_parent->getProperty(name);
    }

}

