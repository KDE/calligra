/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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

#include <powerpointimport.h>
#include <powerpointimport.moc>

#include <QBuffer>
#include <QColor>
#include <QString>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>

#include "libppt.h"
#include <iostream>
#include <math.h>
#include "pictures.h"

using namespace Libppt;

typedef KGenericFactory<PowerPointImport> PowerPointImportFactory;
K_EXPORT_COMPONENT_FACTORY(libpowerpointimport,
                           PowerPointImportFactory("kofficefilters"))

namespace Libppt
{

inline QString string(const Libppt::UString& str)
{
    return QString::fromRawData(reinterpret_cast<const QChar*>(str.data()), str.length());
}

}

class PowerPointImport::Private
{
public:
    QString inputFile;
    QString outputFile;
    QList<QString> pictureNames;

    Presentation *presentation;
};


PowerPointImport::PowerPointImport(QObject*parent, const QStringList&)
        : KoFilter(parent)
{
    d = new Private;
}

PowerPointImport::~PowerPointImport()
{
    delete d;
}

QStringList
createPictures(const char* filename, KoStore* store)
{
    POLE::Storage storage(filename);
    QStringList fileNames;
    if (!storage.open()) return fileNames;
    POLE::Stream* stream = new POLE::Stream(&storage, "/Pictures");
    while (!stream->eof() && !stream->fail()
            && stream->tell() < stream->size()) {
        std::string name = savePicture(*stream, fileNames.size(), store);
        if (name.length() == 0) break;
        fileNames.append(name.c_str());
    }
    storage.close();
    delete stream;
    return fileNames;
}

KoFilter::ConversionStatus PowerPointImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/vnd.ms-powerpoint")
        return KoFilter::NotImplemented;

    if (to != "application/vnd.oasis.opendocument.presentation")
        return KoFilter::NotImplemented;

    d->inputFile = m_chain->inputFile();
    d->outputFile = m_chain->outputFile();

    // open inputFile
    d->presentation = new Presentation;
    if (!d->presentation->load(d->inputFile.toLocal8Bit())) {
        delete d->presentation;
        d->presentation = 0;
        return KoFilter::StupidError;
    }

    // create output store
    KoStore* storeout;
    storeout = KoStore::createStore(d->outputFile, KoStore::Write,
                                    "application/vnd.oasis.opendocument.presentation",   KoStore::Zip);

    if (!storeout) {
        kWarning() << "Couldn't open the requested file.";
        return KoFilter::FileNotFound;
    }

    // store the images from the 'Pictures' stream
    storeout->disallowNameExpansion();
    storeout->enterDirectory("Pictures");
    d->pictureNames = createPictures(d->inputFile.toLocal8Bit(), storeout);
    storeout->leaveDirectory();

    // store document content
    if (!storeout->open("content.xml")) {
        kWarning() << "Couldn't open the file 'content.xml'.";
        return KoFilter::CreationError;
    }
    storeout->write(createContent());
    storeout->close();

    // store document styles
    if (!storeout->open("styles.xml")) {
        kWarning() << "Couldn't open the file 'styles.xml'.";
        return KoFilter::CreationError;
    }
    storeout->write(createStyles());
    storeout->close();

    // store document manifest
    storeout->enterDirectory("META-INF");
    if (!storeout->open("manifest.xml")) {
        kWarning() << "Couldn't open the file 'META-INF/manifest.xml'.";
        return KoFilter::CreationError;
    }
    storeout->write(createManifest());
    storeout->close();

    // we are done!
    delete d->presentation;
    delete storeout;
    d->inputFile.clear();
    d->outputFile.clear();
    d->presentation = 0;

    return KoFilter::OK;
}

QByteArray PowerPointImport::createStyles()
{
    KoXmlWriter* stylesWriter;
    QByteArray stylesData;
    QBuffer stylesBuffer(&stylesData);

    Slide* master = d->presentation->masterSlide();
    QString pageWidth = QString("%1pt").arg((master) ? master->pageWidth() : 0);
    QString pageHeight = QString("%1pt").arg((master) ? master->pageHeight() : 0);

    stylesBuffer.open(QIODevice::WriteOnly);
    stylesWriter = new KoXmlWriter(&stylesBuffer);

    stylesWriter->startDocument("office:document-styles");
    stylesWriter->startElement("office:document-styles");
    stylesWriter->addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    stylesWriter->addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    stylesWriter->addAttribute("xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0");
    stylesWriter->addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    stylesWriter->addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    stylesWriter->addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    contentWriter->addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    stylesWriter->addAttribute("office:version", "1.0");

    // office:styles
    stylesWriter->startElement("office:styles");
    stylesWriter->startElement("draw:marker");
    stylesWriter->addAttribute("draw:name", "msArrowEnd_20_5");
    stylesWriter->addAttribute("draw:display-name", "msArrowEnd 5");
    stylesWriter->addAttribute("svg:viewBox", "0 0 210 210");
    stylesWriter->addAttribute("svg:d", "m105 0 105 210h-210z");
    stylesWriter->endElement();
    stylesWriter->endElement();

    // office:automatic-styles
    stylesWriter->startElement("office:automatic-styles");
    stylesWriter->startElement("style:page-layout");
    stylesWriter->addAttribute("style:name", "pm1");
    stylesWriter->addAttribute("style:page-usage", "all");
    stylesWriter->startElement("style:page-layout-properties");
    stylesWriter->addAttribute("fo:margin-bottom", "0pt");
    stylesWriter->addAttribute("fo:margin-left", "0pt");
    stylesWriter->addAttribute("fo:margin-right", "0pt");
    stylesWriter->addAttribute("fo:margin-top", "0pt");
    stylesWriter->addAttribute("fo:page-height", pageHeight);
    stylesWriter->addAttribute("fo:page-width", pageWidth);
    stylesWriter->addAttribute("style:print-orientation", "landscape");
    stylesWriter->endElement(); // style:page-layout-properties
    stylesWriter->endElement(); // style:page-layout

    stylesWriter->startElement("style:style");
    stylesWriter->addAttribute("style:name", "dp1");
    stylesWriter->addAttribute("style:family", "drawing-page");
    stylesWriter->startElement("style:drawing-page-properties");
    stylesWriter->addAttribute("draw:background-size", "border");
    stylesWriter->addAttribute("draw:fill", "solid");
    stylesWriter->addAttribute("draw:fill-color", "#ffffff");
    stylesWriter->endElement(); // style:drawing-page-properties
    stylesWriter->endElement(); // style:style

    stylesWriter->startElement("style:style");
    stylesWriter->addAttribute("style:name", "P1");
    stylesWriter->addAttribute("style:family", "paragraph");
    stylesWriter->startElement("style:paragraph-properties");
    stylesWriter->addAttribute("fo:margin-left", "0cm");
    stylesWriter->addAttribute("fo:margin-right", "0cm");
    stylesWriter->addAttribute("fo:text-indent", "0cm");
    stylesWriter->endElement(); // style:paragraph-properties
    stylesWriter->startElement("style:text-properties");
    stylesWriter->addAttribute("fo:font-size", "14pt");
    stylesWriter->addAttribute("style:font-size-asian", "14pt");
    stylesWriter->addAttribute("style:font-size-complex", "14pt");
    stylesWriter->endElement(); // style:text-properties
    stylesWriter->endElement(); // style:style

    stylesWriter->startElement("text:list-style");
    stylesWriter->addAttribute("style:name", "L2");
    stylesWriter->startElement("text:list-level-style-bullet");
    stylesWriter->addAttribute("text:level", "1");
    stylesWriter->addAttribute("text:bullet-char", "●");
    stylesWriter->startElement("style:text-properties");
    stylesWriter->addAttribute("fo:font-family", "StarSymbol");
    stylesWriter->addAttribute("font-pitch", "variable");
    stylesWriter->addAttribute("fo:color", "#000000");
    stylesWriter->addAttribute("fo:font-size", "45%");
    stylesWriter->endElement(); // style:text-properties
    stylesWriter->endElement(); // text:list-level-style-bullet
    stylesWriter->endElement(); // text:list-style

    stylesWriter->endElement(); // office:automatic-styles

    // office:master-stylesborder
    stylesWriter->startElement("office:master-styles");
    stylesWriter->startElement("style:master-page");
    stylesWriter->addAttribute("style:name", "Standard");
    stylesWriter->addAttribute("style:page-layout-name", "pm1");
    stylesWriter->addAttribute("draw:style-name", "dp1");
    stylesWriter->endElement();
    stylesWriter->endElement();

    stylesWriter->endElement();  // office:document-styles
    stylesWriter->endDocument();
    delete stylesWriter;
    return stylesData;
}


