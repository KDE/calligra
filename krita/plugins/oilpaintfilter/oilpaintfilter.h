/* 
 * oilpaintfilter.h -- Part of Krita
 *
 * Copyright (c) 2004 Michael Thaler (michael.thaler@physik.tu-muenchen.de)
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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef OILPAINTFILTER_H
#define OILPAINTFILTER_H

#include <kparts/plugin.h>

class KisView;

class OilPaintFilter : public KParts::Plugin
{
	Q_OBJECT
public:
	OilPaintFilter(QObject *parent, const char *name, const QStringList &);
	virtual ~OilPaintFilter();
	
private slots:

	void slotOilPaintFilter();

private:

	KisView * m_view;
	KisPainter * m_painter;

};

#endif // OILPAINTFILTER_H
