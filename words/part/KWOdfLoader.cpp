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
#include "KWPage.h"
#include "KWPageManager.h"
#include "frames/KWTextFrameSet.h"
#include "WordsDebug.h"

// calligra
#include <KoOdfStylesReader.h>
#include <KoOasisSettings.h>
#include <KoOdfReadStore.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoShapeFactoryBase.h>
#include <KoTextShapeData.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoShapeLoadingContext.h>
#include <KoStyleManager.h>
#include <KoOdfLoadingContext.h>
#include <KoUpdater.h>
#include <KoProgressUpdater.h>
#include <KoVariableManager.h>
#include <KoInlineTextObjectManager.h>
#include <KoUnit.h>
#include <KoSectionModel.h>

#ifdef SHOULD_BUILD_RDF
#include <KoDocumentRdf.h>
#endif

// Qt includes
#include <QTextCursor>

#include <KoDocumentRdfBase.h>

KWOdfLoader::KWOdfLoader(KWDocument *document)
        : QObject(document),
        m_document(document)
{
    connect(this, SIGNAL(progressUpdate(int)), m_document, SIGNAL(sigProgress(int)));
}

KWOdfLoader::~KWOdfLoader()
{
}

KWDocument *KWOdfLoader::document() const
{
    return m_document;
}

//1.6: KWDocument::loadOasis
bool KWOdfLoader::load(KoOdfReadStore &odfStore)
{
    //debugWords << "========================> KWOdfLoader::load START";

    QPointer<KoUpdater> updater;
    QPointer<KoUpdater> loadUpdater;
    if (m_document->progressUpdater()) {
        updater = m_document->progressUpdater()->startSubtask(1, "KWOdfLoader::load");
        loadUpdater = m_document->progressUpdater()->startSubtask(5, "KWOdfLoader::loadOdf");
        updater->setProgress(0);
        loadUpdater->setProgress(0);
    }

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        errorWords << "No office:body found!" << endl;
        m_document->setErrorMessage(i18n("Invalid OASIS OpenDocument file. No office:body tag found."));
        return false;
    }

    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "text");
    if (body.isNull()) {
        errorWords << "No office:text found!" << endl;
        KoXmlElement childElem;
        QString localName;
        forEachElement(childElem, realBody)
            localName = childElem.localName();
        if (localName.isEmpty())
            m_document->setErrorMessage(i18n("Invalid OASIS OpenDocument file. No tag found inside office:body."));
        else
            m_document->setErrorMessage(i18n("This is not a word processing document, but %1. Please try opening it with the appropriate application.", KoDocument::tagNameToDocumentType(localName)));
        return false;
    }

    // Load attributes from the office:text.  These are text:global and text:use-soft-page-breaks.
    QString textGlobal = body.attributeNS(KoXmlNS::text, "global");
    bool isTextGlobal = (textGlobal == "true");
    if (isTextGlobal) {
        m_document->setIsMasterDocument(true);
    }
    // FIXME: text:use-soft-page-breaks

    if (updater) updater->setProgress(20);

    KoOdfLoadingContext odfContext(odfStore.styles(), odfStore.store(), QLatin1String("calligrawords/styles/"));
    KoShapeLoadingContext sc(odfContext, m_document->resourceManager());
    sc.setDocumentRdf(m_document->documentRdf());

    // Load user defined variable declarations
    if (KoVariableManager *variableManager = m_document->inlineTextObjectManager()->variableManager()) {
        variableManager->loadOdf(body);
    }

    // Load all styles before the corresponding paragraphs try to use them!
    KWOdfSharedLoadingData *sharedData = new KWOdfSharedLoadingData(this);
    sc.addSharedData(KOTEXT_SHARED_LOADING_ID, sharedData);
    KoStyleManager *styleManager = m_document->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(styleManager);
    sharedData->loadOdfStyles(sc, styleManager);

    if (updater) updater->setProgress(40);

    loadMasterPageStyles(sc);

    // add page background frame set
    KWFrameSet *pageBackgroundFrameSet = new KWFrameSet(Words::BackgroundFrameSet);
    m_document->addFrameSet(pageBackgroundFrameSet);

