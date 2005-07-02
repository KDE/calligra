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
#include "combobox.h"

#include <qlayout.h>
#include <qmap.h>
#include <qvariant.h>
#include <qpainter.h>

#ifdef QT_ONLY
#iinclude <qcombobox.h>
#else
#include <kcombobox.h>
#include <kdebug.h>
#endif

#include "property.h"

namespace KoProperty {

ComboBox::ComboBox(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
{
	QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
#ifdef QT_ONLY
	m_edit = new QComboBox(this);
#else
	m_edit = new KComboBox(this);
#endif
	m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_edit->setMinimumHeight(5);
	l->addWidget(m_edit);

	m_edit->setEditable(false);
	m_edit->setInsertionPolicy(QComboBox::NoInsertion);
	m_edit->setMinimumSize(10, 0); // to allow the combo to be resized to a small size
	m_edit->setAutoCompletion(true);
#ifndef QT_ONLY
	m_edit->setContextMenuEnabled(false);
#endif

	fillBox();
//not needed for combo	setLeavesTheSpaceForRevertButton(true);

	setFocusWidget(m_edit);
	connect(m_edit, SIGNAL(activated(int)), this, SLOT(slotValueChanged(int)));
}

ComboBox::~ComboBox()
{}

QVariant
ComboBox::value() const
{
	if(property()->valueList() && property()->valueList()->contains(m_edit->currentText()))
		return (*(property()->valueList()))[m_edit->currentText()];
	return QVariant();
}

void
ComboBox::setValue(const QVariant &value, bool emitChange)
{
	if(value.isNull())
		return;

	m_edit->blockSignals(true);
	m_edit->setCurrentText(keyForValue(value));
	m_edit->blockSignals(false);
	if (emitChange)
		emit valueChanged(this);
}

void
ComboBox::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
	Widget::drawViewer(p, cg, r, keyForValue(value));
//	p->eraseRect(r);
//	p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, keyForValue(value));
}

void
ComboBox::fillBox()
{
	m_edit->clear();
	//m_edit->clearContents();

	if(!property() || !property()->valueList())
		return;

	QStringList keys = property()->valueList()->keys();
	m_edit->insertStringList(keys);
#ifndef QT_ONLY
	KCompletion *comp = m_edit->completionObject();
	comp->insertItems(keys);
	comp->setCompletionMode(KGlobalSettings::CompletionShell);
#endif
}

void
ComboBox::setProperty(Property *prop)
{
	bool b = (property() == prop);
	Widget::setProperty(prop);
	if(!b)
		fillBox();
	if(prop)
		setValue(prop->value(), false);
}

void
ComboBox::slotValueChanged(int)
{
	emit valueChanged(this);
}


QString
ComboBox::keyForValue(const QVariant &value)
{
	const QMap<QString, QVariant> *list = property()->valueList();
	if (!list)
		return QString::null;
	QMap<QString, QVariant>::ConstIterator endIt = list->constEnd();
	for(QMap<QString, QVariant>::ConstIterator it = list->constBegin(); it != endIt; ++it) {
		if(it.data() == value)
			return it.key();
	}
	return QString::null;
}

}

#include "combobox.moc"

