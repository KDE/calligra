// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QBuffer>
#include <QCryptographicHash>
#include <QLoggingCategory>
#include <QObject>
#include <QTemporaryDir>
#include <QTest>

#include <KCompressionDevice>
#include <KoStore.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <kzip.h>

#include <openssl/evp.h>
#include <openssl/opensslv.h>
#include <openssl/rand.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/provider.h>
#endif

using namespace Qt::StringLiterals;

namespace
{
QByteArray randomBytes(int size)
{
    QByteArray buf(size, ' ');
    while (RAND_bytes(reinterpret_cast<unsigned char *>(buf.data()), size) != 1) { }
    return buf;
}

QByteArray gzipCompress(const QByteArray &data)
{
    QBuffer compressed;
    KCompressionDevice device(&compressed, false, KCompressionDevice::compressionTypeForMimeType("application/x-gzip"));
    device.setSkipHeaders();
    Q_UNUSED(device.open(QIODevice::WriteOnly));
    device.write(data);
    device.close();
    return compressed.buffer();
}

// Encrypts `plainContent` exactly the way KoEncryptedStore used to (Blowfish
// CFB, PBKDF2-HMAC-SHA1 over a SHA1 "start key", 16-byte key, 8-byte IV, no
// start-key-generation element) and writes a complete, self-contained
// encrypted ODF store containing a single file at `entryName`, at `path`.
// `useSpecUriForms` switches key-derivation-name/checksum-type/start-key-
// generation-name from their legacy literal spellings to the alternate IRI
// forms ODF 1.2 Part 3 also requires consumers to accept.
void writeLegacyBlowfishStore(const QString &path, const QByteArray &password, const QByteArray &plainContent, const QString &entryName, bool useSpecUriForms)
{
    const QByteArray compressed = gzipCompress(plainContent);
    const QByteArray salt = randomBytes(16);
    const QByteArray iv = randomBytes(8);
    constexpr int iterationCount = 1024;
    constexpr int keyLength = 16;

    const QByteArray startKey = QCryptographicHash::hash(password, QCryptographicHash::Sha1);
    QByteArray symmetricKey(keyLength, ' ');
    PKCS5_PBKDF2_HMAC_SHA1(startKey.constData(),
                           startKey.size(),
                           reinterpret_cast<const unsigned char *>(salt.constData()),
                           salt.size(),
                           iterationCount,
                           keyLength,
                           reinterpret_cast<unsigned char *>(symmetricKey.data()));

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    static OSSL_PROVIDER *legacyProvider = OSSL_PROVIDER_load(nullptr, "legacy");
    Q_UNUSED(legacyProvider);
#endif
    auto *context = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(context);
    EVP_EncryptInit_ex(context, EVP_bf_cfb(), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_set_key_length(context, symmetricKey.size());
    EVP_EncryptInit_ex(context,
                       nullptr,
                       nullptr,
                       reinterpret_cast<const unsigned char *>(symmetricKey.constData()),
                       reinterpret_cast<const unsigned char *>(iv.constData()));
    EVP_CIPHER_CTX_set_padding(context, 0);

    QByteArray ciphertext(compressed.size() + EVP_CIPHER_CTX_block_size(context), ' ');
    int len = 0;
    EVP_EncryptUpdate(context,
                      reinterpret_cast<unsigned char *>(ciphertext.data()),
                      &len,
                      reinterpret_cast<const unsigned char *>(compressed.constData()),
                      compressed.size());
    ciphertext.resize(len);
    QByteArray final(EVP_CIPHER_CTX_block_size(context), ' ');
    int finalLen = 0;
    EVP_EncryptFinal_ex(context, reinterpret_cast<unsigned char *>(final.data()), &finalLen);
    final.resize(finalLen);
    ciphertext += final;
    EVP_CIPHER_CTX_cleanup(context);
    EVP_CIPHER_CTX_free(context);

    const QByteArray checksumInput = compressed.size() > 1024 ? compressed.left(1024) : compressed;
    const QByteArray checksum = QCryptographicHash::hash(checksumInput, QCryptographicHash::Sha1);

    const QString keyDerivationName = useSpecUriForms ? u"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0#pbkdf2"_s : u"PBKDF2"_s;
    const QString checksumType = useSpecUriForms ? u"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0#sha1-1k"_s : u"SHA1/1K"_s;
    const QString startKeyGenerationName = useSpecUriForms ? u"http://www.w3.org/2000/09/xmldsig#sha1"_s : QString();

    QString manifestXml =
        u"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" manifest:version=\"1.2\">"
        "<manifest:file-entry manifest:full-path=\"/\" manifest:version=\"1.2\" manifest:media-type=\"application/vnd.oasis.opendocument.text\"/>"
        "<manifest:file-entry manifest:full-path=\"%1\" manifest:size=\"%2\" manifest:media-type=\"\">"
        "<manifest:encryption-data manifest:checksum-type=\"%3\" manifest:checksum=\"%4\">"
        "<manifest:algorithm manifest:algorithm-name=\"Blowfish CFB\" manifest:initialisation-vector=\"%5\"/>"_s
            .arg(entryName, QString::number(plainContent.size()), checksumType, QString::fromUtf8(checksum.toBase64()), QString::fromUtf8(iv.toBase64()));
    if (useSpecUriForms) {
        manifestXml += u"<manifest:start-key-generation manifest:start-key-generation-name=\"%1\"/>"_s.arg(startKeyGenerationName);
    }
    manifestXml +=
        u"<manifest:key-derivation manifest:key-derivation-name=\"%1\" manifest:iteration-count=\"%2\" manifest:salt=\"%3\"/>"
        "</manifest:encryption-data>"
        "</manifest:file-entry>"
        "</manifest:manifest>"_s.arg(keyDerivationName, QString::number(iterationCount), QString::fromUtf8(salt.toBase64()));

    KZip zip(path);
    Q_UNUSED(zip.open(QIODevice::WriteOnly));
    zip.setExtraField(KZip::NoExtraField);
    zip.setCompression(KZip::NoCompression);
    zip.writeFile(QStringLiteral("mimetype"), QByteArray("application/vnd.oasis.opendocument.text"));
    zip.prepareWriting(entryName, QString(), QString(), 0);
    zip.writeData(ciphertext.constData(), ciphertext.size());
    zip.finishWriting(ciphertext.size());
    zip.setCompression(KZip::DeflateCompression);
    zip.writeFile(QStringLiteral("META-INF/manifest.xml"), manifestXml.toUtf8());
    zip.close();
}
}

class TestKoEncryptedStore : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testRoundTrip();
    void testManifestUsesAes256CbcAndSha256StartKey();
    void testReadsLegacyBlowfishSha1File();
    void testReadsLegacyFileWithSpecUriForms();
};

