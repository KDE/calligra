/* This file is part of the KDE project
   Copyright (C) 2001-2005 David Faure <faure@kde.org>

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

#include "KWCommand.h"
#include "KWDocument.h"
#include "KWTextDocument.h"
#include "KWTextFrameSet.h"
#include "KWTextParag.h"
#include "KWLoadingInfo.h"
#include "KWVariable.h"
#include "KWAnchor.h"
#include "KWOasisLoader.h"
#include "KWTableFrameSet.h"

#include <KoOasisContext.h>
#include <KoXmlNS.h>
#include <KoDom.h>

#include <kdebug.h>
#include <kglobalsettings.h>
#include <klocale.h>

KWTextDocument::KWTextDocument( KWTextFrameSet * textfs, KoTextFormatCollection *fc, KoTextFormatter *formatter )
    : KoTextDocument( textfs->kWordDocument(), fc, formatter, false ), m_textfs( textfs )
{
    init();
}

KWTextDocument::KWTextDocument( KoTextZoomHandler * zoomHandler )
    : KoTextDocument( zoomHandler, new KoTextFormatCollection( KGlobalSettings::generalFont() /*unused*/, QColor(), KGlobal::locale()->language(), false), 0L, false ),
      m_textfs( 0 )
{
    init();
}

void KWTextDocument::init()
{
    // Create initial paragraph as a KWTextParag
    clear( true );
}

KWTextDocument::~KWTextDocument()
{
}

KoTextParag * KWTextDocument::createParag( KoTextDocument *d, KoTextParag *pr, KoTextParag *nx, bool updateIds )
{
    return new KWTextParag( static_cast<KoTextDocument *>(d), static_cast<KoTextParag *>(pr), static_cast<KoTextParag *>(nx), updateIds );
}

KoTextDocCommand *KWTextDocument::deleteTextCommand( KoTextDocument *textdoc, int id, int index, const QMemArray<KoTextStringChar> & str, const CustomItemsMap & customItemsMap, const QValueList<KoParagLayout> & oldParagLayouts )
{
    //kdDebug(32500)<<" KoTextDocument::deleteTextCommand************\n";
    return new KWTextDeleteCommand( textdoc, id, index, str, customItemsMap, oldParagLayouts );
}

void KWTextDocument::loadOasisTOC( const QDomElement& tag, KoOasisContext& context, KoTextParag* & lastParagraph, KoStyleCollection * styleColl, KoTextParag* nextParagraph )
{
    // table-of-content OOo SPEC 7.5 p452
    //fillStyleStack( tag, "text:style-name" ); that's the section style

    //QDomElement tocSource = KoDom::namedItemNS( toc, KoXmlNS::text, "table-of-content-source" );
    // TODO parse templates and generate "Contents ..." styles from it
    //for ( QDomNode n(tocSource.firstChild()); !text.isNull(); text = text.nextSibling() )
    //{
    //}

    QDomElement tocIndexBody = KoDom::namedItemNS( tag, KoXmlNS::text, "index-body" );
    QDomElement t;
    forEachElement( t, tocIndexBody )
    {
        context.styleStack().save();
        const QString localName = t.localName();
        QDomElement e;
        bool isTextNS = tag.namespaceURI() == KoXmlNS::text;
        if ( isTextNS && localName == "index-title" ) {
            lastParagraph = loadOasisText( t, context, lastParagraph, styleColl, nextParagraph ); // recurse again
            lastParagraph->setPartOfTableOfContents( true );
        } else if ( isTextNS && localName == "p" ) {
            context.fillStyleStack( t, KoXmlNS::text, "style-name", "paragraph" );
            lastParagraph = createParag( this, lastParagraph, nextParagraph );
            uint pos = 0;
            lastParagraph->loadOasis( t, context, styleColl, pos );
            lastParagraph->setPartOfTableOfContents( true );
        } else
            kdWarning() << "OASIS TOC loading: unknown tag " << t.tagName() << " found in index-body" << endl;
        context.styleStack().restore();
    }

    m_textfs->kWordDocument()->setTocPresent( true );
}

