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
#include "rectedit.h"
#include "editoritem.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>

namespace KoProperty {

RectEdit::RectEdit(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
{
	setHasBorders(false);
	QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
	m_edit = new QLabel(this);
	m_edit->setIndent(KPROPEDITOR_ITEM_MARGIN);
	m_edit->setBackgroundMode(Qt::PaletteBase);
	m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_edit->setMinimumHeight(5);
	l->addWidget(m_edit);
	setFocusWidget(m_edit);
}

RectEdit::~RectEdit()
{}

QVariant
RectEdit::value() const
{
	return m_value;
}

void
RectEdit::setValue(const QVariant &value, bool emitChange)
{
	m_value = value;
	m_edit->setText(QString("[ %1, %2, %3, %4 ]").arg(value.toRect().x()).
		arg(value.toRect().y()).arg(value.toRect().width()).arg(value.toRect().height()));

	if (emitChange)
		emit valueChanged(this);
}

void
RectEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
	Widget::drawViewer(p, cg, r, 
		QString("[ %1, %2, %3, %4 ]").arg(value.toRect().x()).arg(value.toRect().y())
	 	.arg(value.toRect().width()).arg(value.toRect().height()));
//	p->eraseRect(r);
//	p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine,
//	 	QString("[ %1, %2, %3, %4 ]").arg(value.toRect().x()).arg(value.toRect().y())
//	 	.arg(value.toRect().width()).arg(value.toRect().height()));
}

}

#include "rectedit.moc"

