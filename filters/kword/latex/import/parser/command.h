/* This file is part of the KDE project
 * Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __LATEXPARSER_COMMAND_H__
#define __LATEXPARSER_COMMAND_H__

#include "param.h"
//#include "option.h"
#include "element.h"

#include <QString>
#include <QMap>
#include <q3ptrlist.h>

class Command: public Element
{
	public:
		Command();
		/**
		 * Create a command which match to \\today
		 */
		Command(const char* command);

		/**
		 * Create a command which match to \\renewcommand{\\headrulewidth}{opt}
		 */
		Command(const char* name, Q3PtrList<Q3PtrList<Element> >* groups);

		/**
		 * Create a command which match to  
		 * \\put(1,1){\\circle[}}.
		 */
		Command(const char* name, Q3PtrList<Q3PtrList<Param> >* options,
				Q3PtrList<Q3PtrList<Element> >* groups);

		/**
		 * Create a command which match to \\parpic(3cm,3cm)[f]{text} or
		 * \\documentclass[11pt]{guidepra}
		 */
		Command(const char* name, Q3PtrList<Q3PtrList<Param> >* params, Q3PtrList<Param>* options, Q3PtrList<Q3PtrList<Element> >* groups);

		~Command();

		QString getName() const { return _name; }

		Q3PtrList<Q3PtrList<Param> > getParams() const { return _params; }
		
		Q3PtrList<Param> getOptions() const { return _options; }
		
		Q3PtrList<Q3PtrList<Element> > getChildren() const { return _elements; }

		void setName(const char* name) { _name = name; }
		void addParam(const char* param);
		//void addParam(QString key, QString value) { _params.append(new Param(key, value)); }
		//void addParams(QPtrList<Param> params) { _params = params; }
		void addGroups(Q3PtrList<Q3PtrList<Element> >* elts) { _elements = *elts; }

		void addOption(const char* option);
		void addOption(QString key, QString value) { _options.append(new Param(key, value)); }
	
		void addChild(Q3PtrList<Element>* elt) { _elements.append(elt); }

		/* useful method */
		void print(int tab = 0);

	private:
		QString _name;
		Q3PtrList<Q3PtrList<Param> > _params;
		Q3PtrList<Param> _options;
		Q3PtrList<Q3PtrList<Element> > _elements;

};

#endif /* __LATEXPARSER_COMMAND_H__ */

