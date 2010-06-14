/* libwpg
 * Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
 * Copyright (C) 2006 Fridrich Strba (fridrich.strba@bluewin.ch)
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02111-1301 USA
 *
 * For further information visit http://libwpg.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by
 * Corel Corporation or Corel Corporation Limited."
 */

#include "OdgExporter.hxx"
#include "GraphicsElement.hxx"
#include "GraphicsHandler.hxx"
#include <locale.h>

static std::string doubleToString(const double value)
{
  std::ostringstream tempStream;
  tempStream << value;
  std::string decimalPoint(localeconv()->decimal_point);
  if ((decimalPoint.size() == 0) || (decimalPoint == "."))
    return tempStream.str();
  std::string stringValue(tempStream.str());
  if (!stringValue.empty())
  {
    std::string::size_type pos;
    while ((pos = stringValue.find(decimalPoint)) != std::string::npos)
          stringValue.replace(pos,decimalPoint.size(),".");
  }
  return stringValue;
}


OdgExporter::OdgExporter(GraphicsHandler *pHandler, const bool isFlatXML):
        mpHandler(pHandler),
        m_fillRule(AlternatingFill),
        m_gradientIndex(1),
        m_dashIndex(1),
        m_styleIndex(1),
        m_width(0.0f),
        m_height(0.0f),
        m_isFlatXML(isFlatXML)
{
}

OdgExporter::~OdgExporter()
{
        for (std::vector <GraphicsElement *>::iterator iterStroke = mStrokeDashElements.begin();
                iterStroke != mStrokeDashElements.end(); iterStroke++)
                delete (*iterStroke);

        for (std::vector <GraphicsElement *>::iterator iterGradient = mGradientElements.begin();
                iterGradient != mGradientElements.end(); iterGradient++)
                delete (*iterGradient);

        for (std::vector <GraphicsElement *>::iterator iterAutomaticStyles = mAutomaticStylesElements.begin();
                iterAutomaticStyles != mAutomaticStylesElements.end(); iterAutomaticStyles++)
                delete (*iterAutomaticStyles);

        for (std::vector<GraphicsElement *>::iterator bodyIter = mBodyElements.begin();
                bodyIter != mBodyElements.end(); bodyIter++)
                delete (*bodyIter);
}

void OdgExporter::startGraphics(double width, double height)
{
        m_gradientIndex = 1;
        m_dashIndex = 1;
        m_styleIndex = 1;
        m_width = width;
        m_height = height;


        mpHandler->startDocument();
        OpenTagGraphicsElement tmpOfficeDocumentContent("office:document");
        tmpOfficeDocumentContent.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        tmpOfficeDocumentContent.addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
        tmpOfficeDocumentContent.addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
        tmpOfficeDocumentContent.addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
        tmpOfficeDocumentContent.addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
        tmpOfficeDocumentContent.addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
        tmpOfficeDocumentContent.addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
        tmpOfficeDocumentContent.addAttribute("xmlns:config", "urn:oasis:names:tc:opendocument:xmlns:config:1.0");
        tmpOfficeDocumentContent.addAttribute("xmlns:ooo", "http://openoffice.org/2004/office");
        tmpOfficeDocumentContent.addAttribute("office:version", "1.0");
        if (m_isFlatXML)
                        tmpOfficeDocumentContent.addAttribute("office:mimetype", "application/x-vnd.oasis.openoffice.drawing");
        tmpOfficeDocumentContent.write(mpHandler);

        OpenTagGraphicsElement("office:settings").write(mpHandler);

        OpenTagGraphicsElement configItemSetOpenElement("config:config-item-set");
        configItemSetOpenElement.addAttribute("config:name", "ooo:view-settings");
        configItemSetOpenElement.write(mpHandler);

        OpenTagGraphicsElement configItemOpenElement1("config:config-item");
        configItemOpenElement1.addAttribute("config:name", "VisibleAreaTop");
        configItemOpenElement1.addAttribute("config:type", "int");
        configItemOpenElement1.write(mpHandler);
        mpHandler->characters("0");
        mpHandler->endElement("config:config-item");

        OpenTagGraphicsElement configItemOpenElement2("config:config-item");
        configItemOpenElement2.addAttribute("config:name", "VisibleAreaLeft");
        configItemOpenElement2.addAttribute("config:type", "int");
        configItemOpenElement2.write(mpHandler);
        mpHandler->characters("0");
        mpHandler->endElement("config:config-item");

        OpenTagGraphicsElement configItemOpenElement3("config:config-item");
        configItemOpenElement3.addAttribute("config:name", "VisibleAreaWidth");
        configItemOpenElement3.addAttribute("config:type", "int");
        configItemOpenElement3.write(mpHandler);
        m_value.str("");
        m_value << (unsigned)(2540 * width);
        mpHandler->characters(m_value.str());
        mpHandler->endElement("config:config-item");

        OpenTagGraphicsElement configItemOpenElement4("config:config-item");
        configItemOpenElement4.addAttribute("config:name", "VisibleAreaHeight");
        configItemOpenElement4.addAttribute("config:type", "int");
        configItemOpenElement4.write(mpHandler);
        m_value.str("");
        m_value << (unsigned)(2540 * height);
        mpHandler->characters(m_value.str());
        mpHandler->endElement("config:config-item");

        mpHandler->endElement("config:config-item-set");

        mpHandler->endElement("office:settings");

}