#if 0 //1.6:
    KWOasisLoader oasisLoader(this);
    // <text:page-sequence> oasis extension for DTP (2003-10-27 post by Daniel)
    m_processingType = (!KoXml::namedItemNS(body, KoXmlNS::text, "page-sequence").isNull()) ? DTP : WP;
    m_hasTOC = false;
    m_tabStop = MM_TO_POINT(15);
    const KoXmlElement *defaultParagStyle = styles.defaultStyle("paragraph");
    if (defaultParagStyle) {
        KoStyleStack stack;
        stack.push(*defaultParagStyle);
        stack.setTypeProperties("paragraph");
        QString tabStopVal = stack.property(KoXmlNS::style, "tab-stop-distance");
        if (!tabStopVal.isEmpty()) m_tabStop = KoUnit::parseValue(tabStopVal);
    }
    m_initialEditing = 0;
    // TODO MAILMERGE
    // Variable settings
    // By default display real variable value
    if (!isReadWrite())
        m_varColl->variableSetting()->setDisplayFieldCode(false);
#endif

    // Load all styles before the corresponding paragraphs try to use them!
#if 0 //1.6:
    if (m_frameStyleColl->loadOasisStyles(context) == 0) {
        // no styles loaded -> load default styles
        loadDefaultFrameStyleTemplates();
    }
    if (m_tableStyleColl->loadOasisStyles(context, *m_styleColl, *m_frameStyleColl) == 0) {
        // no styles loaded -> load default styles
        loadDefaultTableStyleTemplates();
    }
    static_cast<KWVariableSettings *>(m_varColl->variableSetting())->loadNoteConfiguration(styles.officeStyle());
    loadDefaultTableTemplates();
//#else
    /*
    // We always needs at least one valid default paragraph style
    KoParagraphStyle *defaultParagraphStyle = m_document->styleManager()->defaultParagraphStyle();
    //const KoXmlElement *defaultParagraphStyle = context.stylesReader().defaultStyle("paragraph");
    //if(! defaultParagraphStyle) {
    KoParagraphStyle *parastyle = new KoParagraphStyle();
    parastyle->setName("Standard");
    m_document->styleManager()->add(parastyle);
    context.styleStack().setTypeProperties("paragraph"); // load all style attributes from "style:paragraph-properties"
    parastyle->loadOasis(context.styleStack()); // load the KoParagraphStyle from the stylestack
    KoCharacterStyle *charstyle = parastyle->characterStyle();
    context.styleStack().setTypeProperties("text"); // load all style attributes from "style:text-properties"
    charstyle->loadOasis(context.styleStack()); // load the KoCharacterStyle from the stylestack
    //}
    */
