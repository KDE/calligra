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

#include "command.h"
#include <iostream.h>
#include "kdebug.h"

Command::Command()
{
	setType(Element::LATEX_COMMAND);
	_options.setAutoDelete(true);
	_params.setAutoDelete(true);
}

Command::Command(const char* name)
{
	setType(Element::LATEX_COMMAND);
	_options.setAutoDelete(true);
	_params.setAutoDelete(true);
	_name = name;
	_name = _name.stripWhiteSpace();
}

Command::Command(const char* name, QPtrList<QPtrList<Element> >* groups)
{
	setType(Element::LATEX_COMMAND);
	_options.setAutoDelete(true);
	_params.setAutoDelete(true);
	_name = name;
	if(groups != NULL)
		_elements = *groups;
	_name = _name.stripWhiteSpace();
}

Command::Command(const char* name, QPtrList<QPtrList<Param> >* params,
		QPtrList<QPtrList<Element> >* groups)
{
	setType(Element::LATEX_COMMAND);
	_options.setAutoDelete(true);
	_params.setAutoDelete(true);
	_name = name;
	if(groups != NULL)
		_elements = *groups;
	if(params != NULL)
		_params = *params;
	_name = _name.stripWhiteSpace();
}

Command::Command(const char* name, QPtrList<QPtrList<Param> >* params, QPtrList<Param>* options,
		QPtrList<QPtrList<Element> >* groups)
{
	setType(Element::LATEX_COMMAND);
	_options.setAutoDelete(true);
	_params.setAutoDelete(true);
	_name = name;
	if(groups != NULL)
		_elements = *groups;
	if(params != NULL)
		_params = *params;
	if(options != NULL)
		_options = *options;
	_name = name;
	_name = _name.stripWhiteSpace();
}

Command::~Command()
{
}

void Command::addParam(const char* )
{
	/*QString test = QString(name);
	QString key = test.left(test.find("="));
	QString value = test.right(test.find("="));
	addParam(key, value);*/
}

void Command::addOption(const char* )
{
	/*QString test = QString(name);
	QString key = test.left(test.find("="));
	QString value = test.right(test.find("="));
	addOption(key, value);*/
}

void Command::print(int tab)
{
	cout << _name.latin1();
	QPtrList<Param>* params;
	for ( params = _params.first(); params; params = _params.next() )
	{
		cout << "[";
		Param* param;
		for ( param = params->first(); param; param = params->next() )
		{
			param->print(tab);
			if(param != params->getLast())
				cout << ", ";
		}
		cout << "]";
	}
	if(_options.count() > 0)
	{
		cout << " - [";
		Param* param;
		for ( param = _options.first(); param; param = _options.next() )
		{
			param->print(tab);
			if(param != _options.getLast())
			cout << ", ";
		}
		cout << "]";
	}
	
	QPtrList<Element>* group;
	for(group = _elements.first(); group; group = _elements.next() )
	{
		cout << " {";
		Element* elt;
		for ( elt = group->first(); elt; elt = group->next() )
		{
			elt->print(tab);
		}
		cout << "}";
	}
	cout << endl;
}