void OdgExporter::endGraphics()
{
        OpenTagGraphicsElement("office:styles").write(mpHandler);

        for (std::vector <GraphicsElement *>::const_iterator iterStroke = mStrokeDashElements.begin();
                iterStroke != mStrokeDashElements.end(); iterStroke++)
                (*iterStroke)->write(mpHandler);

        for (std::vector <GraphicsElement *>::const_iterator iterGradient = mGradientElements.begin();
                iterGradient != mGradientElements.end(); iterGradient++)
                (*iterGradient)->write(mpHandler);

        mpHandler->endElement("office:styles");

        OpenTagGraphicsElement("office:automatic-styles").write(mpHandler);

        for (std::vector <GraphicsElement *>::const_iterator iterAutomaticStyles = mAutomaticStylesElements.begin();
                iterAutomaticStyles != mAutomaticStylesElements.end(); iterAutomaticStyles++)
                (*iterAutomaticStyles)->write(mpHandler);

        OpenTagGraphicsElement tmpStylePageLayoutOpenElement("style:page-layout");
        tmpStylePageLayoutOpenElement.addAttribute("style:name", "PM0");
        tmpStylePageLayoutOpenElement.write(mpHandler);

        OpenTagGraphicsElement tmpStylePageLayoutPropertiesOpenElement("style:page-layout-properties");
        tmpStylePageLayoutPropertiesOpenElement.addAttribute("fo:margin-top", "0in");
        tmpStylePageLayoutPropertiesOpenElement.addAttribute("fo:margin-bottom", "0in");
        tmpStylePageLayoutPropertiesOpenElement.addAttribute("fo:margin-left", "0in");
        tmpStylePageLayoutPropertiesOpenElement.addAttribute("fo:margin-right", "0in");
        m_value.str("");
        m_value <<  doubleToString(m_width) << "in";
        tmpStylePageLayoutPropertiesOpenElement.addAttribute("fo:page-width", m_value.str());
        m_value.str("");
        m_value << doubleToString(m_height)  << "in";
        tmpStylePageLayoutPropertiesOpenElement.addAttribute("fo:page-height", m_value.str());
        m_value.str("");
        tmpStylePageLayoutPropertiesOpenElement.addAttribute("style:print-orientation", "portrait");
        tmpStylePageLayoutPropertiesOpenElement.write(mpHandler);

        mpHandler->endElement("style:page-layout-properties");

        mpHandler->endElement("style:page-layout");

        OpenTagGraphicsElement tmpStyleStyleOpenElement("style:style");
        tmpStyleStyleOpenElement.addAttribute("style:name", "dp1");
        tmpStyleStyleOpenElement.addAttribute("style:family", "drawing-page");
        tmpStyleStyleOpenElement.write(mpHandler);

        OpenTagGraphicsElement tmpStyleDrawingPagePropertiesOpenElement("style:drawing-page-properties");
        // tmpStyleDrawingPagePropertiesOpenElement.addAttribute("draw:background-size", "border");
        tmpStyleDrawingPagePropertiesOpenElement.addAttribute("draw:fill", "none");
        tmpStyleDrawingPagePropertiesOpenElement.write(mpHandler);

        mpHandler->endElement("style:drawing-page-properties");

        mpHandler->endElement("style:style");

        mpHandler->endElement("office:automatic-styles");

        OpenTagGraphicsElement("office:master-styles").write(mpHandler);

        OpenTagGraphicsElement tmpStyleMasterPageOpenElement("style:master-page");
        tmpStyleMasterPageOpenElement.addAttribute("style:name", "Default");
        tmpStyleMasterPageOpenElement.addAttribute("style:page-layout-name", "PM0");
        tmpStyleMasterPageOpenElement.addAttribute("draw:style-name", "dp1");
        tmpStyleMasterPageOpenElement.write(mpHandler);

        mpHandler->endElement("style:master-page");

        mpHandler->endElement("office:master-styles");

        OpenTagGraphicsElement("office:body").write(mpHandler);

        OpenTagGraphicsElement("office:drawing").write(mpHandler);

        OpenTagGraphicsElement tmpDrawPageOpenElement("draw:page");
        tmpDrawPageOpenElement.addAttribute("draw:name", "page1");
        tmpDrawPageOpenElement.addAttribute("draw:style-name", "dp1");
        tmpDrawPageOpenElement.addAttribute("draw:master-page-name", "Default");
        tmpDrawPageOpenElement.write(mpHandler);

        for (std::vector<GraphicsElement *>::const_iterator bodyIter = mBodyElements.begin();
                bodyIter != mBodyElements.end(); bodyIter++)
        {
                (*bodyIter)->write(mpHandler);
        }

        mpHandler->endElement("draw:page");
        mpHandler->endElement("office:drawing");
        mpHandler->endElement("office:body");
        mpHandler->endElement("office:document");

        mpHandler->endDocument();
}

