#ifndef FAKE_KSAVEFILE_H
#define FAKE_KSAVEFILE_H

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QDebug>
#include <kglobal.h>

class KSaveFile : public QFile
{
public:

    KSaveFile() : QFile() {}
    KSaveFile(const QString &filename, const KComponentData &componentData = KGlobal::mainComponent()) : QFile(filename) {}

#if 0
    /**
     * @brief Set the target filename for the save file.
     * You must use this to set the filename of the target file if you do
     * not use the contructor that does so.
     * @param filename Name of the target file.
     */
    void setFileName(const QString &filename);

    /**
     * @brief Returns the name of the target file.
     * This function returns the name of the target file, or an empty
     * QString if it has not yet been set.
     * @returns The name of the target file.
     */
    QString fileName() const;

    /**
     * @brief Returns the last error that occurred.
     * Use this function to check for errors.
     * @returns The last error that occurred, or QFile::NoError.
     */
    QFile::FileError error() const;

    /**
     * @brief Returns a human-readable description of the last error.
     * Use this function to get a human-readable description of the
     * last error that occurred.
     * @return A string describing the last error that occurred.
     */
    QString errorString() const;
#endif

    bool open(OpenMode flags = QIODevice::ReadWrite)
    {
        return QFile::open(flags);
    }

    void abort()
    {
        qWarning() << Q_FUNC_INFO << "this will do nothing";
    }

    bool finalize()
    {
        return true;
    }

    static bool backupFile( const QString& filename, const QString& backupDir = QString() )
    {
        qWarning() << Q_FUNC_INFO << "this will do nothing";
        return false;
    }
    static bool simpleBackupFile( const QString& filename, const QString& backupDir = QString(), const QString& backupExtension = QLatin1String( "~" ) )
    {
        qWarning() << Q_FUNC_INFO << "this will do nothing";
        return false;
    }
    static bool numberedBackupFile( const QString& filename, const QString& backupDir = QString(), const QString& backupExtension = QString::fromLatin1( "~" ), const uint maxBackups = 10)
    {
        qWarning() << Q_FUNC_INFO << "this will do nothing";
        return false;
    }
    static bool rcsBackupFile( const QString& filename, const QString& backupDir = QString(), const QString& backupMessage = QString())
    {
        qWarning() << Q_FUNC_INFO << "this will do nothing";
        return false;
    }
};

#endif
