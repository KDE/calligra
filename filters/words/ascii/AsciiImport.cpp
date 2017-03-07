/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Michael Johnson <mikej@xnet.com>
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2010-2011 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2010 Christoph Cullmann <cullmann@kde.org> 

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include "AsciiImport.h"

#include <QTextCodec>
#include <QFile>
#include <QTextDocument>
#include <QTextCursor>

#include <kpluginfactory.h>
#include <kencodingprober.h>

#include <CalligraVersionWrapper.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoStore.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoShapeSavingContext.h>
#include <KoTextWriter.h>
#include <KoProgressUpdater.h>
#include <KoUpdater.h>
#include <KoTextDocumentLayout.h>

#include "AsciiImportDebug.h"
#include "ImportDialog.h"

#include <KWDocument.h>
#include <KWPage.h>
#include <frames/KWTextFrameSet.h>

// If defined then the output will be written to OpenDocument ODT rather then
// accessing the Calligra Words API direct. Using the additional ODT-roundtrip
// is slower then using the Calligra Words API direct.
//#define OUTPUT_AS_ODT_FILE

K_PLUGIN_FACTORY_WITH_JSON(AsciiImportFactory, "calligra_filter_ascii2words.json",
                           registerPlugin<AsciiImport>();)

bool checkEncoding(QTextCodec *codec, QByteArray &data)
{
    QTextCodec::ConverterState state(QTextCodec::ConvertInvalidToNull);
    QString unicode = codec->toUnicode(data.constData(), data.size(), &state);
    for (int i = 0; i < unicode.size(); ++i) {
        if (unicode[i] == 0) {
            return false;
        }
    }
    return true;
}

AsciiImport::AsciiImport(QObject *parent, const QVariantList &)
: KoFilter(parent)
{
}

AsciiImport::~AsciiImport()
{
}

KoFilter::ConversionStatus AsciiImport::convert(const QByteArray& from, const QByteArray& to)
{
    // check for proper conversion
    if (to != "application/vnd.oasis.opendocument.text" || from != "text/plain") {
        return KoFilter::NotImplemented;
    }

    QFile in(m_chain->inputFile());
    if (!in.open(QIODevice::ReadOnly)) {
        errorAsciiImport << "Unable to open input file!" << endl;
        in.close();
        return KoFilter::FileNotFound;
    }

#ifdef OUTPUT_AS_ODT_FILE
    
#else
    KoDocument* document = m_chain->outputDocument();
    if (!document)
        return KoFilter::StupidError;
    KWDocument *outputDoc = qobject_cast<KWDocument*>(document);
    outputDoc->setOutputMimeType(to);
    //outputDoc->setSaveInBatchMode(true);

    QPointer<KoUpdater> loadUpdater = outputDoc->progressUpdater()->startSubtask(2, "load");
    loadUpdater->setRange(0, in.size());

    QPointer<KoUpdater> layoutUpdater = outputDoc->progressUpdater()->startSubtask(3, "layout");
#endif

    // try to read 100000 bytes so we can be quite sure the guessed encoding is correct.
    // this code is inspired by the kate encoding guessing first try UTF-8
    QByteArray data = in.read(100000);
    in.seek(0);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    if (!checkEncoding(codec, data)) {
        KEncodingProber prober(KEncodingProber::Universal);
        prober.feed(data);
        debugAsciiImport << "guessed" << prober.encoding() << prober.confidence();
        if (prober.confidence() > 0.5)
            codec = QTextCodec::codecForName(prober.encoding());
        if (!codec || !checkEncoding(codec, data )) {
            codec = QTextCodec::codecForName("ISO 8859-15");
            if (!checkEncoding(codec, data))
                codec = QTextCodec::codecForName("UTF-8");
        }
    }

    int paragraphStrategy = 0;
    if (!m_chain->manager()->getBatchMode()) {
        QPointer<AsciiImportDialog> dialog = new AsciiImportDialog(codec->name(), QApplication::activeWindow());
        if (!dialog) { in.close(); return KoFilter::StupidError; }
        if (!dialog->exec()) { in.close(); return KoFilter::UserCancelled; }
        codec = dialog->getCodec();
        paragraphStrategy = dialog->getParagraphStrategy();
    }
    if (!codec) return KoFilter::StupidError;
    debugAsciiImport << "Charset used:" << codec->name();

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

    KoXmlWriter* contentWriter = odfStore.contentWriter();
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
    KoStyleManager *styleManager = outputDoc->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    KoParagraphStyle *p = styleManager->defaultParagraphStyle();
    p->setFontFamily("dejavu sans mono");
    p->setFontPointSize(10);
    p->setFontStyleHint(QFont::TypeWriter);

    outputDoc->appendPage();
    QTextDocument *doc = outputDoc->mainFrameSet()->document();
    //doc->setDefaultFont(p->font());

    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(doc->documentLayout());
    Q_ASSERT(lay);
    lay->setBlockLayout(true);
    connect(lay, SIGNAL(layoutProgressChanged(int)), layoutUpdater, SLOT(setProgress(int)));

    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    QTextCharFormat charFormat;
    ((KoCharacterStyle*)p)->applyStyle(charFormat);
    cursor.setCharFormat(charFormat);
#endif

    QTextStream stream(&in);
    Q_ASSERT(codec);
    stream.setCodec(codec);

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
                int maxCheck = lastPos >= 10 ? 10: lastPos + 1;
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
            } while(true);
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
        KoXmlWriter* xmlWriter = KoOdfWriteStore::createOasisXmlWriter(&dev, "office:document-meta");
        xmlWriter->startElement("office:meta");
        xmlWriter->startElement("meta:generator");
        xmlWriter->addTextNode(QString("Calligra %1").arg(CalligraVersionWrapper::versionString()));
        xmlWriter->endElement();
        xmlWriter->startElement("meta:creation-date");
        xmlWriter->addTextNode(QDateTime::currentDateTime().toString(Qt::ISODate));
        xmlWriter->endElement();
        xmlWriter->endElement(); // office:meta
        xmlWriter->endElement(); // root element
        xmlWriter->endDocument();
        delete xmlWriter;
        if (store->close())
            odfStore.manifestWriter()->addManifestEntry("meta.xml", "text/xml" );
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