void OdgExporter::setPen(const libwpg::WPGPen& pen)
{
        m_pen = pen;
}

void OdgExporter::setBrush(const libwpg::WPGBrush& brush)
{
        m_brush = brush;
}

void OdgExporter::setFillRule(FillRule rule)
{
        m_fillRule = rule;
}

void OdgExporter::startLayer(unsigned int)
{
}

void OdgExporter::endLayer(unsigned int)
{
}

void OdgExporter::drawRectangle(const libwpg::WPGRect& rect, double rx, double )
{
        writeStyle();
        OpenTagGraphicsElement *pDrawRectElement = new OpenTagGraphicsElement("draw:rect");
        m_value.str("");
        m_value << "gr" << m_styleIndex-1;
        pDrawRectElement->addAttribute("draw:style-name", m_value.str());
        m_value.str("");
        m_value << doubleToString(rect.x1) << "in";
        pDrawRectElement->addAttribute("svg:x", m_value.str());
        m_value.str("");
        m_value << doubleToString(rect.y1) << "in";
        pDrawRectElement->addAttribute("svg:y", m_value.str());
        m_value.str("");
        m_value << doubleToString(rect.x2-rect.x1) << "in";
        pDrawRectElement->addAttribute("svg:width", m_value.str());
        m_value.str("");
        m_value << doubleToString(rect.y2-rect.y1) << "in";
        pDrawRectElement->addAttribute("svg:height", m_value.str());
        m_value.str("");
        m_value << doubleToString(rx) << "in";
        // FIXME: what to do when rx != ry ?
        pDrawRectElement->addAttribute("draw:corner-radius", m_value.str());
        m_value.str("");
        mBodyElements.push_back(static_cast<GraphicsElement *>(pDrawRectElement));
        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:rect")));
}

