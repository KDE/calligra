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

#include "kexismalltoolbutton.h"

#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qstyle.h>

#include <kiconloader.h>
#include <kglobalsettings.h>

#include <core/kexi.h>

KexiSmallToolButton::KexiSmallToolButton(QWidget* parent, const QString& text,
	const QString& icon, const char* name)
 : QToolButton(parent, name)
{
	init();
	update(text, KIcon(icon));
}

KexiSmallToolButton::KexiSmallToolButton(QWidget* parent, const QString& text,
	const QIcon& iconSet, const char* name)
 : QToolButton(parent, name)
{
	init();
	update(text, iconSet);
}

KexiSmallToolButton::KexiSmallToolButton(QWidget* parent, KAction* action)
 : QToolButton(parent, action->name())
 , m_action(action)
{
	init();
	connect(this, SIGNAL(clicked()), action, SLOT(activate()));
	connect(action, SIGNAL(enabled(bool)), this, SLOT(setEnabled(bool)));
	updateAction();
}

KexiSmallToolButton::~KexiSmallToolButton()
{
}

void KexiSmallToolButton::updateAction()
{
	if (!m_action)
		return;
	update(m_action->text(), m_action->iconSet(K3Icon::Small));
	setAccel(m_action->shortcut());
	this->setToolTip( m_action->toolTip());
	Q3WhatsThis::add(this, m_action->whatsThis());
}

void KexiSmallToolButton::init()
{
	setPaletteBackgroundColor(palette().active().background());
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	QFont f(KGlobalSettings::toolBarFont());
	f.setPixelSize(Kexi::smallFont().pixelSize());
	setFont(f);
	setAutoRaise(true);
}

void KexiSmallToolButton::update(const QString& text, const QIcon& iconSet, bool tipToo)
{
	int width = 0;
	if (text.isEmpty()) {
		width = 10;
		setUsesTextLabel(false);
	}
	else {
		width += QFontMetrics(font()).width(text+" ");
		setUsesTextLabel(true);
		setTextPosition(QToolButton::BesideIcon);
		QToolButton::setTextLabel(text, tipToo);
	}
	if (!iconSet.isNull()) {
		width += IconSize(K3Icon::Small);
		QToolButton::setIconSet(iconSet);
	}
	setFixedWidth( width );
}

void KexiSmallToolButton::setIconSet( const QIcon& iconSet )
{
	update(textLabel(), iconSet);
}

void KexiSmallToolButton::setIconSet( const QString& icon )
{
	setIconSet( KIcon(icon) );
}

void KexiSmallToolButton::setTextLabel( const QString & newLabel, bool tipToo )
{
	Q_UNUSED( tipToo );

	update(newLabel, iconSet());
}

void KexiSmallToolButton::drawButton( QPainter *_painter )
{
	QToolButton::drawButton(_painter);
	if (QToolButton::popup()) {
		QStyle::State arrowFlags = QStyle::State_None;
		if (isDown())
			arrowFlags |= QStyle::State_DownArrow;
		if (isEnabled())
			arrowFlags |= QStyle::State_Enabled;
		style().drawPrimitive(QStyle::PE_ArrowDown, _painter,
			QRect(width()-7, height()-7, 5, 5), colorGroup(),
			arrowFlags, QStyleOption() );
	}
}

#include "kexismalltoolbutton.moc"
