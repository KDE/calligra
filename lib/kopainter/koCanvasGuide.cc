/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 * Copyright (C) 2002 Patrick Julien <freak@codepimps.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <koCanvasGuide.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qwmatrix.h>

const char *KoCanvasGuideMgr::s_xbm[] = {
	"15 1 2 1",
	"  c None",
	"# c blue",
	"## ## ## ## ## "};

const char *KoCanvasGuideMgr::s_xbm_selected[] = {
	"15 1 2 1",
	"  c None",
	"# c red",
	"## ## ## ## ## "};

KoCanvasGuide::KoCanvasGuide(Qt::Orientation o) 
{
	hasBuffer = false;
	pos = 0.0;
	orient = o;
	selected = false;
}

KoCanvasGuide::~KoCanvasGuide()
{
}

KoCanvasGuideMgr::KoCanvasGuideMgr() : m_size(1, 1), m_pattern(s_xbm), m_patternSelected(s_xbm_selected)
{
}

KoCanvasGuideMgr::~KoCanvasGuideMgr()
{
	unselectAll();
}

void KoCanvasGuideMgr::resize(const QSize& size)
{
	resizeLinesPixmap(size, &m_vGuideLines, &m_hGuideLines, &m_pattern);
	resizeLinesPixmap(size, &m_vGuideLinesSelected, &m_hGuideLinesSelected, &m_patternSelected);
	m_size = size;
}

void KoCanvasGuideMgr::resizeLinesPixmap(const QSize& size, QPixmap *vLine, QPixmap *hLine, QPixmap *linePattern)
{
	Q_INT32 d = linePattern -> width();
	QPainter p;
	const QBitmap *mask;
	QBitmap *hmask;
	QBitmap *vmask;
	QWMatrix m;
	QPixmap rpattern;

	hLine -> resize(size.width() + d, 1);
	vLine -> resize(1, size.height() + d);

	p.begin(hLine);
	p.drawTiledPixmap(0, 0, hLine -> width(), 1, *linePattern);
	p.end();

	mask = linePattern -> mask();
	hmask = new QBitmap(hLine -> size());
	p.begin(hmask);
	p.drawTiledPixmap(0, 0, hmask -> width(), 1, *mask);
	p.end();
	hLine -> setMask(*hmask);
	delete hmask;

	m.rotate(90);
	rpattern = linePattern -> xForm(m);

	p.begin(vLine);
	p.drawTiledPixmap(0, 0, 1, vLine -> height(), rpattern);
	p.end();

	mask = rpattern.mask();
	vmask = new QBitmap(vLine -> size());
	p.begin(vmask);
	p.drawTiledPixmap(0, 0, 1, vmask -> height(), *mask);
	p.end();
	vLine -> setMask(*vmask);
	delete vmask;
}

void KoCanvasGuideMgr::resize()
{
	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> orientation() == Qt::Vertical)
			g -> buffer.resize(1, m_size.height());
		else
			g -> buffer.resize(m_size.width(), 1);

		g -> hasBuffer = false;
	}
}

void KoCanvasGuideMgr::erase(QPaintDevice *device, QWidget *w, Q_INT32 xOffset, Q_INT32 yOffset, double zoom)
{
	Q_INT32 x;
	Q_INT32 y;

	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> hasBuffer) {
			if (g -> orientation() == Qt::Vertical) {
				x = static_cast<Q_INT32>(g -> position() * zoom);

				if (x >= 0 && x < w -> width())
					bitBlt(device, x - xOffset, 0, &g -> buffer);
			} else {
				y = static_cast<Q_INT32>(g -> position() * zoom);

				if (y >= 0 && y < w -> height())
					bitBlt(device, 0, y - yOffset, &g -> buffer);
			}

		}

		g -> hasBuffer = false;
	}
}

void KoCanvasGuideMgr::paint(QPaintDevice *device, QWidget *w, Q_INT32 xOffset, Q_INT32 yOffset, double zoom)
{
	Q_INT32 d = m_pattern.width();
	Q_INT32 dx = xOffset % d;
	Q_INT32 dy = yOffset % d;
	Q_INT32 x;
	Q_INT32 y;

	if (xOffset < 0)
		dx += d;

	if (yOffset < 0)
		dy += d;

	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> orientation() == Qt::Vertical) {
			x = static_cast<Q_INT32>(g -> position() * zoom);
			
			if ((g -> hasBuffer = x >= 0 && x < w -> width())) {
				bitBlt(&g -> buffer, 0, 0, device, x - xOffset, 0, 1, g -> buffer.height());
				bitBlt(device, x - xOffset, 0, g -> isSelected() ? &m_vGuideLinesSelected : &m_vGuideLines, 0, dy, 1, w -> height());
			}
		} else {
			y = static_cast<Q_INT32>(g -> position() * zoom);

			if ((g -> hasBuffer = y >= 0 && y < w -> height())) {
				bitBlt(&g -> buffer, 0, 0, device, 0, y - yOffset, g -> buffer.width(), 1);
				bitBlt(device, 0, y - yOffset, g -> isSelected() ? &m_hGuideLinesSelected : &m_hGuideLines, dx, 0, w -> width(), 1);
			}
		}
	}
}

