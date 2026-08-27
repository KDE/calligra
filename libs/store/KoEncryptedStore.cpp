/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thomas Schaap <thomas.schaap@kdemail.net>
   SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoEncryptedStore.h"
#include "KoStore_p.h"
#include "KoXmlReader.h"
#include <KoXmlNS.h>

#include <KCompressionDevice>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPasswordDialog>
#include <KoNetAccess.h>
#include <QBuffer>
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <QTemporaryFile>
#include <QWidget>
#include <StoreDebug.h>
#include <knewpassworddialog.h>
#include <kzip.h>
#include <qt6keychain/keychain.h>

#include <openssl/evp.h>
#include <openssl/opensslv.h>
#include <openssl/rand.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/provider.h>
#endif

using namespace Qt::StringLiterals;

namespace
{
// Blowfish (mandated by ODF's legacy encryption scheme) lives in OpenSSL 3's
// "legacy" provider, which isn't loaded by default. Load it once, lazily.
void ensureLegacyOpenSslProviderLoaded()
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    static OSSL_PROVIDER *legacyProvider = OSSL_PROVIDER_load(nullptr, "legacy");
    Q_UNUSED(legacyProvider);
#endif
}

// Blowfish CFB is the original (ODF 1.0/1.1/1.2-era) cipher; AES-256-CBC is
// the baseline ODF 1.2+ producers are asked to use instead (Part 3 §4.8.1).
enum class CipherAlgorithm {
    BlowfishCfb,
    Aes256Cbc
};

static constexpr auto AES256_CBC_ALGORITHM_NAME = "http://www.w3.org/2001/04/xmlenc#aes256-cbc"_L1;
static constexpr auto SHA1_START_KEY_NAME = "http://www.w3.org/2000/09/xmldsig#sha1"_L1;
static constexpr auto SHA256_START_KEY_NAME = "http://www.w3.org/2000/09/xmldsig#sha256"_L1;
static constexpr auto PBKDF2_URI_NAME = "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0#pbkdf2"_L1;
static constexpr auto SHA1_1K_CHECKSUM_URI = "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0#sha1-1k"_L1;
static constexpr auto SHA256_1K_CHECKSUM_URI = "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0#sha256-1k"_L1;

const EVP_CIPHER *evpCipherFor(CipherAlgorithm algorithm)
{
    return algorithm == CipherAlgorithm::Aes256Cbc ? EVP_aes_256_cbc() : EVP_bf_cfb();
}

// Derived-key length in bytes: AES-256 needs a 32-byte key; the legacy
// scheme's Blowfish setup has always used 16 bytes (128 bits).
int keyLengthFor(CipherAlgorithm algorithm)
{
    return algorithm == CipherAlgorithm::Aes256Cbc ? 32 : 16;
}

// Initialisation-vector length in bytes: AES-CBC needs one full 16-byte
// block; Blowfish CFB (an 8-byte-block cipher here) has always used 8.
int ivLengthFor(CipherAlgorithm algorithm)
{
    return algorithm == CipherAlgorithm::Aes256Cbc ? 16 : 8;
}

QString algorithmNameFor(CipherAlgorithm algorithm)
{
    return algorithm == CipherAlgorithm::Aes256Cbc ? AES256_CBC_ALGORITHM_NAME : "Blowfish CFB"_L1;
}

QString startKeyGenerationNameFor(QCryptographicHash::Algorithm algorithm)
{
    return algorithm == QCryptographicHash::Sha256 ? SHA256_START_KEY_NAME : "SHA1"_L1;
}

QString checksumTypeFor(QCryptographicHash::Algorithm algorithm)
{
    return algorithm == QCryptographicHash::Sha256 ? SHA256_1K_CHECKSUM_URI : "SHA1/1K"_L1;
}

// Returns false for an algorithm-name this reader doesn't recognize (mirrors
// the existing "unknown encryption method" handling at the call site).
bool parseCipherAlgorithmName(const QString &name, CipherAlgorithm *algorithm)
{
    if (name == "Blowfish CFB"_L1) {
        *algorithm = CipherAlgorithm::BlowfishCfb;
        return true;
    }
    if (name == AES256_CBC_ALGORITHM_NAME) {
        *algorithm = CipherAlgorithm::Aes256Cbc;
        return true;
    }
    return false;
}

bool parseKeyDerivationName(const QString &name)
{
    return name == "PBKDF2"_L1 || name == PBKDF2_URI_NAME;
}

// ODF 1.2 Part 3 §4.8.6: consumers shall support "SHA1" and both the
// xmldsig#sha1/xmldsig#sha256 IRIs. A missing manifest:start-key-generation
// element (old files) is handled by the caller as the spec's own default: SHA1.
bool parseStartKeyGenerationName(const QString &name, QCryptographicHash::Algorithm *algorithm)
{
    if (name == "SHA1"_L1 || name == SHA1_START_KEY_NAME) {
        *algorithm = QCryptographicHash::Sha1;
        return true;
    }
    if (name == SHA256_START_KEY_NAME) {
        *algorithm = QCryptographicHash::Sha256;
        return true;
    }
    return false;
}

