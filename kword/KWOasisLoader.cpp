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

#include "KWOasisLoader.h"
#include "KWDocument.h"
#include "KWTextDocument.h"
#include "KWTextFrameSet.h"
#include "KWFrameSet.h"
#include "KWPictureFrameSet.h"
#include "KWLoadingInfo.h"
#include "KWTableFrameSet.h"
#include "KWPartFrameSet.h"
#include "KWPageManager.h"

#include <KoStore.h>
#include <KoOasisContext.h>
#include <KoOasisStyles.h>
#include <KoOasisStore.h>
#include <KoOasisSettings.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <KoXmlNS.h>

#include <kdebug.h>
#include <klocale.h>

#include <q3valuelist.h>
#include <qdom.h>

KWOasisLoader::KWOasisLoader( KWDocument* doc )
    : m_doc( doc )
{
}

// Warning, this method has no undo/redo support, it is *called* by the undo/redo commands.
// cursor is set when pasting into a textframesetedit (kwcommand), 0 otherwise.
QList<KWFrame *> KWOasisLoader::insertOasisData( KoStore* store, KoTextCursor* cursor )
{
    QList<KWFrame *> frames;
    if ( store->bad() || !store->hasFile( "content.xml" ) )
    {
        kError(32001) << "Invalid ZIP store in memory" << endl;
        if ( !store->hasFile( "content.xml" ) )
            kError(32001) << "No content.xml file" << endl;
        return frames;
    }
    store->disallowNameExpansion();

    KoOasisStore oasisStore( store );
    QDomDocument contentDoc;
    QString errorMessage;
    bool ok = oasisStore.loadAndParse( "content.xml", contentDoc, errorMessage );
    if ( !ok ) {
        kError(32001) << "Error parsing content.xml: " << errorMessage << endl;
        return frames;
    }

    KoOasisStyles oasisStyles;
    QDomDocument stylesDoc;
    (void)oasisStore.loadAndParse( "styles.xml", stylesDoc, errorMessage );
    // Load styles from style.xml
    oasisStyles.createStyleMap( stylesDoc, true );
    // Also load styles from content.xml
    oasisStyles.createStyleMap( contentDoc, false );

    m_doc->createLoadingInfo();

    QDomElement content = contentDoc.documentElement();

    QDomElement body( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( body.isNull() ) {
        kError(32001) << "No office:body found!" << endl;
        return frames;
    }
    // We then want to use whichever element is the child of <office:body>,
    // whether that's <office:text> or <office:presentation> or whatever.
    QDomElement iter, realBody;
    forEachElement( iter, body ) {
        realBody = iter;
    }
    if ( realBody.isNull() ) {
        kError(32001) << "No element found inside office:body!" << endl;
        return frames;
    }

    KoOasisContext context( m_doc, *m_doc->variableCollection(), oasisStyles, store );
    if ( cursor )
    {
        KWTextDocument * textdoc = static_cast<KWTextDocument *>(cursor->parag()->document());
        KWTextFrameSet * textFs = textdoc->textFrameSet();

        *cursor = textFs->textObject()->pasteOasisText( realBody, context, *cursor, m_doc->styleCollection() );

        textFs->textObject()->setNeedSpellCheck( true );
    }
    else // No cursor available, load only the frames
    {
        // The main loop from KoTextDocument::loadOasisText but for frames only
        // (can't paste text if there is no text-frameset being edited, where would it go?)
        QDomElement tag;
        forEachElement( tag, realBody )
        {
            context.styleStack().save();
            const QString bodyTagLocalName = tag.localName();
            kDebug() << k_funcinfo << bodyTagLocalName << endl;
            if ( bodyTagLocalName == "frame" && tag.namespaceURI() == KoXmlNS::draw )
            {
                KWFrame * frame = loadFrame( tag, context, KoPoint( 10, 10 ) /*offset pasted object*/ );
                if ( frame )
                {
                    frames.append( frame );
                }
            }
#if 0 // TODO OASIS table:table
            else if ( bodyTagLocalName == "table" && tag.namespaceURI() == KoXmlNS::table )
                ;
#endif
        }
    }

    //kDebug() << "KWOasisLoader::execute calling doc->completePasting" << endl;
    m_doc->completeOasisPasting();
    m_doc->deleteLoadingInfo();
    return frames;
}

void KWOasisLoader::loadOasisSettings( const QDomDocument& settingsDoc )
{
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    if ( !viewSettings.isNull() )
    {
        m_doc->setUnit( KoUnit::unit(viewSettings.parseConfigItemString("unit")) );
    }
    loadOasisIgnoreList( settings );
    m_doc->variableCollection()->variableSetting()->loadOasis( settings );
}

static QString headerTypeToFramesetName( const QString& localName, bool hasEvenOdd )
{
    if ( localName == "header" )
        return hasEvenOdd ? i18n("Odd Pages Header") : i18n( "Header" );
    if ( localName == "header-left" )
        return i18n("Even Pages Header");
    if ( localName == "footer" )
        return hasEvenOdd ? i18n("Odd Pages Footer") : i18n( "Footer" );
    if ( localName == "footer-left" )
        return i18n("Even Pages Footer");
    if ( localName == "header-first" ) // NOT OASIS COMPLIANT
        return i18n("First Page Header");
    if ( localName == "footer-first" ) // NOT OASIS COMPLIANT
        return i18n("First Page Footer");
    kWarning(32001) << "Unknown tag in headerTypeToFramesetName: " << localName << endl;
    return QString::null;
}

static KWFrameSet::Info headerTypeToFrameInfo( const QString& localName, bool /*hasEvenOdd*/ )
{
    if ( localName == "header" )
        return KWFrameSet::FI_ODD_HEADER;
    if ( localName == "header-left" )
        return KWFrameSet::FI_EVEN_HEADER;
    if ( localName == "footer" )
        return KWFrameSet::FI_ODD_FOOTER;
    if ( localName == "footer-left" )
        return KWFrameSet::FI_EVEN_FOOTER;

    // ######## KWord extension, because I'm too lazy.
    // TODO: the real solution is a separate page layout for the first page.
    if ( localName == "header-first" ) // NOT OASIS COMPLIANT
        return KWFrameSet::FI_FIRST_HEADER;
    if ( localName == "footer-first" ) // NOT OASIS COMPLIANT
        return KWFrameSet::FI_FIRST_FOOTER;
    return KWFrameSet::FI_BODY;
}

void KWOasisLoader::loadOasisHeaderFooter( const QDomElement& headerFooter, bool hasEvenOdd, QDomElement& style, KoOasisContext& context )
{
    const QString localName = headerFooter.localName();
    bool isHeader = localName.startsWith( "header" );

    KWTextFrameSet *fs = new KWTextFrameSet( m_doc, headerTypeToFramesetName( localName, hasEvenOdd ) );
    fs->setFrameSetInfo( headerTypeToFrameInfo( localName, hasEvenOdd ) );
    m_doc->addFrameSet( fs, false );

    if ( !style.isNull() )
        context.styleStack().push( style );
    KWFrame* frame = new KWFrame( fs, 29, isHeader?0:567, 798-29, 41 );
    frame->loadCommonOasisProperties( context, fs, "header-footer" );
    const QString minHeight = context.styleStack().attributeNS( KoXmlNS::fo, "min-height" );
    if ( !minHeight.isEmpty() )
        frame->setMinimumFrameHeight( KoUnit::parseValue( minHeight ) );

    frame->setFrameBehavior( KWFrame::AutoExtendFrame );
    frame->setNewFrameBehavior( KWFrame::Copy );
    fs->addFrame( frame );
    if ( !style.isNull() )
        context.styleStack().pop(); // don't let it be active when parsing the text

    context.setUseStylesAutoStyles( true ); // use auto-styles from styles.xml, not those from content.xml
    fs->loadOasisContent( headerFooter, context );
    context.setUseStylesAutoStyles( false );

    if ( isHeader )
        m_doc->m_headerVisible = true;
    else
        m_doc->m_footerVisible = true;
}

void KWOasisLoader::loadOasisIgnoreList( const KoOasisSettings& settings )
{
    KoOasisSettings::Items configurationSettings = settings.itemSet( "configuration-settings" );
    if ( !configurationSettings.isNull() )
    {
        const QString ignorelist = configurationSettings.parseConfigItemString( "SpellCheckerIgnoreList" );
        kDebug()<<" ignorelist :"<<ignorelist<<endl;
        m_doc->setSpellCheckIgnoreList( QStringList::split( ',', ignorelist ) );
    }
}

KWFrame* KWOasisLoader::loadFrame( const QDomElement& frameTag, KoOasisContext& context, const KoPoint& offset )
{
    KWFrame* frame = 0;
    QDomElement elem;
    forEachElement( elem, frameTag )
    {
        if ( elem.namespaceURI() != KoXmlNS::draw )
            continue;
        const QString localName = elem.localName();
        if ( localName == "text-box" )
        {
            //kDebug()<<" append text-box\n";
            frame = loadOasisTextBox( frameTag, elem, context );
            break;
        }
        else if ( localName == "image" )
        {
            KWFrameSet* fs = new KWPictureFrameSet( m_doc, frameTag, elem, context );
            m_doc->addFrameSet( fs, false );
            frame = fs->frame(0);
            break;
        } else if ( localName == "object" )
        {
            KWPartFrameSet* fs = new KWPartFrameSet( m_doc, frameTag, elem, context );
            m_doc->addFrameSet( fs, false );
            frame = fs->frame(0);
            break;
        }
    }
    if ( frame ) {
        const QString anchorType = frameTag.attributeNS( KoXmlNS::text, "anchor-type", QString::null );
        if ( anchorType == "page" ) {
            double x = KoUnit::parseValue( frameTag.attributeNS( KoXmlNS::svg, "x", QString::null ) );
            double y = KoUnit::parseValue( frameTag.attributeNS( KoXmlNS::svg, "y", QString::null ) );
            int pageNum = frameTag.attributeNS( KoXmlNS::text, "anchor-page-number", QString::null ).toInt();
            // Ensure that we have enough pages
            KWPageManager* pageManager = m_doc->pageManager();
            while ( pageNum > pageManager->lastPageNumber() )
                pageManager->appendPage();
            frame->moveTopLeft( KoPoint( x, y + pageManager->topOfPage(pageNum) ) );
        }
        frame->moveBy( offset.x(), offset.y() );
    }
    return frame;
}

KWFrame* KWOasisLoader::loadOasisTextBox( const QDomElement& frameTag, const QDomElement& tag,
                                          KoOasisContext& context )
{
    // Text frame chains. When seeing frame 'B' is chained to this frame A when loading,
    // we store 'B' -> A, so that when loading B we can add it to A's frameset.
    // If we load B first, no problem: when loading A we can chain.
    // This is all made simpler by the fact that we don't have manually configurable order in KWord...
    // But it's made more complex by the fact that frames don't have names in KWord (framesets do).
    // Hence the framename temporary storage in KWLoadingInfo

    KWLoadingInfo* loadingInfo = m_doc->loadingInfo();
    KWTextFrameSet* fs = 0;
    QString frameName = frameTag.attributeNS( KoXmlNS::draw, "name", QString::null );
    QString chainNextName = tag.attributeNS( KoXmlNS::draw, "chain-next-name", QString::null );
    if ( !chainNextName.isEmpty() && loadingInfo ) { // 'B' in the above example
        kDebug(32001) << "Loading " << frameName << " : next-in-chain=" << chainNextName << endl;
        // Check if we already loaded that frame (then we need to go 'before' it)
        KWFrame* nextFrame = loadingInfo->frameByName( chainNextName );
        if ( nextFrame ) {
            fs = dynamic_cast<KWTextFrameSet *>( nextFrame->frameSet() );
            chainNextName = QString::null; // already found, no need to store it
            kDebug(32001) << "  found " << nextFrame << " -> frameset " << ( fs ? fs->name() : QString::null ) << endl;
        }
    }
    KWFrame* prevFrame = loadingInfo->chainPrevFrame( frameName );
    //kDebug(32001) << "Loading " << frameName << " : chainPrevFrame=" << prevFrame << endl;
    if ( prevFrame ) {
        if ( fs ) // we are between prevFrame and nextFrame. They'd better be for the same fs!!
            Q_ASSERT( fs == prevFrame->frameSet() );
        fs = dynamic_cast<KWTextFrameSet *>( prevFrame->frameSet() );
        //kDebug(32001) << "  found " << prevFrame << " -> frameset " << fs->name() << endl;
    }
    KWFrame* frame = 0;
    if ( !fs ) {
        fs = new KWTextFrameSet( m_doc, frameTag, context );
        m_doc->addFrameSet( fs, false );
        frame = fs->loadOasis( frameTag, tag, context );
    } else { // Adding frame to existing frameset
        context.styleStack().save();
        context.fillStyleStack( frameTag, KoXmlNS::draw, "style-name", "graphic" ); // get the style for the graphics element
        frame = fs->loadOasisTextFrame( frameTag, tag, context );
        context.styleStack().restore();
    }

    loadingInfo->storeFrameName( frame, frameName );

    if ( !chainNextName.isEmpty() ) {
        loadingInfo->storeNextFrame( frame, chainNextName );
    }
    return frame;
}

KWTableFrameSet* KWOasisLoader::loadOasisTable( const QDomElement& tag,
                                                KoOasisContext& context )
{
    const QString name = tag.attributeNS( KoXmlNS::table, "name", i18n( "Unnamed Table" ) ); // ### check for unicity?
    KWTableFrameSet* table = new KWTableFrameSet( m_doc, name );
    table->loadOasis( tag, context );
    m_doc->addFrameSet(table, false);
    return table;
}