void OdgExporter::drawEllipse(const libwpg::WPGPoint& center, double rx, double ry)
{
        writeStyle();
        OpenTagGraphicsElement *pDrawEllipseElement = new OpenTagGraphicsElement("draw:ellipse");
        m_value.str("");
        m_value << "gr" << m_styleIndex-1;
        pDrawEllipseElement->addAttribute("draw:style-name", m_value.str());
        m_value.str("");
        m_value << doubleToString(center.x - rx) << "in";
        pDrawEllipseElement->addAttribute("svg:x", m_value.str());
        m_value.str("");
        m_value << doubleToString(center.y - ry) << "in";
        pDrawEllipseElement->addAttribute("svg:y", m_value.str());
        m_value.str("");
        m_value << doubleToString(2 * rx) << "in";
        pDrawEllipseElement->addAttribute("svg:width", m_value.str());
        m_value.str("");
        m_value << doubleToString(2 * ry) << "in";
        pDrawEllipseElement->addAttribute("svg:height", m_value.str());
        m_value.str("");
        mBodyElements.push_back(static_cast<GraphicsElement *>(pDrawEllipseElement));
        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:ellipse")));
}

void OdgExporter::drawPolygon(const libwpg::WPGPointArray& vertices)
{
        if(vertices.count() < 2)
                return;

        if(vertices.count() == 2)
        {
                const libwpg::WPGPoint& p1 = vertices[0];
                const libwpg::WPGPoint& p2 = vertices[1];

                writeStyle();
                OpenTagGraphicsElement *pDrawLineElement = new OpenTagGraphicsElement("draw:line");
                m_value.str("");
                m_value << "gr" << m_styleIndex-1;
                pDrawLineElement->addAttribute("draw:style-name", m_value.str());
                m_value.str("");
                pDrawLineElement->addAttribute("draw:text-style-name", "P1");
                pDrawLineElement->addAttribute("draw:layer", "layout");
                m_value << doubleToString(p1.x)  << "in";
                pDrawLineElement->addAttribute("svg:x1", m_value.str());
                m_value.str("");
                m_value << doubleToString(p1.y)  << "in";
                pDrawLineElement->addAttribute("svg:y1", m_value.str());
                m_value.str("");
                m_value  << doubleToString(p2.x)  << "in";
                pDrawLineElement->addAttribute("svg:x2", m_value.str());
                m_value.str("");
                m_value << doubleToString(p2.y)  << "in";
                pDrawLineElement->addAttribute("svg:y2", m_value.str());
                m_value.str("");
                mBodyElements.push_back(static_cast<GraphicsElement *>(pDrawLineElement));
                mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:line")));
        }
        else
        {
                // draw as path
                libwpg::WPGPath path;
                path.moveTo(vertices[0]);
                for(unsigned long ii = 1; ii < vertices.count(); ii++)
                        path.lineTo(vertices[ii]);
                path.closed = true;
                drawPath(path);
        }
}

