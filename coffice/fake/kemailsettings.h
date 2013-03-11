#ifndef FAKE_KEMAILSETTINGS_H
#define FAKE_KEMAILSETTINGS_H

#include <QString>
#include <QStringList>
#include <kconfiggroup.h>

class KEMailSettings
{
public:
    enum Setting {
        ClientProgram,
        ClientTerminal,
        RealName,
        EmailAddress,
        ReplyToAddress,
        Organization,
        OutServer,
        OutServerLogin,
        OutServerPass,
        OutServerType,
        OutServerCommand,
        OutServerTLS,
        InServer,
        InServerLogin,
        InServerPass,
        InServerType,
        InServerMBXType,
        InServerTLS
    };

    enum Extension {
        POP3,
        SMTP,
        OTHER
    };

    QStringList profiles() const { return QStringList(); }
    QString currentProfileName() const { return QString(); }
    void setProfile (const QString &s) {}
    QString defaultProfileName() const { return QString(); }
    void setDefault(const QString &def) {}
    QString getSetting(KEMailSettings::Setting s) const { return QString(); }
    void setSetting(KEMailSettings::Setting s, const QString &v) {}
};
    
#endif
 
