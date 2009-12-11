/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

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
   Boston, MA  02110-1301  USA.
*/

#include <cmath>
#include <cfloat>

//Qt includes
#include <QByteArray>
#include <QBuffer>
#include <QStringList>
#include <QString>
#include <QTime>
#include <QtAlgorithms>
#include <QMatrix>

//KDE includes
#include <kgenericfactory.h>
#include <kdebug.h>

//KOffice includes
#include <KoStore.h>
#include <KoOdfWriteStore.h>
#include <KoDocumentInfo.h>
#include <KoFilterChain.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdf.h>
#include <KoGenStyle.h>

#include "Filterkpr2odf.h"

using std::sin;
using std::cos;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef KGenericFactory<Filterkpr2odf> Filterkpr2odfFactory;
K_EXPORT_COMPONENT_FACTORY(libFilterkpr2odf, Filterkpr2odfFactory("kofficefilters"))

Filterkpr2odf::Filterkpr2odf(QObject *parent, const QStringList&)
        : KoFilter(parent)
        , m_currentPage(1)
        , m_objectIndex(1)
        , m_sticky(false)
{
}

KoFilter::ConversionStatus Filterkpr2odf::convert(const QByteArray& from, const QByteArray& to)
{
    //Check that the type of files are right
    if (from != "application/x-kpresenter"
            || to != "application/vnd.oasis.opendocument.presentation") {
        return KoFilter::BadMimeType;
    }

    //open the input file file
    KoStore* input = KoStore::createStore(m_chain->inputFile(), KoStore::Read);
    if (!input) {
        return KoFilter::FileNotFound;
    }

    //Load the document
    //Load maindoc.xml
    if (!input->open("maindoc.xml")) {
        return KoFilter::WrongFormat;
    }
    m_mainDoc.setContent(input->device(), false);
    input->close();

    //Load documentinfo.xml
    if (!input->open("documentinfo.xml")) {
        return KoFilter::WrongFormat;
    }

    m_documentInfo.setContent(input->device(), false);
    input->close();

    //Load the preview picture
    QByteArray* preview = new QByteArray();
    if (!input->extractFile("preview.png", *preview)) {
        return KoFilter::WrongFormat;
    }

    //If we find everything let the saving begin

    //Create the output file
    KoStore* output = KoStore::createStore(m_chain->outputFile(), KoStore::Write
                                           , KoOdf::mimeType(KoOdf::Presentation), KoStore::Zip);

    if (!output) {
        return KoFilter::StorageCreationError;
    }

    KoOdfWriteStore odfWriter(output);
    KoXmlWriter* manifest = odfWriter.manifestWriter(KoXmlNS::presentation);
    //Save the preview picture
    output->enterDirectory("Thumbnails");
    output->open("thubnail.png");
    output->write(*preview);
    output->close();
    output->leaveDirectory();
    manifest->addManifestEntry("Thubnails/", "");
    manifest->addManifestEntry("Thubnails/thubnail.png", "");
    delete preview;

    //Write the Pictures directory and its children, also fill the m_pictures hash
    createImageList(output, input, manifest);
    //write the sounds and fill the m_sounds hash
    createSoundList(output, input, manifest);
    delete input;

    //Create the content.xml file
    KoXmlWriter *content = odfWriter.contentWriter();
    KoXmlWriter *body = odfWriter.bodyWriter();
    convertContent(body);
    m_styles.saveOdfAutomaticStyles(content, false);
    odfWriter.closeContentWriter();
    manifest->addManifestEntry("content.xml", "text/xml");

    //Create the styles.xml file
    m_styles.saveOdfStylesDotXml(output, manifest);

    //Create settings.xml
    output->open("settings.xml");
    KoStoreDevice device(output);
    KoXmlWriter *settings = KoOdfWriteStore::createOasisXmlWriter(&device, "office:document-settings");
    //TODO: check which settings we still use in 2.0
    settings->endElement();//office:document-settings
    settings->endDocument();
    output->close();
    manifest->addManifestEntry("settings.xml", "text/xml");

    //Create the meta.xml file
    output->open("meta.xml");
    KoDocumentInfo* meta = new KoDocumentInfo();
    meta->load(m_documentInfo);
    meta->saveOasis(output);
    delete meta;
    output->close();
    manifest->addManifestEntry("meta.xml", "text/xml");

    //Write the document manifest
    odfWriter.closeManifestWriter();

    delete output;

    return KoFilter::OK;
}

//TODO: improve createImageList and createSoundList so that only save the _used_ sounds and images
void Filterkpr2odf::createImageList(KoStore* output, KoStore* input, KoXmlWriter* manifest)
{
    KoXmlElement key(m_mainDoc.namedItem("DOC").namedItem("PICTURES").firstChild().toElement());
    if (key.isNull()) {
        return;
    }

    output->enterDirectory("Pictures");

    //Iterate over all the keys to copy the image, get the file name and
    //its "representation" inside the KPR file
    for (; !key.isNull(); key = key.nextSibling().toElement()) {
        QString name = key.attribute("name");
        QString fullFilename = getPictureNameFromKey(key);

        //Get the name how will be saved in the file
        QStringList filenameComponents = name.split('/');
        QString odfName = filenameComponents.at(filenameComponents.size() - 1);

        m_pictures[ fullFilename ] = odfName;

        //Copy the picture
        QByteArray* image = new QByteArray();
        input->extractFile(name, *image);
        output->open(odfName);
        output->write(*image);
        output->close();
        delete image;

        //generate manifest entry
        QString mediaType;
        if (odfName.endsWith("png")) {
            mediaType = "image/png";
        } else if (odfName.endsWith("jpg")) {
            mediaType = "image/jpg";
        } else if (odfName.endsWith("jpeg")) {
            mediaType = "image/jpeg";
        }
        manifest->addManifestEntry(name, mediaType);
    }
    output->leaveDirectory();
}

void Filterkpr2odf::createSoundList(KoStore* output, KoStore* input, KoXmlWriter* manifest)
{
    KoXmlElement file(m_mainDoc.namedItem("DOC").namedItem("SOUNDS").firstChild().toElement());
    if (file.isNull())
        return;

    output->enterDirectory("Sounds");
    manifest->addManifestEntry("Sounds/", "");

    //Iterate over all files to copy the sound, get the file name and
    //its "representation" inside the KPR file
    for (; !file.isNull(); file = file.nextSibling().toElement()) {
        QString name(file.attribute("name"));
        QString filename(file.attribute("filename"));
        QStringList filenameComponents(name.split('/'));
        QString odfName(filenameComponents.at(filenameComponents.size() - 1));

        m_sounds[ filename ] = odfName;

        //Copy the sound
        QByteArray* sound = new QByteArray();
        input->extractFile(name, *sound);
        output->open(odfName);
        output->write(*sound);
        output->close();
        delete sound;

        //generate manifest entry
        QString mediaType;
        if (odfName.endsWith("wav")) {
            mediaType = "audio/wav";
        } else if (odfName.endsWith("mp3")) {
            mediaType = "audio/mp3";
        }
        manifest->addManifestEntry(name, mediaType);
    }
    output->leaveDirectory();
}

