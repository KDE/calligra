/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/* This file is included by KexiDBLabel and KexiFrame */

//! @todo add more frame types
void ClassName::drawFrame( QPainter *p )
{
	if (frameShape() == QFrame::Box) {
		if ( frameShadow() == Plain )
			qDrawPlainRect( p, frameRect(), d->frameColor, lineWidth() );
		else
			qDrawShadeRect( p, frameRect(), colorGroup(), frameShadow() == QFrame::Sunken, 
				lineWidth(), midLineWidth() );
	}
	else {
		SuperClassName::drawFrame(p);
	}
}

void ClassName::setPalette( const QPalette &pal )
{
	QPalette pal2(pal);
	QColorGroup cg( pal2.active() );
	cg.setColor(QColorGroup::Light, KexiUtils::bleachedColor( d->frameColor, 150 ));
	cg.setColor(QColorGroup::Mid, d->frameColor);
	cg.setColor(QColorGroup::Dark, d->frameColor.dark(150));
	pal2.setActive(cg);
	QColorGroup cg2( pal2.inactive() );
	cg2.setColor(QColorGroup::Light, cg.light() );
	cg2.setColor(QColorGroup::Mid, cg.mid());
	cg2.setColor(QColorGroup::Dark, cg.dark());
	pal2.setInactive(cg2);
	SuperClassName::setPalette(pal2);
}

const QColor& ClassName::frameColor() const
{
	return d->frameColor;
}

void ClassName::setFrameColor(const QColor& color)
{
	d->frameColor = color;
	//update light and dark colors
	setPalette( palette() );
}
