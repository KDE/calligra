/* This file is part of the KDE project
 * Copyright (C) 2006, 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
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
#include "KWPage.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWTextFrame.h"

// koffice
#include <KoShapeRegistry.h>
#include <KoInlineNote.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeContainer.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoListStyle.h>
#include <KoListLevelProperties.h>
#include <KoTextShapeData.h>
#include <KoTextAnchor.h>
#include <KoTextDocumentLayout.h>
#include <KoInlineTextObjectManager.h>
#include <KoColorBackground.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoBorder.h>

// KDE + Qt includes
#include <QTextBlock>
#include <klocale.h>
#include <kdebug.h>

KWDLoader::KWDLoader(KWDocument *parent, KoStore *store)
        : m_document(parent),
        m_store(store),
        m_pageManager(&parent->m_pageManager),
        m_pageStyle(m_pageManager->defaultPageStyle()),
        m_foundMainFS(false)
{
    connect(this, SIGNAL(progressUpdate(int)), m_document, SIGNAL(sigProgress(int)));
}

KWDLoader::~KWDLoader()
{
}

bool KWDLoader::load(KoXmlElement &root)
{
    QTime dt;
    dt.start();
    emit progressUpdate(0);
    kDebug(32001) << "KWDocument::loadXML";

    QString mime = root.attribute("mime");
    if (mime.isEmpty()) {
        kError(32001) << "No mime type specified!";
        m_document->setErrorMessage(i18n("Invalid document. No mimetype specified."));
        return false;
    } else if (mime != "application/x-kword" && mime != "application/vnd.kde.kword") {
        kError(32001) << "Unknown mime type " << mime;
        m_document->setErrorMessage(i18n("Invalid document. Expected mimetype application/x-kword or application/vnd.kde.kword, got %1" , mime));
        return false;
    }
    //KWLoadingInfo *loadingInfo = new KWLoadingInfo();

    emit progressUpdate(5);

    KoPageLayout pgLayout;
    // <PAPER>
    KoXmlElement paper = root.namedItem("PAPER").toElement();
    if (!paper.isNull()) {
        pgLayout.format = static_cast<KoPageFormat::Format>(paper.attribute("format").toInt());
        pgLayout.orientation = static_cast<KoPageFormat::Orientation>(paper.attribute("orientation").toInt());
        pgLayout.width = paper.attribute("width").toDouble();
        pgLayout.height = paper.attribute("height").toDouble();
        kDebug(32001) << " width=" << pgLayout.width;
        kDebug(32001) << " height=" << pgLayout.height;
        if (pgLayout.width <= 0 || pgLayout.height <= 0) {
            // Old document?
            pgLayout.width = paper.attribute("width").toDouble();
            pgLayout.height = paper.attribute("height").toDouble();
            kDebug(32001) << " width2=" << pgLayout.width;
            kDebug(32001) << " height2=" << pgLayout.height;

            // Still wrong?
            if (pgLayout.width <= 0 || pgLayout.height <= 0) {
                m_document->setErrorMessage(i18n("Invalid document. Paper size: %1x%2", pgLayout.width, pgLayout.height));
                return false;
            }
        }

        const int headerType = paper.attribute("hType").toInt();
        const int footerType = paper.attribute("fType").toInt();
        if (headerType == 1 || headerType == 2 || footerType == 1 || footerType == 2)
            m_firstPageStyle = KWPageStyle("First Page"); // we are going to need that

        switch (headerType) {
        case 0: // same on all pages
            m_pageStyle.setHeaderPolicy(KWord::HFTypeUniform); break;
        case 1: // different on first, even and odd pages (2&3)
            m_firstPageStyle.setHeaderPolicy(KWord::HFTypeUniform);
            m_pageStyle.setHeaderPolicy(KWord::HFTypeEvenOdd); break;
        case 2: // different on first and other pages
            m_firstPageStyle.setHeaderPolicy(KWord::HFTypeUniform);
            m_pageStyle.setHeaderPolicy(KWord::HFTypeUniform); break;
        case 3: // different on even and odd pages
            m_pageStyle.setHeaderPolicy(KWord::HFTypeEvenOdd); break;
        }
        switch (footerType) {
        case 0: // same on all pages
            m_pageStyle.setFooterPolicy(KWord::HFTypeUniform); break;
        case 1: // different on first, even and odd pages (2&3)
            m_firstPageStyle.setFooterPolicy(KWord::HFTypeUniform);
            m_pageStyle.setFooterPolicy(KWord::HFTypeEvenOdd); break;
        case 2: // different on first and other pages
            m_firstPageStyle.setFooterPolicy(KWord::HFTypeUniform);
            m_pageStyle.setFooterPolicy(KWord::HFTypeUniform); break;
        case 3: // different on even and odd pages
            m_pageStyle.setFooterPolicy(KWord::HFTypeEvenOdd); break;
        }
        m_pageStyle.setHeaderDistance(paper.attribute("spHeadBody").toDouble());
        if (m_pageStyle.headerDistance() == 0.0) // fallback for kde2 version.
            m_pageStyle.setHeaderDistance(paper.attribute("headBody").toDouble());
        m_pageStyle.setFooterDistance(paper.attribute("spFootBody").toDouble());
        if (m_pageStyle.footerDistance() == 0.0) // fallback for kde2 version
            m_pageStyle.setFooterDistance(paper.attribute("footBody").toDouble());

        m_pageStyle.setFootnoteDistance(paper.attribute("spFootNoteBody", "10.0").toDouble());
        if (paper.hasAttribute("slFootNoteLength"))
            m_pageStyle.setFootNoteSeparatorLineLength(
                paper.attribute("slFootNoteLength").toInt());
        if (paper.hasAttribute("slFootNoteWidth"))
            m_pageStyle.setFootNoteSeparatorLineWidth(paper.attribute(
                        "slFootNoteWidth").toDouble());
        Qt::PenStyle type;
        switch (paper.attribute("slFootNoteType").toInt()) {
        case 1: type = Qt::DashLine; break;
        case 2: type = Qt::DotLine; break;
        case 3: type = Qt::DashDotLine; break;
        case 4: type = Qt::DashDotDotLine; break;
        default: type = Qt::SolidLine; break;
        }
        m_pageStyle.setFootNoteSeparatorLineType(type);

        if (paper.hasAttribute("slFootNotePosition")) {
            QString tmp = paper.attribute("slFootNotePosition");
            KWord::FootNoteSeparatorLinePos pos;
            if (tmp == "centered")
                pos = KWord::FootNoteSeparatorCenter;
            else if (tmp == "right")
                pos = KWord::FootNoteSeparatorRight;
            else // default: if (tmp =="left")
                pos = KWord::FootNoteSeparatorLeft;
            m_pageStyle.setFootNoteSeparatorLinePosition(pos);
        }
        KoColumns columns = m_pageStyle.columns();
        if (paper.hasAttribute("columns"))
            columns.columns = paper.attribute("columns").toInt();
        if (paper.hasAttribute("columnspacing"))
            columns.columnSpacing = paper.attribute("columnspacing").toDouble();
        else if (paper.hasAttribute("columnspc")) // fallback for kde2 version
            columns.columnSpacing = paper.attribute("columnspc").toDouble();
        m_pageStyle.setColumns(columns);

        // <PAPERBORDERS>
        KoXmlElement paperborders = paper.namedItem("PAPERBORDERS").toElement();
        if (!paperborders.isNull()) {
            pgLayout.leftMargin   = paperborders.attribute("left").toDouble();
            pgLayout.topMargin    = paperborders.attribute("top").toDouble();
            pgLayout.rightMargin  = paperborders.attribute("right").toDouble();
            pgLayout.bottomMargin = paperborders.attribute("bottom").toDouble();

            // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
            if (pgLayout.leftMargin == 0.0)
                pgLayout.leftMargin = paperborders.attribute("left").toDouble();
            if (pgLayout.topMargin == 0.0)
                pgLayout.topMargin = paperborders.attribute("top").toDouble();
            if (pgLayout.rightMargin == 0.0)
                pgLayout.rightMargin = paperborders.attribute("right").toDouble();
            if (pgLayout.bottomMargin == 0.0)
                pgLayout.bottomMargin = paperborders.attribute("bottom").toDouble();
        } else
            kWarning(32001) << "No <PAPERBORDERS> tag!";
    } else
        kWarning(32001) << "No <PAPER> tag! This is a mandatory tag! Expect weird page sizes...";

    m_pageManager->pageStyle("Standard").setPageLayout(pgLayout);

    // <ATTRIBUTES>
    KoXmlElement attributes = root.namedItem("ATTRIBUTES").toElement();
    if (!attributes.isNull()) {
        if (attributes.attribute("processing", "0") == "1") {
            m_pageStyle.setHasMainTextFrame(false); // DTP type document.
            m_foundMainFS = true; // we will not reuse the main FS now.
        }

        //KWDocument::getAttribute(attributes, "standardpage", QString::null);
        if (attributes.attribute("hasHeader") != "1") {
            m_pageStyle.setHeaderPolicy(KWord::HFTypeNone);
            if (m_firstPageStyle.isValid())
                m_firstPageStyle.setHeaderPolicy(KWord::HFTypeNone);
        }
        if (attributes.attribute("hasFooter") != "1") {
            m_pageStyle.setFooterPolicy(KWord::HFTypeNone);
            if (m_firstPageStyle.isValid())
                m_firstPageStyle.setFooterPolicy(KWord::HFTypeNone);
        }
        if (attributes.hasAttribute("unit"))
            m_document->setUnit(KoUnit::unit(attributes.attribute("unit")));
        //m_document->m_hasTOC = attributes.attribute("hasTOC") == "1";
        //if(attributes.hasAttribute("tabStopValue"))
        //    m_document->m_tabStop = attributes.attribute("tabStopValue").toDouble();
        /* TODO
                m_initialEditing = new InitialEditing();
                m_initialEditing->m_initialFrameSet = attributes.attribute("activeFrameset");
                m_initialEditing->m_initialCursorParag = attributes.attribute("cursorParagraph").toInt();
                m_initialEditing->m_initialCursorIndex = attributes.attribute("cursorIndex").toInt();
        */
    }
    if (m_firstPageStyle.isValid()
            && m_firstPageStyle.footerPolicy() != KWord::HFTypeNone
            && m_firstPageStyle.headerPolicy() != KWord::HFTypeNone) {
        m_firstPageStyle.setColumns(m_pageStyle.columns());
        m_firstPageStyle.setHasMainTextFrame(m_pageStyle.hasMainTextFrame());
        m_firstPageStyle.setHeaderDistance(m_pageStyle.headerDistance());
        m_firstPageStyle.setFooterDistance(m_pageStyle.footerDistance());
        m_firstPageStyle.setEndNoteDistance(m_pageStyle.endNoteDistance());
        m_firstPageStyle.setPageLayout(m_pageStyle.pageLayout());
        m_pageManager->addPageStyle(m_firstPageStyle);
    }

