/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2000 Michael Johnson <mikej@xnet.com>
   SPDX-FileCopyrightText: 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2010-2011 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Christoph Cullmann <cullmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AsciiImport.h"

#include <QApplication>
#include <QFile>
#include <QTextCursor>
#include <QTextDocument>

#include <KPluginFactory>

#include <KoCharacterStyle.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfWriteStore.h>
#include <KoParagraphStyle.h>
#include <KoProgressUpdater.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoStore.h>
#include <KoStyleManager.h>
#include <KoTextDocumentLayout.h>
#include <KoTextWriter.h>
#include <KoUpdater.h>
#include <KoXmlWriter.h>

#include "AsciiImportDebug.h"
#include "ImportDialog.h"
#include "calligra-version.h"

#include <KWDocument.h>
#include <KWPage.h>
#include <frames/KWTextFrameSet.h>

// If defined then the output will be written to OpenDocument ODT rather then
// accessing the Calligra Words API direct. Using the additional ODT-roundtrip
// is slower then using the Calligra Words API direct.
// #define OUTPUT_AS_ODT_FILE

K_PLUGIN_FACTORY_WITH_JSON(AsciiImportFactory, "calligra_filter_ascii2words.json", registerPlugin<AsciiImport>();)

AsciiImport::AsciiImport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

AsciiImport::~AsciiImport() = default;

