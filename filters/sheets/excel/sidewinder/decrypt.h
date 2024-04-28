/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SWINDER_DECRYPT_H
#define SWINDER_DECRYPT_H

#include <QByteArray>
#include <QString>

namespace Swinder
{

class RC4
{
public:
    RC4(const QByteArray &passwordHash, unsigned blockNr);
    unsigned char nextByte();
    QByteArray decrypt(const QByteArray &d);

private:
    unsigned char m_s[256];
    int m_i, m_j;
};

class RC4Decryption
{
public:
    RC4Decryption(const QByteArray &salt, const QByteArray &encryptedVerifier, const QByteArray &encryptedVerifierHash);
    ~RC4Decryption();

    bool checkPassword(const QString &password);
    void setInitialPosition(unsigned offset);
    void decryptBytes(unsigned count, unsigned char *data);
    void skipBytes(unsigned count);

private:
    QByteArray m_salt;
    QByteArray m_encryptedVerifier;
    QByteArray m_encryptedVerifierHash;
    QByteArray m_passwordHash;
    RC4 *m_rc4;
    unsigned m_offset;

    unsigned char nextCryptByte();
};

} // namespace Swinder

#endif // SWINDER_DECRYPT_H
