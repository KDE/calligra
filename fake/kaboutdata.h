#ifndef FAKE_KABOUTDATA_H
#define FAKE_KABOUTDATA_H

#include <QCoreApplication>
#include <QList>
#include <QVariant>
#include <klocale.h>

#include "kofake_export.h"


class KAboutLicense
{
public:
};

class KAboutPerson
{
public:
    KAboutPerson( const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress, const QByteArray &ocsUsername = QByteArray() ) : m_name(name), m_task(task), m_emailAddress(emailAddress), m_webAddress(webAddress), m_ocsUsername(ocsUsername) {}
    KLocalizedString name() const { return m_name; }
    KLocalizedString task() const { return m_task; }
    QByteArray emailAddress() const { return m_emailAddress; }
    QByteArray webAddress() const { return m_webAddress; }
    QByteArray ocsUsername() const { return m_ocsUsername; }
private:
    KLocalizedString m_name, m_task;
    QByteArray m_emailAddress, m_webAddress, m_ocsUsername;
};

class KAboutData
{
public:
    enum LicenseKey { License_Custom = -2, License_File = -1, License_Unknown = 0, License_GPL  = 1, License_GPL_V2 = 1, License_LGPL = 2, License_LGPL_V2 = 2, License_BSD  = 3, License_Artistic = 4, License_QPL = 5, License_QPL_V1_0 = 5, License_GPL_V3 = 6, License_LGPL_V3 = 7 };
    enum NameFormat { ShortName, FullName };

    KAboutData() { init(); }
    KAboutData( const QByteArray &, const QByteArray &, const KLocalizedString &, const QByteArray &, const KLocalizedString &shortDescription = KLocalizedString(), enum LicenseKey licenseType = License_Unknown, const KLocalizedString &copyrightStatement = KLocalizedString(), const KLocalizedString &otherText = KLocalizedString(), const QByteArray &homePageAddress = QByteArray(), const QByteArray &bugsEmailAddress = QByteArray())
    {
        Q_UNUSED(shortDescription);
        Q_UNUSED(licenseType);
        Q_UNUSED(copyrightStatement);
        Q_UNUSED(otherText);
        Q_UNUSED(bugAddress());
        Q_UNUSED(homePageAddress);
        Q_UNUSED(bugsEmailAddress);

        init();
    }

    QString appName() const { return qApp->applicationName(); }
    QString productName() const { return appName(); }
    QString programName() const { return appName(); }
    QString organizationDomain() const { return QString(); }
    const char* internalProgramName() const { return m_name.constData(); }
    QString programIconName() const { return QString(); }
    QVariant programLogo() const { return QVariant(); }
    QString ocsProviderUrl() const { return QString(); }
    QString version() const { return qApp->applicationVersion(); }
    const char* internalVersion() const { return m_version.constData(); }
    QString shortDescription() const { return QString(); }
    QString catalogName() const { return QString(); }
    QString homepage() const { return QString(); }
    QString bugAddress() const { return QString(); }
    const char* internalBugAddress() const { return 0; }
    QList<KAboutPerson> authors() const { return m_authors; }
    QList<KAboutPerson> credits() const { return m_credits; }
    QList<KAboutPerson> translators() const { return QList<KAboutPerson>(); }
    static QString aboutTranslationTeam() { return QString(); }
    QString otherText() const { return QString(); }
    QString license() const { return QString(); }
    QString licenseName(NameFormat ) const { return QString(); }
    QList<KAboutLicense> licenses() const { return QList<KAboutLicense>(); }
    QString copyrightStatement() const { return QString(); }
    QString customAuthorPlainText() const { return QString(); }
    QString customAuthorRichText() const { return QString(); }
    bool customAuthorTextEnabled() const{ return false; }

    KAboutData &addAuthor( const KLocalizedString &name, const KLocalizedString &task = KLocalizedString(), const QByteArray &emailAddress = QByteArray(), const QByteArray &webAddress = QByteArray(), const QByteArray &ocsUsername = QByteArray() ) {
        m_authors.append(KAboutPerson(name, task, emailAddress, webAddress, ocsUsername));
        return *this;
    }
    KAboutData &addCredit( const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress, const QByteArray &ocsUsername ) {
        m_credits.append(KAboutPerson(name, task, emailAddress, webAddress, ocsUsername));
        return *this;
    }
    KAboutData &setTranslator( const KLocalizedString& , const KLocalizedString& ) { return *this; }
    KAboutData &setLicenseText( const KLocalizedString &) { return *this; }
    KAboutData &addLicenseText( const KLocalizedString &) { return *this; }
    KAboutData &setLicenseTextFile( const QString &) { return *this; }
    KAboutData &addLicenseTextFile( const QString &) { return *this; }
    KAboutData &setAppName( const QByteArray & ) { return *this; }
    KAboutData &setProgramName( const KLocalizedString & ) { return *this; }
    KAboutData &setProgramIconName( const QString &) { return *this; }
    KAboutData &setProgramLogo(const QVariant& ) { return *this; }
    KAboutData &setOcsProvider( const QByteArray &) { return *this; }
    KAboutData &setVersion( const QByteArray & ) { return *this; }
    KAboutData &setShortDescription( const KLocalizedString &) { return *this; }
    KAboutData &setCatalogName( const QByteArray & ) { return *this; }
    KAboutData &setLicense( LicenseKey  ) { return *this; }
    KAboutData &addLicense( LicenseKey ) { return *this; }
    KAboutData &setCopyrightStatement( const KLocalizedString &) { return *this; }
    KAboutData &setOtherText( const KLocalizedString & ) { return *this; }
    KAboutData &setHomepage( const QByteArray & ) { return *this; }
    KAboutData &setBugAddress( const QByteArray &) { return *this; }
    KAboutData &setOrganizationDomain( const QByteArray &) { return *this; }
    KAboutData &setProductName( const QByteArray &) { return *this; }

private:
    QByteArray m_name, m_version;
    QList<KAboutPerson> m_authors, m_credits;

    void init() {
        m_name = qApp->applicationName().toUtf8();
        m_version = qApp->applicationVersion().toUtf8();
    }
};

#endif
