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

#ifndef PROPERTYEDITORFONT_H
#define PROPERTYEDITORFONT_H

#include "kexipropertysubeditor.h"

class QLabel;
class KPushButton;
class QResizeEvent;
class KexiProperty;
class KColorButton;

class KFORMEDITOR_EXPORT PropertyEditorFont : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorFont(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorFont() {;}

		virtual QVariant	getValue();
		
		void resizeEvent(QResizeEvent *ev);

	protected slots:
		void selectFont();

	protected:
		QLabel		*m_label;
		KPushButton	*m_button;
		QFont		m_font;
};


class KFORMEDITOR_EXPORT PropertyEditorColor : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorColor(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorColor() {;}

		virtual QVariant	getValue();
		
	protected slots:
		void valueChanged(const QColor &color);

	protected:
		KColorButton	*m_button;
		QColor		m_color;
};

#endif

