/* This file is part of the KDE project
 * Copyright (C) 2012 Pankaj Kumar <me@panks.in>
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

#include "OdfOutputDev.h"
#include <poppler/Object.h>
#include <poppler/GfxState.h>
#include <poppler/Stream.h>
#include <poppler/GfxFont.h>

#include <KDebug>
#include <QFile>
#include <QTextStream>
#include <QSizeF>
#include <QBuffer>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QImage>
#include<QString>

#include <QTextCodec>
#include <QDateTime>
#include <stddef.h>
#include <kpluginfactory.h>
#include <kencodingprober.h>

#include <calligraversion.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoStore.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>





class OdfOutputDev::Private
{
public:
    Private(const QString &fname)
            : odfFile(fname), defs(0), body(0), state(gTrue), outFile(fname)
            , brush(Qt::SolidPattern) {}

    ~Private() {
        delete defs;
        delete body;
    }

    QFile odfFile;
    QString outFile;
    QString bodyData;
    QString defsData;
    QTextStream * defs;
    QTextStream * body;
    GBool state;
    QSizeF pageSize;
    QPen pen;
    QBrush brush;
};

OdfOutputDev::OdfOutputDev(const QString &fileName)
        : d(new Private(fileName))
{
    if (! d->odfFile.open(QIODevice::WriteOnly)) {
        d->state = gFalse;
        return;
    }

    d->body = new QTextStream(&d->bodyData, QIODevice::ReadWrite);
    d->defs = new QTextStream(&d->defsData, QIODevice::ReadWrite);
}

OdfOutputDev::~OdfOutputDev()
{
    delete d;
}

GBool OdfOutputDev::isOk()
{
    return d->state;
}

void OdfOutputDev::beginString(GfxState* state, GooString* s)
{
    *d->body << "<text:p>";
}



void OdfOutputDev::drawChar(GfxState* state, double x, double y, double dx, double dy, double originX, double originY, CharCode code, int nBytes, Unicode* u, int uLen)
{
//     const char* tmp=(char*)u;
//     QString *qtStrData;
//     qtStrData=new QString(tmp);

    for(int i=0; i<uLen; i++){
    *d->body << (QChar)u[i];
        kDebug(30516) << (QChar)u[i];
    }
}



void OdfOutputDev::endString(GfxState* state)
{
    *d->body << "</text:p>" << endl;
}


GBool OdfOutputDev::upsideDown()
{
    return gTrue;
}

GBool OdfOutputDev::useDrawChar()
{
    return gTrue;
}

GBool OdfOutputDev::interpretType3Chars()
{
    return gFalse;
}


// void OdfOutputDev::startPage(int pageNum, GfxState *state)
// {
//     kDebug(30516) << "starting page" << pageNum;
//     d->pageSize = QSizeF(state->getPageWidth(), state->getPageHeight());
//     kDebug(30516) << "page size =" << d->pageSize;
// 
//     *d->body << "<g id=\"" << QString("%1").arg(pageNum, (int)3, (int)10, QLatin1Char('0')) << "\"" << endl;
//     if (pageNum != 1)
//         *d->body << " display=\"none\"";
//     *d->body << ">" << endl;
// }
// 
// void OdfOutputDev::endPage()
// {
//     kDebug(30516) << "ending page";
//     *d->body << "</g>" << endl;
// }

void OdfOutputDev::dumpContent()
{
    kDebug(30516) << "dumping pages";
    KoStore *store = KoStore::createStore(d->outFile, KoStore::Write, "application/vnd.oasis.opendocument.text", KoStore::Zip);
    if (!store || store->bad()) {
        kWarning(30502) << "Unable to open output file!";
        delete store;
    }
    store->disallowNameExpansion();
    kDebug(30502) << "created store.";
    KoOdfWriteStore odfStore(store);
    odfStore.manifestWriter("application/vnd.oasis.opendocument.text");
    
    KoXmlWriter* contentWriter = odfStore.contentWriter();
    if (!contentWriter) {
        kDebug(30502) << "Couldn't create contentWriter.";
        delete store;
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
    QByteArray bytes  = d->bodyData.toLocal8Bit();
    const char * ptr    = bytes.data();
    bodyWriter->addCompleteElement(ptr);
    bodyWriter->endElement(); // office:text
    bodyWriter->endElement(); // office:body
    
    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);
    odfStore.closeContentWriter();

    //add manifest line for content.xml
    odfStore.manifestWriter()->addManifestEntry("content.xml", "text/xml");
    if (!mainStyles.saveOdfStylesDotXml(odfStore.store(), odfStore.manifestWriter())) {
        kWarning() << "Error while saving odfStylesDotXml to mainStyles.";
        delete store;
    }

    if (!createMeta(odfStore)) {
        kWarning() << "Error while trying to write 'meta.xml'. Partition full?";
        delete store;
    }


    if ( !odfStore.closeManifestWriter() ) {
      kWarning() << "Error while trying to write 'META-INF/manifest.xml'. Partition full?";
        delete store;
    }

    delete store;

}


bool OdfOutputDev::createMeta(KoOdfWriteStore &store)
{
    if (!store.store()->open("meta.xml")) {
        return false;
    }

    KoStoreDevice dev(store.store());
    KoXmlWriter* xmlWriter = KoOdfWriteStore::createOasisXmlWriter(&dev, "office:document-meta");
    xmlWriter->startElement("office:meta");

    xmlWriter->startElement("meta:generator");
    xmlWriter->addTextNode(QString("KOConverter/%1").arg(CALLIGRA_VERSION_STRING));
    xmlWriter->endElement();

    xmlWriter->startElement("meta:creation-date");
    xmlWriter->addTextNode(QDateTime::currentDateTime().toString(Qt::ISODate));
    xmlWriter->endElement();

    xmlWriter->endElement();
    xmlWriter->endElement(); // root element
    xmlWriter->endDocument(); // root element
    delete xmlWriter;
    if (!store.store()->close()) {
        return false;
    }
    store.manifestWriter()->addManifestEntry("meta.xml", "text/xml" );
    return true;
}
