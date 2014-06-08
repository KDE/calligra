#include "kis_locked_properties_server.h"

KisLockedPropertiesServer::KisLockedPropertiesServer()
{
}

KisLockedPropertiesProxy* KisLockedPropertiesServer::createLockedPropertiesProxy(const KisPropertiesConfiguration* config)
{
    KisLockedPropertiesProxy* m = new KisLockedPropertiesProxy(config,m_lockedProperties);
    return m;
}
KisLockedPropertiesServer* KisLockedPropertiesServer::instance()
{
    K_GLOBAL_STATIC(KisLockedPropertiesServer, s_instance);
    if(s_instance)
    {
        return s_instance;
        qDebug("It is not null");
    }

    return NULL;
}
KisLockedProperties* KisLockedPropertiesServer::lockedProperties()
{
    return m_lockedProperties;
}

