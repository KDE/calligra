/*
 *  kis_tool_ellipse.cc - part of Krayon
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
#include "kis_tool_ellipse.h"
#include "kis_dlg_toolopts.h"

EllipseTool::EllipseTool(KisDoc *doc, KisCanvas *canvas) : KisTool(doc)
{
	m_pDoc = doc;
	m_dragging = false;
	pCanvas = canvas;

	// initialize ellipse tool settings
	lineThickness = 4;
	opacity = 255;
	usePattern = false;
	useGradient = false;
	fillSolid = false;
}

EllipseTool::~EllipseTool()
{
}

void EllipseTool::mousePress(QMouseEvent *event)
{
	if (event -> button() == LeftButton) {
		m_dragging = true;
		m_dragStart = event -> pos();
		m_dragEnd = event -> pos();
	}
}

void EllipseTool::mouseMove(QMouseEvent *event)
{
	if (m_dragging) {
		// erase old ellipse on canvas
		drawEllipse(m_dragStart, m_dragEnd);
		// get current mouse position
		m_dragEnd = event -> pos();
		// draw new ellipse on canvas
		drawEllipse(m_dragStart, m_dragEnd);
	}
}

void EllipseTool::mouseRelease(QMouseEvent *event)
{
	if (m_dragging && event -> state() == LeftButton) {
		// erase old ellipse on canvas
		drawEllipse(m_dragStart, m_dragEnd);
		m_dragging = false;

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

		if ( m_dragStart.y() > m_dragEnd.y() ) {
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

		// draw final ellipse onto layer    
		KisPainter *p = m_pView -> kisPainter();
		p -> drawEllipse(rect);
	}    
}

void EllipseTool::drawEllipse(const QPoint& start, const QPoint& end)
{
	QPainter p;
	QPen pen;

	pen.setWidth(lineThickness);
	p.begin(pCanvas);
	p.setPen(pen);
	p.setRasterOp(Qt::NotROP);
	float zF = m_pView -> zoomFactor();

	p.drawEllipse( 
			QRect(start.x() + m_pView->xPaintOffset() - (int)(zF * m_pView->xScrollOffset()),
				start.y() + m_pView->yPaintOffset() - (int)(zF * m_pView->yScrollOffset()), 
				end.x() - start.x(), 
				end.y() - start.y()));
	p.end();
}

void EllipseTool::optionsDialog()
{
	ToolOptsStruct ts;    

	ts.usePattern       = usePattern;
	ts.useGradient      = useGradient;
	ts.lineThickness    = lineThickness;
	ts.opacity      = opacity;
	ts.fillShapes       = fillSolid;
	ts.opacity          = opacity;

	bool old_usePattern       = usePattern;
	bool old_useGradient      = useGradient;
	int  old_lineThickness    = lineThickness;
	unsigned int  old_opacity      = opacity;
	bool old_fillSolid        = fillSolid;

	ToolOptionsDialog OptsDialog(tt_linetool, ts);

	OptsDialog.exec();
    
	if (OptsDialog.result() == QDialog::Rejected)
		return;
        
	lineThickness = OptsDialog.lineToolTab()->thickness();
        opacity   = OptsDialog.lineToolTab()->opacity();
        usePattern    = OptsDialog.lineToolTab()->usePattern();
        fillSolid     = OptsDialog.lineToolTab()->solid(); 
        useGradient   = OptsDialog.lineToolTab()->useGradient();

	// User change value ?
	if ( old_usePattern != usePattern || old_useGradient != useGradient 
			|| old_opacity != opacity || old_lineThickness != lineThickness
			|| old_fillSolid != fillSolid ) {    
		KisPainter *p = m_pView->kisPainter();

		p->setLineThickness( lineThickness );
		p->setLineOpacity( opacity );
		p->setFilledEllipse( fillSolid );
		p->setPatternFill( usePattern );
		p->setGradientFill( useGradient );

		// set ellipse tool settings
		m_pDoc->setModified( true );
	}
}

void EllipseTool::setupAction(QObject *collection)
{
	KToggleAction *toggle = new KToggleAction(i18n("&Ellipse tool"), "ellipse", 0, this, SLOT(toolSelect()), collection, "tool_ellipse");

	toggle -> setExclusiveGroup("tools");
}

void EllipseTool::toolSelect()
{
	if (m_pView) {
		KisPainter *gc = m_pView -> kisPainter();

		gc -> setLineThickness(lineThickness);
	        gc -> setLineOpacity(opacity);
		gc -> setFilledEllipse(usePattern);
		gc -> setGradientFill(useGradient);
		gc -> setPatternFill(fillSolid);

		m_pView -> activateTool(this);
	}
}

QDomElement EllipseTool::saveSettings(QDomDocument& doc) const
{
	// ellipse tool element
	QDomElement ellipseTool = doc.createElement("ellipseTool");

	ellipseTool.setAttribute("thickness", lineThickness);
	ellipseTool.setAttribute("opacity", opacity);
	ellipseTool.setAttribute("fillInteriorRegions", static_cast<int>(fillSolid));
	ellipseTool.setAttribute("useCurrentPattern", static_cast<int>(usePattern));
	ellipseTool.setAttribute("fillWithGradient", static_cast<int>(useGradient));
	return ellipseTool;
}

bool EllipseTool::loadSettings(QDomElement& elem)
{
        bool rc = elem.tagName() == "ellipseTool";

	if (rc) {
		lineThickness = elem.attribute("thickness").toInt();
		opacity = elem.attribute("opacity").toInt();
		fillSolid = static_cast<bool>(elem.attribute("fillInteriorRegions").toInt());
		usePattern = static_cast<bool>(elem.attribute("useCurrentPattern").toInt());
		useGradient = static_cast<bool>(elem.attribute("fillWithGradient").toInt());
	}

	return rc;
}

