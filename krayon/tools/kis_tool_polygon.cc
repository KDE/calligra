/*
 *  polygontool.cc - part of Krayon
 *
 *  Copyright (c) 2001 Toshitaka Fujioka <fujioka@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <assert.h>
#include <math.h>

#include <qpainter.h>
#include <qpointarray.h>

#include <kaction.h>
#include <kdebug.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_painter.h"
#include "kis_color.h"
#include "kis_canvas.h"
#include "kis_tool_polygon.h"
#include "kis_dlg_toolopts.h"

PolyGonTool::PolyGonTool(KisDoc *doc, KisCanvas *canvas) : KisTool(doc)
{
	m_pDoc = doc;
	m_dragging = false;
	pCanvas = canvas;
	lineThickness = 4;
	lineOpacity = 255;
	cornersValue = 3;
	sharpnessValue = 0;
	usePattern = false;
	useGradient = false;
	useRegions = false;
	checkPolygon = true;
	checkConcavePolygon = false;
	mStart = QPoint(-1, -1);
	mFinish = QPoint(-1, -1);     
}

PolyGonTool::~PolyGonTool()
{
}

void PolyGonTool::mousePress(QMouseEvent *event)
{
	if (m_pDoc -> isEmpty())
		return;

	if (event -> button() == LeftButton) {
		m_dragging = true;
		m_dragStart = event -> pos();
		m_dragEnd = event -> pos();
	}
}

void PolyGonTool::mouseMove(QMouseEvent *event)
{
	if (m_pDoc -> isEmpty())
		return;

	if (m_dragging) {
		// erase old polygon on canvas
		drawPolygon(m_dragStart, m_dragEnd);
		// get current mouse position
		m_dragEnd = event -> pos();
		// draw new polygon on canvas
		drawPolygon(m_dragStart, m_dragEnd);
	}
}

void PolyGonTool::mouseRelease(QMouseEvent *event)
{
	if (m_pDoc -> isEmpty())
		return;

	if (m_dragging && event -> state() == LeftButton) {
		// erase old polygon on canvas
		drawPolygon(m_dragStart, m_dragEnd);
		m_dragging = false;

		// draw final polygon onto layer 
		KisPainter *p = m_pView -> kisPainter();
		QRect rect = getDrawRect(drawPoints);
		QPointArray points = zoomPointArray(drawPoints);
		p -> drawPolygon(points, rect);
	}
}

void PolyGonTool::drawPolygon(const QPoint& start, const QPoint& end)
{
	QPainter p;
	QPen pen;

	pen.setWidth(lineThickness);
	p.begin(pCanvas);
	p.setPen(pen);
	p.setRasterOp(Qt::NotROP);

	float zF = m_pView -> zoomFactor();
	double angle = 2 * M_PI / cornersValue;
	float dx = (float) ::fabs(start.x() - end.x());
	float dy = (float) ::fabs(start.y() - end.y());
	float radius = (dx > dy ? dx / 2.0 : dy / 2.0);
	float xoff = start.x() + (start.x() < end.x() ? radius : -radius) + m_pView -> xPaintOffset() - (int)(zF * m_pView -> xScrollOffset());
	float yoff = start.y() + (start.y() < end.y() ? radius : -radius) + m_pView -> yPaintOffset() - (int)(zF * m_pView -> yScrollOffset());
	float xoff_draw = start.x() + (start.x() < end.x() ? radius : -radius);
	float yoff_draw = start.y() + (start.y() < end.y() ? radius : -radius);
	QPointArray points(checkConcavePolygon ? cornersValue * 2 : cornersValue);
	
	points.setPoint(0, (int) xoff, (int)(-radius + yoff));

	// for draw layer
	QPointArray m_drawPoints(checkConcavePolygon ? cornersValue * 2 : cornersValue);
	
	m_drawPoints.setPoint(0, (int)xoff_draw, (int)(-radius + yoff_draw));

	if (checkConcavePolygon) {
		angle /= 2.0;
		
		double a = angle;
		double r = radius - (sharpnessValue / 100.0 * radius);

		for (int i = 1; i < cornersValue * 2; ++i) {
			double xp, yp;

			if (i % 2) {
				xp =  r * sin(a);
				yp = -r * cos(a);
			}
			else {
				xp = radius * sin(a);
				yp = -radius * cos(a);
			}

			a += angle;
			points.setPoint(i, (int)(xp + xoff), (int)(yp + yoff));
			m_drawPoints.setPoint(i, (int)(xp + xoff_draw), (int)(yp + yoff_draw));
		}
	}
	else {
		double a = angle;

		for (int i = 1; i < cornersValue; ++i) {
			double xp = radius * sin(a);
			double yp = -radius * cos(a);

			a += angle;
			points.setPoint(i, (int)(xp + xoff), (int)(yp + yoff));
			m_drawPoints.setPoint(i, (int)(xp + xoff_draw), (int)(yp + yoff_draw));
		}
	}

	p.drawPolygon(points);
	p.end();
	drawPoints = m_drawPoints;
}

/*
    need options for connecting start and finish points 
    automatically and for winding mode also
*/
void PolyGonTool::optionsDialog()
{
	ToolOptsStruct ts;    
    
	ts.usePattern       = usePattern;
	ts.useGradient      = useGradient;
	ts.lineThickness    = lineThickness;
	ts.lineOpacity      = lineOpacity;
	ts.fillShapes       = useRegions;
	ts.polygonCorners   = cornersValue;
	ts.polygonSharpness = sharpnessValue;
	ts.convexPolygon    = checkPolygon;
	ts.concavePolygon   = checkConcavePolygon;

	bool old_usePattern       = usePattern;
	bool old_useGradient      = useGradient;
	int  old_lineThickness    = lineThickness;
	int  old_lineOpacity      = lineOpacity;
	bool old_useRegions       = useRegions;
	int old_cornersValue      = cornersValue;
	int old_sharpnessValue    = sharpnessValue;
	bool old_checkPolygon      = checkPolygon;
	bool old_checkConcavePolygon = checkConcavePolygon;
    
	ToolOptionsDialog *pOptsDialog = new ToolOptionsDialog(tt_polygontool, ts);

	pOptsDialog -> exec();
    
	if(!pOptsDialog->result() == QDialog::Accepted)
		return;

	lineThickness = pOptsDialog->polygonToolTab()->thickness();
	lineOpacity   = pOptsDialog->polygonToolTab()->opacity();
	cornersValue  = pOptsDialog->polygonToolTab()->corners();
	sharpnessValue = pOptsDialog->polygonToolTab()->sharpness();
	usePattern    = pOptsDialog->polygonToolTab()->usePattern();
	useGradient   = pOptsDialog->polygonToolTab()->useGradient();
	useRegions    = pOptsDialog->polygonToolTab()->solid();
	checkPolygon  = pOptsDialog->polygonToolTab()->convexPolygon();
	checkConcavePolygon = pOptsDialog->polygonToolTab()->concavePolygon();

	// User change value ?
	if (old_usePattern != usePattern || old_useGradient != useGradient 
			|| old_lineOpacity != lineOpacity || old_lineThickness != lineThickness
			|| old_useRegions != useRegions || old_cornersValue != cornersValue
			|| old_sharpnessValue != sharpnessValue || old_checkPolygon != checkPolygon
			|| old_checkConcavePolygon != checkConcavePolygon) {    
		KisPainter *p = m_pView -> kisPainter();

		assert(p);
		p->setLineThickness(lineThickness);
		p->setLineOpacity(lineOpacity);
		p->setPatternFill(usePattern);
		p->setGradientFill(useGradient);
		p->setFilledPolygon(useRegions);

		// set polygon tool settings
		m_pDoc->setModified(true);
	}
}

