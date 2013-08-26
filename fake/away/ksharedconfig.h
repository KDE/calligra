#ifndef FAKE_KSHAREDCONFIG_H
#define FAKE_KSHAREDCONFIG_H

#include <kconfig.h>

#include <QMap>
#include <QString>
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

    static KSharedConfig::Ptr openConfig(const QString& fileName = QString(), OpenFlags mode = FullConfig, const char *resourceType = "config") {
        qDebug() << Q_FUNC_INFO << fileName << mode << resourceType;
        return Ptr(new KSharedConfig(fileName));
    }
    static KSharedConfig::Ptr openConfig(const KComponentData &componentData, const QString &fileName = QString(), OpenFlags mode = FullConfig, const char *resourceType = "config") {
        qDebug() << Q_FUNC_INFO << fileName << mode << resourceType;
        return Ptr(new KSharedConfig(fileName));
    }

};

typedef KSharedConfig::Ptr KSharedConfigPtr;

#endif
