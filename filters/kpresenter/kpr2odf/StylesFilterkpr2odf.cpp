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

//Qt's includes
#include <QTime>
#include <QColor>

#include <kdebug.h>
const QString Filterkpr2odf::createPageStyle(const KoXmlElement& page)
{
    KoGenStyle style(KoGenStyle::StyleDrawingPage, "drawing-page");

    bool useMasterBackground = false;
    if (page.nodeName() == "PAGE") {
        KoXmlElement backMaster = page.namedItem("BACKMASTER").toElement();
        if (!backMaster.isNull()) {
            style.addProperty("presentation:background-visible", backMaster.attribute("displayBackground", "1") == "1");
            style.addProperty("presentation:background-objects-visible", backMaster.attribute("displayMasterPageObject", "1") == "1");
            useMasterBackground = backMaster.attribute("useMasterBackground", "0") == "1";
        } else {
            //if BACKMASTER is not found we assume it's true
            style.addProperty("presentation:background-visible", true);
            style.addProperty("presentation:background-objects-visible", true);
            style.addProperty("draw:fill", "none");
        }
    }

    style.setAutoStyleInStylesDotXml(m_sticky);

    if (!page.hasChildNodes()) { //we check if this is an empty page
        return m_styles.lookup(style, "dp");
    }

    if (!useMasterBackground) {
        //it is not an empty page let's keep loading
        KoXmlElement backType = page.namedItem("BACKTYPE").toElement();
        if (backType.isNull() || backType.attribute("value") == "0") {
            //it's some form of color, plain or a gradient
            KoXmlElement bcType = page.namedItem("BCTYPE").toElement();
            if (bcType.isNull() || bcType.attribute("value") == "0") {
                //background is a plain color
                QString color = page.namedItem("BACKCOLOR1").toElement().attribute("color");
                //if the backcolor is not present it's implicitally white
                //unless a draw:fill is found, in which case even though a
                //draw:fill-color is not present it's black in KPresenter2.0
                style.addProperty("draw:fill", "solid");
                style.addProperty("draw:fill-color", page.namedItem("BACKCOLOR1").toElement().attribute("color", "#ffffff"));
            } else {
                //background is a gradient
                style.addProperty("draw:fill", "gradient");
                style.addProperty("draw:fill-gradient-name", createGradientStyle(page));
            }
        } else {
            //it's a picture instead
            QString pictureName = getPictureNameFromKey(page.namedItem("BACKPICTUREKEY").toElement());
            KoXmlElement backView = page.namedItem("BACKVIEW").toElement();
            style.addProperty("draw:fill", "bitmap");

            //The image is specified by a draw:fill-image style in draw:fill-image-name
            KoGenStyle drawFillImage(KoGenStyle::StyleFillImage);

            //default values
            drawFillImage.addAttribute("xlink:href", "Pictures/" + m_pictures[ pictureName ]);
            drawFillImage.addAttribute("xlink:type", "simple");
            drawFillImage.addAttribute("xlink:show", "embed");
            drawFillImage.addAttribute("xlink:actuate", "onLoad");
            QString repeat;
            if (backView.isNull()) {
                //the picture is just centered
                repeat = "no-repeat";
            } else if (backView.attribute("value") == "0") {
                //the picture is stretched
                repeat = "stretch";
            } else if (backView.attribute("value") == "2") {
                //picture is in mosaic
                repeat = "repeat";
            }
            style.addProperty("style:repeat", repeat);

            style.addProperty("draw:fill-image-name", m_styles.lookup(drawFillImage, "picture"));
        }
    }

    //Add the duration of the page effect
    KoXmlElement pageDuration = page.namedItem("PGTIMER").toElement();
    if (!pageDuration.isNull()) {
        QTime time;
        time = time.addSecs(pageDuration.attribute("timer", "0").toInt());
        style.addProperty("presentation:duration", time.toString("'PT'hh'H'mm'M'ss'S'"));
        style.addProperty("presentation:transition-type", "automatic");
    }

    //Add the page effect
    //Enum: PageEffect
    KoXmlElement pageEffect = page.namedItem("PGEFFECT").toElement();
    if (!pageEffect.isNull()) {
        QString effectName("none");  //we default to none
        int effect = pageEffect.attribute("value", "0").toInt();
        switch (effect) {
        case -1: //PEF_RANDOM
            effectName = "random";
            break;
        case 0: //PEF_NONE
            effectName = "none";
            break;
        case 1: //PEF_CLOSE_HORZ
            effectName = "close-horizontal";
            break;
        case 2: //PEF_CLOSE_VERT
            effectName = "close-vertical";
            break;
        case 3: //PEF_CLOSE_ALL
            effectName = "close";
            break;
        case 4: //PEF_OPEN_HORZ
            effectName = "open-horizontal";
            break;
        case 5: //PEF_OPEN VERT
            effectName = "open-vertical";
            break;
        case 6: //PEF_OPEN_ALL
            effectName = "open";
            break;
        case 7: //PEF_INTERLOCKING_HORZ_1
            effectName = "interlocking-horizontal-left";
            break;
        case 8: //PEF_INTERLOCKING_HORZ_2
            effectName = "interlocking-horizontal-right";
            break;
        case 9: //PEF_INTERLOCKING_VERT_1
            effectName = "interlocking-vertical-up";
            break;
        case 10: //PEF_INTERLOCKING_VERT_2
            effectName = "interlocking-vertical-bottom";
            break;
        case 11: //PEF_SURROUND1
            effectName = "spiralin-left";
            break;
        case 12: //PEF_FLY1
            effectName = "fly-away";
            break;
        case 13: //PEF_BLINDS_HOR
            effectName = "horizontal-stripes";
            break;
        case 14: //PEF_BLINDS_VER
            effectName = "vertical-stripes";
            break;
        case 15: //PEF_BOX_IN
            effectName = "fade-to-center";
            break;
        case 16: //PEF_BOX_OUT
            effectName = "fade-from-center";
            break;
        case 17: //PEF_CHECKBOARD_ACROSS
            effectName = "horizontal-checkerboard";
            break;
        case 18: //PEF_CHECKBOARD_DOWN
            effectName = "vertical-checkerboard";
            break;
        case 19: //PEF_COVER_DOWN
            effectName = "fade-from-top";
            break;
        case 20: //PEF_UNCOVER_DOWN
            effectName = "uncover-to-bottom";
            break;
        case 21: //PEF_COVER_UP
            effectName = "fade-from-bottom";
            break;
        case 22: //PEF_UNCOVER_UP
            effectName = "uncover-to-top";
            break;
        case 23: //PEF_COVER_LEFT
            effectName = "fade-from-right";
            break;
        case 24: //PEF_UNCOVER_LEFT
            effectName = "uncover-to-left";
            break;
        case 25: //PEF_COVER_RIGHT
            effectName = "fade-from-left";
            break;
        case 26: //PEF_UNCOVER_RIGHT
            effectName = "uncover-to-right";
            break;
        case 27: //PEF_COVER_LEFT_UP
            effectName = "fade-from-lowerright";
            break;
        case 28: //PEF_UNCOVER_LEFT_UP
            effectName = "uncover-to-upperleft";
            break;
        case 29: //PEF_COVER_LEFT_DOWN
            effectName = "fade-from-upperrigh";
            break;
        case 30: //PEF_UNCOVER_LEFT_DOWN
            effectName = "uncover-to-lowerleft";
            break;
        case 31: //PEF_COVER_RIGHT_UP
            effectName = "fade-from-lowerleft";
            break;
        case 32: //PEF_UNCOVER_RIGHT_UP
            effectName = "uncover-to-upperright";
            break;
        case 33: //PEF_COVER_RIGHT_DOWN
            effectName = "fade-from-upperleft";
            break;
        case 34: //PEF_UNCOVER_RIGHT_DOWN
            effectName = "fade-from-lowerleft";
            break;
        case 35: //PEF_DISSOLVE
            effectName = "dissolve";
            break;
        case 36: //PEF_STRIPS_LEFT_UP
            effectName = "fade-from-lowerright";
            break;
        case 37: //PEF_STRIPS_LEFT_DOWN
            effectName = "fade-from-upperright";
            break;
        case 38: //PEF_STRIPS_RIGHT_UP
            effectName = "fade-from-lowerleft";
            break;
        case 39: //PEF_STRIPS_RIGHT_DOWN
            effectName = "fade-from-upperleft";
            break;
        case 40: //PEF_MELTING
            effectName = "melt";
            break;
        }//switch efect
        style.addProperty("presentation:transition-style", effectName);

        //Add the sound
        KoXmlElement soundEffect = page.namedItem("PGSOUNDEFFECT").toElement();
        if (!soundEffect.isNull() && soundEffect.attribute("soundEffect") != "0") {
            //As this is a "complex" tag we add it "manually"
            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            KoXmlWriter elementWriter(&buffer);
//          FIXME:   elementWriter.startElement( "style:presentation-properties" );
            elementWriter.startElement("presentation:sound");
            elementWriter.addAttribute("xlink:href", "Sounds/" + m_sounds[ soundEffect.attribute("soundFileName")]);
            elementWriter.addAttribute("xlink:type", "simple");
            elementWriter.addAttribute("xlink:show", "new");
            elementWriter.addAttribute("xlink:actuate", "onRequest");
            elementWriter.endElement();//presentation:sound
//             elementWriter.endElement();//style:presentation-properties

            QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
            style.addChildElement("presentationSound", elementContents);
        }
    }//if pageEfect is null
    return m_styles.lookup(style, "dp");
}

