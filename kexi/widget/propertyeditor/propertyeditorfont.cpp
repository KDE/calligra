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

#include "propertyeditorfont.h"
#include "kexiproperty.h"

//FONT CHOOSER

PropertyEditorFont::PropertyEditorFont(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_font = property->value().toFont();
	
	m_label = new QLabel(this);
	m_label->resize(width(), height());

	m_button = new KPushButton(i18n(" ... "), this);
	m_button->resize(height(), height()-15);
	m_button->move(width() - m_button->width(), 2);
	
	m_label->setText(m_font.family() + " " + QString::number(m_font.pointSize()) );
	m_label->setBackgroundMode(Qt::PaletteBase);
	m_label->show();
	m_button->show();
	
	setWidget(m_label);
	m_label->installEventFilter(this);
	
	connect(m_button, SIGNAL(clicked()), this, SLOT(selectFont()));
}

QVariant
PropertyEditorFont::getValue()
{
	return QVariant(m_font);
}

void
PropertyEditorFont::setValue(const QVariant &value)
{
	m_font = value.toFont();
	m_label->setText(m_font.family() + " " + QString::number(m_font.pointSize()));
	emit changed(this);
}

void
PropertyEditorFont::selectFont()
{
	int result = KFontDialog::getFont(m_font, false, this);
	
	if(result == KFontDialog::Accepted)
	{
		m_label->setText(m_font.family() + " " + QString::number(m_font.pointSize()));
		emit changed(this);
	}
}

void
PropertyEditorFont::resizeEvent(QResizeEvent *ev)
{
	m_label->resize(ev->size());
	m_button->move(ev->size().width() - m_button->width(), 2);
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
PropertyEditorColor::getValue()
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

#include "propertyeditorfont.moc"

