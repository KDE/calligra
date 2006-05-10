/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef __KARBON_PART_IFACE_H__
#define __KARBON_PART_IFACE_H__


#include <QString>

#include <dcopref.h>
#include <KoDocumentIface.h>


class KarbonPart;


class KarbonPartIface : virtual public KoDocumentIface
{
	K_DCOP

public:
	KarbonPartIface( KarbonPart* part );

k_dcop:
	void selectAllObjects();
	void deselectAllObjects();

	bool showStatusBar() const;
	void setShowStatusBar( bool b );
	void setUndoRedoLimit( int undo );
	void initConfig();
	void clearHistory();
	QString unitName() const;
	QString widthInUnits();
	QString heightInUnits();
	double width();
	double height();

	int nbLayer() const;
	DCOPRef activeLayer();

private:
	KarbonPart* m_part;
};

#endif

