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

#ifndef __KARBON_VIEW_IFACE_H__
#define __KARBON_VIEW_IFACE_H__


#include <qstring.h>

#include <KoViewIface.h>

class KarbonView;


class KarbonViewIface : public KoViewIface
{
	K_DCOP

public:
	KarbonViewIface( KarbonView* view );

k_dcop:
	void editCut();
	void editCopy();
	void editPaste();
	void editSelectAll();
	void editDeselectAll();
	void editDeleteSelection();
	void editPurgeHistory();

	void objectMoveToTop();
	void objectMoveToBottom();
	void objectMoveUp();
	void objectMoveDown();

	double zoomFactor() const;

	void groupSelection();
	void ungroupSelection();
	void configure();
	void setLineWidth( double val );

	void insertKnots();
	void pathFlatten();
	void pathRoundCorners();
	void pathWhirlPinch();

	void repaint();

private:
	KarbonView* m_view;
};

#endif

