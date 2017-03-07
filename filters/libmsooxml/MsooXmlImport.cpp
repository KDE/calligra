/*
 * This file is part of Office 2007 Filters for Calligra
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MsooXmlImport.h"
#include "MsooXmlReader.h"
#include "MsooXmlUtils.h"
#include "MsooXmlSchemas.h"
#include "MsooXmlContentTypes.h"
#include "MsooXmlRelationships.h"
#include "MsooXmlTheme.h"
#include "ooxml_pole.h"

#include <QColor>
#include <QFile>
#include <QFont>
#include <QPen>
#include <QRegExp>
#include <QImage>
#include <QInputDialog>
#include <QImageReader>
#include <QFileInfo>

#include "MsooXmlDebug.h"
#include <kzip.h>
#include <QTemporaryFile>

#include <KoEmbeddedDocumentSaver.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>

#include <memory>

#ifdef HAVE_QCA2
// QCA headers have "slots" and "signals", which QT_NO_SIGNALS_SLOTS_KEYWORDS does not like
#define slots Q_SLOTS
#define signals Q_SIGNALS
#include <QtCrypto>
#undef slots
#undef signals
#endif

using namespace MSOOXML;

MsooXmlImport::MsooXmlImport(const QString& bodyContentElement, QObject* parent)
        : KoOdfExporter(bodyContentElement, parent),
        m_zip(0),
        m_outputStore(0)
{
}

MsooXmlImport::~MsooXmlImport()
{
}

void MsooXmlImport::reportProgress(unsigned progress)
{
    emit sigProgress(progress);
}

void MsooXmlImport::writeConfigurationSettings(KoXmlWriter* settings) const
{
    settings->startElement("config:config-item");
    settings->addAttribute("config:name", "UseFormerLineSpacing");
    settings->addAttribute("config:type", "boolean");
    settings->addTextSpan("false");
    settings->endElement();

    settings->startElement("config:config-item");
    settings->addAttribute("config:name", "TabsRelativeToIndent");
    settings->addAttribute("config:type", "boolean");
    settings->addTextSpan("false"); // ODF=true, MSOffice=false
    settings->endElement();
}

KoFilter::ConversionStatus MsooXmlImport::createDocument(KoStore *outputStore,
                                                         KoOdfWriters *writers)
{
    debugMsooXml << "######################## start ####################";
    KoFilter::ConversionStatus status = OK;
//! @todo show this message in error details in the GUI:
    QString errorMessage;

    KZip* zip = new KZip(m_chain->inputFile());
    debugMsooXml << "Store created";

    QTemporaryFile* tempFile = 0;

    if (!zip->open(QIODevice::ReadOnly)) {
        errorMessage = i18n("Could not open the requested file %1", m_chain->inputFile());
//! @todo transmit the error to the GUI...
        debugMsooXml << errorMessage;
        delete zip;

        // If the file can't be opened by the zip, it may be a
        // password protected file.  In OOXML, this is stored as a
        // standard OLE file with some special streams.
        QString  inputFilename = m_chain->inputFile();
        if (isPasswordProtectedFile(inputFilename)) {
            if ((tempFile = tryDecryptFile(inputFilename))) {
                zip = new KZip(tempFile->fileName());
                if (!zip->open(QIODevice::ReadOnly)) {
                    return KoFilter::PasswordProtected;
                }
            } else {
                return KoFilter::PasswordProtected;
            }
        } else
            return KoFilter::FileNotFound;
    }

    if (!zip->directory()) {
        errorMessage = i18n("Could not read ZIP directory of the requested file %1", m_chain->inputFile());
//! @todo transmit the error to the GUI...
        debugMsooXml << errorMessage;
        delete zip;
        return KoFilter::FileNotFound;
    }

    m_zip = zip; // set context
    m_outputStore = outputStore; // set context

    status = openFile(writers, errorMessage);

    m_zip = 0; // clear context
    m_outputStore = 0; // clear context

    QImage thumbnail;
    if (status == KoFilter::OK) {
        // We do not care about the failure
        Utils::loadThumbnail(thumbnail, zip);
    } else {
        debugMsooXml << "openFile() != OK";
//! @todo transmit the error to the GUI...
        debugMsooXml << errorMessage;
        delete tempFile;
        delete zip;
        return status;
    }

    if (!zip->close()) {
        delete tempFile;
        delete zip;
        return KoFilter::StorageCreationError;
    }

    if (status != KoFilter::OK) {
//! @todo transmit the error to the GUI...
        debugMsooXml << errorMessage;
    }
    debugMsooXml << "######################## done ####################";
    delete tempFile;
    delete zip;
    return status;
}

static inline unsigned long readU32(const void* p)
{
    const unsigned char* ptr = (const unsigned char*) p;
    return ptr[0] + (ptr[1] << 8) + (ptr[2] << 16) + (ptr[3] << 24);
}

static inline unsigned long readU16(const void* p)
{
    const unsigned char* ptr = (const unsigned char*) p;
    return ptr[0] + (ptr[1] << 8);
}

static inline quint64 readU64(const void* p)
{
    return quint64(readU32(p)) | quint64(readU32(reinterpret_cast<const char*>(p)+4)) << 32;
}

#ifdef HAVE_QCA2
static QByteArray sha1sum(const QByteArray& data)
{
    QCA::Hash sha1Hash("sha1");
    sha1Hash.update(data);
    return sha1Hash.final().toByteArray();
}
#endif

bool MsooXmlImport::isPasswordProtectedFile(QString &filename)
{
    // Open the file.
    QFile  file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        //debugMsooXml << "Cannot open " << filename;
        return false;
    }

    // Open the OLE storage.
    OOXML_POLE::Storage storage(&file);
    if (!storage.open()) {
        //debugMsooXml << "Cannot open" << filename << "as storage";
        file.close();
        return false;
    }

    //debugMsooXml << "This seems to be an OLE file";

    // Loop through the streams in the file and if one of them is named
    // "EncryptionInfo", then we probably have a password protected file.
    bool result = false;
    std::list<std::string> entries = storage.entries();
    std::list<std::string>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        debugMsooXml << it->c_str();
        if (*it == "EncryptionInfo") {
            result = true;
            break;
        }
    }

    // Clean up after us.
    storage.close();
    file.close();

    return result;
}

#ifdef HAVE_QCA2
QCA::Cipher createCipher(const QByteArray& blockKey, const QByteArray& hn, const QByteArray& salt)
{
    QByteArray hfinal = sha1sum(hn + blockKey);
    if (hfinal.size() * 8 < 128) hfinal.append(QByteArray(128/8 - hfinal.size(), 0x36));
    if (hfinal.size() * 8 > 128) hfinal = hfinal.left(128/8);
    // not clear which is correct
    //QByteArray iv = sha1sum(salt + blockKey);
    QByteArray iv = salt;
    QCA::Cipher aes("aes128", // TODO: size from xml
                    QCA::Cipher::CBC, // TODO: from xml
                    QCA::Cipher::NoPadding,
                    QCA::Decode,
                    hfinal, // key
                    iv);
    return aes;
}
#endif

QTemporaryFile* MsooXmlImport::tryDecryptFile(QString &filename)
{
#ifdef HAVE_QCA2
    QCA::Initializer qcainit;
    debugMsooXml << QCA::isSupported("sha1") << QCA::isSupported("aes128-ecb") << QCA::supportedFeatures();
    if (!QCA::isSupported("sha1") || !QCA::isSupported("aes128-ecb")) {
#endif
        debugMsooXml << "sha1 or aes128_ecb are not supported";
        return 0;
#ifdef HAVE_QCA2
    }

    // Open the file.
    QFile  file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        //debugMsooXml << "Cannot open " << filename;
        return 0;
    }

    // Open the OLE storage.
    OOXML_POLE::Storage storage(&file);
    if (!storage.open()) {
        //debugMsooXml << "Cannot open" << filename << "as storage";
        file.close();
        return 0;
    }

    OOXML_POLE::Stream infoStream(&storage, "/EncryptionInfo");
    if (infoStream.size() < 50) {
        debugMsooXml << "Invalid encryption info";
        return 0;
    }

    unsigned char buffer[4096];
    unsigned bytes_read = infoStream.read(buffer, 8);
    Q_ASSERT(bytes_read == 8);
    unsigned vMajor = readU16(buffer + 0);
    unsigned vMinor = readU16(buffer + 2);
    unsigned flags = readU32(buffer + 4);
    debugMsooXml << "major:" << vMajor << "minor:" << vMinor << "flags:" << flags;
    if ((vMajor != 3 && vMajor != 4) || (vMinor != 2 && vMinor != 4)) {
        debugMsooXml << "unsupported encryption version";
        return 0;
    }

    if (vMinor == 2) {
        bytes_read = infoStream.read(buffer, 4);
        unsigned headerSize = readU32(buffer);
        debugMsooXml << "headersize:" << headerSize;

        bytes_read = infoStream.read(buffer, qMin(4096u, headerSize));
        unsigned flags2 = readU32(buffer + 0);
        if (bytes_read != headerSize || flags != flags2) {
            debugMsooXml << "corrupt encrypted file";
            return 0;
        }

        unsigned algId = readU32(buffer + 8);
        unsigned algIdHash = readU32(buffer + 12);
        unsigned keySize = readU32(buffer + 16);
        unsigned providerType = readU32(buffer + 20);
        QString cspName;
        for (unsigned i = 32; i < headerSize; i += 2) {
            unsigned c = readU16(buffer + i);
            if (c) {
                cspName += QChar(c);
            } else break;
        }
        debugMsooXml << QString::number(algId, 16) << QString::number(algIdHash, 16) << keySize << QString::number(providerType, 16) << cspName;

        // now read verifier info
        bytes_read = infoStream.read(buffer, 40);
        if (bytes_read != 40 || readU32(buffer) != 16) {
            debugMsooXml << "Invalid verifier info";
            return 0;
        }

        QByteArray salt(reinterpret_cast<const char*>(buffer + 4), 16);
        QByteArray encryptedVerifier(reinterpret_cast<const char*>(buffer + 20), 16);
        unsigned verifierHashSize = readU32(buffer + 36);
        // verifier hash
        unsigned rem = infoStream.size() - infoStream.tell();
        bytes_read = infoStream.read(buffer, qMin(4096u, rem));
        QByteArray encryptedVerifierHash(reinterpret_cast<const char*>(buffer), bytes_read);
        const int spinCount = 50000; //FIXME better use long int or qint32

        bool first = true;
        while (true) {
            bool ok;
            QString password = QInputDialog::getText(0, i18n("Enter password"),
                                                     first ?
                                                         i18n("This document is encrypted, please enter the password to decrypt it:")
                                                       : i18n("Incorrect password, please enter the password to decrypt this document:"),
                                                     QLineEdit::Password, "", &ok);
            first = false;
            if (!ok) {
                return 0;
            }
            QByteArray unicodePassword(reinterpret_cast<const char*>(password.utf16()), password.length()*2);
            QByteArray h0 = sha1sum(salt + unicodePassword);
            QByteArray hn = h0;
            for (int i = 0; i < spinCount; i++) {
                QByteArray it;
                it.append(i & 0xff).append((i >> 8) & 0xff).append((i >> 16) & 0xff).append((i >> 24) & 0xff);
                hn = sha1sum(it + hn);
            }
            QByteArray block(4, '\0');
            QByteArray hfinal = sha1sum(hn + block);
            //debugMsooXml << hfinal;
            QByteArray x1(64, 0x36);
            QByteArray x2(64, 0x5C);
            for (int i = 0; i < hfinal.size(); i++) {
                x1[i] = x1[i] ^ hfinal[i];
                x2[i] = x2[i] ^ hfinal[i];
            }
            x1 = sha1sum(x1);
            x2 = sha1sum(x2);
            QByteArray x3 = x1 + x2;
            QByteArray key = x3.left(128 / 8);

            QCA::Cipher aes("aes128", QCA::Cipher::ECB, QCA::Cipher::DefaultPadding, QCA::Decode, key);
            QByteArray verifier = aes.update(encryptedVerifier).toByteArray();
            verifier += aes.final().toByteArray();
            debugMsooXml << verifier.size() << QCA::arrayToHex(verifier);
            QByteArray hashedVerifier = sha1sum(verifier);
            aes.clear();
            QByteArray verifierHash = aes.update(encryptedVerifierHash).toByteArray();
            debugMsooXml << verifierHash.size() << QCA::arrayToHex(verifierHash);
            verifierHash += aes.final().toByteArray();
            debugMsooXml << QCA::arrayToHex(hashedVerifier) << QCA::arrayToHex(verifierHash) << verifierHash.size();
            bool passwordCorrect = hashedVerifier.left(verifierHashSize) == verifierHash.left(verifierHashSize);
            debugMsooXml << "Correct?" << passwordCorrect;

            if (!passwordCorrect) {
                continue;
            }

            OOXML_POLE::Stream *dataStream = new OOXML_POLE::Stream(&storage, "/EncryptedPackage");
            QTemporaryFile* outf = new QTemporaryFile;
            outf->open();

            aes.clear();
            bytes_read = dataStream->read(buffer, 8);
            debugMsooXml << readU32(buffer);
            while (bytes_read > 0) {
                bytes_read = dataStream->read(buffer, 4096);
                debugMsooXml << bytes_read;
                outf->write(aes.update(QByteArray::fromRawData(reinterpret_cast<const char*>(buffer), bytes_read)).toByteArray());
            }
            outf->write(aes.final().toByteArray());

            outf->close(); delete dataStream;

            return outf;
        }
    } else {
        QByteArray xmlData;
        do {
            bytes_read = infoStream.read(buffer, 4096);
            xmlData.append(reinterpret_cast<const char*>(buffer), bytes_read);
        } while (bytes_read > 0);
        // bah, seems there is some random garbage at the end
        int lastIdx = xmlData.lastIndexOf('>');
        if (lastIdx >= 0) xmlData = xmlData.left(lastIdx+1);
        debugMsooXml << xmlData;
        QBuffer b(&xmlData);
        KoXmlDocument doc;
        QString errorMsg; int errorLine, errorColumn;
        if (!doc.setContent(&b, true, &errorMsg, &errorLine, &errorColumn)) {
            debugMsooXml << errorMsg << errorLine << errorColumn;
            return 0;
        }
        const QString encNS = QString::fromLatin1("http://schemas.microsoft.com/office/2006/encryption");
        const QString pNS = QString::fromLatin1("http://schemas.microsoft.com/office/2006/keyEncryptor/password");
        KoXmlElement keyData = KoXml::namedItemNS(doc.documentElement(), encNS, "keyData");
        KoXmlElement keyEncryptors = KoXml::namedItemNS(doc.documentElement(), encNS, "keyEncryptors");
        KoXmlElement keyEncryptor = keyEncryptors.firstChild().toElement();
        if (keyEncryptor.namespaceURI() != encNS || keyEncryptor.localName() != "keyEncryptor") {
            debugMsooXml << "can't parse encryption xml";
            return 0;
        }
        if (keyEncryptor.attribute("uri") != "http://schemas.microsoft.com/office/2006/keyEncryptor/password") {
            debugMsooXml << "unsupported key encryptor " << keyEncryptor.attribute("uri");
            return 0;
        }
        KoXmlElement encryptedKey = keyEncryptor.firstChild().toElement();
        if (encryptedKey.namespaceURI() != pNS || encryptedKey.localName() != "encryptedKey") {
            debugMsooXml << "unexpected element in key encryptor";
            return 0;
        }
        const int spinCount = encryptedKey.attribute("spinCount").toInt();
        QByteArray keyDataSalt = QByteArray::fromBase64(keyData.attribute("saltValue").toLatin1());
        QByteArray salt = QByteArray::fromBase64(encryptedKey.attribute("saltValue").toLatin1());
        QByteArray encryptedVerifierHashInput = QByteArray::fromBase64(encryptedKey.attribute("encryptedVerifierHashInput").toLatin1());
        QByteArray encryptedVerifierHashValue = QByteArray::fromBase64(encryptedKey.attribute("encryptedVerifierHashValue").toLatin1());
        QByteArray encryptedKeyValue = QByteArray::fromBase64(encryptedKey.attribute("encryptedKeyValue").toLatin1());
        debugMsooXml << spinCount << QCA::arrayToHex(salt) << QCA::arrayToHex(encryptedVerifierHashInput) << QCA::arrayToHex(encryptedVerifierHashValue) << QCA::arrayToHex(encryptedKeyValue);
        debugMsooXml << QCA::arrayToHex(keyDataSalt) << keyDataSalt.length();

        bool first = true;
        while (true) {
            bool ok;
            QString password = QInputDialog::getText(0, i18n("Enter password"),
                                                     first ?
                                                         i18n("This document is encrypted, please enter the password to decrypt it:")
                                                       : i18n("Incorrect password, please enter the password to decrypt this document:"),
                                                     QLineEdit::Password, "", &ok);
            first = false;
            if (!ok) {
                return 0;
            }

            QByteArray unicodePassword(reinterpret_cast<const char*>(password.utf16()), password.length()*2);
            QByteArray h0 = sha1sum(salt + unicodePassword);
            QByteArray hn = h0;
            for (int i = 0; i < spinCount; i++) {
                QByteArray it;
                it.append(i & 0xff).append((i >> 8) & 0xff).append((i >> 16) & 0xff).append((i >> 24) & 0xff);
                hn = sha1sum(it + hn);
            }
            const char blockKeyData1[] = "\xfe\xa7\xd2\x76\x3b\x4b\x9e\x79";
            QByteArray blockKey1(blockKeyData1, sizeof(blockKeyData1) - 1);
            QCA::Cipher aes1 = createCipher(blockKey1, hn, salt);

            QByteArray verifierHashInput = aes1.update(encryptedVerifierHashInput.append(QByteArray(4, 0))).toByteArray();
            verifierHashInput.append(aes1.final().toByteArray());
            verifierHashInput = verifierHashInput.left(16);

            debugMsooXml << "verifier hash input:" << QCA::arrayToHex(verifierHashInput);
            QByteArray hashedVerifierHashInput = sha1sum(verifierHashInput);
            debugMsooXml << "hashed verifier hash input:" << QCA::arrayToHex(hashedVerifierHashInput);

            const char blockKeyData2[] = "\xd7\xaa\x0f\x6d\x30\x61\x34\x4e";
            QByteArray blockKey2(blockKeyData2, sizeof(blockKeyData2) - 1);
            QCA::Cipher aes2 = createCipher(blockKey2, hn, salt);
            QByteArray verifierHashValue = aes2.update(encryptedVerifierHashValue.append(QByteArray(12, 0))).toByteArray();
            verifierHashValue.append(aes2.final().toByteArray());

            debugMsooXml << "verifier hash value:" << QCA::arrayToHex(verifierHashValue);
            bool passwordCorrect = hashedVerifierHashInput == verifierHashValue.left(20);
            if (!passwordCorrect) {
                continue;
            }

            const char blockKeyData3[] = "\x14\x6e\x0b\xe7\xab\xac\xd0\xd6";
            QByteArray blockKey3(blockKeyData3, sizeof(blockKeyData3) - 1);
            QCA::Cipher aes3 = createCipher(blockKey3, hn, salt);
            QByteArray keyValue = aes3.update(encryptedKeyValue.append(QByteArray(4, 0))).toByteArray();
            keyValue.append(aes3.final().toByteArray());
            keyValue = keyValue.left(128/8);
            debugMsooXml << "key value:" << QCA::arrayToHex(keyValue);

            OOXML_POLE::Stream *dataStream = new OOXML_POLE::Stream(&storage, "/EncryptedPackage");
            QTemporaryFile* outf = new QTemporaryFile;
            outf->open();

            bytes_read = dataStream->read(buffer, 8);
            quint64 totSize = readU64(buffer);
            debugMsooXml << totSize;
            quint64 sizeRead = 0;
            unsigned segment = 0;
            while (bytes_read > 0) {
                bytes_read = dataStream->read(buffer, 4096);
                QByteArray blockKey;
                blockKey.append(segment & 0xff).append((segment >> 8) & 0xff).append((segment >> 16) & 0xff).append((segment >> 24) & 0xff);
                //blockKey.append((segment >> 24) & 0xff).append((segment >> 16) & 0xff).append((segment >> 8) & 0xff).append(segment & 0xff);
                QByteArray iv = sha1sum(keyDataSalt + blockKey);
                if (iv.size() * 8 < 128) iv.append(QByteArray(128/8 - iv.size(), 0x36));
                if (iv.size() * 8 > 128) iv = iv.left(128/8);
                QCA::Cipher aes("aes128",
                                QCA::Cipher::CBC,
                                QCA::Cipher::NoPadding,
                                QCA::Decode,
                                keyValue,
                                iv);

                QByteArray d = aes.update(QByteArray::fromRawData(reinterpret_cast<const char*>(buffer), bytes_read)).toByteArray();
                d.append(aes.final().toByteArray());
                if (sizeRead + d.size() > totSize) {
                    d = d.left(totSize - sizeRead);
                }
                outf->write(d);
                sizeRead += d.size();
                segment++;
            }

            outf->close(); delete dataStream;

            return outf;
        }
    }
#endif
}

KoFilter::ConversionStatus MsooXmlImport::createImage(const QImage& source,
                                       const QString& destinationName)
{
    if (!m_zip || !m_outputStore) {
        return KoFilter::UsageError;
    }
    QString errorMessage;
    const KoFilter::ConversionStatus status = Utils::createImage(errorMessage, source, m_outputStore, destinationName);
    if (status != KoFilter::OK) {
        warnMsooXml << "Failed to createImage:" << errorMessage;
    }
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::copyFile(const QString& sourceName,
        const QString& destinationName, bool oleFile)
{
    if (!m_zip || !m_outputStore) {
        return KoFilter::UsageError;
    }
    QString errorMessage;
    const KoFilter::ConversionStatus status = Utils::copyFile(
                m_zip, errorMessage, sourceName, m_outputStore, destinationName, oleFile);
//! @todo transmit the error to the GUI...
    if(status != KoFilter::OK)
        warnMsooXml << "Failed to copyFile:" << errorMessage;
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::imageFromFile(const QString& sourceName, QImage& image)
{
    if (!m_zip) {
        return KoFilter::UsageError;
    }

    QString errorMessage;
    KoFilter::ConversionStatus status = KoFilter::OK;

    std::auto_ptr<QIODevice> inputDevice(Utils::openDeviceForFile(m_zip, errorMessage, sourceName, status));
    if (!inputDevice.get()) {
        return status;
    }
    QImageReader r(inputDevice.get(), QFileInfo(sourceName).suffix().toLatin1());
    if (!r.canRead()) {
        return KoFilter::WrongFormat;
    }
    image = r.read();

    return status;
}

KoFilter::ConversionStatus MsooXmlImport::imageSize(const QString& sourceName, QSize& size)
{
    if (!m_zip) {
        return KoFilter::UsageError;
    }

    QString errorMessage;
    KoFilter::ConversionStatus status = KoFilter::OK;

    const QMap<QString, QSize>::ConstIterator it(m_imageSizes.constFind(sourceName));
    if (it == m_imageSizes.constEnd()) {
        status = Utils::imageSize(m_zip, errorMessage, sourceName, &size);

        if (status != KoFilter::OK)
            size = QSize(-1, -1);
        m_imageSizes.insert(sourceName, size);
    }
    else {
        size = it.value();
    }

//! @todo transmit the error to the GUI...
    debugMsooXml << errorMessage;
    return status;
}

// private
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentInternal(
    const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context, bool *pathFound)
{
    *pathFound = false;
    const QString fileName = m_contentTypes.value(contentType);
    debugMsooXml << contentType << "fileName=" << fileName;
    if (fileName.isEmpty()) {
        errorMessage = i18n("Could not find path for type %1", QString(contentType));
        warnMsooXml << errorMessage;
        return KoFilter::FileNotFound;
    }
    KoFilter::ConversionStatus status = loadAndParseDocumentFromFileInternal(
        fileName, reader, writers, errorMessage, context, pathFound);
    *pathFound = status != KoFilter::FileNotFound;
    return status;
}

// private
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentFromFileInternal(
    const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context, bool *pathFound)
{
    *pathFound = false;
    if (!m_zip) {
        return KoFilter::UsageError;
    }
    KoFilter::ConversionStatus status = Utils::loadAndParseDocument(
               reader, m_zip, writers, errorMessage, fileName, context);
    *pathFound = status != KoFilter::FileNotFound;
    return status;
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocument(
    const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    return loadAndParseDocumentInternal(contentType, reader, writers, errorMessage, context, &pathFound);
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentFromFile(
    const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    return loadAndParseDocumentFromFileInternal(fileName, reader, writers, errorMessage, context, &pathFound);
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentIfExists(
    const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    const KoFilter::ConversionStatus status( loadAndParseDocumentInternal(
        contentType, reader, writers, errorMessage, context, &pathFound) );
    if (!pathFound)
        return KoFilter::OK;
    return status;
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentFromFileIfExists(
    const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    const KoFilter::ConversionStatus status( loadAndParseDocumentFromFileInternal(
        fileName, reader, writers, errorMessage, context, &pathFound) );
    if (!pathFound)
        return KoFilter::OK;
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocument(
    MsooXmlReader *reader, const QString& path,
    MsooXmlReaderContext* context)
{
    if (!m_zip) {
        return KoFilter::UsageError;
    }
    QString errorMessage;
    KoFilter::ConversionStatus status = Utils::loadAndParseDocument(
                                            reader, m_zip, reader, errorMessage, path, context);
    if (status != KoFilter::OK)
        reader->raiseError(errorMessage);
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocument(
    MsooXmlReader *reader, const QString& path,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    if (!m_zip) {
        return KoFilter::UsageError;
    }
    KoFilter::ConversionStatus status = Utils::loadAndParseDocument(
                                            reader, m_zip, reader, errorMessage, path, context);
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::loadAndParseFromDevice(MsooXmlReader* reader, QIODevice* device,
        MsooXmlReaderContext* context)
{
    KoFilter::ConversionStatus status;
    reader->setDevice(device);
    reader->setFileName("PreDefinedDrawingMLTables"); // for error reporting
    status = reader->read(context);
    if (status != KoFilter::OK) {
        reader->raiseError(reader->errorString());
        return status;
    }
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::openFile(KoOdfWriters *writers, QString& errorMessage)
{
    static const char Content_Types_xml[] = "[Content_Types].xml";
    KoFilter::ConversionStatus status = loadAndParse(Content_Types_xml, m_contentTypesXML, errorMessage);
    if (status != KoFilter::OK) {
        debugMsooXml << Content_Types_xml << "could not be parsed correctly! Aborting!";
        return status;
    }
    RETURN_IF_ERROR( Utils::loadContentTypes(m_contentTypesXML, m_contentTypes) )

    static const char docPropy_core_xml[] = "docProps/core.xml";
    KoXmlDocument coreXML;
    if (loadAndParse(docPropy_core_xml, coreXML, errorMessage) == KoFilter::OK) {
        RETURN_IF_ERROR( Utils::loadDocumentProperties(coreXML, m_documentProperties) )
    }

    static const char docPropy_app_xml[] = "docProps/app.xml";
    KoXmlDocument appXML;
    if (loadAndParse(docPropy_app_xml, appXML, errorMessage) == KoFilter::OK) {
        RETURN_IF_ERROR( Utils::loadDocumentProperties(appXML, m_documentProperties) )
    }

    MsooXmlRelationships relationships(*this, writers, errorMessage);
    RETURN_IF_ERROR( parseParts(writers, &relationships, errorMessage) )
//! @todo sigProgress()
    emit sigProgress(10);

    return KoFilter::OK;
}

KoFilter::ConversionStatus MsooXmlImport::loadAndParse(const QString& filename, KoXmlDocument& doc, QString& errorMessage)
{
    return Utils::loadAndParse(doc, m_zip, errorMessage, filename);
}