void OdgExporter::drawPath(const libwpg::WPGPath& path)
{
        if(path.count() == 0)
                return;

        // try to find the bounding box
        // this is simple convex hull technique, the bounding box might not be
        // accurate but that should be enough for this purpose
        libwpg::WPGPoint p = path.element(0).point;
        libwpg::WPGPoint q = path.element(0).point;
        for(unsigned k = 0; k < path.count(); k++)
        {
                libwpg::WPGPathElement element = path.element(k);
                p.x = (p.x > element.point.x) ? element.point.x : p.x;
                p.y = (p.y > element.point.y) ? element.point.y : p.y;
                q.x = (q.x < element.point.x) ? element.point.x : q.x;
                q.y = (q.y < element.point.y) ? element.point.y : q.y;
                if(element.type == libwpg::WPGPathElement::CurveToElement)
                {
                        p.x = (p.x > element.extra1.x) ? element.extra1.x : p.x;
                        p.y = (p.y > element.extra1.y) ? element.extra1.y : p.y;
                        q.x = (q.x < element.extra1.x) ? element.extra1.x : q.x;
                        q.y = (q.y < element.extra1.y) ? element.extra1.y : q.y;
                        p.x = (p.x > element.extra2.x) ? element.extra2.x : p.x;
                        p.y = (p.y > element.extra2.y) ? element.extra2.y : p.y;
                        q.x = (q.x < element.extra2.x) ? element.extra2.x : q.x;
                        q.y = (q.y < element.extra2.y) ? element.extra2.y : q.y;
                }
        }
        double vw = q.x - p.x;
        double vh = q.y - p.y;

        writeStyle();

        OpenTagGraphicsElement *pDrawPathElement = new OpenTagGraphicsElement("draw:path");
        m_value.str("");
        m_value << "gr" << m_styleIndex-1;
        pDrawPathElement->addAttribute("draw:style-name", m_value.str());
        m_value.str("");
        pDrawPathElement->addAttribute("draw:text-style-name", "P1");
        pDrawPathElement->addAttribute("draw:layer", "layout");
        m_value << doubleToString(p.x)  << "in";
        pDrawPathElement->addAttribute("svg:x", m_value.str());
        m_value.str("");
        m_value << doubleToString(p.y)  << "in";
        pDrawPathElement->addAttribute("svg:y", m_value.str());
        m_value.str("");
        m_value << doubleToString(vw) << "in";
        pDrawPathElement->addAttribute("svg:width", m_value.str());
        m_value.str("");
        m_value << doubleToString(vh) << "in";
        pDrawPathElement->addAttribute("svg:height", m_value.str());
        m_value.str("");
        m_value << "0 0 " << (int)(vw*2540) << " " << (int)(vh*2540);
        pDrawPathElement->addAttribute("svg:viewBox", m_value.str());
        m_value.str("");

        for(unsigned i = 0; i < path.count(); i++)
        {
                libwpg::WPGPathElement element = path.element(i);
                libwpg::WPGPoint point = element.point;
                switch(element.type)
                {
                        // 2540 is 2.54*1000, 2.54 in = 1 in
                        case libwpg::WPGPathElement::MoveToElement:
                                m_value << "M" << (int)((point.x-p.x)*2540) << " ";
                                m_value << (int)((point.y-p.y)*2540);
                                break;

                        case libwpg::WPGPathElement::LineToElement:
                                m_value << "L" << (int)((point.x-p.x)*2540) << " ";
                                m_value << (int)((point.y-p.y)*2540);
                                break;

                        case libwpg::WPGPathElement::CurveToElement:
                                m_value << "C" << (int)((element.extra1.x-p.x)*2540) << " ";
                                m_value << (int)((element.extra1.y-p.y)*2540) << " ";
                                m_value << (int)((element.extra2.x-p.x)*2540) << " ";
                                m_value << (int)((element.extra2.y-p.y)*2540) << " ";
                                m_value << (int)((point.x-p.x)*2540) << " ";
                                m_value << (int)((point.y-p.y)*2540);
                                break;

                        default:
                                break;
                }
        }
        if(path.closed)
                m_value << " Z";
        pDrawPathElement->addAttribute("svg:d", m_value.str());
        m_value.str("");
        mBodyElements.push_back(static_cast<GraphicsElement *>(pDrawPathElement));
        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:path")));
}


void OdgExporter::drawBitmap(const libwpg::WPGBitmap& bitmap)
{
        OpenTagGraphicsElement *pDrawFrameElement = new OpenTagGraphicsElement("draw:frame");
        m_value.str("");
        m_value << doubleToString(bitmap.rect.x1) << "in";
        pDrawFrameElement->addAttribute("svg:x", m_value.str());
        m_value.str("");
        m_value << doubleToString(bitmap.rect.y1) << "in";
        pDrawFrameElement->addAttribute("svg:y", m_value.str());
        m_value.str("");
        m_value << doubleToString(bitmap.rect.height()) << "in";
        pDrawFrameElement->addAttribute("svg:height", m_value.str());
        m_value.str("");
        m_value << doubleToString(bitmap.rect.width()) << "in";
        pDrawFrameElement->addAttribute("svg:width", m_value.str());
        mBodyElements.push_back(static_cast<GraphicsElement *>(pDrawFrameElement));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new OpenTagGraphicsElement("draw:image")));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new OpenTagGraphicsElement("office:binary-data")));

        libwpg::WPGString base64Binary;
        bitmap.generateBase64DIB(base64Binary);
        mBodyElements.push_back(static_cast<GraphicsElement *>(new CharDataGraphicsElement(base64Binary.cstr())));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("office:binary-data")));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:image")));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:frame")));
}

