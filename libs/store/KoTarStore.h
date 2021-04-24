/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef koTarStore_h
#define koTarStore_h

#include "KoStore.h"

#include <QByteArray>

class KTar;
class KArchiveDirectory;
class QUrl;

class KoTarStore : public KoStore
{
public:
    KoTarStore(const QString & _filename, Mode _mode, const QByteArray & appIdentification,
               bool writeMimetype);
    KoTarStore(QIODevice *dev, Mode mode, const QByteArray & appIdentification,
               bool writeMimetype);
    /**
     * QUrl-constructor
     * @todo saving not completely implemented (fixed temporary file)
     */
    KoTarStore(QWidget* window, const QUrl &url, const QString & _filename, Mode _mode,
               const QByteArray & appIdentification, bool writeMimetype = true);
    ~KoTarStore() override;

    QStringList directoryList() const override;

protected:
    void init(const QByteArray &appIdentification);
    bool doFinalize() override;
    bool openWrite(const QString& name) override;
    bool openRead(const QString& name) override;
    bool closeWrite() override;
    bool closeRead() override {
        return true;
    }
    bool enterRelativeDirectory(const QString& dirName) override;
    bool enterAbsoluteDirectory(const QString& path) override;
    bool fileExists(const QString& absPath) const override;

    static QByteArray completeMagic(const QByteArray& appMimetype);

    /// The tar archive
    KTar * m_pTar;

    /** In "Read" mode this pointer is pointing to the
    current directory in the archive to speed up the verification process */
    const KArchiveDirectory* m_currentDir;

    /// Buffer used when writing
    QByteArray m_byteArray;
private:
    Q_DECLARE_PRIVATE(KoStore)
};

#endif
