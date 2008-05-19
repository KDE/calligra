/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWOdfLoader.h"
#include "KWOdfSharedLoadingData.h"
#include "KWDocument.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWTextFrame.h"
#include "frames/KWFrame.h"

// koffice
#include <KoOdfStylesReader.h>
#include <KoOasisSettings.h>
#include <KoOdfReadStore.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoImageData.h>
#include <KoTextShapeData.h>
#include <KoTextAnchor.h>
#include <KoShapeContainer.h>
#include <KoTextDocumentLayout.h>
#include <KoShapeLoadingContext.h>
#include <KoInlineTextObjectManager.h>
//#include <KoTextFrameLoader.h>
#include <KWPage.h>
#include <KoProperties.h>
#include <KoStyleManager.h>
#include <KoOdfLoadingContext.h>
//#include <TextShape.h>

// KDE + Qt includes
#include <QTextCursor>
#include <klocale.h>
#include <kdebug.h>
#include <QTextBlock>

/// \internal d-pointer class.
class KWOdfLoader::Private
{
    public:
        /// The KWord document.
        QPointer<KWDocument> document;
        /// Current master-page name (OASIS loading)
        QString currentMasterPage;
        /// Current KWFrameSet name.
        KWTextFrame *currentFrame;

        /// helper function to create a KWTextFrameSet+KWTextFrame for a header/footer.
        void loadHeaderFooterFrame(KoOdfLoadingContext& context, const KoXmlElement& elem, KWord::HeaderFooterType hfType, KWord::TextFrameSetType fsType);
};

KWOdfLoader::KWOdfLoader(KWDocument *document)
: QObject()
, d(new Private())
{
    d->document = document;
    d->currentFrame = 0;
    connect(this, SIGNAL(sigProgress(int)), d->document, SIGNAL(sigProgress(int)));
}

KWOdfLoader::~KWOdfLoader() {
    delete d;
}

KWDocument* KWOdfLoader::document() const { return d->document; }
KWPageManager* KWOdfLoader::pageManager() { return & d->document->m_pageManager; }
QString KWOdfLoader::currentMasterPage() const { return d->currentMasterPage; }
QString KWOdfLoader::currentFramesetName() const { return d->currentFrame ? d->currentFrame->frameSet()->name() : QString(); }
KWTextFrame* KWOdfLoader::currentFrame() const { return d->currentFrame; }

