/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
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

#include "KWOpenDocumentLoader.h"
#include "KWDocument.h"
//#include "KWPageSettings.h"
#include "frame/KWTextFrameSet.h"
#include "frame/KWTextFrame.h"

// koffice
#include <KoOasisLoadingContext.h>
#include <KoOasisStyles.h>
#include <KoOasisSettings.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
//#include <KoListStyle.h>
//#include <KoTextShapeData.h>
#include <KoPageLayout.h>

// KDE + Qt includes
#include <QDomDocument>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <klocale.h>

#if 0
/// Temporary information used only during loading
class KWLoadingInfo
{
public:
    KWLoadingInfo() {
        columns.columns = 1;
        // columns.ptColumnSpacing must be initialized by KWDocument

        hf.header = HF_SAME;
        hf.footer = HF_SAME;
        hf.ptHeaderBodySpacing = 10.0;
        hf.ptFooterBodySpacing = 10.0;
        hf.ptFootNoteBodySpacing = 10.0;
    }
    ~KWLoadingInfo() {}

    /// Current master-page name (OASIS loading)
    QString m_currentMasterPage;

    /// Bookmarks (kword-1.3 XML: they need all framesets to be loaded first)
    struct BookMark
    {
        QString bookname;
        int paragStartIndex;
        int paragEndIndex;
        QString frameSetName;
        int cursorStartIndex;
        int cursorEndIndex;
    };

    typedef QValueList<BookMark> BookMarkList;
    BookMarkList bookMarkList;

    /// Bookmarks (OASIS XML). Only need to store bookmark starts, until hitting bookmark ends
    struct BookmarkStart {
        BookmarkStart() {} // for stupid QValueList
        BookmarkStart( KWTextDocument* _doc, KoTextParag* par, int ind )
            : doc( _doc ), parag( par ), pos( ind ) {}
        KWTextDocument* doc;
        KoTextParag* parag;
        int pos;
    };
    typedef QMap<QString, BookmarkStart> BookmarkStartsMap;
    BookmarkStartsMap m_bookmarkStarts;

    // Text frame chains; see KWTextFrameSet::loadOasisText

    void storeNextFrame( KWFrame* thisFrame, const QString& chainNextName ) {
        m_nextFrameDict.insert( chainNextName, thisFrame );
    }
    KWFrame* chainPrevFrame( const QString& frameName ) const {
        return m_nextFrameDict[frameName]; // returns 0 if not found
    }

    void storeFrameName( KWFrame* frame, const QString& name ) {
        m_frameNameDict.insert( name, frame );
    }
    KWFrame* frameByName( const QString& name ) const {
        return m_frameNameDict[name]; // returns 0 if not found
    }

    KoColumns columns;
    KoKWHeaderFooter hf;

private:
    // Ignore warnings about operator delete from those dicts; we don't use it here...
    QDict<KWFrame> m_nextFrameDict;
    QDict<KWFrame> m_frameNameDict;
};
#endif

/// \internal d-pointer class.
class KWOpenDocumentLoader::Private
{
    public:
        /// The KWord document.
        KWDocument *document;
        /// Current master-page name (OASIS loading)
        QString currentMasterPage;
        /// Structure for columns defined in KoPageLayout.h
        KoColumns columns;
};

KWOpenDocumentLoader::KWOpenDocumentLoader(KWDocument *parent)
    : d(new Private())
    //, m_pageSettings(&parent->m_pageSettings)
    //, m_pageManager(&parent->m_pageManager)
{
    d->document = parent;
    connect(this, SIGNAL(sigProgress(int)), d->document, SIGNAL(sigProgress(int)));
}

KWOpenDocumentLoader::~KWOpenDocumentLoader() {
    delete d;
}

bool KWOpenDocumentLoader::load(const QDomDocument& doc, KoOasisStyles& styles, const QDomDocument& settings, KoStore* store) {
    QTime dt;
    dt.start();
    emit sigProgress( 0 );

    kDebug(32001) << "========================> KWOpenDocumentLoader::load START" << endl;

    QDomElement content = doc.documentElement();
    QDomElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        kError(32001) << "No office:body found!" << endl;
        d->document->setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }

    QDomElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "text" );
    if ( body.isNull() )
    {
        kError(32001) << "No office:text found!" << endl;
        QDomElement childElem;
        QString localName;
        forEachElement( childElem, realBody ) {
            localName = childElem.localName();
        }
        if ( localName.isEmpty() )
            d->document->setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            d->document->setErrorMessage( i18n( "This is not a word processing document, but %1. Please try opening it with the appropriate application." ).arg( KoDocument::tagNameToDocumentType( localName ) ) );
        return false;
    }

    // TODO check versions and mimetypes etc.

    KoOasisLoadingContext context( d->document, styles, store );

    d->columns.columns = 1;
    d->columns.columnSpacing = d->document->m_defaultColumnSpacing;

    // In theory the page format is the style:master-page-name of the first paragraph...
    // But, hmm, in a doc with only a table there was no reference to the master page at all...
    // So we load the standard page layout to start with, and in KWTextParag
    // we might overwrite it with another one.
    d->currentMasterPage = "Standard";
    if ( !loadOasisPageLayout( d->currentMasterPage, context ) )
        return false;