bool KWTextDocument::loadOasisBodyTag( const QDomElement& tag, KoOasisContext& context,
                                       KoTextParag* & lastParagraph, KoStyleCollection* styleColl,
                                       KoTextParag* nextParagraph )
{
    const QString localName( tag.localName() );
    // Non-inline frame (i.e. anchored to page)
    if ( localName == "frame" && tag.namespaceURI() == KoXmlNS::draw )
    {
        KWDocument* doc = m_textfs->kWordDocument();
        KWOasisLoader loader( doc );
        KWFrame* frame = loader.loadFrame( tag, context, KoPoint() );
        if ( frame )
            return true;
    }

    // Anchored-to-paragraph table.
    else if ( localName == "table" && tag.namespaceURI() == KoXmlNS::table )
    {
        KWDocument* doc = m_textfs->kWordDocument();
        KWOasisLoader loader( doc );
        KWTableFrameSet* table = loader.loadOasisTable( tag, context );
        table->finalize();
        // Create paragraph for this table
        KoTextParag *parag = createParag( this, lastParagraph, nextParagraph );
        if ( !lastParagraph )        // First parag
            setFirstParag( parag );
        lastParagraph = parag;
        // Put inline table in that paragraph
        parag->insert( 0, KoTextObject::customItemChar() );
        table->setAnchorFrameset( m_textfs );
        parag->setCustomItem( 0, table->createAnchor( m_textfs->textDocument(), 0 ), 0 );
        return true;
    }
    else if ( localName == "table-of-content" && tag.namespaceURI() == KoXmlNS::text )
    {
        loadOasisTOC( tag, context, lastParagraph, styleColl, nextParagraph );
        return true;
    }

    return false;
}

void KWTextDocument::appendBookmark( KoTextParag* parag, int pos, KoTextParag* endParag, int endPos, const QString& name )
{
    // The OASIS format is cool. No need to store the bookmarks until end of loading (e.g. KWLoadingInfo)
    // We can "resolve" them right away.
    m_textfs->kWordDocument()->insertBookmark( name, parag, endParag, pos, endPos );
}

void KWTextDocument::loadOasisFootnote( const QDomElement& tag, KoOasisContext& context,
                                        KoTextCustomItem* & customItem )
{
    const QString frameName( tag.attributeNS( KoXmlNS::text, "id", QString::null) );
    const QString localName( tag.localName() );
    const QDomElement citationElem = tag.namedItem( localName + "-citation" ).toElement();

    bool endnote = localName == "endnote" && tag.namespaceURI() == KoXmlNS::text;

    QString label = citationElem.attributeNS( KoXmlNS::text, "label", QString::null );
    bool autoNumbered = label.isEmpty();

    KWFootNoteFrameSet *fs = m_textfs->insertFootNote(
        endnote ? EndNote : FootNote,
        autoNumbered ? KWFootNoteVariable::Auto : KWFootNoteVariable::Manual,
        label );
    customItem = fs->footNoteVariable();

    fs->createInitialFrame( 0 ); // we don't know the page number...

    // Parse contents into the frameset
    const QDomElement bodyElem = KoDom::namedItemNS( tag, KoXmlNS::text, QCString( localName.latin1() ) + "-body" ).toElement();
    fs->loadOasisContent( bodyElem, context );
}

