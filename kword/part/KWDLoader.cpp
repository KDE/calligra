/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "KWDLoader.h"
#include "KWDocument.h"
#include "KWPageSettings.h"
#include "frame/KWTextFrameSet.h"
#include "frame/KWTextFrame.h"

// koffice
#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoTextShape.h>

// KDE + Qt includes
#include <QDomDocument>
#include <QTextCursor>
#include <klocale.h>

KWDLoader::KWDLoader(KWDocument *parent)
    : m_document(parent),
    m_pageSettings(&parent->m_pageSettings),
    m_pageManager(&parent->m_pageManager),
    m_foundMainFS(false)
{
    connect(this, SIGNAL(sigProgress(int)), m_document, SIGNAL(sigProgress(int)));
}

KWDLoader::~KWDLoader() {
}

bool KWDLoader::load(QDomElement &root) {
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    kDebug(32001) << "KWDocument::loadXML" << endl;

    QString mime = root.attribute("mime");
    if ( mime.isEmpty() ) {
        kError(32001) << "No mime type specified!" << endl;
        m_document->setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    }
    else if ( mime != "application/x-kword" && mime != "application/vnd.kde.kword" ) {
        kError(32001) << "Unknown mime type " << mime << endl;
        m_document->setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-kword or application/vnd.kde.kword, got %1" , mime ) );
        return false;
    }
    //KWLoadingInfo *loadingInfo = new KWLoadingInfo();

    emit sigProgress(5);

    KoPageLayout pgLayout = KoPageLayout::standardLayout();
    // <PAPER>
    QDomElement paper = root.firstChildElement("PAPER");
    if ( !paper.isNull() )
    {
        pgLayout.format = static_cast<KoFormat>( paper.attribute("format").toInt() );
        pgLayout.orientation = static_cast<KoOrientation>( paper.attribute("orientation").toInt() );
        pgLayout.ptWidth = paper.attribute("width").toDouble();
        pgLayout.ptHeight = paper.attribute("height").toDouble();
        kDebug() << " ptWidth=" << pgLayout.ptWidth << endl;
        kDebug() << " ptHeight=" << pgLayout.ptHeight << endl;
        if ( pgLayout.ptWidth <= 0 || pgLayout.ptHeight <= 0 )
        {
            // Old document?
            pgLayout.ptWidth = paper.attribute("ptWidth").toDouble();
            pgLayout.ptHeight = paper.attribute("ptHeight").toDouble();
            kDebug() << " ptWidth2=" << pgLayout.ptWidth << endl;
            kDebug() << " ptHeight2=" << pgLayout.ptHeight << endl;

            // Still wrong?
            if ( pgLayout.ptWidth <= 0 || pgLayout.ptHeight <= 0 )
            {
                m_document->setErrorMessage( i18n( "Invalid document. Paper size: %1x%2", pgLayout.ptWidth, pgLayout.ptHeight ) );
                return false;
            }
        }

        m_pageSettings->setFirstHeaderPolicy(KWord::HFTypeUniform);
        switch(paper.attribute("hType").toInt()) {
            // assume its on; will turn it off in the next section.
            case 0:
                m_pageSettings->setHeaderPolicy(KWord::HFTypeSameAsFirst); break;
            case 1:
                m_pageSettings->setHeaderPolicy(KWord::HFTypeEvenOdd); break;
            case 2:
                m_pageSettings->setHeaderPolicy(KWord::HFTypeUniform); break;
            case 3:
                m_pageSettings->setHeaderPolicy(KWord::HFTypeEvenOdd); break;
        }
        m_pageSettings->setFirstFooterPolicy(KWord::HFTypeUniform);
        switch(paper.attribute("fType").toInt()) {
            // assume its on; will turn it off in the next section.
            case 0:
                m_pageSettings->setFooterPolicy(KWord::HFTypeSameAsFirst); break;
            case 1:
                m_pageSettings->setFooterPolicy(KWord::HFTypeEvenOdd); break;
            case 2:
                m_pageSettings->setFooterPolicy(KWord::HFTypeUniform); break;
            case 3:
                m_pageSettings->setFooterPolicy(KWord::HFTypeEvenOdd); break;
        }
        m_pageSettings->setHeaderDistance(paper.attribute("spHeadBody").toDouble());
        if(m_pageSettings->headerDistance() == 0.0) // fallback for kde2 version.
            m_pageSettings->setHeaderDistance(paper.attribute("ptHeadBody").toDouble());
        m_pageSettings->setFooterDistance(paper.attribute("spFootBody").toDouble());
        if(m_pageSettings->footerDistance() == 0.0) // fallback for kde2 version
            m_pageSettings->setFooterDistance(paper.attribute("ptFootBody").toDouble());

        m_pageSettings->setFootnoteDistance(paper.attribute("spFootNoteBody", "10.0").toDouble());
        if ( paper.hasAttribute( "slFootNoteLength" ) )
            m_pageSettings->setFootNoteSeparatorLineLength(
                    paper.attribute("slFootNoteLength").toInt());
        if ( paper.hasAttribute( "slFootNoteWidth" ) )
            m_pageSettings->setFootNoteSeparatorLineWidth( paper.attribute(
                        "slFootNoteWidth").toDouble());
        Qt::PenStyle type;
        switch(paper.attribute("slFootNoteType").toInt()) {
            case 1: type = Qt::DashLine; break;
            case 2: type = Qt::DotLine; break;
            case 3: type = Qt::DashDotLine; break;
            case 4: type = Qt::DashDotDotLine; break;
            default: type = Qt::SolidLine; break;
        }
        m_pageSettings->setFootNoteSeparatorLineType(type);

        if ( paper.hasAttribute("slFootNotePosition"))
        {
            QString tmp = paper.attribute("slFootNotePosition");
            KWord::FootNoteSeparatorLinePos pos;
            if ( tmp =="centered" )
                pos = KWord::FootNoteSeparatorCenter;
            else if ( tmp =="right")
                pos = KWord::FootNoteSeparatorRight;
            else if ( tmp =="left" )
                pos = KWord::FootNoteSeparatorLeft;
            m_pageSettings->setFootNoteSeparatorLinePosition(pos);
        }
        KoColumns columns = m_pageSettings->columns();
        if(paper.hasAttribute("columns"))
            columns.columns = paper.attribute("columns").toInt();
        if(paper.hasAttribute("columnspacing"))
            columns.ptColumnSpacing = paper.attribute("columnspacing").toDouble();
        else if(paper.hasAttribute("ptColumnspc")) // fallback for kde2 version
            columns.ptColumnSpacing = paper.attribute("ptColumnspc").toDouble();
        m_pageSettings->setColumns(columns);

        // <PAPERBORDERS>
        QDomElement paperborders = paper.namedItem( "PAPERBORDERS" ).toElement();
        if ( !paperborders.isNull() )
        {
            pgLayout.ptLeft = paperborders.attribute("left").toDouble();
            pgLayout.ptTop = paperborders.attribute("top").toDouble();
            pgLayout.ptRight = paperborders.attribute("right").toDouble();
            pgLayout.ptBottom = paperborders.attribute("bottom").toDouble();

            // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
            if ( pgLayout.ptLeft == 0.0 )
                pgLayout.ptLeft = paperborders.attribute("ptLeft").toDouble();
            if ( pgLayout.ptTop == 0.0 )
                pgLayout.ptTop = paperborders.attribute("ptTop").toDouble();
            if ( pgLayout.ptRight == 0.0 )
                pgLayout.ptRight = paperborders.attribute("ptRight").toDouble();
            if ( pgLayout.ptBottom == 0.0 )
                pgLayout.ptBottom = paperborders.attribute("ptBottom").toDouble();
        }
        else
            kWarning() << "No <PAPERBORDERS> tag!" << endl;
    }
    else
        kWarning() << "No <PAPER> tag! This is a mandatory tag! Expect weird page sizes..." << endl;

    m_pageManager->setDefaultPage(pgLayout);

    // <ATTRIBUTES>
    QDomElement attributes = root.firstChildElement("ATTRIBUTES");
    if ( !attributes.isNull() )
    {
        //m_processingType = static_cast<ProcessingType>( KWDocument::getAttribute( attributes, "processing", 0 ) );
        //KWDocument::getAttribute( attributes, "standardpage", QString::null );
        if(attributes.attribute("hasHeader") != "1") {
            m_pageSettings->setFirstHeaderPolicy(KWord::HFTypeNone);
            m_pageSettings->setHeaderPolicy(KWord::HFTypeNone);
        }
        if(attributes.attribute("hasFooter") != "1") {
            m_pageSettings->setFirstFooterPolicy(KWord::HFTypeNone);
            m_pageSettings->setFooterPolicy(KWord::HFTypeNone);
        }
        if ( attributes.hasAttribute( "unit" ) )
            m_document->setUnit( KoUnit::unit( attributes.attribute( "unit" ) ) );
        m_document->m_hasTOC = attributes.attribute("hasTOC") == "1";
        if(attributes.hasAttribute("tabStopValue"))
            m_document->m_tabStop = attributes.attribute("tabStopValue").toDouble();
/* TODO
        m_initialEditing = new InitialEditing();
        m_initialEditing->m_initialFrameSet = attributes.attribute( "activeFrameset" );
        m_initialEditing->m_initialCursorParag = attributes.attribute( "cursorParagraph" ).toInt();
        m_initialEditing->m_initialCursorIndex = attributes.attribute( "cursorIndex" ).toInt();
*/
    }

#if 0
    variableCollection()->variableSetting()->load(root );
    //by default display real variable value
    if ( !isReadWrite())
        variableCollection()->variableSetting()->setDisplayFieldCode(false);

    emit sigProgress(10);

    QDomElement mailmerge = root.namedItem( "MAILMERGE" ).toElement();
    if (mailmerge!=QDomElement())
    {
        m_slDataBase->load(mailmerge);
    }

    emit sigProgress(15);

    // Load all styles before the corresponding paragraphs try to use them!
    QDomElement stylesElem = root.namedItem( "STYLES" ).toElement();
    if ( !stylesElem.isNull() )
        loadStyleTemplates( stylesElem );

    emit sigProgress(17);

    QDomElement frameStylesElem = root.namedItem( "FRAMESTYLES" ).toElement();
    if ( !frameStylesElem.isNull() )
        loadFrameStyleTemplates( frameStylesElem );
    else // load default styles
        loadDefaultFrameStyleTemplates();

    emit sigProgress(18);

    QDomElement tableStylesElem = root.namedItem( "TABLESTYLES" ).toElement();
    if ( !tableStylesElem.isNull() )
        loadTableStyleTemplates( tableStylesElem );
    else // load default styles
        loadDefaultTableStyleTemplates();

    emit sigProgress(19);

    loadDefaultTableTemplates();

    emit sigProgress(20);

    QDomElement bookmark = root.namedItem( "BOOKMARKS" ).toElement();
    if( !bookmark.isNull() )
    {
        QDomElement bookmarkitem = root.namedItem("BOOKMARKS").toElement();
        bookmarkitem = bookmarkitem.firstChild().toElement();

        while ( !bookmarkitem.isNull() )
        {
            if ( bookmarkitem.tagName() == "BOOKMARKITEM" )
            {
                KWLoadingInfo::BookMark bk;
                bk.bookname=bookmarkitem.attribute("name");
                bk.cursorStartIndex=bookmarkitem.attribute("cursorIndexStart").toInt();
                bk.frameSetName=bookmarkitem.attribute("frameset");
                bk.paragStartIndex = bookmarkitem.attribute("startparag").toInt();
                bk.paragEndIndex = bookmarkitem.attribute("endparag").toInt();
                bk.cursorEndIndex = bookmarkitem.attribute("cursorIndexEnd").toInt();
                Q_ASSERT( m_loadingInfo );
                m_loadingInfo->bookMarkList.append( bk );
            }
            bookmarkitem = bookmarkitem.nextSibling().toElement();
        }
    }

    QStringList lst;
    QDomElement spellCheckIgnore = root.namedItem( "SPELLCHECKIGNORELIST" ).toElement();
    if( !spellCheckIgnore.isNull() )
    {
        QDomElement spellWord=root.namedItem("SPELLCHECKIGNORELIST").toElement();
        spellWord=spellWord.firstChild().toElement();
        while ( !spellWord.isNull() )
        {
            if ( spellWord.tagName()=="SPELLCHECKIGNOREWORD" )
                lst.append(spellWord.attribute("word"));
            spellWord=spellWord.nextSibling().toElement();
        }
    }
    setSpellCheckIgnoreList( lst );
#endif
    emit sigProgress(25);


    QDomElement framesets = root.namedItem( "FRAMESETS" ).toElement();
    if ( !framesets.isNull() )
        loadFrameSets( framesets );

    emit sigProgress(85);
#if 0

    loadPictureMap( root );

    emit sigProgress(90);

    // <EMBEDDED>
    loadEmbeddedObjects( root );
#endif
    emit sigProgress(100); // the rest is only processing, not loading

    kDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    return true;
}

