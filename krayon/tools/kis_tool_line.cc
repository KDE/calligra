/*
 *  linetool.cc - part of Krayon
 *
 *  Copyright (c) 2000 John Califf 
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
#include "kis_tool_line.h"
#include "kis_dlg_toolopts.h"

LineTool::LineTool(KisDoc *doc, KisCanvas *canvas) : KisTool(doc)
{
	m_pDoc = doc;
	m_dragging = false;
	pCanvas =  canvas;

	// initialize line tool settings
	lineThickness = 4;
	opacity = 255;
	usePattern = false;
	useGradient = false;
	useRegions = false;
}

LineTool::~LineTool()
{
}

void LineTool::mousePress(QMouseEvent *event)
{
	if (event -> button() == LeftButton) {
		m_dragging = true;
		m_dragStart = event->pos();
		m_dragEnd = event->pos();
	}
}

void LineTool::mouseMove(QMouseEvent *event)
{
	if (m_dragging) {
		// erase old line
		drawLine(m_dragStart, m_dragEnd);
		// get current position
		m_dragEnd = event -> pos();
		// draw line to current position
		drawLine(m_dragStart, m_dragEnd);
	}
}

void LineTool::mouseRelease(QMouseEvent *event)
{
	if (m_dragging && event -> state() == LeftButton) {
		// erase old line
		drawLine(m_dragStart, m_dragEnd);
		m_dragging = false;
	}
    
	KisPainter *p = m_pView -> kisPainter();
	
	p -> drawLine(zoomed(m_dragStart.x()), zoomed(m_dragStart.y()), zoomed(m_dragEnd.x()),   zoomed(m_dragEnd.y()));
}

void LineTool::drawLine(const QPoint& start, const QPoint& end)
{
	QPainter p;
	QPen pen;

	pen.setWidth(lineThickness);

	p.begin(pCanvas);
	p.setPen(pen);    
	p.setRasterOp(Qt::NotROP);
	float zF = m_pView -> zoomFactor();

	p.drawLine(
			QPoint(start.x() + m_pView->xPaintOffset() - (int)(zF * m_pView->xScrollOffset()),
				start.y() + m_pView->yPaintOffset() - (int)(zF * m_pView->yScrollOffset())), 
			QPoint( end.x() + m_pView->xPaintOffset() - (int)(zF * m_pView->xScrollOffset()),
				end.y() + m_pView->yPaintOffset() - (int)(zF * m_pView->yScrollOffset())));

	p.end();
}

void LineTool::optionsDialog()
{
	ToolOptsStruct ts;    
    
	ts.usePattern = usePattern;
	ts.useGradient = useGradient;
    ts.lineThickness  = lineThickness;
    ts.opacity    = opacity;
    ts.fillShapes     = useRegions;

    bool old_usePattern       = usePattern;
    bool old_useGradient      = useGradient;
    int  old_lineThickness    = lineThickness;
    int  old_opacity      = opacity;
    bool old_useRegions       = useRegions;
    
    ToolOptionsDialog OptsDialog(tt_linetool, ts);

    OptsDialog.exec();
    
    if(OptsDialog.result() != QDialog::Accepted)
        return;

    lineThickness = OptsDialog.lineToolTab()->thickness();
    opacity   = OptsDialog.lineToolTab()->opacity();
    usePattern    = OptsDialog.lineToolTab()->usePattern();
    useGradient   = OptsDialog.lineToolTab()->useGradient();
    useRegions    = OptsDialog.lineToolTab()->solid();

    // User change value ?
    if ( old_usePattern != usePattern || old_useGradient != useGradient 
		    || old_opacity != opacity || old_lineThickness != lineThickness
		    || old_useRegions != useRegions ) {    
	    KisPainter *p = m_pView->kisPainter();

	    p->setLineThickness( lineThickness );
	    p->setLineOpacity( opacity );
	    p->setPatternFill( usePattern );
	    p->setGradientFill( useGradient );

	    // set line tool settings
	    m_pDoc->setModified( true );
    }
}

void LineTool::setupAction(QObject *collection)
{
	KToggleAction *toggle = new KToggleAction(i18n("&Line tool"), "line", 0, this, SLOT(toolSelect()), collection, "tool_line");

	toggle -> setExclusiveGroup("tools");
}

void LineTool::toolSelect()
{
	if (m_pView) {
		KisPainter *gc = m_pView -> kisPainter();

		kdDebug() << "opacity = " << opacity << endl;

		gc -> setLineThickness(lineThickness);
		gc -> setLineOpacity(opacity);
		gc -> setPatternFill(usePattern);
		gc -> setGradientFill(useGradient);
		m_pView -> activateTool(this);
	}
}

QDomElement LineTool::saveSettings(QDomDocument& doc) const
{
	QDomElement lineTool = doc.createElement("lineTool");

	lineTool.setAttribute("thickness", lineThickness);
	lineTool.setAttribute("opacity", opacity);
	lineTool.setAttribute("fillInteriorRegions", static_cast<int>(useRegions));
	lineTool.setAttribute("useCurrentPattern", static_cast<int>(usePattern));
	lineTool.setAttribute("fillWithGradient", static_cast<int>(useGradient));
	return lineTool;
}

bool LineTool::loadSettings(QDomElement& elem)
{
	bool rc = elem.tagName() == "lineTool";

	if (rc) {
		lineThickness = elem.attribute("thickness").toInt();
		opacity = elem.attribute("opacity").toInt();
		useRegions = static_cast<bool>(elem.attribute("fillInteriorRegions").toInt());
		usePattern = static_cast<bool>(elem.attribute("useCurrentPattern").toInt());
		useGradient = static_cast<bool>(elem.attribute("fillWithGradient").toInt());
	}

	return rc;
}