#endif

    // load text:page-sequence
    KoXmlElement pageSequence = KoXml::namedItemNS(body, KoXmlNS::text, "page-sequence");
    if (! pageSequence.isNull()) {
        KWPageManager *pageManager = m_document->pageManager();
        KoXmlElement page;
        forEachElement(page, pageSequence) {
            if (page.namespaceURI() == KoXmlNS::text && page.localName() == "page") {
                QString master = page.attributeNS(KoXmlNS::text, "master-page-name", QString());
                pageManager->appendPage(pageManager->pageStyle(master));
            }
        }
    }

    if (updater) updater->setProgress(50);

    KoTextShapeData textShapeData;
    KWTextFrameSet *mainFs = new KWTextFrameSet(m_document, Words::MainTextFrameSet);
    mainFs->setPageStyle(m_document->pageManager()->pageStyle("Standard"));
    m_document->addFrameSet(mainFs);
    textShapeData.setDocument(mainFs->document(), false);
    sc.setSectionModel(new KoSectionModel(mainFs->document()));

    // disable the undo recording during load so the kotexteditor is in sync with
    // the app's undostack
    textShapeData.document()->setUndoRedoEnabled(false);

    if (updater) updater->setProgress(60);

    // load the main text shape right here so we can use the progress information of the KoTextLoader
    KoTextLoader loader(sc);
    QTextCursor cursor(textShapeData.document());

    if (loadUpdater) {
        connect(&loader, SIGNAL(sigProgress(int)), loadUpdater, SLOT(setProgress(int)));
    }

    loader.loadBody(body, cursor);   // now let's load the body from the ODF KoXmlElement.

    sharedData->connectFlowingTextShapes();

    if (loadUpdater) {
        loadUpdater->setProgress(100);
    }

    //reenable the undo recording
    textShapeData.document()->setUndoRedoEnabled(true);

    KoTextEditor *editor = KoTextDocument(textShapeData.document()).textEditor();
    if (editor) // at one point we have to get the position from the odf doc instead.
        editor->setPosition(0);

    if (updater) updater->setProgress(90);

    // Grab weak references to all the Rdf stuff that was loaded
    if (KoDocumentRdfBase *rdf = m_document->documentRdf()) {
        rdf->updateInlineRdfStatements(textShapeData.document());
    }

    if (updater) updater->setProgress(95);

    loadSettings(odfStore.settingsDoc(), textShapeData.document());

    if (updater) updater->setProgress(100);
    return true;
}

void KWOdfLoader::loadSettings(const KoXmlDocument &settingsDoc, QTextDocument *textDoc)
{
    KoTextDocument(textDoc).setRelativeTabs(true);
    if (settingsDoc.isNull())
        return;

    debugWords << "KWOdfLoader::loadSettings";
    KoOasisSettings settings(settingsDoc);
    KoOasisSettings::Items viewSettings = settings.itemSet("ooo:view-settings");
    if (!viewSettings.isNull()) {
        m_document->setUnit(KoUnit::fromSymbol(viewSettings.parseConfigItemString("unit")));
    }

    KoOasisSettings::Items configurationSettings = settings.itemSet("ooo:configuration-settings");
    if (!configurationSettings.isNull()) {
        const QString ignorelist = configurationSettings.parseConfigItemString("SpellCheckerIgnoreList");
        debugWords << "Ignorelist:" << ignorelist;

        KoTextDocument(textDoc).setRelativeTabs(configurationSettings.parseConfigItemBool("TabsRelativeToIndent", true));

        KoTextDocument(textDoc).setParaTableSpacingAtStart(configurationSettings.parseConfigItemBool("AddParaTableSpacingAtStart", true));
    }
    //1.6: m_document->variableCollection()->variableSetting()->loadOasis(settings);
}

void KWOdfLoader::loadMasterPageStyles(KoShapeLoadingContext &context)
{
    debugWords << " !!!!!!!!!!!!!! loadMasterPageStyles called !!!!!!!!!!!!!!";
    debugWords << "Number of items :" << context.odfLoadingContext().stylesReader().masterPages().size();

    //TODO probably we should introduce more logic to handle the "standard" even
    //in faulty documents. See also bugreport #129585 as example.
    const KoOdfStylesReader &styles = context.odfLoadingContext().stylesReader();
    QHashIterator<QString, KoXmlElement *> it(styles.masterPages());
    while (it.hasNext()) {
        it.next();
        Q_ASSERT(! it.key().isEmpty());
        const KoXmlElement *masterNode = it.value();
        Q_ASSERT(masterNode);
        QString displayName = masterNode->attributeNS(KoXmlNS::style, "display-name", QString());
        KWPageStyle masterPage = m_document->pageManager()->pageStyle(it.key());
        if (!masterPage.isValid()) // use display-name as fall-back if there is no page-style with the defined name. See bug 281922 and 282082.
            masterPage = m_document->pageManager()->pageStyle(displayName);
        bool alreadyExists = masterPage.isValid();
        if (!alreadyExists)
            masterPage = KWPageStyle(it.key(), displayName);
        const KoXmlElement *masterPageStyle = styles.findStyle(masterNode->attributeNS(KoXmlNS::style, "page-layout-name", QString()));
        if (masterPageStyle) {
            masterPage.loadOdf(context.odfLoadingContext(), *masterNode, *masterPageStyle, m_document->resourceManager());
            loadHeaderFooter(context, masterPage, *masterNode, LoadHeader);
            loadHeaderFooter(context, masterPage, *masterNode, LoadFooter);
        }
        if (!alreadyExists)
            m_document->pageManager()->addPageStyle(masterPage);
    }
}

