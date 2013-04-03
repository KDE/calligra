/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOXMLSTREAMREADER_H
#define KOXMLSTREAMREADER_H


#include "KoXmlStreamReader.h"

#include <QXmlStreamReader>
#include <QStringRef>

#include "koodf_export.h"

class QByteArray;
class QString;
class QIODevice;


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
class KOODF_EXPORT KoXmlStreamReader : public QXmlStreamReader
{
public:

    KoXmlStreamReader();
    KoXmlStreamReader(QIODevice *device);
    KoXmlStreamReader(const QByteArray &data);
    KoXmlStreamReader(const QString &data);
    KoXmlStreamReader(const char *data);

    ~KoXmlStreamReader();

    void clear();

    void addExpectedNamespace(QString prefix, QString namespaceUri);
    void addExtraNamespace(QString prefix, QString namespaceUri);

    // --------------------------------
    // Reimplemented from QXmlStreamReader

    QStringRef qualifiedName() const;

    void setDevice(QIODevice *device);

private:
    // No copying
    KoXmlStreamReader(KoXmlStreamReader&other);
    KoXmlStreamReader &operator=(KoXmlStreamReader&other);

    class Private;
    Private * const d;
};


void prepareForOdf(KoXmlStreamReader &reader);


#endif /* KOGENCHANGES_H */
