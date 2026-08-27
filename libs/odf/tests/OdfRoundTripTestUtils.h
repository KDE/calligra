/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ODFROUNDTRIPTESTUTILS_H
#define ODFROUNDTRIPTESTUTILS_H

#include <QBuffer>
#include <QByteArray>
#include <QVector>

#include <KoXmlReader.h>
#include <KoXmlWriter.h>

/**
 * Reusable helpers for tests that load/save a type's own ODF fragment
 * (a Ko*::loadOdf(const KoXmlElement &)/saveOdf*(KoXmlWriter &) pair)
 * without a full document/store, cutting down per-test boilerplate.
 *
 * Note: a KoXmlDocument owns the data behind every element parsed from it,
 * which dangles once the document is destroyed -- so both helpers take the
 * document as an out-parameter; keep it alive as long as you use the result.
 */
namespace OdfTestUtils
{
/** A namespace declaration to put on the root element, e.g. {"dr3d", KoXmlNS::dr3d}. */
struct Namespace {
    QByteArray prefix;
    QString uri;
};

/** Parses a self-contained XML fragment into `doc` and returns its root element (null on parse failure). */
inline KoXmlElement parseXmlFragment(const QString &xml, KoXmlDocument &doc)
{
    if (!doc.setContent(xml, true)) {
        return KoXmlElement();
    }
    return doc.documentElement();
}

/**
 * Writes `rootElementName` with the given namespace declarations, calls
 * `writeContents(writer)` to fill it in (typically the type's saveOdf*()
 * methods), then reparses the result into `doc` and returns its root
 * element -- a full write-then-reparse round trip.
 */
template<typename WriteContentsFn>
KoXmlElement writeAndReparse(const char *rootElementName, const QVector<Namespace> &namespaces, WriteContentsFn writeContents, KoXmlDocument &doc)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    {
        KoXmlWriter writer(&buffer);
        writer.startElement(rootElementName);
        for (const Namespace &ns : namespaces) {
            const QByteArray attrName = QByteArray("xmlns:") + ns.prefix;
            writer.addAttribute(attrName.constData(), ns.uri);
        }
        writeContents(writer);
        writer.endElement();
    }
    buffer.close();

    return parseXmlFragment(QString::fromUtf8(buffer.data()), doc);
}
}

#endif