void KWDLoader::loadFrameSets( const QDomElement &framesets ) {
    // <FRAMESET>
    // First prepare progress info
    m_nrItemsToLoad = 0; // total count of items (mostly paragraph and frames)
    QDomElement framesetElem = framesets.firstChild().toElement();
    // Workaround the slowness of QDom's elementsByTagName
    QList<QDomElement> frameSetsList;
    for ( ; !framesetElem.isNull() ; framesetElem = framesetElem.nextSibling().toElement() )
    {
        if ( framesetElem.tagName() == "FRAMESET" )
        {
            frameSetsList.append( framesetElem );
            m_nrItemsToLoad += framesetElem.childNodes().count();
        }
    }

    m_itemsLoaded = 0;
    foreach(QDomElement elem, frameSetsList) {
        loadFrameSet(elem);
    }
}

KWFrameSet *KWDLoader::loadFrameSet( QDomElement framesetElem, bool loadFrames, bool loadFootnote) {
    QString fsname = framesetElem.attribute("name");

    switch(framesetElem.attribute("frameType").toInt()) {
    case 1: { // FT_TEXT
        QString tableName = framesetElem.attribute("grpMgr");
        if ( !tableName.isEmpty() ) { // Text frameset belongs to a table -> find table by name
/*
            KWTableFrameSet *table = 0;
            Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
            for ( ; fit.current() ; ++fit ) {
                KWFrameSet *f = fit.current();
                if( f->type() == FT_TABLE &&
                    f->isVisible() &&
                    f->name() == tableName ) {
                    table = static_cast<KWTableFrameSet *> (f);
                    break;
                }
            }
            // No such table yet -> create
            if ( !table ) {
                table = new KWTableFrameSet( this, tableName );
                addFrameSet(table, false);
            }
            // Load the cell
            return table->loadCell( framesetElem );
 */
            return 0; // TODO support backwards compatible tables
        }
        else {
            if ( framesetElem.attribute("frameInfo").toInt() == 7 ) // of type FOOTNOTE
            {
                return 0; // TODO support old footnote frameset
/*
                if ( !loadFootnote )
                    return 0;
                // Footnote -> create a KWFootNoteFrameSet
                KWFootNoteFrameSet *fs = new KWFootNoteFrameSet( this, fsname );
                fs->load( framesetElem, loadFrames );
                addFrameSet(fs, false);
                return fs; */

            }
            else { // Normal text frame
                KWord::TextFrameSetType type;
                switch(framesetElem.attribute("frameInfo").toInt()) {
                    case 0: // body
                        type = m_foundMainFS?KWord::OtherTextFrameSet:KWord::MainTextFrameSet;
                        m_foundMainFS = true;
                        break;
                    case 1: // first header
                        type = KWord::FirstPageHeaderTextFrameSet; break;
                    case 2: // even header
                        type = KWord::EvenPagesHeaderTextFrameSet; break;
                    case 3: // odd header
                        type = KWord::OddPagesHeaderTextFrameSet; break;
                    case 4: // first footer
                        type = KWord::FirstPageFooterTextFrameSet; break;
                    case 5: // even footer
                        type = KWord::EvenPagesFooterTextFrameSet; break;
                    case 6: // odd footer
                        type = KWord::OddPagesFooterTextFrameSet; break;
                    case 7: // footnote
                        type = KWord::FootNoteTextFrameSet; break;
                    default:
                        type = KWord::OtherTextFrameSet; break;
                }
                KWTextFrameSet *fs = new KWTextFrameSet(type);
                fs->setName( fsname );
                fill(fs, framesetElem);
                m_document->addFrameSet(fs);

                // Old file format had autoCreateNewFrame as a frameset attribute
                if ( framesetElem.hasAttribute( "autoCreateNewFrame" ) ) {
                    KWord::FrameBehavior behav;
                    switch(framesetElem.attribute( "autoCreateNewFrame" ).toInt()) {
                        case 1: behav = KWord::AutoCreateNewFrameBehavior; break;
                        case 2: behav = KWord::IgnoreContentFrameBehavior; break;
                        default: behav = KWord::AutoExtendFrameBehavior; break;
                    }
                    foreach(KWFrame *frame, fs->frames())
                        frame->setFrameBehavior(behav);
                }
                return fs;
            }
        }
    }
    case 5: // FT_CLIPART
    {
        kError(32001) << "FT_CLIPART used! (in KWDocument::loadFrameSet)" << endl;
        // Do not break!
    }
    case 2: // FT_PICTURE
    {
/*
        KWPictureFrameSet *fs = new KWPictureFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        addFrameSet(fs, false);
        return fs; */
        // TODO return image frameset
        return 0;
    }
    case 4: { //FT_FORMULA
#if 0
        KWFormulaFrameSet *fs = new KWFormulaFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        addFrameSet(fs, false);
        return fs;
#endif
        // TODO support old formula frameset
        return 0;
    }
    // Note that FT_PART cannot happen when loading from a file (part frames are saved into the SETTINGS tag)
    // and FT_TABLE can't happen either.
    case 3: // FT_PART
        kWarning(32001) << "loadFrameSet: FT_PART: impossible case" << endl;
        return 0;
    case 10: // FT_TABLE
        kWarning(32001) << "loadFrameSet: FT_TABLE: impossible case" << endl;
        return 0;
    case 0: // FT_BASE
        kWarning(32001) << "loadFrameSet: FT_BASE !?!?" << endl;
        return 0;
    default: // other
        kWarning(32001) << "loadFrameSet error: unknown type, skipping" << endl;
        return 0;
    }
}

