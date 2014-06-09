#ifndef KIS_LOCKED_PROPERTIES_PROXY_H
#define KIS_LOCKED_PROPERTIES_PROXY_H

#include "kis_locked_properties.h"
#include "kis_properties_configuration.h"

class KisLockedPropertiesProxy: public KisPropertiesConfiguration
{
public:
    KisLockedPropertiesProxy() ;
    KisLockedPropertiesProxy(KisLockedProperties* p);
    KisLockedPropertiesProxy( const KisPropertiesConfiguration *, KisLockedProperties *);
    using KisPropertiesConfiguration::getProperty;
    QVariant getProperty(const QString &name) const;
    using KisPropertiesConfiguration::setProperty;
    void setProperty(const QString & name, const QVariant & value);






private:
    KisLockedProperties* m_lockedProperties;
    const KisPropertiesConfiguration* m_parent;

};

#endif // KIS_LOCKED_PROPERTIES_PROXY_H