const QString Filterkpr2odf::createGradientStyle(const KoXmlElement& gradientElement)
{
    KoGenStyle style(KoGenStyle::StyleGradient);

    //KPresenter didn't allow to customize those attributes
    style.addAttribute("draw:start-intensity", "100%");
    style.addAttribute("draw:end-intensity", "100%");
    style.addAttribute("draw:border", "0%");
    //Check whether the gradient belongs to a page or to an object
    int type = 1;//we default to 1
    if (gradientElement.nodeName() == "PAGE" || gradientElement.nodeName() == "MASTERPAGE") {
        KoXmlElement backColor1 = gradientElement.namedItem("BACKCOLOR1").toElement();
        if (!backColor1.isNull()) {
            style.addAttribute("draw:start-color", backColor1.attribute("color"));
        } else {
            style.addAttribute("draw:end-color", "#ffffff");
        }

        KoXmlElement backColor2 = gradientElement.namedItem("BACKCOLOR2").toElement();
        if (!backColor2.isNull()) {
            style.addAttribute("draw:end-color", backColor2.attribute("color"));
        } else {
            style.addAttribute("draw:end-color", "#ffffff");
        }

        KoXmlElement bcType = gradientElement.namedItem("BCTYPE").toElement();
        if (!bcType.isNull()) {
            type = bcType.attribute("value").toInt();
        }

        KoXmlElement bGradient = gradientElement.namedItem("BGRADIENT").toElement();
        if (!bGradient.isNull()) {
            if (bGradient.attribute("unbalanced") == "0") {
                style.addAttribute("draw:cx", "50%");
                style.addAttribute("draw:cy", "50%");
            } else {
                int cx = bGradient.attribute("xfactor").toInt();
                int cy = bGradient.attribute("yfactor").toInt();
                style.addAttribute("draw:cx", QString("%1%").arg(cx / 4 + 50));
                style.addAttribute("draw:cy", QString("%1%").arg(cy / 4 + 50));
            }
        }
    } else {
        style.addAttribute("draw:start-color", gradientElement.attribute("color1"));
        style.addAttribute("draw:end-color", gradientElement.attribute("color2"));
        type = gradientElement.attribute("type").toInt();
        if (gradientElement.hasAttribute("unbalanced")) {
            if (gradientElement.attribute("unbalanced") == "0") {
                style.addAttribute("draw:cx", "50%");
                style.addAttribute("draw:cy", "50%");
            } else {
                int cx = gradientElement.attribute("xfactor").toInt();
                int cy = gradientElement.attribute("yfactor").toInt();
                style.addAttribute("draw:cx", QString("%1%").arg(cx / 4 + 50));
                style.addAttribute("draw:cy", QString("%1%").arg(cy / 4 + 50));
            }
        }
    }

    //export the type of the gradient
    QString typeString;
    QString angle;
    switch (type) {
    case 1:
        typeString = "linear";
        angle = "0";
        break;
    case 2:
        typeString = "linear";
        angle = "900";
        break;
    case 3:
        typeString = "linear";
        angle = "450";
        break;
    case 4:
        typeString = "linear";
        angle = "135";
        break;
    case 5:
        typeString = "radial";
        angle = "0";
        break;
    case 6:
        typeString = "square";
        angle = "0";
        break;
    case 7:
        typeString = "axial";
        angle = "0";
        break;
    }//switch type
    style.addAttribute("draw:style", typeString);
    style.addAttribute("draw:angle", angle);

    return m_styles.lookup(style, "gr");
}

