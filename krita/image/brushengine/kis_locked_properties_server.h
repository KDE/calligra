#ifndef KIS_LOCKED_PROPERTIES_SERVER_H
#define KIS_LOCKED_PROPERTIES_SERVER_H

#include"kis_locked_properties.h"
#include"kis_locked_properties_proxy.h"
#include"kis_properties_configuration.h"

class KRITAIMAGE_EXPORT KisLockedPropertiesServer: public QObject
{
public:
    KisLockedPropertiesServer();
    static KisLockedPropertiesServer* instance();
    KisLockedPropertiesProxy* createLockedPropertiesProxy(KisPropertiesConfiguration*);
    KisLockedProperties* lockedProperties();

private:
    KisLockedProperties* m_lockedProperties;
};

#endif // KIS_LOCKED_PROPERTIES_SERVER_H