#if 0
    KWOasisLoader oasisLoader( this );

    // <text:page-sequence> oasis extension for DTP (2003-10-27 post by Daniel)
    m_processingType = ( !KoDom::namedItemNS( body, KoXmlNS::text, "page-sequence" ).isNull() )
                       ? DTP : WP;
    m_hasTOC = false;
    m_tabStop = MM_TO_POINT(15);
    const QDomElement* defaultParagStyle = styles.defaultStyle( "paragraph" );
    if ( defaultParagStyle ) {
        KoStyleStack stack;
        stack.push( *defaultParagStyle );
        stack.setTypeProperties( "paragraph" );
        QString tabStopVal = stack.attributeNS( KoXmlNS::style, "tab-stop-distance" );
        if ( !tabStopVal.isEmpty() )
            m_tabStop = KoUnit::parseValue( tabStopVal );
    }
    m_initialEditing = 0;

    // TODO MAILMERGE

    // Variable settings
    // By default display real variable value
    if ( !isReadWrite())
        m_varColl->variableSetting()->setDisplayFieldCode(false);
#endif

    // Load all styles before the corresponding paragraphs try to use them!
    loadOasisStyles( context );

#if 0
    if ( m_frameStyleColl->loadOasisStyles( context ) == 0 ) {
         // no styles loaded -> load default styles
        loadDefaultFrameStyleTemplates();
    }

    if ( m_tableStyleColl->loadOasisStyles( context, *m_styleColl, *m_frameStyleColl ) == 0 ) {
        // no styles loaded -> load default styles
        loadDefaultTableStyleTemplates();
    }

    static_cast<KWVariableSettings *>( m_varColl->variableSetting() )
        ->loadNoteConfiguration( styles.officeStyle() );

    loadDefaultTableTemplates();
#endif

