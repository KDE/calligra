/*
 * This file is part of Office 2007 Filters for KOffice
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
#include "MsooXmlThemesReader.h"
#include "pole.h"

#include <QColor>
#include <QFile>
#include <QFont>
#include <QPen>
#include <QRegExp>
#include <QImage>
#include <QInputDialog>

#include <kdeversion.h>
#include <KDebug>
#include <KZip>
#include <KMessageBox>
#include <KTemporaryFile>

#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoUnit.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>

#include <memory>

#ifdef HAVE_QCA2
#include <QtCrypto>
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

KoFilter::ConversionStatus MsooXmlImport::createDocument(KoStore *outputStore,
                                                         KoOdfWriters *writers)
{
    kDebug() << "######################## start ####################";
    KoFilter::ConversionStatus status = OK;
//! @todo show this message in error details in the GUI:
    QString errorMessage;

    KZip* zip = new KZip(m_chain->inputFile());
    kDebug() << "Store created";

    KTemporaryFile* tempFile = 0;

    if (!zip->open(QIODevice::ReadOnly)) {
        errorMessage = i18n("Could not open the requested file %1", m_chain->inputFile());
//! @todo transmit the error to the GUI...
        kDebug() << errorMessage;
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
        kDebug() << errorMessage;
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
        kDebug() << "openFile() != OK";
//! @todo transmit the error to the GUI...
        kDebug() << errorMessage;
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
        kDebug() << errorMessage;
    }
    kDebug() << "######################## done ####################";
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
        //kDebug() << "Cannot open " << filename;
        return false;
    }

    // Open the OLE storage.
    POLE::Storage storage(&file);
    if (!storage.open()) {
        //kDebug() << "Cannot open" << filename << "as storage";
        file.close();
        return false;
    }

    //kDebug() << "This seems to be an OLE file";

    // Loop through the streams in the file and if one of them is named
    // "EncryptionInfo", then we probably have a password protected file.
    bool result = false;
    std::list<std::string> entries = storage.entries();
    std::list<std::string>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        kDebug() << it->c_str();
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

KTemporaryFile* MsooXmlImport::tryDecryptFile(QString &filename)
{
#ifdef HAVE_QCA2
    QCA::Initializer qcainit;
    kDebug() << QCA::isSupported("sha1") << QCA::isSupported("aes128-ecb") << QCA::supportedFeatures();
    if (!QCA::isSupported("sha1") || !QCA::isSupported("aes128-ecb")) {
#endif
        kDebug() << "sha1 or aes128_ecb are not supported";
        return 0;
#ifdef HAVE_QCA2
    }

    // Open the file.
    QFile  file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        //kDebug() << "Cannot open " << filename;
        return 0;
    }

    // Open the OLE storage.
    POLE::Storage storage(&file);
    if (!storage.open()) {
        //kDebug() << "Cannot open" << filename << "as storage";
        file.close();
        return 0;
    }

    POLE::Stream infoStream(&storage, "/EncryptionInfo");
    if (infoStream.size() < 50) {
        kDebug() << "Invalid encryption info";
        return 0;
    }

    unsigned char buffer[2048];
    unsigned bytes_read = infoStream.read(buffer, 12);
    Q_ASSERT(bytes_read == 12);
    unsigned vMajor = readU16(buffer + 0);
    unsigned vMinor = readU16(buffer + 2);
    unsigned flags = readU32(buffer + 4);
    unsigned headerSize = readU32(buffer + 8);
    kDebug() << "major:" << vMajor << "minor:" << vMinor << "flags:" << flags << "headersize:" << headerSize;
    if ((vMajor != 3 && vMajor != 4) || vMinor != 2) {
        kDebug() << "unsupported encryption version";
        return 0;
    }

    bytes_read = infoStream.read(buffer, qMin(2048u, headerSize));
    unsigned flags2 = readU32(buffer + 0);
    if (bytes_read != headerSize || flags != flags2) {
        kDebug() << "corrupt encrypted file";
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
    kDebug() << QString::number(algId, 16) << QString::number(algIdHash, 16) << keySize << QString::number(providerType, 16) << cspName;

    // now read verifier info
    bytes_read = infoStream.read(buffer, 40);
    if (bytes_read != 40 || readU32(buffer) != 16) {
        kDebug() << "Invalid verifier info";
        return 0;
    }

    QByteArray salt(reinterpret_cast<const char*>(buffer + 4), 16);
    QByteArray encryptedVerifier(reinterpret_cast<const char*>(buffer + 20), 16);
    unsigned verifierHashSize = readU32(buffer + 36);
    // verifier hash
    unsigned rem = infoStream.size() - infoStream.tell();
    bytes_read = infoStream.read(buffer, qMin(2048u, rem));
    QByteArray encryptedVerifierHash(reinterpret_cast<const char*>(buffer), bytes_read);

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
        for (int i = 0; i < 50000; i++) {
            QByteArray it;
            it.append(i & 0xff).append((i >> 8) & 0xff).append((i >> 16) & 0xff).append((i >> 24) & 0xff);
            hn = sha1sum(it + hn);
        }
        QByteArray block(4, '\0');
        QByteArray hfinal = sha1sum(hn + block);
        //kDebug() << hfinal;
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
        kDebug() << verifier.size() << QCA::arrayToHex(verifier);
        QByteArray hashedVerifier = sha1sum(verifier);
        aes.clear();
        QByteArray verifierHash = aes.update(encryptedVerifierHash).toByteArray();
        kDebug() << verifierHash.size() << QCA::arrayToHex(verifierHash);
        verifierHash += aes.final().toByteArray();
        kDebug() << QCA::arrayToHex(hashedVerifier) << QCA::arrayToHex(verifierHash) << verifierHash.size();
        bool passwordCorrect = hashedVerifier.left(verifierHashSize) == verifierHash.left(verifierHashSize);
        kDebug() << "Correct?" << passwordCorrect;

        if (!passwordCorrect) {
            continue;
        }

        POLE::Stream *dataStream = new POLE::Stream(&storage, "/EncryptedPackage");
        KTemporaryFile* outf = new KTemporaryFile;
        outf->open();

        aes.clear();
        bytes_read = dataStream->read(buffer, 8);
        kDebug() << readU32(buffer);
        while (bytes_read > 0) {
            bytes_read = dataStream->read(buffer, 2048);
            kDebug() << bytes_read;
            outf->write(aes.update(QByteArray::fromRawData(reinterpret_cast<const char*>(buffer), bytes_read)).toByteArray());
        }
        outf->write(aes.final().toByteArray());

        outf->close(); delete dataStream;

        return outf;
    }
#endif
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
        kWarning() << "Failed to copyFile:" << errorMessage;
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
    kDebug() << errorMessage;
    return status;
}

// private
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentInternal(
    const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context, bool *pathFound)
{
    *pathFound = false;
    const QString fileName = m_contentTypes.value(contentType);
    kDebug() << contentType << "fileName=" << fileName;
    if (fileName.isEmpty()) {
        errorMessage = i18n("Could not find path for type %1", QString(contentType));
        kWarning() << errorMessage;
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

KoFilter::ConversionStatus MsooXmlImport::openFile(KoOdfWriters *writers, QString& errorMessage)
{
    static const char *Content_Types_xml = "[Content_Types].xml";
    KoFilter::ConversionStatus status = loadAndParse(Content_Types_xml, m_contentTypesXML, errorMessage);
    if (status != KoFilter::OK) {
        kDebug() << Content_Types_xml << "could not be parsed correctly! Aborting!";
        return status;
    }
    RETURN_IF_ERROR( Utils::loadContentTypes(m_contentTypesXML, m_contentTypes) )

    static const char *docPropy_core_xml = "docProps/core.xml";
    KoXmlDocument coreXML;
    if (loadAndParse(docPropy_core_xml, coreXML, errorMessage) == KoFilter::OK) {
        RETURN_IF_ERROR( Utils::loadDocumentProperties(coreXML, m_documentProperties) )
    }

    static const char *docPropy_app_xml = "docProps/app.xml";
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

#include "MsooXmlImport.moc"