// ODF 1.2 Part 3 §4.8.3: consumers shall support "SHA1/1K" and both the
// sha1-1k/sha256-1k IRIs -- all three are "first 1K bytes" checksums.
bool parseChecksumType(const QString &type, QCryptographicHash::Algorithm *algorithm, bool *isShort)
{
    if (type == "SHA1/1K"_L1 || type == SHA1_1K_CHECKSUM_URI) {
        *algorithm = QCryptographicHash::Sha1;
        *isShort = true;
        return true;
    }
    if (type == SHA256_1K_CHECKSUM_URI) {
        *algorithm = QCryptographicHash::Sha256;
        *isShort = true;
        return true;
    }
    if (type == "SHA1"_L1) {
        *algorithm = QCryptographicHash::Sha1;
        *isShort = false;
        return true;
    }
    return false;
}
}

QByteArray randomArray(int size)
{
    QByteArray buf(size, ' ');
    // RAND_priv_bytes(), not RAND_bytes(): this feeds key derivation (salts/IVs).
    if (RAND_priv_bytes((unsigned char *)(buf.data()), size) != 1) {
        errorStore << "Failed to generate" << size << "cryptographically secure random bytes";
        return {};
    }
    return buf;
}

using namespace QKeychain;

struct KoEncryptedStore_EncryptionData {
    // Needed for Key Derivation
    QByteArray salt;
    unsigned int iterationCount;
    // Derived-key length in bytes. 0 means "not given in the manifest": fall
    // back to whatever cipher normally needs (see keyLengthFor()).
    unsigned int keySize = 0;

    // Cipher (manifest:algorithm) and start-key hash (manifest:start-key-generation).
    CipherAlgorithm cipherAlgorithm = CipherAlgorithm::BlowfishCfb;
    QCryptographicHash::Algorithm startKeyHashAlgorithm = QCryptographicHash::Sha1;

    // Needed for enc/decryption
    QByteArray initVector;

    // Needed for (optional) password-checking
    QByteArray checksum;
    QCryptographicHash::Algorithm checksumHashAlgorithm = QCryptographicHash::Sha1;
    // checksumShort is set to true if the checksum-algorithm is one of the "1K" variants, which basically means we only use the first 1024 bytes of the
    // unencrypted file to check against (see also http://www.openoffice.org/servlets/ReadMsg?list=dev&msgNo=17498)
    bool checksumShort;

    // The size of the uncompressed file
    qint64 filesize;
};

// TODO: Discuss naming of this filer in saving-dialogues
// TODO: Discuss possibility of allowing programs to remember the password after opening to enable them to supply it when saving
// TODO: Discuss autosaving and password/leakage-problem (currently: hardcoded no autosave)
namespace
{
const char MANIFEST_FILE[] = "META-INF/manifest.xml";
const char META_FILE[] = "meta.xml";
const char THUMBNAIL_FILE[] = "Thumbnails/thumbnail.png";
}

