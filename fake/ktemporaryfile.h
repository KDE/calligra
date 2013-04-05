#ifndef FAKE_KTEMPORARYFILE_H
#define FAKE_KTEMPORARYFILE_H

#include <QTemporaryFile>
#include <QDir>

class KTemporaryFile : public QTemporaryFile
{
public:
    void setPrefix(const QString &s) {
        m_prefix = s;
        setFileTemplate(QDir::tempPath() + m_prefix + QLatin1String("XXXXXX") + m_suffix);
    }
    void setSuffix(const QString &s) {
        m_suffix = s;
        setFileTemplate(QDir::tempPath() + m_prefix + QLatin1String("XXXXXX") + m_suffix);
    }
private:
    QString m_prefix, m_suffix;
};

#endif
