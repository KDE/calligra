/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2010 Inge Wallin <inge@lysator.liu.se>

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

#include "FormulaDocument.h"

// Qt
#include <QWidget>
#include <QIODevice>
#include <QPainter>

// KF5
#include <kmessagebox.h>
#include <kguiitem.h>

// Calligra
#include <KoDocument.h>
#include <KoXmlWriter.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlNS.h>
#include <KoGenStyles.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoView.h>
#include <KoStore.h>

// KFormula
#include "FormulaDebug.h"
#include "KoFormulaShape.h"
#include "FormulaPart.h"

class FormulaDocument::Private
{
public:
    Private();
    ~Private();

    KoFormulaShape *parent;
    QByteArray content;
};

FormulaDocument::Private::Private()
{
}

FormulaDocument::Private::~Private()
{
}

FormulaDocument::FormulaDocument( KoFormulaShape *parent )
    : KoDocument(new FormulaPart(0))
    , d ( new Private )
{
    d->parent = parent;
}

FormulaDocument::~FormulaDocument()
{
    delete d;
}


bool FormulaDocument::loadOdf( KoOdfReadStore &odfStore )
{
    KoXmlDocument doc = odfStore.contentDoc();
    KoXmlElement  bodyElement = doc.documentElement();

    debugFormula << bodyElement.nodeName();

    if (bodyElement.localName() != "math" || bodyElement.namespaceURI() != KoXmlNS::math) {
        errorFormula << "No <math:math> element found.";
        return false;
    }

    KoOdfLoadingContext   odfLoadingContext( odfStore.styles(), odfStore.store() );
    KoShapeLoadingContext context(odfLoadingContext, d->parent->resourceManager());

    return loadOdfEmbedded( bodyElement, context );
}

bool FormulaDocument::loadXML( const KoXmlDocument &doc, KoStore *)
{
    Q_UNUSED( doc );

    // We don't support the old XML format any more.
    return false;
}

bool FormulaDocument::loadEmbeddedDocument( KoStore *store,
                                           const KoXmlElement &objectElement,
                                           const KoOdfLoadingContext &odfLoadingContext)
{
    if ( !objectElement.hasAttributeNS( KoXmlNS::xlink, "href" ) ) {
        errorFormula << "Object element has no valid xlink:href attribute";
        return false;
    }

    QString url = objectElement.attributeNS( KoXmlNS::xlink, "href" );

    // It can happen that the url is empty e.g. when it is a
    // presentation:placeholder.
    if ( url.isEmpty() ) {
        return true;
    }

    QString tmpURL;
    if ( url[0] == '#' )
        url.remove( 0, 1 );

    #define INTERNAL_PROTOCOL "intern"
    #define STORE_PROTOCOL "tar"

    if (QUrl::fromUserInput(url).isRelative()) {
        if ( url.startsWith( "./" ) )
            tmpURL = QString( INTERNAL_PROTOCOL ) + ":/" + url.mid( 2 );
        else
            tmpURL = QString( INTERNAL_PROTOCOL ) + ":/" + url;
    }
    else
        tmpURL = url;

    QString path = tmpURL;
    if ( tmpURL.startsWith( INTERNAL_PROTOCOL ) ) {
        path = store->currentPath();
        if ( !path.isEmpty() && !path.endsWith( '/' ) )
            path += '/';
        QString relPath = QUrl::fromUserInput(tmpURL).path();
        path += relPath.mid( 1 ); // remove leading '/'
    }
    if ( !path.endsWith( '/' ) )
        path += '/';

    const QString mimeType = odfLoadingContext.mimeTypeForPath( path );
    debugFormula << "path for manifest file=" << path << "mimeType=" << mimeType;
    if ( mimeType.isEmpty() ) {
        debugFormula << "Manifest doesn't have media-type for" << path;
        return false;
    }

    const bool isOdf = mimeType.startsWith( "application/vnd.oasis.opendocument" );
    if ( !isOdf ) {
        tmpURL += "/maindoc.xml";
        debugFormula << "tmpURL adjusted to" << tmpURL;
    }

    //debugFormula << "tmpURL=" << tmpURL;
    QString errorMsg;

    bool res = true;
    if ( tmpURL.startsWith( STORE_PROTOCOL )
        || tmpURL.startsWith( INTERNAL_PROTOCOL )
        || QUrl::fromUserInput(tmpURL).isRelative() )
    {
        if ( isOdf ) {
            store->pushDirectory();
            Q_ASSERT( tmpURL.startsWith( INTERNAL_PROTOCOL ) );
            QString relPath = QUrl::fromUserInput(tmpURL).path().mid( 1 );
            store->enterDirectory( relPath );
            res = loadOasisFromStore( store );
            store->popDirectory();
        } else {
            if ( tmpURL.startsWith( INTERNAL_PROTOCOL ) )
                tmpURL = QUrl::fromUserInput(tmpURL).path().mid( 1 );
            res = loadFromStore( store, tmpURL );
        }
        setStoreInternal( true );
    }
    else {
        // Reference to an external document. Hmmm...
        setStoreInternal( false );
        QUrl url = QUrl::fromUserInput(tmpURL);
        if ( !url.isLocalFile() ) {
            //QApplication::restoreOverrideCursor();

            // For security reasons we need to ask confirmation if the
            // url is remote.
            int result = KMessageBox::warningYesNoCancel(
                0, i18n( "This document contains an external link to a remote document\n%1", tmpURL ),
                   i18n( "Confirmation Required" ), KGuiItem( i18n( "Download" ) ), KGuiItem( i18n( "Skip" ) )
            );
            if ( result == KMessageBox::Cancel ) {
                //d->m_parent->setErrorMessage("USER_CANCELED");
                return false;
            }
            if ( result == KMessageBox::Yes )
                res = openUrl( url );
            // and if == No, res will still be false so we'll use a kounavail below
        }
        else
            res = openUrl( url );
    }

    if ( !res ) {
        return false;
    }

    tmpURL.clear();

    return res;
}