#if 0
    variableCollection()->variableSetting()->load(root);
    //by default display real variable value
    if (!isReadWrite())
        variableCollection()->variableSetting()->setDisplayFieldCode(false);

    emit progressUpdate(10);

    KoXmlElement mailmerge = root.namedItem("MAILMERGE").toElement();
    if (mailmerge != KoXmlElement()) {
        m_slDataBase->load(mailmerge);
    }
#endif

    emit progressUpdate(15);

    // Load all styles before the corresponding paragraphs try to use them!
    KoXmlElement stylesElem = root.namedItem("STYLES").toElement();
    if (!stylesElem.isNull())
        loadStyleTemplates(stylesElem);

    emit progressUpdate(17);
#if 0

    KoXmlElement frameStylesElem = root.namedItem("FRAMESTYLES").toElement();
    if (!frameStylesElem.isNull())
        loadFrameStyleTemplates(frameStylesElem);
    else // load default styles
        loadDefaultFrameStyleTemplates();

    emit progressUpdate(18);

    KoXmlElement tableStylesElem = root.namedItem("TABLESTYLES").toElement();
    if (!tableStylesElem.isNull())
        loadTableStyleTemplates(tableStylesElem);
    else // load default styles
        loadDefaultTableStyleTemplates();

    emit progressUpdate(19);

    loadDefaultTableTemplates();

    emit progressUpdate(20);

    KoXmlElement bookmark = root.namedItem("BOOKMARKS").toElement();
    if (!bookmark.isNull()) {
        KoXmlElement bookmarkitem = root.namedItem("BOOKMARKS").toElement();
        bookmarkitem = bookmarkitem.firstChild().toElement();

        while (!bookmarkitem.isNull()) {
            if (bookmarkitem.tagName() == "BOOKMARKITEM") {
                KWLoadingInfo::BookMark bk;
                bk.bookname = bookmarkitem.attribute("name");
                bk.cursorStartIndex = bookmarkitem.attribute("cursorIndexStart").toInt();
                bk.frameSetName = bookmarkitem.attribute("frameset");
                bk.paragStartIndex = bookmarkitem.attribute("startparag").toInt();
                bk.paragEndIndex = bookmarkitem.attribute("endparag").toInt();
                bk.cursorEndIndex = bookmarkitem.attribute("cursorIndexEnd").toInt();
                Q_ASSERT(m_loadingInfo);
                m_loadingInfo->bookMarkList.append(bk);
            }
            bookmarkitem = bookmarkitem.nextSibling().toElement();
        }
    }

    QStringList lst;
    KoXmlElement spellCheckIgnore = root.namedItem("SPELLCHECKIGNORELIST").toElement();
    if (!spellCheckIgnore.isNull()) {
        KoXmlElement spellWord = root.namedItem("SPELLCHECKIGNORELIST").toElement();
        spellWord = spellWord.firstChild().toElement();
        while (!spellWord.isNull()) {
            if (spellWord.tagName() == "SPELLCHECKIGNOREWORD")
                lst.append(spellWord.attribute("word"));
            spellWord = spellWord.nextSibling().toElement();
        }
    }
    setSpellCheckIgnoreList(lst);