const QString Filterkpr2odf::createPageLayout()
{
    //Create the page-layout that is: paper, header and footer sizes
    KoXmlElement paper = m_mainDoc.namedItem("DOC").namedItem("PAPER").toElement();
    KoXmlElement paperBorders = paper.namedItem("PAPERBORDERS").toElement();

    //page-layout-properties
    KoGenStyle style(KoGenStyle::StylePageLayout);
    style.setAutoStyleInStylesDotXml(true);

    if (paperBorders.hasAttribute("ptTop")) {
        style.addPropertyPt("fo:margin-top", paperBorders.attribute("ptTop").toDouble());
    }
    if (paperBorders.hasAttribute("ptBottom")) {
        style.addPropertyPt("fo:margin-bottom", paperBorders.attribute("ptBottom").toDouble());
    }
    if (paperBorders.hasAttribute("ptLeft")) {
        style.addPropertyPt("fo:margin-left", paperBorders.attribute("ptLeft").toDouble());
    }
    if (paperBorders.hasAttribute("ptRight")) {
        style.addPropertyPt("fo:margin-right", paperBorders.attribute("ptRight").toDouble());
    }
    if (paper.hasAttribute("ptWidth")) {
        style.addPropertyPt("fo:page-width", paper.attribute("ptWidth").toDouble());
    }
    if (paper.hasAttribute("ptHeight")) {
        style.addPropertyPt("fo:page-height", paper.attribute("ptHeight").toDouble());
    }
    style.addProperty("style:print-orientation", "landscape");

    //NOTE: header-style and footer-style are not present because in KPresenter they are treated as text boxes

    return m_styles.lookup(style, "pm");
}

