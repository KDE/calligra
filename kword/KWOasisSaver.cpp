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

#include "KWOasisSaver.h"
#include <KoStore.h>
#include <KoOasisStore.h>
#include <KoOasisContext.h>
#include <KoXmlWriter.h>
#include "KWDocument.h"
//Added by qt3to4:
#include <Q3ValueList>

KWOasisSaver::KWOasisSaver( KWDocument* doc )
    : m_doc( doc )
{
    const QByteArray mimeType = selectionMimeType();
    m_store = KoStore::createStore( &m_buffer, KoStore::Write, mimeType );
    Q_ASSERT( m_store );
    Q_ASSERT( !m_store->bad() );

    m_oasisStore = new KoOasisStore( m_store );

    //KoXmlWriter* manifestWriter = oasisStore.manifestWriter( mimeType );

    m_savingContext = new KoSavingContext( m_mainStyles, 0, false, KoSavingContext::Store );

    KoXmlWriter* bodyWriter = m_oasisStore->bodyWriter();
    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:text" );
}

void KWOasisSaver::saveParagraphs( const Q3ValueList<const KoTextParag *>& paragraphs )
{
    for ( Q3ValueList<const KoTextParag *>::const_iterator it = paragraphs.begin(),
                                                   end = paragraphs.end();
                  it != end ; ++it ) {
        saveParagraph( *it );
    }
}

void KWOasisSaver::saveParagraph( const KoTextParag* parag )
{
    // keep in sync with KoTextDocument::copySelection
    KoXmlWriter* bodyWriter = m_oasisStore->bodyWriter();
    parag->saveOasis( *bodyWriter, *m_savingContext, 0, parag->length()-2, true );
}

QByteArray KWOasisSaver::data() const
{
    Q_ASSERT( !m_store ); // forgot to call finish() (or to check its return value) ?
    return m_buffer.buffer();
}

KWOasisSaver::~KWOasisSaver()
{
    delete m_store;
    delete m_oasisStore;
    delete m_savingContext;
}

bool KWOasisSaver::finish()
{
    KoXmlWriter* bodyWriter = m_oasisStore->bodyWriter();
    bodyWriter->endElement(); // office:text
    bodyWriter->endElement(); // office:body

    KoXmlWriter* contentWriter = m_oasisStore->contentWriter();
    Q_ASSERT( contentWriter );

    m_savingContext->writeFontFaces( *contentWriter );
    contentWriter->startElement( "office:automatic-styles" );
    writeAutomaticStyles( *contentWriter, m_mainStyles, false );
    contentWriter->endElement(); // office:automatic-styles

    m_oasisStore->closeContentWriter();

    if ( !m_store->open( "styles.xml" ) )
        return false;
    //manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    m_doc->saveOasisDocumentStyles( m_store, m_mainStyles, *m_savingContext,
                                    KWDocument::SaveSelected /* simply means not SaveAll */,
                                    QByteArray() /* no headers/footers */ );
    if ( !m_store->close() ) // done with styles.xml
        return false;

    delete m_oasisStore; m_oasisStore = 0;
    delete m_store; m_store = 0;

    return true;
}

void KWOasisSaver::writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles, bool stylesDotXml )
{
    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_AUTO, stylesDotXml );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_AUTO_LIST, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "text:list-style", (*it).name, 0 );
    }

    styles = mainStyles.styles( KWDocument::STYLE_FRAME_AUTO, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }

    styles = mainStyles.styles( KWDocument::STYLE_TABLE, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:table-properties"  );
    }

    styles = mainStyles.styles( KWDocument::STYLE_TABLE_COLUMN, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:table-column-properties"  );
    }

    styles = mainStyles.styles( KWDocument::STYLE_TABLE_CELL_AUTO, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:table-cell-properties"  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_DATE, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:date-style", (*it).name, 0 /*TODO ????*/  );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_TIME, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:time-style", (*it).name, 0 /*TODO ????*/  );
    }
}

const char* KWOasisSaver::selectionMimeType()
{
    return "application/vnd.oasis.opendocument.text";
}