#endif
    KoXmlElement pixmaps = root.namedItem("PIXMAPS").toElement();
    if (pixmaps.isNull())
        pixmaps = root.namedItem("PICTURES").toElement();
    if (! pixmaps.isNull()) {
        KoXmlElement key;
        forEachElement(key, pixmaps) {
            if (! key.isNull() && key.nodeName() != "KEY")
                continue;
            ImageKey ik;
            fill(&ik, key);
            m_images.append(ik);
        }
    }


    emit progressUpdate(25);


    KoXmlElement framesets = root.namedItem("FRAMESETS").toElement();
    if (!framesets.isNull())
        loadFrameSets(framesets);

    emit progressUpdate(85);

    insertAnchors();
    insertNotes();

#if 0

    loadPictureMap(root);

    emit progressUpdate(90);

    // <EMBEDDED>
    loadEmbeddedObjects(root);
#endif
    if (m_firstPageStyle.isValid()) {
        m_pageManager->appendPage(m_firstPageStyle);
        m_pageManager->appendPage(m_pageStyle);
    }

    emit progressUpdate(100); // the rest is only processing, not loading

    kDebug(32001) << "Loading took" << (float)(dt.elapsed()) / 1000 << " seconds";

    return true;
}

void KWDLoader::loadFrameSets(const KoXmlElement &framesets)
{
    // <FRAMESET>
    // First prepare progress info
    m_nrItemsToLoad = 0; // total count of items (mostly paragraph and frames)
    KoXmlElement framesetElem;
    // Workaround the slowness of KoXml's elementsByTagName
    QList<KoXmlElement> frameSetsList;
    forEachElement(framesetElem, framesets) {
        if (framesetElem.tagName() == "FRAMESET") {
            frameSetsList.append(framesetElem);
            m_nrItemsToLoad += KoXml::childNodesCount(framesetElem);
        }
    }

    m_itemsLoaded = 0;
    foreach (const KoXmlElement &elem, frameSetsList) {
        loadFrameSet(elem);
    }
}