const QString Filterkpr2odf::createMasterPageStyle(const KoXmlNode & objects, const KoXmlElement & masterBackground)
{
    //KoXmlElement header( m_mainDoc.namedItem( "DOC" ).namedItem( "HEADER" ).toElement() );
    //KoXmlElement footer( m_mainDoc.namedItem( "DOC" ).namedItem( "FOOTER" ).toElement() );

    // set that we work on master
    m_sticky = true;

    KoGenStyle style(KoGenStyle::StyleMaster, "");
    style.addAttribute("style:page-layout-name", createPageLayout());

    style.addAttribute("draw:style-name", createPageStyle(masterBackground));

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);

    convertObjects(&xmlWriter, objects);

    m_sticky = false;

    QString contentElement = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("master", contentElement);

    return m_styles.lookup(style, "Default");
}
//TODO: load the tags still missing
const QString Filterkpr2odf::createGraphicStyle(const KoXmlElement& element)
{
    //A graphic style is wiely used by a broad type of objects, hence can have many different properties
    KoGenStyle style(KoGenStyle::StyleGraphicAuto, "graphic");
    KoXmlElement textObject(element.namedItem("TEXTOBJ").toElement());
    if (!textObject.isNull()) {
        if (textObject.hasAttribute("verticalAlign")) {
            QString textAligment = textObject.attribute("verticalAlign");
            if (textAligment == "center") {
                //all the other values are valid except center that is called middle in ODF
                textAligment = "middle";
            }
            style.addProperty("draw:textarea-vertical-align", textAligment);
        }
        if (textObject.hasAttribute("bleftpt")) {
            style.addPropertyPt("fo:padding-left", textObject.attribute("bleftpt").toDouble());
        }
        if (textObject.hasAttribute("bbottompt")) {
            style.addPropertyPt("fo:padding-bottom", textObject.attribute("bbottompt").toDouble());
        }
        if (textObject.hasAttribute("btoppt")) {
            style.addPropertyPt("fo:padding-top", textObject.attribute("btoppt").toDouble());
        }
        if (textObject.hasAttribute("brightpt")) {
            style.addPropertyPt("fo:padding-right", textObject.attribute("brightpt").toDouble());
        }
    }

    KoXmlElement pen = element.namedItem("PEN").toElement();
    if (!pen.isNull()) {
        style.addPropertyPt("svg:stroke-width", pen.attribute("width").toDouble());
        style.addProperty("svg:stroke-color", pen.attribute("color"));

        QString stroke;
        int strokeStyle = pen.attribute("style").toInt();
        if (strokeStyle == 1) {
            stroke = "solid";
        } else if (strokeStyle >= 2 && strokeStyle <= 5) {
            stroke = "dash";
            style.addProperty("draw:stroke-dash", createStrokeDashStyle(strokeStyle));
        } else {
            stroke = "none";
        }
        style.addProperty("draw:stroke", stroke);
    } else {
        //even if the pen is not pressent for some objects it has to have a black stroke
        if (element.attribute("type") == "1"
                || element.attribute("type") == "2"
                || element.attribute("type") == "3"
                || element.attribute("type") == "5"
                || element.attribute("type") == "8"
                || element.attribute("type") == "11"
                || element.attribute("type") == "12"
                || element.attribute("type") == "13"
                || element.attribute("type") == "14"
                || element.attribute("type") == "15"
                || element.attribute("type") == "16") {
            style.addProperty("draw:stroke", "solid");
            style.addProperty("svg:stroke-width", "1px");
            style.addProperty("svg:stroke-color", "#000000");
        }
    }

    //We now define what's the object filled with, we "default" to a brush if both attributes are present
    KoXmlElement brush = element.namedItem("BRUSH").toElement();
    KoXmlElement fillType = element.namedItem("FILLTYPE").toElement();

    int fillTypeValue = 0;
    if (! fillType.isNull()) {
        fillTypeValue = fillType.attribute("value").toInt();
    }

    KoXmlElement gradient = element.namedItem("GRADIENT").toElement();
    KoXmlElement filename = element.namedItem("FILENAME").toElement();
    bool isConnection = filename.attribute("value").startsWith("Connections");
    QString fill;
    if (!brush.isNull() && !isConnection && fillTypeValue == 0) {
        QString fillColor(brush.attribute("color"));

        int brushStyle = brush.attribute("style").toInt();
        if (brushStyle == 1) {
            fill = "solid";
            style.addProperty("draw:fill-color", fillColor);
        } else if (brushStyle >= 9 && brushStyle <= 14) {
            fill = "hatch";
            style.addProperty("draw:fill-hatch-name",  createHatchStyle(brushStyle, fillColor));
        } else if (brushStyle >= 2 && brushStyle <= 8) {
            //in KPR files transparency was used in ODF opacity is used instead calculated as 100-transparency
            int opacity = 100;
            if (brushStyle == 2)
                opacity = 6;
            else if (brushStyle == 3)
                opacity = 12;
            else if (brushStyle == 4)
                opacity = 37;
            else if (brushStyle == 5)
                opacity = 50;
            else if (brushStyle == 6)
                opacity = 63;
            else if (brushStyle == 7)
                opacity = 88;
            else if (brushStyle == 8)
                opacity = 94;
            //Opacity is a gradient, in this case an uniform one
            style.addProperty("draw:opacity", createOpacityGradientStyle(opacity));
        }
    } else if (!gradient.isNull() && fillTypeValue == 1) {
        fill = "gradient";
        style.addProperty("draw:fill-gradient-name", createGradientStyle(gradient));
    } else {
        fill = "none";
    }

    if (!fill.isNull()) { //don't save ""
        style.addProperty("draw:fill", fill);
    }

    KoXmlElement lineBegin = element.namedItem("LINEBEGIN").toElement();
    if (!lineBegin.isNull()) {
        style.addProperty("draw:marker-start-width", "0.25cm");

        int markerStyle = lineBegin.attribute("value").toInt();
        style.addProperty("draw:marker-start", createMarkerStyle(markerStyle));
    }

    KoXmlElement lineEnd = element.namedItem("LINEEND").toElement();
    if (!lineEnd.isNull()) {
        style.addProperty("draw:marker-end-width", "0.25cm");

        int markerStyle = lineEnd.attribute("value").toInt();
        style.addProperty("draw:marker-end", createMarkerStyle(markerStyle));
    }

    KoXmlElement shadow = element.namedItem("SHADOW").toElement();
    if (!shadow.isNull()) {
        style.addProperty("draw:shadow", "visible");
        style.addProperty("draw:shadow-color", shadow.attribute("color"));

        QString shadowOffsetX;
        QString shadowOffsetY;
        int direction = shadow.attribute("direction").toInt();
        QString distance = QString("%1pt").arg(shadow.attribute("distance").toDouble());
        //Enum: ShadowDirection
        switch (direction) {
        case 1: //Left Up
            shadowOffsetX = '-' + distance;
            shadowOffsetY = '-' + distance;
            break;
        case 2: //Up
            shadowOffsetX = "0pt";
            shadowOffsetY = '-' + distance;
            break;
        case 3: //Right Up
            shadowOffsetX = distance;
            shadowOffsetY = '-' + distance;
            break;
        case 4: //Right
            shadowOffsetX = distance;
            shadowOffsetY = "0pt";
            break;
        case 5: //Right Bottom
            shadowOffsetX = distance;
            shadowOffsetY = distance;
            break;
        case 6: //Bottom
            shadowOffsetX = "0pt";
            shadowOffsetY = distance;
            break;
        case 7: //Left Bottom
            shadowOffsetX = '-' + distance;
            shadowOffsetY = distance;
            break;
        case 8: //Left
            shadowOffsetX = '-' + distance;
            shadowOffsetY = "0pt";
            break;
        }
        style.addProperty("draw:shadow-offset-x", shadowOffsetX);
        style.addProperty("draw:shadow-offset-y", shadowOffsetY);
    }

    //If what we're loading is the style for an image and some form of mirror, luminance, greyscale... is applied to it we must load it in the GrpahicStyle
    KoXmlElement pictureSettings = element.namedItem("PICTURESETTINGS").toElement();
    if (!pictureSettings.isNull()) {
        int mirrorType = pictureSettings.attribute("mirrorType", "0").toInt();
        QString mirror;
        switch (mirrorType) {
        case 0: //normal
            break;
        case 1:
            mirror = "horizontal";
            break;
        case 2:
            mirror = "vertical";
            break;
        case 3:
            mirror = "vertical horizontal";
            break;
        }
        if (!mirror.isNull()) {
            style.addProperty("style:mirror", mirror);
        }

        bool grayscal = pictureSettings.attribute("grayscal", "0") == "1";
        if (grayscal) {
            style.addProperty("draw:color-mode", "greyscale");
        }

        int bright = pictureSettings.attribute("bright", "0").toInt();
        if (bright != 0) {
            style.addProperty("draw:luminance", QString("%1%").arg(bright));
        }

        bool swapRGB = pictureSettings.attribute("bright", "0") == "1";
        if (swapRGB) {
            style.addProperty("draw:color-inversion", "true");
        }

        //NOTE: depth is not portable
    }

//     style.addAttribute( "style:parent-style-name", "standard" ); TODO: add the standard Graphic style
    style.setAutoStyleInStylesDotXml(m_sticky);

    return m_styles.lookup(style, "gr");
}

