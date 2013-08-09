#ifndef FAKE_KCMDLINEARGS_H
#define FAKE_KCMDLINEARGS_H

#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <kurl.h>
#include <klocale.h>

#include "kofake_export.h"


class KCmdLineOptions
{
public:
    KCmdLineOptions() {}
    KCmdLineOptions &add(const QByteArray &name, const KLocalizedString &description = KLocalizedString(), const QByteArray &defaultValue = QByteArray()) { return *this; }
    KCmdLineOptions &add(const KCmdLineOptions &options) { return *this; }
};

class KCmdLineArgsList;
class QApplication;
class KAboutData;

class KCmdLineArgs
{
public:
    enum StdCmdLineArg {
        CmdLineArgQt = 0x01,
        CmdLineArgKDE = 0x02,
        CmdLineArgsMask=0x03,
        CmdLineArgNone = 0x00,
        Reserved = 0xff
    };
    Q_DECLARE_FLAGS(StdCmdLineArgs, StdCmdLineArg)

    static void init(int argc, char **argv,
                    const QByteArray &appname,
                    const QByteArray &catalog,
                    const KLocalizedString &programName,
                    const QByteArray &version,
                    const KLocalizedString &description = KLocalizedString(),
                    StdCmdLineArgs stdargs=StdCmdLineArgs(CmdLineArgQt|CmdLineArgKDE)) {}
    static void init(int _argc,
                    char **_argv,
                    const KAboutData *about,
                    StdCmdLineArgs stdargs=StdCmdLineArgs(CmdLineArgQt|CmdLineArgKDE)) {}
    static void init(const KAboutData *about) {}

    static void addStdCmdLineOptions(StdCmdLineArgs stdargs=StdCmdLineArgs(CmdLineArgQt|CmdLineArgKDE)) {}
    static void addCmdLineOptions(const KCmdLineOptions &options,
                                const KLocalizedString &name = KLocalizedString(),
                                const QByteArray &id = QByteArray(),
                                const QByteArray &afterId = QByteArray()) {}

    static KCmdLineArgs *parsedArgs(const QByteArray &id = QByteArray()) { return 0; }

    static QString cwd() { return QString(); }
    static QString appName() { return QString(); }
    static void usage(const QByteArray &id = QByteArray()) {}
    static void usageError(const QString &error) {}
    static void enable_i18n() {}

    QString getOption(const QByteArray &option) const { return QString(); }
    QStringList getOptionList(const QByteArray &option) const { return QStringList(); }
    bool isSet(const QByteArray &option) const { return false; }
    int count() const { return 0; }
    QString arg(int n) const { return QString(); }
    KUrl url(int n) const { return KUrl(); }
    static KUrl makeURL( const QByteArray &urlArg ) { return KUrl(); }
    static void setCwd( const QByteArray &cwd ) {}
    void clear() {}
    static void reset() {}
    static void loadAppArgs( QDataStream &) {}
    static void saveAppArgs( QDataStream &) {}
    static void addTempFileOption() {}
    static bool isTempFileSet() { return false; }
    static int &qtArgc() { static int m_qtArgc = 0; return m_qtArgc; }
    static char **qtArgv() {
        static char ** m_qtArgv = 0;
        static QByteArray m_appFilePath;
        if (!m_qtArgv) {
            m_qtArgv = new char*[2];
            m_appFilePath = QCoreApplication::applicationFilePath().toLocal8Bit();
            m_qtArgv[0] = m_appFilePath.data();
            m_qtArgv[1] = 0;
        }
        return m_qtArgv;
    }
    static QStringList allArguments() { return QStringList(); }

    static const KAboutData *aboutData() { return 0; }
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KCmdLineArgs::StdCmdLineArgs)

#endif