QByteArray PowerPointImport::createContent()
{
    KoXmlWriter* contentWriter;
    QByteArray contentData;
    QBuffer contentBuffer(&contentData);

    contentBuffer.open(QIODevice::WriteOnly);
    contentWriter = new KoXmlWriter(&contentBuffer);

    contentWriter->startDocument("office:document-content");
    contentWriter->startElement("office:document-content");
    contentWriter->addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    contentWriter->addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    contentWriter->addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    contentWriter->addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    contentWriter->addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    contentWriter->addAttribute("xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0");
    contentWriter->addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    contentWriter->addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    contentWriter->addAttribute("office:version", "1.0");

    // office:automatic-styles


    KoGenStyles styles;
    for (unsigned c = 0; c < d->presentation->slideCount(); c++) {
        Slide* slide = d->presentation->slide(c);
        processSlideForStyle(c, slide, styles);
    }
    styles.saveOdfAutomaticStyles(contentWriter, false);

    // office:body

    contentWriter->startElement("office:body");
    contentWriter->startElement("office:presentation");

    for (unsigned c = 0; c < d->presentation->slideCount(); c++) {
        Slide* slide = d->presentation->slide(c);
        processSlideForBody(c, slide, contentWriter);
    }

    contentWriter->endElement();  // office:presentation
    contentWriter->endElement();  // office:body

    contentWriter->endElement();  // office:document-content
    contentWriter->endDocument();
    delete contentWriter;
    return contentData;
}

QByteArray PowerPointImport::createManifest()
{
    KoXmlWriter* manifestWriter;
    QByteArray manifestData;
    QBuffer manifestBuffer(&manifestData);

    manifestBuffer.open(QIODevice::WriteOnly);
    manifestWriter = new KoXmlWriter(&manifestBuffer);

    manifestWriter->startDocument("manifest:manifest");
    manifestWriter->startElement("manifest:manifest");
    manifestWriter->addAttribute("xmlns:manifest",
                                 "urn:oasis:names:tc:openoffice:xmlns:manifest:1.0");

    manifestWriter->addManifestEntry("/", "application/vnd.oasis.opendocument.presentation");
    manifestWriter->addManifestEntry("styles.xml", "text/xml");
    manifestWriter->addManifestEntry("content.xml", "text/xml");

    manifestWriter->endElement();
    manifestWriter->endDocument();
    delete manifestWriter;
    return manifestData;
}

void PowerPointImport::processEllipse(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:ellipse");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->endElement(); // draw:ellipse
}

void PowerPointImport::processRectangle(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject) return;
    if (!xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:rect");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    if (drawObject->hasProperty("libppt:rotation")) {

        double rotAngle = drawObject->getDoubleProperty("libppt:rotation");
        double xMid = (drawObject->left() + 0.5 * drawObject->width());
        double yMid = (drawObject->top() + 0.5 * drawObject->height());
        double xVec = drawObject->left() - xMid;
        double yVec = yMid - drawObject->top();

        double xNew = xVec * cos(rotAngle) - yVec * sin(rotAngle);
        double yNew = xVec * sin(rotAngle) + yVec * cos(rotAngle);
        QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew + xMid).arg(yMid - yNew);
        xmlWriter->addAttribute("draw:transform", rot);
    } else {
        xmlWriter->addAttribute("svg:x", xStr);
        xmlWriter->addAttribute("svg:y", yStr);
    }
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->endElement(); // draw:rect
}

void PowerPointImport::processRoundRectangle(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());


    if (drawObject->hasProperty("libppt:rotation")) {
        double rotAngle = drawObject->getDoubleProperty("libppt:rotation");



        if (rotAngle > 0.785399) { // > 45 deg
            xmlWriter->addAttribute("svg:width", heightStr);
            xmlWriter->addAttribute("svg:height", widthStr);
            double xMid = (drawObject->left() - 0.5 * drawObject->height());
            double yMid = (drawObject->top() + 0.5 * drawObject->width());
            double xVec = drawObject->left() - xMid;
            double yVec =  drawObject->top() - yMid;

            double xNew = xVec * cos(rotAngle) - yVec * sin(rotAngle);
            double yNew = xVec * sin(rotAngle) + yVec * cos(rotAngle);
            QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew + xMid).arg(yMid + yNew);
            xmlWriter->addAttribute("draw:transform", rot);
        } else {
            xmlWriter->addAttribute("svg:width", widthStr);
            xmlWriter->addAttribute("svg:height", heightStr);
            double xMid = (drawObject->left() + 0.5 * drawObject->width());
            double yMid = (drawObject->top() + 0.5 * drawObject->height());
            double xVec = drawObject->left() - xMid;
            double yVec = yMid - drawObject->top();

            double xNew = xVec * cos(rotAngle) - yVec * sin(rotAngle);
            double yNew = xVec * sin(rotAngle) + yVec * cos(rotAngle);
            QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew + xMid).arg(yMid - yNew);
            xmlWriter->addAttribute("draw:transform", rot);
        }


    } else {
        xmlWriter->addAttribute("svg:width", widthStr);
        xmlWriter->addAttribute("svg:height", heightStr);
        xmlWriter->addAttribute("svg:x", xStr);
        xmlWriter->addAttribute("svg:y", yStr);
    }
// xmlWriter->addAttribute( "svg:x", xStr );
// xmlWriter->addAttribute( "svg:y", yStr );

    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:enhanced-geometry");
    xmlWriter->addAttribute("draw:type", "round-rectangle");
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 /3");
    xmlWriter->addAttribute("draw:name", "f0");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "right-?f0 ");
    xmlWriter->addAttribute("draw:name", "f1");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "bottom-?f0 ");
    xmlWriter->addAttribute("draw:name", "f2");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "left+?f0 ");
    xmlWriter->addAttribute("draw:name", "f3");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "top+?f0 ");
    xmlWriter->addAttribute("draw:name", "f4");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->endElement(); // draw:enhanced-geometry
    xmlWriter->endElement(); // draw:custom-shape
}

void PowerPointImport::processDiamond(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 0);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 10);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:enhanced-geometry");
    xmlWriter->addAttribute("draw:type", "diamond");
    xmlWriter->endElement();
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->endElement();
}

