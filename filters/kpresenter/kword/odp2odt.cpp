/* This file is part of the KDE project
   Copyright (C) 2006 David Faure <faure@kde.org>

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

#include "odp2odt.h"

#include <kgenericfactory.h>
#include <KoStoreDevice.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoXmlNS.h>
#include <KoOdfReadStore.h>
#include <KoDom.h>
#include <klocale.h>
#include <kdebug.h>

typedef KGenericFactory<Odp2Odt> Odp2OdtFactory;
K_EXPORT_COMPONENT_FACTORY(libkodp2odt, Odp2OdtFactory("kofficefilters"))

Odp2Odt::Odp2Odt(QObject *parent, const QStringList&) :
        KoFilter(parent)
{
}

// This filter can act as an import filter for KWord and as an export
// filter for KPresenter (isn't our architecture really nice ? :)
// This is why we use the file-to-file method, not a QDomDoc one.
KoFilter::ConversionStatus Odp2Odt::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "application/vnd.oasis.opendocument.text"
            || from != "application/vnd.oasis.opendocument.presentation")
        return KoFilter::NotImplemented;

    KoStoreDevice* inpdev = m_chain->storageFile("content.xml", KoStore::Read);
    if (!inpdev) {
        kError(31000) << "Unable to open input stream";
        return KoFilter::StorageCreationError;
    }

    // Parse presentation content.xml
    QString errorMsg;
    if (!KoOdfReadStore::loadAndParse(inpdev, doc, errorMsg, "content.xml")) {
        return KoFilter::ParsingError;
    }

    QDomElement docElem = doc.documentElement();
    QDomElement realBody(KoDom::namedItemNS(docElem, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        kError(31000) << "No office:body found!";
        //setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return KoFilter::WrongFormat;
    }

    QDomElement body = KoDom::namedItemNS(realBody, KoXmlNS::office, "presentation");
    if (body.isNull()) {
        kError(32001) << "No office:text found!";
        return KoFilter::WrongFormat;
    }

    // Transformations start here:

    fixBodyTagName(body);

    processPages(body);

    adjustStyles();

    copyFile("settings.xml");
    copyFile("meta.xml");
    // TODO generate the manifest along the way

    // Write output file

    KoStoreDevice* out = m_chain->storageFile("content.xml", KoStore::Write);
    if (!out) {
        kError(30502) << "Unable to open output file!";
        return KoFilter::StorageCreationError;
    }
    const QByteArray cstring = doc.toByteArray(); // utf-8 already
    //kDebug() << QString::fromUtf8( cstring );
    const int len = cstring.length();
    const int written = out->write(cstring.constData(), len);
    if (written != len)
        kError(30502) << "Wrote only " << written << " out of " << len;
    out->close();

    return KoFilter::OK;
}

void Odp2Odt::fixBodyTagName(QDomElement& body)
{
    body.setTagName("text");   // the prefix is kept, apparently
}

void Odp2Odt::processPages(QDomElement& body)
{
    // For each page
    QDomElement pageElem;
    forEachElement(pageElem, body) {
        if (pageElem.localName() == "page" && pageElem.namespaceURI() == KoXmlNS::draw) {
            // For each frame
            QDomElement frame;
            forEachElement(frame, pageElem) {
                const bool isTitle = frame.attributeNS(KoXmlNS::draw, "name") == "Title";
                // Only intested in text boxes. Images are often used for background effects...
                QDomElement textBox = KoDom::namedItemNS(frame, KoXmlNS::draw, "text-box");
                if (textBox.isNull())
                    continue;
                // Get any text:p or text:numbered-paragraph or list etc.
                QList<QDomElement> textNodes;
                QDomElement textElem;
                bool first = true;
                forEachElement(textElem, textBox) {
                    if (isTitle && first) {
                        // This needs outline-style and stuff
                        //textElem.setTagName( "h" ); // title -> text:h
                        // Broken in Qt-4.2.0
                        //textElem.setAttributeNS( KoXmlNS::text, "style-name", "generatedSlideTitle" );
                        textElem.removeAttributeNS(KoXmlNS::text, "style-name");
                        textElem.setAttribute("text:style-name", "generatedSlideTitle");
                        first = false;
                    }
                    textNodes.append(textElem);
                }
                // Now reparent them up (can't be done while iterating)
                foreach(const QDomElement & e, textNodes) {
                    body.appendChild(e);
                }
            }
        }
    }
    // Get rid of the pages
    QDomNode n = body.firstChild();
    for (; !n.isNull();) {
        QDomNode next = n.nextSibling();
        if (n.isElement() && n.localName() == "page" && n.namespaceURI() == KoXmlNS::draw) {
            body.removeChild(n);
        }
        n = next;
    }
    // Get rid of presentation:settings
    n = KoDom::namedItemNS(body, KoXmlNS::presentation, "settings");
    body.removeChild(n);
}

void Odp2Odt::copyFile(const QString& fileName)
{
    KoStoreDevice* inpdev = m_chain->storageFile(fileName, KoStore::Read);
    if (!inpdev) {
        kError(31000) << "Unable to open " << fileName;
        return;
    }

    const QByteArray data = inpdev->readAll();

    KoStoreDevice* out = m_chain->storageFile(fileName, KoStore::Write);
    if (!out) {
        kError(30502) << "Unable to open output file " << fileName;
        return;
    }

    out->write(data);
}


// QDomDocument is just too messed up when saving with namespace support
//#define NAMESPACE_SUPPORT

void Odp2Odt::adjustStyles()
{
    KoStoreDevice* inpdev = m_chain->storageFile("styles.xml", KoStore::Read);
    if (!inpdev) {
        kError(31000) << "Unable to open styles.xml";
        return;
    }

    QDomDocument stylesDoc;

    // Parse styles.xml
    QString errorMsg;
    int errorLine, errorColumn;
#ifdef NAMESPACE_SUPPORT
    QXmlInputSource source(inpdev);
    QXmlSimpleReader reader;
    KoOdfReadStore::setupXmlReader(reader, true /*namespaceProcessing*/);
    bool ok = stylesDoc.setContent(&source, &reader, &errorMsg, &errorLine, &errorColumn);
