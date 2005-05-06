/* This file is part of the KDE project
   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include <klineedit.h>
#include <kpushbutton.h>
#include <kfiledialog.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qvariant.h>
#include <qevent.h>
#include <qlabel.h>
#include <qcursor.h>

#include <klocale.h>
#include <kdebug.h>

#include "propertyeditorfile.h"
#include "kexiproperty.h"
#include "kexipropertybuffer.h"
#include "pixmapcollection.h"

PropertyEditorFile::PropertyEditorFile(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_lineedit = new KLineEdit(this);
	m_lineedit->resize(width(), height()-2);

	m_button = new KPushButton(i18n(" ... "), this);
	m_button->resize(height(), height()-10);
	m_button->move(width() - m_button->width() -1, 1);

	m_lineedit->setText(property->value().toString());
	m_lineedit->show();
	m_button->show();

	setWidget(m_lineedit);

	connect(m_button, SIGNAL(clicked()), this, SLOT(selectFile()));
}

QVariant
PropertyEditorFile::value()
{
	return QVariant(m_url.path());
}

void
PropertyEditorFile::setValue(const QVariant &value)
{
	m_url = value.toString();
	m_lineedit->setText(m_url.filename());
}


void
PropertyEditorFile::selectFile()
{
	m_url = KFileDialog::getOpenFileName(QString::null, m_filter, this, i18n("Choose File"));
	m_lineedit->setText(m_url.filename());
	emit changed(this);
}

void
PropertyEditorFile::setFilter(QString filter, bool add)
{
	if(add)
	m_filter += filter;
	else
	m_filter = filter;
}

void
PropertyEditorFile::resizeEvent(QResizeEvent *ev)
{
	m_lineedit->resize(ev->size());
	m_button->move(ev->size().width() - m_button->width()-1, 1);
}

bool
PropertyEditorFile::eventFilter(QObject* watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if((ev->key() == Key_Enter) || (ev->key()== Key_Space) || (ev->key() == Key_Return))
		{
			m_button->animateClick();
			return true;
		}
	}
	return KexiPropertySubEditor::eventFilter(watched, e);
}

//PIXMAP

PropertyEditorPixmap::PropertyEditorPixmap(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
 {
	m_label = new QLabel(this);
	m_label->setPixmap(property->value().toPixmap());
	m_label->setAlignment(Qt::AlignTop);
	m_label->resize(width(), height()-1);
	m_label->setBackgroundMode(Qt::PaletteBase);
	m_label->show();

	m_button = new KPushButton(i18n(" ... "), this);
	m_button->resize(height(), height()-8);
	m_button->move(width() - m_button->width() -1, 0);
	m_button->show();

	setWidget(m_label, m_button);

	m_popup = new QLabel(0, 0, Qt::WStyle_NoBorder|Qt::WX11BypassWM|WStyle_StaysOnTop);
	m_popup->hide();

	connect(m_button, SIGNAL(clicked()), this, SLOT(selectFile()));
 }

void
PropertyEditorPixmap::resizeEvent(QResizeEvent *ev)
{
	m_label->resize(ev->size().width(), ev->size().height()-1);
	m_button->resize(m_button->width(), ev->size().height());
	m_button->move(ev->size().width() - m_button->width(), 0);
}

bool
PropertyEditorPixmap::eventFilter(QObject *o, QEvent *ev)
{
	if(o == m_label)
	{
		if(ev->type() == QEvent::MouseButtonPress)
		{
			if(m_label->pixmap()->size().height() < height()-2
			     && m_label->pixmap()->size().width() < width()-20)
				return false;
			m_popup->setPixmap(*(m_label->pixmap()));
			m_popup->resize(m_label->pixmap()->size());
			m_popup->move(QCursor::pos());
			m_popup->show();
		}
		if(ev->type() == QEvent::MouseButtonRelease)
		{
			if(m_popup->isVisible())
				m_popup->hide();
		}
		if(ev->type() == QEvent::KeyPress)
		{
			QKeyEvent* e = static_cast<QKeyEvent*>(ev);
			if((e->key() == Key_Enter) || (e->key()== Key_Space) || (e->key() == Key_Return))
			{
				m_button->animateClick();
				return true;
			}
		}
	}
	return KexiPropertySubEditor::eventFilter(o, ev);
}

QVariant
PropertyEditorPixmap::value()
{
	return *(m_label->pixmap());
}

void
PropertyEditorPixmap::setValue(const QVariant &value)
{
	m_label->setPixmap(value.toPixmap());
	emit changed(this);
}

void
PropertyEditorPixmap::selectFile()
{
	if(m_property->buffer() && m_property->buffer()->collection())
	{
		QString name( m_property->option("pixmapName").toString() );
//		QString name = m_property->buffer()->pixmapName(m_property->name());
		PixmapCollectionChooser dialog(m_property->buffer()->collection(), name, topLevelWidget());
		if(dialog.exec() == QDialog::Accepted)
		{
			setValue(dialog.pixmap());
			m_property->buffer()->addCollectionPixmap(m_property, dialog.pixmapName());
		}
	}
	else
	{
		m_url = KFileDialog::getOpenFileName(QString::null, i18n("*.png *.xpm *.bmp *.jpg|Pixmap Files"),
			this, i18n("Choose File"));
		if(!m_url.isEmpty())
			m_label->setPixmap(QPixmap(m_url.path()));
		emit changed(this);
	}
}

PropertyEditorPixmap::~PropertyEditorPixmap()
{
	delete m_popup;
}

#include "propertyeditorfile.moc"

