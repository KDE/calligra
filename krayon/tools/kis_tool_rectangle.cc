/*
 *  kis_tool_rectangle.cc - part of Krayon
 *
 *  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
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

#include <qpainter.h>

#include <kaction.h>
#include <kdebug.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_painter.h"
#include "kis_color.h"
#include "kis_canvas.h"
#include "kis_tool_rectangle.h"
#include "kis_dlg_toolopts.h"

RectangleTool::RectangleTool(KisDoc *doc, KisCanvas *canvas) : KisTool(doc)
{
	m_pDoc = doc;
	m_dragging = false;
	pCanvas = canvas;

	// initialize rectangle tool settings
	lineThickness = 4;
	lineOpacity = 255;
	usePattern = false;
	useGradient = false;
	fillSolid = false;
}

RectangleTool::~RectangleTool()
{
}

void RectangleTool::mousePress(QMouseEvent *event)
{
	if (m_pDoc -> isEmpty())
		return;

	if (event -> button() == LeftButton) {
		m_dragging = true;
		m_dragStart = event -> pos();
		m_dragEnd = event -> pos();
	}
}

void RectangleTool::mouseMove(QMouseEvent *event)
{
	if (m_pDoc -> isEmpty())
		return;

	if (m_dragging) {
		// erase old lines on canvas
		drawRectangle(m_dragStart, m_dragEnd);
		// get current mouse position
		m_dragEnd = event -> pos();
		// draw new lines on canvas
		drawRectangle(m_dragStart, m_dragEnd);
	}
}

void RectangleTool::mouseRelease(QMouseEvent *event)
{
	if (m_pDoc -> isEmpty())
		return;

	if (m_dragging && event -> state() == LeftButton) {
		// erase old lines on canvas
		drawRectangle(m_dragStart, m_dragEnd);
		m_dragging = false;
	}
    
	// get topLeft and bottomRight.
	int maxX = 0, maxY = 0;
	int minX = 0, minY = 0;

	if (m_dragStart.x() > m_dragEnd.x()) {
		maxX = m_dragStart.x();
		minX = m_dragEnd.x();
	}
	else {
		maxX = m_dragEnd.x();
		minX = m_dragStart.x();
	}

	if (m_dragStart.y() > m_dragEnd.y()) {
		maxY = m_dragStart.y();
		minY = m_dragEnd.y();
	}
	else {
		maxY = m_dragEnd.y();
		minY = m_dragStart.y();
	}

	QPoint topLeft = QPoint(minX, minY);
	QPoint bottomRight = QPoint(maxX, maxY);
	QRect rect = QRect(zoomed(topLeft), zoomed(bottomRight));

	// draw final lines onto layer
	KisPainter *p = m_pView -> kisPainter();
	p -> drawRectangle( rect );
}

void RectangleTool::drawRectangle(const QPoint& start, const QPoint& end )
{
    QPainter p;
    QPen pen;
    pen.setWidth(lineThickness);
    
    p.begin( pCanvas );
    p.setPen(pen);
    p.setRasterOp( Qt::NotROP );
    float zF = m_pView->zoomFactor();
    p.drawRect( QRect(start.x() + m_pView->xPaintOffset() 
                                - (int)(zF * m_pView->xScrollOffset()),
                      start.y() + m_pView->yPaintOffset() 
                                - (int)(zF * m_pView->yScrollOffset()), 
                      end.x() - start.x(), 
                      end.y() - start.y()) );
    p.end();
}

void RectangleTool::optionsDialog()
{
	ToolOptsStruct ts;    

	ts.usePattern = usePattern;
	ts.useGradient      = useGradient;
	ts.lineThickness    = lineThickness;
	ts.lineOpacity      = lineOpacity;
	ts.opacity          = lineOpacity;
	ts.fillShapes       = fillSolid;

	bool old_usePattern       = usePattern;
	bool old_useGradient      = useGradient;
	int  old_lineThickness    = lineThickness;
	int  old_lineOpacity      = lineOpacity;
	bool old_fillSolid        = fillSolid;

	ToolOptionsDialog OptsDialog(tt_linetool, ts);

	OptsDialog.exec();
    
	if (OptsDialog.result() != QDialog::Accepted)
		return;

	lineThickness = OptsDialog.lineToolTab()->thickness();
	lineOpacity   = OptsDialog.lineToolTab()->opacity();
	usePattern    = OptsDialog.lineToolTab()->usePattern();
	useGradient   = OptsDialog.lineToolTab()->useGradient();
	fillSolid     = OptsDialog.lineToolTab()->solid();  

	// User change value ?
	if ( old_usePattern != usePattern || old_useGradient != useGradient 
			|| old_lineOpacity != lineOpacity || old_lineThickness != lineThickness
			|| old_fillSolid != fillSolid) {    
		KisPainter *p = m_pView->kisPainter();

		p->setLineThickness(lineThickness);
		p->setLineOpacity(lineOpacity);
		p->setFilledRectangle(fillSolid);
		p->setPatternFill(usePattern);
		p->setGradientFill(useGradient);

		// set rectangle tool settings
		m_pDoc->setModified( true );
	}
}

void RectangleTool::setupAction(QObject *collection)
{
	KToggleAction *toggle = new KToggleAction(i18n("&Rectangle tool"), "rectangle", 0, this, SLOT(toolSelect()), collection, "tool_rectangle");

	toggle -> setExclusiveGroup("tools");
}

void RectangleTool::toolSelect()
{
	if (m_pView) {
		KisPainter *gc = m_pView -> kisPainter();

		gc -> setLineThickness(lineThickness);
		gc -> setLineOpacity(opacity);
		gc -> setFilledRectangle(fillSolid);
		gc -> setGradientFill(useGradient);
		gc -> setPatternFill(usePattern);

		m_pView -> activateTool(this);
	}
}

QDomElement RectangleTool::saveSettings(QDomDocument& doc) const
{
	// rectangle tool element
	QDomElement rectangleTool = doc.createElement("rectangleTool");

	rectangleTool.setAttribute("thickness", lineThickness);
	rectangleTool.setAttribute("opacity", opacity);
	rectangleTool.setAttribute("fillInteriorRegions", static_cast<int>(fillSolid));
	rectangleTool.setAttribute("useCurrentPattern", static_cast<int>(usePattern));
	rectangleTool.setAttribute("fillWithGradient", static_cast<int>(useGradient));
	return rectangleTool;

}

bool RectangleTool::loadSettings(QDomElement& elem)
{
	bool rc = elem.tagName() == "rectangleTool";

	if (rc) {
		lineThickness = elem.attribute("thickness").toInt();
		opacity = elem.attribute("opacity").toInt();
		fillSolid = static_cast<bool>(elem.attribute("fillInteriorRegions").toInt());
		usePattern = static_cast<bool>(elem.attribute("useCurrentPattern").toInt());
		useGradient = static_cast<bool>(elem.attribute("fillWithGradient").toInt());
	}

	return rc;
}