//1.6: KWDocument::loadOasis
bool KWOdfLoader::load( KoOdfReadStore & odfStore )
{
    emit sigProgress( 0 );
    kDebug(32001) <<"========================> KWOdfLoader::load START";

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody ( KoXml::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() ) {
        kError(32001) << "No office:body found!" << endl;
        d->document->setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }

    KoXmlElement body = KoXml::namedItemNS( realBody, KoXmlNS::office, "text" );
    if ( body.isNull() ) {
        kError(32001) << "No office:text found!" << endl;
        KoXmlElement childElem;
        QString localName;
        forEachElement( childElem, realBody )
            localName = childElem.localName();
        if ( localName.isEmpty() )
            d->document->setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            d->document->setErrorMessage( i18n( "This is not a word processing document, but %1. Please try opening it with the appropriate application.", KoDocument::tagNameToDocumentType( localName ) ) );
        return false;
    }

    // TODO check versions and mimetypes etc.

    KoOdfLoadingContext odfContext( odfStore.styles(), odfStore.store() );
    KoShapeLoadingContext sc( odfContext, d->document );

    // Load all styles before the corresponding paragraphs try to use them!
    //KoTextSharedLoadingData * sharedData = new KoTextSharedLoadingData();
    KWOdfSharedLoadingData * sharedData = new KWOdfSharedLoadingData(this);
    KoStyleManager *styleManager = dynamic_cast<KoStyleManager *>( d->document->dataCenterMap()["StyleManager"] );
    Q_ASSERT( styleManager );
    sharedData->loadOdfStyles( odfContext, styleManager, true );
    sc.addSharedData( KOTEXT_SHARED_LOADING_ID, sharedData );

    KoTextLoader * loader = new KoTextLoader( sc );
Q_UNUSED(loader);
    KoOdfLoadingContext context( odfStore.styles(), odfStore.store() );


    KoColumns columns;
    columns.columns = 1;
    columns.columnSpacing = d->document->config().defaultColumnSpacing();

    // In theory the page format is the style:master-page-name of the first paragraph...
    // But, hmm, in a doc with only a table there was no reference to the master page at all...
    // So we load the standard page layout to start with, and in KWTextParag
    // we might overwrite it with another one.
    d->currentMasterPage = "Standard";
    if ( !loadPageLayout(context, d->currentMasterPage) )
        return false;
    // It's quite possible that the following line asserts if we load e.g. an document
    // that does not contain anything except a single table.
    //Q_ASSERT( context.stylesReader().masterPages().contains( d->currentMasterPage ) );

#if 0 //1.6:
    KWOasisLoader oasisLoader( this );
    // <text:page-sequence> oasis extension for DTP (2003-10-27 post by Daniel)
    m_processingType = ( !KoXml::namedItemNS( body, KoXmlNS::text, "page-sequence" ).isNull() ) ? DTP : WP;
    m_hasTOC = false;
    m_tabStop = MM_TO_POINT(15);
    const KoXmlElement* defaultParagStyle = styles.defaultStyle( "paragraph" );
    if ( defaultParagStyle ) {
        KoStyleStack stack;
        stack.push( *defaultParagStyle );
        stack.setTypeProperties( "paragraph" );
        QString tabStopVal = stack.property( KoXmlNS::style, "tab-stop-distance" );
        if ( !tabStopVal.isEmpty() ) m_tabStop = KoUnit::parseValue( tabStopVal );
    }
    m_initialEditing = 0;
    // TODO MAILMERGE
    // Variable settings
    // By default display real variable value
    if ( !isReadWrite())
        m_varColl->variableSetting()->setDisplayFieldCode(false);
#endif

    // Load all styles before the corresponding paragraphs try to use them!

#if 0 //1.6:
    if ( m_frameStyleColl->loadOasisStyles( context ) == 0 ) {
         // no styles loaded -> load default styles
        loadDefaultFrameStyleTemplates();
    }
    if ( m_tableStyleColl->loadOasisStyles( context, *m_styleColl, *m_frameStyleColl ) == 0 ) {
        // no styles loaded -> load default styles
        loadDefaultTableStyleTemplates();
    }
    static_cast<KWVariableSettings *>( m_varColl->variableSetting() )->loadNoteConfiguration( styles.officeStyle() );
    loadDefaultTableTemplates();
#else
    /*
    // We always needs at least one valid default paragraph style
    KoParagraphStyle *defaultParagraphStyle = d->document->styleManager()->defaultParagraphStyle();
    //const KoXmlElement* defaultParagraphStyle = context.stylesReader().defaultStyle("paragraph");
    //if( ! defaultParagraphStyle ) {
        KoParagraphStyle *parastyle = new KoParagraphStyle();
        parastyle->setName("Standard");
        d->document->styleManager()->add(parastyle);
        context.styleStack().setTypeProperties( "paragraph" ); // load all style attributes from "style:paragraph-properties"
        parastyle->loadOasis(context.styleStack()); // load the KoParagraphStyle from the stylestack
        KoCharacterStyle *charstyle = parastyle->characterStyle();
        context.styleStack().setTypeProperties( "text" ); // load all style attributes from "style:text-properties"
        charstyle->loadOasis(context.styleStack()); // load the KoCharacterStyle from the stylestack
    //}
    */
#endif

    KWord::TextFrameSetType type = KWord::MainTextFrameSet;
    KWTextFrameSet *fs = new KWTextFrameSet( d->document, type );
    fs->setAllowLayout(false);
    fs->setName( i18n( "Main Text Frameset" ) );
    //fs->loadOasisContent( body, context );

    // Get the factory for the TextShape
    KoShapeFactory *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
    Q_ASSERT(factory);
    // Create a TextShape
    KoShape *shape = factory->createDefaultShape( d->document );
    Q_ASSERT(shape);
    // The TextShape will be displayed within a KWTextFrame
    KWTextFrame *frame = new KWTextFrame(shape, fs);
    Q_UNUSED(frame);
    d->currentFrame = frame;
    d->document->addFrameSet(fs);

    // The KoTextShapeData does contain the data for our TextShape
    KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(textShapeData);
    textShapeData->setDocument( fs->document(), false /*transferOwnership*/ );
    // Let the TextShape handle loading the body element.
    textShapeData->loadOdf(body, sc);

    /*
    QTextCursor cursor( fs->document() );
    Q_ASSERT(! d->frameLoader);
    d->frameLoader = new KWOpenDocumentFrameLoader(this);
    loadBody(context, body, cursor);
    */

    if ( !loadMasterPageStyle(context, d->currentMasterPage) )
        return false;

    loadSettings( odfStore.settingsDoc() );

#if 0 //1.6:
    // This sets the columns and header/footer flags, and calls recalcFrames, so it must be done last.
    setPageLayout( m_pageLayout, m_loadingInfo->columns, m_loadingInfo->hf, false );
#else
    d->document->m_pageSettings.setColumns( columns );
    //d->document->setDefaultPageLayout( KoPageLayout::standardLayout() );
#endif

    //delete d->frameLoader;
    //d->frameLoader = 0;

    kDebug(32001) <<"========================> KWOdfLoader::load END";
    emit sigProgress(100);
    return true;
}