KoCanvasGuideSP KoCanvasGuideMgr::add(double pos, Qt::Orientation o)
{
	KoCanvasGuideSP gd = new KoCanvasGuide(o);

	gd -> pos = pos;
	m_lines.push_back(gd);

	if (o == Qt::Vertical)
		gd -> buffer.resize(1, m_size.height());
	else
		gd -> buffer.resize(m_size.width(), 1);

	return gd;
}

void KoCanvasGuideMgr::remove(KoCanvasGuideSP gd)
{
	vKoCanvasGuideSP_it it = m_lines.find(gd);

	if (it != m_lines.end())
		m_lines.erase(it);
}

KoCanvasGuideSP KoCanvasGuideMgr::find(double x, double y, double d)
{
	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> orientation() == Qt::Horizontal && QABS(g -> position() - y) < d)
			return g;

		if (g -> orientation() == Qt::Vertical && QABS(g -> position() - x) < d)
			return g;
	}

	return 0;
}

KoCanvasGuideSP KoCanvasGuideMgr::findHorizontal(double y, double d)
{
	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> orientation() == Qt::Horizontal && QABS(g -> position() - y) < d)
			return g;
	}

	return 0;
}

KoCanvasGuideSP KoCanvasGuideMgr::findVertical(double x, double d)
{
	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> orientation() == Qt::Vertical && QABS(g -> position() - x) < d)
			return g;
	}

	return 0;
}

void KoCanvasGuideMgr::select(KoCanvasGuideSP gd)
{
	if (gd && !gd -> selected) {
		gd -> selected = true;
		m_slines.push_back(gd);
	}
}

void KoCanvasGuideMgr::unselect(KoCanvasGuideSP gd)
{
	if (gd && gd -> selected) {
		vKoCanvasGuideSP_it it = m_slines.find(gd);

		gd -> selected = false;

		if (it != m_slines.end())
			m_slines.erase(it);
	}
}

void KoCanvasGuideMgr::unselectAll()
{
	while (!m_slines.empty())
		unselect(*m_slines.begin());
}

void KoCanvasGuideMgr::selectAll()
{
	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++)
		select(*it);
}

bool KoCanvasGuideMgr::hasSelected() const
{
	return !m_slines.empty();
}

void KoCanvasGuideMgr::moveSelectedByX(double d)
{
	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> isSelected() && g -> orientation() == Qt::Vertical)
			g -> pos = g -> pos + d;
	}
}

void KoCanvasGuideMgr::moveSelectedByY(double d)
{
	for (vKoCanvasGuideSP_it it = m_lines.begin(); it != m_lines.end(); it++) {
		KoCanvasGuideSP g = *it;

		if (g -> isSelected() && g -> orientation() == Qt::Horizontal)
			g -> pos = g -> pos + d;
	}
}

Q_INT32 KoCanvasGuideMgr::selectedCount() const
{
	return m_slines.size();
}

void KoCanvasGuideMgr::removeSelected()
{
	vKoCanvasGuideSP_it rm_it;

	for (vKoCanvasGuideSP_it it = m_slines.begin(); it != m_slines.end(); it++) {
		rm_it = m_lines.find(*it);

		if (rm_it != m_lines.end())
			m_lines.erase(rm_it);
	}

	m_slines.clear();
}

void KoCanvasGuideMgr::save(QDomElement& element)
{
	for (vKoCanvasGuideSP_it it = m_slines.begin(); it != m_slines.end(); it++) {
		KoCanvasGuideSP g = *it;
		QDomElement e = element.ownerDocument().createElement("Guideline");

		element.appendChild(e);
		e.setAttribute("pos", g -> pos);
		e.setAttribute("orient", static_cast<Q_INT32>(g -> orient));
	}
}

void KoCanvasGuideMgr::load(const QDomElement& element)
{
	QString attr;

	m_slines.clear();
	m_lines.clear();

	for (QDomElement e = element.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement()) {
		double pos;
		Qt::Orientation orient;

		if ((attr = e.attribute("pos")) == QString::null)
			return;

		pos = attr.toDouble();

		if ((attr = e.attribute("orient")) == QString::null)
			return;

		orient = static_cast<Qt::Orientation>(attr.toInt());
		add(pos, orient);
	}
}

