/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "KoStore.h"

class KZip;
class KArchiveDirectory;
class QUrl;

class KoZipStore : public KoStore
{
public:
    KoZipStore(const QString &_filename, Mode _mode, const QByteArray &appIdentification, bool writeMimetype = true);
    KoZipStore(QIODevice *dev, Mode mode, const QByteArray &appIdentification, bool writeMimetype = true);
    /**
     * QUrl-constructor
     * @todo saving not completely implemented (fixed temporary file)
     */
    KoZipStore(QWidget *window, const QUrl &_url, const QString &_filename, Mode _mode, const QByteArray &appIdentification, bool writeMimetype = true);
    ~KoZipStore() override;

    void setCompressionEnabled(bool e) override;
    qint64 write(const char *_data, qint64 _len) override;

    QStringList directoryList() const override;

protected:
    void init(const QByteArray &appIdentification);
    bool doFinalize() override;
    bool openWrite(const QString &name) override;
    bool openRead(const QString &name) override;
    bool closeWrite() override;
    bool closeRead() override
    {
        return true;
    }
    bool enterRelativeDirectory(const QString &dirName) override;
    bool enterAbsoluteDirectory(const QString &path) override;
    bool fileExists(const QString &absPath) const override;

private:
    /// The archive
    KZip *m_pZip;

    /** In "Read" mode this pointer is pointing to the
    current directory in the archive to speed up the verification process */
    const KArchiveDirectory *m_currentDir;
};
