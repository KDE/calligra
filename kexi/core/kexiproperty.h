/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef KEXIPROPERTY_H
#define KEXIPROPERTY_H

#include <qvariant.h>

class QObject;
class QString;
class QStringList;

class KEXIPROPERTYEDITOR_EXPORT KexiProperty
{
	public:
		KexiProperty(const QString &name, QVariant value);
		KexiProperty(const QString &name, QVariant value, const QString &desc);
		KexiProperty(const QString &name, QVariant value, const QStringList &list);
		KexiProperty(const KexiProperty &property);
		//KexiProperty(const QString &name, QVariant value, QStringList *list);
		KexiProperty();
		~KexiProperty();
		
		const KexiProperty& operator=(const KexiProperty &property);

		QString		name() const { return m_name; }

		QVariant	value() const { return m_value; }
		/*! Sets this property value to a new value \a v. If this is a first change, 
		 and \a saveOldValue is true, an old value is saved, and can be later retrieved
		 using oldValue(). If \a saveOldValue if false, old value is cleared and the 
		 property looks loke it was not changed.
		*/
		void setValue(const QVariant &v, bool saveOldValue = true);

		/*! \return old preperty value. This makes only sense when changed() is true. 
		 The old value is saved on first change.
		*/
		QVariant	oldValue() const { return m_oldValue; }
		QString		desc() const { return m_desc; }
		QVariant::Type  type() const;
		QStringList*	list() const{ return m_list;}
		int		autoSync() const { return m_autosync; }

		void setAutoSync(int sync) { m_autosync = sync; }

		//! \return true is this preperty value is changed. 
		bool changed() const { return m_changed; }
		void setChanged(bool set);
	private:
		QString		m_name;
		QString		m_desc;
		QVariant	m_value;
		QVariant	m_oldValue;
		QStringList	*m_list;
		int		m_autosync;
		bool m_changed : 1;
};

#endif