void KWOdfLoader::loadSettings( const KoXmlDocument& settingsDoc )
{
    if ( settingsDoc.isNull() )
        return;

    kDebug(32001)<<"KWOdfLoader::loadSettings";
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    if ( !viewSettings.isNull() )
        d->document->setUnit( KoUnit::unit(viewSettings.parseConfigItemString("unit")) );

    //1.6: KWOasisLoader::loadOasisIgnoreList
    KoOasisSettings::Items configurationSettings = settings.itemSet( "configuration-settings" );
    if ( !configurationSettings.isNull() ) {
        const QString ignorelist = configurationSettings.parseConfigItemString( "SpellCheckerIgnoreList" );
        kDebug(32001) <<"Ignorelist:" << ignorelist;
        //1.6: d->document->setSpellCheckIgnoreList( QStringList::split( ',', ignorelist ) );
    }
    //1.6: d->document->variableCollection()->variableSetting()->loadOasis( settings );
}

bool KWOdfLoader::loadPageLayout(KoOdfLoadingContext& context, const QString& masterPageName)
{
    kDebug(32001)<<"KWOdfLoader::loadPageLayout masterPageName="<<masterPageName;
    const KoOdfStylesReader& styles = context.stylesReader();
    const KoXmlElement* masterPage = styles.masterPages()[ masterPageName ];
    const KoXmlElement *masterPageStyle = masterPage ? styles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString() ) ) : 0;
    if ( masterPageStyle ) {
        KoPageLayout pageLayout = KoPageLayout::standardLayout();
        pageLayout.loadOasis( *masterPageStyle );
        //d->document->m_pageManager.setDefaultPage(pageLayout);
        d->document->setDefaultPageLayout(pageLayout);
#if 0 //1.6:
        const KoXmlElement properties( KoXml::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
        const KoXmlElement footnoteSep = KoXml::namedItemNS( properties, KoXmlNS::style, "footnote-sep" );
        if ( !footnoteSep.isNull() ) {
            // style:width="0.018cm" style:distance-before-sep="0.101cm"
            // style:distance-after-sep="0.101cm" style:adjustment="left"
            // style:rel-width="25%" style:color="#000000"
            const QString width = footnoteSep.attributeNS( KoXmlNS::style, "width", QString::null );
            if ( !width.isEmpty() ) m_footNoteSeparatorLineWidth = KoUnit::parseValue( width );
            QString pageWidth = footnoteSep.attributeNS( KoXmlNS::style, "rel-width", QString::null );
            if ( pageWidth.endsWith( '%' ) ) {
                pageWidth.truncate( pageWidth.length() - 1 ); // remove '%'
                m_iFootNoteSeparatorLineLength = qRound( pageWidth.toDouble() );
            }
            // Not in KWord: color, distance before and after separator
            const QString style = footnoteSep.attributeNS( KoXmlNS::style, "line-style", QString::null );
            if ( style == "solid" || style.isEmpty() ) m_footNoteSeparatorLineType = SLT_SOLID;
            else if ( style == "dash" ) m_footNoteSeparatorLineType = SLT_DASH;
            else if ( style == "dotted" ) m_footNoteSeparatorLineType = SLT_DOT;
            else if ( style == "dot-dash" ) m_footNoteSeparatorLineType = SLT_DASH_DOT;
            else if ( style == "dot-dot-dash" ) m_footNoteSeparatorLineType = SLT_DASH_DOT_DOT;
            else kdDebug() << "Unknown value for m_footNoteSeparatorLineType: " << style << endl;
            const QString pos = footnoteSep.attributeNS( KoXmlNS::style, "adjustment", QString::null );
            if ( pos == "centered" ) m_footNoteSeparatorLinePos = SLP_CENTERED;
            else if ( pos == "right") m_footNoteSeparatorLinePos = SLP_RIGHT;
            else // if ( pos == "left" ) m_footNoteSeparatorLinePos = SLP_LEFT;
        }
        const KoXmlElement columnsElem = KoXml::namedItemNS( properties, KoXmlNS::style, "columns" );
        if ( !columnsElem.isNull() ) {
            columns.columns = columnsElem.attributeNS( KoXmlNS::fo, "column-count", QString::null ).toInt();
            if ( columns.columns == 0 ) columns.columns = 1;
            // TODO OASIS OpenDocument supports columns of different sizes, using <style:column style:rel-width="...">
            // (with fo:start-indent/fo:end-indent for per-column spacing)
            // But well, it also allows us to specify a single gap.
            if ( columnsElem.hasAttributeNS( KoXmlNS::fo, "column-gap" ) ) columns.ptColumnSpacing = KoUnit::parseValue( columnsElem.attributeNS( KoXmlNS::fo, "column-gap", QString::null ) );
            // It also supports drawing a vertical line as a separator...
        }
        // TODO spHeadBody (where is this in OOo?)
        // TODO spFootBody (where is this in OOo?)
        // Answer: margins of the <style:header-footer> element
#endif
    }
#if 0 //1.6:
    else { // this doesn't happen with normal documents, but it can happen if copying something, pasting into konq as foo.odt, then opening that...
        d->columns.columns = 1;
        d->columns.columnSpacing = 2;
        m_headerVisible = false;
        m_footerVisible = false;
        m_pageLayout = KoPageLayout::standardLayout();
        pageManager()->setDefaultPage(m_pageLayout);
    }
#else
    else {
        KoPageLayout pageLayout = KoPageLayout::standardLayout();
        d->document->setDefaultPageLayout(pageLayout);
    }
#endif
    return true;
}

