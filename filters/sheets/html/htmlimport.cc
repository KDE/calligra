/* This file is part of the KDE project
   Copyright (C) 2001 Eva Brucherseifer <eva@kde.org>
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
   based on kspread csv export filter by David Faure

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
 * Boston, MA 02110-1301, USA.
*/

#include "htmlimport.h"

#include "HtmlImportDebug.h"
//#include <exportdialog.h>

#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>
#include <QByteArray>
#include <QEventLoop>
#include <kpluginfactory.h>
#include <KoFilterChain.h>
#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoGenStyle.h>

#include <khtml_part.h>
#include <khtmlview.h>
#include <dom/dom_text.h>
#include <dom/dom2_views.h>
#include <dom/dom_doc.h>
#include <dom/dom_element.h>
#include <dom/dom_string.h>
//#include <dom/html_table.h>
//#include <dom/html_misc.h>

//using namespace Calligra::Sheets;

K_PLUGIN_FACTORY_WITH_JSON(HTMLImportFactory, "calligra_filter_html2ods.json",
                           registerPlugin<HTMLImport>();)

HTMLImport::HTMLImport(QObject* parent, const QVariantList&)
    : KoFilter(parent)
{
}

HTMLImport::~HTMLImport()
{
}

KoFilter::ConversionStatus HTMLImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "application/vnd.oasis.opendocument.spreadsheet" || from != "text/html") {
        warnHtml << "Invalid mimetypes " << to << " " << from;
        return KoFilter::NotImplemented;
    }

    QString inputFile = m_chain->inputFile();
    QString outputFile = m_chain->outputFile();
    debugHtml<<"inputFile="<<inputFile<<"outputFile="<<outputFile;

    // check if the inout file exists
    m_inputDir = QFileInfo(m_chain->inputFile()).dir();
    if(!m_inputDir.exists())
        return KoFilter::StupidError;

    // create output store
    KoStore* storeout = KoStore::createStore(outputFile, KoStore::Write, "application/vnd.oasis.opendocument.spreadsheet", KoStore::Zip);
    if (!storeout)
        return KoFilter::FileNotFound;

    KoOdfWriteStore oasisStore(storeout);
    m_manifestWriter = oasisStore.manifestWriter("application/vnd.oasis.opendocument.spreadsheet");
    m_store = &oasisStore;

    m_mainStyles = new KoGenStyles();

    KoXmlWriter* bodyWriter = m_store->bodyWriter();
    m_store->contentWriter(); // we need to create the instance even if the contentWriter is not used

    bodyWriter->startElement("office:body");
    KoFilter::ConversionStatus result = loadUrl(QUrl::fromLocalFile(m_chain->inputFile()));
    if(result != KoFilter::OK)
        warnHtml << "Failed to load url=" << m_chain->inputFile();
    bodyWriter->endElement(); // office:body

    if(m_store->closeContentWriter())
        m_manifestWriter->addManifestEntry("content.xml", "text/xml");

    if(createStyle())
        m_manifestWriter->addManifestEntry("styles.xml", "text/xml");

    if(createMeta())
        m_manifestWriter->addManifestEntry("meta.xml", "text/xml");

    m_store->closeManifestWriter();
    delete storeout;
    m_manifestWriter = 0;
    m_store = 0;
    return result;
}

bool HTMLImport::createStyle()
{
    if (!m_store->store()->open("styles.xml"))
        return false;
    KoStoreDevice dev(m_store->store());
    KoXmlWriter* stylesWriter = new KoXmlWriter(&dev);

    stylesWriter->startDocument("office:document-styles");
    stylesWriter->startElement("office:document-styles");
    stylesWriter->addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    stylesWriter->addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    stylesWriter->addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    stylesWriter->addAttribute("xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
    stylesWriter->addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    stylesWriter->addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    stylesWriter->addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    stylesWriter->addAttribute("office:version", "1.0");

    m_mainStyles->saveOdfStyles(KoGenStyles::MasterStyles, stylesWriter);
    m_mainStyles->saveOdfStyles(KoGenStyles::DocumentStyles, stylesWriter); // office:style
    m_mainStyles->saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, stylesWriter); // office:automatic-styles

    stylesWriter->endElement();  // office:document-styles
    stylesWriter->endDocument();

    delete stylesWriter;
    return m_store->store()->close();
}

bool HTMLImport::createMeta()
{
    if (!m_store->store()->open("meta.xml"))
        return false;

    KoStoreDevice dev(m_store->store());
    KoXmlWriter* metaWriter = new KoXmlWriter(&dev);
    metaWriter->startDocument("office:document-meta");
    metaWriter->startElement("office:document-meta");
    metaWriter->addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    metaWriter->addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    metaWriter->addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    metaWriter->addAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
    metaWriter->startElement("office:meta");

    //metaWriter->startElement("dc:title");
    //metaWriter->addTextNode(workbook->property(Workbook::PIDSI_TITLE).toString());
    //metaWriter->endElement();

    //metaWriter->startElement("dc:subject", false);
    //metaWriter->addTextNode(workbook->property(Workbook::PIDSI_SUBJECT).toString());
    //metaWriter->endElement();

    metaWriter->endElement(); // office:meta
    metaWriter->endElement(); // office:document-meta
    metaWriter->endDocument();

    delete metaWriter;
    return m_store->store()->close();
}