void Filterkpr2odf::convertContent(KoXmlWriter* content)
{
    content->startElement("office:body");
    content->startElement(KoOdf::bodyContentElement(KoOdf::Presentation, true));

    //We search all this here so that we can make the search just once
    KoXmlNode titles = m_mainDoc.namedItem("DOC").namedItem("PAGETITLES");
    KoXmlNode notes = m_mainDoc.namedItem("DOC").namedItem("PAGENOTES");
    KoXmlNode backgrounds = m_mainDoc.namedItem("DOC").namedItem("BACKGROUND");
    KoXmlNode objects = m_mainDoc.namedItem("DOC").namedItem("OBJECTS");
    KoXmlNode paper = m_mainDoc.namedItem("DOC").namedItem("PAPER");
    m_pageHeight = paper.toElement().attribute("ptHeight").toFloat();

    //Go to the first background, there might be missing backgrounds
    KoXmlElement pageBackground = backgrounds.firstChild().toElement();
    KoXmlElement masterBackground = backgrounds.namedItem("MASTERPAGE").toElement();
    //Parse pages
    //create the master page style
    const QString masterPageStyleName = createMasterPageStyle(objects, masterBackground);
    //The pages are all stored inside PAGETITLES
    //and all notes in PAGENOTES
    KoXmlNode title = titles.firstChild();
    KoXmlNode note = notes.firstChild();
    for (; !title.isNull() && !note.isNull();
            title = title.nextSibling(), note = note.nextSibling()) {
        //Every page is a draw:page
        content->startElement("draw:page");
        content->addAttribute("draw:name", title.toElement().attribute("title"));
        content->addAttribute("draw:style-name", createPageStyle(pageBackground));
        pageBackground = pageBackground.nextSibling().toElement();//next background
        content->addAttribute("draw:id", QString("page%1").arg(m_currentPage));
        content->addAttribute("draw:master-page-name", masterPageStyleName);

        //convert the objects (text, images, charts...) in this page
        convertObjects(content, objects);

        //Append the notes
        content->startElement("presentation:notes");
        content->startElement("draw:page-thumbnail");
        content->endElement();//draw:page-thumbnail
        content->startElement("draw:frame");  //FIXME: add drawing attributes
        content->startElement("draw:text-box");
        QStringList noteTextList = note.toElement().attribute("note").split('\n');

        foreach(const QString & string, noteTextList) {
            content->startElement("text:p");
            content->addTextNode(string);
            content->endElement();
        }

        content->endElement();//draw:text-box
        content->endElement();//draw:frame
        content->endElement();//presentation:notes

        saveAnimations(content);

        content->endElement();//draw:page

        ++m_currentPage;
    }//page's for

    content->startElement("presentation:settings");

    //Load whether the presentation ends or it's in an infinite loop
    KoXmlElement infinitLoop(m_mainDoc.namedItem("DOC").namedItem("INFINITLOOP").toElement());
    if (!infinitLoop.isNull()) {
        bool value = infinitLoop.attribute("value", "0") == "1";
        content->addAttribute("presentation:endless", (value) ? "true" : "false");
    }

    //Specify whether the effects can be started automatically or
    //ignore any previous order and start them manually
    KoXmlElement manualSwitch = m_mainDoc.namedItem("DOC").namedItem("MANUALSWITCH").toElement();
    if (!manualSwitch.isNull()) {
        bool value = manualSwitch.attribute("value", "0") == "1";
        content->addAttribute("presentation:force-manual", (value) ? "true" : "false");
    }

    //Store the default show
    KoXmlElement customSlideShowDefault = m_mainDoc.namedItem("DOC").namedItem("DEFAULTCUSTOMSLIDESHOWNAME").toElement();
    if (!customSlideShowDefault.isNull()) {
        content->addAttribute("presentation:show", customSlideShowDefault.attribute("name"));
    }

    //Now store all the shows
    KoXmlElement customSlideShowConfig = m_mainDoc.namedItem("DOC").namedItem("CUSTOMSLIDESHOWCONFIG").toElement();
    for (KoXmlElement customSlideShow = customSlideShowConfig.firstChild().toElement(); !customSlideShow.isNull();
            customSlideShow = customSlideShow.nextSibling().toElement()) {
        content->startElement("presentation:show");
        content->addAttribute("presentation:name", customSlideShow.attribute("name"));
        content->addAttribute("presentation:pages", customSlideShow.attribute("pages"));
        content->endElement();//presentation:show
    }

    content->endElement();//presentation:settings
    content->endElement();//office:presentation
    content->endElement();//office:body
    content->endDocument();
}