// helper function to create a KWTextFrameSet for a header/footer.
void KWOdfLoader::loadHeaderFooterFrame(KoShapeLoadingContext &context, const KWPageStyle &pageStyle, const KoXmlElement &elem, Words::TextFrameSetType fsType)
{
    KWTextFrameSet *fs = new KWTextFrameSet(m_document, fsType);
    fs->setPageStyle(pageStyle);
    m_document->addFrameSet(fs);

    debugWords << "KWOdfLoader::loadHeaderFooterFrame localName=" << elem.localName() << " type=" << fs->name();

    // use auto-styles from styles.xml, not those from content.xml
    context.odfLoadingContext().setUseStylesAutoStyles(true);

    // disable the undo recording during load so the kotexteditor is in sync with
    // the app's undostack
    fs->document()->setUndoRedoEnabled(false);

    KoTextLoader loader(context);
    QTextCursor cursor(fs->document());
    loader.loadBody(elem, cursor);

    fs->document()->setUndoRedoEnabled(true);

    // restore use of auto-styles from content.xml, not those from styles.xml
    context.odfLoadingContext().setUseStylesAutoStyles(false);
}

//1.6: KWOasisLoader::loadOasisHeaderFooter
void KWOdfLoader::loadHeaderFooter(KoShapeLoadingContext &context, KWPageStyle &pageStyle,
                                   const KoXmlElement &masterPage, HFLoadType headerFooter)
{
    // The actual content of the header/footer.
    KoXmlElement elem = KoXml::namedItemNS(masterPage, KoXmlNS::style,
                                           headerFooter == LoadHeader ? "header" : "footer");

    // The two additional elements <style:header-left> and <style:footer-left>
    // specifies if defined that even and odd pages should be displayed
    // different. If they are missing, the content of odd and even (aka left
    // and right) pages are the same.
    KoXmlElement leftElem = KoXml::namedItemNS(masterPage, KoXmlNS::style,
                                               headerFooter == LoadHeader ? "header-left" : "footer-left");

    // Used in KWPageStyle to determine if, and what kind of header/footer to use.
    Words::HeaderFooterType hfType = elem.isNull() ? Words::HFTypeNone
                                                   : leftElem.isNull() ? Words::HFTypeUniform
                                                                       : Words::HFTypeEvenOdd;

    // header-left and footer-left
    if (! leftElem.isNull()) {
        loadHeaderFooterFrame(context, pageStyle, leftElem,
                              headerFooter == LoadHeader ? Words::EvenPagesHeaderTextFrameSet
                                                         : Words::EvenPagesFooterTextFrameSet);
    }

    // header and footer
    if (! elem.isNull()) {
        loadHeaderFooterFrame(context, pageStyle, elem,
                              headerFooter == LoadHeader ? Words::OddPagesHeaderTextFrameSet
                                                         : Words::OddPagesFooterTextFrameSet);
    }

    if (headerFooter == LoadHeader) {
        pageStyle.setHeaderPolicy(hfType);
    } else {
        pageStyle.setFooterPolicy(hfType);
    }
}

