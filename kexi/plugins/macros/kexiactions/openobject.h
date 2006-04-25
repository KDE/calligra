/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KEXIMACRO_OPENOBJECT_H
#define KEXIMACRO_OPENOBJECT_H

//#include "../lib/komacro_export.h"
#include "../lib/action.h"
#include "../lib/variable.h"

namespace KexiMacro {

	/*
	class VariableT : public KoMacro::Variable
	{
		public:
			QStringList list;
	};
	
	class PartVariable : public VariableT
	{
		PartVariable(KoMacro::Action* action, QStringList list) {
			setCaption(i18n("Object"));
			setName("object");
		}
		virtual void update() {
			KexiPart::PartInfoList* parts = Kexi::partManager().partInfoList();
			//if(project && project->isConnected())
			QStringList list;
			for(KexiPart::PartInfoListIterator it(*parts); it.current(); ++it) {
				KexiPart::Info* info = it.current();
				list << info->objectName();
			}
			setVariant(list);
	
			for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
				Variable::Ptr v = action->variable(*it);
				v->update();
			}
		}
	};
	
	class ViewVariable : public VariableT
	{
		ViewVariable() {
		}
		virtual void update() {
			foreach(v : variable->references()) v->update();
		}
	};
	
	class NameVariable : public VariableT
	{
		NameVariable() {
		}
		virtual void update() {
			foreach(v : variable->references()) v->update();
		}
	};
	*/

	/**
	* The OpenObject class implements a @a KoMacro::Action
	* to provide functionality to open any kind of Kexi
	* object (e.g. table, query, form, script, ...).
	*/
	class OpenObject : public KoMacro::GenericAction<OpenObject>
	{
		public:

			/**
			* Constructor.
			*/
			explicit OpenObject();
			
			/**
			* Destructor.
			*/
			virtual ~OpenObject();

			/**
			*
			*/
			virtual KoMacro::Variable::List notifyUpdated(KoMacro::Variable::Ptr variable);

		/*
		public slots:
			virtual void activate() {}
		*/

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};
}

#endif