const QString Filterkpr2odf::createOpacityGradientStyle(int opacity)
{
    //Opacity wasn't a gradient in KPR so we go from and to the same value
    KoGenStyle style(KoGenStyle::StyleOpacity);
    QString opacityString = QString("%1%").arg(opacity);
    style.addAttribute("draw:start", opacityString);
    style.addAttribute("draw:end", opacityString);
    return m_styles.lookup(style, "op");
}

const QString Filterkpr2odf::createMarkerStyle(int markerType)
{
    KoGenStyle style(KoGenStyle::StyleMarker);

    QString displayName;
    QString viewBox;
    QString d;

    //Enum: LineEnd
    switch (markerType) {
    case 0: //Normal
        //nothing
        break;
    case 1:
        displayName = "Arrow";
        viewBox = "0 0 20 30";
        d = "m10 0-10 30h20z";
        break;
    case 2:
        displayName = "Square";
        viewBox = "0 0 10 10";
        d = "m0 0h10v10h-10z";
        break;
    case 3:
        displayName = "Circle";
        viewBox = "0 0 1131 1131";
        d = "m462 1118-102-29-102-51-93-72-72-93-51-102-29-102-13-105 13-102 29-106 51-102 72-89 93-72 102-50 102-34 106-9 101 9 106 34 98 50 93 72 72 89 51 102 29 106 13 102-13 105-29 102-51 102-72 93-93 72-98 51-106 29-101 13z";
        break;
    case 4:
        displayName = "Line Arrow";
        viewBox = "0 0 1122 2243";
        d = "m0 2108v17 17l12 42 30 34 38 21 43 4 29-8 30-21 25-26 13-34 343-1532 339 1520 13 42 29 34 39 21 42 4 42-12 34-30 21-42v-39-12l-4 4-440-1998-9-42-25-39-38-25-43-8-42 8-38 25-26 39-8 42z";
        break;
    case 5:
        displayName = "Dimension Lines";
        viewBox = "0 0 836 110";
        d = "m0 0h278 278 280v36 36 38h-278-278-280v-36-36z";
        break;
    case 6:
        displayName = "Doble Arrow";
        viewBox = "0 0 1131 1918";//FIXME: same as Double line arrow, not sure if it's ok, nothing in KPresenter1.6
        d = "m737 1131h394l-564-1131-567 1131h398l-398 787h1131z";
        break;
    case 7:
        displayName = "Double Line Arrow";
        viewBox = "0 0 1131 1918";
        d = "m0 11h312 312h122z";
        break;
    }//switch markerType

    style.addAttribute("draw:display-name", displayName);
    style.addAttribute("draw:viewBox", viewBox);
    style.addAttribute("draw:d", d);

    return m_styles.lookup(style, "mks");;
}