KoFilter::ConversionStatus HTMLImport::loadUrl(const QUrl &url)
{
    debugHtml << url;

    KoXmlWriter* bodyWriter = m_store->bodyWriter();
    //KoXmlWriter* contentWriter = m_store->contentWriter();

    QStringList sheets;
    {
        KHTMLPart html;
        html.view()->resize(600, 530);
        html.setAutoloadImages(false);
        html.setJScriptEnabled(false);
        html.setPluginsEnabled(false);
        html.setJavaEnabled(false);
        html.setMetaRefreshEnabled(false);

        QEventLoop loop;
        connect(&html, SIGNAL(completed()), &loop, SLOT(quit()));
        QMetaObject::invokeMethod(&html,"openUrl", Qt::QueuedConnection, Q_ARG(QUrl,url));
        //if (!html.openUrl(url)) { warnHtml << "Failed loadUrl" << url; return KoFilter::StupidError; }
        loop.exec(QEventLoop::ExcludeUserInputEvents);

        // body
        DOM::Document doc = html.document();
        DOM::NodeList body = doc.getElementsByTagName("body");
        DOM::Node docbody = body.item(0);
        if (!docbody.isNull()) {
            m_states.push(InBody);
            bodyWriter->startElement("office:spreadsheet");
            parseNode(docbody);
            bodyWriter->endElement(); // office:spreadsheet
            m_states.pop();
        }

        // frames
        DOM::NodeList frameset = doc.getElementsByTagName("frameset");
        DOM::Node frame = frameset.item(0);
        if (!frame.isNull()) {
            for(uint i = 0; i < frameset.length(); ++i) {
                for (DOM::Node n = frameset.item(i).firstChild(); !n.isNull(); n = n.nextSibling()) {
                    DOM::Element f = n;
                    if(!f.isNull() && f.nodeName().lower() == "frame" && f.getAttribute("name").string() == "frSheet")
                        sheets.append(f.getAttribute("src").string());
                }
            }
        }
    }

    // the KHTMLPart and DOM::Document are no more and we can call us recursivly now.
    if(!sheets.isEmpty()) {
        m_states.push(InFrameset);
        foreach(const QString &src, sheets) {
            const QUrl u = QUrl::fromLocalFile(QFileInfo(m_inputDir, src).absoluteFilePath());
            loadUrl(u);
        }
        m_states.pop();
    }

    return KoFilter::OK;
}

void HTMLImport::parseNode(DOM::Node node)
{
    KoXmlWriter* bodyWriter = m_store->bodyWriter();
    //KoXmlWriter* contentWriter = m_store->contentWriter();

    // check if this is a text node.
    DOM::Text t = node;
    if (!t.isNull()) {
        if(!m_states.isEmpty() && m_states.top() == InCell) {
            const QString s = t.data().string().trimmed();
            if(!s.isEmpty()) {
                //debugHtml<<"TEXT tagname=" << node.nodeName() << "TEXT="<<t.data().string();
                bodyWriter->addAttribute("office:value-type", "string");
                bodyWriter->addAttribute("office:string-value", s);
            }
        }
        return; // no children anymore...
    }

    DOM::DOMString tag = node.nodeName().lower();

    if(tag == "table") {
        m_states.push(InTable);
        bodyWriter->startElement("table:table");

        // hack to get some name defined
        static int sheetCount = 0;
        bodyWriter->addAttribute("table:name", QString("Sheet %1").arg(++sheetCount));
    }
    else if(tag == "tr") {
        m_states.push(InRow);
        bodyWriter->startElement("table:table-row");
        //xmlWriter->addAttribute("table:number-columns-spanned", );
        //xmlWriter->addAttribute("table:number-rows-spanned", );
    }
    else if(tag == "td") {
        m_states.push(InCell);
        bodyWriter->startElement("table:table-cell");
    } else {
        m_states.push(InNone);
    }

    //debugHtml<<"...START nodeName="<<node.nodeName();

    DOM::Element e = node;
    bool go_recursive = true;
    if (!e.isNull()) {
        //parseStyle(e); // get the CSS information
        go_recursive = parseTag(e); // get the tag information
    }
    if (go_recursive) {
        for (DOM::Node n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
            parseNode(n);
        }
    }

    State state = m_states.pop();
    if(state == InTable || state == InRow || state == InCell) {
        bodyWriter->endElement();
    }

    //debugHtml<<"...END nodeName="<<node.nodeName();
}

bool HTMLImport::parseTag(DOM::Element element)
{
    DOM::DOMString tag = element.tagName().lower();

    // Don't handle the content of comment- or script-nodes.
    if (element.nodeType() == DOM::Node::COMMENT_NODE || tag == "script") {
        return false;
    }

    return true;
}

#include <htmlimport.moc>
