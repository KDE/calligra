/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2002-2003 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <kdebug.h>		/* for kDebug() stream */
#include "config.h"
//Added by qt3to4:
#include <QTextStream>

/* Static variable */
const char Config::SPACE_CHAR = ' ';
Config* Config::_instance = 0;

/*******************************************/
/* Constructor                             */
/*******************************************/
Config::Config()
{
	_tabSize = 4;
	_tabulation = 0;
	_useLatexStyle = true;
	_isEmbeded = false;
	_convertPictures = false;
}

Config::Config(const Config &config)
{
	setTabSize(config.getTabSize());
	setIndentation(config.getIndentation());
	setClass(config.getClass());
	setEmbeded(config.isEmbeded());

	setEncoding(config.getEncoding());
	if(config.isKwordStyleUsed())	useKwordStyle();
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Config::~Config()
{
}

void Config::indent()
{
	kDebug(30522) << "Indent tab = " << (_tabulation + getTabSize()) << endl;
	_tabulation = _tabulation + getTabSize();
}

void Config::unindent()
{
	if ((_tabulation - getTabSize()) > 0)
	{
		kDebug(30522) << "Unindent tab = " << (_tabulation - getTabSize()) << endl;
		_tabulation = _tabulation - getTabSize();
	}
	else
	{
		kDebug(30522) << "Unindent tab = 0" << endl;
		_tabulation = 0;
	}
}

void Config::writeIndent(QTextStream& out)
{
	for(int index = 0; index < _tabulation; index++)
	{
		out << " ";
	}
}

Config* Config::instance()
{
	if(_instance == 0)
		_instance = new Config();
	return _instance;
}