void OdgExporter::drawImageObject(const libwpg::WPGBinaryData& binaryData)
{
        if (binaryData.mimeType.length() <= 0)
                return;
        OpenTagGraphicsElement *pDrawFrameElement = new OpenTagGraphicsElement("draw:frame");
        m_value.str("");
        m_value << doubleToString(binaryData.rect.x1) << "in";
        pDrawFrameElement->addAttribute("svg:x", m_value.str());
        m_value.str("");
        m_value << doubleToString(binaryData.rect.y1) << "in";
        pDrawFrameElement->addAttribute("svg:y", m_value.str());
        m_value.str("");
        m_value << doubleToString(binaryData.rect.height()) << "in";
        pDrawFrameElement->addAttribute("svg:height", m_value.str());
        m_value.str("");
        m_value << doubleToString(binaryData.rect.width()) << "in";
        pDrawFrameElement->addAttribute("svg:width", m_value.str());
        mBodyElements.push_back(static_cast<GraphicsElement *>(pDrawFrameElement));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new OpenTagGraphicsElement("draw:image")));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new OpenTagGraphicsElement("office:binary-data")));

        libwpg::WPGString base64Binary = binaryData.getBase64Data();
        mBodyElements.push_back(static_cast<GraphicsElement *>(new CharDataGraphicsElement(base64Binary.cstr())));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("office:binary-data")));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:image")));

        mBodyElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:frame")));
}


static std::string colorToHex(const libwpg::WPGColor& color)
{
        char hexdigits[] = "0123456789abcdef";
        char buffer[] = "123456";
        buffer[0] = hexdigits[(color.red >> 4) & 15];
        buffer[1] = hexdigits[color.red & 15];
        buffer[2] = hexdigits[(color.green >> 4) & 15];
        buffer[3] = hexdigits[color.green & 15];
        buffer[4] = hexdigits[(color.blue >> 4) & 15];
        buffer[5] = hexdigits[color.blue & 15];
        return std::string(buffer);
}

