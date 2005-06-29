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

#ifndef KPROPERTY_SYMBOLCOMBO_H
#define KPROPERTY_SYMBOLCOMBO_H

#include <koffice_export.h>
#include "widget.h"

class QLineEdit;
class QPushButton;

namespace KOProperty {

class SymbolCombo : public Widget
{
	Q_OBJECT

	public:
		SymbolCombo(Property *property, QWidget *parent=0, const char *name=0);
		~SymbolCombo();

		virtual QVariant value() const;
		virtual void setValue(const QVariant &value, bool emitChange=true);

		virtual void drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value);

	protected slots:
		void  selectChar();
		void  slotValueChanged(const QString &text);

	private:
		QLineEdit  *m_edit;
		QPushButton  *m_select;
};

}

#endif
