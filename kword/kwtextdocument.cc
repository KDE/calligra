/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwcommand.h"
#include "kwdoc.h"
#include "kwtextdocument.h"
#include "kwtextframeset.h"
#include "kwtextparag.h"
#include "kwloadinginfo.h"
#include "kwvariable.h"
#include "kwanchor.h"

#include <kooasiscontext.h>

#include <kdebug.h>
#include <kglobalsettings.h>
#include <klocale.h>

KWTextDocument::KWTextDocument( KWTextFrameSet * textfs, KoTextFormatCollection *fc, KoTextFormatter *formatter )
    : KoTextDocument( textfs->kWordDocument(), fc, formatter, false ), m_textfs( textfs )
{
    init();
}

KWTextDocument::KWTextDocument( KoZoomHandler * zoomHandler )
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

    //QDomElement tocSource = toc.namedItem( "text:table-of-content-source" );
    // TODO parse templates and generate "Contents ..." styles from it
    //for ( QDomNode n(tocSource.firstChild()); !text.isNull(); text = text.nextSibling() )
    //{
    //}

    QDomElement tocIndexBody = tag.namedItem( "text:index-body" ).toElement();
    for ( QDomNode n(tocIndexBody.firstChild()); !n.isNull(); n = n.nextSibling() )
    {
        context.styleStack().save();
        QDomElement t = n.toElement();
        QString tagName = t.tagName();
        QDomElement e;
        if ( tagName == "text:index-title" ) {
            lastParagraph = loadOasisText( t, context, lastParagraph, styleColl, nextParagraph ); // recurse again
        } else if ( tagName == "text:p" ) {
            context.fillStyleStack( t, "text:style-name" );
            lastParagraph = createParag( this, lastParagraph, nextParagraph );
            uint pos = 0;
            lastParagraph->loadOasis( t, context, styleColl, pos );
        } else
            kdWarning() << "OASIS TOC loading: unknown tag " << tagName << " found in text:index-body" << endl;
        context.styleStack().restore();
    }

    m_textfs->kWordDocument()->hasTOC( true );
}

KWFrame* KWTextDocument::loadFrame( const QDomElement& tag, KoOasisContext& context )
{
    for ( QDomNode child = tag.firstChild(); !child.isNull(); child = child.nextSibling() ) {
        QDomElement elem = child.toElement();
        QCString tagName = elem.tagName().latin1();
        if ( tagName == "draw:text-box" )
        {
            kdDebug()<<" append text-box\n";
            return m_textfs->loadOasisTextBox( tag, elem, context );
        }
        else if ( tagName == "draw:image" )
        {
            KWFrameSet* fs = new KWPictureFrameSet( m_textfs->kWordDocument(), tag, elem, context );
            m_textfs->kWordDocument()->addFrameSet( fs, false );
            return fs->frame(0);
        }
    }
    return 0;
}

bool KWTextDocument::loadOasisBodyTag( const QDomElement& tag, KoOasisContext& context,
                                       KoTextParag* & lastParagraph, KoStyleCollection* styleColl,
                                       KoTextParag* nextParagraph )
{
    QCString tagName( tag.tagName().latin1() );
    if ( tagName == "draw:frame" )
    {
        KWFrame* frame = loadFrame( tag, context );
        if ( frame )
            return true;
    }
#if 0 // TODO OASIS table:table
    else if ( tagName == "table:table" )
    {
        //todo
        parseTable(tag, currentFramesetElement);
        kdDebug(32002) << "Table found!" << endl;
        return true;
    }
#endif
    else if ( tagName == "text:table-of-content" )
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
    m_textfs->kWordDocument()->insertBookMark( name, static_cast<KWTextParag *>( parag ),
                                               static_cast<KWTextParag *>( endParag ),
                                               m_textfs, pos, endPos );
}

void KWTextDocument::loadOasisFootnote( const QDomElement& tag, KoOasisContext& context,
                                        KoTextCustomItem* & customItem )
{
    const QString frameName( tag.attribute("text:id") );
    const QString tagName( tag.tagName() );
    QDomElement citationElem = tag.namedItem( tagName + "-citation" ).toElement();

    bool endnote = tagName == "text:endnote";

    QString label = citationElem.attribute( "text:label" );
    bool autoNumbered = label.isEmpty();

    KWFootNoteFrameSet *fs = m_textfs->insertFootNote(
        endnote ? EndNote : FootNote,
        autoNumbered ? KWFootNoteVariable::Auto : KWFootNoteVariable::Manual,
        label );
    customItem = fs->footNoteVariable();

    fs->createInitialFrame( 0 ); // we don't know the page number...

    // Parse contents into the frameset
    QDomElement bodyElem = tag.namedItem( tagName + "-body" ).toElement();
    fs->loadOasisContent( bodyElem, context );
}