void OdgExporter::writeStyle()
{
        m_value.str("");
        m_name.str("");

        if(!m_pen.solid && (m_pen.dashArray.count() >=2 ) )
        {
                // ODG only supports dashes with the same length of spaces inbetween
                // here we take the first space and assume everything else the same
                // note that dash length is written in percentage
                double distance = m_pen.dashArray.at(1);
                OpenTagGraphicsElement *tmpDrawStrokeDashElement = new OpenTagGraphicsElement("draw:stroke-dash");
                tmpDrawStrokeDashElement->addAttribute("draw:style", "rect");
                m_value << "Dash_" << m_dashIndex++;
                tmpDrawStrokeDashElement->addAttribute("draw:name", m_value.str());
                m_value.str("");
                m_value << doubleToString(distance*100) << "%";
                tmpDrawStrokeDashElement->addAttribute("draw:distance", m_value.str());
                m_value.str("");
                for(unsigned i = 0; i < m_pen.dashArray.count()/2; i++)
                {
                        m_name << "draw:dots" << i+1;
                        tmpDrawStrokeDashElement->addAttribute(m_name.str(), "1");
                        m_name.str("");
                        m_name << "draw:dots" << i+1 << "-length";
                        m_value << doubleToString(100*m_pen.dashArray.at(i*2)) << "%";
                        tmpDrawStrokeDashElement->addAttribute(m_name.str(), m_value.str());
                        m_name.str("");
                        m_value.str("");
                }
                mStrokeDashElements.push_back(static_cast<GraphicsElement *>(tmpDrawStrokeDashElement));
                mStrokeDashElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:stroke-dash")));
        }

        if(m_brush.style == libwpg::WPGBrush::Gradient)
        {
                OpenTagGraphicsElement *tmpDrawGradientElement = new OpenTagGraphicsElement("draw:gradient");
                tmpDrawGradientElement->addAttribute("draw:style", "linear");
                m_value << "Gradient_" << m_gradientIndex++;
                tmpDrawGradientElement->addAttribute("draw:name", m_value.str());
                m_value.str("");

                // ODG angle unit is 0.1 degree
                double angle = -m_brush.gradient.angle();
                while(angle < 0)
                        angle += 360;
                while(angle > 360)
                        angle -= 360;

                m_value << (unsigned)(angle*10);
                tmpDrawGradientElement->addAttribute("draw:angle", m_value.str());
                m_value.str("");

                libwpg::WPGColor startColor = m_brush.gradient.stopColor(0);
                libwpg::WPGColor stopColor = m_brush.gradient.stopColor(1);
                m_value << "#" << colorToHex(startColor);
                tmpDrawGradientElement->addAttribute("draw:start-color", m_value.str());
                m_value.str("");
                m_value << "#" << colorToHex(stopColor);
                tmpDrawGradientElement->addAttribute("draw:end-color", m_value.str());
                m_value.str("");
                tmpDrawGradientElement->addAttribute("draw:start-intensity", "100%");
                tmpDrawGradientElement->addAttribute("draw:end-intensity", "100%");
                tmpDrawGradientElement->addAttribute("draw:border", "0%");
                mGradientElements.push_back(static_cast<GraphicsElement *>(tmpDrawGradientElement));
                mGradientElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("draw:gradient")));
        }

        OpenTagGraphicsElement *tmpStyleStyleElement = new OpenTagGraphicsElement("style:style");
        m_value << "gr" << m_styleIndex;
        tmpStyleStyleElement->addAttribute("style:name", m_value.str());
        m_value.str("");
        tmpStyleStyleElement->addAttribute("style:family", "graphic");
        tmpStyleStyleElement->addAttribute("style:parent-style-name", "standard");
        mAutomaticStylesElements.push_back(static_cast<GraphicsElement *>(tmpStyleStyleElement));

        OpenTagGraphicsElement *tmpStyleGraphicPropertiesElement = new OpenTagGraphicsElement("style:graphic-properties");

        if(m_pen.width > 0.0)
        {
                m_value << doubleToString(m_pen.width) << "in";
                tmpStyleGraphicPropertiesElement->addAttribute("svg:stroke-width", m_value.str());
                m_value.str("");
                m_value << "#" << colorToHex(m_pen.foreColor);
                tmpStyleGraphicPropertiesElement->addAttribute("svg:stroke-color", m_value.str());
                m_value.str("");

                if(!m_pen.solid)
                {
                        tmpStyleGraphicPropertiesElement->addAttribute("draw:stroke", "dash");
                        m_value << "Dash_" << m_dashIndex-1;
                        tmpStyleGraphicPropertiesElement->addAttribute("draw:stroke-dash", m_value.str());
                        m_value.str("");
                }
        }
        else
                tmpStyleGraphicPropertiesElement->addAttribute("draw:stroke", "none");

        if(m_brush.style == libwpg::WPGBrush::NoBrush)
                tmpStyleGraphicPropertiesElement->addAttribute("draw:fill", "none");

        if(m_brush.style == libwpg::WPGBrush::Solid)
        {
                tmpStyleGraphicPropertiesElement->addAttribute("draw:fill", "solid");
                m_value << "#" << colorToHex(m_brush.foreColor);
                tmpStyleGraphicPropertiesElement->addAttribute("draw:fill-color", m_value.str());
                m_value.str("");
        }

        if(m_brush.style == libwpg::WPGBrush::Gradient)
        {
                tmpStyleGraphicPropertiesElement->addAttribute("draw:fill", "gradient");
                m_value << "Gradient_" << m_gradientIndex-1;
                tmpStyleGraphicPropertiesElement->addAttribute("draw:fill-gradient-name", m_value.str());
                m_value.str("");
        }

        mAutomaticStylesElements.push_back(static_cast<GraphicsElement *>(tmpStyleGraphicPropertiesElement));
        mAutomaticStylesElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("style:graphic-properties")));

        mAutomaticStylesElements.push_back(static_cast<GraphicsElement *>(new CloseTagGraphicsElement("style:style")));
        m_styleIndex++;
}
