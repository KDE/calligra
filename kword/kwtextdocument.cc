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

#include "kwtextdocument.h"
#include "kwtextparag.h"
#include "kwdoc.h"
#include "kwtextframeset.h"
#include <kdebug.h>
#include <kglobalsettings.h>
#include "kwcommand.h"
#include <klocale.h>

KWTextDocument::KWTextDocument( KWTextFrameSet * textfs, KoTextFormatCollection *fc, KoTextFormatter *formatter )
    : KoTextDocument( textfs->kWordDocument(), fc, formatter, false ), m_textfs( textfs )
{
    init();
}

KWTextDocument::KWTextDocument( KoZoomHandler * zoomHandler )
    : KoTextDocument( zoomHandler, new KoTextFormatCollection( KGlobalSettings::generalFont() /*unused*/, QColor(), KGlobal::locale()->language() , false, 1.0), 0L, false ),
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
    //kdDebug()<<" KoTextDocument::deleteTextCommand************\n";
    return new KWTextDeleteCommand( textdoc, id, index, str, customItemsMap, oldParagLayouts );
}

bool KWTextDocument::loadSpanTag( const QDomElement& tag, KoOasisContext& context,
                                  KoTextParag* parag, int pos,
                                  QString& textData, KoTextCustomItem* & /*customItem*/ )
{
    const QString tagName( tag.tagName() );
    const bool textFoo = tagName.startsWith( "text:" );
    kdDebug() << k_funcinfo << tagName << endl;

    if ( textFoo )
    {
        // TODO
    }
    else // non "text:" tags
    {
        if ( tagName == "draw:image" )
        {
            KWFrameSet* fs = new KWPictureFrameSet( m_textfs->kWordDocument(), tag, context );
            m_textfs->kWordDocument()->addFrameSet( fs, false );
            fs->setAnchored( m_textfs, parag, pos, false /*no placeholder yet*/, false /*don't repaint yet*/ );
            return true;
        }
        if ( tagName == "draw:text-box" )
        {
            textData = '#'; // anchor placeholder
            // TODO new KWTextFrameSet
            // TODO anchorFrameset( frameName, parag, pos );
            return false;
        }
    }

#if 0 // TODO
    if ( textFoo &&
         ( tagName == "text:footnote" || tagName == "text:endnote" ) )
        {
            textData = '#'; // anchor placeholder
            importFootnote( doc, ts, outputFormats, pos, tagName );
            // do me last, combination of variable and frameset.
        }
        else if ( textFoo && tagName == "text:a" )
        {
            m_styleStack.save();
            QString href( ts.attribute("xlink:href") );
            if ( href.startsWith("#") )
            {
                // We have a reference to a bookmark (### TODO)
                // As we do not support it now, treat it as a <text:span> without formatting
                parseSpanOrSimilar( doc, ts, outputParagraph, outputFormats, paragraphText, pos);
            }
            else
            {
                // The problem is that KWord's hyperlink text is not inside the normal text, but for OOWriter it is nearly a <text:span>
                // So we have to fake.
                QDomElement fakeParagraph, fakeFormats;
                uint fakePos=0;
                QString text;
                parseSpanOrSimilar( doc, ts, fakeParagraph, fakeFormats, text, fakePos);
                textData = '#'; // hyperlink placeholder
                QDomElement linkElement (doc.createElement("LINK"));
                linkElement.setAttribute("hrefName",ts.attribute("xlink:href"));
                linkElement.setAttribute("linkName",text);
                appendKWordVariable(doc, outputFormats, ts, pos, "STRING", 9, linkElement);
            }
            m_styleStack.restore();
        }
        else if ( textFoo && tagName == "text:bookmark" )
        {
            // the number of <PARAGRAPH> tags in the frameset element is the parag id
            // (-1 for starting at 0, +1 since not written yet)
            Q_ASSERT( !m_currentFrameset.isNull() );
            appendBookmark( doc, numberOfParagraphs( m_currentFrameset ),
                            pos, ts.attribute( "text:name" ) );
        }
        else if ( textFoo && tagName == "text:bookmark-start" ) {
            m_bookmarkStarts.insert( ts.attribute( "text:name" ),
                                     BookmarkStart( m_currentFrameset.attribute( "name" ),
                                                    numberOfParagraphs( m_currentFrameset ),
                                                    pos ) );
        }
        else if ( textFoo && tagName == "text:bookmark-end" ) {
            QString bkName = ts.attribute( "text:name" );
            BookmarkStartsMap::iterator it = m_bookmarkStarts.find( bkName );
            if ( it == m_bookmarkStarts.end() ) { // bookmark end without start. This seems to happen..
                // insert simple bookmark then
                appendBookmark( doc, numberOfParagraphs( m_currentFrameset ),
                                pos, ts.attribute( "text:name" ) );
            } else {
                if ( (*it).frameSetName != m_currentFrameset.attribute( "name" ) ) {
                    // Oh tell me this never happens...
                    kdWarning(32500) << "Cross-frameset bookmark! Not supported." << endl;
                } else {
                    appendBookmark( doc, (*it).paragId, (*it).pos,
                                    numberOfParagraphs( m_currentFrameset ), pos, it.key() );
                }
                m_bookmarkStarts.remove( it );
            }
        }
#endif
    return false;
}

#include "kwtextdocument.moc"