void KWDLoader::loadFrameSet(const KoXmlElement &framesetElem)
{
    QString fsname = framesetElem.attribute("name");

    switch (framesetElem.attribute("frameType").toInt()) {
    case 1: { // FT_TEXT
        QString tableName = framesetElem.attribute("grpMgr");
        if (!tableName.isEmpty()) {   // Text frameset belongs to a table -> find table by name
            return; // TODO support backwards compatible tables
        } else {
            KWord::TextFrameSetType type;
            KWPageStyle styleForFS;
            switch (framesetElem.attribute("frameInfo").toInt()) {
            case 0: // body
                type = m_foundMainFS ? KWord::OtherTextFrameSet : KWord::MainTextFrameSet;
                m_foundMainFS = true;
                break;
            case 1: // first header
                if (! m_firstPageStyle.isValid())
                    return; // we don't need this FS.
                styleForFS = m_firstPageStyle;
                type = KWord::OddPagesHeaderTextFrameSet; break;
            case 2: // even header
                styleForFS = m_pageStyle;
                type = KWord::EvenPagesHeaderTextFrameSet; break;
            case 3: // odd header
                styleForFS = m_pageStyle;
                type = KWord::OddPagesHeaderTextFrameSet; break;
            case 4: // first footer
                if (! m_firstPageStyle.isValid())
                    return; // we don't need this FS.
                styleForFS = m_firstPageStyle;
                type = KWord::OddPagesFooterTextFrameSet; break;
            case 5: // even footer
                styleForFS = m_pageStyle;
                type = KWord::EvenPagesFooterTextFrameSet; break;
            case 6: // odd footer
                styleForFS = m_pageStyle;
                type = KWord::OddPagesFooterTextFrameSet; break;
            case 7: // footnote
                 // FS will be deleted soon...
            default:
                type = KWord::OtherTextFrameSet; break;
            }
            KWTextFrameSet *fs = new KWTextFrameSet(m_document, type);
            fs->setAllowLayout(false);
            fs->setName(fsname);
            fs->setPageStyle(styleForFS);
            fill(fs, framesetElem);
            m_document->addFrameSet(fs);

            // Old file format had autoCreateNewFrame as a frameset attribute
            if (framesetElem.hasAttribute("autoCreateNewFrame")) {
                KWord::FrameBehavior behav;
                switch (framesetElem.attribute("autoCreateNewFrame").toInt()) {
                case 1: behav = KWord::AutoCreateNewFrameBehavior; break;
                case 2: behav = KWord::IgnoreContentFrameBehavior; break;
                default: behav = KWord::AutoExtendFrameBehavior; break;
                }
                foreach (KWFrame *frame, fs->frames())
                    frame->setFrameBehavior(behav);
            }
            return;
        }
    }
    case 5: { // FT_CLIPART
        kError(32001) << "FT_CLIPART used! (in KWDocument::loadFrameSet)";
        // Do not break!
    }
    case 2: { // FT_PICTURE
        KoXmlElement frame = framesetElem.namedItem("FRAME").toElement();
        if (frame.isNull())
            return;
        KoXmlElement image = framesetElem.namedItem("IMAGE").toElement();
        if (image.isNull())
            image = framesetElem.namedItem("PICTURE").toElement();
        if (image.isNull())
            return;
        KoXmlElement key = image.namedItem("KEY").toElement();
        if (key.isNull())
            return;

        ImageKey imageKey;
        fill(&imageKey, key);
        if (imageKey.filename.isEmpty()) {
            kWarning(32001) << "could not find image in the store";
            return;
        }

        KWFrameSet *fs = new KWFrameSet();
        fill(fs, framesetElem);
        fs->setName(fsname);

        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("PictureShape");
        Q_ASSERT(factory);
        KoShape *shape = factory->createDefaultShape(m_document->resourceManager());
        shape->setKeepAspectRatio(image.attribute("keepAspectRatio", "true") == "true");

        KoImageCollection *collection = m_document->resourceManager()->imageCollection();
        Q_ASSERT(collection);
        KoImageData *data = collection->createImageData(imageKey.filename, m_store);
        shape->setUserData(data);

        KWFrame *f = new KWFrame(shape, fs);
        fill(f, frame);
        m_document->addFrameSet(fs);
        return;
    }
    case 4: { //FT_FORMULA
#if 0
        KWFormulaFrameSet *fs = new KWFormulaFrameSet(this, fsname);
        fs->load(framesetElem, loadFrames);
        addFrameSet(fs, false);
        return;
#endif
        // TODO support old formula frameset
        return;
    }
    // Note that FT_PART cannot happen when loading from a file (part frames are saved into the SETTINGS tag)
    // and FT_TABLE can't happen either.
    case 3: // FT_PART
        kWarning(32001) << "loadFrameSet: FT_PART: impossible case";
        return;
    case 10: // FT_TABLE
        kWarning(32001) << "loadFrameSet: FT_TABLE: impossible case";
        return;
    case 0: // FT_BASE
        kWarning(32001) << "loadFrameSet: FT_BASE !?!?";
        return;
    default: // other
        kWarning(32001) << "loadFrameSet error: unknown type, skipping";
        return;
    }
}

void KWDLoader::fill(KWFrameSet *fs, const KoXmlElement &framesetElem)
{
    if (framesetElem.hasAttribute("visible")) {
        bool visible = framesetElem.attribute("visible").toLower() == "true";
        foreach (KWFrame *frame, fs->frames()) {
            frame->shape()->setVisible(visible);
        }
    }
    if (framesetElem.hasAttribute("protectSize")) {
        bool protectSize = framesetElem.attribute("protectSize").toLower() == "true";
        foreach (KWFrame *frame, fs->frames()) {
            frame->shape()->setGeometryProtected(protectSize);
        }
    }

}

