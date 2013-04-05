#ifndef FAKE_KSHAREDCONFIG_H
#define FAKE_KSHAREDCONFIG_H

#include <kconfig.h>

#include <QSharedData>
#include <QSharedDataPointer>

class KComponentData;

class KSharedConfig : public KConfig, public QSharedData
{
public:
    typedef QSharedDataPointer<KSharedConfig> Ptr;

    KSharedConfig(const QString &file) : KConfig(file) {}
    virtual ~KSharedConfig() {}

    void reparseConfiguration() {}

    static KSharedConfig::Ptr openConfig(const QString& fileName = QString(), OpenFlags mode = FullConfig, const char *resourceType = "config") { Q_UNUSED(fileName); Q_UNUSED(mode); Q_UNUSED(resourceType); return Ptr(); }
    static KSharedConfig::Ptr openConfig(const KComponentData &componentData, const QString &fileName = QString(), OpenFlags mode = FullConfig, const char *resourceType = "config") { Q_UNUSED(componentData); Q_UNUSED(fileName); Q_UNUSED(mode); Q_UNUSED(resourceType);return Ptr(); }

};

typedef KSharedConfig::Ptr KSharedConfigPtr;

#endif