const QString Filterkpr2odf::createStrokeDashStyle(int strokeStyle)
{
    KoGenStyle style(KoGenStyle::StyleStrokeDash);

    //"Containment" strings, filled according to the type of the strokeStyle
    QString displayName;
    QString styleString;
    QString dots1;
    QString dots1_length;
    QString dots2;
    QString dots2_length;
    QString distance;

    switch (strokeStyle) {
    case 2:
        displayName = "Fine Dashed";
        styleString = "rect";
        dots1 = "1";
        dots1_length = "0.508cm";
        dots2 = "1";
        dots2_length = "0.508cm";
        distance = "0.508cm";
        break;
    case 3:
        displayName = "Fine Dotted";
        styleString = "rect";
        dots1 = "1";
        distance = "0.257cm";
        break;
    case 4:
        displayName = "Ultrafine 1 Dot 1 Dash";
        styleString = "rect";
        dots1 = "1";
        dots1_length = "0.051cm";
        dots2 = "1";
        dots2_length = "0.254cm";
        distance = "0.127cm";
        break;
    case 5:
        displayName = "2 Dots 1 Dash";
        styleString = "rect";
        dots1 = "2";
        dots2 = "1";
        dots2_length = "0.203cm";
        distance = "0.203cm";
        break;
    }//switch strokeStyle

    //Not all the strings are filled always so in oder to not
    //flood the style with unneeded "", we check if it was written
    style.addAttribute("draw:display-name", displayName);
    style.addAttribute("draw:style", styleString);
    style.addAttribute("draw:dots1", dots1);
    style.addAttribute("draw:distance", distance);
    if (!dots1_length.isNull()) {
        style.addAttribute("draw:dots1-length", dots1_length);
    }
    if (!dots2.isNull()) {
        style.addAttribute("draw:dots2", dots2);
    }
    if (!dots2_length.isNull()) {
        style.addAttribute("draw:dots2-length", dots2_length);
    }

    return m_styles.lookup(style, "stroke");
}

const QString Filterkpr2odf::createHatchStyle(int brushStyle, QString fillColor)
{
    KoGenStyle style(KoGenStyle::StyleHatch);

    //"Contaimnet" strings
    QString displayName;
    QString styleString;
    QString distance;
    QString rotation;

    //Other numbers aren't needed because those aren't a hatch style
    switch (brushStyle) {
    case 9:
        displayName = fillColor + " 0 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "0";
        break;
    case 10:
        displayName = fillColor + " 90 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "900";
        break;
    case 11:
        displayName = fillColor + " Crossed 0 Degrees";
        styleString = "double";
        distance = "0.076cm";
        rotation = "900";
        break;
    case 12:
        displayName = fillColor + " 45 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "450";
        break;
    case 13:
        displayName = fillColor + " -45 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "3150";
        break;
    case 14:
        displayName = fillColor + " Crossed 45 Degrees";
        styleString = "double";
        distance = "0.076cm";
        rotation = "450";
        break;
    }

    style.addAttribute("draw:display-name", displayName);
    style.addAttribute("draw:style", styleString);
    style.addAttribute("draw:color", fillColor);
    style.addAttribute("draw:distance", distance);
    style.addAttribute("draw:rotation", rotation);

    return m_styles.lookup(style, "hs");
}

