/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include "kexitableedit.h"

#include <qpalette.h>

KexiTableEdit::KexiTableEdit(QWidget* parent, const char* name)
	: QWidget(parent, name), m_view(0)
{
	setPaletteBackgroundColor( palette().color(QPalette::Active, QColorGroup::Base) );
//	installEventFilter(this);
}

void KexiTableEdit::setView(QWidget *v)
{
	m_view = v;
	m_view->installEventFilter(this);
}

void KexiTableEdit::resize(int w, int h)
{
	QWidget::resize(w, h);
	if (m_view)
		m_view->resize(w, h);
}

bool
KexiTableEdit::eventFilter(QObject* watched, QEvent* e)
{
/*	if (watched == m_view) {
		if(e->type() == QEvent::KeyPress) {
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);
			if (ev->key()==Key_Tab) {

			}
		}
	}
*/
	if(watched == this)
	{
		if(e->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);

			if(ev->key() == Key_Escape)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	
	return QWidget::eventFilter(watched, e);
}

bool KexiTableEdit::valueChanged()
{
	bool ok;
	return (m_origValue != value(ok)) && ok;
}
