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

#include <klocale.h>
#include <kdebug.h>

#include "propertyeditorfile.h"
#include "kexiproperty.h"

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
PropertyEditorFile::getValue()
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
	m_url = KFileDialog::getOpenFileName(QString::null, m_filter, this, i18n("Choose a file"));
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



//PIXMAP

PropertyEditorPixmap::PropertyEditorPixmap(QWidget *parent, KexiProperty *property, const char *name)
 : PropertyEditorFile(parent, property, name)
 {
	setFilter(i18n("*.png *.xpm *.bmp *.jpg|Pixmap Files"), false);
	m_property = property;
	m_pixmap = QPixmap();
 }
 
 
QVariant
PropertyEditorPixmap::getValue()
{
	if(!m_url.isEmpty() )
	{
		QVariant v = QPixmap(m_url.path());
		return v;
	}
	else
	{
		if(!(m_pixmap.isNull()))
		{
			QVariant p = QVariant(m_pixmap);
			m_pixmap = QPixmap();
			return p;
		}
		QVariant p = m_property->value();
		return p;
	}
}

void
PropertyEditorPixmap::setValue(const QVariant &value)
{
	m_url = KURL();
	kdDebug() << value.toPixmap().width() << endl;
	m_pixmap = value.toPixmap();
	m_lineedit->setText("");
	emit changed(this);
}


#include "propertyeditorfile.moc"