KoEncryptedStore::KoEncryptedStore(const QString &filename, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
    , m_filename(filename)
    , m_tempFile(nullptr)
    , m_bPasswordUsed(false)
    , m_bPasswordDeclined(false)
    , m_currentDir(nullptr)
{
    Q_D(KoStore);

    m_pZip = new KZip(filename);
    d->good = true;
    d->localFileName = filename;

    init(appIdentification);
}

KoEncryptedStore::KoEncryptedStore(QIODevice *dev, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
    , m_tempFile(nullptr)
    , m_bPasswordUsed(false)
    , m_bPasswordDeclined(false)
    , m_currentDir(nullptr)
{
    Q_D(KoStore);

    m_pZip = new KZip(dev);
    d->good = true;

    init(appIdentification);
}

KoEncryptedStore::KoEncryptedStore(QWidget *window,
                                   const QUrl &url,
                                   const QString &filename,
                                   Mode mode,
                                   const QByteArray &appIdentification,
                                   bool writeMimetype)
    : KoStore(mode, writeMimetype)
    , m_filename(url.url())
    , m_tempFile(nullptr)
    , m_bPasswordUsed(false)
    , m_bPasswordDeclined(false)
    , m_currentDir(nullptr)
{
    Q_D(KoStore);

    d->window = window;
    d->good = true;

    if (mode == Read) {
        d->fileMode = KoStorePrivate::RemoteRead;
        d->localFileName = filename;
        m_pZip = new KZip(d->localFileName);
    } else {
        d->fileMode = KoStorePrivate::RemoteWrite;
        m_tempFile = new QTemporaryFile();
        if (!m_tempFile->open()) {
            d->good = false;
        } else {
            d->localFileName = m_tempFile->fileName();
            m_pZip = new KZip(m_tempFile);
        }
    }
    d->url = url;

    init(appIdentification);
}

void KoEncryptedStore::init(const QByteArray &appIdentification)
{
    Q_D(KoStore);
    bool checksumErrorShown = false;
    bool unreadableErrorShown = false;
    if (d->mode == Write) {
        d->good = true;
        if (!m_pZip->open(QIODevice::WriteOnly)) {
            d->good = false;
            return;
        }
        m_pZip->setExtraField(KZip::NoExtraField);
        // Write identification
        if (d->writeMimetype) {
            m_pZip->setCompression(KZip::NoCompression);
            (void)m_pZip->writeFile(QStringLiteral("mimetype"), appIdentification);
        }
        // FIXME: Hmm, seems to be a bug here since this is
        //        inconsistent with the code in openWrite():
        m_pZip->setCompression(KZip::DeflateCompression);
        // We don't need the extra field in Calligra - so we leave it as "no extra field".
    } else {
        d->good = m_pZip->open(QIODevice::ReadOnly);
        d->good &= m_pZip->directory() != nullptr;
        if (!d->good) {
            return;
        }

        // Read the manifest-file, so we can get the data we'll need to decrypt the other files in the store
        const KArchiveEntry *manifestArchiveEntry = m_pZip->directory()->entry(MANIFEST_FILE);
        if (!manifestArchiveEntry || !manifestArchiveEntry->isFile()) {
            // No manifest file? OK, *I* won't complain
            return;
        }
        QIODevice *dev = (static_cast<const KArchiveFile *>(manifestArchiveEntry))->createDevice();

        KoXmlDocument xmldoc;
        bool namespaceProcessing = true; // for the manifest ignore the namespace (bug #260515)
        if (!xmldoc.setContent(dev, namespaceProcessing) || xmldoc.documentElement().localName() != "manifest"
            || xmldoc.documentElement().namespaceURI() != KoXmlNS::manifest) {
            // KMessage::message(KMessage::Warning, i18n("The manifest file seems to be corrupted. The document could not be opened."));
            /// FIXME this message is not something we actually want to not mention, but it makes thumbnails noisy at times, so... let's not
            dev->close();
            delete dev;
            m_pZip->close();
            d->good = false;
            return;
        }
        KoXmlElement xmlroot = xmldoc.documentElement();
        if (xmlroot.hasChildNodes()) {
            KoXmlNode xmlnode = xmlroot.firstChild();
            while (!xmlnode.isNull()) {
                // Search for files
                if (!xmlnode.isElement() || xmlroot.namespaceURI() != KoXmlNS::manifest || xmlnode.toElement().localName() != "file-entry"
                    || !xmlnode.toElement().hasAttribute("full-path") || !xmlnode.hasChildNodes()) {
                    xmlnode = xmlnode.nextSibling();
                    continue;
                }

                // Build a structure to hold the data and fill it with defaults
                KoEncryptedStore_EncryptionData encData;
                encData.filesize = 0;
                encData.checksum = {};
                encData.checksumShort = false;
                encData.checksumHashAlgorithm = QCryptographicHash::Sha1;
                encData.salt = {};
                encData.iterationCount = 0;
                encData.keySize = 0;
                encData.initVector = {};
                encData.cipherAlgorithm = CipherAlgorithm::BlowfishCfb;
                // ODF 1.2 Part 3 §4.8.6's own default when manifest:start-key-generation is absent.
                encData.startKeyHashAlgorithm = QCryptographicHash::Sha1;

                // Get some info about the file
                QString fullpath = xmlnode.toElement().attribute("full-path");

                if (xmlnode.toElement().hasAttribute("size")) {
                    encData.filesize = xmlnode.toElement().attribute("size").toUInt();
                }

                // Find the embedded encryption-data block
                KoXmlNode xmlencnode = xmlnode.firstChild();
                while (!xmlencnode.isNull()
                       && (!xmlencnode.isElement() || xmlencnode.toElement().localName() != "encryption-data" || !xmlencnode.hasChildNodes())) {
                    xmlencnode = xmlencnode.nextSibling();
                }
                if (xmlencnode.isNull()) {
                    xmlnode = xmlnode.nextSibling();
                    continue;
                }

                // Find some things about the checksum. ODF 1.2 Part 3 §4.8.3: consumers
                // shall support "SHA1/1K" and the sha1-1k/sha256-1k IRIs.
                if (xmlencnode.toElement().hasAttribute("checksum")) {
                    encData.checksum = QByteArray::fromBase64(xmlencnode.toElement().attribute("checksum").toLatin1());
                    if (xmlencnode.toElement().hasAttribute("checksum-type")) {
                        QString checksumType = xmlencnode.toElement().attribute("checksum-type");
                        if (!parseChecksumType(checksumType, &encData.checksumHashAlgorithm, &encData.checksumShort)) {
                            // Checksum type unknown
                            if (!checksumErrorShown) {
                                KMessageBox::information(nullptr,
                                                         i18n("This document contains an unknown checksum. When you give a password it might not be verified."),
                                                         i18nc("@dialog:title", "Warning"));
                                checksumErrorShown = true;
                            }
                            encData.checksum = {};
                        }
                    } else {
                        encData.checksumShort = false;
                    }
                }

                KoXmlNode xmlencattr = xmlencnode.firstChild();
                bool algorithmFound = false;
                bool keyDerivationFound = false;
                // Search all data about encryption
                while (!xmlencattr.isNull()) {
                    if (!xmlencattr.isElement()) {
                        xmlencattr = xmlencattr.nextSibling();
                        continue;
                    }

                    // Find some things about the encryption algorithm. ODF 1.2 Part 3
                    // §4.8.1 delegates algorithm-name to the W3C XML Encryption IRIs;
                    // "Blowfish CFB" is the ODF-specific legacy value.
                    if (xmlencattr.toElement().localName() == "algorithm" && xmlencattr.toElement().hasAttribute("initialisation-vector")) {
                        algorithmFound = true;
                        encData.initVector = QByteArray::fromBase64(xmlencattr.toElement().attribute("initialisation-vector").toLatin1());
                        if (xmlencattr.toElement().hasAttribute("algorithm-name")
                            && !parseCipherAlgorithmName(xmlencattr.toElement().attribute("algorithm-name"), &encData.cipherAlgorithm)) {
                            if (!unreadableErrorShown) {
                                KMessageBox::information(nullptr,
                                                         i18n("This document contains an unknown encryption method. Some parts may be unreadable."),
                                                         i18nc("@title:dialog", "Warning"));
                                unreadableErrorShown = true;
                            }
                            encData.initVector = {};
                        }
                    }

                    // Find some things about the key derivation. ODF 1.2 Part 3 §4.8.9:
                    // consumers shall support "PBKDF2" and the manifest#pbkdf2 IRI.
                    if (xmlencattr.toElement().localName() == "key-derivation" && xmlencattr.toElement().hasAttribute("salt")) {
                        keyDerivationFound = true;
                        encData.salt = QByteArray::fromBase64(xmlencattr.toElement().attribute("salt").toLatin1());
                        encData.iterationCount = 1024;
                        if (xmlencattr.toElement().hasAttribute("iteration-count")) {
                            encData.iterationCount = xmlencattr.toElement().attribute("iteration-count").toUInt();
                        }
                        if (xmlencattr.toElement().hasAttribute("key-size")) {
                            encData.keySize = xmlencattr.toElement().attribute("key-size").toUInt();
                        }
                        if (xmlencattr.toElement().hasAttribute("key-derivation-name")
                            && !parseKeyDerivationName(xmlencattr.toElement().attribute("key-derivation-name"))) {
                            if (!unreadableErrorShown) {
                                KMessageBox::information(nullptr,
                                                         i18n("This document contains an unknown encryption method. Some parts may be unreadable."),
                                                         i18nc("@title:dialog", "Warning"));
                                unreadableErrorShown = true;
                            }
                            encData.salt = {};
                        }
                    }

                    // Find the (optional) start-key-generation step -- ODF 1.2 Part 3
                    // §4.8.6. Absent entirely, the spec's own default (SHA1) already set
                    // above applies; present-but-unrecognized is treated the same as an
                    // unrecognized algorithm/key-derivation-name.
                    if (xmlencattr.toElement().localName() == "start-key-generation" && xmlencattr.toElement().hasAttribute("start-key-generation-name")) {
                        if (!parseStartKeyGenerationName(xmlencattr.toElement().attribute("start-key-generation-name"), &encData.startKeyHashAlgorithm)) {
                            if (!unreadableErrorShown) {
                                KMessageBox::information(nullptr,
                                                         i18n("This document contains an unknown encryption method. Some parts may be unreadable."),
                                                         i18nc("@title:dialog", "Warning"));
                                unreadableErrorShown = true;
                            }
                            encData.salt = {};
                        }
                    }

                    xmlencattr = xmlencattr.nextSibling();
                }

                // Only use this encryption data if it makes sense to use it
                if (!(encData.salt.isEmpty() || encData.initVector.isEmpty())) {
                    m_encryptionData.insert(fullpath, encData);
                    if (!(algorithmFound && keyDerivationFound)) {
                        if (!unreadableErrorShown) {
                            KMessageBox::information(nullptr,
                                                     i18n("This document contains incomplete encryption data. Some parts may be unreadable."),
                                                     i18nc("@title:dialog", "Warning"));
                            unreadableErrorShown = true;
                        }
                    }
                }

                xmlnode = xmlnode.nextSibling();
            }
        }
        dev->close();
        delete dev;
    }
}

namespace
{
// Fills in (or updates) an manifest:encryption-data element's checksum
// attributes and its algorithm/start-key-generation/key-derivation children.
// Shared by the two places doFinalize() writes one, so they can't drift.
void writeEncryptionDataOdf(QDomDocument &document, QDomElement &encryptionElement, const KoEncryptedStore_EncryptionData &encData)
{
    encryptionElement.setAttribute("manifest:checksum-type", checksumTypeFor(encData.checksumHashAlgorithm));
    encryptionElement.setAttribute("manifest:checksum", QString::fromUtf8(encData.checksum.toBase64()));

    QDomNodeList childElements = encryptionElement.elementsByTagName("manifest:algorithm");
    QDomElement algorithmElement = childElements.isEmpty() ? document.createElement("manifest:algorithm") : childElements.item(0).toElement();
    if (childElements.isEmpty()) {
        encryptionElement.appendChild(algorithmElement);
    }
    algorithmElement.setAttribute("manifest:algorithm-name", algorithmNameFor(encData.cipherAlgorithm));
    algorithmElement.setAttribute("manifest:initialisation-vector", QString::fromUtf8(encData.initVector.toBase64()));

    // ODF 1.2 Part 3 §4.8.6.
    childElements = encryptionElement.elementsByTagName("manifest:start-key-generation");
    QDomElement startKeyElement = childElements.isEmpty() ? document.createElement("manifest:start-key-generation") : childElements.item(0).toElement();
    if (childElements.isEmpty()) {
        encryptionElement.appendChild(startKeyElement);
    }
    startKeyElement.setAttribute("manifest:start-key-generation-name", startKeyGenerationNameFor(encData.startKeyHashAlgorithm));

    childElements = encryptionElement.elementsByTagName("manifest:key-derivation");
    QDomElement keyDerivationElement = childElements.isEmpty() ? document.createElement("manifest:key-derivation") : childElements.item(0).toElement();
    if (childElements.isEmpty()) {
        encryptionElement.appendChild(keyDerivationElement);
    }
    keyDerivationElement.setAttribute("manifest:key-derivation-name", "PBKDF2");
    keyDerivationElement.setAttribute("manifest:iteration-count", QString::number(encData.iterationCount));
    keyDerivationElement.setAttribute("manifest:salt", QString::fromUtf8(encData.salt.toBase64()));
    keyDerivationElement.setAttribute("manifest:key-size", QString::number(encData.keySize > 0 ? encData.keySize : keyLengthFor(encData.cipherAlgorithm)));
}
}

bool KoEncryptedStore::doFinalize()
{
    Q_D(KoStore);
    if (d->good) {
        if (isOpen()) {
            close();
        }
        if (d->mode == Write) {
            // First change the manifest file and write it
            // We'll use the QDom classes here, since KoXmlReader and KoXmlWriter have no way of copying a complete xml-file
            // other than parsing it completely and rebuilding it.
            // Errorhandling here is done to prevent data from being lost whatever happens
            // TODO: Convert this to KoXML when KoXML is extended enough
            // Note: right now this is impossible due to lack of possibilities to copy an element as-is
            QDomDocument document;
            if (m_manifestBuffer.isEmpty()) {
                // No manifest? Better create one
                document = QDomDocument();
                QDomElement rootElement = document.createElement("manifest:manifest");
                rootElement.setAttribute("xmlns:manifest", "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0");
                document.appendChild(rootElement);
            }
            if (!m_manifestBuffer.isEmpty() && !document.setContent(m_manifestBuffer)) {
                // Oi! That's fresh XML we should have here!
                // This is the only case we can't fix
                KMessageBox::error(nullptr,
                                   i18n("The manifest file seems to be corrupted. It cannot be modified and the document will remain unreadable. Please try "
                                        "and save the document again to prevent losing your work."));
                m_pZip->close();
                return false;
            }
            QDomElement documentElement = document.documentElement();
            QDomNodeList fileElements = documentElement.elementsByTagName("manifest:file-entry");
            // Search all files in the manifest
            QStringList foundFiles;
            for (int i = 0; i < fileElements.size(); i++) {
                QDomElement fileElement = fileElements.item(i).toElement();
                QString fullpath = fileElement.toElement().attribute("manifest:full-path");
                // See if it's encrypted
                if (fullpath.isEmpty() || !m_encryptionData.contains(fullpath)) {
                    continue;
                }
                foundFiles += fullpath;
                KoEncryptedStore_EncryptionData encData = m_encryptionData.value(fullpath);
                // Set the unencrypted size of the file
                fileElement.setAttribute("manifest:size", encData.filesize);
                // See if the user of this store has already provided (old) encryption data
                QDomNodeList childElements = fileElement.elementsByTagName("manifest:encryption-data");
                QDomElement encryptionElement;
                if (childElements.isEmpty()) {
                    encryptionElement = document.createElement("manifest:encryption-data");
                    fileElement.appendChild(encryptionElement);
                } else {
                    encryptionElement = childElements.item(0).toElement();
                }
                writeEncryptionDataOdf(document, encryptionElement, encData);
            }
            if (foundFiles.size() < m_encryptionData.size()) {
                QList<QString> keys = m_encryptionData.keys();
                for (int i = 0; i < keys.size(); i++) {
                    if (!foundFiles.contains(keys.value(i))) {
                        KoEncryptedStore_EncryptionData encData = m_encryptionData.value(keys.value(i));
                        QDomElement fileElement = document.createElement("manifest:file-entry");
                        fileElement.setAttribute("manifest:full-path", keys.value(i));
                        fileElement.setAttribute("manifest:size", encData.filesize);
                        fileElement.setAttribute("manifest:media-type", "");
                        documentElement.appendChild(fileElement);
                        QDomElement encryptionElement = document.createElement("manifest:encryption-data");
                        fileElement.appendChild(encryptionElement);
                        writeEncryptionDataOdf(document, encryptionElement, encData);
                    }
                }
            }
            m_manifestBuffer = document.toByteArray();
            m_pZip->setCompression(KZip::DeflateCompression);
            if (!m_pZip->writeFile(QLatin1String(MANIFEST_FILE), m_manifestBuffer)) {
                KMessageBox::error(nullptr,
                                   i18n("The manifest file cannot be written. The document will remain unreadable. Please try and save the document again to "
                                        "prevent losing your work."));
                m_pZip->close();
                return false;
            }
        }
    }
    if (m_pZip)
        return m_pZip->close();
    else
        return true;
}

KoEncryptedStore::~KoEncryptedStore()
{
    Q_D(KoStore);
    /* Finalization of an encrypted store must happen earlier than deleting the zip. This rule normally is executed by KoStore, but too late to do any good.*/
    if (!d->finalized) {
        finalize();
    }

    delete m_pZip;

    if (d->fileMode == KoStorePrivate::RemoteWrite) {
        KIO::NetAccess::upload(d->localFileName, d->url, d->window);
        delete m_tempFile;
    } else if (d->fileMode == KoStorePrivate::RemoteRead) {
        KIO::NetAccess::removeTempFile(d->localFileName);
    }

    delete d->stream;
}

bool KoEncryptedStore::isEncrypted()
{
    Q_D(KoStore);
    if (d->mode == Read) {
        return !m_encryptionData.isEmpty();
    }
    return true;
}

QStringList KoEncryptedStore::directoryList() const
{
    QStringList retval;
    const KArchiveDirectory *directory = m_pZip->directory();
    foreach (const QString &name, directory->entries()) {
        const KArchiveEntry *fileArchiveEntry = m_pZip->directory()->entry(name);
        if (fileArchiveEntry->isDirectory()) {
            retval << name;
        }
    }
    return retval;
}

bool KoEncryptedStore::isToBeEncrypted(const QString &name)
{
    return !(name == META_FILE || name == MANIFEST_FILE || name == THUMBNAIL_FILE);
}

bool KoEncryptedStore::openRead(const QString &name)
{
    Q_D(KoStore);
    if (bad())
        return false;

    const KArchiveEntry *fileArchiveEntry = m_pZip->directory()->entry(name);
    if (!fileArchiveEntry) {
        return false;
    }
    if (fileArchiveEntry->isDirectory()) {
        warnStore << name << " is a directory!";
        return false;
    }
    const KZipFileEntry *fileZipEntry = static_cast<const KZipFileEntry *>(fileArchiveEntry);

    delete d->stream;
    d->stream = fileZipEntry->createDevice();
    d->size = fileZipEntry->size();
    if (m_encryptionData.contains(name)) {
        // This file is encrypted, do some decryption first
        if (m_bPasswordDeclined) {
            // The user has already declined to give a password
            // Open the file as empty
            d->stream->close();
            delete d->stream;
            d->stream = new QBuffer();
            d->stream->open(QIODevice::ReadOnly);
            d->size = 0;
            return true;
        }
        QByteArray encryptedFile(d->stream->readAll());
        if (encryptedFile.size() != d->size) {
            // Read error detected
            d->stream->close();
            delete d->stream;
            d->stream = nullptr;
            warnStore << "read error";
            return false;
        }
        d->stream->close();
        delete d->stream;
        d->stream = nullptr;
        KoEncryptedStore_EncryptionData encData = m_encryptionData.value(name);
        QByteArray decrypted;

        // If we don't have a password yet, try and find one
        if (m_password.isEmpty()) {
            findPasswordInKWallet();
        }

        while (true) {
            QByteArray password;
            bool keepPass = false;
            // I already have a password! Let's test it. If it's not good, we can dump it, anyway.
            if (!m_password.isEmpty()) {
                password = m_password;
                m_password = {};
            } else {
                if (!m_filename.isNull())
                    keepPass = false;
                KPasswordDialog dlg(d->window, keepPass ? KPasswordDialog::ShowKeepPassword : KPasswordDialog::KPasswordDialogFlags());
                dlg.setPrompt(i18n("Please enter the password to open this file."));
                if (!dlg.exec()) {
                    m_bPasswordDeclined = true;
                    d->stream = new QBuffer();
                    d->stream->open(QIODevice::ReadOnly);
                    d->size = 0;
                    return true;
                }
                password = dlg.password().toUtf8();
                if (keepPass)
                    keepPass = dlg.keepPassword();
                if (password.isEmpty()) {
                    continue;
                }
            }

            decrypted = decryptFile(encryptedFile, encData, password);
            if (decrypted.isEmpty()) {
                errorStore << "empty decrypted file" << Qt::endl;
                return false;
            }

            if (!encData.checksum.isEmpty()) {
                QByteArray checksum;
                if (encData.checksumShort && decrypted.size() > 1024) {
                    checksum = QCryptographicHash::hash(decrypted.left(1024), encData.checksumHashAlgorithm);
                } else {
                    checksum = QCryptographicHash::hash(decrypted, encData.checksumHashAlgorithm);
                }
                if (checksum != encData.checksum) {
                    continue;
                }
            }

            // The password passed all possible tests, so let's accept it
            m_password = password;
            m_bPasswordUsed = true;

            if (keepPass) {
                savePasswordInKWallet();
            }

            break;
        }

        QByteArray *resultArray = new QByteArray(decrypted);
        KCompressionDevice::CompressionType type = KCompressionDevice::compressionTypeForMimeType("application/x-gzip");
        KCompressionDevice *resultDevice = new KCompressionDevice(new QBuffer(resultArray, nullptr), false, type);

        if (!resultDevice) {
            delete resultArray;
            return false;
        }
        resultDevice->setSkipHeaders();
        d->stream = resultDevice;
        d->size = encData.filesize;
    }
    if (!d->stream->isOpen()) {
        d->stream->open(QIODevice::ReadOnly);
    }
    return true;
}

bool KoEncryptedStore::closeRead()
{
    Q_D(KoStore);
    delete d->stream;
    d->stream = nullptr;
    return true;
}

void KoEncryptedStore::findPasswordInKWallet()
{
    Q_D(KoStore);

    if (!m_filename.isNull()) {
        auto readJob = new ReadPasswordJob(QLatin1String("Calligra"));
        readJob->setKey(m_filename);
        QObject::connect(readJob, &ReadPasswordJob::finished, readJob, [this, readJob]() {
            if (readJob->error() != Error::NoError) {
                warnStore << "requestPassword: Failed to read password";
                return;
            }
            m_password = readJob->textData().toUtf8();
        });
        readJob->start();
    }
}

void KoEncryptedStore::savePasswordInKWallet()
{
    Q_D(KoStore);
    auto writeJob = new WritePasswordJob(QLatin1String("Calligra"));
    writeJob->setKey(m_filename);
    writeJob->setTextData(QString::fromUtf8(m_password));
    writeJob->start();
}

QByteArray KoEncryptedStore::decryptFile(QByteArray &encryptedFile, KoEncryptedStore_EncryptionData &encData, QByteArray &password)
{
    // ODF 1.2 Part 3 §4.8.6: hash the raw password with whichever algorithm
    // manifest:start-key-generation specified (SHA1 if that element was absent).
    QByteArray keyhash = QCryptographicHash::hash(password, encData.startKeyHashAlgorithm);

    // manifest:key-size if the manifest gave one, otherwise whatever the cipher needs.
    const int keyLength = encData.keySize > 0 ? static_cast<int>(encData.keySize) : keyLengthFor(encData.cipherAlgorithm);

    // create symmetric key
    QByteArray symmetricKey(keyLength, ' ');
    PKCS5_PBKDF2_HMAC_SHA1((char *)keyhash.data(),
                           keyhash.size(),
                           (unsigned char *)encData.salt.data(),
                           encData.salt.size(),
                           encData.iterationCount,
                           keyLength,
                           (unsigned char *)symmetricKey.data());

    // setup decrypt context with the manifest-specified cipher
    ensureLegacyOpenSslProviderLoaded();
    auto context = EVP_CIPHER_CTX_new();
    int resultLength;
    auto cryptoAlgorithm = evpCipherFor(encData.cipherAlgorithm);
    EVP_CIPHER_CTX_init(context);

    EVP_DecryptInit_ex(context, cryptoAlgorithm, nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_set_key_length(context, symmetricKey.size());
    EVP_DecryptInit_ex(context, nullptr, nullptr, (const unsigned char *)(symmetricKey.data()), (const unsigned char *)(encData.initVector.data()));

    // Blowfish CFB needs no block padding; AES-CBC's PKCS#7 padding must be stripped.
    EVP_CIPHER_CTX_set_padding(context, encData.cipherAlgorithm == CipherAlgorithm::Aes256Cbc ? 1 : 0);

    // actually decrypt with the manifest-specified cipher
    QByteArray result(encryptedFile.size() + EVP_CIPHER_CTX_block_size(context), ' ');
    int ok = EVP_DecryptUpdate(context, (unsigned char *)result.data(), &resultLength, (unsigned char *)encryptedFile.data(), encryptedFile.size());
    if (!ok) {
        return {};
    }
    result.resize(resultLength);

    // Finalized
    QByteArray final;
    int finalLength;
    final.resize(EVP_CIPHER_CTX_block_size(context));
    EVP_DecryptFinal_ex(context, (unsigned char *) final.data(), &finalLength);
    final.resize(finalLength);

    result += final;

    EVP_CIPHER_CTX_cleanup(context);
    EVP_CIPHER_CTX_free(context);

    return result;
}

bool KoEncryptedStore::setPassword(const QString &password)
{
    if (m_bPasswordUsed || password.isEmpty()) {
        return false;
    }
    m_password = password.toUtf8();
    return true;
}

QString KoEncryptedStore::password()
{
    if (m_password.isEmpty()) {
        return QString();
    }
    return QString::fromUtf8(m_password);
}

bool KoEncryptedStore::openWrite(const QString &name)
{
    Q_D(KoStore);
    if (bad())
        return false;
    if (isToBeEncrypted(name)) {
        // Encrypted files will be compressed by this class and should be stored in the zip as not compressed
        m_pZip->setCompression(KZip::NoCompression);
    } else {
        m_pZip->setCompression(KZip::DeflateCompression);
    }
    d->stream = new QBuffer();
    (static_cast<QBuffer *>(d->stream))->open(QIODevice::WriteOnly);
    if (name == MANIFEST_FILE)
        return true;
    return m_pZip->prepareWriting(name, "", "", 0);
}

bool KoEncryptedStore::closeWrite()
{
    Q_D(KoStore);
    bool passWasAsked = false;
    if (d->fileName == MANIFEST_FILE) {
        m_manifestBuffer = static_cast<QBuffer *>(d->stream)->buffer();
        return true;
    }

    // Find a password
    // Do not accept empty passwords for compatibility with OOo
    if (m_password.isEmpty()) {
        findPasswordInKWallet();
    }
    while (m_password.isEmpty()) {
        KNewPasswordDialog dlg(d->window);
        dlg.setPrompt(i18n("Please enter the password to encrypt the document with."));
        if (!dlg.exec()) {
            // Without the first password, prevent asking again by deadsimply refusing to continue functioning
            // TODO: This feels rather hackish. There should be a better way to do this.
            delete m_pZip;
            m_pZip = nullptr;
            d->good = false;
            return false;
        }
        m_password = dlg.password().toUtf8();
        passWasAsked = true;
    }

    // Ask the user to save the password
    if (passWasAsked
        && KMessageBox::questionTwoActions(d->window, i18n("Do you want to save the password?"), {}, KStandardGuiItem::save(), KStandardGuiItem::cancel())
            == KMessageBox::PrimaryAction) {
        savePasswordInKWallet();
    }

    QByteArray result;
    if (d->fileName == THUMBNAIL_FILE) {
        // TODO: Replace with a generic 'encrypted'-thumbnail
        result = static_cast<QBuffer *>(d->stream)->buffer();
    } else if (!isToBeEncrypted(d->fileName)) {
        result = static_cast<QBuffer *>(d->stream)->buffer();
    } else {
        m_bPasswordUsed = true;
        // New documents use the ODF 1.2+ baseline (AES-256-CBC, SHA256 start key)
        // rather than the legacy Blowfish/SHA1 scheme (Part 3 §4.8.1/§4.8.6).
        KoEncryptedStore_EncryptionData encData;
        encData.cipherAlgorithm = CipherAlgorithm::Aes256Cbc;
        encData.startKeyHashAlgorithm = QCryptographicHash::Sha256;
        QByteArray passwordHash = QCryptographicHash::hash(m_password, encData.startKeyHashAlgorithm);
        encData.initVector = randomArray(ivLengthFor(encData.cipherAlgorithm));
        encData.salt = randomArray(16);
        if (encData.initVector.isEmpty() || encData.salt.isEmpty()) {
            return false;
        }
        encData.iterationCount = 1024;
        encData.keySize = keyLengthFor(encData.cipherAlgorithm);

        const int keyLength = static_cast<int>(encData.keySize);

        QByteArray symmetricKey(keyLength, ' ');
        PKCS5_PBKDF2_HMAC_SHA1((char *)passwordHash.data(),
                               passwordHash.size(),
                               (unsigned char *)encData.salt.data(),
                               encData.salt.size(),
                               encData.iterationCount,
                               keyLength,
                               (unsigned char *)symmetricKey.data());

        // Get the written data
        QByteArray data = static_cast<QBuffer *>(d->stream)->buffer();
        encData.filesize = data.size();

        // Compress the data
        QBuffer compressedData;
        KCompressionDevice::CompressionType type = KCompressionDevice::compressionTypeForMimeType("application/x-gzip");
        KCompressionDevice compressDevice(&compressedData, false, type);

        compressDevice.setSkipHeaders();
        if (!compressDevice.open(QIODevice::WriteOnly)) {
            return false;
        }
        if (compressDevice.write(data) != data.size()) {
            return false;
        }
        compressDevice.close();

        // ODF 1.2 Part 3 §4.8.3: producers should use the SHA256/1K checksum.
        encData.checksumHashAlgorithm = QCryptographicHash::Sha256;
        encData.checksumShort = true;
        const QByteArray &checksumInput = compressedData.buffer().size() > 1024 ? compressedData.buffer().left(1024) : compressedData.buffer();
        encData.checksum = QCryptographicHash::hash(checksumInput, encData.checksumHashAlgorithm);

        // setup context to encrypt with the chosen cipher
        auto context = EVP_CIPHER_CTX_new();
        auto cryptoAlgorithm = evpCipherFor(encData.cipherAlgorithm);
        EVP_CIPHER_CTX_init(context);

        int ok = EVP_EncryptInit_ex(context, cryptoAlgorithm, nullptr, nullptr, nullptr);
        if (!ok)
            return false;

        ok = EVP_CIPHER_CTX_set_key_length(context, symmetricKey.size());
        if (!ok)
            return false;

        ok = EVP_EncryptInit_ex(context, nullptr, nullptr, (const unsigned char *)(symmetricKey.data()), (const unsigned char *)(encData.initVector.data()));
        if (!ok)
            return false;

        // Blowfish CFB needs no block padding; AES-CBC needs PKCS#7 padding applied.
        ok = EVP_CIPHER_CTX_set_padding(context, encData.cipherAlgorithm == CipherAlgorithm::Aes256Cbc ? 1 : 0);
        if (!ok)
            return false;

        // Encrypt the data
        int resultLength;
        result = QByteArray(compressedData.buffer().size() + EVP_CIPHER_CTX_block_size(context), ' ');
        ok = EVP_EncryptUpdate(context,
                               (unsigned char *)result.data(),
                               &resultLength,
                               (unsigned char *)compressedData.buffer().data(),
                               compressedData.buffer().size());
        if (!ok)
            return false;
        result.resize(resultLength);

        // Finalize
        QByteArray final;
        int finalLength;
        final.resize(EVP_CIPHER_CTX_block_size(context));
        ok = EVP_EncryptFinal_ex(context, (unsigned char *) final.data(), &finalLength);
        if (!ok)
            return false;
        final.resize(finalLength);

        result += final;

        // Cleanup
        EVP_CIPHER_CTX_cleanup(context);
        EVP_CIPHER_CTX_free(context);

        m_encryptionData.insert(d->fileName, encData);
    }

    if (!m_pZip->writeData(result.data(), result.size())) {
        m_pZip->finishWriting(result.size());
        return false;
    }

    return m_pZip->finishWriting(result.size());
}

bool KoEncryptedStore::enterRelativeDirectory(const QString &dirName)
{
    Q_D(KoStore);
    if (d->mode == Read) {
        if (!m_currentDir) {
            m_currentDir = m_pZip->directory(); // initialize
        }
        const KArchiveEntry *entry = m_currentDir->entry(dirName);
        if (entry && entry->isDirectory()) {
            m_currentDir = dynamic_cast<const KArchiveDirectory *>(entry);
            return m_currentDir != nullptr;
        }
        return false;
    } else { // Write, no checking here
        return true;
    }
}

bool KoEncryptedStore::enterAbsoluteDirectory(const QString &path)
{
    if (path.isEmpty()) {
        m_currentDir = nullptr;
        return true;
    }
    m_currentDir = dynamic_cast<const KArchiveDirectory *>(m_pZip->directory()->entry(path));
    return m_currentDir != nullptr;
}

bool KoEncryptedStore::fileExists(const QString &absPath) const
{
    const KArchiveEntry *entry = m_pZip->directory()->entry(absPath);
    return (entry && entry->isFile()) || (absPath == MANIFEST_FILE && !m_manifestBuffer.isNull());
}