bool FormulaDocument::loadOdfEmbedded( const KoXmlElement &mathElement,
                                      KoShapeLoadingContext &context )
{
    Q_UNUSED(context);

    QDomDocument mathDomDoc;
    KoXml::asQDomElement(mathDomDoc, mathElement);
    QDomElement mathDomElement = mathDomDoc.documentElement();

    QBuffer* buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);
    KoXmlWriter writer(buffer, 0);
    processMathML(mathDomElement, writer);
    buffer->putChar('\0');
    buffer->seek(0);
    d->content = buffer->readAll();
    buffer->close();
    delete buffer;

    debugFormula << d->content.constData();
    return true;
}

// recursively write dom elements using UTF-8 encoding
void FormulaDocument::processMathML(QDomElement &element, KoXmlWriter &writer) {
    // tagName str need to be alive until endElement() is called
    QByteArray tagName = "math:" + element.tagName().toUtf8();

    // annotation element is only useful to libreoffice
    if (tagName == "math:annotation") {
        return;
    }

    // write element
    writer.startElement(tagName.constData(), true);

    // write namespaceURI, for <math> tag
    if (tagName == "math:math") {
        writer.addAttribute("xmlns:math", KoXmlNS::math);
    }
    QDomNamedNodeMap attrMap = element.attributes();
    for (int i=0; i<attrMap.length(); ++i) {
        QDomAttr attr = attrMap.item(i).toAttr();
        // ignore namespaceURI for other tags
        if (attr.name() == "xmlns" && attr.value() == KoXmlNS::math) {
            continue;
        }
        writer.addAttribute(attr.name().toUtf8().constData(), attr.value());
    }

    QDomElement child = element.firstChildElement();
    if (child.isNull()) {
        writer.addTextNode(element.text());
    } else {
        // write children elements
        while (!child.isNull()) {
            processMathML(child, writer);
            child = child.nextSiblingElement();
        }
    }
    writer.endElement();
}

bool FormulaDocument::saveOdf( SavingContext &context )
{
    // FIXME: This code is copied from ChartDocument, so it needs to
    // be adapted to the needs of the KoFormulaShape.

    KoOdfWriteStore &odfStore = context.odfStore;
    KoStore *store = odfStore.store();
    KoXmlWriter *manifestWriter = odfStore.manifestWriter();
    KoXmlWriter *contentWriter  = odfStore.contentWriter();
    if ( !contentWriter )
        return false;

    KoGenStyles mainStyles;
    KoXmlWriter *bodyWriter = odfStore.bodyWriter();
    if ( !bodyWriter )
        return false;

    KoEmbeddedDocumentSaver& embeddedSaver = context.embeddedSaver;

    KoShapeSavingContext savingContext( *bodyWriter, mainStyles, embeddedSaver );

    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:formula" );

    d->parent->saveOdf( savingContext );

    bodyWriter->endElement(); // office:formula
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfStyles( KoGenStyles::DocumentAutomaticStyles, contentWriter );
    odfStore.closeContentWriter();

    // Add manifest line for content.xml and styles.xml
    manifestWriter->addManifestEntry( url().path() + "/content.xml", "text/xml" );
    manifestWriter->addManifestEntry( url().path() + "/styles.xml", "text/xml" );

    // save the styles.xml
    if ( !mainStyles.saveOdfStylesDotXml( store, manifestWriter ) )
        return false;

    if ( !savingContext.saveDataCenter( store, manifestWriter ) ) {
        return false;
    }

    return true;
}

const QByteArray & FormulaDocument::content() {
    return d->content;
}

void FormulaDocument::paintContent( QPainter &painter, const QRect &rect )
{
    Q_UNUSED( painter );
    Q_UNUSED( rect );
}