void KWDLoader::fill(KWTextFrameSet *fs, const KoXmlElement &framesetElem)
{
    fill(static_cast<KWFrameSet*>(fs), framesetElem);
    // <FRAME>
    KoXmlElement frameElem;
    forEachElement(frameElem, framesetElem) {
        if (frameElem.tagName() == "FRAME") {
            KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
            Q_ASSERT(factory);
            KoShape *shape = factory->createDefaultShape(m_document->resourceManager());
            KWTextFrame *frame = new KWTextFrame(shape, fs);
            fill(frame, frameElem);

            //m_doc->progressItemLoaded();
        }
    }


    //m_info = static_cast<KWFrameSet::Info>(KWDocument::getAttribute(framesetElem, "frameInfo", KWFrameSet::FI_BODY)); // TODO
    switch (framesetElem.attribute("frameInfo").toInt()) {
    case 0: ;
    }
    if (framesetElem.hasAttribute("protectContent") && fs->frameCount())
        fs->frames().first()->shape()->setContentProtected((bool)framesetElem.attribute("protectContent").toInt());

    QTextCursor cursor(fs->document());
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();

    // <PARAGRAPH>
    bool firstParag = true;
    KoXmlElement paragraph;
    forEachElement(paragraph, framesetElem) {
        if (paragraph.tagName() == "PARAGRAPH") {
            if (! firstParag) {
                QTextBlockFormat emptyTbf;
                QTextCharFormat emptyCf;
                cursor.insertBlock(emptyTbf, emptyCf);
            }

            KoStyleManager *styleManager = m_document->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
            Q_ASSERT(styleManager);

            firstParag = false;
            KoXmlElement layout = paragraph.namedItem("LAYOUT").toElement();
            if (!layout.isNull()) {
                QString styleName = layout.namedItem("NAME").toElement().attribute("value");
                KoParagraphStyle *style = styleManager->paragraphStyle(styleName);
                if (!style)
                    style = styleManager->defaultParagraphStyle();
                KoParagraphStyle paragStyle;  // tmp style
                paragStyle.copyProperties(style);
                fill(&paragStyle, layout);

                QTextBlock block = cursor.block();
                paragStyle.applyStyle(block);
                if (style->listStyle() && paragStyle.listStyle()) {
                    Q_ASSERT(block.textList());
                    // this parag has a parag specific list.  Lets see if we can merge it with
                    // previous ones.
                    const int level = paragStyle.listLevel();
                    const QTextFormat format = block.textList()->format();
                    QTextBlock prev = block.previous();
                    bool merge = false;
                    for (; prev.isValid(); prev = prev.previous()) {
                        if (! prev.textList())
                            continue;
                        QTextFormat prevFormat = prev.textList()->format();
                        if (prevFormat.intProperty(KoListStyle::Level) == level) {
                            if (format == prevFormat)
                                merge = true;
                            break;
                        } else if (prevFormat.intProperty(KoListStyle::Level) < level)
                            break;
                    }
                    if (merge) {
                        Q_ASSERT(block.textList()->count() == 1);
                        block.textList()->remove(block);
                        Q_ASSERT(block.textList() == 0);
                        Q_ASSERT(prev.textList());
                        prev.textList()->add(block);
                    }
                }
            }
            cursor.insertText(paragraph.namedItem("TEXT").toElement().text().replace('\n', QChar(0x2028)));

            // re-apply char format after we added the text
            KoCharacterStyle *style = styleManager->characterStyle(
                                          cursor.blockCharFormat().intProperty(KoCharacterStyle::StyleId));
            if (style) {
                QTextBlock block = cursor.block();
                style->applyStyle(block);
            }

            KoXmlElement formats = paragraph.namedItem("FORMATS").toElement();
            if (!formats.isNull()) {
                KoCharacterStyle defaultStyle;
                if (style == 0) // parag is not based on any style, just text.
                    style = &defaultStyle;

                QTextBlock block = cursor.block();
                KoXmlElement format;
                forEachElement(format, formats) {
                    if (format.tagName() != "FORMAT")
                        continue;
                    QString id = format.attribute("id", "0");
                    QTextCursor formatCursor(block) ;
                    int pos = format.attribute("pos", "-1").toInt();
                    if (format.hasAttribute("pos") && pos >= 0) {
                        int length = format.attribute("len").toInt();
                        if (length > 0) {
                            formatCursor.setPosition(block.position() + pos);
                            formatCursor.setPosition(block.position() + pos + length, QTextCursor::KeepAnchor);
                        } else {
                            kWarning(32001) << "Format has missing or invalid 'len' value, ignoring";
                            continue;
                        }
                    } else {
                        kWarning(32001) << "Format has missing or invalid 'pos' value, ignoring";
                        continue;
                    }
                    if (id == "1") {
                        KoCharacterStyle s2;
                        s2.copyProperties(style);
                        fill(&s2, format);
                        s2.applyStyle(&formatCursor);
                    } else if (id == "2") {
                        kWarning(32001) << "File to old, image can not be recovered";
                    } else if (id == "4") {
                        KoXmlElement variable = format.namedItem("VARIABLE").toElement();
                        if (variable.isNull()) {
                            kWarning(32001) << "Missing VARIABLE tag";
                            continue;
                        }
                        KoXmlElement type = variable.namedItem("TYPE").toElement();
                        int typeId = type.attribute("type", "1").toInt();
                        switch (typeId) {
                        case 11: { // footnote
                            KoXmlElement footnote = variable.namedItem("FOOTNOTE").toElement();
                            KoInlineNote *note = new KoInlineNote(KoInlineNote::Footnote);
                            note->setLabel(footnote.attribute("value"));
                            note->setAutoNumbering(footnote.attribute("numberingtype", "auto") == "auto");
                            note->setText(i18n("Unable to locate footnote text"));
                            KoTextDocumentLayout *layout = qobject_cast<KoTextDocumentLayout*>(
                                    fs->document()->documentLayout());
                            Q_ASSERT(layout);
                            Q_ASSERT(layout->inlineTextObjectManager());
                            layout->inlineTextObjectManager()->insertInlineObject(formatCursor, note);
                            NotesData notesData;
                            notesData.note = note;
                            notesData.frameSetName = footnote.attribute("frameset");
                            m_notes.append(notesData);
                            break;
                        }
                        case 10: { // note
                            KoXmlElement footEndNote = variable.namedItem("NOTE").toElement();

                            KoInlineNote::Type type = footEndNote.attribute("notetype") == "footnote"
                                        ? KoInlineNote::Footnote : KoInlineNote::Endnote;
                            KoInlineNote *note = new KoInlineNote(type);
                            note->setLabel(footEndNote.attribute("value"));
                            note->setAutoNumbering(footEndNote.attribute("numberingtype", "auto") == "auto");
                            note->setText(i18n("Unable to locate note-text"));
                            KoTextDocumentLayout *layout = qobject_cast<KoTextDocumentLayout*>(
                                    fs->document()->documentLayout());
                            Q_ASSERT(layout);
                            Q_ASSERT(layout->inlineTextObjectManager());
                            layout->inlineTextObjectManager()->insertInlineObject(formatCursor, note);
                            NotesData notesData;
                            notesData.note = note;
                            notesData.frameSetName = footEndNote.attribute("frameset");
                            m_notes.append(notesData);
                            break;
                        }
                        case 0: // date TODO
                        case 2: // fixed time TODO
                        case 4: // page number TODO
                        case 8: // field TODO
                        case 9: // link TODO
                        case 12: // statistic TODO
                        default: {
                            QString replacementText = type.attribute("text");
                            formatCursor.insertText(replacementText);
                        }
                        }
                    } else if (id == "5") {
                        kWarning(32001) << "File to old, footnote can not be recovered";
                    } else if (id == "6") { // anchor for floating frame.
                        KoXmlElement anchor = format.namedItem("ANCHOR").toElement();
                        if (anchor.isNull()) {
                            kWarning(32001) << "Missing ANCHOR tag";
                            continue;
                        }
                        QString type = anchor.attribute("type", "frameset");
                        if (type == "frameset") {
                            if (! anchor.hasAttribute("instance")) {
                                kWarning(32001) << "ANCHOR is missing its instance attribute";
                                continue;
                            }
                            AnchorData data;
                            data.textShape = fs->frames().first()->shape();
                            data.cursorPosition = formatCursor.anchor();
                            data.document = fs->document();
                            data.frameSetName = anchor.attribute("instance");
                            m_anchors.append(data);
                        } else {
                            // TODO
                        }
                    }
                }
            }
            //m_doc->progressItemLoaded();
        }
    }
}

