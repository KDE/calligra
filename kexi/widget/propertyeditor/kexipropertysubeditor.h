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

#ifndef KEXIPROPERTYSUBEDITOR_H
#define KEXIPROPERTYSUBEDITOR_H

#include <qwidget.h>

class KexiProperty;

//! The base class for all editors used in KexiPropertyEditor.
 
class KEXIPROPERTYEDITOR_EXPORT KexiPropertySubEditor : public QWidget
{
	Q_OBJECT

	public:
		KexiPropertySubEditor(QWidget *parent, KexiProperty *property, const char *name=0);
		~KexiPropertySubEditor();

		virtual bool		eventFilter(QObject* watched, QEvent* e);
		virtual QVariant	getValue();
		virtual void		setValue(const QVariant &value);

		//! Sets \a w as editor 's widget, ie the widget which events are filtered and which is resized.
		void			setWidget(QWidget *w);

	signals:
		//! Validate the input in the editor.
		void			accept(KexiPropertySubEditor *);
		//! Do not validate the contents of the editor.
		void			reject(KexiPropertySubEditor *);
		//! The editor's value has changed.
		void			changed(KexiPropertySubEditor *);

	protected:
		virtual void		resizeEvent(QResizeEvent *ev);

	private:
		QWidget			*m_childWidget;
};

#endif
