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

#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>

#include <klocale.h>
#include <kfontdialog.h>
#include <kpushbutton.h>
#include <kcolorbutton.h>
#include <kdebug.h>

#include "propertyeditorfont.h"
#include "kexiproperty.h"

//FONT CHOOSER

PropertyEditorFont::PropertyEditorFont(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
//	m_font = property->value().toFont();
	
	m_label = new QLabel(this);
	m_label->setMargin( 2 );
	m_label->resize(width(), height()-1);

	m_button = new KPushButton(" ... ", this);
	m_button->resize(height(), height()-8);
	m_button->move(width() - m_button->width(), 0);
	
//	m_label->setText( KexiProperty::format( m_font ) );
	setValue(property->value());
	m_label->setBackgroundMode(Qt::PaletteBase);
	m_label->show();
	m_button->show();
	
	setWidget(m_label);
	
	connect(m_button, SIGNAL(clicked()), this, SLOT(selectFont()));
}

QVariant
PropertyEditorFont::value()
{
	return QVariant(m_font);
}

void
PropertyEditorFont::setValue(const QVariant &value)
{
	m_font = value.toFont();
	m_label->setText( KexiProperty::format( m_font ) );
	emit changed(this);
}

void
PropertyEditorFont::selectFont()
{
	int result = KFontDialog::getFont(m_font, false, this);
	
	if(result == KFontDialog::Accepted)
	{
		setValue(m_font);
//		m_label->setText(m_font.family() + " " + QString::number(m_font.pointSize()));
		emit changed(this);
	}
}

void
PropertyEditorFont::resizeEvent(QResizeEvent *ev)
{
	m_label->resize(ev->size().width(), ev->size().height()-1);
	m_button->resize(m_button->width(), ev->size().height());
	m_button->move(ev->size().width() - m_button->width(), 0);
}

bool
PropertyEditorFont::eventFilter(QObject* watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if((ev->key() == Key_Enter) | (ev->key()== Key_Space) || (ev->key() == Key_Return))
		{
			m_button->animateClick();
			return true;
		}
	}
	return KexiPropertySubEditor::eventFilter(watched, e);
}

//COLOR CHOOSER

PropertyEditorColor::PropertyEditorColor(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
 {
	m_button = new KColorButton(property->value().asColor(), this);
	m_button->resize(width(), height());
	m_button->show();
	setWidget(m_button);
	
	connect(m_button, SIGNAL(changed(const QColor&)), this, SLOT(valueChanged(const QColor&)));
 }

 
 QVariant
PropertyEditorColor::value()
{
	return m_button->color();
}

void
PropertyEditorColor::setValue(const QVariant &value)
{
	m_button->setColor(value.toColor());
}

void
PropertyEditorColor::valueChanged(const QColor &color)
{
	emit changed(this);
}

bool
PropertyEditorColor::eventFilter(QObject* watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if((ev->key() == Key_Enter) | (ev->key()== Key_Space) || (ev->key() == Key_Return))
		{
			m_button->animateClick();
			return true;
		}
	}
	return KexiPropertySubEditor::eventFilter(watched, e);
}

#include "propertyeditorfont.moc"