bool KWOdfLoader::loadMasterPageStyle(KoOdfLoadingContext& context, const QString& masterPageName)
{
    kDebug(32001)<<"KWOdfLoader::loadMasterPageStyle masterPageName="<<masterPageName;
    const KoOdfStylesReader& styles = context.stylesReader();
    const KoXmlElement *masterPage = styles.masterPages()[ masterPageName ];
    const KoXmlElement *masterPageStyle = masterPage ? styles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString() ) ) : 0;
#if 0 //1.6:
    // This check is done here and not in loadOasisPageLayout in case the Standard master-page
    // has no page information but the first paragraph points to a master-page that does (#129585)
    if ( m_pageLayout.ptWidth <= 1e-13 || m_pageLayout.ptHeight <= 1e-13 ) {
        // Loading page layout failed, try to see why.
        KoXmlElement properties( KoXml::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
        //if ( properties.isNull() )
        //    setErrorMessage( i18n( "Invalid document. No page layout properties were found. The application which produced this document isn't OASIS-compliant." ) );
        //else if ( properties.hasAttributeNS( KoXmlNS::fo, "page-width" ) )
        //    setErrorMessage( i18n( "Invalid document. Page layout has no page width. The application which produced this document isn't OASIS-compliant." ) );
        //else
        if ( properties.hasAttributeNS( "http://www.w3.org/1999/XSL/Format", "page-width" ) )
            setErrorMessage( i18n( "Invalid document. 'fo' has the wrong namespace. The application which produced this document is not OASIS-compliant." ) );
        else
            setErrorMessage( i18n( "Invalid document. Paper size: %1x%2", m_pageLayout.ptWidth, m_pageLayout.ptHeight ) );
        return false;
    }
#endif
    if ( masterPageStyle ) {
        loadHeaderFooter(context, *masterPage, *masterPageStyle, true); // Load headers
        loadHeaderFooter(context, *masterPage, *masterPageStyle, false); // Load footers
    }
    return true;
}