const QString Filterkpr2odf::createParagraphStyle(const KoXmlElement& element)
{
    KoGenStyle style(KoGenStyle::StyleAuto, "paragraph");

    QString textAlign;
    if (element.hasAttribute("align")) {
        int align = element.attribute("align").toInt();
        switch (align) {
        case 0: // left
            textAlign = "start";
            break;
        case 2: // right
            textAlign = "end";
            break;
        case 4: // center
            textAlign = "center";
            break;
        case 8: // justify
            textAlign = "justify";
            break;
        }
    }
    if (!textAlign.isNull()) {
        style.addProperty("fo:text-align", textAlign);
    }

    KoXmlElement indents = element.namedItem("INDENTS").toElement();
    if (!indents.isNull()) {
        QString marginLeft = QString("%1pt").arg(indents.attribute("left").toDouble());
        QString marginRight = QString("%1pt").arg(indents.attribute("right").toDouble());
        QString textIndent = QString("%1pt").arg(indents.attribute("first").toDouble());

        if (indents.hasAttribute("left")) {
            style.addPropertyPt("fo:margin-left", indents.attribute("left").toDouble());
        }
        if (indents.hasAttribute("right")) {
            style.addPropertyPt("fo:margin-right", indents.attribute("right").toDouble());
        }
        if (indents.hasAttribute("first")) {
            style.addPropertyPt("fo:text-indent", indents.attribute("first").toDouble());
        }
    }

    KoXmlElement offsets = element.namedItem("OFFSETS").toElement();
    if (!offsets.isNull()) {
        if (offsets.hasAttribute("before")) {
            style.addPropertyPt("fo:margin-top", offsets.attribute("before").toDouble());
        }
        if (offsets.hasAttribute("after")) {
            style.addPropertyPt("fo:margin-bottom", offsets.attribute("after").toDouble());
        }
    }

    KoXmlElement lineSpacing = element.namedItem("LINESPACING").toElement();
    if (!lineSpacing.isNull()) {
        QString type = lineSpacing.attribute("type");
        QString lineHeight;
        double lineSpacingDouble = -1;
        double lineHeightAtLeast = -1;
        if (type == "single") {
            lineHeight = "100%";
        } else if (type == "oneandhalf") {
            lineHeight = "150%";
        } else if (type == "double") {
            lineHeight = "200%";
        } else if (type == "multiple") {
            lineHeight = QString("%1%").arg(lineSpacing.attribute("spacingvalue").toInt() * 100);
        } else if (type == "custom") {
            lineSpacingDouble = lineSpacing.attribute("spacingvalue").toDouble();
        } else if (type == "atleast") {
            lineHeightAtLeast = lineSpacing.attribute("spacingvalue").toDouble();
        }

        if (!lineHeight.isNull()) {
            style.addProperty("fo:line-height", lineHeight);
        }
        if (lineSpacingDouble != -1) {
            style.addPropertyPt("text:line-spacing", lineSpacingDouble);
        }
        if (lineHeightAtLeast != -1) {
            style.addPropertyPt("style:line-height-at-least", lineHeightAtLeast);
        }
    }

    KoXmlElement leftBorder = element.namedItem("LEFTBORDER").toElement();
    KoXmlElement rightBorder = element.namedItem("RIGHTBORDER").toElement();
    KoXmlElement topBorder = element.namedItem("TOPBORDER").toElement();
    KoXmlElement bottomBorder = element.namedItem("BOTTOMBORDER").toElement();
    if (!leftBorder.isNull()) {
        style.addProperty("fo:border-left", convertBorder(leftBorder));
    }
    if (!rightBorder.isNull()) {
        style.addProperty("fo:border-right", convertBorder(rightBorder));
    }
    if (!topBorder.isNull()) {
        style.addProperty("fo:border-top", convertBorder(topBorder));
    }
    if (!bottomBorder.isNull()) {
        style.addProperty("fo:border-left", convertBorder(bottomBorder));
    }

    return m_styles.lookup(style, "P");
}

QString Filterkpr2odf::convertBorder(const KoXmlElement& border)
{
    QString style;
    int styleInt = border.attribute("style").toInt();
    if (styleInt == 5) {
        style = "double";
    } else {
        style = "solid";
    }
    QString width = QString("%1pt").arg(border.attribute("width").toDouble());

    QColor color(border.attribute("red").toInt(), border.attribute("green").toInt(), border.attribute("blue").toInt());

    return QString("%1 %2 %3").arg(width).arg(style).arg(color.name());
}