void KWDLoader::fill(KoParagraphStyle *style, const KoXmlElement &layout)
{
    QString align = layout.namedItem("FLOW").toElement().attribute("align", "auto");
    if (align == "left") {
        style->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    } else if (align == "right") {
        style->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    } else if (align == "center") {
        style->setAlignment(Qt::AlignCenter | Qt::AlignAbsolute);
    } else if (align == "justify") {
        style->setAlignment(Qt::AlignJustify);
    } else {
        style->setAlignment(Qt::AlignLeft);
    }

    KoXmlElement element = layout.namedItem("INDENTS").toElement();
    if (!element.isNull()) {
        style->setTextIndent(element.attribute("first").toDouble());
        style->setLeftMargin(element.attribute("left").toDouble());
        style->setRightMargin(element.attribute("right").toDouble());
    }
    element = layout.namedItem("OFFSETS").toElement();
    if (!element.isNull()) {
        style->setTopMargin(element.attribute("before").toDouble());
        style->setBottomMargin(element.attribute("after").toDouble());
    }
    element = layout.namedItem("LINESPACING").toElement();
    if (!element.isNull()) {
        QString type = element.attribute("type", "fixed");
        qreal spacing = element.attribute("spacingValue").toDouble();
        if (type == "oneandhalf")
            style->setLineHeightPercent(150);
        else if (type == "double")
            style->setLineHeightPercent(200);
        else if (type == "custom") {
            if (spacing == 0.0) {
                // see if kword 1.1 compatibility is needed
                if (element.attribute("value") == "double")
                    style->setLineHeightPercent(200);
                else if (element.attribute("value") == "oneandhalf")
                    style->setLineHeightPercent(150);
            } else
                style->setLineSpacing(spacing);
        } else if (type == "atleast")
            style->setMinimumLineHeight(spacing);
        else if (type == "multiple")
            style->setLineHeightPercent(qRound(100 * spacing));
        else if (type == "fixed")
            style->setLineHeightAbsolute(spacing);
    }

    element = layout.namedItem("PAGEBREAKING").toElement();
    if (!element.isNull()) {
        if (element.attribute("linesTogether") == "true")
            style->setNonBreakableLines(true);
        if (element.attribute("hardFrameBreak") == "true")
            style->setBreakBefore(true);
        if (element.attribute("hardFrameBreakAfter") == "true")
            style->setBreakAfter(true);
    }
    element = layout.namedItem("HARDBRK").toElement();   // KWord-0.8
    if (!element.isNull())
        style->setBreakBefore(true);
    element = layout.namedItem("COUNTER").toElement();
    if (!element.isNull()) {
        KoListStyle *orig = style->listStyle();
        KoListStyle *lstyle = orig ? orig : new KoListStyle(style);

        KoListLevelProperties llp = lstyle->levelProperties(element.attribute("depth").toInt() + 1);

        int type = element.attribute("type").toInt();
        switch (type) {
        case 1: llp.setStyle(KoListStyle::DecimalItem); break;
        case 2: llp.setStyle(KoListStyle::AlphaLowerItem); break;
        case 3: llp.setStyle(KoListStyle::UpperAlphaItem); break;
        case 4: llp.setStyle(KoListStyle::RomanLowerItem); break;
        case 5: llp.setStyle(KoListStyle::UpperRomanItem); break;
        case 6: {
            llp.setStyle(KoListStyle::CustomCharItem);
            QChar character(element.attribute("bullet", QString::number('*')).toInt());
            llp.setBulletCharacter(character);
            break;
        }
        case 8: llp.setStyle(KoListStyle::CircleItem); break;
        case 9: llp.setStyle(KoListStyle::SquareItem); break;
        case 10: llp.setStyle(KoListStyle::DiscItem); break;
        case 11: llp.setStyle(KoListStyle::BoxItem); break;
        case 7: llp.setStyle(KoListStyle::CustomCharItem);
            kWarning(32001) << "According to spec COUNTER with type 7 is not supported, ignoring";
            // fall through
        default: {
            lstyle = 0;
        }
        }
        if (lstyle) { // was a valid type
            llp.setStartValue(element.attribute("start", "1").toInt());
            llp.setListItemPrefix(element.attribute("lefttext"));
            llp.setListItemSuffix(element.attribute("righttext"));
            llp.setDisplayLevel(element.attribute("display-levels").toInt());
            switch (element.attribute("align", "0").toInt()) {
            case 0: llp.setAlignment(Qt::AlignLeading); break; // align = auto
            case 1: llp.setAlignment(Qt::AlignAbsolute | Qt::AlignLeft); break; // align = left
            case 2: llp.setAlignment(Qt::AlignAbsolute | Qt::AlignRight); break; // align = right
            }
            if (element.attribute("restart", "false") == "true")
                style->setRestartListNumbering(true);
            style->setListLevel(llp.level());
            lstyle->setLevelProperties(llp);
        }
        style->setListStyle(lstyle);
    }

    class BorderConverter
    {
    public:
        BorderConverter(const KoXmlElement &element) {
            width = element.attribute("width").toInt(),
                    innerWidth = 0.0;
            spacing = 0.0;
            switch (element.attribute("style").toInt()) {
            case 0: borderStyle = KoBorder::BorderSolid; break;
            case 1: borderStyle = KoBorder::BorderDashed; break;
            case 2: borderStyle = KoBorder::BorderDotted; break;
            case 3: borderStyle = KoBorder::BorderDashDotPattern; break;
            case 4: borderStyle = KoBorder::BorderDashDotDotPattern; break;
            case 5:
                borderStyle = KoBorder::BorderDouble;
                spacing = width;
                innerWidth = width;
                break;
            }
        }
        qreal width, innerWidth, spacing;
        KoBorder::BorderStyle borderStyle;
    };
    element = layout.namedItem("LEFTBORDER").toElement();
    if (!element.isNull()) {
        style->setLeftBorderColor(colorFrom(element));
        BorderConverter bc(element);
        style->setLeftBorderWidth(bc.width);
        style->setLeftBorderStyle(bc.borderStyle);
        if (bc.spacing > 0.0) {
            style->setLeftInnerBorderWidth(bc.innerWidth);
            style->setLeftBorderSpacing(bc.spacing);
        }
    }
    element = layout.namedItem("RIGHTBORDER").toElement();
    if (!element.isNull()) {
        style->setRightBorderColor(colorFrom(element));
        BorderConverter bc(element);
        style->setRightBorderWidth(bc.width);
        style->setRightBorderStyle(bc.borderStyle);
        if (bc.spacing > 0.0) {
            style->setRightInnerBorderWidth(bc.innerWidth);
            style->setRightBorderSpacing(bc.spacing);
        }
    }
    element = layout.namedItem("TOPBORDER").toElement();
    if (!element.isNull()) {
        style->setTopBorderColor(colorFrom(element));
        BorderConverter bc(element);
        style->setTopBorderWidth(bc.width);
        style->setTopBorderStyle(bc.borderStyle);
        if (bc.spacing > 0.0) {
            style->setTopInnerBorderWidth(bc.innerWidth);
            style->setTopBorderSpacing(bc.spacing);
        }
    }
    element = layout.namedItem("BOTTOMBORDER").toElement();
    if (!element.isNull()) {
        style->setBottomBorderColor(colorFrom(element));
        BorderConverter bc(element);
        style->setBottomBorderWidth(bc.width);
        style->setBottomBorderStyle(bc.borderStyle);
        if (bc.spacing > 0.0) {
            style->setBottomInnerBorderWidth(bc.innerWidth);
            style->setBottomBorderSpacing(bc.spacing);
        }
    }

    // TODO read rest of properties
    // FORMAT
    // TABULATOR

    // OHEAD, OFOOT, IFIRST, ILEFT
}

