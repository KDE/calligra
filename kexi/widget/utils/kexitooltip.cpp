/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexitooltip.h"

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qtooltip.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qtimer.h>

#warning KexiToolTip ported to Qt4 but not tested

KexiToolTip::KexiToolTip(const QVariant& value, QWidget* parent)
 : QWidget(parent)
 , m_value(value)
{
	setWindowFlags( Qt::WStyle_Customize | Qt::WType_Popup | Qt::WStyle_NoBorder
		| Qt::WX11BypassWM | Qt::WDestructiveClose );
	setPalette( QToolTip::palette() );
	setFocusPolicy(Qt::NoFocus);
}

KexiToolTip::~KexiToolTip()
{
}

QSize KexiToolTip::sizeHint() const
{
	QSize sz(fontMetrics().boundingRect(m_value.toString()).size());
	return sz;
}

void KexiToolTip::show()
{
	updateGeometry();
	QWidget::show();
}

void KexiToolTip::paintEvent( QPaintEvent *pev )
{
	QWidget::paintEvent(pev);
	QPainter p(this);
	drawFrame(p);
	drawContents(p);
}

void KexiToolTip::drawFrame(QPainter& p)
{
	p.setPen( QPen(palette().active().foreground(), 1) );
	p.drawRect(rect());
}

void KexiToolTip::drawContents(QPainter& p)
{
	p.drawText(rect(), Qt::AlignCenter, m_value.toString());
}

#include "kexitooltip.moc"
