/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "decrypt.h"

#include <QCryptographicHash>

using namespace Swinder;

RC4Decryption::RC4Decryption(const QByteArray &salt, const QByteArray &encryptedVerifier, const QByteArray &encryptedVerifierHash)
    : m_salt(salt)
    , m_encryptedVerifier(encryptedVerifier)
    , m_encryptedVerifierHash(encryptedVerifierHash)
    , m_rc4(nullptr)
{
}

RC4Decryption::~RC4Decryption()
{
    delete m_rc4;
}

static QByteArray md5sum(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Md5);
}

bool RC4Decryption::checkPassword(const QString &password)
{
    QByteArray unicodePassword(reinterpret_cast<const char *>(password.utf16()), password.length() * 2); // depends on correct host-byte order

    QByteArray h0 = md5sum(unicodePassword);
    QByteArray trunc = h0.left(5);
    QByteArray ba = trunc + m_salt;
    ba = ba.repeated(16);
    QByteArray interm = ba;
    QByteArray h1 = md5sum(interm);
    m_passwordHash = h1;

    RC4 rc4(m_passwordHash, 0);

    QByteArray verifier = rc4.decrypt(m_encryptedVerifier);
    QByteArray hashedVerifier = md5sum(verifier);
    QByteArray verifierHash = rc4.decrypt(m_encryptedVerifierHash);

    return hashedVerifier == verifierHash;
}

void RC4Decryption::setInitialPosition(unsigned offset)
{
    int block = offset / 1024;
    m_offset = block * 1024;
    m_rc4 = new RC4(m_passwordHash, block);
    skipBytes(offset % 1024);
}

unsigned char RC4Decryption::nextCryptByte()
{
    Q_ASSERT(m_rc4);
    unsigned char ret = m_rc4->nextByte();
    m_offset++;
    if (m_offset % 1024 == 0) {
        delete m_rc4;
        int block = m_offset / 1024;
        m_rc4 = new RC4(m_passwordHash, block);
    }
    return ret;
}

void RC4Decryption::skipBytes(unsigned count)
{
    for (unsigned i = 0; i < count; ++i)
        nextCryptByte();
}

void RC4Decryption::decryptBytes(unsigned count, unsigned char *data)
{
    for (unsigned i = 0; i < count; ++i) {
        data[i] ^= nextCryptByte();
    }
}

RC4::RC4(const QByteArray &passwordHash, unsigned blockNr)
    : m_i(0)
    , m_j(0)
{
    QByteArray key = passwordHash.left(5);
    key.append(blockNr & 0xff).append((blockNr >> 8) & 0xff).append((blockNr >> 16) & 0xff).append((blockNr >> 24) & 0xff);
    key = md5sum(key);

    for (int i = 0; i < 256; i++)
        m_s[i] = i;
    for (int i = 0, j = 0; i < 256; i++) {
        j = (j + m_s[i] + key[i % key.length()]) & 0xff;
        qSwap(m_s[i], m_s[j]);
    }
}

unsigned char RC4::nextByte()
{
    m_i = (m_i + 1) & 0xff;
    m_j = (m_j + m_s[m_i]) & 0xff;
    qSwap(m_s[m_i], m_s[m_j]);
    return m_s[(m_s[m_i] + m_s[m_j]) & 0xff];
}

QByteArray RC4::decrypt(const QByteArray &d)
{
    QByteArray r;
    r.resize(d.size());
    for (int i = 0; i < d.size(); i++) {
        r[i] = d[i] ^ nextByte();
    }
    return r;
}
