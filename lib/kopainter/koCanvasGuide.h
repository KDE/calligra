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
#if !defined KO_CANVAS_GUIDE_H_
#define KO_CANVAS_GUIDE_H_

#include <qdom.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qvaluelist.h>
#include <qwidget.h>
#include <ksharedptr.h>

struct KoCanvasGuide : public KShared {
public:
	KoCanvasGuide(Qt::Orientation o);
	virtual ~KoCanvasGuide();

	Qt::Orientation orientation() { return orient; }
	double position(){ return pos; }
	bool isSelected() { return selected; }

public:
	double pos;
	QPixmap buffer;
	bool hasBuffer;
	bool selected;
	Qt::Orientation orient;
};

typedef KSharedPtr<KoCanvasGuide> KoCanvasGuideSP;

class KoCanvasGuideMgr {
	typedef QValueList<KoCanvasGuideSP> vKoCanvasGuideSP;
	typedef vKoCanvasGuideSP::iterator vKoCanvasGuideSP_it;
	typedef vKoCanvasGuideSP::const_iterator vKoCanvasGuideSP_cit;

public:
	KoCanvasGuideMgr();
	~KoCanvasGuideMgr();

public:
	KoCanvasGuideSP add(double pos, Qt::Orientation o);
	void remove(KoCanvasGuideSP gd);
	KoCanvasGuideSP find(double x, double y, double d);
	KoCanvasGuideSP findHorizontal(double y, double d);
	KoCanvasGuideSP findVertical(double x, double d);
	bool hasSelected() const;
	Q_INT32 selectedCount() const;

	void save(QDomElement& element);
	void load(const QDomElement& element);

	void select(KoCanvasGuideSP gd);
	void unselect(KoCanvasGuideSP gd);
	void selectAll();
	void unselectAll();
	void removeSelected();

	void resize(const QSize& size);
	void resize();
	void erase(QPaintDevice *device, QWidget *w, Q_INT32 xOffset, Q_INT32 yOffset, double zoom);
	void paint(QPaintDevice *device, QWidget *w, Q_INT32 xOffset, Q_INT32 yOffset, double zoom);
	void moveSelectedByX(double d);
	void moveSelectedByY(double d);

private:
	void resizeLinesPixmap(const QSize& size, QPixmap *vLine, QPixmap *hLine, QPixmap *linePattern);

private:
	QSize m_size;
	QPixmap m_vGuideLines;
	QPixmap m_hGuideLines;
	QPixmap m_pattern;
	QPixmap m_vGuideLinesSelected;
	QPixmap m_hGuideLinesSelected;
	QPixmap m_patternSelected;
	vKoCanvasGuideSP m_lines;
	vKoCanvasGuideSP m_slines;

private:
	static const char *s_xbm[];
	static const char *s_xbm_selected[];
};

#endif // KIVIO_GUIDELINES_DATA_H