void KWDLoader::fill(KWFrameSet *fs, QDomElement framesetElem) {
    //m_visible = static_cast<bool>( KWDocument::getAttribute( framesetElem, "visible", true ) ); // TODO
    //m_protectSize=static_cast<bool>( KWDocument::getAttribute( framesetElem, "protectSize", false ) ); TODO

}

void KWDLoader::fill(KWTextFrameSet *fs, QDomElement framesetElem) {
    fill(static_cast<KWFrameSet*>(fs), framesetElem);
    // <FRAME>
    QDomElement frameElem = framesetElem.firstChild().toElement();
    for ( ; !frameElem.isNull() ; frameElem = frameElem.nextSibling().toElement() )
    {
        if ( frameElem.tagName() == "FRAME" )
        {
            QPointF origin( frameElem.attribute("left").toDouble(),
                    frameElem.attribute("top").toDouble() );
            QSizeF size( frameElem.attribute("right").toDouble() - origin.x(),
                   frameElem.attribute("bottom").toDouble() - origin.y() );
            KoShapeFactory *factory = KoShapeRegistry::instance()->get(KoTextShape_SHAPEID);
            Q_ASSERT(factory);
            KoShape *shape = factory->createDefaultShape();
            shape->setPosition(origin);
            shape->resize(size);
            KWTextFrame *frame = new KWTextFrame(shape, fs); // TODO make more general and not assume these are text frames

            //frame->load( frameElem, this, m_doc->syntaxVersion() );
            //m_doc->progressItemLoaded();
        }
    }
    // move above to be fs-type independent


    //m_info = static_cast<KWFrameSet::Info>( KWDocument::getAttribute( framesetElem, "frameInfo", KWFrameSet::FI_BODY ) ); // TODO
    switch(framesetElem.attribute("frameInfo").toInt()) {
        case 0: ;
    }
    if ( framesetElem.hasAttribute( "protectContent"))
        fs->setProtectContent((bool)framesetElem.attribute( "protectContent" ).toInt());

    fs->document()->clear(); // Get rid of dummy paragraph (and more if any)

    QTextCursor cursor(fs->document());
    // <PARAGRAPH>
    QDomElement paragraph = framesetElem.firstChild().toElement();
    for ( ; !paragraph.isNull() ; paragraph = paragraph.nextSibling().toElement() )
    {
        if ( paragraph.tagName() == "PARAGRAPH" )
        {
            cursor.insertText( paragraph.firstChildElement("TEXT").text() );
            cursor.insertText("\n");
/*
            KWTextParag *parag = new KWTextParag( textDocument(), lastParagraph );
            parag->load( paragraph );
            if ( !lastParagraph )        // First parag
                textDocument()->setFirstParag( parag );
            lastParagraph = parag;
            m_doc->progressItemLoaded(); */
        }
    }
}

#include "KWDLoader.moc"