void PowerPointImport::processTriangle(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    /* draw IsocelesTriangle or RightTriangle */
    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 2.5);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 0);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 10);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 7.5);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();

    xmlWriter->startElement("draw:enhanced-geometry");

    if (drawObject->hasProperty("draw:mirror-vertical")) {
        xmlWriter->addAttribute("draw:mirror-vertical", "true");
    }
    if (drawObject->hasProperty("draw:mirror-horizontal")) {
        xmlWriter->addAttribute("draw:mirror-horizontal", "true");
    }
    if (drawObject->hasProperty("libppt:rotation")) { // draw:transform="rotate (1.5707963267946) translate (6.985cm 14.181cm)"

        double rotAngle = drawObject->getDoubleProperty("libppt:rotation");
        double xMid = (drawObject->left() + 0.5 * drawObject->width());
        double yMid = (drawObject->top() + 0.5 * drawObject->height());
        QString rot = QString("rotate (%1) translate (%2cm %3cm)").arg(rotAngle).arg(xMid).arg(yMid);
        xmlWriter->addAttribute("draw:transform", rot);
    }
    if (drawObject->shape() == DrawObject::RightTriangle) {
        xmlWriter->addAttribute("draw:type", "right-triangle");
    } else if (drawObject->shape() == DrawObject::IsoscelesTriangle) {
        xmlWriter->addAttribute("draw:type", "isosceles-triangle");
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "$0 ");
        xmlWriter->addAttribute("draw:name", "f0");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "$0 /2");
        xmlWriter->addAttribute("draw:name", "f1");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "?f1 +10800");
        xmlWriter->addAttribute("draw:name", "f2");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "$0 *2/3");
        xmlWriter->addAttribute("draw:name", "f3");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "?f3 +7200");
        xmlWriter->addAttribute("draw:name", "f4");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "21600-?f0 ");
        xmlWriter->addAttribute("draw:name", "f5");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "?f5 /2");
        xmlWriter->addAttribute("draw:name", "f6");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:equation");
        xmlWriter->addAttribute("draw:formula", "21600-?f6 ");
        xmlWriter->addAttribute("draw:name", "f7");
        xmlWriter->endElement();
        xmlWriter->startElement("draw:handle");
        xmlWriter->addAttribute("draw:handle-range-x-maximum", 21600);
        xmlWriter->addAttribute("draw:handle-range-x-minimum", 0);
        xmlWriter->addAttribute("draw:handle-position", "$0 top");
        xmlWriter->endElement();
    }

    xmlWriter->endElement();    // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processTrapezoid(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");

    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 2.5);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 0);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:enhanced-geometry");
    if (drawObject->hasProperty("draw:mirror-vertical")) {
        xmlWriter->addAttribute("draw:mirror-vertical", "true");
    }
    if (drawObject->hasProperty("draw:mirror-horizontal")) {
        xmlWriter->addAttribute("draw:mirror-horizontal", "true");
    }
    xmlWriter->addAttribute("draw:type", "trapezoid");
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-$0 ");
    xmlWriter->addAttribute("draw:name", "f0");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0");
    xmlWriter->addAttribute("draw:name", "f1");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 *10/18");
    xmlWriter->addAttribute("draw:name", "f2");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f2 +1750");
    xmlWriter->addAttribute("draw:name", "f3");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-?f3");
    xmlWriter->addAttribute("draw:name", "f4");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 /2");
    xmlWriter->addAttribute("draw:name", "f5");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-?f5");
    xmlWriter->addAttribute("draw:name", "f6");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:handle");
    xmlWriter->addAttribute("draw:handle-range-x-maximum", 10800);
    xmlWriter->addAttribute("draw:handle-range-x-minimum", 0);
    xmlWriter->addAttribute("draw:handle-position", "$0 bottom");
    xmlWriter->endElement();
    xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processParallelogram(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 6.25);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 4.5);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 8.75);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 3.75);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 1.25);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:enhanced-geometry");
    if (drawObject->hasProperty("draw:mirror-vertical")) {
        xmlWriter->addAttribute("draw:mirror-vertical", "true");
    }
    if (drawObject->hasProperty("draw:mirror-horizontal")) {
        xmlWriter->addAttribute("draw:mirror-horizontal", "true");
    }
    xmlWriter->addAttribute("draw:type", "parallelogram");
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 ");
    xmlWriter->addAttribute("draw:name", "f0");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-$0");
    xmlWriter->addAttribute("draw:name", "f1");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 *10/24");
    xmlWriter->addAttribute("draw:name", "f2");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f2 +1750");
    xmlWriter->addAttribute("draw:name", "f3");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-?f3");
    xmlWriter->addAttribute("draw:name", "f4");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f0 /2");
    xmlWriter->addAttribute("draw:name", "f5");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "10800+?f5");
    xmlWriter->addAttribute("draw:name", "f6");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f0-10800 ");
    xmlWriter->addAttribute("draw:name", "f7");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "if(?f7,?f12,0");
    xmlWriter->addAttribute("draw:name", "f8");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "10800-?f5");
    xmlWriter->addAttribute("draw:name", "f9");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "if(?f7, ?f12, 21600");
    xmlWriter->addAttribute("draw:name", "f10");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-?f5");
    xmlWriter->addAttribute("draw:name", "f11");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600*10800/?f0");
    xmlWriter->addAttribute("draw:name", "f12");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-?f12");
    xmlWriter->addAttribute("draw:name", "f13");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:handle");
    xmlWriter->addAttribute("draw:handle-range-x-maximum", 21600);
    xmlWriter->addAttribute("draw:handle-range-x-minimum", 0);
    xmlWriter->addAttribute("draw:handle-position", "$0 top");
    xmlWriter->endElement();
    xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processHexagon(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 0);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 10);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:enhanced-geometry");
    xmlWriter->addAttribute("draw:type", "hexagon");
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 ");
    xmlWriter->addAttribute("draw:name", "f0");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-$0");
    xmlWriter->addAttribute("draw:name", "f1");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 *100/234");
    xmlWriter->addAttribute("draw:name", "f2");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f2 +1700");
    xmlWriter->addAttribute("draw:name", "f3");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-?f3");
    xmlWriter->addAttribute("draw:name", "f4");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:handle");
    xmlWriter->addAttribute("draw:handle-range-x-maximum", 10800);
    xmlWriter->addAttribute("draw:handle-range-x-minimum", 0);
    xmlWriter->addAttribute("draw:handle-position", "$0 top");
    xmlWriter->endElement();
    xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processOctagon(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 0);
    xmlWriter->addAttribute("svg:y", 4.782);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 10);
    xmlWriter->addAttribute("svg:y", 4.782);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:enhanced-geometry");
    xmlWriter->addAttribute("draw:type", "octagon");
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "left+$0 ");
    xmlWriter->addAttribute("draw:name", "f0");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "top+$0 ");
    xmlWriter->addAttribute("draw:name", "f1");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "right-$0 ");
    xmlWriter->addAttribute("draw:name", "f2");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "bottom-$0 ");
    xmlWriter->addAttribute("draw:name", "f3");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0 /2");
    xmlWriter->addAttribute("draw:name", "f4");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "left+?f4 ");
    xmlWriter->addAttribute("draw:name", "f5");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "top+?f4 ");
    xmlWriter->addAttribute("draw:name", "f6");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "right-?f4 ");
    xmlWriter->addAttribute("draw:name", "f7");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "bottom-?f4 ");
    xmlWriter->addAttribute("draw:name", "f8");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:handle");
    xmlWriter->addAttribute("draw:handle-range-x-maximum", 10800);
    xmlWriter->addAttribute("draw:handle-range-x-minimum", 0);
    xmlWriter->addAttribute("draw:handle-position", "$0 top");
    xmlWriter->endElement();
    xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processArrow(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:enhanced-geometry");

    if (drawObject->shape() == DrawObject::RightArrow)
        xmlWriter->addAttribute("draw:type", "right-arrow");
    else if (drawObject->shape() == DrawObject::LeftArrow)
        xmlWriter->addAttribute("draw:type", "left-arrow");
    else if (drawObject->shape() == DrawObject::UpArrow)
        xmlWriter->addAttribute("draw:type", "up-arrow");
    else if (drawObject->shape() == DrawObject::DownArrow)
        xmlWriter->addAttribute("draw:type", "down-arrow");
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$1");
    xmlWriter->addAttribute("draw:name", "f0");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0");
    xmlWriter->addAttribute("draw:name", "f1");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-$1");
    xmlWriter->addAttribute("draw:name", "f2");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "21600-?f1");
    xmlWriter->addAttribute("draw:name", "f3");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f3 *?f0 /10800");
    xmlWriter->addAttribute("draw:name", "f4");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f1 +?f4 ");
    xmlWriter->addAttribute("draw:name", "f5");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f1 *?f0 /10800");
    xmlWriter->addAttribute("draw:name", "f6");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "?f1 -?f6 ");
    xmlWriter->addAttribute("draw:name", "f7");
    xmlWriter->endElement(); // draw:equation
    xmlWriter->startElement("draw:handle");
    if (drawObject->shape() == DrawObject::RightArrow || drawObject->shape() == DrawObject::LeftArrow) {
        xmlWriter->addAttribute("draw:handle-range-x-maximum", 21600);
        xmlWriter->addAttribute("draw:handle-range-x-minimum", 0);
        xmlWriter->addAttribute("draw:handle-position", "$0 $1");
        xmlWriter->addAttribute("draw:handle-range-y-maximum", 10800);
        xmlWriter->addAttribute("draw:handle-range-y-minimum", 0);
    } else if (drawObject->shape() == DrawObject::UpArrow || drawObject->shape() == DrawObject::DownArrow) {
        xmlWriter->addAttribute("draw:handle-range-x-maximum", 10800);
        xmlWriter->addAttribute("draw:handle-range-x-minimum", 0);
        xmlWriter->addAttribute("draw:handle-position", "$1 $0");
        xmlWriter->addAttribute("draw:handle-range-y-maximum", 21600);
        xmlWriter->addAttribute("draw:handle-range-y-minimum", 0);
    }
    xmlWriter->endElement(); // draw:handle
    xmlWriter->endElement(); // draw:enhanced-geometry
    xmlWriter->endElement(); // draw:custom-shape
}

