#include "kwoasisloader.h"
#include "kwdoc.h"
#include "kwtextdocument.h"
#include "kwtextframeset.h"
#include "kwframe.h"
#include "kwloadinginfo.h"
#include "kwtableframeset.h"

#include <koStore.h>
#include <kooasiscontext.h>
#include <koOasisStyles.h>
#include <koOasisStore.h>
#include <koOasisSettings.h>
#include <koxmlns.h>
#include <kodom.h>
#include <koxmlns.h>

#include <kdebug.h>
#include <klocale.h>

#include <qvaluelist.h>
#include <qdom.h>

KWOasisLoader::KWOasisLoader( KWDocument* doc )
    : m_doc( doc )
{
}

// Warning, this method has no undo/redo support, it is *called* by the undo/redo commands.
// cursor is set when pasting into a textframesetedit (kwcommand), 0 otherwise.
QValueList<KWFrame *> KWOasisLoader::insertOasisData( KoStore* store, KoTextCursor* cursor )
{
    QValueList<KWFrame *> frames;
    if ( store->bad() || !store->hasFile( "content.xml" ) )
    {
        kdError(32001) << "Invalid ZIP store in memory" << endl;
        if ( !store->hasFile( "content.xml" ) )
            kdError(32001) << "No content.xml file" << endl;
        return frames;
    }
    store->disallowNameExpansion();

    KoOasisStore oasisStore( store );
    QDomDocument contentDoc;
    QString errorMessage;
    bool ok = oasisStore.loadAndParse( "content.xml", contentDoc, errorMessage );
    if ( !ok ) {
        kdError(32001) << "Error parsing content.xml: " << errorMessage << endl;
        return frames;
    }

    KoOasisStyles oasisStyles;
    QDomDocument stylesDoc;
    (void)oasisStore.loadAndParse( "styles.xml", stylesDoc, errorMessage );
    // Load styles from style.xml
    oasisStyles.createStyleMap( stylesDoc );
    // Also load styles from content.xml
    oasisStyles.createStyleMap( contentDoc );

    m_doc->createLoadingInfo();

    QDomElement content = contentDoc.documentElement();

    QDomElement body( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( body.isNull() ) {
        kdError(32001) << "No office:body found!" << endl;
        return frames;
    }
    // We then want to use whichever element is the child of <office:body>,
    // whether that's <office:text> or <office:presentation> or whatever.
    QDomElement iter, realBody;
    forEachElement( iter, body ) {
        realBody = iter;
    }
    if ( realBody.isNull() ) {
        kdError(32001) << "No element found inside office:body!" << endl;
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
            kdDebug() << k_funcinfo << bodyTagLocalName << endl;
            if ( bodyTagLocalName == "frame" && tag.namespaceURI() == KoXmlNS::draw )
            {
                // From KWTextDocument::loadFrame:
                QDomElement elem;
                forEachElement( elem, tag )
                {
                    if ( elem.namespaceURI() != KoXmlNS::draw )
                        continue;
                    const QString localName = elem.localName();
                    KWFrame* frame = 0;
                    if ( localName == "text-box" )
                    {
                        frame = loadOasisTextBox( tag, elem, context );
                    }
                    else if ( localName == "image" )
                    {
                        KWFrameSet* fs = new KWPictureFrameSet( m_doc, tag, elem, context );
                        m_doc->addFrameSet( fs, false );
                        frame = fs->frame( 0 );
                        frames.append( frame );
                    }
                    if ( frame )
                        frame->moveBy( 10, 10 );
                }
            }
#if 0 // TODO OASIS table:table
            else if ( bodyTagLocalName == "table" && tag.namespaceURI() == KoXmlNS::table )
                ;
#endif
        }
    }

    //kdDebug() << "KWOasisLoader::execute calling doc->completePasting" << endl;
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

static QString headerTypeToFramesetName( const QString& tagName, bool hasEvenOdd )
{
    if ( tagName == "style:header" )
        return hasEvenOdd ? i18n("Odd Pages Header") : i18n( "Header" );
    if ( tagName == "style:header-left" )
        return i18n("Even Pages Header");
    if ( tagName == "style:footer" )
        return hasEvenOdd ? i18n("Odd Pages Footer") : i18n( "Footer" );
    if ( tagName == "style:footer-left" )
        return i18n("Even Pages Footer");
    kdWarning(32001) << "Unknown tag in headerTypeToFramesetName: " << tagName << endl;
    // ######
    //return i18n("First Page Header");
    //return i18n("First Page Footer");
    return QString::null;
}

static KWFrameSet::Info headerTypeToFrameInfo( const QString& tagName, bool /*hasEvenOdd*/ )
{
    if ( tagName == "style:header" )
        return KWFrameSet::FI_ODD_HEADER;
    if ( tagName == "style:header-left" )
        return KWFrameSet::FI_EVEN_HEADER;
    if ( tagName == "style:footer" )
        return KWFrameSet::FI_ODD_FOOTER;
    if ( tagName == "style:footer-left" )
        return KWFrameSet::FI_EVEN_FOOTER;

    // ### return KWFrameSet::FI_FIRST_HEADER; TODO
    // ### return KWFrameSet::FI_FIRST_FOOTER; TODO
    return KWFrameSet::FI_BODY;
}

void KWOasisLoader::loadOasisHeaderFooter( const QDomElement& headerFooter, bool hasEvenOdd, QDomElement& style, KoOasisContext& context )
{
    const QString tagName = headerFooter.tagName();
    bool isHeader = tagName.startsWith( "style:header" );

    KWTextFrameSet *fs = new KWTextFrameSet( m_doc, headerTypeToFramesetName( tagName, hasEvenOdd ) );
    fs->setFrameSetInfo( headerTypeToFrameInfo( tagName, hasEvenOdd ) );
    m_doc->m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading

    if ( !style.isNull() )
        context.styleStack().push( style );
    KWFrame* frame = new KWFrame( fs, 29, isHeader?0:567, 798-29, 41 );
    frame->loadCommonOasisProperties( context, fs );
    frame->setFrameBehavior( KWFrame::AutoExtendFrame );
    frame->setNewFrameBehavior( KWFrame::Copy );
    fs->addFrame( frame );
    if ( !style.isNull() )
        context.styleStack().pop(); // don't let it be active when parsing the text

    fs->loadOasisContent( headerFooter, context );

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
        kdDebug()<<" ignorelist :"<<ignorelist<<endl;
        m_doc->setSpellCheckIgnoreList( QStringList::split( ',', ignorelist ) );
    }
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
        kdDebug(32001) << "Loading " << frameName << " : next-in-chain=" << chainNextName << endl;
        // Check if we already loaded that frame (then we need to go 'before' it)
        KWFrame* nextFrame = loadingInfo->frameByName( chainNextName );
        if ( nextFrame ) {
            fs = dynamic_cast<KWTextFrameSet *>( nextFrame->frameSet() );
            chainNextName = QString::null; // already found, no need to store it
            kdDebug(32001) << "  found " << nextFrame << " -> frameset " << ( fs ? fs->getName() : QString::null ) << endl;
        }
    }
    KWFrame* prevFrame = loadingInfo->chainPrevFrame( frameName );
    //kdDebug(32001) << "Loading " << frameName << " : chainPrevFrame=" << prevFrame << endl;
    if ( prevFrame ) {
        if ( fs ) // we are between prevFrame and nextFrame. They'd better be for the same fs!!
            Q_ASSERT( fs == prevFrame->frameSet() );
        fs = dynamic_cast<KWTextFrameSet *>( prevFrame->frameSet() );
        //kdDebug(32001) << "  found " << prevFrame << " -> frameset " << fs->getName() << endl;
    }
    KWFrame* frame = 0;
    if ( !fs ) {
        fs = new KWTextFrameSet( m_doc, frameTag, context );
        m_doc->addFrameSet( fs, false );
        frame = fs->loadOasis( frameTag, tag, context );
    } else { // Adding frame to existing frameset
        context.styleStack().save();
        context.fillStyleStack( frameTag, KoXmlNS::draw, "style-name" ); // get the style for the graphics element
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
    m_doc->m_lstFrameSet.append( table );
    table->loadOasis( tag, context );
    return table;
}
