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

#include <klocale.h>
#include <qdatetimeedit.h>

#include "propertyeditordate.h"
#include "kexiproperty.h"

//DATE CHOOSER

PropertyEditorDate::PropertyEditorDate(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_leaveTheSpaceForRevertButton = true;
	m_dateedit = new QDateEdit(property->value().asDate(), this);
	m_dateedit->resize(width(), height());
	m_dateedit->show();
	setWidget(m_dateedit);
	
	connect(m_dateedit, SIGNAL(valueChanged(const QDate&)), this, SLOT(valueChanged(const QDate&)));
}

QVariant
PropertyEditorDate::value()
{
	return QVariant(m_dateedit->date());
}

void
PropertyEditorDate::setValue(const QVariant &value)
{
	m_dateedit->setDate(value.toDate());
}

void
PropertyEditorDate::valueChanged(const QDate & /*date*/)
{
	emit changed(this);
}

//TIME CHOOSER

PropertyEditorTime::PropertyEditorTime(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_leaveTheSpaceForRevertButton = true;
	m_timeedit = new QTimeEdit(property->value().asTime(), this);
	m_timeedit->resize(width(), height());
	m_timeedit->show();
	setWidget(m_timeedit);
	
	connect(m_timeedit, SIGNAL(valueChanged(const QTime&)), this, SLOT(valueChanged(const QTime&)));
}

QVariant
PropertyEditorTime::value()
{
	return QVariant(m_timeedit->time());
}

void
PropertyEditorTime::setValue(const QVariant &value)
{
	m_timeedit->setTime(value.toTime());
}

void
PropertyEditorTime::valueChanged(const QTime & /*time*/)
{
	emit changed(this);
}


// DATE/TIME CHOOSER

PropertyEditorDateTime::PropertyEditorDateTime(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_leaveTheSpaceForRevertButton = true;
	m_datetime = new QDateTimeEdit(property->value().asDateTime(), this);
	m_datetime->resize(width(), height());
	m_datetime->show();
	setWidget(m_datetime, m_datetime->dateEdit());
	
	connect(m_datetime, SIGNAL(valueChanged(const QDateTime&)), this, SLOT(valueChanged(const QDateTime&)));
}

QVariant
PropertyEditorDateTime::value()
{
	return QVariant(m_datetime->dateTime());
}

void
PropertyEditorDateTime::setValue(const QVariant &value)
{
	m_datetime->setDateTime(value.toDateTime());
}

void
PropertyEditorDateTime::valueChanged(const QDateTime & /*dateTime*/)
{
	emit changed(this);
}


#include "propertyeditordate.moc"


