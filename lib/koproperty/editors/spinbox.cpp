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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "spinbox.h"

#include "property.h"

#include <qlayout.h>
#include <qvariant.h>
#include <qpainter.h>
#include <limits>

#ifdef QT_ONLY
//! \todo
#else
#include <qlineedit.h>
#endif

using namespace KoProperty;

IntSpinBox::IntSpinBox(int lower, int upper, int step, int value, int base, QWidget *parent, const char *name)
: KIntSpinBox(lower, upper, step, value, base, parent, name)
{
	editor()->setAlignment(Qt::AlignLeft);
}

bool
IntSpinBox::eventFilter(QObject *o, QEvent *e)
{
	if(o == editor())
	{
		if(e->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);
			if((ev->key()==Key_Up || ev->key()==Key_Down) && ev->state() !=ControlButton)
			{
				parentWidget()->eventFilter(o, e);
				return true;
			}
		}
	}

	return KIntSpinBox::eventFilter(o, e);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IntEdit::IntEdit(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
{
	QVariant minVal( property ? property->option("min") : 0 );
	QVariant maxVal( property ? property->option("max") : QVariant() );
	QVariant minValueText( property ? property->option("minValueText") : QVariant() );
	if (minVal.isNull())
		minVal = 0;
	if (maxVal.isNull())
		maxVal = INT_MAX;

	m_edit = new IntSpinBox(minVal.toInt(), maxVal.toInt(), 1, 0, 10, this);
	if (!minValueText.isNull())
		m_edit->setSpecialValueText(minValueText.toString());
	m_edit->setMinimumHeight(5);
	setEditor(m_edit);

	setLeavesTheSpaceForRevertButton(true);
	setFocusWidget(m_edit);
	connect(m_edit, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));
}

IntEdit::~IntEdit()
{}

QVariant
IntEdit::value() const
{
	//return m_edit->cleanText().toInt();  adymo: why cleanText()
	return m_edit->value();
}

void
IntEdit::setValue(const QVariant &value, bool emitChange)
{
	m_edit->blockSignals(true);
	m_edit->setValue(value.toInt());
	m_edit->blockSignals(false);
	if (emitChange)
		emit valueChanged(this);
}

void
IntEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
	QString valueText = value.toString();
	if (property() && property()->hasOptions()) {
		//replace min value with minValueText if defined
		QVariant minValue( property()->option("min") );
		QVariant minValueText( property()->option("minValueText") );
		if (!minValue.isNull() && !minValueText.isNull() && minValue.toInt() == value.toInt()) {
			valueText = minValueText.toString();
		}
	}

	Widget::drawViewer(p, cg, r, valueText);
//	p->eraseRect(r);
//	p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, valueText);
}

void
IntEdit::slotValueChanged(int)
{
	emit valueChanged(this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleSpinBox::DoubleSpinBox (double lower, double upper, double step, double value, int precision, QWidget *parent)
: KDoubleSpinBox(lower, upper, step, value, precision, parent)
{
	editor()->setAlignment(Qt::AlignLeft);
}

bool
DoubleSpinBox::eventFilter(QObject *o, QEvent *e)
{
	if(o == editor())
	{
		if(e->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);
			if((ev->key()==Key_Up || ev->key()==Key_Down) && ev->state()!=ControlButton)
			{
				parentWidget()->eventFilter(o, e);
				return true;
			}
		}
	}

	return KDoubleSpinBox::eventFilter(o, e);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleEdit::DoubleEdit(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
{
	QVariant minVal( property ? property->option("min") : 0 );
	QVariant maxVal( property ? property->option("max") : QVariant() );
	QVariant step( property ? property->option("step") : QVariant());
	QVariant precision( property ? property->option("precision") : QVariant());
	QVariant minValueText( property ? property->option("minValueText") : QVariant() );
	if (minVal.isNull())
		minVal = 0;
	if (maxVal.isNull())
		maxVal = (float)INT_MAX;
	if(step.isNull())
		step = 0.1;
	if(precision.isNull())
		precision = 2;

	m_edit = new DoubleSpinBox(minVal.toDouble(), maxVal.toDouble(), step.toDouble(),
		 0, precision.toInt(), this);
	if (!minValueText.isNull())
		m_edit->setSpecialValueText(minValueText.toString());
	m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_edit->setMinimumHeight(5);
	setEditor(m_edit);

	setLeavesTheSpaceForRevertButton(true);
	setFocusWidget(m_edit);
	connect(m_edit, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));
}

DoubleEdit::~DoubleEdit()
{}

QVariant
DoubleEdit::value() const
{
	//return m_edit->cleanText().toInt();  adymo: why cleanText()
	return m_edit->value();
}

void
DoubleEdit::setValue(const QVariant &value, bool emitChange)
{
	m_edit->blockSignals(true);
	m_edit->setValue(value.toInt());
	m_edit->blockSignals(false);
	if (emitChange)
		emit valueChanged(this);
}

void
DoubleEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
	QString valueText = value.toString();
	if (property() && property()->hasOptions()) {
		//replace min value with minValueText if defined
		QVariant minValue( property()->option("min") );
		QVariant minValueText( property()->option("minValueText") );
		if (!minValue.isNull() && !minValueText.isNull() && minValue.toInt() == value.toInt()) {
			valueText = minValueText.toString();
		}
	}

	Widget::drawViewer(p, cg, r, valueText);
//	p->eraseRect(r);
//	p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, valueText);
}

void
DoubleEdit::slotValueChanged(int)
{
	emit valueChanged(this);
}

#include "spinbox.moc"
