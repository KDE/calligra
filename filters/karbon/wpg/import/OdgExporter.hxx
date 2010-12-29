/* libwpg
 * Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
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

#ifndef __ODGEXPORTER_H__
#define __ODGEXPORTER_H__

#include <iostream>
#include <sstream>
#include <string>

#include <libwpg/libwpg.h>
#include "GraphicsElement.hxx"

class OdgExporter : public libwpg::WPGPaintInterface {
public:
	explicit OdgExporter(GraphicsHandler *pHandler,
			     const bool isFlatXML = false);
	~OdgExporter();

#if LIBWPG_VERSION_MINOR<2
	void startGraphics(double imageWidth, double imageHeight);
	void endGraphics();
	void startLayer(unsigned int id);
	void endLayer(unsigned int id);

	void setPen(const libwpg::WPGPen& pen);
	void setBrush(const libwpg::WPGBrush& brush);
	void setFillRule(FillRule rule);

	void drawRectangle(const libwpg::WPGRect& rect, double rx, double ry);
	void drawEllipse(const libwpg::WPGPoint& center, double rx, double ry);
	void drawPolygon(const libwpg::WPGPointArray& vertices);
	void drawPath(const libwpg::WPGPath& path);
	void drawBitmap(const libwpg::WPGBitmap& bitmap);
	void drawImageObject(const libwpg::WPGBinaryData& binaryData);

private:
	std::vector <GraphicsElement *> mBodyElements;
	std::vector <GraphicsElement *> mAutomaticStylesElements;
	std::vector <GraphicsElement *> mStrokeDashElements;
	std::vector <GraphicsElement *> mGradientElements;
	GraphicsHandler *mpHandler;

	libwpg::WPGPen m_pen;
	libwpg::WPGBrush m_brush;
	FillRule m_fillRule;
	int m_gradientIndex;
	int m_dashIndex;
	int m_styleIndex;
	void writeStyle();
	std::ostringstream m_value, m_name;
	double m_width, m_height;
	const bool m_isFlatXML;
#else
	virtual void startGraphics(const ::WPXPropertyList &propList);
	virtual void endGraphics();
	virtual void setStyle(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &gradient);
	virtual void startLayer(const ::WPXPropertyList &propList);
	virtual void endLayer();
	virtual void drawRectangle(const ::WPXPropertyList& propList);
	virtual void drawEllipse(const ::WPXPropertyList& propList);
	virtual void drawPolygon(const ::WPXPropertyListVector &vertices);
	virtual void drawPath(const ::WPXPropertyListVector &path);
	virtual void drawGraphicObject(const ::WPXPropertyList &propList, const ::WPXBinaryData &binaryData);
	virtual void startEmbeddedGraphics(const ::WPXPropertyList &propList);
	virtual void endEmbeddedGraphics();
	virtual void drawPolyline(const ::WPXPropertyListVector &vertices);
	virtual void startTextObject(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path);
	virtual void endTextObject();
	virtual void startTextLine(const ::WPXPropertyList &propList);
	virtual void endTextLine();
	virtual void startTextSpan(const ::WPXPropertyList &propList);
	virtual void endTextSpan();
	virtual void insertText(const ::WPXString &str);


private:
	std::vector <GraphicsElement *> mBodyElements;
	std::vector <GraphicsElement *> mAutomaticStylesElements;
	std::vector <GraphicsElement *> mStrokeDashElements;
	std::vector <GraphicsElement *> mGradientElements;
	GraphicsHandler *mpHandler;

	int m_gradientIndex;
	int m_dashIndex;
	int m_styleIndex;
	void writeStyle();
	std::ostringstream m_value, m_name;
	double m_width, m_height;
	const bool m_isFlatXML;
#endif
};

#endif // __ODGEXPORTER_H__
