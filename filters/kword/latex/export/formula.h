/* A TEXT IS A SET OF TITLES, A (SET OF) PARAGRAPHS OR LISTS */
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000 Robert JACOLIN
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

#ifndef __KWORD_LATEXFORMULA__
#define __KWORD_LATEXFORMULA__

#include "element.h"
#include <QString>
#include <QTextStream>

/***********************************************************************/
/* Class: Formula                                                      */
/***********************************************************************/

/**
 * This class hold a formula. The formula must be generated not for this
 * filter but from the kformula lib. So I keep only a string of the formula. 
 */
class Formula: public Element
{
	/* DATA MARKUP */
	int      _left,
		 _top,
		 _right,
		 _bottom;
	TAround  _runaround;
	double   _runaroundGap;
	TCreate  _autoCreate;
	TNFrame  _newFrameBehaviour;
	TSide   _sheetSide;

	/* CHILDREN MARKUPS */
	QString  _formula;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Formula.
		 */
		Formula();

		/* 
		 * Destructor
		 *
		 * Nothing to do.
		 */
		virtual ~Formula() {
			kDebug(30522) << "Destruction of a formula" << endl; }

		/**
		 * Accessors
		 */
		/*bool    hasColor      () const;
		bool    hasUline      () const;*/
		TAround getRunAround  () const { return _runaround;         }
		double  getAroundGap  () const { return _runaroundGap;      }
		TCreate getAutoCreate () const { return _autoCreate;        }
		TNFrame getNewFrame   () const { return _newFrameBehaviour; }
		TSide   getSheetSide  () const { return _sheetSide;         }

		void getFormula(QDomNode, int);

		/**
		 * Modifiers
		 */
		void setRunAround (const int a)    { _runaround = (TAround) a;  }
		void setAroundGap (const double r) { _runaroundGap = r;         }
		void setAutoCreate(const int a)    { _autoCreate = (TCreate) a; }
		void setNewFrame  (const int n)    { _newFrameBehaviour = (TNFrame) n; }
		void setSheetSide (const int s)    { _sheetSide = (TSide) s;    }

		/**
		 * Helpfull functions
		 */

		/**
		 * Get information from a markup tree and put the formula
		 * in a QString.
		 */
		void analyse(const QDomNode);

		/**
		 * Write the formula in a file.
		 */
		void generate(QTextStream&);

	private:
		void analyseParamFrame(const QDomNode);
};

#endif /* __KWORD_LATEXFORMULA_H__ */
