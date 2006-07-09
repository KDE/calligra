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

#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>

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
		Command(const char* name, QPtrList<QPtrList<Element> >* groups);

		/**
		 * Create a command which match to  
		 * \\put(1,1){\\circle[}}.
		 */
		Command(const char* name, QPtrList<QPtrList<Param> >* options,
				QPtrList<QPtrList<Element> >* groups);

		/**
		 * Create a command which match to \\parpic(3cm,3cm)[f]{text} or
		 * \\documentclass[11pt]{guidepra}
		 */
		Command(const char* name, QPtrList<QPtrList<Param> >* params, QPtrList<Param>* options, QPtrList<QPtrList<Element> >* groups);

		~Command();

		QString getName() const { return _name; }

		QPtrList<QPtrList<Param> > getParams() const { return _params; }
		
		QPtrList<Param> getOptions() const { return _options; }
		
		QPtrList<QPtrList<Element> > getChildren() const { return _elements; }

		void setName(const char* name) { _name = name; }
		void addParam(const char* param);
		//void addParam(QString key, QString value) { _params.append(new Param(key, value)); }
		//void addParams(QPtrList<Param> params) { _params = params; }
		void addGroups(QPtrList<QPtrList<Element> >* elts) { _elements = *elts; }

		void addOption(const char* option);
		void addOption(QString key, QString value) { _options.append(new Param(key, value)); }
	
		void addChild(QPtrList<Element>* elt) { _elements.append(elt); }

		/* useful method */
		void print(int tab = 0);

	private:
		QString _name;
		QPtrList<QPtrList<Param> > _params;
		QPtrList<Param> _options;
		QPtrList<QPtrList<Element> > _elements;

};

#endif /* __LATEXPARSER_COMMAND_H__ */

