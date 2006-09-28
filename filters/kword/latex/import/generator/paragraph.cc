/* This file is part of the KDE project
 * Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "paragraph.h"
#include <element.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3PtrList>

bool Paragraph::analyze(Q3PtrList<Element>* body)
{
}


bool Paragraph::generate(QDomElement& paragraphNode, QDomDocument& doc)
{
	/* FRAMESET */
	QDomElement textNode = doc.createElement("TEXT");
	
	/* required attributes */
	/*framesetNode.setAttribute("frameType", TEXTFRAME);
	framesetNode.setAttribute("frameInfo", NORMALTEXT);*/

	paragraphNode.appendChild(textNode);

	/* FRAME */
	//QDomElement frameNode = doc.createElement("FRAME");
	
	/* required attributes */
	/*frameNode.setAttribute("newFrameBehavior", "0");
	frameNode.setAttribute("left", _leftMargin);
	frameNode.setAttribute("right", _rightMargin);
	frameNode.setAttribute("top", _topMargin);
	frameNode.setAttribute("bottom", _bottomMargin);
	framesetNode.appendChild(frameNode);*/
	
}

