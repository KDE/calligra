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
KisLockedPropertiesProxy::KisLockedPropertiesProxy(KisPropertiesConfiguration *p, KisLockedProperties *l)
{
    m_lockedProperties = l;
    m_parent = p;
}

bool KisLockedPropertiesProxy::getBool(const QString &name, bool def) const
{
    qDebug("Overridden Setting");
    return true;
}
