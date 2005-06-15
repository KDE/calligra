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

#include "pixmapedit.h"
#include "editoritem.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qcursor.h>
#include <qpushbutton.h>

#ifndef PURE_QT
#include <kfiledialog.h>
#include <klocale.h>
#else
#include <qfiledialog.h>
#endif

using namespace KOProperty;

PixmapEdit::PixmapEdit(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
{
	setHasBorders(false);
	QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
	m_edit = new QLabel(this);
	m_edit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//	m_edit->resize(width(), height()-1);
	m_edit->setBackgroundMode(Qt::PaletteBase);
	setBackgroundMode(Qt::PaletteBase);

	m_button = new QPushButton(i18n("..."), this);
	m_button->resize(height(), height()-8);
	m_button->move(width() - m_button->width() -1, 0);
	m_button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	m_button->setFocusPolicy(NoFocus);
	l->addSpacing(2);
	l->addWidget(m_edit);
	l->addWidget(m_button);
	m_popup = new QLabel(0, 0, Qt::WStyle_NoBorder|Qt::WX11BypassWM|WStyle_StaysOnTop);
	m_popup->hide();

	setFocusWidget(m_edit);
	connect(m_button, SIGNAL(clicked()), this, SLOT(selectPixmap()));
}

PixmapEdit::~PixmapEdit()
{
	delete m_popup;
}

QVariant
PixmapEdit::value() const
{
	return *(m_edit->pixmap());
}

void
PixmapEdit::setValue(const QVariant &value, bool emitChange)
{
	m_edit->setPixmap(value.toPixmap());
	if (emitChange)
		emit valueChanged(this);
}

void
PixmapEdit::drawViewer(QPainter *p, const QColorGroup &, const QRect &r, const QVariant &value)
{
	p->eraseRect(r);
	QPixmap pix( value.toPixmap() );
	p->drawPixmap(r.topLeft().x()+KPROPEDITOR_ITEM_MARGIN, 
		r.topLeft().y()+(r.height()-pix.height())/2, pix);
}

void
PixmapEdit::selectPixmap()
{
#ifndef PURE_QT
	KURL url = KFileDialog::getImageOpenURL(QString::null, this);
	if (!url.isEmpty()){
		m_edit->setPixmap(QPixmap(url.path()));
		emit valueChanged(this);
	}
#else
	QString url = QFileDialog::getOpenFileName();
	if (!url.isEmpty()) {
		m_edit->setPixmap(QPixmap(url));
		emit valueChanged(this);
	}
#endif
}

void
PixmapEdit::resizeEvent(QResizeEvent *ev)
{
//	m_edit->resize(ev->size().width(), ev->size().height()-1);
//	m_button->move(ev->size().width() - m_button->width(), 0);
	m_edit->setMaximumHeight(m_button->height());
}

bool
PixmapEdit::eventFilter(QObject *o, QEvent *ev)
{
	if(o == m_edit) {
		if(ev->type() == QEvent::MouseButtonPress)  {
			if(m_edit->pixmap()->size().height() < height()-2
				&& m_edit->pixmap()->size().width() < width()-20)
				return false;

			m_popup->setPixmap(*(m_edit->pixmap()));
			m_popup->resize(m_edit->pixmap()->size());
			m_popup->move(QCursor::pos());
			m_popup->show();
		}
		else if(ev->type() == QEvent::MouseButtonRelease) {
			if(m_popup->isVisible())
				m_popup->hide();
		}
		else if(ev->type() == QEvent::KeyPress) {
			QKeyEvent* e = static_cast<QKeyEvent*>(ev);
			if((e->key() == Key_Enter) || (e->key()== Key_Space) || (e->key() == Key_Return)) {
				m_button->animateClick();
				return true;
			}
		}
	}

	return Widget::eventFilter(o, ev);
}

#include "pixmapedit.moc"

