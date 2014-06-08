#ifndef KISLOCKEDPROPERTIES_H
#define KISLOCKEDPROPERTIES_H

#include "kis_properties_configuration.h"
#include "kis_paintop_settings.h"

class KisLockedProperties
{
public:
    KisLockedProperties();
    KisLockedProperties(KisPropertiesConfiguration *p);

    /**Whenever any setting is made locked**/
    void addToLockedProperties(KisPropertiesConfiguration *p);

    /**Whenever any property is unlocked**/
    void removeFromLockedProperties(KisPropertiesConfiguration *p);
    KisPropertiesConfiguration *lockedProperties();


private:
    KisPropertiesConfiguration* m_lockedProperties;
};

#endif // KISLOCKEDPROPERTIES_H