#if 0
    if ( m_processingType == WP ) {
        // Create main frameset
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Main Text Frameset" ) );
        m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading
        fs->loadOasisContent( body, context );
        KWFrame* frame = new KWFrame( fs, 29, 42, 566-29, 798-42 );
        frame->setFrameBehavior( KWFrame::AutoCreateNewFrame );
        frame->setNewFrameBehavior( KWFrame::Reconnect );
        fs->addFrame( frame );

        // load padding, background and borders for the main frame
        const QDomElement* masterPage = context.styles().masterPages()[ d->currentMasterPage ];
        const QDomElement *masterPageStyle = masterPage ? context.styles().findStyle(masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
        if ( masterPageStyle )
        {
          KoStyleStack styleStack;
          styleStack.push(  *masterPageStyle );
          styleStack.setTypeProperties( "page-layout" );
          frame->loadBorderProperties( styleStack );
        }
        fs->renumberFootNotes( false /*no repaint*/ );

    } else {
        // DTP mode: the items in the body are page-sequence and then frames
        QDomElement tag;
        forEachElement( tag, body )
        {
            context.styleStack().save();
            const QString localName = tag.localName();
            if ( localName == "page-sequence" && tag.namespaceURI() == KoXmlNS::text )
            {
                // We don't have support for changing the page layout yet, so just take the
                // number of pages
                int pages=1;
                QDomElement page;
                forEachElement( page, tag )
                    ++pages;
                kDebug() << "DTP mode: found " << pages << "pages" << endl;
                //setPageCount ( pages );
            }
            else if ( localName == "frame" && tag.namespaceURI() == KoXmlNS::draw )
                oasisLoader.loadFrame( tag, context, KoPoint() );
            else
                kWarning(32001) << "Unsupported tag in DTP loading:" << tag.tagName() << endl;
        }
    }
#else
    KoPageLayout pgLayout = KoPageLayout::standardLayout();
    d->document->m_pageManager.setDefaultPage(pgLayout);

    KWord::TextFrameSetType type = KWord::MainTextFrameSet;
    KWTextFrameSet *fs = new KWTextFrameSet( d->document, type );
    fs->setAllowLayout(false);
    fs->setName( i18n( "Main Text Frameset" ) );
    d->document->addFrameSet(fs);
    //fs->loadOasisContent( body, context );

    KoShapeFactory *factory = KoShapeRegistry::instance()->get(TextShape_SHAPEID);
    Q_ASSERT(factory);
    KoShape *shape = factory->createDefaultShape();
    //shape->setZIndex(123);
    KWTextFrame *frame = new KWTextFrame(shape, fs);
    frame->setFrameBehavior(KWord::AutoExtendFrameBehavior);

    QTextCursor cursor( fs->document() );
    loadOasisText(body, context, cursor);

#endif

    if ( !loadMasterPageStyle( d->currentMasterPage, context ) )
        return false;

#if 0
    if ( context.cursorTextParagraph() ) {
        // Maybe, once 1.3-support is dropped, we can get rid of InitialEditing and fetch the
        // values from KoOasisContext? But well, it lives a bit longer.
        // At least we could store a KWFrameSet* and a KoTextParag* instead of a name and an id.
        m_initialEditing = new InitialEditing();
        KWTextFrameSet* fs = static_cast<KWTextDocument *>( context.cursorTextParagraph()->textDocument() )->textFrameSet();
        m_initialEditing->m_initialFrameSet = fs->name();
        m_initialEditing->m_initialCursorParag = context.cursorTextParagraph()->paragId();
        m_initialEditing->m_initialCursorIndex = context.cursorTextIndex();
    }
#endif

    loadOasisSettings( settings );

#if 0
    // This sets the columns and header/footer flags, and calls recalcFrames,
    // so it must be done last.
    setPageLayout( m_pageLayout, m_loadingInfo->columns, m_loadingInfo->hf, false );
#endif

    kDebug(32001) << "========================> KWOpenDocumentLoader::load END" << endl;
    kDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
    emit sigProgress(100);
    return true;
}

//KoTextDocument::loadOasisText
void KWOpenDocumentLoader::loadOasisText( const QDomElement& bodyElem, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    KoXmlElement tag;
    forEachElement(tag, bodyElem)
    {
        const QString localName = tag.localName();
        const bool isTextNS = tag.namespaceURI() == KoXmlNS::text;

        if ( isTextNS && localName == "p" ) {  // text paragraph
            //kDebug()<<"==> PARAGRAPH <=="<<endl;
            context.fillStyleStack( tag, KoXmlNS::text, "style-name", "paragraph" );
            loadOasisSpan(tag, context, cursor);
        }
        else if ( isTextNS && localName == "h" ) // heading
        {
            //kDebug()<<"==> HEADING <=="<<endl;
            context.fillStyleStack( tag, KoXmlNS::text, "style-name", "paragraph" );

            //int level = tag.attributeNS( KoXmlNS::text, "outline-level", QString::null ).toInt();
            //context.pushOutlineListLevelStyle( level );

            loadOasisSpan(tag, context, cursor);
        }
        //TODO else...
    }
}

void KWOpenDocumentLoader::loadOasisSettings(const QDomDocument& settingsDoc)
{
    if ( settingsDoc.isNull() )
    {
        return;
    }

    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    if ( !viewSettings.isNull() )
    {
        d->document->setUnit( KoUnit::unit(viewSettings.parseConfigItemString("unit")) );
    }

    //KWOasisLoader::loadOasisIgnoreList
    KoOasisSettings::Items configurationSettings = settings.itemSet( "configuration-settings" );
    if ( !configurationSettings.isNull() )
    {
        const QString ignorelist = configurationSettings.parseConfigItemString( "SpellCheckerIgnoreList" );
        kDebug(32001) << "Ignorelist: " << ignorelist << endl;
#if 0
        d->document->setSpellCheckIgnoreList( QStringList::split( ',', ignorelist ) );
#endif
    }

#if 0
    d->document->variableCollection()->variableSetting()->loadOasis( settings );
#endif
}

