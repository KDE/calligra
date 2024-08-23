/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thomas Schaap <thomas.schaap@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KoEncryptedStore_h
#define KoEncryptedStore_h
#include "KoStore.h"
#include <QHash>

class QString;
class QByteArray;
class QIODevice;
class QWidget;
class QUrl;
class KZip;
class KArchiveDirectory;
class QTemporaryFile;
struct KoEncryptedStore_EncryptionData;

class KoEncryptedStore : public KoStore
{
public:
    KoEncryptedStore(const QString &filename, Mode mode, const QByteArray &appIdentification, bool writeMimetype);
    KoEncryptedStore(QIODevice *dev, Mode mode, const QByteArray &appIdentification, bool writeMimetype);
    KoEncryptedStore(QWidget *window, const QUrl &url, const QString &filename, Mode mode, const QByteArray &appIdentification, bool writeMimetype);
    ~KoEncryptedStore();

    /*
     * Sets the password to be used for decryption or encryption of the file.
     *
     * This method only works if no password has been set or found yet,
     * i.e. when no file has been opened yet and this method hasn't been used yet.
     *
     * @param   password    A non-empty password.
     *
     * @return  True if the password was set
     */
    bool setPassword(const QString &password) override;

    /*
     * Returns whether a store opened for reading is actually encrypted.
     * This function will always return true in Write-mode.
     *
     * @return  True if the store is encrypted.
     */
    bool isEncrypted() override;

    QStringList directoryList() const override;

protected:
    void init(const QByteArray &appIdentification);
    bool doFinalize() override;
    bool openWrite(const QString &name) override;
    bool openRead(const QString &name) override;
    bool closeWrite() override;
    bool closeRead() override;
    bool enterRelativeDirectory(const QString &dirName) override;
    bool enterAbsoluteDirectory(const QString &path) override;
    bool fileExists(const QString &absPath) const override;

    /**
     * Tries and find a password for this document in KWallet.
     * Uses m_filename as base for finding the password and stores it in m_password if found.
     */
    void findPasswordInKWallet();

    /*
     * Retrieves the password used to encrypt or decrypt the store. Note that
     * QString() will returned if no password has been given or the store is
     * not encrypted.
     *
     * @return  The password this store is encrypted with.
     */
    QString password() override;

    /**
     * Stores the password for this document in KWallet.
     * Uses m_filename as base for storing the password and stores the value in m_password.
     */
    void savePasswordInKWallet();

private:
    QByteArray decryptFile(QByteArray &encryptedFile, KoEncryptedStore_EncryptionData &encData, QByteArray &password);

    /** returns true if the file should be encrypted, false otherwise **/
    bool isToBeEncrypted(const QString &fullpath);

protected:
    QHash<QString, KoEncryptedStore_EncryptionData> m_encryptionData;
    QByteArray m_password;
    QString m_filename;
    QByteArray m_manifestBuffer;
    KZip *m_pZip;
    QTemporaryFile *m_tempFile;
    bool m_bPasswordUsed;
    bool m_bPasswordDeclined;

    /** In "Read" mode this pointer is pointing to the
    current directory in the archive to speed up the verification process */
    const KArchiveDirectory *m_currentDir;

private:
    Q_DECLARE_PRIVATE(KoStore)
};
#endif
