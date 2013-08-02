#ifndef FAKE_KRUN_H
#define FAKE_KRUN_H

#include <QObject>
#include <QWidget>
#include <kurl.h>
#include <kservice.h>

class KRun : public QObject
{
public:
    KRun(const KUrl& url, QWidget* window, mode_t mode = 0,
         bool isLocalFile = false, bool showProgressInfo = true,
         const QByteArray& asn = QByteArray()) : QObject(window) {}
    virtual ~KRun() {}
    void abort() {}
    bool hasError() const { return false; }
    bool hasFinished() const { return true; }
    bool autoDelete() const  { return false; }
    void setAutoDelete(bool b) {}
    void setPreferredService(const QString& desktopEntryName) {}
    void setRunExecutables(bool b) {}
    void setEnableExternalBrowser(bool b) {}
    void setSuggestedFileName(const QString& fileName) {}
    QString suggestedFileName() const { return QString(); }
    static bool run(const KService& service, const KUrl::List& urls, QWidget* window, bool tempFiles = false, const QString& suggestedFileName = QString(), const QByteArray& asn = QByteArray())
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }
    static bool run(const QString& exec, const KUrl::List& urls, QWidget* window, const QString& name = QString(), const QString& icon = QString(), const QByteArray& asn = QByteArray())
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }
    static bool runUrl(const KUrl& url, const QString& mimetype, QWidget* window, bool tempFile = false , bool runExecutables = true, const QString& suggestedFileName = QString(), const QByteArray& asn = QByteArray())
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }
    static bool runCommand(const QString &cmd, QWidget* window)
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }
    static bool runCommand(const QString &cmd, QWidget* window, const QString& workingDirectory)
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }
    static bool runCommand(const QString& cmd, const QString & execName, const QString & icon, QWidget* window, const QByteArray& asn = QByteArray())
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }
    static bool runCommand(const QString& cmd, const QString & execName, const QString & icon, QWidget* window, const QByteArray& asn, const QString& workingDirectory)
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }
    static bool displayOpenWithDialog(const KUrl::List& lst, QWidget* window, bool tempFiles = false, const QString& suggestedFileName = QString(), const QByteArray& asn = QByteArray())
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return false;
    }
    static QStringList processDesktopExec(const KService &_service, const KUrl::List &_urls, bool tempFiles = false, const QString& suggestedFileName = QString())
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return QStringList();
    }
    static QString binaryName(const QString & execLine, bool removePath)
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return QString();
    }

    static bool isExecutable(const QString& serviceType);
    static bool isExecutableFile(const QUrl& url, const QString &mimetype);

    static bool checkStartupNotify(const QString& binName, const KService* service, bool* silent_arg, QByteArray* wmclass_arg)
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        return false;
    }

#if 0
Q_SIGNALS:
    void finished();
    void error();
#endif
};

#endif
 