bool KWTextDocument::loadSpanTag( const QDomElement& tag, KoOasisContext& context,
                                  KoTextParag* parag, uint pos,
                                  QString& textData, KoTextCustomItem* & customItem )
{
    const QString localName( tag.localName() );
    const bool isTextNS = tag.namespaceURI() == KoXmlNS::text;
    kdDebug(32500) << "KWTextDocument::loadSpanTag: " << localName << endl;

    if ( isTextNS )
    {
        if ( localName == "a" )
        {
            QString href( tag.attributeNS( KoXmlNS::xlink, "href", QString::null) );
            if ( href.startsWith("#") )
            {
                context.styleStack().save();
                // We have a reference to a bookmark (### TODO)
                // As we do not support it now, treat it as a <span> without formatting
                bool leadingSpace = false;
                parag->loadOasisSpan( tag, context, pos, leadingSpace ); // recurse
                context.styleStack().restore();
            }
            else
            {
                // The text is contained in a <span> inside the <a> element. In theory
                // we could have multiple spans there, but OO ensures that there is always only one,
                // splitting the hyperlink if necessary (at format changes).
                // Note that we ignore the formatting of the span.
                QDomElement spanElem = KoDom::namedItemNS( tag, KoXmlNS::text, "span" );
                QString text;
                if( spanElem.isNull() )
                    text = tag.text();
                else {
                    // The save/restore of the stack is done by the caller (KoTextParag::loadOasisSpan)
                    // This allows to use the span's format for the variable.
                    //kdDebug(32500) << "filling stack with " << spanElem.attributeNS( KoXmlNS::text, "style-name", QString::null ) << endl;
                    context.fillStyleStack( spanElem, KoXmlNS::text, "style-name", "text" );
                    text = spanElem.text();
                }
                textData = KoTextObject::customItemChar(); // hyperlink placeholder
                // unused tag.attributeNS( KoXmlNS::office, "name", QString::null )
                KoVariableCollection& coll = context.variableCollection();
                customItem = new KoLinkVariable( this, text, href,
                                                 coll.formatCollection()->format( "STRING" ),
                                                 &coll );
            }
            return true;
        }
        else if ( localName == "bookmark" )
        {
            appendBookmark( parag, pos, parag, pos, tag.attributeNS( KoXmlNS::text, "name", QString::null ) );
            return true;
        }
        else if ( localName == "bookmark-start" ) {
            KWLoadingInfo* loadingInfo = m_textfs->kWordDocument()->loadingInfo();
            loadingInfo->m_bookmarkStarts.insert( tag.attributeNS( KoXmlNS::text, "name", QString::null ),
                                                  KWLoadingInfo::BookmarkStart( this, parag, pos ) );
            return true;
        }
        else if ( localName == "bookmark-end" ) {
            KWLoadingInfo* loadingInfo = m_textfs->kWordDocument()->loadingInfo();
            QString bkName = tag.attributeNS( KoXmlNS::text, "name", QString::null );
            KWLoadingInfo::BookmarkStartsMap::iterator it = loadingInfo->m_bookmarkStarts.find( bkName );
            if ( it == loadingInfo->m_bookmarkStarts.end() ) { // bookmark end without start. This seems to happen..
                // insert simple bookmark then
                appendBookmark( parag, pos, parag, pos, tag.attributeNS( KoXmlNS::text, "name", QString::null ) );
            } else {
                if ( (*it).doc != this ) {
                    // Oh tell me this never happens...
                    kdWarning(32500) << "Cross-frameset bookmark! Not supported." << endl;
                } else {
                    appendBookmark( (*it).parag, (*it).pos, parag, pos, it.key() );
                }
                loadingInfo->m_bookmarkStarts.remove( it );
            }
            return true;
        }
        else if ( localName == "footnote" || localName == "endnote" )
        {
            textData = KoTextObject::customItemChar(); // anchor placeholder
            loadOasisFootnote( tag, context, customItem );
            return true;
        }
    }
    else // not in the "text" namespace
    {
        if ( tag.namespaceURI() == KoXmlNS::draw && localName == "frame" )
        {
            if ( tag.attributeNS( KoXmlNS::koffice, "is-wrapper-frame", QString::null )
                 == "true" )
            {
                QDomElement textbox = KoDom::namedItemNS( tag, KoXmlNS::draw, "text-box" );
                if ( !textbox.isNull() )
                {
                    int numberOfElements = 0;
                    QDomElement elem;
                    QDomElement firstElem;
                    forEachElement( elem, textbox )
                    {
                        ++numberOfElements;
                        firstElem = elem;
                    }
                    if ( numberOfElements == 1 ) // if someone added more stuff, keep the wrapper frame
                    {
                        kdDebug(32001) << "Wrapper frame removed, loading " << firstElem.tagName() << " directly" << endl;
                        // load the only child, e.g. table:table
                        return loadSpanTag( firstElem, context, parag, pos, textData, customItem );
                    }
                }
                return true;
            }

            KWDocument* doc = m_textfs->kWordDocument();
            KWOasisLoader loader( doc );
            KWFrame* frame = loader.loadFrame( tag, context, KoPoint() );
            if ( frame )
            {
                KWFrameSet* fs = frame->frameSet();
                // Hmm, if this is a continuation frame of a non-inline frameset,
                // it's going to inline the whole frameset...
                // ###### In fact this shows we should inline frames, not framesets, in KWord (!!!!) (big TODO)
                // ## well, for tables it's the whole frameset.
                textData = KoTextObject::customItemChar();
                fs->setAnchorFrameset( m_textfs );
                customItem = fs->createAnchor( m_textfs->textDocument(), 0 /*frame number; TODO somehow*/ );
            }
            return true;
        }
        // anchored-as-char table. Not supported by OASIS directly, but we end up
        // calling this when removing the wrapper frame above.
        else if ( tag.namespaceURI() == KoXmlNS::table && localName == "table" )
        {
            KWDocument* doc = m_textfs->kWordDocument();
            KWOasisLoader loader( doc );
            KWTableFrameSet* table = loader.loadOasisTable( tag, context );
            table->finalize();
            textData = KoTextObject::customItemChar();
            table->setAnchorFrameset( m_textfs );
            customItem = table->createAnchor( m_textfs->textDocument(), 0 /*frame number*/ );
            return true;
        }
    }
    return false;
}

#include "KWTextDocument.moc"