void TestKoEncryptedStore::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.store=true");
}

void TestKoEncryptedStore::testRoundTrip()
{
    QTemporaryDir dir;
    const QString path = dir.filePath(QStringLiteral("test.zip"));
    const QByteArray content = "Hello, encrypted world!";

    KoStore *writeStore = KoStore::createStore(path, KoStore::Write, "", KoStore::Encrypted);
    QVERIFY(writeStore);
    QVERIFY(!writeStore->bad());
    QVERIFY(writeStore->setPassword("correct horse battery staple"));
    QVERIFY(writeStore->open("content.txt"));
    QCOMPARE(writeStore->write(content), qint64(content.size()));
    writeStore->close();
    delete writeStore;

    KoStore *readStore = KoStore::createStore(path, KoStore::Read, "", KoStore::Encrypted);
    QVERIFY(readStore);
    QVERIFY(!readStore->bad());
    QVERIFY(readStore->isEncrypted());
    QVERIFY(readStore->setPassword("correct horse battery staple"));
    QVERIFY(readStore->open("content.txt"));
    QCOMPARE(readStore->device()->readAll(), content);
    readStore->close();
    delete readStore;
}

void TestKoEncryptedStore::testManifestUsesAes256CbcAndSha256StartKey()
{
    QTemporaryDir dir;
    const QString path = dir.filePath(QStringLiteral("test.zip"));

    KoStore *writeStore = KoStore::createStore(path, KoStore::Write, "", KoStore::Encrypted);
    QVERIFY(writeStore);
    QVERIFY(writeStore->setPassword("hunter2"));
    QVERIFY(writeStore->open("content.txt"));
    writeStore->write(QByteArray("some content"));
    writeStore->close();
    delete writeStore;

    KZip zip(path);
    QVERIFY(zip.open(QIODevice::ReadOnly));
    const KArchiveEntry *manifestEntry = zip.directory()->entry(QStringLiteral("META-INF/manifest.xml"));
    QVERIFY(manifestEntry && manifestEntry->isFile());
    QIODevice *manifestDevice = static_cast<const KArchiveFile *>(manifestEntry)->createDevice();

    KoXmlDocument doc;
    QVERIFY(doc.setContent(manifestDevice, true));
    manifestDevice->close();
    delete manifestDevice;
    zip.close();

    KoXmlElement encryptionData;
    KoXmlElement elem;
    forEachElement(elem, doc.documentElement())
    {
        if (elem.localName() != QLatin1String("file-entry") || elem.attribute("full-path") != QLatin1String("content.txt")) {
            continue;
        }
        encryptionData = elem.namedItemNS(KoXmlNS::manifest, "encryption-data").toElement();
    }
    QVERIFY(!encryptionData.isNull());

    const KoXmlElement algorithm = encryptionData.namedItemNS(KoXmlNS::manifest, "algorithm").toElement();
    QVERIFY(!algorithm.isNull());
    QCOMPARE(algorithm.attribute("algorithm-name"), QStringLiteral("http://www.w3.org/2001/04/xmlenc#aes256-cbc"));
    QCOMPARE(QByteArray::fromBase64(algorithm.attribute("initialisation-vector").toLatin1()).size(), 16);

    const KoXmlElement startKeyGeneration = encryptionData.namedItemNS(KoXmlNS::manifest, "start-key-generation").toElement();
    QVERIFY(!startKeyGeneration.isNull());
    QCOMPARE(startKeyGeneration.attribute("start-key-generation-name"), QStringLiteral("http://www.w3.org/2000/09/xmldsig#sha256"));

    const KoXmlElement keyDerivation = encryptionData.namedItemNS(KoXmlNS::manifest, "key-derivation").toElement();
    QVERIFY(!keyDerivation.isNull());
    QCOMPARE(keyDerivation.attribute("key-derivation-name"), QStringLiteral("PBKDF2"));
    QCOMPARE(keyDerivation.attribute("key-size"), QStringLiteral("32"));
    QCOMPARE(QByteArray::fromBase64(keyDerivation.attribute("salt").toLatin1()).size(), 16);

    QCOMPARE(encryptionData.attribute("checksum-type"), QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:manifest:1.0#sha256-1k"));
}

void TestKoEncryptedStore::testReadsLegacyBlowfishSha1File()
{
    QTemporaryDir dir;
    const QString path = dir.filePath(QStringLiteral("legacy.zip"));
    const QByteArray content = "This file was encrypted the old way (Blowfish CFB + SHA1 start key).";
    writeLegacyBlowfishStore(path, "legacypw", content, QStringLiteral("content.txt"), false);

    KoStore *store = KoStore::createStore(path, KoStore::Read, "", KoStore::Encrypted);
    QVERIFY(store);
    QVERIFY(!store->bad());
    QVERIFY(store->isEncrypted());
    QVERIFY(store->setPassword("legacypw"));
    QVERIFY(store->open("content.txt"));
    QCOMPARE(store->device()->readAll(), content);
    store->close();
    delete store;
}

void TestKoEncryptedStore::testReadsLegacyFileWithSpecUriForms()
{
    // Same legacy Blowfish/SHA1 scheme, but the manifest spells key-derivation-name,
    // checksum-type and start-key-generation-name using the alternate IRI forms
    // ODF 1.2 Part 3 also requires consumers to accept, rather than the plain
    // literal strings -- see parseKeyDerivationName()/parseChecksumType()/
    // parseStartKeyGenerationName() in KoEncryptedStore.cpp.
    QTemporaryDir dir;
    const QString path = dir.filePath(QStringLiteral("legacy-uri.zip"));
    const QByteArray content = "Same scheme, spelled with the spec's alternate IRIs.";
    writeLegacyBlowfishStore(path, "otherpw", content, QStringLiteral("content.txt"), true);

    KoStore *store = KoStore::createStore(path, KoStore::Read, "", KoStore::Encrypted);
    QVERIFY(store);
    QVERIFY(!store->bad());
    QVERIFY(store->isEncrypted());
    QVERIFY(store->setPassword("otherpw"));
    QVERIFY(store->open("content.txt"));
    QCOMPARE(store->device()->readAll(), content);
    store->close();
    delete store;
}

QTEST_MAIN(TestKoEncryptedStore)
#include "TestKoEncryptedStore.moc"