QColor KWDLoader::colorFrom(const KoXmlElement &element)
{
    QColor color(element.attribute("red").toInt(),
                 element.attribute("green").toInt(),
                 element.attribute("blue").toInt());
    return color;
}

void KWDLoader::fill(KoCharacterStyle *style, const KoXmlElement &formatElem)
{
    KoXmlElement element = formatElem.namedItem("COLOR").toElement();
    if (!element.isNull()) {
        QBrush fg = style->foreground();
        QColor c = colorFrom(element);
        if (c.isValid())
            fg.setColor(c);
        style->setForeground(fg);
    }
    element = formatElem.namedItem("FONT").toElement();
    if (!element.isNull())
        style->setFontFamily(element.attribute("name", "Serif"));
    element = formatElem.namedItem("SIZE").toElement();
    if (!element.isNull())
        style->setFontPointSize(element.attribute("value", "12").toDouble());
    element = formatElem.namedItem("WEIGHT").toElement();
    if (!element.isNull())
        style->setFontWeight(element.attribute("value", "80").toInt());
    element = formatElem.namedItem("ITALIC").toElement();
    if (!element.isNull())
        style->setFontItalic(element.attribute("value", "0") == "1");
    element = formatElem.namedItem("STRIKEOUT").toElement();
    if (!element.isNull()) {
        QString value = element.attribute("value", "0");
        // TODO store other properties
        if (value != "0")
            style->setStrikeOutStyle(KoCharacterStyle::SolidLine);
    }
    element = formatElem.namedItem("UNDERLINE").toElement();
    if (!element.isNull()) {
        KoCharacterStyle::LineStyle underline = KoCharacterStyle::NoLineStyle;
        QString value = element.attribute("value", "0"); // "0" is NoUnderline
        if (value == "1" || value == "single")
            style->setUnderlineType(KoCharacterStyle::SingleLine);
        else if (value == "double")
            style->setUnderlineType(KoCharacterStyle::DoubleLine);
        else if (value == "single-bold")
            style->setUnderlineType(KoCharacterStyle::SingleLine);   // TODO support single-bold underline!
        else if (value == "wave") {
            style->setUnderlineType(KoCharacterStyle::SingleLine);
            underline = KoCharacterStyle::WaveLine;
        }

        QString type = element.attribute("styleline", "solid");
        if (type == "solid")
            underline = KoCharacterStyle::SolidLine;
        else if (type == "dash")
            underline = KoCharacterStyle::DashLine;
        else if (type == "dot")
            underline = KoCharacterStyle::DottedLine;
        else if (type == "dashdot")
            underline = KoCharacterStyle::DotDashLine;
        else if (type == "dashdotdot")
            underline = KoCharacterStyle::DotDotDashLine;

        //style->setFontUnderline(underline != QTextCharFormat::NoUnderline);
        style->setUnderlineStyle((KoCharacterStyle::LineStyle) underline);
    }
    element = formatElem.namedItem("TEXTBACKGROUNDCOLOR").toElement();
    if (!element.isNull()) {
        QColor background = colorFrom(element);
        if (background.isValid())
            style->setBackground(QBrush(background));
        else
            style->setBackground(Qt::NoBrush);
    }

    //VERTALIGN
    //SHADOW
    //FONTATTRIBUTE
    //LANGUAGE
    //OFFSETFROMBASELINE
}

