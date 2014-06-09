#include "kis_locked_properties_server.h"

KisLockedPropertiesServer::KisLockedPropertiesServer()
{
    m_lockedProperties = new KisLockedProperties();
}

KisLockedPropertiesProxy* KisLockedPropertiesServer::createLockedPropertiesProxy(const KisPropertiesConfiguration* config)
{
    KisLockedPropertiesProxy* m = new KisLockedPropertiesProxy(config,lockedProperties());
    return m;
}
KisLockedPropertiesServer* KisLockedPropertiesServer::instance()
{
    K_GLOBAL_STATIC(KisLockedPropertiesServer, s_instance);

    if(s_instance)
    {
        return s_instance;
    }

    return NULL;
}
KisLockedProperties* KisLockedPropertiesServer::lockedProperties()
{
    return m_lockedProperties;
}