void Filterkpr2odf::convertObjects(KoXmlWriter* content, const KoXmlNode& objects)
{
    //We search through all the objects' nodes because
    //we are not sure if they are saved in order
    for (KoXmlNode object = objects.firstChild(); !object.isNull(); object = object.nextSibling()) {
        float y = object.namedItem("ORIG").toElement().attribute("y").toFloat();

        //We check if the y is on the current page
        if (y < m_pageHeight * (m_currentPage - 1)
                || y >= m_pageHeight * m_currentPage)
            continue; // object not on current page

        //Now define what kind of object is
        KoXmlElement objectElement = object.toElement();

        bool sticky = objectElement.attribute("sticky", "0").toInt() == 1;
        if (sticky != m_sticky) {
            continue;
        }

        //Enum: ObjType
        switch (objectElement.attribute("type").toInt()) {
        case 0: // picture
            appendPicture(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 1: // line
            appendLine(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 2: // rectangle
            appendRectangle(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 3: // ellipse or circle
            appendEllipse(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 4: // text
            appendTextBox(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 5: //autoform
            appendAutoform(content, objectElement);
            //NOTE: we cannot add the animation since we're not totally sure it's a valid Autoform
            break;
        case 6: //clipart
            break;
            //NOTE: 7 is undefined, never happens in a file (according to kpresenter.dtd)
        case 8: // pie, chord, arc
            appendPie(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 9: //part
            break;
        case 10: //group
            appendGroupObject(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 11: //freehand
            appendFreehand(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 12: // polyline
            //a bunch of points that are connected and not closed
            appendPoly(content, objectElement, false /*polyline*/);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 13: //quadric bezier curve
        case 14: //cubic bezier curve
            appendBezier(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 15: //polygon
            //a regular polygon, easily drawn by the number of sides it has
            appendPolygon(content, objectElement);
            exportAnimation(objectElement, content->indentLevel());
            break;
        case 16: //closed polyline
            //that is a closed non-regular polygon
            appendPoly(content, objectElement, true /*closedPolygon*/);
            exportAnimation(objectElement, content->indentLevel());
            break;
        default:
            kWarning() << "Unexpected object found in page " << m_currentPage;
            break;
        }//switch objectElement

        ++m_objectIndex;
    }//for
}

void Filterkpr2odf::appendPicture(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:frame");
    set2DGeometry(content, objectElement);  //sizes mostly
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    content->startElement("draw:image");
    content->addAttribute("xlink:type", "simple");
    content->addAttribute("xlink:show", "embed");
    content->addAttribute("xlink:actuate", "onLoad");
    content->addAttribute("xlink:href", "Pictures/" + m_pictures[ getPictureNameFromKey(objectElement.namedItem("KEY").toElement())]);

    content->endElement();//draw:image
    content->endElement();//draw:frame
    //NOTE: the effects seem to not be portable
}

void Filterkpr2odf::appendLine(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:line");
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    KoXmlElement orig = objectElement.namedItem("ORIG").toElement();
    KoXmlElement size = objectElement.namedItem("SIZE").toElement();

    QRectF r(orig.attribute("x").toDouble(),
             orig.attribute("y").toDouble() - m_pageHeight * (m_currentPage - 1),
             size.attribute("width").toDouble(),
             size.attribute("height").toDouble());

    double x1 = 0.0;
    double y1 = 0.0;
    double x2 = 0.0;
    double y2 = 0.0;
    QPointF center(r.width() / 2, r.height() / 2);

    KoXmlElement lineType = objectElement.namedItem("LINETYPE").toElement();
    int type = 0;
    if (!lineType.isNull()) {
        type = lineType.attribute("value").toInt();
    }

    switch (type) {
    case 0:
        x1 = -center.x();
        x2 = -x1;
        break;
    case 1:
        y1 = -center.y();
        y2 = -y1;
        break;
    case 2:
        x1 = -center.x();
        y1 = -center.y();
        x2 = -x1;
        y2 = -y1;
        break;
    case 3:
        x1 = -center.x();
        y1 = center.y();
        x2 = -x1;
        y2 = -y1;
        break;
    }

    KoXmlElement angle = objectElement.namedItem("ANGLE").toElement();
    if (!angle.isNull()) {
        double angInRad = -angle.attribute("value").toDouble() * M_PI / 180.0;
        QMatrix m(cos(angInRad), -sin(angInRad), sin(angInRad), cos(angInRad), 0, 0);
        qreal transX1 = 0.0;
        qreal transY1 = 0.0;
        qreal transX2 = 0.0;
        qreal transY2 = 0.0;
        m.map(x1, y1, &transX1, &transY1);
        m.map(x2, y2, &transX2, &transY2);
        x1 = transX1;
        y1 = transY1;
        x2 = transX2;
        y2 = transY2;
    }

    x1 += r.x() + center.x();
    y1 += r.y() + center.y();
    x2 += r.x() + center.x();
    y2 += r.y() + center.y();

    //save all into pt
    content->addAttributePt("svg:x1", x1);
    content->addAttributePt("svg:y1", y1);
    content->addAttributePt("svg:x2", x2);
    content->addAttributePt("svg:y2", y2);

    KoXmlElement name = objectElement.namedItem("OBJECTNAME").toElement();
    QString nameString = name.attribute("objectName");
    if (!nameString.isNull()) {
        content->addAttribute("draw:name", nameString);
    }
    content->endElement();//draw:line
}

void Filterkpr2odf::appendRectangle(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:rect");

    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    set2DGeometry(content, objectElement);

    KoXmlElement size = objectElement.namedItem("SIZE").toElement();
    double width = size.attribute("width").toDouble();
    double height = size.attribute("height").toDouble();

    //<RNDS x="75" y="75"/>
    const KoXmlElement rnds = objectElement.namedItem("RNDS").toElement();
    if (!rnds.isNull()) {
        if (rnds.hasAttribute("x") && rnds.hasAttribute("y")) {
            int x = rnds.attribute("x").toInt();
            int y = rnds.attribute("y").toInt();
            content->addAttribute("svg:rx", x / 200.0 * width);
            content->addAttribute("svg:ry", y / 200.0 * height);
        }
    }

    content->endElement();//draw:rect
}

void Filterkpr2odf::appendEllipse(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    KoXmlElement size = objectElement.namedItem("SIZE").toElement();
    double width = size.attribute("width").toDouble();
    double height = size.attribute("height").toDouble();

    content->startElement((width == height) ? "draw:circle" : "draw:ellipse");
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));
    set2DGeometry(content, objectElement);

    content->endElement();//draw:circle or draw:ellipse
}

void Filterkpr2odf::appendTextBox(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:frame");
    set2DGeometry(content, objectElement);
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    content->startElement("draw:text-box");
    KoXmlElement textObject = objectElement.namedItem("TEXTOBJ").toElement();

    //export every paragraph
    for (KoXmlElement paragraph = textObject.firstChild().toElement(); !paragraph.isNull(); paragraph = paragraph.nextSibling().toElement()) {
        appendParagraph(content, paragraph);
    }

    content->endElement();//draw:text-box
    content->endElement();//draw:frame
}

void Filterkpr2odf::appendParagraph(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    KoXmlElement counter = objectElement.namedItem("COUNTER").toElement();
    if (!counter.isNull()) { //it's part of a list
        content->startElement("text:numbered-paragraph");
        content->addAttribute("text:style-name", createListStyle(objectElement));
        content->addAttribute("text:level", counter.attribute("depth", "0").toInt() + 1);
    }

    content->startElement("text:p", false);  //false: we should not indent the inner tags
    content->addAttribute("text:style-name", createParagraphStyle(objectElement));
    //convert every text element
    for (KoXmlElement text = objectElement.firstChild().toElement(); !text.isNull();  text = text.nextSibling().toElement()) {
        if (text.nodeName() == "TEXT") { //only TEXT children are relevant
            appendText(content, text);
        }
    }
    content->endElement();//text:p

    if (!counter.isNull()) { //it's part of a list
        content->endElement();//text:numbered-paragraph
    }
}

void Filterkpr2odf::appendText(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    //Avoid the creation of so many unneded text:span
    static QString lastStyle;
    static QString textChain;
    bool lastSpan = objectElement.nextSibling().isNull();

    QString styleName = createTextStyle(objectElement);

    textChain += objectElement.text();

    bool whitespace = objectElement.attribute("whitespace", "0") == "1";
    if (whitespace) {
        textChain += ' ';
    }

    if (lastSpan || ((!lastStyle.isEmpty()) && (lastStyle != styleName))) {
        content->startElement("text:span");

        content->addAttribute("text:style-name", styleName);
        content->addTextNode(textChain);

        content->endElement();//text:span

        textChain.clear();//reset textChain
    }

    //We have to reset the last style if we are going to change the paragraph
    if (!lastSpan) {
        lastStyle = styleName;
    } else {
        lastStyle.clear();
    }
}

void Filterkpr2odf::appendPie(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    //NOTE: we cannot use set2dGeometry becuse we have to convert the
    //given size and origen into the real ones before saving them

    KoXmlElement size = objectElement.namedItem("SIZE").toElement();
    double width = size.attribute("width").toDouble();
    double height = size.attribute("height").toDouble();

    KoXmlElement pieAngle = objectElement.namedItem("PIEANGLE").toElement();
    int startAngle = 45; //default value take it into kppieobject
    if (!pieAngle.isNull()) {
        startAngle = (pieAngle.attribute("value").toInt()) / 16;
    }

    KoXmlElement pieLength = objectElement.namedItem("PIELENGTH").toElement();
    int endAngle = 90 + startAngle; //default
    if (!pieLength.isNull()) {
        endAngle = pieLength.attribute("value").toInt() / 16 + startAngle;
    }

    //rotation
    KoXmlElement angle = objectElement.namedItem("ANGLE").toElement();
    int rotationAngleValue = angle.attribute("value").toDouble() / 16.0;

    //Type of the enclosure of the circle/ellipse
    KoXmlElement pie = objectElement.namedItem("PIETYPE").toElement();
    QString kind;
    //Enum: PieType
    int pieType = pie.attribute("value", "0").toInt();  //We didn't find the type, we set "section" by default
    switch (pieType) {
    case 0:
        kind = "section";
        break;
    case 1:
        kind = "arc";
        break;
    case 2:
        kind = "cut";
        break;
    }

    KoXmlElement orig = objectElement.namedItem("ORIG").toElement();
    double x = orig.attribute("x").toDouble();
    double y = orig.attribute("y").toDouble();
    y -= m_pageHeight * (m_currentPage - 1);
    QPointF realOrig(x, y);
    QSizeF realSize(width, height);

    getRealSizeAndOrig(realSize, realOrig, startAngle, endAngle, rotationAngleValue, pieType);

    content->startElement((width == height) ? "draw:circle" : "draw:ellipse");
    KoXmlElement name = objectElement.namedItem("OBJECTNAME").toElement();
    QString nameStr = name.attribute("objectName");
    if (!nameStr.isEmpty()) {
        content->addAttribute("draw:name", nameStr);
    }
    content->addAttribute("draw:id", QString("object%1").arg(m_objectIndex));
    content->addAttributePt("svg:x", realOrig.x());
    content->addAttributePt("svg:y",  realOrig.y());
    content->addAttributePt("svg:width", realSize.width());
    content->addAttributePt("svg:height", realSize.height());
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));
    content->addAttribute("draw:kind", kind);
    content->addAttribute("draw:start-angle", startAngle);
    content->addAttribute("draw:end-angle", endAngle);

    content->endElement();//draw:circle or draw:ellipse
}

void Filterkpr2odf::setEndPoints(QPointF points[], const QSizeF& size, int startAngle, int endAngle)
{
    //NOTE: this code is ported from the 1.6 series from the KPrPieObject.cpp file

    int angles[] = { startAngle, endAngle };
    double anglesInRad[] = { angles[0] * M_PI / 180, angles[1] * M_PI / 180 };

    double radius1 = 0.5 * size.width();
    double radius2 = 0.5 * size.height();

    double prop = radius2 / radius1;

    for (int i = 0; i < 2; i++) {
        double x = 0;
        double y = 0;

        // be carefull
        if (angles[i] == 90) {
            y = radius2;
        } else if (angles[i] == 270) {
            y = -radius2;
        } else {
            // The real angle is not what was given. It is only ok if radius1 == radius2,
            // otherwise it is arctan ( radius2 / radius1 tan ( angle ) )
            double tanalpha = tan(anglesInRad[i]) * prop;
            x = sqrt(1 / (pow(1 / radius1, 2) + pow(tanalpha / radius2, 2)));
            if (angles[i] > 90 && angles[i] < 270)
                x = -x;
            y = tanalpha * x;
        }
        points[i].setX(x);
        points[i].setY(y);
    }
}

void Filterkpr2odf::getRealSizeAndOrig(QSizeF &size, QPointF &realOrig, int startAngle, int endAngle, int angle, int pieType)
{
    //NOTE: this code is ported from the 1.6 series from the KPrPieObject.cpp file

    int len;
    if (endAngle < startAngle) {
        len = (360 - startAngle + endAngle);
    } else {
        len = (endAngle - startAngle);
    }

    double radius1 = 0.5 * size.width();
    double radius2 = 0.5 * size.height();

    // the rotation angle
    double angInRad = angle * M_PI / 180;

    // 1. calulate position of end points
    QPointF points[2];
    setEndPoints(points, size, startAngle, endAngle);

    // rotate point
    for (int i = 0; i < 2; ++i) {
        if (angle != 0) {
            double sinus = sin(angInRad);
            double cosinus = cos(angInRad);

            double tmp_x = points[i].x();
            double tmp_y = points[i].y();

            double x = tmp_x * cosinus + tmp_y * sinus;
            double y = - tmp_x * sinus + tmp_y * cosinus;
            points[i].setX(x);
            points[i].setY(y);
        }
    }

    QPointF firstPoint(points[0]);
    QPointF secondPoint(points[1]);

    // 2. calulate maximal points
    QPointF maxPoints[4];
    if (angle == 0) {
        maxPoints[0].setX(0);
        maxPoints[0].setY(radius2);

        maxPoints[1].setX(radius1);
        maxPoints[1].setY(0);

        maxPoints[2].setX(0);
        maxPoints[2].setY(-radius2);

        maxPoints[3].setX(-radius1);
        maxPoints[3].setY(0);
    } else {
        double sinus = sin(angInRad);
        double cosinus = cos(angInRad);

        double x = sqrt(pow(radius1 * cosinus , 2) + pow(radius2 * sinus, 2));
        double y = (pow(radius2, 2) - pow(radius1, 2)) * sinus * cosinus / x;
        maxPoints[0].setX(x);
        maxPoints[0].setY(y);
        maxPoints[1].setX(-x);
        maxPoints[1].setY(-y);

        y = sqrt(pow(radius1 * sinus , 2) + pow(radius2 * cosinus, 2));
        x = (pow(radius1, 2) - pow(radius2, 2)) * sinus * cosinus / y;
        maxPoints[2].setX(x);
        maxPoints[2].setY(y);
        maxPoints[3].setX(-x);
        maxPoints[3].setY(-y);
    }

    // 3. find minimal and maximal points
    double min_x = firstPoint.x();
    double min_y = firstPoint.y();
    double max_x = firstPoint.x();
    double max_y = firstPoint.y();

    if (pieType == 0) { //PT_PIE
        QPointF zero(0, 0);
        setMinMax(min_x, min_y, max_x, max_y, zero);
    }
    setMinMax(min_x, min_y, max_x, max_y, secondPoint);

    /* 4. check if maximal points lie on the arc.
     * There are three possibilities how many sections have to
     * been checked.
     * 1. the arc is only once on one side of the x axis
     * 2. the arc is on both sides of the x axis
     * 3. the arc is twice on one one side of the x axis
     *
     * 1)                 2)              3)
     *      y                  y               y
     *    ex|xx              xx|xs           s |
     *      |  x            x  |            x  |  e
     *      |   s          x   |           x   |   x
     *  ----+----  x       ----+----  x    ----+----  x
     *      |              x   |           x   |   x
     *      |               x  |            x  |  x
     *      |                e |             xx|xx
     *
     */
    if (firstPoint.y() >= 0) {
        if (secondPoint.y() >= 0) {
            if (firstPoint.x() > secondPoint.x() || len == 0) {
                // 1 section
                // f.x() <= x <= s.x() && y >= 0
                for (int i = 0; i < 4; ++i) {
                    if (maxPoints[i].y() >= 0
                            && maxPoints[i].x() <= firstPoint.x()
                            && maxPoints[i].x() >= secondPoint.x()) {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                }//for
            } else {
                // 3 sections
                // x <= f.x() && y >= 0
                // y < 0
                // x >= s.x() && y >= 0
                for (int i = 0; i < 4 ; ++i) {
                    if (maxPoints[i].y() >= 0) {
                        if (maxPoints[i].x() <= firstPoint.x()
                                || maxPoints[i].x() >= secondPoint.x()) {
                            setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                        }
                    } else {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                }//for
            }//else
        } else {
            // 2 sections
            // x <= f.x() && y >= 0
            // x <= s.x() && y < 0
            for (int i = 0; i < 4 ; ++i) {
                if (maxPoints[i].y() >= 0) {
                    if (maxPoints[i].x() <= firstPoint.x()) {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                } else {
                    if (maxPoints[i].x() <= secondPoint.x()) {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                }
            }//for
        }//else
    } else {
        if (secondPoint.y() >= 0) {
            // 2 sections
            // x >= f.x() && y < 0
            // x >= s.x() && y >= 0
            for (int i = 0 ; i < 4 ; ++i) {
                if (maxPoints[i].y() < 0) {
                    if (maxPoints[i].x() >= firstPoint.x()) {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                } else {
                    if (maxPoints[i].x() >= secondPoint.x()) {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                }
            }//for
        } else {
            if (firstPoint.x() < secondPoint.x() || len == 0) {
                // 1 section
                // f.x() <= x <= s.x() && y < 0
                for (int i = 0; i < 4; ++i) {
                    if (maxPoints[i].y() < 0
                            && maxPoints[i].x() >= firstPoint.x()
                            && maxPoints[i].x() <= secondPoint.x()) {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                }
            } else {
                // 3 sections
                // x >= f.x() && y < 0
                // y >= 0
                // x <= s.x() && y < 0
                for (int i = 0; i < 4; ++i) {
                    if (maxPoints[i].y() < 0) {
                        if (maxPoints[i].x() >= firstPoint.x() || maxPoints[i].x() <= secondPoint.x()) {
                            setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                        }
                    } else {
                        setMinMax(min_x, min_y, max_x, max_y, maxPoints[i]);
                    }
                }//for
            }//else
        }//else
    }

    double mid_x = 0.5 * size.width();
    double mid_y = 0.5 * size.height();

    size.setWidth(max_x - min_x);
    size.setHeight(max_y - min_y);

    realOrig.setX(realOrig.x() + mid_x + min_x);
    realOrig.setY(realOrig.y() + mid_y - max_y);
}

void Filterkpr2odf::setMinMax(double &min_x, double &min_y,
                              double &max_x, double &max_y, QPointF point)
{
    //NOTE: this code is ported from the 1.6 series from the KPrPieObject.cpp file

    double tmp_x = point.x();
    double tmp_y = point.y();

    if (tmp_x < min_x) {
        min_x = tmp_x;
    } else if (tmp_x > max_x) {
        max_x = tmp_x;
    }

    if (tmp_y < min_y) {
        min_y = tmp_y;
    } else if (tmp_y > max_y) {
        max_y = tmp_y;
    }
}

void Filterkpr2odf::appendGroupObject(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:g");

    set2DGeometry(content, objectElement);
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    KoXmlElement objects = objectElement.namedItem("OBJECTS").toElement();
    convertObjects(content, objects);

    content->endElement();//draw:g
}

void Filterkpr2odf::appendPoly(KoXmlWriter* content, const KoXmlElement& objectElement, bool polygon)
{
    //The function was written so to add polygon and polyline because it's basically the same,
    //only the name is changed and I didn't want to copy&paste
    content->startElement((polygon) ? "draw:polygon" : "draw:polyline");

    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    set2DGeometry(content, objectElement);
    KoXmlElement points = objectElement.namedItem("POINTS").toElement();
    if (!points.isNull()) {
        KoXmlElement point = points.firstChild().toElement();
        QString listOfPoints;

        //No white spaces allowed before the first element
        int tmpX = (int)(point.attribute("point_x", "0").toDouble() * 10000);
        int tmpY = (int)(point.attribute("point_y", "0").toDouble() * 10000);
        listOfPoints = QString("%1,%2").arg(tmpX).arg(tmpY);

        int maxX = tmpX;
        int maxY = tmpY;
        int previousX = tmpX;
        int previousY = tmpY;

        point = point.nextSibling().toElement();

        while (!point.isNull()) {
            tmpX = (int)(point.attribute("point_x", "0").toDouble() * 10000);
            tmpY = (int)(point.attribute("point_y", "0").toDouble() * 10000);
            //For some reason the last point is saved twice for some polygons, so we need to ignore the last one of them if they are equal
            //this fix assumes that the last child of the POINTS tag is a Point, seems to work but it's not garanteed
            if (tmpX == previousX && tmpY == previousY && point.nextSibling().isNull()) {
                break;
            }

            listOfPoints += QString(" %1,%2").arg(tmpX).arg(tmpY);

            maxX = qMax(maxX, tmpX);
            maxY = qMax(maxY, tmpY);
            previousX = tmpX;
            previousY = tmpY;

            point = point.nextSibling().toElement();
        }//while !element.isNull()
        content->addAttribute("draw:points", listOfPoints);
        content->addAttribute("svg:viewBox", QString("0 0 %1 %2").arg(maxX).arg(maxY));
    }//if !points.isNull()

    content->endElement();//draw:polygon or draw:polyline
}

void Filterkpr2odf::appendPolygon(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:regular-polygon");

    set2DGeometry(content, objectElement);
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    KoXmlElement settings = objectElement.namedItem("SETTINGS").toElement();
    int corners = settings.attribute("cornersValue").toInt();
    content->addAttribute("draw:corners", corners);
    bool concavePolygon = settings.attribute("checkConcavePolygon", "0") == "1";
    if (concavePolygon) {
        content->addAttribute("draw:concave", "true");
        content->addAttribute("draw:sharpness", QString("%1%").arg(settings.attribute("sharpnessValue")));
    } else {
        content->addAttribute("draw:concave", "false");
    }

    content->endElement();//draw:regular-polygon
}

void Filterkpr2odf::appendAutoform(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    QString fileName = objectElement.namedItem("FILENAME").toElement().attribute("value");
    if (fileName.contains("Arrow")) {
        appendArrow(content, objectElement);
        exportAnimation(objectElement, content->indentLevel());
        return;
    }

    QString d;

    KoXmlElement size = objectElement.namedItem("SIZE").toElement();
    double width = size.attribute("width").toDouble();
    double height = size.attribute("height").toDouble();

    if (fileName.endsWith("Connection1.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
    } else if (fileName.endsWith("Connection2.atf")) {
        d += QString("M%1 %2").arg((int)(width * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 100));
    } else if (fileName.endsWith("Connection3.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
    } else if (fileName.endsWith("Connection4.atf")) {
        d += QString("M%1 %2").arg((int)(width * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 100));
    } else if (fileName.endsWith("Connection5.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(0));
        d += QString("M%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 100));
    } else if (fileName.endsWith("Connection6.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
        d += QString("M%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(0));
    } else if (fileName.endsWith("Connection7.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 100));
        d += QString("M%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 0.50 * 100));
    } else if (fileName.endsWith("Connection8.atf")) {
        d += QString("M%1 %2").arg((int)(width * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
        d += QString("M%1 %2").arg((int)(width * 0.50 * 100)).arg((int)(height * 0.50 * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 0.50 * 100));
    } else if (fileName.endsWith("Connection9.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(0));
    } else if (fileName.endsWith("Connection10.atf")) {
        d += QString("M%1 %2").arg((int)(width * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
    } else if (fileName.endsWith("Connection11.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
    } else if (fileName.endsWith("Connection12.atf")) {
        d += QString("M%1 %2").arg((int)(0)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(0));
        d += QString("L%1 %2").arg((int)(width * 100)).arg((int)(height * 100));
        d += QString("L%1 %2").arg((int)(0)).arg((int)(height * 100));
    } else {
        //Not implemented
        return;
    }

    content->startElement("draw:path");
    set2DGeometry(content, objectElement);
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));
    content->addAttribute("svg:viewBox", QString("0 0 %1 %2").arg((int)(width*100)).arg((int)(height*100)));
    content->addAttribute("svg:d", d);

    exportAnimation(objectElement, content->indentLevel());
    content->endElement();//draw:path
}

void Filterkpr2odf::appendArrow(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    //NOTE: we cannot use set2dGeometry neither here

    KoXmlElement name = objectElement.namedItem("OBJECTNAME").toElement();
    content->startElement("draw:custom-shape");
    QString nameStr = name.attribute("objectName");
    if (!nameStr.isEmpty()) {
        content->addAttribute("draw:name", nameStr);
    }
    content->addAttribute("draw:id", QString("object%1").arg(m_objectIndex));
    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));
    content->addAttribute("svg:x", "0pt");
    content->addAttribute("svg:y", "0pt");

    KoXmlElement size = objectElement.namedItem("SIZE").toElement();
    double width = size.attribute("width").toDouble();
    double height = size.attribute("height").toDouble();
    content->addAttributePt("svg:width", width);
    content->addAttributePt("svg:height", height);

    double rotateAngle = 0.0;
    QString fileName = objectElement.namedItem("FILENAME").toElement().attribute("value");
    if (fileName.endsWith("ArrowUp.atf")) {
        rotateAngle = 90;
    } else if (fileName.endsWith("ArrowRightUp.atf")) {
        rotateAngle = 45;
    } else if (fileName.endsWith("ArrowRight.atf")) {
        rotateAngle = 0;
    } else if (fileName.endsWith("ArrowRightDown.atf")) {
        rotateAngle = 315;
    } else if (fileName.endsWith("ArrowDown.atf")) {
        rotateAngle = 270;
    } else if (fileName.endsWith("ArrowLeftDown.atf")) {
        rotateAngle = 225;
    } else if (fileName.endsWith("ArrowLeft.atf")) {
        rotateAngle = 180;
    } else if (fileName.endsWith("ArrowLeftUp.atf")) {
        rotateAngle = 135;
    }

    KoXmlElement orig = objectElement.namedItem("ORIG").toElement();
    double x = orig.attribute("x").toDouble();
    double y = orig.attribute("y").toDouble();
    y -= m_pageHeight * (m_currentPage - 1);

    QMatrix matrix;
    matrix.translate(x + 0.5 * width, y + 0.5 * height);
    matrix.rotate(rotateAngle);
    matrix.translate(-0.5 * width, -0.5 * height);

    QString matrixString = QString("matrix(%1 %2 %3 %4 %5pt %6pt)")
                           .arg(matrix.m11()).arg(matrix.m12())
                           .arg(matrix.m21()).arg(matrix.m22())
                           .arg(matrix.dx()) .arg(matrix.dy());

    content->addAttribute("draw:transform", matrixString);

    content->startElement("draw:enhanced-geometry");
    content->addAttribute("svg:viewBox", "0 0 100 100");
    content->addAttribute("draw:modifiers", "60 35");
    content->addAttribute("draw:enhanced-path", "M $0 $1 L $0 0 width ?HalfHeight $0 height $0 ?LowerCorner 0 ?LowerCorner 0 $1 Z");

    content->startElement("draw:equation");
    content->addAttribute("draw:name", "HalfHeight");
    content->addAttribute("draw:formula", "0.5 * height");
    content->endElement();//draw:equation

    content->startElement("draw:equation");
    content->addAttribute("draw:name", "LowerCorner");
    content->addAttribute("draw:formula", "height - $1");
    content->endElement();//draw:equation

    content->startElement("draw:handle");
    content->addAttribute("draw:handle-position", "$0 $1");
    content->addAttribute("draw:handle-range-x-minimum", "0");
    content->addAttribute("draw:handle-range-x-maximum", "width");
    content->addAttribute("draw:handle-range-y-minimum", "0");
    content->addAttribute("draw:handle-range-y-maximum", "?HalfHeight");
    content->endElement();//draw:handle

    content->endElement();//draw:enhanced-geometry
    content->endElement();//draw:custom-shape
}

void Filterkpr2odf::appendFreehand(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:path");

    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));
    set2DGeometry(content, objectElement);

    KoXmlElement points = objectElement.namedItem("POINTS").toElement();
    if (!points.isNull()) {
        KoXmlElement point = points.firstChild().toElement();
        QString d;

        int tmpX = (int)(point.attribute("point_x", "0").toDouble() * 10000);
        int tmpY = (int)(point.attribute("point_y", "0").toDouble() * 10000);
        int maxX = tmpX;
        int maxY = tmpY;

        point = point.nextSibling().toElement();

        d += QString("M%1 %2").arg(tmpX).arg(tmpY);
        while (!point.isNull()) {
            tmpX = (int)(point.attribute("point_x", "0").toDouble() * 10000);
            tmpY = (int)(point.attribute("point_y", "0").toDouble() * 10000);

            d += QString("L%1 %2").arg(tmpX).arg(tmpY);

            maxX = qMax(maxX, tmpX);
            maxY = qMax(maxY, tmpY);
            point = point.nextSibling().toElement();
        }
        content->addAttribute("svg:d", d);
        content->addAttribute("svg:viewBox", QString("0 0 %1 %2").arg(maxX).arg(maxY));
    }//if !points.isNull()

    content->endElement();//draw:path
}

void Filterkpr2odf::appendBezier(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    content->startElement("draw:path");

    content->addAttribute("draw:style-name", createGraphicStyle(objectElement));
    set2DGeometry(content, objectElement);

    KoXmlElement points = objectElement.namedItem("POINTS").toElement();
    if (!points.isNull()) {
        QString d;

        KoXmlElement point1 = points.firstChild().toElement();
        KoXmlElement point2 = point1.nextSibling().toElement();
        KoXmlElement point3 = point2.nextSibling().toElement();
        KoXmlElement point4 = point3.nextSibling().toElement();

        int maxX = 0;
        int maxY = 0;

        d += QString("M%1 %2").arg((int) point1.attribute("point_x").toDouble() * 10000)
             .arg((int) point1.attribute("point_y").toDouble() * 10000);
        while (!point3.isNull()) { //if point3 is null then point4 is null too, not need to check it
            int point1X = (int)(point1.attribute("point_x").toDouble() * 10000);
            int point1Y = (int)(point1.attribute("point_y").toDouble() * 10000);
            int point2X = (int)(point2.attribute("point_x").toDouble() * 10000);
            int point2Y = (int)(point2.attribute("point_y").toDouble() * 10000);
            int point3X = (int)(point3.attribute("point_x").toDouble() * 10000);
            int point3Y = (int)(point3.attribute("point_y").toDouble() * 10000);
            int point4X = (int)(point4.attribute("point_x").toDouble() * 10000);
            int point4Y = (int)(point4.attribute("point_y").toDouble() * 10000);

            d += QString("C%1 %2 %3 %4 %5 %6").arg(point3X).arg(point3Y)
                 .arg(point4X).arg(point4Y)
                 .arg(point2X).arg(point2Y);

            maxX = qMax(point1X, qMax(point2X, qMax(point3X, qMax(point4X, maxX))));
            maxY = qMax(point1Y, qMax(point2Y, qMax(point3Y, qMax(point4Y, maxY))));

            point1 = point4.nextSibling().toElement();
            point2 = point1.nextSibling().toElement();
            point3 = point2.nextSibling().toElement();
            point4 = point3.nextSibling().toElement();
        }

        if (!point2.isNull()) {
            int point2X = (int)(point2.attribute("point_x").toDouble() * 10000);
            int point2Y = (int)(point2.attribute("point_y").toDouble() * 10000);
            maxX = qMax(maxX, point2X);
            maxY = qMax(maxY, point2Y);

            d += QString("L%1 %2").arg(point2X).arg(point2Y);
        }

        content->addAttribute("svg:d", d);
        content->addAttribute("svg:viewBox", QString("0 0 %1 %2").arg(maxX).arg(maxY));
    }//if !points.isNull

    content->endElement();//draw:path
}

const QString Filterkpr2odf::getPictureNameFromKey(const KoXmlElement& key)
{
    return key.attribute("msec") + key.attribute("second") + key.attribute("minute")
           + key.attribute("hour") + key.attribute("day") + key.attribute("month")
           + key.attribute("year") + key.attribute("filename");
}

void Filterkpr2odf::set2DGeometry(KoXmlWriter* content, const KoXmlElement& objectElement)
{
    //This function sets the needed geometry-related attributes
    //for any object that is passed to it

    KoXmlElement name = objectElement.namedItem("OBJECTNAME").toElement();

    QString nameStr = name.attribute("objectName");
    if (!nameStr.isEmpty()) {
        content->addAttribute("draw:name", nameStr);
    }

    KoXmlElement size = objectElement.namedItem("SIZE").toElement();
    KoXmlElement orig = objectElement.namedItem("ORIG").toElement();

    double y = orig.attribute("y").toDouble();
    y -= m_pageHeight * (m_currentPage - 1);

    QPointF o(orig.attribute("x").toDouble(), y);

    content->addAttribute("draw:id", QString("object%1").arg(m_objectIndex));

    QSizeF s(size.attribute("width").toDouble(), size.attribute("height").toDouble());
    content->addAttributePt("svg:width", s.width());
    content->addAttributePt("svg:height", s.height());

    KoXmlElement angle = objectElement.namedItem("ANGLE").toElement();
    if (!angle.isNull()) {
        double angInRad = -angle.attribute("value").toDouble() * M_PI / 180.0;
        QMatrix m(cos(angInRad), -sin(angInRad), sin(angInRad), cos(angInRad), 0, 0);
        QPointF center(s.width() / 2, s.height() / 2);
        qreal rotX = 0.0;
        qreal rotY = 0.0;
        m.map(center.x(), center.y(), &rotX, &rotY);
        QPointF rot(rotX, rotY);
        QPointF trans(center - rot + o);

        QString transX;
        transX.setNum(trans.x(), 'g', DBL_DIG);
        QString transY;
        transY.setNum(trans.y(), 'g', DBL_DIG);
        QString str = QString("rotate(%1) translate(%2pt %3pt)").arg(angInRad).arg(transX).arg(transY);
        content->addAttribute("draw:transform", str);
    } else {
        content->addAttributePt("svg:x", o.x());
        content->addAttributePt("svg:y", o.y());
    }
}

QString Filterkpr2odf::rotateValue(double val)
{
    QString str;
    if (val != 0.0) {
        double value = ((double)val * M_PI) / -180.0;
        str = QString("rotate(%1)").arg(value);
    }
    return str;
}

void Filterkpr2odf::exportAnimation(const KoXmlElement& objectElement, int indentLevel)
{
    KoXmlElement effects = objectElement.namedItem("EFFECTS").toElement();
    if (!effects.isNull()) {
        QBuffer animationsBuffer;
        animationsBuffer.open(QIODevice::WriteOnly);
        KoXmlWriter animationsWriter(&animationsBuffer, indentLevel + 1);

        animationsWriter.startElement("presentation:show-shape");
        animationsWriter.addAttribute("draw:shape-id", QString("object%1").arg(m_objectIndex));

        if (effects.hasAttribute("effect")) {
            QString effect;
            QString direction;
            int effectInt = effects.attribute("effect").toInt();

            //Enum: Effect
            switch (effectInt) {
            case 0: //EF_NONE
                effect = "appear";
                break;
            case 1: //EF_COME_RIGHT
                effect = "move";
                direction = "from-right";
                break;
            case 2: //EF_COME_LEFT
                effect = "move";
                direction = "from-left";
                break;
            case 3: //EF_COME_TOP
                effect = "move";
                direction = "from-top";
                break;
            case 4: //EF_COME_BOTTOM
                effect = "move";
                direction = "from-bottom";
                break;
            case 5: //EF_COME_RIGHT_TOP
                effect = "move";
                direction = "from-upper-right";
                break;
            case 6: //EF_COME_RIGHT_BOTTOM
                effect = "move";
                direction = "from-lower-right";
                break;
            case 7: //EF_COME_LEFT_TOP
                effect = "move";
                direction = "from-upper-left";
                break;
            case 8: //EF_COME_LEFT_BOTTOM
                effect = "move";
                direction = "from-lower-left";
                break;
            case 9: //EF_WIPE_LEFT
                effect = "fade";
                direction = "from-left";
                break;
            case 10: //EF_WIPE_RIGHT
                effect = "fade";
                direction = "from-right";
                break;
            case 11: //EF_WIPE_TOP
                effect = "fade";
                direction = "from-top";
                break;
            case 12: //EF_WIPE_BOTTOM
                effect = "fade";
                direction = "from-bottom";
                break;
            }//switch effectInt

            animationsWriter.addAttribute("presentation:effect", effect);
            if (!direction.isNull()) {
                animationsWriter.addAttribute("presentation:direction", direction);
            }
        }//if effect

        if (effects.hasAttribute("speed")) {
            QString speed;
            int speedInt = effects.attribute("speed").toInt();
            //Enum: EffectSpeed
            switch (speedInt) {
            case 0: //ES_SLOW
                speed = "slow";
                break;
            case 1: //ES_MEDIUM
                speed = "medium";
                break;
            case 2: //ES_FAST
                speed = "fast";
                break;
            }
            animationsWriter.addAttribute("presentation:speed", speed);
        }

        KoXmlElement timer = objectElement.namedItem("TIMER").toElement();
        if (timer.hasAttribute("appearTimer") && (timer.attribute("appearTimer") != "1")) {
            QTime time;
            time = time.addSecs(timer.attribute("timer").toInt());
            animationsWriter.addAttribute("presentation:delay", time.toString("'PT'hh'H'mm'M'ss'S'"));
        }

        KoXmlElement appearSoundEffect = objectElement.namedItem("APPEARSOUNDEFFECT").toElement();
        if (appearSoundEffect.attribute("appearSoundFileName") == "1") {
            animationsWriter.startElement("presentation:sound");
            animationsWriter.addAttribute("xlink:href", m_sounds[ appearSoundEffect.attribute("appearSoundFileName")]);
            animationsWriter.addAttribute("xlink:type", "simple");
            animationsWriter.addAttribute("xlink:show", "new");
            animationsWriter.addAttribute("xlink:actuate", "onRequest");
            animationsWriter.endElement();//presentation:sound
        }
        animationsWriter.endElement();//presentation:show-shape

        KoXmlElement presnum = objectElement.namedItem("PRESNUM").toElement();
        int presnumValue = presnum.attribute("value", "0").toInt();

        QString animationsContents = QString::fromUtf8(animationsBuffer.buffer(),
                                     animationsBuffer.buffer().size());

        QList<QString> effectList = m_pageAnimations.take(presnumValue);  //Qt constructs a default object if Key is not found
        effectList.append(animationsContents);
        m_pageAnimations.insert(presnumValue, effectList);
    }//if !effects.isNull()

    KoXmlElement disappear = objectElement.namedItem("DISAPPEAR").toElement();
    if (!disappear.isNull() && (disappear.attribute("doit") == "1"))
        //in KPR the effect it's saved and not displayed unless doit is set to 1
    {
        QBuffer animationsBuffer;
        animationsBuffer.open(QIODevice::WriteOnly);
        KoXmlWriter animationsWriter(&animationsBuffer, indentLevel + 1);

        animationsWriter.startElement("presentation:hide-shape");
        animationsWriter.addAttribute("draw:shape-id", QString("object%1").arg(m_objectIndex));
        if (disappear.hasAttribute("effect")) {
            QString effect;
            QString direction;
            int effectInt = effects.attribute("effect").toInt();

            //Enum: Effect3
            switch (effectInt) {
            case 0: //EF3_NONE
                effect = "hide";
                break;
            case 1: //EF3_GO_RIGHT
                effect = "move";
                direction = "to-right";
                break;
            case 2: //EF3_GO_LEFT
                effect = "move";
                direction = "to-left";
                break;
            case 3: //EF3_GO_TOP
                effect = "move";
                direction = "to-top";
                break;
            case 4: //EF3_GO_BOTTOM
                effect = "move";
                direction = "to-bottom";
                break;
            case 5: //EF3_GO_RIGHT_TOP
                effect = "move";
                direction = "to-upper-right";
                break;
            case 6: //EF3_GO_RIGHT_BOTTOM
                effect = "move";
                direction = "to-lower-right";
                break;
            case 7: //EF3_GO_LEFT_TOP
                effect = "move";
                direction = "to-upper-left";
                break;
            case 8: //EF3_GO_LEFT_BOTTOM
                effect = "move";
                direction = "to-lower-left";
                break;
            case 9: //EF3_WIPE_LEFT
                effect = "fade";
                direction = "from-left";
                break;
            case 10: //EF3_WIPE_RIGHT
                effect = "fade";
                direction = "from-right";
                break;
            case 11: //EF3_WIPE_TOP
                effect = "fade";
                direction = "from-top";
                break;
            case 12: //EF3_WIPE_BOTTOM
                effect = "fade";
                direction = "from-bottom";
                break;
            }//switch effectInt

            animationsWriter.addAttribute("presentation:effect", effect);
            if (!direction.isNull()) {
                animationsWriter.addAttribute("presentation:direction", direction);
            }
        }//if effect

        if (disappear.hasAttribute("speed")) {
            QString speed;
            int speedInt = effects.attribute("speed").toInt();
            //Enum: EffectSpeed
            switch (speedInt) {
            case 0: //ES_SLOW
                speed = "slow";
                break;
            case 1: //ES_MEDIUM
                speed = "medium";
                break;
            case 2: //ES_FAST
                speed = "fast";
                break;
            }
            animationsWriter.addAttribute("presentation:speed", speed);
        }

        KoXmlElement timer = objectElement.namedItem("TIMER").toElement();
        if (timer.hasAttribute("disappearTimer") && (timer.attribute("disappearTimer") != "1")) {
            QTime time;
            time = time.addSecs(timer.attribute("timer").toInt());
            animationsWriter.addAttribute("presentation:delay", time.toString("'PT'hh'H'mm'M'ss'S'"));
        }

        KoXmlElement appearSoundEffect = objectElement.namedItem("DISAPPEARSOUNDEFFECT").toElement();
        if (appearSoundEffect.attribute("disappearSoundEffect") == "1") {
            animationsWriter.startElement("presentation:sound");
            animationsWriter.addAttribute("xlink:href", m_sounds[ appearSoundEffect.attribute("disappearSoundFileName")]);
            animationsWriter.addAttribute("xlink:type", "simple");
            animationsWriter.addAttribute("xlink:show", "new");
            animationsWriter.addAttribute("xlink:actuate", "onRequest");
            animationsWriter.endElement();//presentation:sound
        }
        animationsWriter.endElement();//presentation:hide-shape

        int num = disappear.attribute("num", "0").toInt();

        QString animationsContents = QString::fromUtf8(animationsBuffer.buffer(),
                                     animationsBuffer.buffer().size());

        QList<QString> effectList = m_pageAnimations.take(num);  //Qt constructs a default object if Key is not found
        effectList.append(animationsContents);
        m_pageAnimations.insert(num, effectList);
    }//if !disappear.isNull()
}

void Filterkpr2odf::saveAnimations(KoXmlWriter* content)
{
    content->startElement("presentation:animations");
    QList<int> keys = m_pageAnimations.keys();
    qSort(keys);  //we need to store the effects in the order of their keys
    foreach(int key, keys) {
        QList<QString> effectList = m_pageAnimations.value(key);
        if (effectList.size() > 1) { //if it's just 1 effect we don't add the group tag
            content->startElement("presentation:animation-group");
            foreach(const QString & effect, effectList) {
                content->addCompleteElement(effect.toLatin1().data());
            }
            content->endElement();//presentation:animation-group
        } else {
            QString effect = effectList.at(0);
            content->addCompleteElement(effect.toLatin1().data());
        }
    }
    content->endElement();//presentation:animations

    //Clear the animation's hash because we save the animations per page
    m_pageAnimations.clear();
}

#include "StylesFilterkpr2odf.cpp"

#include "Filterkpr2odf.moc"
