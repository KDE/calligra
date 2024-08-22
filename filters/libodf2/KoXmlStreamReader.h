/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOXMLSTREAMREADER_H
#define KOXMLSTREAMREADER_H

#include "KoXmlStreamReader.h"

#include <QSharedData>
#include <QVector>
#include <QXmlStreamReader>

#include "koodf2_export.h"

class QByteArray;
class QString;
class QIODevice;

class KoXmlStreamAttributes;

/**
 * @brief An XML stream reader based on QXmlStreamReader and with namespace handling better suited to use for ODF in Calligra.
 *
 * Opendocument uses an XML encoding which makes heavy use of
 * namespaces. So normally you would want to compare the qualified
 * name when accessing tagnames and attributes.
 *
 * However, in QXmlStreamReader you have to either make an explicit
 * comparison with the namespace URI for every element and attribute
 * or risk that documents that use the correct namespaces but not the
 * normal namespace prefixes are wrongly interpreted.  This is because
 * the prefix (e.g. "fo" in "fo:border-width") is declared at the
 * beginning of the document using a namespace declaration attribute
 * such as: xmlns:fo="http://www.w3.org/1999/XSL/Format". In this case
 * xmlns:fo could just as well be xmlns:xxx which makes the expected
 * fo:border-width become xxx:border-width in the rest of this
 * document.
 *
 * However, it is extremely rare to find document that uses such
 * non-standard namespace prefixes. This gives us the opportunity to
 * optimize for the common case, which is exactly what
 * KoXmlStreamReader does.
 *
 * The way to use this class is to tell it which namespaces and
 * prefixes that you expect before you open the XML stream. Then it
 * checks if the namespaces and prefixes in the document are the same
 * as the expected ones.  If they are in fact the same, the document
 * is pronounced "sound", and for the rest of the processing you can
 * use the qualified name with the expected prefix ("fo:border-width")
 * with the maximum performance.
 *
 * If the namespace(s) in the document are the expected ones but the
 * prefix(es) are not, you can still compare the qualified name to
 * your expected ones.  But in this case the document is deemed
 * "unsound" and for every access to attributes or calls to
 * qualifiedName(), KoXmlStreamReader will rewrite the actual name in
 * the document to become what you expect.  The functions
 * namespaceUri() and name() are not affected, only the prefixes.
 */
class KOODF2_EXPORT KoXmlStreamReader : public QXmlStreamReader
{
    friend class KoXmlStreamAttribute;
    friend class KoXmlStreamAttributes;

public:
    KoXmlStreamReader();
    explicit KoXmlStreamReader(QIODevice *device);
    explicit KoXmlStreamReader(const QByteArray &data);
    explicit KoXmlStreamReader(const QString &data);
    explicit KoXmlStreamReader(const char *data);

    ~KoXmlStreamReader();

    void clear();

    void addExpectedNamespace(const QString &prefix, const QString &namespaceUri);
    void addExtraNamespace(const QString &prefix, const QString &namespaceUri);

    // --------------------------------
    // Reimplemented from QXmlStreamReader

    QStringView prefix() const;
    QStringView qualifiedName() const;
    void setDevice(QIODevice *device);
    KoXmlStreamAttributes attributes() const;

private:
    // No copying
    KoXmlStreamReader(const KoXmlStreamReader &other) = delete;
    KoXmlStreamReader &operator=(const KoXmlStreamReader &other) = delete;

    // Only for friend classes KoXmlStreamAttributes and KoXmlStreamAttribute.
    bool isSound() const;

    class Private;
    Private *const d;
};

/**
 * @brief KoXmlStreamAttribute is a source-compatible replacement for QXmlStreamAttribute.
 *
 * In addition to the API from QXmlStreamAttribute, it offers the same
 * advantages that KoXmlStreamReader does over QXmlStreamReader: when
 * asked for the qualified name of an attribute it will return the
 * expected one even if the prefix declared in the namespace
 * declaration of the document is different.
 *
 * @see KoXmlStreamReader
 */
class KOODF2_EXPORT KoXmlStreamAttribute
{
    friend QList<KoXmlStreamAttribute>; // For the default constructor
    friend KoXmlStreamAttributes; // For the normal constructor
    friend KoXmlStreamReader;

public:
    ~KoXmlStreamAttribute();

    // TODO move that back to the private part
    KoXmlStreamAttribute();
    KoXmlStreamAttribute(const KoXmlStreamAttribute &other);
    KoXmlStreamAttribute(const QXmlStreamAttribute *attr, const KoXmlStreamReader *reader);

    // API taken from QXmlStreamAttribute
    bool isDefault() const;
    QStringView name() const;
    QStringView namespaceUri() const;
    QStringView prefix() const;
    QStringView qualifiedName() const;
    QStringView value() const;

    bool operator==(const KoXmlStreamAttribute &other) const;
    bool operator!=(const KoXmlStreamAttribute &other) const;
    KoXmlStreamAttribute &operator=(const KoXmlStreamAttribute &other);

private:
    class Private;
    Private *const d;
};

/**
 * @brief KoXmlStreamAttributes is a mostly source-compatible replacement for QXmlStreamAttributes.
 *
 * All the convenience functions of KoXmlStreamAttributes work exactly
 * like the counterparts of QXmlStreamAttributes but they give the
 * expected prefix for the registered expected namespaces.
 *
 * Not all functions from QVector are implemented but the ones that
 * make sense for this read-only class are. This class can only be
 * used in connection with KoXmlStreamReader.
 *
 * @see KoXmlStreamReader
 */
class KOODF2_EXPORT KoXmlStreamAttributes
{
    friend class KoXmlStreamReader;

public:
    using const_iterator = const QList<KoXmlStreamAttribute>::const_iterator;

    KoXmlStreamAttributes(const KoXmlStreamAttributes &other);
    ~KoXmlStreamAttributes();

    KoXmlStreamAttributes &operator=(const KoXmlStreamAttributes &other);

    // Relevant parts of the QVector API
    const KoXmlStreamAttribute &at(int i) const;
    int size() const;
    KoXmlStreamAttribute value(int i) const;
    const KoXmlStreamAttribute &operator[](int i) const;
    const_iterator begin() const;
    const_iterator end() const;

    // Convenience functions taken from QXmlStreamAttributes API
    void append(const QString &namespaceUri, const QString &name, const QString &value);
    void append(const QXmlStreamAttribute &attribute);
    void append(const QString &qualifiedName, const QString &value);
    bool hasAttribute(const QString &qualifiedName) const;
    bool hasAttribute(const QLatin1String &qualifiedName) const;
    bool hasAttribute(const QString &namespaceUri, const QString &name) const;
    QStringView value(const QString &namespaceUri, const QString &name) const;
    QStringView value(const QString &namespaceUri, const QLatin1String &name) const;
    QStringView value(const QLatin1String &namespaceUri, const QLatin1String &name) const;
    QStringView value(const QString &qualifiedName) const;
    QStringView value(const QLatin1String &qualifiedName) const;

private:
    // Only available from friend class KoXmlStreamReader.
    KoXmlStreamAttributes(const KoXmlStreamReader *r, const QXmlStreamAttributes &qAttrs);

    // This class is implicitly shared.
    class Private;
    QSharedDataPointer<Private> d;
};

void KOODF2_EXPORT prepareForOdf(KoXmlStreamReader &reader);

#endif /* KOXMLSTREAMREADER_H */