// helper function to create a KWTextFrameSet+KWTextFrame for a header/footer.
void KWOdfLoader::Private::loadHeaderFooterFrame(KoOdfLoadingContext& context, const KoXmlElement& elem, KWord::HeaderFooterType hfType, KWord::TextFrameSetType fsType)
{
    KWTextFrameSet *fs = new KWTextFrameSet(document, fsType);
    fs->setAllowLayout(false);
    switch (fsType) {
        case KWord::FirstPageHeaderTextFrameSet: fs->setName(i18n("First Page Header")); break;
        case KWord::OddPagesHeaderTextFrameSet: fs->setName(i18n("Odd Pages Header")); break;
        case KWord::EvenPagesHeaderTextFrameSet: fs->setName(i18n("Even Pages Header")); break;
        case KWord::FirstPageFooterTextFrameSet: fs->setName(i18n("First Page Footer")); break;
        case KWord::OddPagesFooterTextFrameSet: fs->setName(i18n("Odd Pages Footer")); break;
        case KWord::EvenPagesFooterTextFrameSet: fs->setName(i18n("Even Pages Footer")); break;
        default: break;
    }

    kDebug(32001)<<"KWOdfLoader::loadHeaderFooterFrame localName="<<elem.localName()<<" type="<<fs->name();

    // Add the frameset and the shape for the header/footer to the document.
    KoShapeFactory *sf = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
    Q_ASSERT(sf);
    KoShape *s = sf->createDefaultShape( document );
    Q_ASSERT(s);
    KWTextFrame *f = new KWTextFrame(s, fs);
    document->addFrameSet(fs);

    KWTextFrame *prevFrame = currentFrame;
    currentFrame = f;

    // use auto-styles from styles.xml, not those from content.xml
    context.setUseStylesAutoStyles( true );

    KoShapeLoadingContext ctxt( context, document );
    KoTextLoader loader( ctxt );
    QTextCursor cursor( fs->document() );
    loader.loadBody(elem, cursor);

    // restore use of auto-styles from content.xml, not those from styles.xml
    context.setUseStylesAutoStyles( false );

    currentFrame = prevFrame; // restore the previous current frame
}

//1.6: KWOasisLoader::loadOasisHeaderFooter
void KWOdfLoader::loadHeaderFooter(KoOdfLoadingContext& context, const KoXmlElement& masterPage, const KoXmlElement& masterPageStyle, bool isHeader)
{
    // Not OpenDocument compliant element to define the first header/footer.
    KoXmlElement firstElem = KoXml::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header-first" : "footer-first" );
    // The actual content of the header/footer.
    KoXmlElement elem = KoXml::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header" : "footer" );
    // The two additional elements <style:header-left> and <style:footer-left> specifies if defined that even and odd pages
    // should be displayed different. If they are missing, the conent of odd and even (aka left and right) pages are the same.
    KoXmlElement leftElem = KoXml::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header-left" : "footer-left" );

    if ( firstElem.isNull() && elem.isNull() ) {
        return; // no header/footer
    }

    KWord::HeaderFooterType hfType = leftElem.isNull() ? KWord::HFTypeSameAsFirst : KWord::HFTypeEvenOdd;

    if ( ! firstElem.isNull() ) { // header-first and footer-first
        d->loadHeaderFooterFrame(context, firstElem, hfType, isHeader ? KWord::FirstPageHeaderTextFrameSet : KWord::FirstPageFooterTextFrameSet);
    }

    if ( ! leftElem.isNull() ) { // header-left and footer-left
        d->loadHeaderFooterFrame(context, leftElem, hfType, isHeader ? KWord::EvenPagesHeaderTextFrameSet : KWord::EvenPagesFooterTextFrameSet);
    }

    if ( ! elem.isNull() ) { // header and footer
        d->loadHeaderFooterFrame(context, elem, hfType, isHeader ? KWord::OddPagesHeaderTextFrameSet : KWord::OddPagesFooterTextFrameSet);
    }

    if (isHeader) {
        d->document->pageSettings().setFirstHeaderPolicy(hfType);
        d->document->pageSettings().setHeaderPolicy(hfType);
    }
    else {
        d->document->pageSettings().setFirstFooterPolicy(hfType);
        d->document->pageSettings().setFooterPolicy(hfType);
    }
}

void KWOdfLoader::loadFrame(KoOdfLoadingContext& context, const KoXmlElement& frameElem, QTextCursor& cursor)
{
#if 0 // TODO differently
    Q_ASSERT(d->frameLoader);
    d->frameLoader->loadFrame(context, frameElem, cursor);
#endif
}

