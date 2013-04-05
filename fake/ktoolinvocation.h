#ifndef FAKE_KTOOLINVOCATION_H
#define FAKE_KTOOLINVOCATION_H

#include <QObject>
#include <QProcess>
#include <QDebug>

class OrgKdeKLauncherInterface;

class KToolInvocation
{
public:
    static KToolInvocation *self()
    {
        static KToolInvocation *s_instance = 0;
        if (!s_instance)
            s_instance = new KToolInvocation();
        return s_instance;
    }

    static void invokeHelp( const QString& anchor = QString(), const QString& appname = QString(), const QByteArray& startup_id = QByteArray()) {}
    static void invokeMailer( const QString &address, const QString &subject, const QByteArray& startup_id = QByteArray() ) {}
    static void invokeMailer( const KUrl &mailtoURL, const QByteArray& startup_id = QByteArray(), bool allowAttachments = false ) {}
    static void invokeMailer(const QString &to, const QString &cc, const QString &bcc, const QString &subject, const QString &body, const QString &messageFile = QString(), const QStringList &attachURLs = QStringList(), const QByteArray& startup_id = QByteArray() ) {}
    static void invokeBrowser( const QString &url, const QByteArray& startup_id = QByteArray() ) {}
    static void invokeTerminal(const QString &command, const QString& workdir = QString(), const QByteArray &startup_id = "") {}
    static OrgKdeKLauncherInterface *klauncher() { return 0; }
    static int startServiceByDesktopPath( const QString& _name, const QString &URL, QString *error=0, QString *serviceName=0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false ) { return 0; }
    static int startServiceByDesktopPath( const QString& _name, const QStringList &URLs=QStringList(), QString *error=0, QString *serviceName=0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false ) { return 0; }
    static int startServiceByDesktopName( const QString& _name, const QString &URL, QString *error=0, QString *serviceName=0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false ) { return 0; }
    static int startServiceByDesktopName( const QString& _name, const QStringList &URLs=QStringList(), QString *error=0, QString *serviceName=0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false ) { return 0; }
    static int kdeinitExec( const QString& name, const QStringList &args=QStringList(), QString *error=0, int *pid = 0, const QByteArray& startup_id = QByteArray() )
    {
        QProcess p;
        if (!p.startDetached(name, args)) {
            if (error)
                *error = QString("Failed to execute \"%1\"").arg(name);
            return 1;
        }
        return 0;
    }
    static int kdeinitExecWait( const QString& name, const QStringList &args=QStringList(), QString *error=0, int *pid = 0, const QByteArray& startup_id = QByteArray() ) { return 0; }

};

#endif
