/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2002 Robert JACOLIN
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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_CONFIG_H__
#define __KWORD_CONFIG_H__

#include <qtextstream.h>

/***********************************************************************/
/* Class: Config                                                       */
/***********************************************************************/

/**
 * This class hold all parameters and configuration from a file or from
 * the configuration dialog box.
 */
class Config
{
	static int _tabSize;	/* Size of the para indentation. */
	static int _tabulation;	/* Total size  of the indentation. */

	public:

		static const char SPACE_CHAR;
		/**
		 * Constructors
		 *
		 * Creates a new instance of Config.
		 * Initialise param. at default value.
		 */
		Config();

		/* 
		 * Destructor
		 */
		virtual ~Config();

		/**
		 * Accessors
		 */

		/**
		 * Return the value of a tabulation.
		 */
		int getTabSize() const { return _tabSize; }

		int getIndentation() { return _tabulation; }

		/**
		 * Modifiers
		 */

		/**
		 * Initialise teh tab size.
		 * @param size New size. Must be superior or eguals to 0.
		 */
		void setTabSize(int size)
		{
			if(size >= 0)
				_tabSize = size;
		}

		/**
		 * Helpfull functions
		 */
		void indent();
		void desindent();

		void writeIndent(QTextStream& out);

	private:

};

#endif /* __KWORD_CONFIG_H__ */
