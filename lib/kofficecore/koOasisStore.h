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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef KOOASISSTORE_H
#define KOOASISSTORE_H

class QString;
class QDomDocument;
class KTempFile;
class KoXmlWriter;
class KoStore;
class KoStoreDevice;
class QIODevice;

/**
 * Helper class around KoStore for writing out OASIS files.
 * This class helps solving the problem that automatic styles must be before
 * the body, but it's easier to iterate over the application's objects only
 * once. So we open a KoXmlWriter into a memory buffer, write the body into it,
 * collect automatic styles while doing that, write out automatic styles,
 * and then copy the body XML from the buffer into the real KoXmlWriter.
 *
 * The typical use of this class is therefore:
 *   - write body into bodyWriter() and collect auto styles
 *   - write auto styles into contentWriter()
 *   - call closeContentWriter()
 *   - write other files into the store (styles.xml, settings.xml etc.)
 *
 *
 * TODO: maybe we could encapsulate a bit more things, to e.g. handle
 * adding manifest entries automatically.
 *
 * @author: David Faure <faure@kde.org>
 */
class KoOasisStore
{
public:
    /// @param store recontents the property of the caller
    KoOasisStore( KoStore* store );

    ~KoOasisStore();

    KoStore* store() const { return m_store; }

    /// Open contents.xml for writing and return the KoXmlWriter
    KoXmlWriter* contentWriter();

    /// Open another KoXmlWriter for writing out the contents
    /// into a temporary file, to collect automatic styles while doing that.
    KoXmlWriter* bodyWriter();

    /// This will copy the body into the content writer,
    /// delete the bodyWriter and the contentWriter, and then
    /// close contents.xml.
    bool closeContentWriter();

    /// For other files in the store, use open/addManifestEntry/KoStoreDevice/createOasisXmlWriter/close

    /// A completely unrelated method, for loading a file from an oasis store
    bool loadAndParse( const QString& fileName, QDomDocument& doc, QString& errorMessage );

private:
    KoStore* m_store;
    KoStoreDevice* m_storeDevice;
    KoXmlWriter* m_contentWriter;
    KoXmlWriter* m_bodyWriter;
    KTempFile* m_contentTmpFile;
};

#endif /* KOOASISSTORE_H */
