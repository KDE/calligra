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

bool KisLockedPropertiesProxy::getBool(const QString &name, bool def = false) const
{
    qDebug(name.toLatin1());
    return true;
}
void KisLockedPropertiesProxy::sayMyName()
{
    qDebug("mohit");
}