void KWDLoader::fill(KWFrame *frame, const KoXmlElement &frameElem)
{
    Q_ASSERT(frame);
    Q_ASSERT(frame->shape());
    QPointF origin(frameElem.attribute("left").toDouble(),
                   frameElem.attribute("top").toDouble());
    QSizeF size(frameElem.attribute("right").toDouble() - origin.x(),
                frameElem.attribute("bottom").toDouble() - origin.y());

    // increase offset of each frame to account for the padding.
    qreal pageHeight = m_pageManager->defaultPageStyle().pageLayout().height;
    Q_ASSERT(pageHeight); // can not be 0
    qreal offset = (int)(origin.y() / pageHeight) * (m_pageManager->padding().top + m_pageManager->padding().bottom);
    origin.setY(origin.y() + offset);

    frame->shape()->setPosition(origin);
    frame->shape()->setSize(size);

    QColor background(frameElem.attribute("bkRed", "255").toInt(),
                      frameElem.attribute("bkGreen", "255").toInt(),
                      frameElem.attribute("bkBlue", "255").toInt());
    Qt::BrushStyle bs = static_cast<Qt::BrushStyle>(frameElem.attribute("bkStyle", "1").toInt());

    frame->shape()->setBackground(new KoColorBackground(background, bs));

    switch (frameElem.attribute("runaround", "0").toInt()) {
    case 0:
        frame->setTextRunAround(KWord::RunThrough);
        break;
    case 2:
        frame->setTextRunAround(KWord::NoRunAround);
        break;
    default:
        frame->setTextRunAround(KWord::RunAround);
        break;
    }

    QString side = frameElem.attribute("runaroundSide", "biggest");
    if (side == "left")
        frame->setRunAroundSide(KWord::LeftRunAroundSide);
    else if (side == "right")
        frame->setRunAroundSide(KWord::RightRunAroundSide);

    int zIndex = frameElem.attribute("z-index", "0").toInt();

    KWTextFrame *tf = dynamic_cast<KWTextFrame*>(frame);
    if (tf) {
        if (zIndex <= 0 && static_cast<KWTextFrameSet*>(tf->frameSet())->textFrameSetType() == KWord::OtherTextFrameSet)
            zIndex = 1; // OtherTextFrameSet types always live on top of the main frames.
        KoTextShapeData *textShapeData = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
        Q_ASSERT(textShapeData);
        KoInsets margins;
        margins.left = frameElem.attribute("bleftpt", "0.0").toDouble();
        margins.right = frameElem.attribute("brightpt", "0.0").toDouble();
        margins.top = frameElem.attribute("btoppt", "0.0").toDouble();
        margins.bottom = frameElem.attribute("bbottompt", "0.0").toDouble();

        textShapeData->setShapeMargins(margins);
    } else
        zIndex = qMax(zIndex, 1); // non-text types always live on top of the main frames.

    frame->shape()->setZIndex(zIndex);
}

void KWDLoader::fill(ImageKey *key, const KoXmlElement &keyElement)
{
    key->year = keyElement.attribute("year");
    key->month = keyElement.attribute("month");
    key->day = keyElement.attribute("day");
    key->hour = keyElement.attribute("hour");
    key->minute = keyElement.attribute("minute");
    key->second = keyElement.attribute("second");
    key->milisecond = keyElement.attribute("msec");
    key->oldFilename = keyElement.attribute("filename");
    key->filename = keyElement.attribute("name");

    if (key->filename.isEmpty()) {
        foreach (const ImageKey &storedKey, m_images) {
            if (storedKey.year == key->year && storedKey.oldFilename == key->oldFilename &&
                    storedKey.month == key->month && storedKey.day == key->day &&
                    storedKey.hour == key->hour && storedKey.minute == key->minute &&
                    storedKey.second == key->second && storedKey.year == key->year &&
                    storedKey.milisecond == key->milisecond) {
                key->filename = storedKey.filename;
                return;
            }
        }
    }
}

void KWDLoader::loadStyleTemplates(const KoXmlElement &stylesElem)
{
    KoStyleManager *styleManager = m_document->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();

    Q_ASSERT(styleManager);

    KoXmlElement style;
    forEachElement(style, stylesElem) {
        if (style.tagName() != "STYLE")
            continue;
        QString styleName = style.namedItem("NAME").toElement().attribute("value");
        KoParagraphStyle *paragStyle = styleManager->paragraphStyle(styleName);
        if (!paragStyle) {
            paragStyle = new KoParagraphStyle();
            paragStyle->setName(styleName);
            styleManager->add(paragStyle);
        }
        fill(paragStyle, style);
#if 0
        if (m_syntaxVersion < 3) {
            // Convert old style (up to 1.2.x included)
            // "include in TOC if chapter numbering" to the new attribute
            if (sty->paragLayout().counter && sty->paragLayout().counter->numbering() == KoParagCounter::NUM_CHAPTER)
                sty->setOutline(true);
        }
#endif
        KoXmlElement format = style.namedItem("FORMAT").toElement();
        if (! format.isNull())
            fill(paragStyle->characterStyle(), format);
    }

    // second pass, to set the 'following'
    forEachElement(style, stylesElem) {
        if (style.tagName() != "STYLE")
            continue;
        QString styleName = style.namedItem("NAME").toElement().attribute("value");
        KoParagraphStyle *paragStyle = styleManager->paragraphStyle(styleName);
        Q_ASSERT(paragStyle);
        QString following = style.namedItem("FOLLOWING").toElement().attribute("name");
        KoParagraphStyle *next = styleManager->paragraphStyle(following);
        if (next)
            paragStyle->setNextStyle(next->styleId());
    }
}

void KWDLoader::insertAnchors()
{
    foreach (const AnchorData &anchor, m_anchors) {
        KWFrameSet *fs = m_document->frameSetByName(anchor.frameSetName);
        if (fs == 0) {
            kWarning(32001) << "Anchored frameset not found: '" << anchor.frameSetName;
            continue;
        }
        if (fs->frames().count() == 0)  continue;
        KWFrame *frame = fs->frames().first();
        frame->shape()->setPosition(QPointF(0, 0));
        KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(anchor.textShape);
        Q_ASSERT(container);
        if (! container) continue;
        container->addShape(frame->shape());   // attach here & avoid extra layouts
        KoTextAnchor *textAnchor = new KoTextAnchor(frame->shape());
        QTextCursor cursor(anchor.document);
        cursor.setPosition(anchor.cursorPosition);
        cursor.setPosition(anchor.cursorPosition + 1, QTextCursor::KeepAnchor);
        KoTextDocumentLayout *layout = qobject_cast<KoTextDocumentLayout*>(cursor.block().document()->documentLayout());
        Q_ASSERT(layout);
        Q_ASSERT(layout->inlineTextObjectManager());
        layout->inlineTextObjectManager()->insertInlineObject(cursor, textAnchor);
    }
    m_anchors.clear();
}

void KWDLoader::insertNotes()
{
    foreach (const NotesData &note, m_notes) {
        KWFrameSet *fs = m_document->frameSetByName(note.frameSetName);
        if (fs == 0) {
            kWarning(32001) << "Frameset data for note not found: '" << note.frameSetName;
            continue;
        }
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs && tfs->document()) {
            note.note->setText(tfs->document()->toPlainText());
//kDebug(32001) << "setting the text to" << note.note->text();
        }
        m_document->removeFrameSet(fs);
        delete fs; // we don't want to keep it around.
    }
    m_notes.clear();
}
