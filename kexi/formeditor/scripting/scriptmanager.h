/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <qobject.h>
#include <qptrdict.h>

class FormScript;

namespace Kross {
	namespace Api  {
		class Manager;
	}
}

namespace KFormDesigner {
	class FormManager;
	class Form;
}

using namespace KFormDesigner;

class ScriptManager : public QObject
{
	Q_OBJECT

	public:
		ScriptManager(QObject *parent=0, const char *name=0);
		~ScriptManager();

		/*! \return A new FormScript object associated to the Form \a form.  */
		FormScript*  newFormScript(Form *form);
		/*! \return The FormScript object associated to this Form. */
		FormScript*  scriptForForm(Form *form);

		void  setFormManager(FormManager *manager);
		FormManager*  formManager() { return m_formManager; }
		Kross::Api::Manager*  krossManager() { return m_manager; }

	private slots:
		/*! Called when a form is deleted. It is removed from the dict. */
		void slotFormDeleted(KFormDesigner::Form *form);

	private:
		Kross::Api::Manager  *m_manager;
		KFormDesigner::FormManager *m_formManager;
		QPtrDict<FormScript>  m_dict;
};

#endif