void PolyGonTool::setupAction(QObject *collection)
{
	KToggleAction *toggle = new KToggleAction(i18n("&Polygon tool"), "polygon", 0, this, SLOT(toolSelect()), collection, "tool_polygon");

	toggle -> setExclusiveGroup("tools");
}

void PolyGonTool::toolSelect()
{
	if (m_pView) {
		KisPainter *gc = m_pView -> kisPainter();

		gc -> setLineThickness(lineThickness);
		gc -> setLineOpacity(opacity);
		gc -> setPatternFill(usePattern);
		gc -> setGradientFill(useGradient);

		m_pView -> activateTool(this);
	}
}

QDomElement PolyGonTool::saveSettings(QDomDocument& doc) const
{
	// polygon tool element
	QDomElement polygonTool = doc.createElement("polygonTool");

	polygonTool.setAttribute("thickness", lineThickness);
	polygonTool.setAttribute("opacity", opacity);
	polygonTool.setAttribute("corners", cornersValue);
	polygonTool.setAttribute("sharpness", sharpnessValue);
	polygonTool.setAttribute("fillInteriorRegions", static_cast<int>(useRegions));
	polygonTool.setAttribute("useCurrentPattern", static_cast<int>(usePattern));
	polygonTool.setAttribute("fillWithGradient", static_cast<int>(useGradient));
	polygonTool.setAttribute("polygon", static_cast<int>(checkPolygon));
	polygonTool.setAttribute("concavePolygon", static_cast<int>(checkConcavePolygon));
	return polygonTool;
}

bool PolyGonTool::loadSettings(QDomElement& elem)
{
	bool rc = elem.tagName() == "polygonTool";

	if (rc) {
		lineThickness = elem.attribute("thickness").toInt();
		opacity = elem.attribute("opacity").toInt();
		cornersValue = elem.attribute("corners").toInt();
		sharpnessValue = elem.attribute("sharpness").toInt();
		useRegions = static_cast<bool>(elem.attribute("fillInteriorRegions").toInt());
		usePattern = static_cast<bool>(elem.attribute("useCurrentPattern").toInt());
		useGradient = static_cast<bool>(elem.attribute("fillWithGradient").toInt());
		checkPolygon = static_cast<bool>(elem.attribute("polygon").toInt());
		checkConcavePolygon = static_cast<bool>(elem.attribute("concavePolygon").toInt());
	}

	return rc;
}

