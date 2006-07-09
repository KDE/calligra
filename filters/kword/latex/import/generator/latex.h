/* Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>
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

#ifndef __LATEX_IMPORT_LATEXUTILS__
#define __LATEX_IMPORT_LATEXUTILS__

#include <qptrlist.h>

class Element;

class Latex
{

	public:
		static Latex* instance()
		{
			if(_instance == 0)
				_instance = new Latex();
			return _instance;
		}

		Element* getEnv(QPtrList<Element>* root, const char* name);
		Element* getCommand(QPtrList<Element>* root, const char* name);

	private:
		static Latex* _instance;
		Latex()
		{
		}
};

#endif /* __LATEX_IMPORT_LATEXUTILS__ */
