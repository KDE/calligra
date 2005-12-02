/* This file is part of the KDE project
   Copyright (C) 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOOASISLOADINGCONTEXT_H
#define KOOASISLOADINGCONTEXT_H

class KoXmlWriter;
class QDomElement;
class KoDocument;
class KoOasisStyles;
class KoPictureCollection;
class KoStore;

#include <qmap.h>
#include <koffice_export.h>
#include <qstringlist.h>
#include <koStyleStack.h>

/**
 * Used during loading of Oasis format (and discarded at the end of the loading).
 *
 * @author David Faure <faure@kde.org>
 */
class KOFFICECORE_EXPORT KoOasisLoadingContext
{
public:
    /**
     * Stores reference to the KoOasisStyles and stored passed by KoDocument.
     * Make sure that the KoOasisStyles instance outlives this KoOasisLoadingContext instance.
     * (This is the case during loading, when using the KoOasisStyles given by KoDocument)
     *
     * @param doc the KoDocument being loaded
     * @param styles reference to the KoOasisStyles parsed by KoDocument
     * @param store pointer to store, if available, for e.g. loading images.
     */
    KoOasisLoadingContext( KoDocument* doc, KoOasisStyles& styles, KoStore* store );
    ~KoOasisLoadingContext();

    KoDocument* koDocument() { return m_doc; }
    KoStore* store() { return m_store; }

    KoOasisStyles& oasisStyles() { return m_styles; }
    KoStyleStack& styleStack() { return m_styleStack; }

    const QDomDocument& manifestDocument() const { return m_manifestDoc; }

    /// Return the <meta:generator> of the document, e.g. "KOffice/1.4.0a"
    QString generator() const;

    void fillStyleStack( const QDomElement& object, const char* nsURI, const char* attrName );
    void addStyles( const QDomElement* style );

private:
    void parseMeta() const;

private:
    KoDocument* m_doc;
    KoStore* m_store;
    KoOasisStyles& m_styles;
    KoStyleStack m_styleStack;

    mutable QString m_generator;
    mutable bool m_metaXmlParsed;
    bool m_unused1;

    QDomDocument m_manifestDoc;

    class Private;
    Private *d;
};

#endif /* KOOASISLOADINGCONTEXT_H */

