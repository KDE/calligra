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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_LATEXTEXT_H__
#define __KWORD_LATEXTEXT_H__

#include <qlist.h>
#include "element.h"
#include "border.h"
//#include "listepara.h"
#include "para.h"

/***********************************************************************/
/* Class: Texte                                                        */
/***********************************************************************/

/**
 * This class hold 2 lists of paragraphs for the text and for the footnotes.
 * NOTE : The frame analyse can be (must be) in a Element class.
 */
class Texte: public Element, Border
{
	/* DATA MARKUP */
	int      _left,
		 _top,
		 _right,
		 _bottom;
	TAround  _runaround;
	double   _runaroundGap;
	TCreate _autoCreate;
	TNFrame _newFrameBehaviour;
	TSide   _sheetSide;

	/* CHILD MARKUP */
	QList<Para> _parags;
	QList<Para> _footnotes;

	/* USEFULL DATA */
	EEnv      _lastEnv;
	EType     _lastTypeEnum;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instances of Texte.
		 *
		 */
		Texte();

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of parag and footnotes.
		 */
		virtual ~Texte() {
			//delete _footnotes;
			kdDebug() << "Destruction of a txt" << endl; }
		/**
		 * Accessors
		 */
		/*bool    hasColor      () const;
		bool    hasUline      () const;*/
		int     getLeft       () const { return _left;              }
		int     getRight      () const { return _right;             }
		int     getTop        () const { return _top;               }
		int     getBottom     () const { return _bottom;            }
		TAround getRunAround  () const { return _runaround;         }
		double  getAroundGap  () const { return _runaroundGap;      }
		TCreate getAutoCreate () const { return _autoCreate;        }
		TNFrame getNewFrame   () const { return _newFrameBehaviour; }
		TSide   getSheetSide  () const { return _sheetSide;         }
		Para*   getFirstPara  () const { return _parags.getFirst(); }
		EEnv    getNextEnv    (QList<Para>, const int);
		bool    isBeginEnum   (Para*, Para*);
		bool    isCloseEnum   (Para*, Para*);

		/**
		 * Modifiors
		 */
		void setLeft      (const int l)    { _left   = l;               }
		void setRight     (const int r)    { _right  = r;               }
		void setTop       (const int t)    { _top    = t;               }
		void setBottom    (const int b)    { _bottom = b;               }

		void setRunAround (const int a)    { _runaround = (TAround) a;  }
		void setAroundGap (const double r) { _runaroundGap = r;         }
		void setAutoCreate(const int a)    { _autoCreate = (TCreate) a; }
		void setNewFrame  (const int n)    { _newFrameBehaviour = (TNFrame) n; }
		void setSheetSide (const int s)    { _sheetSide = (TSide) s;    }

		Para* searchFootnote(const QString);

		/**
		 * Get informations from a markup tree.
		 */
		void analyse(const QDomNode);

		/**
		 * Write the text in a file.
		 */
		void generate(QTextStream&);

	private:
		/**
		 * Get informations from a markup tree (only parameters
		 * in a frame.
		 */
		void analyseParamFrame(const QDomNode);

};

#endif /* __KWORD_LATEXTEXT_H__ */