const QString Filterkpr2odf::createTextStyle(const KoXmlElement& element)
{
    KoGenStyle style(KoGenStyle::StyleTextAuto, "text");

    if (element.hasAttribute("family")) {
        style.addProperty("style:font-name", element.attribute("family"));
    }
    if (element.hasAttribute("pointSize")) {
        style.addPropertyPt("fo:font-size", element.attribute("pointSize").toDouble());
    }
    if (element.hasAttribute("color")) {
        style.addProperty("fo:color", element.attribute("color"));
    }
    if (element.hasAttribute("bold")) {
        if (element.attribute("bold") == "1") {
            style.addProperty("fo:font-weight", "bold");
        }
    }
    if (element.hasAttribute("italic")) {
        if (element.attribute("italic") == "1") {
            style.addProperty("fo:font-style", "italic");
        }
    }
    if (element.hasAttribute("strikeOut")) {
        QString strikeOut = element.attribute("strikeOut");
        QString textLineThroughType;
        QString textLineThroughWidth;
        if (strikeOut == "single") {
            textLineThroughType = "single";
        } else if (strikeOut == "single-bold") {
            textLineThroughType = "single";
            textLineThroughWidth = "bold";
        } else if (strikeOut == "double") {
            textLineThroughType = "double";
        }
        style.addProperty("style:text-line-through-type", textLineThroughType);
        style.addProperty("style:text-line-through-style", "solid");
        if (!textLineThroughWidth.isNull()) { //avoid saving ""
            style.addProperty("style:text-line-through-width", textLineThroughWidth);
        }
    }
    if (element.hasAttribute("underline")) {
        QString underline = element.attribute("underline");
        QString underlineStyleLine = element.attribute("underlinestyleline");

        style.addProperty("style:text-underline-color", element.attribute("underlinecolor"));

        QString textUnderlineStyle;
        QString textUnderlineWidth;
        QString textUnderlineType;
        if (underlineStyleLine == "solid") {
            if (underline == "1") {
                textUnderlineType = "single";
                textUnderlineStyle = "solid";
            } else if (underline == "single-bold") {
                textUnderlineType = "single";
                textUnderlineStyle = "solid";
                textUnderlineWidth = "bold";
            } else if (underline == "double") {
                textUnderlineType = "double";
                textUnderlineStyle = "solid";
            } else if (underline == "wave") {
                textUnderlineType = "single";
                textUnderlineStyle = "wave";
            }
        }//if underlineStyleLine == solid
        else if (underlineStyleLine == "dot") {
            if (underline == "1") {
                textUnderlineType = "single";
                textUnderlineStyle = "dotted";
            } else if (underline == "single-bold") {
                textUnderlineType = "single";
                textUnderlineStyle = "dotted";
                textUnderlineWidth = "bold";
            }
        } else if (underlineStyleLine == "dash") {
            textUnderlineType = "single";
            textUnderlineStyle = "dash";
        }

        style.addProperty("style:text-underline-type", textUnderlineType);
        style.addProperty("style:text-underline-style", textUnderlineStyle);
        if (!textUnderlineWidth.isNull()) { //avoid saving ""
            style.addProperty("style:text-underline-width", textUnderlineWidth);
        }
    }//if element.hasAttribute( "strikeOut" )

    QString shadow = element.attribute("text-shadow");
    if (!shadow.isNull()) {
        style.addProperty("fo:text-shadow", shadow);
    }

    return m_styles.lookup(style, "T");
}

const QString Filterkpr2odf::createListStyle(const KoXmlElement& element)
{
    KoGenStyle style(KoGenStyle::StyleListAuto);

    static const int s_oasisCounterTypes[] = { '\0', '1', 'a', 'A', 'i', 'I',
            '\0', '\0', // custombullet, custom
            0x2022, // circle -> small disc
            0xE00A, // square
            0x25CF, // disc -> large disc
            0x27A2  // box -> right-pointing triangle
                                             };

    KoXmlElement counter = element.namedItem("COUNTER").toElement();

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer, 3);

    int type = counter.attribute("type").toInt();
    bool isBullet = false;
    if (6 <= type) {
        isBullet = true;
    }

    int depth = counter.attribute("depth", "0").toInt();
    elementWriter.startElement((isBullet) ? "text:list-level-style-bullet" : "text:list-level-style-number");
    elementWriter.addAttribute("text:level", depth + 1);

    if (isBullet) {
        QChar bulletChar;
        if (type == 6) { //STYLE_CUSTOMBULLET
            bulletChar = QChar(counter.attribute("bullet").toInt());
        } else {
            bulletChar = s_oasisCounterTypes[ type ];
        }
        elementWriter.addAttribute("text:bullet-char", QString(bulletChar));
    } else {
        if (type != 7) { //STYLE_CUSTOM in 1.6 was not implemented, I assume it's never used
            elementWriter.addAttribute("style:number-format", s_oasisCounterTypes[ type ]);
        }
        if (counter.attribute("restart", "0") == "1"
                || counter.attribute("restart") == "true") {
            QString start = counter.attribute("start", "1");
            elementWriter.addAttribute("text:start-value", start);
        }
    }

    elementWriter.addAttribute("style:num-prefix", counter.attribute("lefttext"));
    elementWriter.addAttribute("style:num-suffix", counter.attribute("righttext"));
    elementWriter.endElement();// text:list-level-style-bullet or text:list-level-style-number

    QString align;
    switch ((Qt::AlignmentFlag) element.attribute("align", "0").toInt()) {
    case Qt::AlignLeft:
        align = "left";
        break;
    case Qt::AlignRight:
        align = "right";
        break;
    case Qt::AlignHCenter:
        align = "center";
        break;
    case Qt::AlignJustify:
        align = "justify";
        break;
    default:
        align = "start";
        break;
    }
    elementWriter.startElement("text:list-level-properties");
    elementWriter.addAttribute("fo:text-align", align);
    elementWriter.endElement();//

    QString elementContent = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("listLevelStyle", elementContent);

    return m_styles.lookup(style, "L");
}
