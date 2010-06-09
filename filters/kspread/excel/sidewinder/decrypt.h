/* Swinder - Portable library for spreadsheet
   Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA
 */
#ifndef SWINDER_DECRYPT_H
#define SWINDER_DECRYPT_H

#include <QtCore/QByteArray>

#ifdef HAVE_QCA2
    #include <QtCrypto>
#endif

namespace Swinder
{

class RC4
{
public:
    RC4(const QByteArray& passwordHash, unsigned blockNr);
    unsigned char nextByte();
    QByteArray decrypt(const QByteArray& d);
private:
    unsigned char m_s[256];
    int m_i, m_j;
};

class RC4Decryption
{
public:
    RC4Decryption(const QByteArray& salt, const QByteArray& encryptedVerifier, const QByteArray& encryptedVerifierHash);
    ~RC4Decryption();

    bool checkPassword(const QString& password);
    void setInitialPosition(unsigned offset);
    void decryptBytes(unsigned count, unsigned char* data);
    void skipBytes(unsigned count);
private:
#ifdef HAVE_QCA2
    QCA::Initializer m_qca;
#endif
    QByteArray m_salt;
    QByteArray m_encryptedVerifier;
    QByteArray m_encryptedVerifierHash;
    QByteArray m_passwordHash;
    RC4* m_rc4;
    unsigned m_offset;

    unsigned char nextCryptByte();
};

} // namespace Swinder

#endif // SWINDER_DECRYPT_H
