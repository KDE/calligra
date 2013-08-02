#ifndef FAKE_KPLUGININFO_H
#define FAKE_KPLUGININFO_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QDebug>

#include <kservice.h>
#include <kconfiggroup.h>

class KPluginInfo
{
public:
    typedef QList<KPluginInfo> List;

    KPluginInfo( const QString & filename, const char* resource = 0 ) {}
    KPluginInfo( const KService::Ptr service ) {}
    KPluginInfo() {}
    ~KPluginInfo() {}

    static KPluginInfo::List fromServices(const KService::List &services, const KConfigGroup &config = KConfigGroup()) { return List(); }
    static KPluginInfo::List fromFiles(const QStringList &files, const KConfigGroup &config = KConfigGroup()) { return List(); }
    static KPluginInfo::List fromKPartsInstanceName(const QString &componentName, const KConfigGroup &config = KConfigGroup()) { return List(); }

    bool isHidden() const { return false; }
    void setPluginEnabled(bool enabled) {}
    bool isPluginEnabled() const { return true; }
    bool isPluginEnabledByDefault() const { return true; }
    QVariant property( const QString & key ) const { return QVariant(); }
    QString name() const { return QString(); }
    QString comment() const { return QString(); }
    QString icon() const { return QString(); }
    QString entryPath() const { return QString(); }
    QString author() const { return QString(); }
    QString email() const { return QString(); }
    QString category() const { return QString(); }
    QString pluginName() const { return QString(); }
    QString version() const { return QString(); }
    QString website() const { return QString(); }
    QString license() const { return QString(); }
    //KAboutLicense fullLicense() const { return KAboutLicense(); }
    QStringList dependencies() const { return QStringList(); }
    KService::Ptr service() const { return KService::Ptr(); }
    QList<KService::Ptr> kcmServices() const { return QList<KService::Ptr>(); }
    void setConfig(const KConfigGroup &config) {}
    KConfigGroup config() const { return KConfigGroup(); }
    void save(KConfigGroup config = KConfigGroup()) {}
    void load(const KConfigGroup &config = KConfigGroup()) {}
    void defaults() {}
    bool isValid() const { return true; }

    //KPluginInfo(const KPluginInfo &copy)
    //KPluginInfo &operator=(const KPluginInfo &rhs);
    //bool operator==(const KPluginInfo &rhs) const;
    //bool operator!=(const KPluginInfo &rhs) const;
    //bool operator<(const KPluginInfo &rhs) const;
    //bool operator>(const KPluginInfo &rhs) const;
};

#endif
