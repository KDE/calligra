#ifndef FAKE_kprocess_H
#define FAKE_kprocess_H

#include <QProcess>
#include <QDebug>

class KProcess : public QProcess
{
public:
    enum OutputChannelMode {
        SeparateChannels = QProcess::SeparateChannels,
        MergedChannels = QProcess::MergedChannels,
        ForwardedChannels = QProcess::ForwardedChannels,
        OnlyStdoutChannel,
        OnlyStderrChannel
    };

    KProcess(QObject *parent = 0) : QProcess(parent), m_outputChannelMode(ForwardedChannels) {}
    virtual ~KProcess() {}

    /**
     * Set how to handle the output channels of the child process.
     *
     * The default is ForwardedChannels, which is unlike in QProcess.
     * Do not request more than you actually handle, as this output is
     * simply lost otherwise.
     *
     * This function must be called before starting the process.
     *
     * @param mode the output channel handling mode
     */
    void setOutputChannelMode(OutputChannelMode mode) { m_outputChannelMode = mode; }
    OutputChannelMode outputChannelMode() const { return m_outputChannelMode; }

#if 0
    /**
     * Set the QIODevice open mode the process will be opened in.
     *
     * This function must be called before starting the process, obviously.
     *
     * @param mode the open mode. Note that this mode is automatically
     *   "reduced" according to the channel modes and redirections.
     *   The default is QIODevice::ReadWrite.
     */
    void setNextOpenMode(QIODevice::OpenMode mode);

    /**
     * Adds the variable @p name to the process' environment.
     *
     * This function must be called before starting the process.
     *
     * @param name the name of the environment variable
     * @param value the new value for the environment variable
     * @param overwrite if @c false and the environment variable is already
     *   set, the old value will be preserved
     */
    void setEnv(const QString &name, const QString &value, bool overwrite = true);

    /**
     * Removes the variable @p name from the process' environment.
     *
     * This function must be called before starting the process.
     *
     * @param name the name of the environment variable
     */
    void unsetEnv(const QString &name);

    /**
     * Empties the process' environment.
     *
     * Note that LD_LIBRARY_PATH/DYLD_LIBRARY_PATH is automatically added
     * on *NIX.
     *
     * This function must be called before starting the process.
     */
    void clearEnvironment();
#endif

    QStringList program() const
    {
        return QStringList() << m_program << m_args;
    }
    void setProgram(const QString &exe, const QStringList &args = QStringList())
    {
        m_program = exe;
        m_args = args;
    }
    void setProgram(const QStringList &argv)
    {
        m_args = argv;
        m_program = m_args.takeFirst();
    }
    KProcess &operator<<(const QString& arg)
    {
        if (m_program.isEmpty())
            m_program = arg;
        else
            m_args << arg;
        return *this;
    }
    KProcess &operator<<(const QStringList& args)
    {
        if (m_program.isEmpty())
            setProgram(args);
        else
            m_args << args;
        return *this;
    }
    void clearProgram()
    {
        m_program.clear();
        m_args.clear();
    }

#if 0

    /**
     * Set a command to execute through a shell (a POSIX sh on *NIX
     * and cmd.exe on Windows).
     *
     * Using this for anything but user-supplied commands is usually a bad
     * idea, as the command's syntax depends on the platform.
     * Redirections including pipes, etc. are better handled by the
     * respective functions provided by QProcess.
     *
     * If KProcess determines that the command does not really need a
     * shell, it will trasparently execute it without one for performance
     * reasons.
     *
     * This function must be called before starting the process, obviously.
     *
     * @param cmd the command to execute through a shell.
     *   The caller must make sure that all filenames etc. are properly
     *   quoted when passed as argument. Failure to do so often results in
     *   serious security holes. See KShell::quoteArg().
     */
    void setShellCommand(const QString &cmd);
#endif

    void start()
    {
        QIODevice::OpenMode mode = QIODevice::ReadWrite;
        QProcess::start(m_program, m_args, mode);
    }
    int execute(int msecs = -1)
    {
        start();
        waitForFinished(msecs);
        return exitCode();
    }
    static int execute(const QString &exe, const QStringList &args = QStringList(), int msecs = -1)
    {
        KProcess p;
        p << exe << args;
        return p.execute(msecs);
    }
    static int execute(const QStringList &argv, int msecs = -1)
    {
        KProcess p;
        p << argv;
        return p.execute(msecs);
    }

    int startDetached()
    {
        qint64 pid;
        if (!QProcess::startDetached(m_program, m_args, workingDirectory(), &pid))
            return 0;
        return (int) pid;
    }
    static int startDetached(const QString &exe, const QStringList &args = QStringList())
    {
        KProcess p;
        p << exe << args;
        return p.startDetached();
    }
    static int startDetached(const QStringList &argv)
    {
        KProcess p;
        p << argv;
        return p.startDetached();
    }

private:
    OutputChannelMode m_outputChannelMode;
    QString m_program;
    QStringList m_args;
};

QDebug operator<<(QDebug dbg, const KProcess &p)
{
    dbg.nospace() << "KProcess(" << p.program() << ")";
    return dbg.space();
}

#endif
