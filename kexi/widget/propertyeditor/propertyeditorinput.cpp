/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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
#include <qiconset.h>

#include <klineedit.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <kdebug.h>

#include "propertyeditorinput.h"
#include "kexiproperty.h"

PropertyEditorInput::PropertyEditorInput(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_lineedit = new KLineEdit(this);
	m_lineedit->resize(width(), height());

	m_lineedit->setText(property->value().toString());
	m_lineedit->show();

	setWidget(m_lineedit);

	connect(m_lineedit, SIGNAL(textChanged(const QString &)), this, SLOT(slotTextChanged(const QString &)));
}

void
PropertyEditorInput::slotTextChanged(const QString &text)
{
	emit changed(this);
}

QVariant
PropertyEditorInput::getValue()
{
	return QVariant(m_lineedit->text());
}

void
PropertyEditorInput::setValue(const QVariant &value)
{
	m_lineedit->setText(value.toString());
}

//INT

PropIntSpinBox::PropIntSpinBox(int lower, int upper, int step, int value, int base=10, QWidget *parent=0, const char *name=0) 
: KIntSpinBox(lower, upper, step, value, base, parent, name)
{
	editor()->setAlignment(Qt::AlignLeft);
}


PropertyEditorSpin::PropertyEditorSpin(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent,property, name)
{
	m_spinBox = new PropIntSpinBox(0,50000, 1, 0, 10, this);
	m_spinBox->resize(width(), height());
	m_spinBox->setValue(property->value().toInt());
	m_spinBox->show();

	setWidget(m_spinBox);
	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

QVariant
PropertyEditorSpin::getValue()
{
	return QVariant(m_spinBox->value());
}

void
PropertyEditorSpin::setValue(const QVariant &value)
{
	m_spinBox->setValue(value.toInt());
}

void
PropertyEditorSpin::valueChange(int)
{
	emit changed(this);
}

//DOUBLE

PropDoubleSpinBox::PropDoubleSpinBox(QWidget *parent=0) 
: KDoubleSpinBox(parent)
{
	editor()->setAlignment(Qt::AlignLeft);
}


PropertyEditorDblSpin::PropertyEditorDblSpin(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_spinBox = new PropDoubleSpinBox(this);
	m_spinBox->resize(width(), height());
	m_spinBox->setValue(property->value().toDouble());
	m_spinBox->show();

	setWidget(m_spinBox);
	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

QVariant
PropertyEditorDblSpin::getValue()
{
	return QVariant(m_spinBox->value());
}

void
PropertyEditorDblSpin::setValue(const QVariant &value)
{
	m_spinBox->setValue(value.toDouble());
}

void
PropertyEditorDblSpin::valueChange(int)
{
	emit changed(this);
}


/*********************
 * BOOL-EDITOR       *
 *********************/

PropertyEditorBool::PropertyEditorBool(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_toggle = new KPushButton(this);
	m_toggle->setToggleButton(true);
	m_toggle->resize(width(), height());
	
	connect(m_toggle, SIGNAL(toggled(bool)), this, SLOT(setState(bool)));
	if(property->value().toBool())
		m_toggle->setOn(true);
	else
	{
		m_toggle->toggle();
		m_toggle->setOn(false);
	}
	
	m_toggle->show();
	setWidget(m_toggle);
}

QVariant
PropertyEditorBool::getValue()
{
	return QVariant(m_toggle->isOn(), 3);
}

void
PropertyEditorBool::setValue(const QVariant &value)
{
	m_toggle->setOn(value.toBool());
}

void
PropertyEditorBool::setState(bool state)
{
	if(state)
	{
		m_toggle->setIconSet(QIconSet(SmallIcon("button_ok")));
		m_toggle->setText(i18n("True"));
	}
	else
	{
		m_toggle->setIconSet(QIconSet(SmallIcon("button_cancel")));
		m_toggle->setText(i18n("False"));
	}

	emit changed(this);
}

#include "propertyeditorinput.moc"