#else
    bool ok = stylesDoc.setContent(inpdev, &errorMsg, &errorLine, &errorColumn);
#endif
    if (!ok) {
        kError(31000) << "Parsing error! Aborting!" << endl
        << " In line: " << errorLine << ", column: " << errorColumn << endl
        << " Error message: " << errorMsg;
        return;
    }

    kDebug() << QString::fromUtf8(stylesDoc.toByteArray());

    QDomElement docElem = stylesDoc.documentElement();
    Q_ASSERT(!docElem.isNull());

    fixPageLayout(docElem);
    addSlideTitleStyle(docElem);

    KoStoreDevice* out = m_chain->storageFile("styles.xml", KoStore::Write);
    if (!out) {
        kError(30502) << "Unable to open output file styles.xml";
        return;
    }

    const QByteArray cstring = stylesDoc.toByteArray(); // utf-8 already
    //kDebug() << QString::fromUtf8( cstring );
    const int len = cstring.length();
    const int written = out->write(cstring.constData(), len);
    if (written != len)
        kError(30502) << "Wrote only " << written << " out of " << len;
    out->close();
}

// Presentation documents can have 0pt as margins, and a weird size.
// Text documents need margins, and a normal paper size.
void Odp2Odt::fixPageLayout(QDomElement& docElem)
{
#ifdef NAMESPACE_SUPPORT
    QDomElement automaticStyles = KoDom::namedItemNS(docElem, KoXmlNS::office, "automatic-styles");
#else
    QDomElement automaticStyles = docElem.namedItem("office:automatic-styles").toElement();
#endif
    if (automaticStyles.isNull()) {
        kDebug() << "automatic-styles not found";
        return;
    }
    // # what if there's more than one?
#ifdef NAMESPACE_SUPPORT
    QDomElement pageLayoutElement = KoDom::namedItemNS(automaticStyles, KoXmlNS::style, "page-layout");
#else
    QDomElement pageLayoutElement = automaticStyles.namedItem("style:page-layout").toElement();
#endif
    if (pageLayoutElement.isNull())
        return;
#ifdef NAMESPACE_SUPPORT
    QDomElement pageLayoutProperties = KoDom::namedItemNS(pageLayoutElement, KoXmlNS::style, "page-layout-properties");
#else
    QDomElement pageLayoutProperties = pageLayoutElement.namedItem("style:page-layout-properties").toElement();
#endif
    if (pageLayoutProperties.isNull())
        return;
#ifdef NAMESPACE_SUPPORT
    pageLayoutProperties.setAttributeNS(KoXmlNS::fo, "margin-top", "42pt");
    pageLayoutProperties.setAttributeNS(KoXmlNS::fo, "margin-bottom", "42pt");
    pageLayoutProperties.setAttributeNS(KoXmlNS::fo, "margin-left", "57pt");
    pageLayoutProperties.setAttributeNS(KoXmlNS::fo, "margin-right", "57pt");
    pageLayoutProperties.setAttributeNS(KoXmlNS::fo, "page-width", "210mm");
    pageLayoutProperties.setAttributeNS(KoXmlNS::fo, "page-height", "297mm");
#else
    pageLayoutProperties.setAttribute("fo:margin-top", "42pt");
    pageLayoutProperties.setAttribute("fo:margin-bottom", "42pt");
    pageLayoutProperties.setAttribute("fo:margin-left", "57pt");
    pageLayoutProperties.setAttribute("fo:margin-right", "57pt");
    pageLayoutProperties.setAttribute("fo:page-width", "210mm");
    pageLayoutProperties.setAttribute("fo:page-height", "297mm");
#endif
}

void Odp2Odt::addSlideTitleStyle(QDomElement& docElem)
{
    QDomElement styles = docElem.namedItem("office:styles").toElement();

    QDomElement style = docElem.ownerDocument().createElement("style:style");
    style.setAttribute("style:family", "paragraph");
    style.setAttribute("style:name", "generatedSlideTitle");
    style.setAttribute("style:display-name", i18n("Slide Title"));

    styles.appendChild(style);
}

#include "odp2odt.moc"