bool KWTextDocument::loadSpanTag( const QDomElement& tag, KoOasisContext& context,
                                  KoTextParag* parag, uint pos,
                                  QString& textData, KoTextCustomItem* & customItem )
{
    const QString tagName( tag.tagName() );
    const bool textFoo = tagName.startsWith( "text:" );
    kdDebug(32500) << "KWTextDocument::loadSpanTag: " << tagName << endl;

    if ( textFoo )
    {
        if ( tagName == "text:a" )
        {
            QString href( tag.attribute("xlink:href") );
            if ( href.startsWith("#") )
            {
                context.styleStack().save();
                // We have a reference to a bookmark (### TODO)
                // As we do not support it now, treat it as a <text:span> without formatting
                parag->loadOasisSpan( tag, context, pos ); // recurse
                context.styleStack().restore();
            }
            else
            {
                // The text is contained in a text:span inside the text:a element. In theory
                // we could have multiple spans there, but OO ensures that there is always only one,
                // splitting the hyperlink if necessary (at format changes).
                // Note that we ignore the formatting of the span.
                QDomElement spanElem = tag.namedItem( "text:span" ).toElement();
                QString text;
                if( spanElem.isNull() )
                    text = tag.text();
                else {
                    // The save/restore of the stack is done by the caller (KoTextParag::loadOasisSpan)
                    // This allows to use the span's format for the variable.
                    //kdDebug(32500) << "filling stack with " << spanElem.attribute( "text:style-name" ) << endl;
                    context.fillStyleStack( spanElem, "text:style-name" );
                    text = spanElem.text();
                }
                textData = KoTextObject::customItemChar(); // hyperlink placeholder
                // unused tag.attribute( "office:name" )
                KoVariableCollection& coll = context.variableCollection();
                customItem = new KoLinkVariable( this, text, href,
                                                 coll.formatCollection()->format( "STRING" ),
                                                 &coll );
            }
            return true;
        }
        else if ( tagName == "text:bookmark" ) // this is an empty element
        {
            // the number of <PARAGRAPH> tags in the frameset element is the parag id
            // (-1 for starting at 0, +1 since not written yet)
            appendBookmark( parag, pos, parag, pos, tag.attribute( "text:name" ) );
            return true;
        }
        else if ( tagName == "text:bookmark-start" ) {
            KWLoadingInfo* loadingInfo = m_textfs->kWordDocument()->loadingInfo();
            loadingInfo->m_bookmarkStarts.insert( tag.attribute( "text:name" ),
                                                  KWLoadingInfo::BookmarkStart( this, parag, pos ) );
            return true;
        }
        else if ( textFoo && tagName == "text:bookmark-end" ) {
            KWLoadingInfo* loadingInfo = m_textfs->kWordDocument()->loadingInfo();
            QString bkName = tag.attribute( "text:name" );
            KWLoadingInfo::BookmarkStartsMap::iterator it = loadingInfo->m_bookmarkStarts.find( bkName );
            if ( it == loadingInfo->m_bookmarkStarts.end() ) { // bookmark end without start. This seems to happen..
                // insert simple bookmark then
                appendBookmark( parag, pos, parag, pos, tag.attribute( "text:name" ) );
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
        else if ( tagName == "text:footnote" || tagName == "text:endnote" )
        {
            textData = KoTextObject::customItemChar(); // anchor placeholder
            loadOasisFootnote( tag, context, customItem );
            return true;
        }
    }
    else // non "text:" tags
    {
        if ( tagName == "draw:frame" )
        {
            KWFrame* frame = loadFrame( tag, context );
            if ( frame )
            {
                KWFrameSet* fs = frame->frameSet();
                // Hmm, if this is a continuation frame of a non-inline frameset,
                // it's going to inline the whole frameset...
                // ###### In fact this shows we should inline frames, not framesets, in KWord (!!!!) (big TODO)
                textData = KoTextObject::customItemChar();
                fs->setAnchorFrameset( m_textfs );
                customItem = fs->createAnchor( m_textfs->textDocument(), 0 /*frame number; TODO somehow*/ );

                // bad way: doing it all by hand. Doesn't work, pos is no reference(!)
                //parag->insert( index, KoTextObject::customItemChar() );
                //fs->setAnchored( m_textfs, parag, pos, false /*no placeholder yet*/, false /*don't repaint yet*/ );
                //++pos;
            }
            return true;
        }
    }
    return false;
}

#include "kwtextdocument.moc"
