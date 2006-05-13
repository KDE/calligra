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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWOASISSAVER_H
#define KWOASISSAVER_H

#include <QBuffer>
//Added by qt3to4:
#include <QList>
#include <KoGenStyles.h>
#include <KoOasisStore.h>

class KWDocument;
class KoSavingContext;
class KoXmlWriter;
class KoTextParag;
class KoStore;
template <class T> class QList;

/**
 *  This is used to save a bunch of paragraphs into a oasis store in memory,
 *  as some features do (e.g. sort text, or convert table to text)
 *
 *  Note: this is NOT used for File/Save!
 */
class KWOasisSaver
{
public:
    /// @param doc only needed for the user-style-collection
    KWOasisSaver( KWDocument* doc );
    ~KWOasisSaver();

    /// Save a list of paragraphs
    void saveParagraphs( const QList<const KoTextParag *>& paragraphs );

    /// Save a paragraph
    void saveParagraph( const KoTextParag* parag );

    /// Another way to save paragraphs is to use KoTextObject::saveOasisContent
    /// It needs a body writer and a saving context.
    KoXmlWriter& bodyWriter() { return *m_oasisStore->bodyWriter(); }
    KoSavingContext& savingContext() { return *m_savingContext; }

    /// You MUST call this after saving the paragraphs and before calling data()
    /// Return true on success
    bool finish();

    /// Return the entire ZIP store as a byte array
    QByteArray data() const;

    /// Helper function, also used by KWDocument.
    static void writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles, bool stylesDotXml );

    static const char* selectionMimeType();

private:
    QBuffer m_buffer;
    KoStore* m_store;
    KoOasisStore* m_oasisStore;
    KoGenStyles m_mainStyles;
    KoSavingContext* m_savingContext;
    KWDocument* m_doc;
};

#endif