//KoStyleCollection::loadOasisStyles
void KWOpenDocumentLoader::loadOasisStyles(KoOasisLoadingContext& context)
{
#if 0
    QStringList followingStyles;
    QList<KoXmlElement*> userStyles = context.oasisStyles().customStyles( "paragraph" ).values();
    bool defaultStyleDeleted = false;
    int stylesLoaded = 0;
    const unsigned int nStyles = userStyles.count();
    for (unsigned int item = 0; item < nStyles; item++) {
        KoXmlElement* styleElem = userStyles[item];
        if ( !styleElem ) continue;
	Q_ASSERT( !styleElem->isNull() );

        if( !defaultStyleDeleted ) { // we are going to import at least one style.
            KoParagStyle *s = defaultStyle();
            //kDebug() << "loadOasisStyles looking for Standard, to delete it. Found " << s << endl;
            if(s) // delete the standard style.
                removeStyle(s);
            defaultStyleDeleted = true;
        }

        KoParagStyle *sty = new KoParagStyle( QString::null );
        // Load the style
        sty->loadStyle( *styleElem, context );
        // Style created, now let's try to add it
        const int oldStyleCount = count();
        sty = addStyle( sty );
        // the real value of followingStyle is set below after loading all styles
        sty->setFollowingStyle( sty );

        kDebug() << " Loaded style " << sty->name() << endl;

        if ( count() > oldStyleCount )
        {
            const QString following = styleElem->attributeNS( KoXmlNS::style, "next-style-name", QString::null );
            followingStyles.append( following );
            ++stylesLoaded;
        }
        else
            kWarning() << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }

    if( followingStyles.count() != styleList().count() ) {
        kDebug() << "Ouch, " << followingStyles.count() << " following-styles, but "
                       << styleList().count() << " styles in styleList" << endl;
    }

    unsigned int i = 0;
    QString tmpString;
    foreach( tmpString, followingStyles )
    {
        const QString followingStyleName = tmpString;
	if ( !followingStyleName.isEmpty() ) {
            KoParagStyle * style = findStyle( followingStyleName );
	    if ( style )
                styleAt(i)->setFollowingStyle( style );
	}
    }

    // TODO the same thing for style inheritance (style:parent-style-name) and setParentStyle()

    Q_ASSERT( defaultStyle() );
    return stylesLoaded;
#else
    QList<KoXmlElement*> userStyles = context.oasisStyles().customStyles( "paragraph" ).values();
    foreach(KoXmlElement* styleElem, userStyles) {
        if ( !styleElem ) continue;
        Q_ASSERT( !styleElem->isNull() );

        // KoParagStyle::loadStyle
        QString name = styleElem->attributeNS( KoXmlNS::style, "name", QString::null );
        QString displayName = styleElem->attributeNS( KoXmlNS::style, "display-name", QString::null );
        if ( displayName.isEmpty() )
            displayName = name;

        KoParagraphStyle *parastyle = new KoParagraphStyle();
        parastyle->setName(name);
        //parastyle->setParent( d->document->styleManager()->defaultParagraphStyle() );
        d->document->styleManager()->add(parastyle);

        kDebug()<<"Style name="<<name<<" displayName="<<displayName<<endl;

        KoCharacterStyle *charstyle = parastyle->characterStyle();
        //charstyle->setFontFamily();
        //charstyle->setFontPointSize();
        //charstyle->setFontWeight();
        //charstyle->setFontItalic();
        //charstyle->setFontOverline();
        //charstyle->setFontStrikeOut();
        //charstyle->setFontFixedPitch();

        //KoTextFormat::load
        KoStyleStack& styleStack = context.styleStack();
        Q_ASSERT( ! styleStack.hasAttributeNS( KoXmlNS::fo, "font-weight" ) ); //REMINDER
        /*
        if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-weight" ) ) { // 3.10.24
            QString fontWeight = styleStack.attributeNS( KoXmlNS::fo, "font-weight" );
            int boldness;
            if ( fontWeight == "normal" )
                boldness = 50;
            else if ( fontWeight == "bold" )
                boldness = 75;
            else
                // XSL/CSS has 100,200,300...900. Not the same scale as Qt!
                // See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#font-weight
                boldness = fontWeight.toInt() / 10;
            charstyle->setFontWeight( boldness );
        }
        else
            charstyle->setFontWeight( 95 );
        */

        if(name.startsWith("Head")) { //TESTCASE
            //charstyle->setFontPointSize(20.0);
            //style->setBreakAfter(true);
            //style->setLeftPadding(20.0);
            //style->setLeftMargin(20.0);
            //parastyle->setAlignment(Qt::AlignCenter | Qt::AlignAbsolute);
        }
        else {
            //parastyle->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        }

        //bool isOutline = styleElem.hasAttributeNS( KoXmlNS::style, "default-outline-level" );
        //int level = styleElem.attributeNS( KoXmlNS::style, "default-level", "1" ).toInt(); // 1-based
    }
#endif
}

//KoTextParag::loadOasisSpan
void KWOpenDocumentLoader::loadOasisSpan(const KoXmlElement& parent, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    QString styleName = context.styleStack().userStyleName( "paragraph" );
    KoParagraphStyle *style = d->document->styleManager()->paragraphStyle(styleName);

    kDebug(32001) << "KWOpenDocumentLoader::loadOasisSpan styleName=" << styleName << " styleFound=" << (style != 0) << " style->alignment="<<(style ? int(style->alignment()) : -1)<<endl;

    //QTextCursor cursor(fs->document()->end());
    //QTextCursor cursor(fs->document());
    //cursor.movePosition(QTextCursor::EndOfBlock);

    for (KoXmlNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        KoXmlElement ts = node.toElement();
        const QString localName( ts.localName() );
        const bool isTextNS = ts.namespaceURI() == KoXmlNS::text;

        if ( node.isText() )
        {
            QString text = node.toText().data();
            kDebug() << "  localName=" << localName << " parent.localName="<<parent.localName()<<" text=" << text << endl;
            Q_ASSERT( ! context.styleStack().hasAttributeNS( KoXmlNS::fo, "font-weight" ) ); //REMINDER

            QTextBlock block = cursor.block();
            style->applyStyle(block);

            cursor.insertText( text.replace('\n', QChar(0x2028)) );

            // re-apply char format after we added the text
            KoCharacterStyle *charstyle = d->document->styleManager()->characterStyle( cursor.blockCharFormat().intProperty(KoCharacterStyle::StyleId) );
            if(charstyle) {
                QTextBlock block = cursor.block();
                charstyle->applyStyle(block);
            }
            //cursor.insertBlock();
        }
        else if ( isTextNS && localName == "span" ) // text:span
        {
            kDebug() << "  localName=" << localName << " is span" << endl;
            context.styleStack().save();
            context.fillStyleStack( ts, KoXmlNS::text, "style-name", "text" );

            loadOasisSpan( ts, context, cursor ); // recurse

            context.styleStack().restore();
        }
        else if ( isTextNS && localName == "s" ) // text:s
        {
            int howmany = 1;
            if (ts.hasAttributeNS( KoXmlNS::text, "c"))
                howmany = ts.attributeNS( KoXmlNS::text, "c", QString::null).toInt();
            kDebug() << "  localName=" << localName << " is space(s) howmany=" << howmany << endl;
            cursor.insertText( QString().fill(32, howmany) );
        }
#if 0
        else if ( isTextNS && localName == "tab" ) // text:tab (it's tab-stop in OO-1.1 but tab in oasis)
        {
            textData = '\t';
        }
#endif
        else if ( isTextNS && localName == "line-break" ) // text:line-break
        {
            kDebug() << "  Node localName=" << localName << " is line-break" << endl;
            QTextBlockFormat emptyTbf;
            QTextCharFormat emptyCf;
            cursor.insertBlock(emptyTbf, emptyCf);
        }
        else if ( isTextNS && localName == "number" ) // text:number
        {
            // This is the number in front of a numbered paragraph,
            // written out to help export filters. We can ignore it.
        }
#if 0
#ifdef KOXML_USE_QDOM
        else if ( node.isProcessingInstruction() )
        {
            QDomProcessingInstruction pi = node.toProcessingInstruction();
            if ( pi.target() == "opendocument" && pi.data().startsWith( "cursor-position" ) )
            {
                context.setCursorPosition( this, pos );
            }
        }
#endif
#endif
        else
        {
#if 0
            bool handled = false;
            // Check if it's a variable
            KoVariable* var = context.variableCollection().loadOasisField( textDocument(), ts, context );
            if ( var )
            {
                textData = "#";     // field placeholder
                customItem = var;
                handled = true;
            }
            if ( !handled )
            {
                handled = textDocument()->loadSpanTag( ts, context,
                                                    this, pos,
                                                    textData, customItem );
                if ( !handled )
                {
                    kWarning(32500) << "Ignoring tag " << ts.tagName() << endl;
                    context.styleStack().restore();
                    continue;
                }
            }
#else
            kDebug() << "  Node localName=" << localName << " is UNHANDLED" << endl;
#endif
        }
    }

    QTextBlockFormat emptyTbf;
    QTextCharFormat emptyCf;
    cursor.insertBlock(emptyTbf, emptyCf);
}

bool KWOpenDocumentLoader::loadOasisPageLayout(const QString& masterPageName, KoOasisLoadingContext& context)
{
    const KoOasisStyles& styles = context.oasisStyles();
    Q_ASSERT( styles.masterPages().contains(masterPageName) );
    const QDomElement* masterPage = styles.masterPages()[ masterPageName ];
    Q_ASSERT( masterPage );
    const QDomElement *masterPageStyle = masterPage ? styles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
    Q_ASSERT( masterPageStyle );
    if ( masterPageStyle )
    {
#if 0
        m_pageLayout.loadOasis( *masterPageStyle );
        pageManager()->setDefaultPage(m_pageLayout);
        const QDomElement properties( KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
        const QDomElement footnoteSep = KoDom::namedItemNS( properties, KoXmlNS::style, "footnote-sep" );
        if ( !footnoteSep.isNull() ) {
            // style:width="0.018cm" style:distance-before-sep="0.101cm"
            // style:distance-after-sep="0.101cm" style:adjustment="left"
            // style:rel-width="25%" style:color="#000000"
            const QString width = footnoteSep.attributeNS( KoXmlNS::style, "width", QString::null );
            if ( !width.isEmpty() ) {
                m_footNoteSeparatorLineWidth = KoUnit::parseValue( width );
            }

            QString pageWidth = footnoteSep.attributeNS( KoXmlNS::style, "rel-width", QString::null );
            if ( pageWidth.endsWith( "%" ) ) {
                pageWidth.truncate( pageWidth.length() - 1 ); // remove '%'
                m_iFootNoteSeparatorLineLength = qRound( pageWidth.toDouble() );
            }
            // Not in KWord: color, distance before and after separator

            const QString style = footnoteSep.attributeNS( KoXmlNS::style, "line-style", QString::null );
            if ( style == "solid" || style.isEmpty() )
                m_footNoteSeparatorLineType = SLT_SOLID;
            else if ( style == "dash" )
                m_footNoteSeparatorLineType = SLT_DASH;
            else if ( style == "dotted" )
                m_footNoteSeparatorLineType = SLT_DOT;
            else if ( style == "dot-dash" )
                m_footNoteSeparatorLineType = SLT_DASH_DOT;
            else if ( style == "dot-dot-dash" )
                m_footNoteSeparatorLineType = SLT_DASH_DOT_DOT;
            else
                kdDebug() << "Unknown value for m_footNoteSeparatorLineType: " << style << endl;

            const QString pos = footnoteSep.attributeNS( KoXmlNS::style, "adjustment", QString::null );
            if ( pos == "centered" )
                m_footNoteSeparatorLinePos = SLP_CENTERED;
            else if ( pos == "right")
                m_footNoteSeparatorLinePos = SLP_RIGHT;
            else // if ( pos == "left" )
                m_footNoteSeparatorLinePos = SLP_LEFT;
        }

        const QDomElement columnsElem = KoDom::namedItemNS( properties, KoXmlNS::style, "columns" );
        if ( !columnsElem.isNull() )
        {
            columns.columns = columnsElem.attributeNS( KoXmlNS::fo, "column-count", QString::null ).toInt();
            if ( columns.columns == 0 )
                columns.columns = 1;
            // TODO OASIS OpenDocument supports columns of different sizes, using <style:column style:rel-width="...">
            // (with fo:start-indent/fo:end-indent for per-column spacing)
            // But well, it also allows us to specify a single gap.
            if ( columnsElem.hasAttributeNS( KoXmlNS::fo, "column-gap" ) )
                columns.ptColumnSpacing = KoUnit::parseValue( columnsElem.attributeNS( KoXmlNS::fo, "column-gap", QString::null ) );
            // It also supports drawing a vertical line as a separator...
        }

        m_headerVisible = false;
        m_footerVisible = false;

        // TODO spHeadBody (where is this in OOo?)
        // TODO spFootBody (where is this in OOo?)
        // Answer: margins of the <style:header-footer> element
#else
        const KoPageLayout* pageLayout = d->document->m_pageManager.defaultPage();
        Q_ASSERT(pageLayout);
        const_cast<KoPageLayout*>(pageLayout)->loadOasis( *masterPageStyle ); //FIXME const_cast seems to be wrong here
        d->document->setDefaultPageLayout(*pageLayout);
#endif
    }
#if 0
    else // this doesn't happen with normal documents, but it can happen if copying something,
         // pasting into konq as foo.odt, then opening that...
    {
        d->columns.columns = 1;
        d->columns.columnSpacing = 2;
        m_headerVisible = false;
        m_footerVisible = false;
        m_pageLayout = KoPageLayout::standardLayout();
        pageManager()->setDefaultPage(m_pageLayout);
    }
#endif
    return true;
}

bool KWOpenDocumentLoader::loadMasterPageStyle(const QString& masterPageName, KoOasisLoadingContext& context)
{
    const KoOasisStyles& styles = context.oasisStyles();
    Q_ASSERT( styles.masterPages().contains(masterPageName) );
    const QDomElement* masterPage = styles.masterPages()[ masterPageName ];
    Q_ASSERT( masterPage );
    const QDomElement *masterPageStyle = masterPage ? styles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
    Q_ASSERT( masterPageStyle );

#if 0
    // This check is done here and not in loadOasisPageLayout in case the Standard master-page
    // has no page information but the first paragraph points to a master-page that does (#129585)
    if ( m_pageLayout.ptWidth <= 1e-13 || m_pageLayout.ptHeight <= 1e-13 )
    {
        // Loading page layout failed, try to see why.
        QDomElement properties( KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
        //if ( properties.isNull() )
        //    setErrorMessage( i18n( "Invalid document. No page layout properties were found. The application which produced this document isn't OASIS-compliant." ) );
        //else if ( properties.hasAttributeNS( KoXmlNS::fo, "page-width" ) )
        //    setErrorMessage( i18n( "Invalid document. Page layout has no page width. The application which produced this document isn't OASIS-compliant." ) );
        //else
        if ( properties.hasAttributeNS( "http://www.w3.org/1999/XSL/Format", "page-width" ) )
            setErrorMessage( i18n( "Invalid document. 'fo' has the wrong namespace. The application which produced this document is not OASIS-compliant." ) );
        else
            setErrorMessage( i18n( "Invalid document. Paper size: %1x%2" ).arg( m_pageLayout.ptWidth ).arg( m_pageLayout.ptHeight ) );
        return false;
    }
#endif

    if ( masterPageStyle )
    {
        // Load headers
        loadOasisHeaderFooter(*masterPage, *masterPageStyle, context, true);
        // Load footers
        loadOasisHeaderFooter(*masterPage, *masterPageStyle, context, false);

#if 0
        // The bottom margin of headers is what we call headerBodySpacing
        // (TODO support the 3 other margins)
        if ( !headerStyle.isNull() ) {
            context.styleStack().push( headerStyle );
            context.styleStack().setTypeProperties( "header-footer" );
            d->hf.ptHeaderBodySpacing = KoUnit::parseValue( context.styleStack().attributeNS( KoXmlNS::fo, "margin-bottom" ) );
            context.styleStack().pop();
        }
        // The top margin of footers is what we call footerBodySpacing
        // (TODO support the 3 other margins)
        if ( !footerStyle.isNull() ) {
            context.styleStack().push( footerStyle );
            context.styleStack().setTypeProperties( "header-footer" );
            d->hf.ptFooterBodySpacing = KoUnit::parseValue( context.styleStack().attributeNS( KoXmlNS::fo, "margin-top" ) );
            context.styleStack().pop();
        }
        // TODO ptFootNoteBodySpacing
#endif
    }

    return true;
}

//KWOasisLoader::loadOasisHeaderFooter
void KWOpenDocumentLoader::loadOasisHeaderFooter(const QDomElement& masterPage, const QDomElement& masterPageStyle, KoOasisLoadingContext& context, bool isHeader)
{
    // Not OpenDocument compliant element to define the first header/footer.
    QDomElement firstElem = KoDom::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header-first" : "footer-first" );
    // The actual content of the header/footer.
    QDomElement elem = KoDom::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header" : "footer" );

    const bool hasFirst = !firstElem.isNull();
    if ( !hasFirst && elem.isNull() )
        return; // no header/footer

    const QString localName = elem.localName();
    kDebug()<<"KWOpenDocumentLoader::loadOasisHeaderFooter localName="<<localName<<" isHeader="<<isHeader<<" hasFirst="<<hasFirst<<endl;

    // Formatting properties for headers and footers on a page.
    QDomElement styleElem = KoDom::namedItemNS( masterPageStyle, KoXmlNS::style, isHeader ? "header-style" : "footer-style" );

    // The two additional elements <style:header-left> and <style:footer-left> specifies if defined that even and odd pages
    // should be displayed different. If they are missing, the conent of odd and even (aka left and right) pages are the same.
    QDomElement leftElem = KoDom::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header-left" : "footer-left" );

    // Determinate the type of the frameset used for the header/footer.
    QString fsTypeName;
    KWord::TextFrameSetType fsType = KWord::OtherTextFrameSet;
    if ( localName == "header" ) {
        fsType = KWord::OddPagesHeaderTextFrameSet;
        fsTypeName = leftElem.isNull() ? i18n( "Header" ) : i18n("Odd Pages Header");
    }
    else if ( localName == "header-left" ) {
        fsType = KWord::EvenPagesHeaderTextFrameSet;
        fsTypeName = i18n("Even Pages Header");
    }
    else if ( localName == "footer" ) {
        fsType = KWord::OddPagesFooterTextFrameSet;
        fsTypeName = leftElem.isNull() ? i18n( "Footer" ) : i18n("Odd Pages Footer");
    }
    else if ( localName == "footer-left" ) {
        fsType = KWord::EvenPagesFooterTextFrameSet;
        fsTypeName = i18n("Even Pages Footer");
    }
    else if ( localName == "header-first" ) { // NOT OASIS COMPLIANT
        fsType = KWord::FirstPageHeaderTextFrameSet;
        fsTypeName = i18n("First Page Header");
    }
    else if ( localName == "footer-first" ) { // NOT OASIS COMPLIANT
        fsType = KWord::FirstPageFooterTextFrameSet;
        fsTypeName = i18n("First Page Footer");
    }
    else {
        kWarning(32001) << "Unknown tag in KWOpenDocumentLoader::loadOasisHeaderFooter: " << localName << endl;
        return;
    }

    // Set the type of the header/footer in the KWPageSettings instance of our document.
    /*
        enum HeaderFooterType {//KWord2
            HFTypeNone,       ///< Don't show the frames
            HFTypeEvenOdd,    ///< Show different content for even and odd pages
            HFTypeUniform,    ///< Show the same content for each page
            HFTypeSameAsFirst ///< Show the same content for each page, including the first page
        };
        enum KoHFType { //KWord1.6
            HF_SAME = 0,            ///< 0: Header/Footer is the same on all pages
            HF_FIRST_EO_DIFF = 1,   ///< 1: Header/Footer is different on first, even and odd pages (2&3)
            HF_FIRST_DIFF = 2,      ///< 2: Header/Footer for the first page differs
            HF_EO_DIFF = 3          ///< 3: Header/Footer for even - odd pages are different
        };
    */
    if ( !leftElem.isNull() ) {
        //d->hf.header = hasFirst ? HF_FIRST_EO_DIFF : HF_EO_DIFF;
        if( isHeader ) {
            d->document->m_pageSettings.setHeaderPolicy(KWord::HFTypeEvenOdd);
            //d->document->m_pageSettings.setFirstHeaderPolicy(KWord::HFTypeEvenOdd);
        }
        else {
            d->document->m_pageSettings.setFooterPolicy(KWord::HFTypeEvenOdd);
            //d->document->m_pageSettings.setFirstFooterPolicy(KWord::HFTypeEvenOdd);
        }
    }
    else {
        //d->hf.header = hasFirst ? HF_FIRST_DIFF : HF_SAME;
        if( isHeader ) {
            d->document->m_pageSettings.setHeaderPolicy(KWord::HFTypeSameAsFirst);
            d->document->m_pageSettings.setFirstHeaderPolicy(KWord::HFTypeEvenOdd);
        }
        else {
            d->document->m_pageSettings.setFooterPolicy(KWord::HFTypeSameAsFirst);
            d->document->m_pageSettings.setFirstFooterPolicy(KWord::HFTypeEvenOdd);
        }
    }

#if 0
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
#else

    // Add the frameset and the shape for the header/footer to the document.
    KWTextFrameSet *fs = new KWTextFrameSet( d->document, fsType );
    fs->setAllowLayout(false);
    fs->setName(fsTypeName);
    d->document->addFrameSet(fs);
    KoShapeFactory *factory = KoShapeRegistry::instance()->get(TextShape_SHAPEID);
    Q_ASSERT(factory);
    KoShape *shape = factory->createDefaultShape();
    //shape->setZIndex(123);
    KWTextFrame *frame = new KWTextFrame(shape, fs);
    frame->setFrameBehavior(KWord::AutoExtendFrameBehavior);

    QTextCursor cursor( fs->document() );
    //cursor.insertText(fsTypeName); //TESTCASE

    //TODO I am pretty sure that is wrong somehow
    if ( !leftElem.isNull() )
        loadOasisText(leftElem, context, cursor);
    else if( hasFirst )
        loadOasisText(firstElem, context, cursor);
    else
        loadOasisText(elem, context, cursor);

#endif
}

#include "KWOpenDocumentLoader.moc"