KoFilter::ConversionStatus AsciiImport::convert(const QByteArray &from, const QByteArray &to)
{
    // check for proper conversion
    if (to != "application/vnd.oasis.opendocument.text" || from != "text/plain") {
        return KoFilter::NotImplemented;
    }

    QFile in(m_chain->inputFile());
    if (!in.open(QIODevice::ReadOnly)) {
        errorAsciiImport << "Unable to open input file!" << Qt::endl;
        in.close();
        return KoFilter::FileNotFound;
    }

#ifdef OUTPUT_AS_ODT_FILE

#else
    KoDocument *document = m_chain->outputDocument();
    if (!document)
        return KoFilter::StupidError;
    KWDocument *outputDoc = qobject_cast<KWDocument *>(document);
    outputDoc->setOutputMimeType(to);
    // outputDoc->setSaveInBatchMode(true);

    QPointer<KoUpdater> loadUpdater = outputDoc->progressUpdater()->startSubtask(2, "load");
    loadUpdater->setRange(0, in.size());

    QPointer<KoUpdater> layoutUpdater = outputDoc->progressUpdater()->startSubtask(3, "layout");
#endif

    // try to read 100000 bytes so we can be quite sure the guessed encoding is correct.
    // this code is inspired by the kate encoding guessing first try UTF-8
    QByteArray data = in.read(100000);
    in.seek(0);

    int paragraphStrategy = 0;
    if (!m_chain->manager()->getBatchMode()) {
        QPointer<AsciiImportDialog> dialog = new AsciiImportDialog(QApplication::activeWindow());
        if (!dialog->exec()) {
            in.close();
            return KoFilter::UserCancelled;
        }
        paragraphStrategy = dialog->getParagraphStrategy();
    }

#ifdef OUTPUT_AS_ODT_FILE
    KoStore *store = KoStore::createStore(m_chain->outputFile(), KoStore::Write, to, KoStore::Zip);
    if (!store || store->bad()) {
        warnAsciiImport << "Unable to open output file!";
        delete store;
        return KoFilter::FileNotFound;
    }

    debugAsciiImport << "created store.";
    KoOdfWriteStore odfStore(store);
    odfStore.manifestWriter(to);

    KoXmlWriter *contentWriter = odfStore.contentWriter();
    if (!contentWriter) {
        delete store;
        return KoFilter::CreationError;
    }

    KoGenStyles mainStyles;
    KoXmlWriter *bodyWriter = odfStore.bodyWriter();

    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:text");

    QString styleName("txt");
    KoGenStyle style(KoGenStyle::ParagraphStyle, "paragraph");
    style.addAttribute("style:display-name", styleName);
    style.addProperty("fo:font-family", "dejavu sans mono", KoGenStyle::TextType);
    style.addProperty("fo:font-family-generic", "modern", KoGenStyle::TextType);
    style.addProperty("fo:font-size", "10pt", KoGenStyle::TextType);

    style.addProperty("fo:font-weight", "normal", KoGenStyle::TextType);
    QString name(QString(QUrl::toPercentEncoding(styleName, "", " ")).replace('%', '_'));

    name = mainStyles.insert(style, name, KoGenStyles::DontAddNumberToName);
#else
    KoStyleManager *styleManager = outputDoc->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager *>();
    KoParagraphStyle *p = styleManager->defaultParagraphStyle();
    p->setFontFamily("dejavu sans mono");
    p->setFontPointSize(10);
    p->setFontStyleHint(QFont::TypeWriter);

    outputDoc->appendPage();
    QTextDocument *doc = outputDoc->mainFrameSet()->document();
    // doc->setDefaultFont(p->font());

    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout *>(doc->documentLayout());
    Q_ASSERT(lay);
    lay->setBlockLayout(true);
    connect(lay, &KoTextDocumentLayout::layoutProgressChanged, layoutUpdater.data(), &KoUpdater::setProgress);

    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    QTextCharFormat charFormat;
    ((KoCharacterStyle *)p)->applyStyle(charFormat);
    cursor.setCharFormat(charFormat);
#endif

    QTextStream stream(&in);

    switch (paragraphStrategy) {
    case 1: { // Sentence: Line-break at the end of a sentence.
        QString stoppingPunctuation(".!?");
        QString skippingEnd(" \"')");
        while (!stream.atEnd()) {
            QString paragraph;
            for (;;) {
                const QString line = stream.readLine();
                if (line.isEmpty())
                    break;
                paragraph += line + ' ';
                int lastPos = line.length() - 1;
                int maxCheck = lastPos >= 10 ? 10 : lastPos + 1;
                QChar lastChar;
                // Skip a maximum of 10 quotes (or similar) at the end of the line
                for (int i = 0; i < maxCheck; ++i, --lastPos) {
                    lastChar = line[lastPos];
                    if (lastPos == 0 || lastChar.isNull() || skippingEnd.indexOf(lastChar) == -1)
                        break;
                }
                lastChar = line[lastPos];
                if (lastChar.isNull())
                    continue;
                if (stoppingPunctuation.indexOf(lastChar) != -1)
                    break;
            }
            if (!paragraph.isNull()) {
                QString s = paragraph.simplified();
#ifdef OUTPUT_AS_ODT_FILE
                bodyWriter->startElement("text:p");
                bodyWriter->addAttribute("text:style-name", styleName);
                if (!s.isEmpty())
                    bodyWriter->addTextSpan(s);
                bodyWriter->endElement();
#else
                if (!s.isEmpty())
                    cursor.insertText(s /*, charFormat*/);
                cursor.insertBlock();
                loadUpdater->setValue(stream.device()->pos());
#endif
            }
        }
    } break;
    case 2: { // Empty Line: Line-break if the line is empty.
        while (!stream.atEnd()) {
            QString paragraph;
            do {
                const QString line = stream.readLine();
                if (line.isEmpty())
                    break;
                paragraph.append(line + ' ');
            } while (true);
            if (!paragraph.isNull()) {
                QString s = paragraph.simplified();
#ifdef OUTPUT_AS_ODT_FILE
                bodyWriter->startElement("text:p");
                bodyWriter->addAttribute("text:style-name", styleName);
                if (!s.isEmpty())
                    bodyWriter->addTextSpan(s);
                bodyWriter->endElement();
#else
                if (!s.isEmpty()) {
                    cursor.insertText(s /*, charFormat*/);
                    cursor.insertBlock();
                    loadUpdater->setValue(stream.device()->pos());
                }
#endif
            }
        }
    } break;
    default: { // As Is: Line-break at the end of line.
        while (!stream.atEnd()) {
            QString s = stream.readLine();
#ifdef OUTPUT_AS_ODT_FILE
            bodyWriter->startElement("text:p");
            bodyWriter->addAttribute("text:style-name", styleName);
            if (!s.isEmpty())
                bodyWriter->addTextSpan(s);
            bodyWriter->endElement();
#else
            if (!s.isEmpty())
                cursor.insertText(s /*, charFormat*/);
            cursor.insertBlock();
            loadUpdater->setValue(stream.device()->pos());
#endif
        }
    } break;
    }

#ifdef OUTPUT_AS_ODT_FILE
    bodyWriter->endElement(); // office:text
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);
    odfStore.closeContentWriter();

    odfStore.manifestWriter()->addManifestEntry("content.xml", "text/xml");
    if (!mainStyles.saveOdfStylesDotXml(odfStore.store(), odfStore.manifestWriter())) {
        delete store;
        return KoFilter::CreationError;
    }
    if (store->open("meta.xml")) {
        KoStoreDevice dev(store);
        KoXmlWriter *xmlWriter = KoOdfWriteStore::createOasisXmlWriter(&dev, "office:document-meta");
        xmlWriter->startElement("office:meta");
        xmlWriter->startElement("meta:generator");
        xmlWriter->addTextNode(QString("Calligra %1").arg(CALLIGRA_VERSION_STRING));
        xmlWriter->endElement();
        xmlWriter->startElement("meta:creation-date");
        xmlWriter->addTextNode(QDateTime::currentDateTime().toString(Qt::ISODate));
        xmlWriter->endElement();
        xmlWriter->endElement(); // office:meta
        xmlWriter->endElement(); // root element
        xmlWriter->endDocument();
        delete xmlWriter;
        if (store->close())
            odfStore.manifestWriter()->addManifestEntry("meta.xml", "text/xml");
    }
    if (!odfStore.closeManifestWriter()) {
        warnAsciiImport << "Error while trying to write 'META-INF/manifest.xml'. Partition full?";
        delete store;
        return KoFilter::CreationError;
    }
    delete store;
#else
    cursor.endEditBlock();
    lay->setBlockLayout(false);
    lay->layout();
#endif

    return KoFilter::OK;
}

#include "AsciiImport.moc"