void PowerPointImport::processLine(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString x1Str = QString("%1mm").arg(drawObject->left());
    QString y1Str = QString("%1mm").arg(drawObject->top());
    QString x2Str = QString("%1mm").arg(drawObject->left() + drawObject->width());
    QString y2Str = QString("%1mm").arg(drawObject->top() + drawObject->height());

    if (drawObject->hasProperty("draw:mirror-vertical")) {
        QString temp = y1Str;
        y1Str = y2Str;
        y2Str = temp;
    }
    if (drawObject->hasProperty("draw:mirror-horizontal")) {
        QString temp = x1Str;
        x1Str = x2Str;
        x2Str = temp;
    }

    xmlWriter->startElement("draw:line");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:y1", y1Str);
    xmlWriter->addAttribute("svg:y2", y2Str);
    xmlWriter->addAttribute("svg:x1", x1Str);
    xmlWriter->addAttribute("svg:x2", x2Str);
    xmlWriter->addAttribute("draw:layer", "layout");

    xmlWriter->endElement();
}

void PowerPointImport::processSmiley(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 0);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 1.461);
    xmlWriter->addAttribute("svg:y", 1.461);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 0);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 1.461);
    xmlWriter->addAttribute("svg:y", 8.536);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 10);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 8.536);
    xmlWriter->addAttribute("svg:y", 1.461);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:enhanced-geometry");
    xmlWriter->addAttribute("draw:type", "smiley");
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "$0-15510 ");
    xmlWriter->addAttribute("draw:name", "f0");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "17520-?f0");
    xmlWriter->addAttribute("draw:name", "f1");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:equation");
    xmlWriter->addAttribute("draw:formula", "15510+?f0");
    xmlWriter->addAttribute("draw:name", "f2");
    xmlWriter->endElement();
    xmlWriter->startElement("draw:handle");
    xmlWriter->addAttribute("draw:position", 10800);
    xmlWriter->addAttribute("draw:handle-range-y-maximum", 17520);
    xmlWriter->addAttribute("draw:handle-range-y-minimum", 15510);
    xmlWriter->addAttribute("draw:handle-position", "$0 top");
    xmlWriter->endElement();
    xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processHeart(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 1);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 1.43);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 5);
    xmlWriter->addAttribute("svg:y", 10);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:glue-point");
    xmlWriter->addAttribute("svg:x", 8.553);
    xmlWriter->addAttribute("svg:y", 5);
    xmlWriter->endElement();
    xmlWriter->startElement("draw:enhanced-geometry");
    xmlWriter->addAttribute("draw:type", "heart");

    xmlWriter->endElement(); // enhanced-geometry
    xmlWriter->endElement(); // custom-shape
}

void PowerPointImport::processFreeLine(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:path");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->endElement(); // path
}

