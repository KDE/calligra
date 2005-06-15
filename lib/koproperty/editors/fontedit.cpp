/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "fontedit.h"
#include "editoritem.h"

#include <qpushbutton.h>
#include<qpainter.h>
#include <kfontrequester.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qfont.h>

#ifdef QT_ONLY
// \todo
#else
#include <klocale.h>
#endif

namespace KOProperty {

FontEdit::FontEdit(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
{
	QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
	m_edit = new KFontRequester(this);
	m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_edit->setMinimumHeight(5);
#ifndef QT_ONLY
	m_edit->button()->setText(i18n("..."));
#endif
	l->addWidget(m_edit);

	setFocusWidget(m_edit);
	connect(m_edit, SIGNAL(fontSelected(const QFont& )), this, SLOT(slotValueChanged(const QFont&)));
}

FontEdit::~FontEdit()
{}

QVariant
FontEdit::value() const
{
	return m_edit->font();
}

void
FontEdit::setValue(const QVariant &value, bool emitChange)
{
	m_edit->blockSignals(true);
	m_edit->setFont(value.toFont());
	m_edit->blockSignals(false);
	if (emitChange)
		emit valueChanged(this);
}

void
FontEdit::drawViewer(QPainter *p, const QColorGroup &, const QRect &r, const QVariant &value)
{
	p->eraseRect(r);
	p->setFont(value.toFont());
	QFontInfo fi(value.toFont());
	QRect r2(r);
	r2.setLeft(r2.left()+KPROPEDITOR_ITEM_MARGIN);
	p->drawText(r2, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine,
		fi.family() + (fi.bold() ? i18n(" Bold") : QString("")) +
		(fi.italic() ? i18n(" Italic") : QString("")) +
		" " + QString("%1").arg(fi.pointSize()) );
}

void
FontEdit::slotValueChanged(const QFont &)
{
	emit valueChanged(this);
}

bool
FontEdit::eventFilter(QObject* watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress) {
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if(ev->key() == Key_Space) {
			m_edit->button()->animateClick();
			return true;
		}
	}
	return Widget::eventFilter(watched, e);
}

}

#include "fontedit.moc"