void PowerPointImport::processPictureFrame(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{
    if (!drawObject || !xmlWriter) return;

    int picturePosition = drawObject->getIntProperty("pib") - 1;
    QString url;
    if (picturePosition >= 0 && picturePosition < d->pictureNames.size()) {
        url = "Pictures/" + d->pictureNames[picturePosition];
    } else {
        url = "Error:" + QString::number(d->pictureNames.size())
              + " != " + QString::number(picturePosition);
        kWarning() << "Picture index is out of range.";
    }
    QString widthStr = QString("%1mm").arg(drawObject->width());
    QString heightStr = QString("%1mm").arg(drawObject->height());
    QString xStr = QString("%1mm").arg(drawObject->left());
    QString yStr = QString("%1mm").arg(drawObject->top());

    xmlWriter->startElement("draw:frame");
    xmlWriter->addAttribute("draw:style-name", drawObject->styleName());
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->startElement("draw:image");
    xmlWriter->addAttribute("xlink:href", url);
    xmlWriter->addAttribute("xlink:type", "simple");
    xmlWriter->addAttribute("xlink:show", "embed");
    xmlWriter->addAttribute("xlink:actuate", "onLoad");
    xmlWriter->endElement(); // image
    xmlWriter->endElement(); // frame
}

void PowerPointImport::processDrawingObjectForBody(DrawObject* drawObject, KoXmlWriter* xmlWriter)
{

    if (!drawObject || !xmlWriter) return;

    if (drawObject->shape() == DrawObject::Ellipse) {
        processEllipse(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Rectangle) {
        processRectangle(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::RoundRectangle) {
        processRoundRectangle(drawObject, xmlWriter);
    } else  if (drawObject->shape() == DrawObject::Diamond) {
        processDiamond(drawObject, xmlWriter);
    } else  if (drawObject->shape() == DrawObject::IsoscelesTriangle ||
                drawObject->shape() == DrawObject::RightTriangle) {
        processTriangle(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Trapezoid) {
        processTrapezoid(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Parallelogram) {
        processParallelogram(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Hexagon) {
        processHexagon(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Octagon) {
        processOctagon(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::RightArrow ||
               drawObject->shape() == DrawObject::LeftArrow ||
               drawObject->shape() == DrawObject::UpArrow ||
               drawObject->shape() == DrawObject::DownArrow) {
        processArrow(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Line) {
        processLine(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Smiley) {
        processSmiley(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::Heart) {
        processHeart(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::FreeLine) {
        processFreeLine(drawObject, xmlWriter);
    } else if (drawObject->shape() == DrawObject::PictureFrame) {
        processPictureFrame(drawObject, xmlWriter);
    }
}

void PowerPointImport::processGroupObjectForBody(GroupObject* groupObject,
        KoXmlWriter* xmlWriter)
{
    if (!groupObject || !xmlWriter) return;
    if (!groupObject->objectCount()) return;

    xmlWriter->startElement("draw:g");

    for (unsigned i = 0; i < groupObject->objectCount(); i++) {
        Object* object = groupObject->object(i);
        if (object)
            processObjectForBody(object, xmlWriter);
    }

    xmlWriter->endElement(); // draw:g
}

void PowerPointImport::writeTextObjectIndent(KoXmlWriter* xmlWriter,
        const unsigned int count,
        const QString &style)
{
    //[MS-PPT].pdf says the itendation level can be 4 at most
    for (unsigned int i = 0;i < count && i < 4;i++) {
        xmlWriter->startElement("text:list");


        if (!style.isEmpty() && i == 0) {
            xmlWriter->addAttribute("text:style-name", style);
        }

        if (i != count - 1) { //We only want to have a list at the specified level
            //not the list item. So we'll ignore the last item
            xmlWriter->startElement("text:list-item");
        }
    }
}

void PowerPointImport::writeTextObjectDeIndent(KoXmlWriter* xmlWriter,
        const unsigned int count)
{

    //[MS-PPT].pdf says the itendation level can be 4 at most
    for (unsigned int i = 0;i < count && i < 4;i++) {
        xmlWriter->endElement(); // list
        if (i != 0) {
            //We assume the intendation starts at list level, so we'll
            //ignore the first list-item
            xmlWriter->endElement(); // list-item
        }
    }
}



void PowerPointImport::writeTextCFException(KoXmlWriter* xmlWriter,
        TextCFException *cf,
        TextPFException *pf,
        TextObject *textObject,
        const QString &text,
        bool bullet)
{
    if (bullet) {
        xmlWriter->startElement("text:span");
        xmlWriter->addAttribute("text:style-name", textObject->textStyleName(cf, pf));
    }

    xmlWriter->addTextSpan(text);

    if (bullet) {
        xmlWriter->endElement(); // text:span
    }
}

void PowerPointImport::writeTextLine(KoXmlWriter* xmlWriter,
                                     TextPFException *pf,
                                     TextObject *textObject,
                                     const QString &text,
                                     const unsigned int linePosition,
                                     bool bullet)
{
    StyleTextPropAtom *atom = textObject->styleTextProperty();
    QString part = "";
    TextCFRun *cf = atom->findTextCFRun(linePosition);

    if (!cf) {
        return;
    }

    if (text.isEmpty()) {
        writeTextCFException(xmlWriter, cf->textCFException(), pf, textObject, text, bullet);
        return;
    }


    //Iterate through all the characters in text
    for (int i = 0;i < text.length();i++) {
        TextCFRun *nextCFRun = atom->findTextCFRun(linePosition + i);

        //While character exception stays the same
        if (cf == nextCFRun) {
            //Catenate strings to our substring
            part += text[i];
        } else {
            //When exception changes we write the text to xmlwriter unless the text style
            //name stays the same, then we'll reuse the same stylename for the next
            //character exception
            if (nextCFRun && textObject->textStyleName(cf->textCFException(), pf) != textObject->textStyleName(nextCFRun->textCFException(), pf)) {
                writeTextCFException(xmlWriter, cf->textCFException(), pf, textObject, part, bullet);
                part = text[i];
            } else {
                part += text[i];
            }

            cf = nextCFRun;
        }
    }

    //If at the end we still have some text left, write it out
    if (!part.isEmpty()) {
        writeTextCFException(xmlWriter, cf->textCFException(), pf, textObject, part, bullet);
    }
}

void PowerPointImport::writeTextPFException(KoXmlWriter* xmlWriter,
        TextPFRun *pf,
        TextObject *textObject,
        const unsigned int textPos,
        unsigned int &indent,
        QString &listStyle)
{
    StyleTextPropAtom *atom = textObject->styleTextProperty();
    if (!atom || !pf || !textObject) {
        return;
    }

    QString text = textObject->text().mid(textPos, pf->count());
    //Text lines are separated with carriage return
    //There seems to be an extra carriage return at the end. We'll remove the last
    // carriage return so we don't end up with a single empty line in the end
    if (text.endsWith("\r")) {
        text = text.left(text.length() - 1);
    }

    //Then split the text into lines
    QStringList lines = text.split("\r");
    unsigned int linePos = textPos;


    //Indentaion level paragraph wants
    unsigned int paragraphIndent = pf->indentLevel() + 1;

    bool bullet = false;
    if (pf->textPFException()->hasBullet()) {
        bullet = pf->textPFException()->bullet();
    } else {
        TextPFException *masterPF = masterTextPFException(pf->indentLevel() + 1,
                                    pf->textPFException()->indent());
        if (masterPF && masterPF->hasBullet()) {
            bullet = masterPF->bullet();
        }
    }

    //Handle all lines
    for (int i = 0;i < lines.size();i++) {
        if (!bullet && paragraphIndent == 1) {
            paragraphIndent = 0;
        }
        //If list style changes we'll have to get "out" of the list
        TextCFRun *cf = atom->findTextCFRun(linePos);

        if (!cf) {
            return;
        }

        QString currentStyle = textObject->listStyleName(cf->textCFException(), pf->textPFException());

        if (listStyle != currentStyle) {
            writeTextObjectDeIndent(xmlWriter, indent);
            writeTextObjectIndent(xmlWriter, paragraphIndent, currentStyle);
        } else if (indent < paragraphIndent) {
            writeTextObjectIndent(xmlWriter, paragraphIndent - indent, "");
        } else if (indent > paragraphIndent) {
            writeTextObjectDeIndent(xmlWriter, indent - paragraphIndent);
        }

        indent = paragraphIndent;

        listStyle = currentStyle;
        if (bullet || paragraphIndent > 0) {
            xmlWriter->startElement("text:list-item");
        }

        xmlWriter->startElement("text:p");
        xmlWriter->addAttribute("text:style-name", textObject->paragraphStyleName(cf->textCFException(), pf->textPFException()));
        writeTextLine(xmlWriter, pf->textPFException(), textObject, lines[i], linePos, bullet);

        //Add +1 to line position to compensate for carriage return that is missing
        //from line due to the split method
        linePos += lines[i].size() + 1;

        xmlWriter->endElement(); // text:p
        if (bullet || paragraphIndent > 0) {
            xmlWriter->endElement(); // text:list-item
        }
    }
}


void PowerPointImport::processTextObjectForBody(TextObject* textObject, KoXmlWriter* xmlWriter)
{
    if (!textObject || !xmlWriter) return;

    QString classStr = "subtitle";
    if (textObject->type() == TextObject::Title)
        classStr = "title";

    QString widthStr = QString("%1mm").arg(textObject->width());
    QString heightStr = QString("%1mm").arg(textObject->height());
    QString xStr = QString("%1mm").arg(textObject->left());
    QString yStr = QString("%1mm").arg(textObject->top());

    xmlWriter->startElement("draw:frame");
    xmlWriter->addAttribute("presentation:style-name", "pr1");
    xmlWriter->addAttribute("draw:layer", "layout");
    xmlWriter->addAttribute("svg:width", widthStr);
    xmlWriter->addAttribute("svg:height", heightStr);
    xmlWriter->addAttribute("svg:x", xStr);
    xmlWriter->addAttribute("svg:y", yStr);
    xmlWriter->addAttribute("presentation:class", classStr);
    xmlWriter->startElement("draw:text-box");


    StyleTextPropAtom *atom = textObject->styleTextProperty();
    if (atom) {
        //Paragraph formatting that applies to substring
        TextPFRun *pf = 0;

        unsigned int indent = 0;
        QString listStyleName = "";
        pf = atom->findTextPFRun(0);
        unsigned int index = 0;
        int pos = 0;

        while (pos < textObject->text().length()) {
            if (!pf) {
                kWarning() << "Failed to get text paragraph exception!";
                return;
            }

            writeTextPFException(xmlWriter,
                                 pf,
                                 textObject,
                                 pos,
                                 indent,
                                 listStyleName);

            pos += pf->count();
            index++;
            pf = atom->textPFRun(index);
        }

        if (indent > 0) {
            writeTextObjectDeIndent(xmlWriter, indent);
        }
    } else {
        xmlWriter->startElement("text:span");
        xmlWriter->addTextSpan(textObject->text());
        xmlWriter->endElement(); // text:span
    }

    xmlWriter->endElement(); // draw:text-box
    xmlWriter->endElement(); // draw:frame
}

void PowerPointImport::processObjectForBody(Object* object, KoXmlWriter* xmlWriter)
{
    if (!object ||  !xmlWriter) return;

    if (object->isText())
        processTextObjectForBody(static_cast<TextObject*>(object), xmlWriter);
    else if (object->isGroup())
        processGroupObjectForBody(static_cast<GroupObject*>(object), xmlWriter);
    else if (object->isDrawing())
        processDrawingObjectForBody(static_cast<DrawObject*>(object), xmlWriter);
}

void PowerPointImport::processSlideForBody(unsigned slideNo, Slide* slide, KoXmlWriter* xmlWriter)
{
    if (!slide || !xmlWriter) return;

    QString nameStr = slide->title();
    if (nameStr.isEmpty())
        nameStr = QString("page%1").arg(slideNo + 1);

    QString styleNameStr = QString("dp%1").arg(slideNo + 1);

    xmlWriter->startElement("draw:page");
    xmlWriter->addAttribute("draw:master-page-name", "Default");
    xmlWriter->addAttribute("draw:name", nameStr);
    xmlWriter->addAttribute("draw:style-name", styleNameStr);
    xmlWriter->addAttribute("presentation:presentation-page-layout-name", "AL1T0");

    GroupObject* root = slide->rootObject();
    if (root)
        for (unsigned i = 0; i < root->objectCount(); i++) {
            Object* object = root->object(i);
            if (object)
                processObjectForBody(object, xmlWriter);
        }

    xmlWriter->endElement(); // draw:page
}

void PowerPointImport::processSlideForStyle(unsigned , Slide* slide, KoGenStyles &styles)
{
    if (!slide) return;

    GroupObject* root = slide->rootObject();
    if (root)
        for (unsigned int i = 0; i < root->objectCount(); i++) {
            Object* object = root->object(i);
            if (object)
                processObjectForStyle(object, styles);
        }
}

void PowerPointImport::processObjectForStyle(Object* object, KoGenStyles &styles)
{
    if (!object) return;

    if (object->isText())
        processTextObjectForStyle(static_cast<TextObject*>(object), styles);
    else if (object->isGroup())
        processGroupObjectForStyle(static_cast<GroupObject*>(object), styles);
    else if (object->isDrawing())
        processDrawingObjectForStyle(static_cast<DrawObject*>(object), styles);
}

QString PowerPointImport::paraSpacingToCm(int value) const
{
    if (value < 0) {
        unsigned int temp = -value;
        return pptMasterUnitToCm(temp);
    }

    return QString("%1%").arg(value);
}

QString PowerPointImport::pptMasterUnitToCm(unsigned int value) const
{
    qreal result = value;
    result *= 2.54;
    result /= 576;
    return QString("%1cm").arg(result);
}

QString PowerPointImport::textAlignmentToString(unsigned int value) const
{
    switch (value) {
        /**
        Tx_ALIGNLeft            0x0000 For horizontal text, left aligned.
                                   For vertical text, top aligned.
        */
    case 0:
        return "left";
        /**
        Tx_ALIGNCenter          0x0001 For horizontal text, centered.
                                   For vertical text, middle aligned.
        */
    case 1:
        return "center";
        /**
        Tx_ALIGNRight           0x0002 For horizontal text, right aligned.
                                   For vertical text, bottom aligned.
        */
    case 2:
        return "right";

        /**
        Tx_ALIGNJustify         0x0003 For horizontal text, flush left and right.
                                   For vertical text, flush top and bottom.
        */
        return "justify";

        //TODO these were missing from ODF specification v1.1, but are
        //in [MS-PPT].pdf

        /**
        Tx_ALIGNDistributed     0x0004 Distribute space between characters.
        */
    case 4:

        /**
        Tx_ALIGNThaiDistributed 0x0005 Thai distribution justification.
        */
    case 5:

        /**
        Tx_ALIGNJustifyLow      0x0006 Kashida justify low.
        */
    case 6:
        return "";

        //TODO these two are in ODF specification v1.1 but are missing from
        //[MS-PPT].pdf
        //return "end";
        //return "start";
    }

    return QString();
}


QColor PowerPointImport::colorIndexStructToQColor(const ColorIndexStruct &color)
{
    if (color.index() == 0xFE) {
        return QColor(color.red(), color.green(), color.blue());
    }

    MainMasterContainer *mainMaster = d->presentation->getMainMasterContainer();
    if (mainMaster) {
        return mainMaster->getSlideSchemeColorSchemeAtom()->getColor(color.index());
    }

    kError() << "failed to get main master!";

    return QColor();
}

TextPFException *PowerPointImport::masterTextPFException(unsigned int atom, unsigned int level)
{
    MainMasterContainer *mainMaster = d->presentation->getMainMasterContainer();
    if (!mainMaster) {
        return 0;
    }

    TxMasterStyleAtom *style = mainMaster->textMasterStyleAtom(atom);
    if (!style) {
        return 0;
    }

    TextMasterStyleLevel *styleLevel = style->level(level);
    if (!styleLevel) {
        return 0;
    }

    return styleLevel->pf();
}

TextCFException *PowerPointImport::masterTextCFException(unsigned int atom, unsigned int level)
{
    MainMasterContainer *mainMaster = d->presentation->getMainMasterContainer();
    if (!mainMaster) {
        return 0;
    }

    TxMasterStyleAtom *style = mainMaster->textMasterStyleAtom(atom);
    if (!style) {
        return 0;
    }

    TextMasterStyleLevel *styleLevel = style->level(level);
    if (!styleLevel) {
        return 0;
    }

    return styleLevel->cf();
}


void PowerPointImport::processTextExceptionsForStyle(TextCFRun *cf,
        TextPFRun *pf,
        KoGenStyles &styles,
        TextObject* textObject)
{
    if (!textObject) {
        return;
    }

    //TODO the atom values to fetch masterCF/PF exceptions are hard coded, need to
    //figure out what values should really be used
    //TODO need to parse default style and use that as well
    TextCFException *masterCF = masterTextCFException(1,
                                pf->indentLevel());

    TextPFException *masterPF = masterTextPFException(1,
                                pf->indentLevel());

    if (!masterCF || !masterPF) {
        kWarning() << "Failed to get master pf/cf exception!";
        return;
    }

    KoGenStyle styleParagraph(KoGenStyle::StyleAuto, "paragraph");
    if (pf->textPFException()->hasLeftMargin()) {
        styleParagraph.addProperty("fo:margin-left",
                                   pptMasterUnitToCm(pf->textPFException()->leftMargin()),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF->hasLeftMargin()) {
            styleParagraph.addProperty("fo:margin-left",
                                       pptMasterUnitToCm(masterPF->leftMargin()),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf->textPFException()->hasIndent()) {
        styleParagraph.addProperty("fo:text-indent",
                                   pptMasterUnitToCm(pf->textPFException()->indent()),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF->hasIndent()) {
            styleParagraph.addProperty("fo:text-indent",
                                       pptMasterUnitToCm(masterPF->indent()),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf->textPFException()->hasAlign()) {
        styleParagraph.addProperty("fo:text-align",
                                   textAlignmentToString(pf->textPFException()->textAlignment()),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF->hasAlign()) {
            styleParagraph.addProperty("fo:text-align",
                                       textAlignmentToString(masterPF->textAlignment()),
                                       KoGenStyle::ParagraphType);
        }
    }

    //Text style
    KoGenStyle styleText(KoGenStyle::StyleTextAuto, "text");
    if (cf->textCFException()->hasColor()) {
        styleText.addProperty("fo:color",
                              colorIndexStructToQColor(cf->textCFException()->color()).name(),
                              KoGenStyle::TextType);
    } else {

        //Make sure that character formatting has color aswell
        if (masterCF->hasColor()) {
            styleText.addProperty("fo:color",
                                  colorIndexStructToQColor(masterCF->color()).name(),
                                  KoGenStyle::TextType);
        }
    }

    if (cf->textCFException()->hasFontSize()) {
        styleText.addProperty("fo:font-size",
                              QString("%1pt").arg(cf->textCFException()->fontSize()),
                              KoGenStyle::TextType);
    } else {
        if (masterCF->hasFontSize()) {
            styleText.addProperty("fo:font-size",
                                  QString("%1pt").arg(masterCF->fontSize()),
                                  KoGenStyle::TextType);
        }
    }

    if (cf->textCFException()->hasItalic()) {
        if (cf->textCFException()->italic()) {
            styleText.addProperty("fo:font-style",
                                  "italic",
                                  KoGenStyle::TextType);
        }
    } else {
        if (masterCF->hasItalic() && masterCF->italic()) {
            styleText.addProperty("fo:font-style",
                                  "italic",
                                  KoGenStyle::TextType);
        }
    }

    if (cf->textCFException()->hasBold()) {
        if (cf->textCFException()->bold()) {
            styleText.addProperty("fo:font-weight",
                                  "bold",
                                  KoGenStyle::TextType);
        }
    } else {
        if (masterCF->hasBold() && masterCF->bold()) {
            styleText.addProperty("fo:font-weight",
                                  "bold",
                                  KoGenStyle::TextType);
        }
    }


//  if (cf->textCFException()->hasFont()) {
//      TextFont font = d->presentation->getFont(cf->textCFException()->fontRef());
//      styleText.addProperty( "fo:font-family", font.name() );
//    } else {
//      if (masterCF->hasFont()) {
//        TextFont font = d->presentation->getFont(masterCF->fontRef());
//        styleText.addProperty( "fo:font-family", font.name() );
//      }
//    }


    if (cf->textCFException()->hasFontSize()) {
        styleText.addProperty("fo:font-size",
                              QString("%1pt").arg(cf->textCFException()->fontSize()),
                              KoGenStyle::TextType);

    } else {
        if (masterCF->hasFontSize()) {
            styleText.addProperty("fo:font-size",
                                  QString("%1pt").arg(masterCF->fontSize()),
                                  KoGenStyle::TextType);
        }
    }


    KoGenStyle styleList(KoGenStyle::StyleListAuto, "text");
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);    // TODO pass indentation level


    for (int i = 0;i < (int)pf->indentLevel() + 1;i++) {
        TextCFException *levelCF = masterTextCFException(i,
                                   pf->textPFException()->indent());

        TextPFException *levelPF = masterTextPFException(i,
                                   pf->textPFException()->indent());
        if (!levelCF || !levelPF) {
            kWarning() << "Failed to get master pf/cf exception for level" << i << "!";
            return;
        }

        elementWriter.startElement("text:list-level-style-bullet");
        elementWriter.addAttribute("text:level", i + 1);


        if (i == pf->textPFException()->indent() && pf->textPFException()->hasBulletChar()) {
            elementWriter.addAttribute("text:bullet-char", QChar(pf->textPFException()->bulletChar()));
        } else {
            if (levelPF->hasBulletChar()) {
                elementWriter.addAttribute("text:bullet-char", QChar(levelPF->bulletChar()));
            } else {
                //Should we omit default bullet character?
                elementWriter.addAttribute("text:bullet-char", "●");
            }
        }

        elementWriter.startElement("style:list-level-properties");


        if (i == pf->textPFException()->indent() && pf->textPFException()->hasSpaceAfter()) {
            elementWriter.addAttribute("text:space-after",
                                       pptMasterUnitToCm(pf->textPFException()->spaceAfter()));
        } else {
            if (levelPF->hasSpaceAfter()) {
                elementWriter.addAttribute("text:space-after",
                                           pptMasterUnitToCm(levelPF->spaceAfter()));
            }
        }

        elementWriter.endElement(); // style:list-level-properties

        elementWriter.startElement("style:text-properties");
        TextFont *font = 0;
        if (i == pf->textPFException()->indent()
                && pf->textPFException()->hasBulletFont()) {
            font = d->presentation->getFont(pf->textPFException()->bulletFontRef());
        } else if (levelPF->hasBulletFont()) {
            font = d->presentation->getFont(levelPF->bulletFontRef());
        }

        if (font) {
// disabled until the font->name() is checked to contain only utf8 strings
//      elementWriter.addAttribute( "fo:font-family", font->name() );
        }

        if (i == pf->textPFException()->indent() && pf->textPFException()->hasBulletColor()) {
            elementWriter.addAttribute("fo:color",
                                       colorIndexStructToQColor(pf->textPFException()->bulletColor()).name());
        } else {
            if (levelPF->hasBulletColor()) {
                elementWriter.addAttribute("fo:color",
                                           colorIndexStructToQColor(levelPF->bulletColor()).name());
            }
        }

        if (i == pf->textPFException()->indent() && pf->textPFException()->hasBulletSize()) {
            elementWriter.addAttribute("fo:font-size",
                                       QString("%1%").arg(pf->textPFException()->bulletSize()));

        } else {
            if (levelPF->hasSpaceAfter()) {
                elementWriter.addAttribute("fo:font-size",
                                           QString("%1%").arg(levelPF->bulletSize()));
            } else {
                elementWriter.addAttribute("fo:font-size", "100%");
            }
        }

        if (i == pf->textPFException()->indent() && pf->textPFException()->hasSpaceBefore()) {
            elementWriter.addAttribute("text:space-before",
                                       paraSpacingToCm(pf->textPFException()->spaceBefore()));
        } else {
            if (levelPF->hasSpaceBefore()) {
                elementWriter.addAttribute("text:space-before",
                                           paraSpacingToCm(levelPF->spaceBefore()));
            }
        }

        elementWriter.endElement(); // style:text-properties
        elementWriter.endElement();  // text:list-level-style-bullet
    }

    styleList.addChildElement("text:list-level-style-bullet",
                              QString::fromUtf8(buffer.buffer(),
                                                buffer.buffer().size()));

    textObject->addStylenames(cf->textCFException(),
                              pf->textPFException(),
                              styles.lookup(styleText),
                              styles.lookup(styleParagraph),
                              styles.lookup(styleList));
}


void PowerPointImport::processTextObjectForStyle(TextObject* textObject,
        KoGenStyles &styles)
{
    if (!textObject)  {
        return;
    }
    StyleTextPropAtom* atom  = textObject->styleTextProperty();
    QString text = textObject->text();

    if (!atom) {
        return;
    }

    //What paragraph/character exceptions were used last
    TextPFRun *pf = 0;
    TextCFRun *cf = 0;

    //TODO this can be easily optimized by calculating proper increments to i
    //from both exception's character count
    for (int i = 0;i < text.length();i++) {
        if (cf == atom->findTextCFRun(i) && pf == atom->findTextPFRun(i)) {
            continue;
        }

        pf = atom->findTextPFRun(i);
        cf = atom->findTextCFRun(i);

        if (cf && pf) {
            processTextExceptionsForStyle(cf, pf, styles, textObject);
        } else {
            kWarning() << "Failed to find pf or cf for text at position!" << i << pf << cf;
            return;
        }
    }

}

void PowerPointImport::processGroupObjectForStyle(GroupObject* groupObject,
        KoGenStyles &styles)
{
    if (!groupObject) return;

    for (unsigned int i = 0; i < groupObject->objectCount(); ++i) {
        processObjectForStyle(groupObject->object(i), styles);
    }
}

QString hexname(const Color &c)
{
    QColor qc(c.red, c.green, c.blue);
    return qc.name();
}

void PowerPointImport::processDrawingObjectForStyle(DrawObject* drawObject, KoGenStyles &styles)
{
    if (!drawObject) return;

    KoGenStyle style(KoGenStyle::StyleGraphicAuto, "graphic");
    style.setParentName("standard");

    if (drawObject->hasProperty("libppt:invisibleLine")) {
        if (drawObject->getBoolProperty("libppt:invisibleLine") == true)
            style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
    } else if (drawObject->hasProperty("draw:stroke")) {
        if (drawObject->getStrProperty("draw:stroke") == "dash") {
            style.addProperty("draw:stroke", "dash", KoGenStyle::GraphicType);
            std::string s = drawObject->getStrProperty("draw:stroke-dash");
            QString ss(s.c_str());
            style.addProperty("draw:stroke-dash", ss, KoGenStyle::GraphicType);
        } else if (drawObject->getStrProperty("draw:stroke") == "solid") {
            style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
        }
    }

    if (drawObject->hasProperty("svg:stroke-width")) {
        double strokeWidth = drawObject->getDoubleProperty("svg:stroke-width");
        style.addProperty("svg:stroke-width", QString("%1mm").arg(strokeWidth), KoGenStyle::GraphicType);
    }

    if (drawObject->hasProperty("svg:stroke-color")) {
        Color strokeColor = drawObject->getColorProperty("svg:stroke-color");
        style.addProperty("svg:stroke-color", hexname(strokeColor), KoGenStyle::GraphicType);
    }

    if (drawObject->hasProperty("draw:marker-start")) {
        std::string s = drawObject->getStrProperty("draw:marker-start");
        QString ss(s.c_str());
        style.addProperty("draw:marker-start", ss, KoGenStyle::GraphicType);
    }
    if (drawObject->hasProperty("draw:marker-end")) {
        std::string s = drawObject->getStrProperty("draw:marker-end");
        QString ss(s.c_str());
        style.addProperty("draw:marker-end", ss, KoGenStyle::GraphicType);
    }
    if (drawObject->hasProperty("draw:marker-start-width")) {
        double strokeWidth = drawObject->getDoubleProperty("svg:stroke-width");
        double arrowWidth = (drawObject->getDoubleProperty("draw:marker-start-width") * strokeWidth);
        style.addProperty("draw:marker-start-width", QString("%1cm").arg(arrowWidth), KoGenStyle::GraphicType);
    }

    if (drawObject->hasProperty("draw:marker-end-width")) {
        double strokeWidth = drawObject->getDoubleProperty("svg:stroke-width");
        double arrowWidth = (drawObject->getDoubleProperty("draw:marker-end-width") * strokeWidth);
        style.addProperty("draw:marker-end-width", QString("%1cm").arg(arrowWidth), KoGenStyle::GraphicType);
    }

    if (drawObject->hasProperty("draw:fill")) {
        std::string s = drawObject->getStrProperty("draw:fill");
        QString ss(s.c_str());
        style.addProperty("draw:fill", ss, KoGenStyle::GraphicType);
    }

    if (drawObject->hasProperty("draw:fill-color")) {
        Color fillColor = drawObject->getColorProperty("draw:fill-color");
        style.addProperty("draw:fill-color", hexname(fillColor), KoGenStyle::GraphicType);
    }  else {
        style.addProperty("draw:fill-color", "#99ccff", KoGenStyle::GraphicType);
    }

#if 0
    if (drawObject->hasProperty("draw:shadow-color")) {
        elementWriter.addAttribute("draw:shadow", "visible");
        Color shadowColor = drawObject->getColorProperty("draw:shadow-color");
        style.addProperty("draw:shadow-color", hexname(shadowColor), KoGenStyle::GraphicType);
    } else {
        style.addProperty("draw:shadow", "hidden", KoGenStyle::GraphicType);
    }
#endif

    if (drawObject->hasProperty("draw:shadow-opacity")) {
        double opacity = drawObject->getDoubleProperty("draw:shadow-opacity") ;
        style.addProperty("draw:shadow-opacity", QString("%1%").arg(opacity), KoGenStyle::GraphicType);
    }

    if (drawObject->hasProperty("draw:shadow-offset-x")) {
        double offset = drawObject->getDoubleProperty("draw:shadow-offset-x") ;
        style.addProperty("draw:shadow-offset-x", QString("%1cm").arg(offset), KoGenStyle::GraphicType);
    }

    if (drawObject->hasProperty("draw:shadow-offset-y")) {
        double offset = drawObject->getDoubleProperty("draw:shadow-offset-y");
        style.addProperty("draw:shadow-offset-y", QString("%1cm").arg(offset), KoGenStyle::GraphicType);
    }

    drawObject->setStyleName(styles.lookup(style));
}

